/*****************************************************************************
**  fxEffectDX11.cpp
**
**      D3D11 backend for plain-HLSL effects. See fxEffectDX11.hpp.
\****************************************************************************/

#include "GraphicsDX11/Fx/fxEffectDX11.hpp"

#include <d3dcompiler.h>

#include <algorithm>
#include <cstring>
#include <string.h>

#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

namespace
{
	//--------------------------------------------------------------------
	// Sampler state names, as Effects spelled them (case-insensitive)
	//--------------------------------------------------------------------
	struct NamedValue
	{
		const char* m_Name;
		int m_Value;
	};

	const NamedValue k_Filters[] =
	{
		{ "MIN_MAG_MIP_POINT",								D3D11_FILTER_MIN_MAG_MIP_POINT },
		{ "MIN_MAG_POINT_MIP_LINEAR",						D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR },
		{ "MIN_POINT_MAG_LINEAR_MIP_POINT",					D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT },
		{ "MIN_POINT_MAG_MIP_LINEAR",						D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR },
		{ "MIN_LINEAR_MAG_MIP_POINT",						D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT },
		{ "MIN_LINEAR_MAG_POINT_MIP_LINEAR",				D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR },
		{ "MIN_MAG_LINEAR_MIP_POINT",						D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT },
		{ "MIN_MAG_MIP_LINEAR",								D3D11_FILTER_MIN_MAG_MIP_LINEAR },
		{ "ANISOTROPIC",									D3D11_FILTER_ANISOTROPIC },
		{ "COMPARISON_MIN_MAG_MIP_POINT",					D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT },
		{ "COMPARISON_MIN_MAG_POINT_MIP_LINEAR",			D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR },
		{ "COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT",		D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT },
		{ "COMPARISON_MIN_POINT_MAG_MIP_LINEAR",			D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR },
		{ "COMPARISON_MIN_LINEAR_MAG_MIP_POINT",			D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT },
		{ "COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR",		D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR },
		{ "COMPARISON_MIN_MAG_LINEAR_MIP_POINT",			D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT },
		{ "COMPARISON_MIN_MAG_MIP_LINEAR",					D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR },
		{ "COMPARISON_ANISOTROPIC",							D3D11_FILTER_COMPARISON_ANISOTROPIC },
	};

	const NamedValue k_AddressModes[] =
	{
		{ "WRAP",			D3D11_TEXTURE_ADDRESS_WRAP },
		{ "MIRROR",			D3D11_TEXTURE_ADDRESS_MIRROR },
		{ "CLAMP",			D3D11_TEXTURE_ADDRESS_CLAMP },
		{ "BORDER",			D3D11_TEXTURE_ADDRESS_BORDER },
		{ "MIRROR_ONCE",	D3D11_TEXTURE_ADDRESS_MIRROR_ONCE },
	};

	const NamedValue k_ComparisonFuncs[] =
	{
		{ "NEVER",			D3D11_COMPARISON_NEVER },
		{ "LESS",			D3D11_COMPARISON_LESS },
		{ "EQUAL",			D3D11_COMPARISON_EQUAL },
		{ "LESS_EQUAL",		D3D11_COMPARISON_LESS_EQUAL },
		{ "GREATER",		D3D11_COMPARISON_GREATER },
		{ "NOT_EQUAL",		D3D11_COMPARISON_NOT_EQUAL },
		{ "GREATER_EQUAL",	D3D11_COMPARISON_GREATER_EQUAL },
		{ "ALWAYS",			D3D11_COMPARISON_ALWAYS },
	};

	template <size_t N>
	bool lookup(const NamedValue (&i_Table)[N], const std::string& i_Name, int& o_Value)
	{
		for (const NamedValue& nv : i_Table)
		{
			if (_stricmp(nv.m_Name, i_Name.c_str()) == 0)
			{
				o_Value = nv.m_Value;
				return true;
			}
		}
		return false;
	}

