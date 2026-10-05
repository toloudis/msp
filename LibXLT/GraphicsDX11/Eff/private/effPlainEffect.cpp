/*****************************************************************************
**  effPlainEffect.cpp
**
**      fxEffect on top of a plain-HLSL effect. See effPlainEffect.hpp.
\****************************************************************************/

#include "GraphicsDX11/eff/effPlainEffect.hpp"
#include "GraphicsDX11/Fx/fxEffectDX11.hpp"

#include <wrl/client.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string.h>

namespace
{
	typedef fxEffectDX11::Type TypeInfo;

	//--------------------------------------------------------------------
	// Types the manifest spells out ("float4", "int", "Texture2D", a struct
	// name), for variables and annotations reflection knows nothing about.
	//--------------------------------------------------------------------
	struct NamedType
	{
		const char* m_Name;
		D3D_SHADER_VARIABLE_TYPE m_Type;
	};

	const NamedType k_Scalars[] =
	{
		{ "float", D3D_SVT_FLOAT },
		{ "half", D3D_SVT_FLOAT },
		{ "int", D3D_SVT_INT },
		{ "uint", D3D_SVT_UINT },
		{ "dword", D3D_SVT_UINT },
		{ "bool", D3D_SVT_BOOL },
		{ "double", D3D_SVT_DOUBLE },
	};

	const NamedType k_Objects[] =
	{
		{ "string", D3D_SVT_STRING },
		{ "texture", D3D_SVT_TEXTURE },
		{ "Texture1D", D3D_SVT_TEXTURE1D },
		{ "Texture1DArray", D3D_SVT_TEXTURE1DARRAY },
		{ "Texture2D", D3D_SVT_TEXTURE2D },
		{ "Texture2DArray", D3D_SVT_TEXTURE2DARRAY },
		{ "Texture2DMS", D3D_SVT_TEXTURE2DMS },
		{ "Texture2DMSArray", D3D_SVT_TEXTURE2DMSARRAY },
		{ "Texture3D", D3D_SVT_TEXTURE3D },
		{ "TextureCube", D3D_SVT_TEXTURECUBE },
		{ "TextureCubeArray", D3D_SVT_TEXTURECUBEARRAY },
		{ "Buffer", D3D_SVT_BUFFER },
		{ "StructuredBuffer", D3D_SVT_STRUCTURED_BUFFER },
		{ "ByteAddressBuffer", D3D_SVT_BYTEADDRESS_BUFFER },
		{ "SamplerState", D3D_SVT_SAMPLER },
		{ "SamplerComparisonState", D3D_SVT_SAMPLER },
	};

	TypeInfo parse_type(const std::string& i_Name)
	{
		TypeInfo type;
		type.m_Name = i_Name;

		// strip a template argument: Texture2D<float4>
		std::string name = i_Name.substr(0, i_Name.find('<'));
		for (const NamedType& t : k_Objects)
		{
			if (_stricmp(t.m_Name, name.c_str()) == 0)
			{
				type.m_Class = D3D_SVC_OBJECT;
				type.m_Type = t.m_Type;
				return type;
			}
		}
		for (const NamedType& t : k_Scalars)
		{
			size_t len = strlen(t.m_Name);
			if (name.compare(0, len, t.m_Name) != 0)
				continue;
			std::string dims = name.substr(len);
			int rows = 1, columns = 1;
			if (dims.empty())
				type.m_Class = D3D_SVC_SCALAR;
			else if (dims.size() == 1 && isdigit((unsigned char)dims[0]))
			{
				columns = dims[0] - '0';
				type.m_Class = D3D_SVC_VECTOR;
			}
			else if (dims.size() == 3 && dims[1] == 'x')
			{
				rows = dims[0] - '0';
				columns = dims[2] - '0';
				type.m_Class = D3D_SVC_MATRIX_COLUMNS;	// the HLSL default
			}
			else
				continue;
			type.m_Type = t.m_Type;
			type.m_Rows = (uint32_t)rows;
			type.m_Columns = (uint32_t)columns;
			return type;
		}
		// a struct; its members are known only from reflection
		type.m_Class = D3D_SVC_STRUCT;
		type.m_Type = D3D_SVT_VOID;
		type.m_Rows = 1;
		return type;
	}

