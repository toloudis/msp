/*****************************************************************************
**  fxEffectVk.cpp
**
**      Vulkan backend for plain-HLSL effects. See fxEffectVk.hpp. The
**      parameter bookkeeping is GraphicsDX11's fxEffectDX11.cpp, unchanged
**      except for types.
\****************************************************************************/
#include "GraphicsVulkan/Fx/fxEffectVk.hpp"

#include <algorithm>
#include <cstring>

namespace gvk
{
	namespace
	{
		const VkShaderStageFlagBits k_StageBits[fx_NumStages] =
		{
			VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
			VK_SHADER_STAGE_GEOMETRY_BIT, VK_SHADER_STAGE_FRAGMENT_BIT, VK_SHADER_STAGE_COMPUTE_BIT
		};

		//	A constant buffer can't hold a SPIR-V bool, so DXC stores a bool
		//	as a uint; the manifest or the HLSL has the declared type.
		void fix_bool(vkdShaderType& io_Type, const std::string& i_Name, const fxVariableDesc* i_pDesc,
					  const std::vector<std::string>& i_BoolNames)
		{
			const bool declaredBool = (i_pDesc && i_pDesc->m_Type.compare(0, 4, "bool") == 0) ||
									  std::find(i_BoolNames.begin(), i_BoolNames.end(), i_Name) != i_BoolNames.end();
			if (declaredBool && io_Type.m_Type == fx_SVT_UINT)
			{
				io_Type.m_Type = fx_SVT_BOOL;
				if (io_Type.m_Name.compare(0, 4, "uint") == 0)
				{
					io_Type.m_Name = "bool" + io_Type.m_Name.substr(4);
				}
			}
		}
	}