	bool to_d3d(const fxSamplerDesc& i_Desc, D3D11_SAMPLER_DESC& o_Desc, std::string& o_Error)
	{
		int v = 0;
		o_Desc = D3D11_SAMPLER_DESC();
		if (!lookup(k_Filters, i_Desc.m_Filter, v))
		{
			o_Error = "sampler " + i_Desc.m_Name + ": unknown filter " + i_Desc.m_Filter;
			return false;
		}
		o_Desc.Filter = (D3D11_FILTER)v;

		D3D11_TEXTURE_ADDRESS_MODE* address[3] = { &o_Desc.AddressU, &o_Desc.AddressV, &o_Desc.AddressW };
		for (int i = 0; i < 3; i++)
		{
			if (!lookup(k_AddressModes, i_Desc.m_Address[i], v))
			{
				o_Error = "sampler " + i_Desc.m_Name + ": unknown address mode " + i_Desc.m_Address[i];
				return false;
			}
			*address[i] = (D3D11_TEXTURE_ADDRESS_MODE)v;
		}

		if (!lookup(k_ComparisonFuncs, i_Desc.m_ComparisonFunc, v))
		{
			o_Error = "sampler " + i_Desc.m_Name + ": unknown comparison " + i_Desc.m_ComparisonFunc;
			return false;
		}
		o_Desc.ComparisonFunc = (D3D11_COMPARISON_FUNC)v;

		o_Desc.MipLODBias = i_Desc.m_MipLODBias;
		o_Desc.MaxAnisotropy = (UINT)i_Desc.m_MaxAnisotropy;
		for (int i = 0; i < 4; i++)
			o_Desc.BorderColor[i] = i_Desc.m_BorderColor[i];
		o_Desc.MinLOD = i_Desc.m_MinLOD;
		o_Desc.MaxLOD = i_Desc.m_MaxLOD;
		return true;
	}

	//--------------------------------------------------------------------
	// Per-stage binding calls
	//--------------------------------------------------------------------
	void set_shader(ID3D11DeviceContext* i_pContext, int i_Stage, ID3D11DeviceChild* i_pShader)
	{
		switch (i_Stage)
		{
		case fx_VS: i_pContext->VSSetShader(static_cast<ID3D11VertexShader*>(i_pShader), NULL, 0); break;
		case fx_HS: i_pContext->HSSetShader(static_cast<ID3D11HullShader*>(i_pShader), NULL, 0); break;
		case fx_DS: i_pContext->DSSetShader(static_cast<ID3D11DomainShader*>(i_pShader), NULL, 0); break;
		case fx_GS: i_pContext->GSSetShader(static_cast<ID3D11GeometryShader*>(i_pShader), NULL, 0); break;
		case fx_PS: i_pContext->PSSetShader(static_cast<ID3D11PixelShader*>(i_pShader), NULL, 0); break;
		}
	}

	void set_constant_buffer(ID3D11DeviceContext* i_pContext, int i_Stage, UINT i_Slot, ID3D11Buffer* i_pBuffer)
	{
		switch (i_Stage)
		{
		case fx_VS: i_pContext->VSSetConstantBuffers(i_Slot, 1, &i_pBuffer); break;
		case fx_HS: i_pContext->HSSetConstantBuffers(i_Slot, 1, &i_pBuffer); break;
		case fx_DS: i_pContext->DSSetConstantBuffers(i_Slot, 1, &i_pBuffer); break;
		case fx_GS: i_pContext->GSSetConstantBuffers(i_Slot, 1, &i_pBuffer); break;
		case fx_PS: i_pContext->PSSetConstantBuffers(i_Slot, 1, &i_pBuffer); break;
		}
	}

	void set_resource(ID3D11DeviceContext* i_pContext, int i_Stage, UINT i_Slot, ID3D11ShaderResourceView* i_pView)
	{
		switch (i_Stage)
		{
		case fx_VS: i_pContext->VSSetShaderResources(i_Slot, 1, &i_pView); break;
		case fx_HS: i_pContext->HSSetShaderResources(i_Slot, 1, &i_pView); break;
		case fx_DS: i_pContext->DSSetShaderResources(i_Slot, 1, &i_pView); break;
		case fx_GS: i_pContext->GSSetShaderResources(i_Slot, 1, &i_pView); break;
		case fx_PS: i_pContext->PSSetShaderResources(i_Slot, 1, &i_pView); break;
		}
	}