	bool is_resource(const TypeInfo& i_Type)
	{
		return i_Type.m_Class == D3D_SVC_OBJECT &&
			i_Type.m_Type != D3D_SVT_STRING && i_Type.m_Type != D3D_SVT_SAMPLER;
	}

	uint32_t register_stride(const TypeInfo& i_Type)
	{
		return (i_Type.GetElementSize() + 15) & ~15u;
	}
}

//============================================================================
//============================================================================
class effPlainEffect::Type : public fxEffectType
{
public:
	explicit Type(const TypeInfo& i_Info) : m_Info(i_Info) {}

	virtual HRESULT GetDesc(fxEffectTypeDesc* o_pDesc)
	{
		o_pDesc->TypeName = m_Info.m_Name.c_str();
		o_pDesc->Class = (D3D_SHADER_VARIABLE_CLASS)m_Info.m_Class;
		o_pDesc->Type = (D3D_SHADER_VARIABLE_TYPE)m_Info.m_Type;
		o_pDesc->Elements = m_Info.m_Elements;
		o_pDesc->Members = (UINT)m_Info.m_Members.size();
		o_pDesc->Rows = m_Info.m_Rows;
		o_pDesc->Columns = m_Info.m_Columns;
		return S_OK;
	}

private:
	const TypeInfo& m_Info;
};

//============================================================================
// A global variable, a struct member or array element of one, or an
// annotation. Values live in one of:
//	- the effect's constant buffer storage (m_Constant), for constants some
//	  entry point reads;
//	- the effect's resource table (m_Resource), for textures it reads;
//	- m_Shadow / m_pShadowView, for variables no entry point reads;
//	- the manifest, read-only, for annotations.
//============================================================================
class effPlainEffect::Variable : public fxEffectVariable
{
public:
	enum Kind { e_Invalid, e_Constant, e_Resource, e_Shadow, e_Annotation };

	Variable(effPlainEffect* i_pOwner, Kind i_Kind, const TypeInfo& i_Type)
	:	m_pOwner(i_pOwner), m_Kind(i_Kind), m_TypeInfo(i_Type), m_Type(m_TypeInfo)
	{
	}

	static std::unique_ptr<Variable> MakeGlobal(effPlainEffect* i_pOwner, const fxVariableDesc& i_Desc,
		const std::string& i_Array);
	static std::unique_ptr<Variable> MakeReflected(effPlainEffect* i_pOwner, const std::string& i_Name);

	fxEffectDX11* Effect() { return m_pOwner->GetEffectDX11(); }
	const std::string& GetName() const { return m_Name; }
	const std::string& GetSemantic() const { return m_Semantic; }

	//--------------------------------------------------------------------
	virtual bool IsValid() { return m_Kind != e_Invalid; }

	virtual HRESULT GetDesc(fxEffectVariableDesc* o_pDesc)
	{
		if (!IsValid())
			return E_FAIL;
		o_pDesc->Name = m_Name.c_str();
		o_pDesc->Semantic = m_Semantic.empty() ? nullptr : m_Semantic.c_str();
		o_pDesc->Annotations = m_pAnnotations ? (UINT)m_pAnnotations->size() : 0;
		return S_OK;
	}

	virtual fxEffectType* GetType() { return &m_Type; }

	virtual fxEffectVariable* GetAnnotationByName(LPCSTR i_Name)
	{
		if (!m_pAnnotations || !i_Name)
			return m_pOwner->GetInvalidVariable();
		auto found = m_Children.find(std::string("@") + i_Name);
		if (found != m_Children.end())
			return found->second.get();
		for (const fxAnnotationDesc& a : *m_pAnnotations)
		{
			if (_stricmp(a.m_Name.c_str(), i_Name) != 0)	// Effects matches annotations case-insensitively
				continue;
			std::unique_ptr<Variable> v(new Variable(m_pOwner, e_Annotation, parse_type(a.m_Type)));
			v->m_Name = a.m_Name;
			v->m_pAnnotation = &a;
			Variable* result = v.get();
			m_Children[std::string("@") + i_Name] = std::move(v);
			return result;
		}
		return m_pOwner->GetInvalidVariable();
	}

