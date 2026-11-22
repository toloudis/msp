/*****************************************************************************
**  fxEffectVk.cpp
**
**      Vulkan backend for plain-HLSL effects. See fxEffectVk.hpp. The
**      parameter bookkeeping is GraphicsDX11's fxEffectDX11.cpp, unchanged
**      except for types.
\****************************************************************************/
#include "GraphicsVulkan/Fx/fxEffectVk.hpp"
#ifndef GVK_VKDBINDINGS_HPP
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#endif
#ifndef GVK_VKDBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdBuffer.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif

#include <algorithm>
#include <cctype>
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

		bool same_nocase(const std::string& a, const char* b)
		{
			const size_t n = strlen(b);
			if (a.size() != n)
			{
				return false;
			}
			for (size_t i = 0; i < n; i++)
			{
				if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i]))
				{
					return false;
				}
			}
			return true;
		}

		//	D3D11 filter names (as Effects spelled them) -> min, mag, mip linear
		struct FilterName { const char* m_Name; bool m_bMin, m_bMag, m_bMip; };
		const FilterName c_Filters[] =
		{
			{ "MIN_MAG_MIP_POINT",					false, false, false },
			{ "MIN_MAG_POINT_MIP_LINEAR",			false, false, true },
			{ "MIN_POINT_MAG_LINEAR_MIP_POINT",		false, true, false },
			{ "MIN_POINT_MAG_MIP_LINEAR",			false, true, true },
			{ "MIN_LINEAR_MAG_MIP_POINT",			true, false, false },
			{ "MIN_LINEAR_MAG_POINT_MIP_LINEAR",	true, false, true },
			{ "MIN_MAG_LINEAR_MIP_POINT",			true, true, false },
			{ "MIN_MAG_MIP_LINEAR",					true, true, true },
			{ "ANISOTROPIC",						true, true, true },
		};

		struct NamedValue { const char* m_Name; int m_Value; };
		const NamedValue c_AddressModes[] =
		{
			{ "WRAP",			VK_SAMPLER_ADDRESS_MODE_REPEAT },
			{ "MIRROR",			VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT },
			{ "CLAMP",			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE },
			{ "BORDER",			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER },
			{ "MIRROR_ONCE",	VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE },
		};
		const NamedValue c_CompareOps[] =
		{
			{ "NEVER",			VK_COMPARE_OP_NEVER },
			{ "LESS",			VK_COMPARE_OP_LESS },
			{ "EQUAL",			VK_COMPARE_OP_EQUAL },
			{ "LESS_EQUAL",		VK_COMPARE_OP_LESS_OR_EQUAL },
			{ "GREATER",		VK_COMPARE_OP_GREATER },
			{ "NOT_EQUAL",		VK_COMPARE_OP_NOT_EQUAL },
			{ "GREATER_EQUAL",	VK_COMPARE_OP_GREATER_OR_EQUAL },
			{ "ALWAYS",			VK_COMPARE_OP_ALWAYS },
		};

		template <size_t N>
		bool lookup(const NamedValue (&i_Table)[N], const std::string& i_Name, int& o_Value)
		{
			for (const NamedValue& nv : i_Table)
			{
				if (same_nocase(i_Name, nv.m_Name))
				{
					o_Value = nv.m_Value;
					return true;
				}
			}
			return false;
		}

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
		effect->m_Name = i_Desc.m_Hlsl;
		if (!effect->build(i_pDevice, i_Bytecode, o_Error) || !effect->check_samplers(o_Error))
		{
			o_Error = i_Desc.m_Hlsl + ": " + o_Error;
			return nullptr;
		}
		//	A bound image or buffer destroyed before the effect is unbound (it
		//	reads as the null texture or buffer), where D3D11's view would keep
		//	it alive
		if (i_pDevice)
		{
			fxEffectVk* pEffect = effect.get();
			effect->m_pDevice = i_pDevice;
			effect->m_ImageDestroyHook = i_pDevice->AddImageDestroyHook([pEffect](const vkdImage* i_pImage)
			{
				pEffect->forget_image(i_pImage);
			});
			effect->m_BufferDestroyHook = i_pDevice->AddBufferDestroyHook([pEffect](const vkdBuffer* i_pBuffer)
			{
				pEffect->forget_buffer(i_pBuffer);
			});
		}
		return effect;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fxEffectVk::~fxEffectVk()
	{
		if (m_pDevice)
		{
			m_pDevice->RemoveImageDestroyHook(m_ImageDestroyHook);
			m_pDevice->RemoveBufferDestroyHook(m_BufferDestroyHook);
		}
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
					const Shader* shader = get_shader(i_pDevice, (fxStage)s, pass.m_Stages[s], i_Bytecode, o_Error);
					if (!shader)
					{
						return false;
					}
					pipeline.m_Stages[s] = shader;
					pipeline.m_bUavs |= !shader->m_UnorderedAccess.empty();
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
	// get_shader() - shaders are shared between passes
	//------------------------------------------------------------------------
	const fxEffectVk::Shader* fxEffectVk::get_shader(vkdDevice* i_pDevice, fxStage i_Stage, const fxStageDesc& i_Desc,
		const fxBytecodeSource& i_Bytecode, std::string& o_Error)
	{
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

		const std::string name = m_Name + " " + i_Desc.m_Entry;
		shader->m_pShader = vkdShader::Create(i_pDevice, k_StageBits[i_Stage], shader->m_Bytecode.m_pData,
											  shader->m_Bytecode.m_Size, i_Desc.m_Entry.c_str(), name.c_str(), o_Error);
		if (!shader->m_pShader || !reflect(*shader, o_Error))
		{
			o_Error = i_Desc.m_Entry + ": " + o_Error;
			return nullptr;
		}

		const Shader* result = shader.get();
		m_Shaders[key] = std::move(shader);
		return result;
	}

	//------------------------------------------------------------------------
	// reflect() - record what the shader reads and at which register
	//------------------------------------------------------------------------
	bool fxEffectVk::reflect(Shader& io_Shader, std::string& o_Error)
	{
		for (const vkdShaderBuffer& buffer : io_Shader.m_pShader->Reflection().m_ConstantBuffers)
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

		for (const vkdShaderResource& resource : io_Shader.m_pShader->Reflection().m_Resources)
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
						m_ResourceBuffers.push_back(nullptr);
						m_ResourceIsBuffer.push_back(resource.m_bStorageBuffer);
						m_ResourceNames[resource.m_Name] = index;
					}
					else
					{
						index = it->second;
						if (m_ResourceIsBuffer[index] != resource.m_bStorageBuffer)
						{
							o_Error = resource.m_Name + ": a buffer in one stage and a texture in another";
							return false;
						}
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

	const vkdShaderReflection* fxEffectVk::GetStageReflection(int i_Technique, int i_Pass, fxStage i_Stage) const
	{
		if (i_Pass < 0 || i_Pass >= GetPassCount(i_Technique) || i_Stage >= fx_NumGraphicsStages)
		{
			return nullptr;
		}
		const Shader* shader = m_Pipelines[i_Technique][i_Pass].m_Stages[i_Stage];
		return shader ? &shader->m_pShader->Reflection() : nullptr;
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

#pragma push_macro("FindResource")
#undef FindResource
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
#pragma pop_macro("FindResource")

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

	bool fxEffectVk::SetResource(const Resource& i_Resource, vkdImage* i_pImage)
	{
		if (!i_Resource.IsValid())
		{
			return true;
		}
		if (m_ResourceIsBuffer[i_Resource.m_Index])
		{
			return false;
		}
		m_Resources[i_Resource.m_Index] = i_pImage;
		return true;
	}

	bool fxEffectVk::SetBuffer(const Resource& i_Resource, vkdBuffer* i_pBuffer)
	{
		if (!i_Resource.IsValid())
		{
			return true;
		}
		if (!m_ResourceIsBuffer[i_Resource.m_Index])
		{
			return false;
		}
		m_ResourceBuffers[i_Resource.m_Index] = i_pBuffer;
		return true;
	}

	bool fxEffectVk::IsBufferResource(const Resource& i_Resource) const
	{
		return i_Resource.IsValid() && m_ResourceIsBuffer[i_Resource.m_Index];
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

	void fxEffectVk::forget_image(const vkdImage* i_pImage)
	{
		for (vkdImage*& image : m_Resources)
		{
			if (image == i_pImage)
			{
				image = nullptr;
			}
		}
		for (vkdImage*& image : m_UnorderedAccess)
		{
			if (image == i_pImage)
			{
				image = nullptr;
			}
		}
	}

	vkdImage* fxEffectVk::GetResource(const Resource& i_Resource) const
	{
		return i_Resource.IsValid() ? m_Resources[i_Resource.m_Index] : nullptr;
	}

	vkdBuffer* fxEffectVk::GetBuffer(const Resource& i_Resource) const
	{
		return i_Resource.IsValid() ? m_ResourceBuffers[i_Resource.m_Index] : nullptr;
	}

	void fxEffectVk::forget_buffer(const vkdBuffer* i_pBuffer)
	{
		for (vkdBuffer*& buffer : m_ResourceBuffers)
		{
			if (buffer == i_pBuffer)
			{
				buffer = nullptr;
			}
		}
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

	//------------------------------------------------------------------------
	// Samplers: every one the shaders use must map to Vulkan (checked by
	//	Create); the Vulkan samplers are made on first use
	//------------------------------------------------------------------------
	bool fxEffectVk::check_samplers(std::string& o_Error) const
	{
		for (const std::string& name : m_SamplerList)
		{
			fxSamplerDesc defaults;		// a sampler the .fx declared without state gets the D3D defaults
			defaults.m_Name = name;
			const fxSamplerDesc* desc = m_Desc.FindSampler(name);
			VkSamplerCreateInfo info;
			if (!fxSamplerToVulkan(desc ? *desc : defaults, info, o_Error))
			{
				return false;
			}
		}
		return true;
	}

	uint32_t fxEffectVk::sampler_index(int i_Sampler)
	{
		if (m_SamplerHeap.empty())
		{
			vkdBindings& bindings = vkdGlobal::Bindings();
			for (const std::string& name : m_SamplerList)
			{
				fxSamplerDesc defaults;
				defaults.m_Name = name;
				const fxSamplerDesc* desc = m_Desc.FindSampler(name);
				VkSamplerCreateInfo info;
				std::string error;
				fxSamplerToVulkan(desc ? *desc : defaults, info, error);
				m_SamplerHeap.push_back(bindings.AddSampler(info));
			}
		}
		return m_SamplerHeap[i_Sampler];
	}

	bool fxSamplerToVulkan(const fxSamplerDesc& i_Desc, VkSamplerCreateInfo& o_Info, std::string& o_Error)
	{
		o_Info = VkSamplerCreateInfo{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
		std::string filter = i_Desc.m_Filter;
		const bool comparison = filter.size() > 11 && same_nocase(filter.substr(0, 11), "COMPARISON_");
		if (comparison)
		{
			filter = filter.substr(11);
		}
		const FilterName* found = nullptr;
		for (const FilterName& f : c_Filters)
		{
			if (same_nocase(filter, f.m_Name))
			{
				found = &f;
			}
		}
		if (!found)
		{
			o_Error = "sampler " + i_Desc.m_Name + ": unknown filter " + i_Desc.m_Filter;
			return false;
		}
		o_Info.minFilter = found->m_bMin ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
		o_Info.magFilter = found->m_bMag ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
		o_Info.mipmapMode = found->m_bMip ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
		o_Info.maxAnisotropy = 1.0f;
		if (same_nocase(filter, "ANISOTROPIC"))
		{
			o_Info.anisotropyEnable = VK_TRUE;
			o_Info.maxAnisotropy = (float)std::min(std::max(i_Desc.m_MaxAnisotropy, 1), 16);
		}

		VkSamplerAddressMode* address[3] = { &o_Info.addressModeU, &o_Info.addressModeV, &o_Info.addressModeW };
		bool border = false;
		for (int i = 0; i < 3; i++)
		{
			int mode;
			if (!lookup(c_AddressModes, i_Desc.m_Address[i], mode))
			{
				o_Error = "sampler " + i_Desc.m_Name + ": unknown address mode " + i_Desc.m_Address[i];
				return false;
			}
			*address[i] = (VkSamplerAddressMode)mode;
			border |= (mode == VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
		}

		if (comparison)
		{
			int op;
			if (!lookup(c_CompareOps, i_Desc.m_ComparisonFunc, op))
			{
				o_Error = "sampler " + i_Desc.m_Name + ": unknown comparison " + i_Desc.m_ComparisonFunc;
				return false;
			}
			o_Info.compareEnable = VK_TRUE;
			o_Info.compareOp = (VkCompareOp)op;
		}

		//	Vulkan has three border colours; the manifests use only two of them
		const float* c = i_Desc.m_BorderColor;
		if (c[0] == 0 && c[1] == 0 && c[2] == 0 && c[3] == 0)
		{
			o_Info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		}
		else if (c[0] == 0 && c[1] == 0 && c[2] == 0 && c[3] == 1)
		{
			o_Info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		}
		else if (c[0] == 1 && c[1] == 1 && c[2] == 1 && c[3] == 1)
		{
			o_Info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		}
		else if (border)
		{
			o_Error = "sampler " + i_Desc.m_Name + ": border colour (" + std::to_string(c[0]) + ", " + std::to_string(c[1]) + ", " +
					  std::to_string(c[2]) + ", " + std::to_string(c[3]) + ") has no Vulkan equivalent";
			return false;
		}

		o_Info.mipLodBias = i_Desc.m_MipLODBias;
		o_Info.minLod = std::max(i_Desc.m_MinLOD, 0.0f);
		o_Info.maxLod = i_Desc.m_MaxLOD >= 3.4e38f ? VK_LOD_CLAMP_NONE : std::max(i_Desc.m_MaxLOD, o_Info.minLod);
		return true;
	}

	//------------------------------------------------------------------------
	// Apply()
	//------------------------------------------------------------------------
	void fxEffectVk::Apply(int i_Technique, int i_Pass)
	{
		if (i_Pass < 0 || i_Pass >= GetPassCount(i_Technique))
		{
			return;
		}
		const Pipeline& pipeline = m_Pipelines[i_Technique][i_Pass];
		vkdContext& context = vkdGlobal::Context();

		const vkdShader* stages[vkdPipelineKey::c_Stages] = {};
		for (int s = 0; s < fx_NumGraphicsStages; s++)
		{
			stages[s] = pipeline.m_Stages[s] ? pipeline.m_Stages[s]->m_pShader.get() : nullptr;
		}
		context.SetShaders(stages);

		//	Constant buffers: upload what changed, and what was uploaded for an
		//	earlier submission (the ring may have reused that memory since)
		vkdDevice& device = vkdGlobal::Device();
		for (int index : pipeline.m_ConstantBuffers)
		{
			ConstantBuffer& cb = m_ConstantBuffers[index];
			if (cb.m_bDirty || cb.m_UploadedFor != device.PendingValue())
			{
				cb.m_Gpu = context.Upload(cb.m_Data.data(), cb.m_Size).m_Gpu;
				cb.m_UploadedFor = device.PendingValue();
				cb.m_bDirty = false;
			}
		}

		//	One binding table serves every stage, where D3D11 had registers per
		//	stage: two stages of a pass can't need different things in one slot
		vkdBindings& bindings = context.Bindings();
		struct Slot { int m_Kind; uint32_t m_Slot; int m_Index; };
		std::vector<Slot> used;
		bool clash = false;
		auto claim = [&used, &clash](int i_Kind, uint32_t i_Slot, int i_Index)
		{
			for (const Slot& u : used)
			{
				if (u.m_Kind == i_Kind && u.m_Slot == i_Slot)
				{
					clash |= (u.m_Index != i_Index);
					return false;
				}
			}
			used.push_back({ i_Kind, i_Slot, i_Index });
			return true;
		};
		for (int s = 0; s < fx_NumGraphicsStages; s++)
		{
			const Shader* shader = pipeline.m_Stages[s];
			if (!shader)
			{
				continue;
			}
			for (const Binding& b : shader->m_ConstantBuffers)
			{
				if (claim(0, b.m_Slot, b.m_Index))
				{
					bindings.SetConstantBuffer(b.m_Slot, m_ConstantBuffers[b.m_Index].m_Gpu, m_ConstantBuffers[b.m_Index].m_Size);
				}
			}
			for (const Binding& b : shader->m_Resources)
			{
				if (claim(1, b.m_Slot, b.m_Index))
				{
					if (m_ResourceIsBuffer[b.m_Index])
					{
						vkdBuffer* buffer = m_ResourceBuffers[b.m_Index];
						bindings.SetTexture(b.m_Slot, buffer ? buffer->ResourceIndex(bindings) : bindings.NullBufferIndex());
					}
					else
					{
						vkdImage* image = m_Resources[b.m_Index] ? m_Resources[b.m_Index] : context.NullTexture();
						bindings.SetTexture(b.m_Slot, image->SampledIndex(bindings));
					}
				}
			}
			for (const Binding& b : shader->m_Samplers)
			{
				if (claim(2, b.m_Slot, b.m_Index))
				{
					bindings.SetSampler(b.m_Slot, sampler_index(b.m_Index));
				}
			}
			for (const Binding& b : shader->m_UnorderedAccess)
			{
				if (claim(3, b.m_Slot, b.m_Index))
				{
					vkdImage* image = m_UnorderedAccess[b.m_Index] ? m_UnorderedAccess[b.m_Index] : context.NullTexture();
					bindings.SetUav(b.m_Slot, image->StorageIndex(bindings));
				}
			}
		}
		context.SetWritesUavs(pipeline.m_bUavs);
		if (clash && !pipeline.m_bReported)
		{
			pipeline.m_bReported = true;
			DBG_LOG("fxEffectVk: " << m_Name << " " << m_Desc.m_Techniques[i_Technique].m_Passes[i_Pass].m_Name
					<< ": two stages read different things from one register; the Vulkan binding table holds one");
		}
	}
}