	void set_sampler(ID3D11DeviceContext* i_pContext, int i_Stage, UINT i_Slot, ID3D11SamplerState* i_pSampler)
	{
		switch (i_Stage)
		{
		case fx_VS: i_pContext->VSSetSamplers(i_Slot, 1, &i_pSampler); break;
		case fx_HS: i_pContext->HSSetSamplers(i_Slot, 1, &i_pSampler); break;
		case fx_DS: i_pContext->DSSetSamplers(i_Slot, 1, &i_pSampler); break;
		case fx_GS: i_pContext->GSSetSamplers(i_Slot, 1, &i_pSampler); break;
		case fx_PS: i_pContext->PSSetSamplers(i_Slot, 1, &i_pSampler); break;
		}
	}

	//--------------------------------------------------------------------
	// A reflected type, with struct members
	//--------------------------------------------------------------------
	fxEffectDX11::Type to_type(ID3D11ShaderReflectionType* i_pType)
	{
		D3D11_SHADER_TYPE_DESC desc;
		i_pType->GetDesc(&desc);
		fxEffectDX11::Type type;
		type.m_Name = desc.Name ? desc.Name : "";
		type.m_Class = desc.Class;
		type.m_Type = desc.Type;
		type.m_Rows = desc.Rows;
		type.m_Columns = desc.Columns;
		type.m_Elements = desc.Elements;
		for (UINT m = 0; m < desc.Members; m++)
		{
			ID3D11ShaderReflectionType* memberType = i_pType->GetMemberTypeByIndex(m);
			D3D11_SHADER_TYPE_DESC memberDesc;
			memberType->GetDesc(&memberDesc);
			fxEffectDX11::Type::Member member;
			member.m_Name = i_pType->GetMemberTypeName(m);
			member.m_Offset = memberDesc.Offset;
			member.m_Type = to_type(memberType);
			type.m_Members.push_back(member);
		}
		return type;
	}

	std::string hr_string(HRESULT i_hr)
	{
		char buf[16];
		sprintf_s(buf, "0x%08X", (unsigned)i_hr);
		return buf;
	}
}

//------------------------------------------------------------------------
// Create()
//------------------------------------------------------------------------
std::unique_ptr<fxEffectDX11> fxEffectDX11::Create(ID3D11Device* i_pDevice,
	const fxEffectDesc& i_Desc, const fxBytecodeSource& i_Bytecode, std::string& o_Error)
{
	std::unique_ptr<fxEffectDX11> effect(new fxEffectDX11());
	effect->m_Desc = i_Desc;
	if (!effect->build(i_pDevice, i_Bytecode, o_Error))
	{
		o_Error = i_Desc.m_Hlsl + ": " + o_Error;
		return nullptr;
	}
	return effect;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
fxEffectDX11::~fxEffectDX11()
{
}

//------------------------------------------------------------------------
// build() - create every pass's pipeline, then the constant buffers
//------------------------------------------------------------------------
bool fxEffectDX11::build(ID3D11Device* i_pDevice, const fxBytecodeSource& i_Bytecode, std::string& o_Error)
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
					continue;
				const Shader* shader = get_shader(i_pDevice, (fxStage)s, pass.m_Stages[s], i_Bytecode, o_Error);
				if (!shader)
					return false;
				pipeline.m_Stages[s] = shader;
				for (const Binding& b : shader->m_ConstantBuffers)
				{
					if (std::find(pipeline.m_ConstantBuffers.begin(), pipeline.m_ConstantBuffers.end(), b.m_Index)
						== pipeline.m_ConstantBuffers.end())
						pipeline.m_ConstantBuffers.push_back(b.m_Index);
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
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = (cb.m_Size + 15) & ~15u;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HRESULT hr = i_pDevice->CreateBuffer(&desc, NULL, &cb.m_pBuffer);
		if (FAILED(hr))
		{
			o_Error = "CreateBuffer failed for constant buffer " + cb.m_Name + " (" + hr_string(hr) + ")";
			return false;
		}
		cb.m_bDirty = true;
	}
	return true;
}