	virtual fxEffectVariable* GetMemberByName(LPCSTR i_Name)
	{
		if (m_Kind != e_Constant || !i_Name || m_TypeInfo.m_Elements > 0)
			return m_pOwner->GetInvalidVariable();
		auto found = m_Children.find(i_Name);
		if (found != m_Children.end())
			return found->second.get();
		for (const TypeInfo::Member& m : m_TypeInfo.m_Members)
		{
			if (m.m_Name != i_Name)
				continue;
			std::unique_ptr<Variable> v(new Variable(m_pOwner, e_Constant, m.m_Type));
			v->m_Name = m.m_Name;
			v->m_Constant = sub_constant(m.m_Offset, m.m_Type);
			Variable* result = v.get();
			m_Children[i_Name] = std::move(v);
			return result;
		}
		return m_pOwner->GetInvalidVariable();
	}

	virtual fxEffectVariable* GetElement(UINT i_Index)
	{
		if (m_Kind != e_Constant || i_Index >= m_TypeInfo.m_Elements)
			return m_pOwner->GetInvalidVariable();
		std::string key = "[" + std::to_string(i_Index) + "]";
		auto found = m_Children.find(key);
		if (found != m_Children.end())
			return found->second.get();
		TypeInfo element = m_TypeInfo;
		element.m_Elements = 0;
		std::unique_ptr<Variable> v(new Variable(m_pOwner, e_Constant, element));
		v->m_Name = m_Name + key;
		v->m_Constant = sub_constant(i_Index * register_stride(element), element);
		Variable* result = v.get();
		m_Children[key] = std::move(v);
		return result;
	}

	//--------------------------------------------------------------------
	virtual HRESULT SetRawValue(const void* i_pData, UINT i_ByteOffset, UINT i_ByteCount)
	{
		return write(i_ByteOffset, i_pData, i_ByteCount);
	}

	virtual HRESULT SetFloat(float i_Value) { double v = i_Value; return set_components(&v, 1); }
	virtual HRESULT SetInt(int i_Value) { double v = i_Value; return set_components(&v, 1); }
	virtual HRESULT SetBool(bool i_Value) { double v = i_Value ? 1.0 : 0.0; return set_components(&v, 1); }

	virtual HRESULT GetFloat(float* o_pValue)
	{
		double v;
		HRESULT hr = get_components(&v, 1);
		if (SUCCEEDED(hr))
			*o_pValue = (float)v;
		return hr;
	}
	virtual HRESULT GetInt(int* o_pValue)
	{
		double v;
		HRESULT hr = get_components(&v, 1);
		if (SUCCEEDED(hr))
			*o_pValue = (int)v;
		return hr;
	}
	virtual HRESULT GetBool(bool* o_pValue)
	{
		double v;
		HRESULT hr = get_components(&v, 1);
		if (SUCCEEDED(hr))
			*o_pValue = (v != 0.0);
		return hr;
	}

	virtual HRESULT SetFloatVector(const float* i_pValue)
	{
		double v[4];
		UINT n = vector_size();
		for (UINT i = 0; i < n; i++)
			v[i] = i_pValue[i];
		return set_components(v, n);
	}
	virtual HRESULT GetFloatVector(float* o_pValue)
	{
		double v[4];
		UINT n = vector_size();
		HRESULT hr = get_components(v, n);
		if (SUCCEEDED(hr))
			for (UINT i = 0; i < n; i++)
				o_pValue[i] = (float)v[i];
		return hr;
	}