	//------------------------------------------------------------------------
	// Create()
	//------------------------------------------------------------------------
	std::unique_ptr<fxEffectVk> fxEffectVk::Create(vkdDevice* i_pDevice, const fxEffectDesc& i_Desc,
												   const fxBytecodeSource& i_Bytecode, std::string& o_Error,
												   const std::vector<std::string>& i_BoolNames)
	{
		std::unique_ptr<fxEffectVk> effect(new fxEffectVk());
		effect->m_Desc = i_Desc;
		effect->m_BoolNames = i_BoolNames;
		if (!effect->build(i_pDevice, i_Bytecode, o_Error))
		{
			o_Error = i_Desc.m_Hlsl + ": " + o_Error;
			return nullptr;
		}
		return effect;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fxEffectVk::~fxEffectVk()
	{
	}

	//------------------------------------------------------------------------
	// build() - reflect every pass's stages, then set up the constant buffers
	//------------------------------------------------------------------------
	bool fxEffectVk::build(vkdDevice* i_pDevice, const fxBytecodeSource& i_Bytecode, std::string& o_Error)
	{
		for (const fxTechniqueDesc& tech : m_Desc.m_Techniques)
		{
			std::vector<Pipeline> passes;
			for (const fxPassDesc& pass : tech.m_Passes)
			{
				if (pass.m_Stages[fx_CS].IsUsed())
				{
					o_Error = tech.m_Name + "/" + pass.m_Name + ": compute passes are not supported yet";
					return false;
				}
				Pipeline pipeline;
				for (int s = 0; s < fx_NumGraphicsStages; s++)
				{
					if (!pass.m_Stages[s].IsUsed())
					{
						continue;
					}
					bool deferred = false;
					const Shader* shader = get_shader(i_pDevice, (fxStage)s, pass.m_Stages[s], i_Bytecode, deferred, o_Error);
					if (deferred)
					{
						pipeline.m_bDeferred = true;
						continue;
					}
					if (!shader)
					{
						return false;
					}
					pipeline.m_Stages[s] = shader;
					for (const Binding& b : shader->m_ConstantBuffers)
					{
						if (std::find(pipeline.m_ConstantBuffers.begin(), pipeline.m_ConstantBuffers.end(), b.m_Index)
							== pipeline.m_ConstantBuffers.end())
						{
							pipeline.m_ConstantBuffers.push_back(b.m_Index);
						}
					}
				}
				passes.push_back(pipeline);
			}
			m_Pipelines.push_back(passes);
		}

		apply_defaults();

		for (ConstantBuffer& cb : m_ConstantBuffers)
		{
			cb.m_Defaults = cb.m_Data;
			cb.m_bDirty = true;
		}
		return true;
	}

	//------------------------------------------------------------------------
	// get_shader() - shaders are shared between passes. A bytecode source
	//	that answers true with no data marks a deferred stage.
	//------------------------------------------------------------------------
	const fxEffectVk::Shader* fxEffectVk::get_shader(vkdDevice* i_pDevice, fxStage i_Stage, const fxStageDesc& i_Desc,
		const fxBytecodeSource& i_Bytecode, bool& o_bDeferred, std::string& o_Error)
	{
		o_bDeferred = false;
		std::string key = i_Desc.m_Entry + "|" + i_Desc.m_Profile;
		auto found = m_Shaders.find(key);
		if (found != m_Shaders.end())
		{
			return found->second.get();
		}

		std::unique_ptr<Shader> shader(new Shader());
		shader->m_Stage = i_Stage;
		if (!i_Bytecode(i_Desc.m_Entry, i_Desc.m_Profile, shader->m_Bytecode))
		{
			o_Error = "no bytecode for " + i_Desc.m_Entry + " (" + i_Desc.m_Profile + ")";
			return nullptr;
		}
		if (!shader->m_Bytecode.m_pData)
		{
			o_bDeferred = true;
			return nullptr;
		}

		if (!vkdReflect(shader->m_Bytecode.m_pData, shader->m_Bytecode.m_Size, shader->m_Reflection, o_Error) ||
			!reflect(*shader, o_Error))
		{
			o_Error = i_Desc.m_Entry + ": " + o_Error;
			return nullptr;
		}
		if (shader->m_Reflection.m_Stage != k_StageBits[i_Stage])
		{
			o_Error = i_Desc.m_Entry + ": SPIR-V is for another stage than " + i_Desc.m_Profile;
			return nullptr;
		}
		(void)i_pDevice;	// shader modules: see vkdShader

		const Shader* result = shader.get();
		m_Shaders[key] = std::move(shader);
		return result;
	}

	//------------------------------------------------------------------------
	// reflect() - record what the shader reads and at which register
	//------------------------------------------------------------------------
	bool fxEffectVk::reflect(Shader& io_Shader, std::string& o_Error)
	{
		for (const vkdShaderBuffer& buffer : io_Shader.m_Reflection.m_ConstantBuffers)
		{
			const std::string& name = buffer.m_Name;
			int index = -1;
			for (size_t i = 0; i < m_ConstantBuffers.size(); i++)
			{
				if (m_ConstantBuffers[i].m_Name == name)
				{
					index = (int)i;
				}
			}
			bool isNew = (index < 0);
			if (isNew)
			{
				index = (int)m_ConstantBuffers.size();
				ConstantBuffer cb;
				cb.m_Name = name;
				cb.m_Size = buffer.m_Size;
				cb.m_Data.assign(buffer.m_Size, 0);
				m_ConstantBuffers.push_back(cb);
			}
			else if (m_ConstantBuffers[index].m_Size != buffer.m_Size)
			{
				o_Error = "constant buffer " + name + " has a different layout than in another entry point;"
					" declare its variables in an explicit cbuffer";
				return false;
			}

			for (const vkdShaderVariable& var : buffer.m_Variables)
			{
				Variable entry;
				entry.m_Constant.m_Buffer = index;
				entry.m_Constant.m_Offset = var.m_Offset;
				entry.m_Constant.m_Size = var.m_Size;
				entry.m_Constant.m_bColumnMajor = var.m_bColumnMajor;
				entry.m_Constant.m_Columns = (uint16_t)var.m_Type.m_Columns;
				entry.m_Constant.m_Elements = (uint16_t)var.m_Type.m_Elements;
				entry.m_Type = var.m_Type;
				fix_bool(entry.m_Type, var.m_Name, m_Desc.FindVariable(var.m_Name), m_BoolNames);

				auto existing = m_Variables.find(var.m_Name);
				if (existing == m_Variables.end())
				{
					if (!isNew)
					{
						o_Error = "constant buffer " + name + " has a different layout than in another entry point";
						return false;
					}
					m_Variables[var.m_Name] = entry;
				}
				else if (existing->second.m_Constant.m_Buffer != index ||
						 existing->second.m_Constant.m_Offset != entry.m_Constant.m_Offset ||
						 existing->second.m_Constant.m_Size != entry.m_Constant.m_Size)
				{
					o_Error = "variable " + var.m_Name + " is laid out differently in two entry points;"
						" declare it in an explicit cbuffer";
					return false;
				}
			}
			io_Shader.m_ConstantBuffers.push_back({ buffer.m_Slot, index });
		}

		for (const vkdShaderResource& resource : io_Shader.m_Reflection.m_Resources)
		{
			if (resource.m_Count != 1)
			{
				o_Error = resource.m_Name + ": resource arrays are not supported yet";
				return false;
			}
			switch (resource.m_Kind)
			{
				case vkdShaderResource::e_Texture:
				{
					auto it = m_ResourceNames.find(resource.m_Name);
					int index;
					if (it == m_ResourceNames.end())
					{
						index = (int)m_Resources.size();
						m_Resources.push_back(nullptr);
						m_ResourceNames[resource.m_Name] = index;
					}
					else
					{
						index = it->second;
					}
					io_Shader.m_Resources.push_back({ resource.m_Slot, index });
					break;
				}
				case vkdShaderResource::e_Sampler:
					io_Shader.m_Samplers.push_back({ resource.m_Slot, get_sampler(resource.m_Name) });
					break;
				case vkdShaderResource::e_Uav:
				{
					auto it = m_UnorderedAccessNames.find(resource.m_Name);
					int index;
					if (it == m_UnorderedAccessNames.end())
					{
						index = (int)m_UnorderedAccess.size();
						m_UnorderedAccess.push_back(nullptr);
						m_UnorderedAccessNames[resource.m_Name] = index;
					}
					else
					{
						index = it->second;
					}
					io_Shader.m_UnorderedAccess.push_back({ resource.m_Slot, index });
					break;
				}
			}
		}
		return true;
	}

	//------------------------------------------------------------------------
	// get_sampler() - one sampler per name; created from the manifest (or
	//	the D3D defaults) when the effect is created on a device
	//------------------------------------------------------------------------
	int fxEffectVk::get_sampler(const std::string& i_Name)
	{
		auto it = m_SamplerNames.find(i_Name);
		if (it != m_SamplerNames.end())
		{
			return it->second;
		}
		int index = (int)m_SamplerList.size();
		m_SamplerList.push_back(i_Name);
		m_SamplerNames[i_Name] = index;
		return index;
	}

	//------------------------------------------------------------------------
	// apply_defaults() - initial values the .fx declared for its variables.
	//	Scalars and vectors, and arrays of them; array elements start on
	//	16-byte boundaries, as constant buffer packing requires.
	//------------------------------------------------------------------------
	void fxEffectVk::apply_defaults()
	{
		for (const fxVariableDesc& var : m_Desc.m_Variables)
		{
			if (var.m_Default.empty())
			{
				continue;
			}
			auto it = m_Variables.find(var.m_Name);
			if (it == m_Variables.end())
			{
				continue;	// no entry point reads it
			}
			const Variable& v = it->second;
			const Type& t = v.m_Type;
			if ((t.m_Rows > 1 && t.m_Columns > 1) || t.m_Class == fx_SVC_STRUCT)
			{
				continue;	// matrix and struct defaults are not supported yet
			}

			const bool isInt = (t.m_Type == fx_SVT_INT || t.m_Type == fx_SVT_UINT || t.m_Type == fx_SVT_BOOL);
			const uint32_t perElement = t.m_Columns * t.m_Rows;
			const uint32_t stride = (t.m_Elements > 0) ? 16 : perElement * 4;
			ConstantBuffer& cb = m_ConstantBuffers[v.m_Constant.m_Buffer];
			// one value for a vector fills every component, as HLSL does
			const bool broadcast = (var.m_Default.size() == 1 && t.m_Elements == 0 && perElement > 1);
			const size_t count = broadcast ? perElement : var.m_Default.size();
			for (size_t i = 0; i < count; i++)
			{
				uint32_t element = (uint32_t)(i / perElement);
				uint32_t component = (uint32_t)(i % perElement);
				const double def = var.m_Default[broadcast ? 0 : i];
				uint32_t offset = v.m_Constant.m_Offset + element * stride + component * 4;
				if (offset + 4 > v.m_Constant.m_Offset + v.m_Constant.m_Size || offset + 4 > cb.m_Size)
				{
					break;
				}
				if (isInt)
				{
					int32_t value = (int32_t)def;
					memcpy(&cb.m_Data[offset], &value, 4);
				}
				else
				{
					float value = (float)def;
					memcpy(&cb.m_Data[offset], &value, 4);
				}
			}
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	int fxEffectVk::FindTechnique(const std::string& i_Name) const
	{
		for (size_t i = 0; i < m_Desc.m_Techniques.size(); i++)
		{
			if (m_Desc.m_Techniques[i].m_Name == i_Name)
			{
				return (int)i;
			}
		}
		return -1;
	}

	int fxEffectVk::GetPassCount(int i_Technique) const
	{
		if (i_Technique < 0 || i_Technique >= (int)m_Pipelines.size())
		{
			return 0;
		}
		return (int)m_Pipelines[i_Technique].size();
	}

	bool fxEffectVk::IsPassDeferred(int i_Technique, int i_Pass) const
	{
		if (i_Pass < 0 || i_Pass >= GetPassCount(i_Technique))
		{
			return false;
		}
		return m_Pipelines[i_Technique][i_Pass].m_bDeferred;
	}

	const vkdShaderReflection* fxEffectVk::GetStageReflection(int i_Technique, int i_Pass, fxStage i_Stage) const
	{
		if (i_Pass < 0 || i_Pass >= GetPassCount(i_Technique) || i_Stage >= fx_NumGraphicsStages)
		{
			return nullptr;
		}
		const Shader* shader = m_Pipelines[i_Technique][i_Pass].m_Stages[i_Stage];
		return shader ? &shader->m_Reflection : nullptr;
	}

	std::vector<fxEffectVk::StageBinding> fxEffectVk::GetStageBindings(int i_Technique, int i_Pass, fxStage i_Stage) const
	{
		std::vector<StageBinding> bindings;
		if (i_Pass < 0 || i_Pass >= GetPassCount(i_Technique) || i_Stage >= fx_NumGraphicsStages)
		{
			return bindings;
		}
		const Shader* shader = m_Pipelines[i_Technique][i_Pass].m_Stages[i_Stage];
		if (!shader)
		{
			return bindings;
		}
		for (const Binding& b : shader->m_ConstantBuffers)
		{
			bindings.push_back({ StageBinding::e_ConstantBuffer, m_ConstantBuffers[b.m_Index].m_Name, b.m_Slot });
		}
		for (const auto& r : m_ResourceNames)
		{
			for (const Binding& b : shader->m_Resources)
			{
				if (b.m_Index == r.second)
				{
					bindings.push_back({ StageBinding::e_Texture, r.first, b.m_Slot });
				}
			}
		}
		for (const Binding& b : shader->m_Samplers)
		{
			bindings.push_back({ StageBinding::e_Sampler, m_SamplerList[b.m_Index], b.m_Slot });
		}
		for (const auto& u : m_UnorderedAccessNames)
		{
			for (const Binding& b : shader->m_UnorderedAccess)
			{
				if (b.m_Index == u.second)
				{
					bindings.push_back({ StageBinding::e_Uav, u.first, b.m_Slot });
				}
			}
		}
		return bindings;
	}

	fxEffectVk::Constant fxEffectVk::FindConstant(const std::string& i_Name) const
	{
		auto it = m_Variables.find(i_Name);
		return (it != m_Variables.end()) ? it->second.m_Constant : Constant();
	}

	fxEffectVk::Resource fxEffectVk::FindResource(const std::string& i_Name) const
	{
		Resource r;
		auto it = m_ResourceNames.find(i_Name);
		if (it != m_ResourceNames.end())
		{
			r.m_Index = it->second;
		}
		return r;
	}

	const fxEffectVk::Type* fxEffectVk::FindConstantType(const std::string& i_Name) const
	{
		auto it = m_Variables.find(i_Name);
		return (it != m_Variables.end()) ? &it->second.m_Type : nullptr;
	}

	std::vector<std::string> fxEffectVk::GetConstantNames() const
	{
		std::vector<std::string> names;
		for (const auto& v : m_Variables)
		{
			names.push_back(v.first);
		}
		return names;
	}

	fxEffectVk::UnorderedAccess fxEffectVk::FindUnorderedAccess(const std::string& i_Name) const
	{
		UnorderedAccess u;
		auto it = m_UnorderedAccessNames.find(i_Name);
		if (it != m_UnorderedAccessNames.end())
		{
			u.m_Index = it->second;
		}
		return u;
	}

	std::vector<std::string> fxEffectVk::GetUnorderedAccessNames() const
	{
		std::vector<std::string> names;
		for (const auto& u : m_UnorderedAccessNames)
		{
			names.push_back(u.first);
		}
		return names;
	}

	std::vector<std::string> fxEffectVk::GetResourceNames() const
	{
		std::vector<std::string> names;
		for (const auto& r : m_ResourceNames)
		{
			names.push_back(r.first);
		}
		return names;
	}

	std::vector<std::string> fxEffectVk::GetSamplerNames() const
	{
		std::vector<std::string> names;
		for (const auto& s : m_SamplerNames)
		{
			names.push_back(s.first);
		}
		return names;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void fxEffectVk::SetConstant(const Constant& i_Constant, const void* i_pData, uint32_t i_Bytes)
	{
		if (!i_Constant.IsValid())
		{
			return;
		}
		ConstantBuffer& cb = m_ConstantBuffers[i_Constant.m_Buffer];
		uint32_t bytes = (i_Bytes < i_Constant.m_Size) ? i_Bytes : i_Constant.m_Size;
		if (memcmp(&cb.m_Data[i_Constant.m_Offset], i_pData, bytes) != 0)
		{
			memcpy(&cb.m_Data[i_Constant.m_Offset], i_pData, bytes);
			cb.m_bDirty = true;
		}
	}

	void fxEffectVk::SetVectorArray(const Constant& i_Constant, const float* i_pPacked,
		uint32_t i_FirstElement, uint32_t i_Count)
	{
		if (!i_Constant.IsValid())
		{
			return;
		}
		const uint32_t elements = (i_Constant.m_Elements > 0) ? i_Constant.m_Elements : 1;
		const uint32_t columns = i_Constant.m_Columns;
		ConstantBuffer& cb = m_ConstantBuffers[i_Constant.m_Buffer];
		for (uint32_t i = 0; i < i_Count && i_FirstElement + i < elements; i++)
		{
			uint32_t offset = i_Constant.m_Offset + (i_FirstElement + i) * 16;
			const float* src = i_pPacked + i * columns;
			if (memcmp(&cb.m_Data[offset], src, columns * sizeof(float)) != 0)
			{
				memcpy(&cb.m_Data[offset], src, columns * sizeof(float));
				cb.m_bDirty = true;
			}
		}
	}

	void fxEffectVk::SetMatrix(const Constant& i_Constant, const float* i_pRowMajor4x4)
	{
		if (!i_Constant.m_bColumnMajor)
		{
			SetConstant(i_Constant, i_pRowMajor4x4, 16 * sizeof(float));
			return;
		}
		float transposed[16];
		for (int r = 0; r < 4; r++)
		{
			for (int c = 0; c < 4; c++)
			{
				transposed[c * 4 + r] = i_pRowMajor4x4[r * 4 + c];
			}
		}
		SetConstant(i_Constant, transposed, sizeof(transposed));
	}

	void fxEffectVk::SetResource(const Resource& i_Resource, vkdImage* i_pImage)
	{
		if (i_Resource.IsValid())
		{
			m_Resources[i_Resource.m_Index] = i_pImage;
		}
	}

	void fxEffectVk::SetUnorderedAccess(const UnorderedAccess& i_UAV, vkdImage* i_pImage)
	{
		if (i_UAV.IsValid())
		{
			m_UnorderedAccess[i_UAV.m_Index] = i_pImage;
		}
	}

	void fxEffectVk::GetConstant(const Constant& i_Constant, void* o_pData, uint32_t i_Bytes) const
	{
		if (!i_Constant.IsValid())
		{
			return;
		}
		const ConstantBuffer& cb = m_ConstantBuffers[i_Constant.m_Buffer];
		uint32_t bytes = (i_Bytes < i_Constant.m_Size) ? i_Bytes : i_Constant.m_Size;
		memcpy(o_pData, &cb.m_Data[i_Constant.m_Offset], bytes);
	}

	vkdImage* fxEffectVk::GetResource(const Resource& i_Resource) const
	{
		return i_Resource.IsValid() ? m_Resources[i_Resource.m_Index] : nullptr;
	}

	//------------------------------------------------------------------------
	// Whole constant buffers
	//------------------------------------------------------------------------
	int fxEffectVk::FindConstantBuffer(const std::string& i_Name) const
	{
		for (size_t i = 0; i < m_ConstantBuffers.size(); i++)
		{
			if (m_ConstantBuffers[i].m_Name == i_Name)
			{
				return (int)i;
			}
		}
		return -1;
	}

	std::vector<std::string> fxEffectVk::GetConstantBufferNames() const
	{
		std::vector<std::string> names;
		for (const ConstantBuffer& cb : m_ConstantBuffers)
		{
			names.push_back(cb.m_Name);
		}
		return names;
	}

	uint32_t fxEffectVk::GetConstantBufferSize(int i_Buffer) const
	{
		if (i_Buffer < 0 || i_Buffer >= (int)m_ConstantBuffers.size())
		{
			return 0;
		}
		return m_ConstantBuffers[i_Buffer].m_Size;
	}

	const void* fxEffectVk::GetConstantBufferData(int i_Buffer) const
	{
		if (i_Buffer < 0 || i_Buffer >= (int)m_ConstantBuffers.size())
		{
			return nullptr;
		}
		return m_ConstantBuffers[i_Buffer].m_Data.data();
	}

	const void* fxEffectVk::GetConstantBufferDefaults(int i_Buffer) const
	{
		if (i_Buffer < 0 || i_Buffer >= (int)m_ConstantBuffers.size())
		{
			return nullptr;
		}
		return m_ConstantBuffers[i_Buffer].m_Defaults.data();
	}

	void fxEffectVk::SetConstantBufferData(int i_Buffer, const void* i_pData, uint32_t i_Bytes)
	{
		if (i_Buffer < 0 || i_Buffer >= (int)m_ConstantBuffers.size())
		{
			return;
		}
		ConstantBuffer& cb = m_ConstantBuffers[i_Buffer];
		uint32_t bytes = (i_Bytes < cb.m_Size) ? i_Bytes : cb.m_Size;
		if (memcmp(cb.m_Data.data(), i_pData, bytes) != 0)
		{
			memcpy(cb.m_Data.data(), i_pData, bytes);
			cb.m_bDirty = true;
		}
	}
}