//------------------------------------------------------------------------
// get_shader() - shader objects are shared between passes
//------------------------------------------------------------------------
const fxEffectDX11::Shader* fxEffectDX11::get_shader(ID3D11Device* i_pDevice, fxStage i_Stage,
	const fxStageDesc& i_Desc, const fxBytecodeSource& i_Bytecode, std::string& o_Error)
{
	std::string key = i_Desc.m_Entry + "|" + i_Desc.m_Profile;
	auto found = m_Shaders.find(key);
	if (found != m_Shaders.end())
		return found->second.get();

	std::unique_ptr<Shader> shader(new Shader());
	if (!i_Bytecode(i_Desc.m_Entry, i_Desc.m_Profile, shader->m_Bytecode))
	{
		o_Error = "no bytecode for " + i_Desc.m_Entry + " (" + i_Desc.m_Profile + ")";
		return nullptr;
	}

	const void* code = shader->m_Bytecode.m_pData;
	SIZE_T size = shader->m_Bytecode.m_Size;
	HRESULT hr = E_FAIL;
	switch (i_Stage)
	{
	case fx_VS:
		{
			ComPtr<ID3D11VertexShader> vs;
			hr = i_pDevice->CreateVertexShader(code, size, NULL, &vs);
			shader->m_pShader = vs;
		}
		break;
	case fx_HS:
		{
			ComPtr<ID3D11HullShader> hs;
			hr = i_pDevice->CreateHullShader(code, size, NULL, &hs);
			shader->m_pShader = hs;
		}
		break;
	case fx_DS:
		{
			ComPtr<ID3D11DomainShader> ds;
			hr = i_pDevice->CreateDomainShader(code, size, NULL, &ds);
			shader->m_pShader = ds;
		}
		break;
	case fx_GS:
		{
			ComPtr<ID3D11GeometryShader> gs;
			hr = i_pDevice->CreateGeometryShader(code, size, NULL, &gs);
			shader->m_pShader = gs;
		}
		break;
	case fx_PS:
		{
			ComPtr<ID3D11PixelShader> ps;
			hr = i_pDevice->CreatePixelShader(code, size, NULL, &ps);
			shader->m_pShader = ps;
		}
		break;
	default:
		break;
	}
	if (FAILED(hr))
	{
		o_Error = "creating shader " + i_Desc.m_Entry + " (" + i_Desc.m_Profile + ") failed (" + hr_string(hr) + ")";
		return nullptr;
	}

	if (!reflect(i_pDevice, *shader, o_Error))
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
bool fxEffectDX11::reflect(ID3D11Device* i_pDevice, Shader& io_Shader, std::string& o_Error)
{
	ComPtr<ID3D11ShaderReflection> refl;
	HRESULT hr = D3DReflect(io_Shader.m_Bytecode.m_pData, io_Shader.m_Bytecode.m_Size,
		__uuidof(ID3D11ShaderReflection), (void**)refl.GetAddressOf());
	if (FAILED(hr))
	{
		o_Error = "D3DReflect failed (" + hr_string(hr) + ")";
		return false;
	}

	D3D11_SHADER_DESC desc;
	refl->GetDesc(&desc);
	for (UINT r = 0; r < desc.BoundResources; r++)
	{
		D3D11_SHADER_INPUT_BIND_DESC bind;
		refl->GetResourceBindingDesc(r, &bind);
		std::string name = bind.Name;

		if (bind.BindCount != 1 && bind.Type != D3D_SIT_CBUFFER)
		{
			o_Error = name + ": resource arrays are not supported yet";
			return false;
		}

		switch (bind.Type)
		{
		case D3D_SIT_CBUFFER:
			{
				ID3D11ShaderReflectionConstantBuffer* cb = refl->GetConstantBufferByName(bind.Name);
				D3D11_SHADER_BUFFER_DESC cbDesc;
				cb->GetDesc(&cbDesc);

				int index = -1;
				for (size_t i = 0; i < m_ConstantBuffers.size(); i++)
					if (m_ConstantBuffers[i].m_Name == name)
						index = (int)i;
				bool isNew = (index < 0);
				if (isNew)
				{
					index = (int)m_ConstantBuffers.size();
					ConstantBuffer buffer;
					buffer.m_Name = name;
					buffer.m_Size = cbDesc.Size;
					buffer.m_Data.assign(cbDesc.Size, 0);
					m_ConstantBuffers.push_back(buffer);
				}
				else if (m_ConstantBuffers[index].m_Size != cbDesc.Size)
				{
					o_Error = "constant buffer " + name + " has a different layout than in another entry point;"
						" declare its variables in an explicit cbuffer";
					return false;
				}

				for (UINT v = 0; v < cbDesc.Variables; v++)
				{
					ID3D11ShaderReflectionVariable* var = cb->GetVariableByIndex(v);
					D3D11_SHADER_VARIABLE_DESC varDesc;
					var->GetDesc(&varDesc);
					D3D11_SHADER_TYPE_DESC typeDesc;
					var->GetType()->GetDesc(&typeDesc);

					Variable entry;
					entry.m_Constant.m_Buffer = index;
					entry.m_Constant.m_Offset = varDesc.StartOffset;
					entry.m_Constant.m_Size = varDesc.Size;
					entry.m_Constant.m_bColumnMajor = (typeDesc.Class == D3D_SVC_MATRIX_COLUMNS);
					entry.m_Constant.m_Columns = (uint16_t)typeDesc.Columns;
					entry.m_Constant.m_Elements = (uint16_t)typeDesc.Elements;
					entry.m_Type = to_type(var->GetType());

					auto existing = m_Variables.find(varDesc.Name);
					if (existing == m_Variables.end())
					{
						if (!isNew)
						{
							o_Error = "constant buffer " + name + " has a different layout than in another entry point";
							return false;
						}
						m_Variables[varDesc.Name] = entry;
					}
					else if (existing->second.m_Constant.m_Buffer != index ||
							 existing->second.m_Constant.m_Offset != entry.m_Constant.m_Offset ||
							 existing->second.m_Constant.m_Size != entry.m_Constant.m_Size)
					{
						o_Error = std::string("variable ") + varDesc.Name + " is laid out differently in two entry points;"
							" declare it in an explicit cbuffer";
						return false;
					}
				}
				io_Shader.m_ConstantBuffers.push_back({ bind.BindPoint, index });
			}
			break;

		case D3D_SIT_TEXTURE:
		case D3D_SIT_STRUCTURED:
		case D3D_SIT_BYTEADDRESS:
		case D3D_SIT_TBUFFER:
			{
				auto it = m_ResourceNames.find(name);
				int index;
				if (it == m_ResourceNames.end())
				{
					index = (int)m_Resources.size();
					m_Resources.push_back(nullptr);
					m_ResourceNames[name] = index;
				}
				else
					index = it->second;
				io_Shader.m_Resources.push_back({ bind.BindPoint, index });
			}
			break;

		case D3D_SIT_SAMPLER:
			{
				int index = get_sampler(i_pDevice, name, o_Error);
				if (index < 0)
					return false;
				io_Shader.m_Samplers.push_back({ bind.BindPoint, index });
			}
			break;

		case D3D_SIT_UAV_RWTYPED:
		case D3D_SIT_UAV_RWSTRUCTURED:
		case D3D_SIT_UAV_RWBYTEADDRESS:
		case D3D_SIT_UAV_APPEND_STRUCTURED:
		case D3D_SIT_UAV_CONSUME_STRUCTURED:
		case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			{
				auto it = m_UnorderedAccessNames.find(name);
				int index;
				if (it == m_UnorderedAccessNames.end())
				{
					index = (int)m_UnorderedAccess.size();
					m_UnorderedAccess.push_back(nullptr);
					m_UnorderedAccessNames[name] = index;
				}
				else
					index = it->second;
				io_Shader.m_UnorderedAccess.push_back({ bind.BindPoint, index });
			}
			break;

		default:
			o_Error = name + ": unsupported resource type";
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------
// get_sampler() - immutable sampler objects, created once per name
//------------------------------------------------------------------------
int fxEffectDX11::get_sampler(ID3D11Device* i_pDevice, const std::string& i_Name, std::string& o_Error)
{
	auto it = m_SamplerNames.find(i_Name);
	if (it != m_SamplerNames.end())
		return it->second;

	// a sampler the .fx declared without state gets the D3D defaults
	fxSamplerDesc defaults;
	defaults.m_Name = i_Name;
	const fxSamplerDesc* desc = m_Desc.FindSampler(i_Name);

	D3D11_SAMPLER_DESC d3dDesc;
	if (!to_d3d(desc ? *desc : defaults, d3dDesc, o_Error))
		return -1;

	ComPtr<ID3D11SamplerState> sampler;
	HRESULT hr = i_pDevice->CreateSamplerState(&d3dDesc, &sampler);
	if (FAILED(hr))
	{
		o_Error = "CreateSamplerState failed for " + i_Name + " (" + hr_string(hr) + ")";
		return -1;
	}
	int index = (int)m_Samplers.size();
	m_Samplers.push_back(sampler);
	m_SamplerNames[i_Name] = index;
	return index;
}

//------------------------------------------------------------------------
// apply_defaults() - initial values the .fx declared for its variables.
//	Scalars and vectors, and arrays of them; array elements start on
//	16-byte boundaries, as constant buffer packing requires.
//------------------------------------------------------------------------
void fxEffectDX11::apply_defaults()
{
	for (const fxVariableDesc& var : m_Desc.m_Variables)
	{
		if (var.m_Default.empty())
			continue;
		auto it = m_Variables.find(var.m_Name);
		if (it == m_Variables.end())
			continue;	// no entry point reads it
		const Variable& v = it->second;
		const Type& t = v.m_Type;
		if ((t.m_Rows > 1 && t.m_Columns > 1) || t.m_Class == D3D_SVC_STRUCT)
			continue;	// matrix and struct defaults are not supported yet

		const bool isInt = (t.m_Type == D3D_SVT_INT || t.m_Type == D3D_SVT_UINT || t.m_Type == D3D_SVT_BOOL);
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
				break;
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
int fxEffectDX11::FindTechnique(const std::string& i_Name) const
{
	for (size_t i = 0; i < m_Desc.m_Techniques.size(); i++)
		if (m_Desc.m_Techniques[i].m_Name == i_Name)
			return (int)i;
	return -1;
}

int fxEffectDX11::GetPassCount(int i_Technique) const
{
	if (i_Technique < 0 || i_Technique >= (int)m_Pipelines.size())
		return 0;
	return (int)m_Pipelines[i_Technique].size();
}

fxEffectDX11::Constant fxEffectDX11::FindConstant(const std::string& i_Name) const
{
	auto it = m_Variables.find(i_Name);
	return (it != m_Variables.end()) ? it->second.m_Constant : Constant();
}

fxEffectDX11::Resource fxEffectDX11::FindResource(const std::string& i_Name) const
{
	Resource r;
	auto it = m_ResourceNames.find(i_Name);
	if (it != m_ResourceNames.end())
		r.m_Index = it->second;
	return r;
}

const fxEffectDX11::Type* fxEffectDX11::FindConstantType(const std::string& i_Name) const
{
	auto it = m_Variables.find(i_Name);
	return (it != m_Variables.end()) ? &it->second.m_Type : nullptr;
}

std::vector<std::string> fxEffectDX11::GetConstantNames() const
{
	std::vector<std::string> names;
	for (const auto& v : m_Variables)
		names.push_back(v.first);
	return names;
}

fxEffectDX11::UnorderedAccess fxEffectDX11::FindUnorderedAccess(const std::string& i_Name) const
{
	UnorderedAccess u;
	auto it = m_UnorderedAccessNames.find(i_Name);
	if (it != m_UnorderedAccessNames.end())
		u.m_Index = it->second;
	return u;
}

std::vector<std::string> fxEffectDX11::GetUnorderedAccessNames() const
{
	std::vector<std::string> names;
	for (const auto& u : m_UnorderedAccessNames)
		names.push_back(u.first);
	return names;
}

std::vector<std::string> fxEffectDX11::GetResourceNames() const
{
	std::vector<std::string> names;
	for (const auto& r : m_ResourceNames)
		names.push_back(r.first);
	return names;
}

//------------------------------------------------------------------------
// Type::GetElementSize() - registers are 16 bytes; a vector never straddles
//	one, a matrix takes one register per row (row_major) or column
//------------------------------------------------------------------------
uint32_t fxEffectDX11::Type::GetElementSize() const
{
	switch (m_Class)
	{
	case D3D_SVC_SCALAR:
	case D3D_SVC_VECTOR:
		return m_Columns * 4;
	case D3D_SVC_MATRIX_ROWS:
		return (m_Rows - 1) * 16 + m_Columns * 4;
	case D3D_SVC_MATRIX_COLUMNS:
		return (m_Columns - 1) * 16 + m_Rows * 4;
	case D3D_SVC_STRUCT:
		{
			uint32_t size = 0;
			for (const Member& m : m_Members)
			{
				uint32_t memberSize = m.m_Type.GetElementSize();
				if (m.m_Type.m_Elements > 0)
					memberSize += (m.m_Type.m_Elements - 1) * ((memberSize + 15) & ~15u);
				if (m.m_Offset + memberSize > size)
					size = m.m_Offset + memberSize;
			}
			return size;
		}
	default:
		return 0;
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fxEffectDX11::SetConstant(const Constant& i_Constant, const void* i_pData, uint32_t i_Bytes)
{
	if (!i_Constant.IsValid())
		return;
	ConstantBuffer& cb = m_ConstantBuffers[i_Constant.m_Buffer];
	uint32_t bytes = (i_Bytes < i_Constant.m_Size) ? i_Bytes : i_Constant.m_Size;
	if (memcmp(&cb.m_Data[i_Constant.m_Offset], i_pData, bytes) != 0)
	{
		memcpy(&cb.m_Data[i_Constant.m_Offset], i_pData, bytes);
		cb.m_bDirty = true;
	}
}

void fxEffectDX11::SetVectorArray(const Constant& i_Constant, const float* i_pPacked,
	uint32_t i_FirstElement, uint32_t i_Count)
{
	if (!i_Constant.IsValid())
		return;
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

void fxEffectDX11::SetMatrix(const Constant& i_Constant, const float* i_pRowMajor4x4)
{
	if (!i_Constant.m_bColumnMajor)
	{
		SetConstant(i_Constant, i_pRowMajor4x4, 16 * sizeof(float));
		return;
	}
	float transposed[16];
	for (int r = 0; r < 4; r++)
		for (int c = 0; c < 4; c++)
			transposed[c * 4 + r] = i_pRowMajor4x4[r * 4 + c];
	SetConstant(i_Constant, transposed, sizeof(transposed));
}

void fxEffectDX11::SetResource(const Resource& i_Resource, ID3D11ShaderResourceView* i_pView)
{
	if (i_Resource.IsValid())
		m_Resources[i_Resource.m_Index] = i_pView;
}

void fxEffectDX11::SetUnorderedAccess(const UnorderedAccess& i_UAV, ID3D11UnorderedAccessView* i_pView)
{
	if (i_UAV.IsValid())
		m_UnorderedAccess[i_UAV.m_Index] = i_pView;
}

void fxEffectDX11::GetConstant(const Constant& i_Constant, void* o_pData, uint32_t i_Bytes) const
{
	if (!i_Constant.IsValid())
		return;
	const ConstantBuffer& cb = m_ConstantBuffers[i_Constant.m_Buffer];
	uint32_t bytes = (i_Bytes < i_Constant.m_Size) ? i_Bytes : i_Constant.m_Size;
	memcpy(o_pData, &cb.m_Data[i_Constant.m_Offset], bytes);
}

ID3D11ShaderResourceView* fxEffectDX11::GetResource(const Resource& i_Resource) const
{
	return i_Resource.IsValid() ? m_Resources[i_Resource.m_Index].Get() : nullptr;
}

//------------------------------------------------------------------------
// Whole constant buffers
//------------------------------------------------------------------------
int fxEffectDX11::FindConstantBuffer(const std::string& i_Name) const
{
	for (size_t i = 0; i < m_ConstantBuffers.size(); i++)
		if (m_ConstantBuffers[i].m_Name == i_Name)
			return (int)i;
	return -1;
}

uint32_t fxEffectDX11::GetConstantBufferSize(int i_Buffer) const
{
	if (i_Buffer < 0 || i_Buffer >= (int)m_ConstantBuffers.size())
		return 0;
	return m_ConstantBuffers[i_Buffer].m_Size;
}

const void* fxEffectDX11::GetConstantBufferData(int i_Buffer) const
{
	if (i_Buffer < 0 || i_Buffer >= (int)m_ConstantBuffers.size())
		return nullptr;
	return m_ConstantBuffers[i_Buffer].m_Data.data();
}

const void* fxEffectDX11::GetConstantBufferDefaults(int i_Buffer) const
{
	if (i_Buffer < 0 || i_Buffer >= (int)m_ConstantBuffers.size())
		return nullptr;
	return m_ConstantBuffers[i_Buffer].m_Defaults.data();
}

void fxEffectDX11::SetConstantBufferData(int i_Buffer, const void* i_pData, uint32_t i_Bytes)
{
	if (i_Buffer < 0 || i_Buffer >= (int)m_ConstantBuffers.size())
		return;
	ConstantBuffer& cb = m_ConstantBuffers[i_Buffer];
	uint32_t bytes = (i_Bytes < cb.m_Size) ? i_Bytes : cb.m_Size;
	if (memcmp(cb.m_Data.data(), i_pData, bytes) != 0)
	{
		memcpy(cb.m_Data.data(), i_pData, bytes);
		cb.m_bDirty = true;
	}
}

//------------------------------------------------------------------------
// Apply()
//------------------------------------------------------------------------
void fxEffectDX11::Apply(int i_Technique, int i_Pass, ID3D11DeviceContext* i_pContext)
{
	if (i_Pass < 0 || i_Pass >= GetPassCount(i_Technique))
		return;
	const Pipeline& pipeline = m_Pipelines[i_Technique][i_Pass];

	for (int index : pipeline.m_ConstantBuffers)
	{
		ConstantBuffer& cb = m_ConstantBuffers[index];
		if (!cb.m_bDirty)
			continue;
		D3D11_MAPPED_SUBRESOURCE mapped;
		if (SUCCEEDED(i_pContext->Map(cb.m_pBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
		{
			memcpy(mapped.pData, cb.m_Data.data(), cb.m_Size);
			i_pContext->Unmap(cb.m_pBuffer.Get(), 0);
			cb.m_bDirty = false;
		}
	}

	for (int s = 0; s < fx_NumGraphicsStages; s++)
	{
		const Shader* shader = pipeline.m_Stages[s];
		set_shader(i_pContext, s, shader ? shader->m_pShader.Get() : NULL);
		if (!shader)
			continue;
		for (const Binding& b : shader->m_ConstantBuffers)
			set_constant_buffer(i_pContext, s, b.m_Slot, m_ConstantBuffers[b.m_Index].m_pBuffer.Get());
		for (const Binding& b : shader->m_Resources)
			set_resource(i_pContext, s, b.m_Slot, m_Resources[b.m_Index].Get());
		for (const Binding& b : shader->m_Samplers)
			set_sampler(i_pContext, s, b.m_Slot, m_Samplers[b.m_Index].Get());
		// pixel-shader UAVs share slots with the render targets, which stay bound
		if (s == fx_PS)
			for (const Binding& b : shader->m_UnorderedAccess)
			{
				ID3D11UnorderedAccessView* uav = m_UnorderedAccess[b.m_Index].Get();
				i_pContext->OMSetRenderTargetsAndUnorderedAccessViews(
					D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, NULL, NULL,
					b.m_Slot, 1, &uav, NULL);
			}
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
fxBytecode fxEffectDX11::GetVertexShaderBytecode(int i_Technique, int i_Pass) const
{
	if (i_Pass < 0 || i_Pass >= GetPassCount(i_Technique))
		return fxBytecode();
	const Shader* vs = m_Pipelines[i_Technique][i_Pass].m_Stages[fx_VS];
	return vs ? vs->m_Bytecode : fxBytecode();
}