	// Packed source; each element starts on its own 16-byte register.
	virtual HRESULT SetFloatVectorArray(const float* i_pValues, UINT i_Offset, UINT i_Count)
	{
		if (m_TypeInfo.m_Elements == 0)
			return E_INVALIDARG;
		TypeInfo element = m_TypeInfo;
		element.m_Elements = 0;
		const UINT columns = vector_size();
		const uint32_t stride = register_stride(element);
		for (UINT i = 0; i < i_Count && i_Offset + i < m_TypeInfo.m_Elements; i++)
		{
			HRESULT hr = store(i_Offset * stride + i * stride, i_pValues + i * columns, columns);
			if (FAILED(hr))
				return hr;
		}
		return S_OK;
	}

	// A row-major 4x4 source, as Effects took it.
	virtual HRESULT SetMatrix(const float* i_pValue)
	{
		return set_matrix(0, i_pValue);
	}
	virtual HRESULT SetMatrixArray(const float* i_pValues, UINT i_Offset, UINT i_Count)
	{
		if (m_TypeInfo.m_Elements == 0)
			return E_INVALIDARG;
		TypeInfo element = m_TypeInfo;
		element.m_Elements = 0;
		const uint32_t stride = register_stride(element);
		for (UINT i = 0; i < i_Count && i_Offset + i < m_TypeInfo.m_Elements; i++)
		{
			HRESULT hr = set_matrix((i_Offset + i) * stride, i_pValues + i * 16);
			if (FAILED(hr))
				return hr;
		}
		return S_OK;
	}

	virtual HRESULT GetString(LPCSTR* o_pValue)
	{
		if (m_Kind != e_Annotation || m_TypeInfo.m_Type != D3D_SVT_STRING)
			return E_FAIL;
		*o_pValue = m_pAnnotation->m_String.c_str();
		return S_OK;
	}

	virtual HRESULT SetResource(ID3D11ShaderResourceView* i_pView)
	{
		if (m_Kind == e_Resource)
			Effect()->SetResource(m_Resource, i_pView);
		else if (m_Kind == e_Shadow && is_resource(m_TypeInfo))
			m_pShadowView = i_pView;
		else
			return E_FAIL;
		return S_OK;
	}
	virtual HRESULT GetResource(ID3D11ShaderResourceView** o_ppView)
	{
		ID3D11ShaderResourceView* view = nullptr;
		if (m_Kind == e_Resource)
			view = Effect()->GetResource(m_Resource);
		else if (m_Kind == e_Shadow && is_resource(m_TypeInfo))
			view = m_pShadowView.Get();
		else
			return E_FAIL;
		if (view)
			view->AddRef();		// as Effects did
		*o_ppView = view;
		return S_OK;
	}
	virtual HRESULT SetUnorderedAccessView(ID3D11UnorderedAccessView*)
	{
		return E_NOTIMPL;	// no material reads a UAV
	}

private:
	//--------------------------------------------------------------------
	// Storage
	//--------------------------------------------------------------------
	fxEffectDX11::Constant sub_constant(uint32_t i_Offset, const TypeInfo& i_Type) const
	{
		fxEffectDX11::Constant c = m_Constant;
		c.m_Offset += i_Offset;
		uint32_t size = i_Type.GetElementSize();
		if (i_Type.m_Elements > 0)
			size += (i_Type.m_Elements - 1) * register_stride(i_Type);
		c.m_Size = (i_Offset < m_Constant.m_Size) ? (std::min)(size, m_Constant.m_Size - i_Offset) : 0;
		c.m_bColumnMajor = (i_Type.m_Class == D3D_SVC_MATRIX_COLUMNS);
		c.m_Columns = (uint16_t)i_Type.m_Columns;
		c.m_Elements = (uint16_t)i_Type.m_Elements;
		return c;
	}

	uint32_t storage_size() const
	{
		if (m_Kind == e_Constant)
			return m_Constant.m_Size;
		if (m_Kind == e_Shadow)
			return (uint32_t)m_Shadow.size();
		return 0;
	}

	HRESULT write(uint32_t i_Offset, const void* i_pData, uint32_t i_Bytes)
	{
		if (m_Kind == e_Shadow && !is_resource(m_TypeInfo))
		{
			// a struct whose layout only reflection knew grows on demand
			if (i_Offset + i_Bytes > m_Shadow.size())
				m_Shadow.resize(i_Offset + i_Bytes, 0);
			memcpy(&m_Shadow[i_Offset], i_pData, i_Bytes);
			return S_OK;
		}
		if (m_Kind != e_Constant || i_Offset + i_Bytes > storage_size())
			return E_INVALIDARG;
		fxEffectDX11::Constant c = m_Constant;
		c.m_Offset += i_Offset;
		c.m_Size = i_Bytes;
		Effect()->SetConstant(c, i_pData, i_Bytes);
		return S_OK;
	}

	HRESULT read(uint32_t i_Offset, void* o_pData, uint32_t i_Bytes)
	{
		if (i_Offset + i_Bytes > storage_size())
			return E_INVALIDARG;
		if (m_Kind == e_Shadow)
		{
			memcpy(o_pData, &m_Shadow[i_Offset], i_Bytes);
			return S_OK;
		}
		fxEffectDX11::Constant c = m_Constant;
		c.m_Offset += i_Offset;
		c.m_Size = i_Bytes;
		Effect()->GetConstant(c, o_pData, i_Bytes);
		return S_OK;
	}

	//--------------------------------------------------------------------
	// Components, converted to the variable's scalar type as Effects did
	//--------------------------------------------------------------------
	UINT vector_size() const
	{
		UINT n = m_TypeInfo.m_Columns;
		return (n == 0) ? 1 : (n > 4 ? 4 : n);
	}

	HRESULT store(uint32_t i_Offset, const float* i_pValues, UINT i_Count)
	{
		double v[4];
		for (UINT i = 0; i < i_Count && i < 4; i++)
			v[i] = i_pValues[i];
		return store(i_Offset, v, i_Count);
	}

	HRESULT store(uint32_t i_Offset, const double* i_pValues, UINT i_Count)
	{
		uint32_t bits[4];
		for (UINT i = 0; i < i_Count && i < 4; i++)
		{
			switch (m_TypeInfo.m_Type)
			{
			case D3D_SVT_INT:	{ int32_t x = (int32_t)i_pValues[i]; memcpy(&bits[i], &x, 4); } break;
			case D3D_SVT_UINT:	bits[i] = (uint32_t)i_pValues[i]; break;
			case D3D_SVT_BOOL:	bits[i] = (i_pValues[i] != 0.0) ? 1u : 0u; break;
			default:			{ float x = (float)i_pValues[i]; memcpy(&bits[i], &x, 4); } break;
			}
		}
		return write(i_Offset, bits, i_Count * 4);
	}

	HRESULT set_components(const double* i_pValues, UINT i_Count)
	{
		if (m_Kind == e_Annotation || m_Kind == e_Invalid || m_Kind == e_Resource || is_resource(m_TypeInfo))
			return E_FAIL;
		uint32_t room = storage_size() / 4;
		if (m_Kind == e_Shadow && room == 0)
		{
			m_Shadow.resize(16, 0);
			room = 4;
		}
		return store(0, i_pValues, (i_Count < room) ? i_Count : room);
	}

	HRESULT get_components(double* o_pValues, UINT i_Count)
	{
		if (m_Kind == e_Annotation)
		{
			for (UINT i = 0; i < i_Count; i++)
				o_pValues[i] = (i < m_pAnnotation->m_Values.size()) ? m_pAnnotation->m_Values[i] : 0.0;
			return m_pAnnotation->m_Values.empty() ? E_FAIL : S_OK;
		}
		if (m_Kind != e_Constant && m_Kind != e_Shadow)
			return E_FAIL;
		uint32_t bits[4] = {};
		UINT n = (i_Count < 4) ? i_Count : 4;
		uint32_t bytes = (std::min<uint32_t>)(n * 4, storage_size());
		HRESULT hr = read(0, bits, bytes);
		if (FAILED(hr))
			return hr;
		for (UINT i = 0; i < n; i++)
		{
			switch (m_TypeInfo.m_Type)
			{
			case D3D_SVT_INT:	{ int32_t x; memcpy(&x, &bits[i], 4); o_pValues[i] = x; } break;
			case D3D_SVT_UINT:	o_pValues[i] = bits[i]; break;
			case D3D_SVT_BOOL:	o_pValues[i] = bits[i] ? 1.0 : 0.0; break;
			default:			{ float x; memcpy(&x, &bits[i], 4); o_pValues[i] = x; } break;
			}
		}
		return S_OK;
	}

	// One matrix at i_Offset from a row-major 4x4 source. A row_major
	// matrix takes a 16-byte register per row, a column_major one per column.
	HRESULT set_matrix(uint32_t i_Offset, const float* i_pRowMajor4x4)
	{
		const UINT rows = m_TypeInfo.m_Rows, columns = m_TypeInfo.m_Columns;
		if (rows == 0 || columns == 0 || rows > 4 || columns > 4)
			return E_INVALIDARG;
		const bool byColumn = (m_TypeInfo.m_Class == D3D_SVC_MATRIX_COLUMNS);
		const UINT registers = byColumn ? columns : rows;
		const UINT perRegister = byColumn ? rows : columns;
		for (UINT r = 0; r < registers; r++)
		{
			float v[4];
			for (UINT i = 0; i < perRegister; i++)
				v[i] = byColumn ? i_pRowMajor4x4[i * 4 + r] : i_pRowMajor4x4[r * 4 + i];
			HRESULT hr = write(i_Offset + r * 16, v, perRegister * 4);
			if (FAILED(hr))
				return hr;
		}
		return S_OK;
	}

	effPlainEffect* m_pOwner;
	Kind m_Kind;
	TypeInfo m_TypeInfo;
	Type m_Type;
	std::string m_Name;
	std::string m_Semantic;
	const std::vector<fxAnnotationDesc>* m_pAnnotations = nullptr;
	const fxAnnotationDesc* m_pAnnotation = nullptr;
	fxEffectDX11::Constant m_Constant;
	fxEffectDX11::Resource m_Resource;
	std::vector<uint8_t> m_Shadow;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShadowView;
	std::map<std::string, std::unique_ptr<Variable>> m_Children;	// members, elements, @annotations
};

//------------------------------------------------------------------------
// MakeGlobal() - a variable the manifest declares
//------------------------------------------------------------------------
std::unique_ptr<effPlainEffect::Variable> effPlainEffect::Variable::MakeGlobal(effPlainEffect* i_pOwner,
	const fxVariableDesc& i_Desc, const std::string& i_Array)
{
	fxEffectDX11* effect = i_pOwner->GetEffectDX11();
	std::unique_ptr<Variable> v;
	if (const TypeInfo* reflected = effect->FindConstantType(i_Desc.m_Name))
	{
		v.reset(new Variable(i_pOwner, e_Constant, *reflected));
		v->m_Constant = effect->FindConstant(i_Desc.m_Name);
	}
	else
	{
		TypeInfo type = parse_type(i_Desc.m_Type);
		if (!i_Array.empty())
			type.m_Elements = (uint32_t)strtoul(i_Array.c_str(), nullptr, 10);
		fxEffectDX11::Resource resource = effect->FindResource(i_Desc.m_Name);
		if (resource.IsValid())
		{
			v.reset(new Variable(i_pOwner, e_Resource, type));
			v->m_Resource = resource;
		}
		else
		{
			// no entry point reads it: keep its value on the CPU, starting
			// from the manifest's default
			v.reset(new Variable(i_pOwner, e_Shadow, type));
			if (!is_resource(type) && type.m_Class != D3D_SVC_OBJECT)
			{
				uint32_t size = type.GetElementSize();
				if (type.m_Elements > 0)
					size += (type.m_Elements - 1) * register_stride(type);
				v->m_Shadow.assign(size, 0);
				if (!i_Desc.m_Default.empty() && type.m_Elements == 0 &&
					(type.m_Class == D3D_SVC_SCALAR || type.m_Class == D3D_SVC_VECTOR))
					v->store(0, i_Desc.m_Default.data(), (UINT)(std::min<size_t>)(i_Desc.m_Default.size(), type.m_Columns));
			}
		}
	}
	v->m_Name = i_Desc.m_Name;
	v->m_Semantic = i_Desc.m_Semantic;
	v->m_pAnnotations = &i_Desc.m_Annotations;
	return v;
}

//------------------------------------------------------------------------
// MakeReflected() - a constant or resource the shaders use that the
//	manifest does not list (added by hand after conversion)
//------------------------------------------------------------------------
std::unique_ptr<effPlainEffect::Variable> effPlainEffect::Variable::MakeReflected(effPlainEffect* i_pOwner,
	const std::string& i_Name)
{
	fxEffectDX11* effect = i_pOwner->GetEffectDX11();
	std::unique_ptr<Variable> v;
	if (const TypeInfo* reflected = effect->FindConstantType(i_Name))
	{
		v.reset(new Variable(i_pOwner, e_Constant, *reflected));
		v->m_Constant = effect->FindConstant(i_Name);
	}
	else
	{
		v.reset(new Variable(i_pOwner, e_Resource, parse_type("texture")));
		v->m_Resource = effect->FindResource(i_Name);
	}
	v->m_Name = i_Name;
	return v;
}

//============================================================================
//============================================================================
class effPlainEffect::Pass : public fxEffectPass
{
public:
	Pass(fxEffectDX11* i_pEffect, int i_Technique, int i_Pass)
	:	m_pEffect(i_pEffect), m_Technique(i_Technique), m_Pass(i_Pass)
	{
	}

	virtual bool IsValid() { return m_pEffect != nullptr; }

	virtual HRESULT GetDesc(fxEffectPassDesc* o_pDesc)
	{
		if (!IsValid())
			return E_FAIL;
		const fxPassDesc& desc = m_pEffect->GetDesc().m_Techniques[m_Technique].m_Passes[m_Pass];
		fxBytecode vs = m_pEffect->GetVertexShaderBytecode(m_Technique, m_Pass);
		o_pDesc->Name = desc.m_Name.c_str();
		o_pDesc->pIAInputSignature = (const BYTE*)vs.m_pData;
		o_pDesc->IAInputSignatureSize = vs.m_Size;
		return S_OK;
	}

	virtual HRESULT Apply(UINT, ID3D11DeviceContext* i_pContext)
	{
		if (!IsValid())
			return E_FAIL;
		m_pEffect->Apply(m_Technique, m_Pass, i_pContext);
		return S_OK;
	}

private:
	fxEffectDX11* m_pEffect;
	int m_Technique;
	int m_Pass;
};

//============================================================================
//============================================================================
class effPlainEffect::Technique : public fxEffectTechnique
{
public:
	Technique(effPlainEffect* i_pOwner, int i_Technique)
	:	m_pOwner(i_pOwner), m_Technique(i_Technique)
	{
		fxEffectDX11* effect = i_pOwner->GetEffectDX11();
		if (effect)
			for (int p = 0; p < effect->GetPassCount(i_Technique); p++)
				m_Passes.emplace_back(new Pass(effect, i_Technique, p));
	}

	virtual bool IsValid() { return m_Technique >= 0; }

	virtual HRESULT GetDesc(fxEffectTechniqueDesc* o_pDesc)
	{
		if (!IsValid())
			return E_FAIL;
		o_pDesc->Name = m_pOwner->GetEffectDX11()->GetDesc().m_Techniques[m_Technique].m_Name.c_str();
		o_pDesc->Passes = (UINT)m_Passes.size();
		return S_OK;
	}

	virtual fxEffectPass* GetPassByIndex(UINT i_Index)
	{
		return (i_Index < m_Passes.size()) ? m_Passes[i_Index].get() : m_pOwner->GetInvalidPass();
	}

	virtual fxEffectPass* GetPassByName(LPCSTR i_Name)
	{
		if (IsValid() && i_Name)
		{
			const fxTechniqueDesc& desc = m_pOwner->GetEffectDX11()->GetDesc().m_Techniques[m_Technique];
			for (size_t p = 0; p < m_Passes.size(); p++)
				if (desc.m_Passes[p].m_Name == i_Name)
					return m_Passes[p].get();
		}
		return m_pOwner->GetInvalidPass();
	}

private:
	effPlainEffect* m_pOwner;
	int m_Technique;
	std::vector<std::unique_ptr<Pass>> m_Passes;
};

//============================================================================
//============================================================================
effPlainEffect::effPlainEffect(std::unique_ptr<fxEffectDX11> i_pEffect)
:	m_pEffect(std::move(i_pEffect))
{
	static const TypeInfo k_Invalid;
	m_pInvalidVariable.reset(new Variable(this, Variable::e_Invalid, k_Invalid));
	m_pInvalidTechnique.reset(new Technique(this, -1));
	m_pInvalidPass.reset(new Pass(nullptr, -1, -1));

	const fxEffectDesc& desc = m_pEffect->GetDesc();

	// the manifest's variables, in declaration order (samplers are
	// immutable state objects, not variables to set)
	for (const fxVariableDesc& var : desc.m_Variables)
	{
		if (var.m_Type == "SamplerState" || var.m_Type == "SamplerComparisonState")
			continue;
		std::unique_ptr<Variable> v = Variable::MakeGlobal(this, var, std::string());
		m_VariableNames[var.m_Name] = v.get();
		m_Variables.push_back(std::move(v));
	}

	// then anything the shaders read that the manifest does not list
	std::vector<std::string> names = m_pEffect->GetConstantNames();
	std::vector<std::string> resources = m_pEffect->GetResourceNames();
	names.insert(names.end(), resources.begin(), resources.end());
	for (const std::string& name : names)
	{
		if (m_VariableNames.count(name))
			continue;
		std::unique_ptr<Variable> v = Variable::MakeReflected(this, name);
		m_VariableNames[name] = v.get();
		m_Variables.push_back(std::move(v));
	}

	for (int t = 0; t < m_pEffect->GetTechniqueCount(); t++)
		m_Techniques.emplace_back(new Technique(this, t));
}

effPlainEffect::~effPlainEffect()
{
}

HRESULT effPlainEffect::GetDesc(fxEffectDesc_* o_pDesc)
{
	o_pDesc->GlobalVariables = (UINT)m_Variables.size();
	o_pDesc->Techniques = (UINT)m_Techniques.size();
	return S_OK;
}

fxEffectVariable* effPlainEffect::GetVariableByIndex(UINT i_Index)
{
	return (i_Index < m_Variables.size()) ? m_Variables[i_Index].get() : m_pInvalidVariable.get();
}

fxEffectVariable* effPlainEffect::GetVariableByName(LPCSTR i_Name)
{
	if (!i_Name)
		return m_pInvalidVariable.get();
	auto found = m_VariableNames.find(i_Name);
	return (found != m_VariableNames.end()) ? found->second : m_pInvalidVariable.get();
}

// Effects matches semantics case-insensitively.
fxEffectVariable* effPlainEffect::GetVariableBySemantic(LPCSTR i_Semantic)
{
	if (i_Semantic)
		for (const std::unique_ptr<Variable>& v : m_Variables)
			if (!v->GetSemantic().empty() && _stricmp(v->GetSemantic().c_str(), i_Semantic) == 0)
				return v.get();
	return m_pInvalidVariable.get();
}

fxEffectTechnique* effPlainEffect::GetTechniqueByIndex(UINT i_Index)
{
	return (i_Index < m_Techniques.size()) ? m_Techniques[i_Index].get() : m_pInvalidTechnique.get();
}

fxEffectTechnique* effPlainEffect::GetTechniqueByName(LPCSTR i_Name)
{
	int t = i_Name ? m_pEffect->FindTechnique(i_Name) : -1;
	return (t >= 0) ? m_Techniques[t].get() : m_pInvalidTechnique.get();
}
