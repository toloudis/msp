/*****************************************************************************
**  fxEffectApi.hpp
**
**      An Effects-shaped interface over a loaded plain-HLSL effect
**      (fxEffectDX11, through effPlainEffect), used by the material shader
**      classes and the renderers that drive them. It was introduced so they
**      could move off D3DX11 Effects (FX11, now removed) one shader at a time.
**
**      The names and call shapes follow the old ID3DX11Effect interfaces, so
**      code written against Effects ported by changing types only:
**
**          pEffect->GetVariableByName("g_Foo")->AsScalar()->SetFloat(1.0f);
**          pEffect->GetTechniqueByName("Default")->GetPassByIndex(0)->Apply(0, ctx);
**
**      As in Effects, a lookup never returns null: a missing variable,
**      technique or pass is an object whose IsValid() is false and whose
**      setters do nothing. Objects are owned by the effect.
**
**      Now that every shader is converted, this interface can be narrowed to
**      what the renderers actually use.
\****************************************************************************/

#ifdef FX_EFFECTAPI_HPP
#error fxEffectApi.hpp multiply included
#endif
#define FX_EFFECTAPI_HPP

#include <d3d11.h>
#include <d3dcommon.h>

class fxEffectVariable;

//============================================================================
// Descriptions, with the field names of the matching D3DX11_*_DESC structs.
// Strings stay valid for the lifetime of the effect.
//============================================================================
struct fxEffectDesc_
{
	UINT GlobalVariables = 0;
	UINT Techniques = 0;
};

struct fxEffectVariableDesc
{
	LPCSTR Name = nullptr;
	LPCSTR Semantic = nullptr;		// null when the variable has none
	UINT Annotations = 0;
};

struct fxEffectTypeDesc
{
	LPCSTR TypeName = nullptr;
	D3D_SHADER_VARIABLE_CLASS Class = D3D_SVC_SCALAR;
	D3D_SHADER_VARIABLE_TYPE Type = D3D_SVT_VOID;
	UINT Elements = 0;				// 0 if not an array
	UINT Members = 0;				// struct members
	UINT Rows = 0;
	UINT Columns = 0;
};

struct fxEffectTechniqueDesc
{
	LPCSTR Name = nullptr;
	UINT Passes = 0;
};

struct fxEffectPassDesc
{
	LPCSTR Name = nullptr;
	const BYTE* pIAInputSignature = nullptr;	// vertex shader bytecode
	SIZE_T IAInputSignatureSize = 0;
};

//============================================================================
//============================================================================
class fxEffectType
{
public:
	virtual HRESULT GetDesc(fxEffectTypeDesc* o_pDesc) = 0;
protected:
	virtual ~fxEffectType() {}
};

//============================================================================
// One variable, struct member, array element or annotation. The As*()
// casts return the same object, as every setter is available on it; a
// setter that does not fit the variable's type does nothing.
//============================================================================
class fxEffectVariable
{
public:
	virtual bool IsValid() = 0;

	fxEffectVariable* AsScalar() { return this; }
	fxEffectVariable* AsVector() { return this; }
	fxEffectVariable* AsMatrix() { return this; }
	fxEffectVariable* AsString() { return this; }
	fxEffectVariable* AsShaderResource() { return this; }
	fxEffectVariable* AsUnorderedAccessView() { return this; }

	virtual HRESULT GetDesc(fxEffectVariableDesc* o_pDesc) = 0;
	virtual fxEffectType* GetType() = 0;
	virtual fxEffectVariable* GetAnnotationByName(LPCSTR i_Name) = 0;
	virtual fxEffectVariable* GetMemberByName(LPCSTR i_Name) = 0;
	virtual fxEffectVariable* GetElement(UINT i_Index) = 0;

	// Bytes copied into the variable's constant buffer storage, from
	// i_ByteOffset into the variable, as Effects' SetRawValue.
	virtual HRESULT SetRawValue(const void* i_pData, UINT i_ByteOffset, UINT i_ByteCount) = 0;

	// Scalars convert between float, int and bool as Effects did.
	virtual HRESULT SetFloat(float i_Value) = 0;
	virtual HRESULT GetFloat(float* o_pValue) = 0;
	virtual HRESULT SetInt(int i_Value) = 0;
	virtual HRESULT GetInt(int* o_pValue) = 0;
	virtual HRESULT SetBool(bool i_Value) = 0;
	virtual HRESULT GetBool(bool* o_pValue) = 0;

	// As many floats as the vector has components.
	virtual HRESULT SetFloatVector(const float* i_pValue) = 0;
	virtual HRESULT GetFloatVector(float* o_pValue) = 0;
	// Packed source (a float2[] as 2 floats per element).
	virtual HRESULT SetFloatVectorArray(const float* i_pValues, UINT i_Offset, UINT i_Count) = 0;

	// Row-major 4x4 source, as Effects' SetMatrix.
	virtual HRESULT SetMatrix(const float* i_pValue) = 0;
	virtual HRESULT SetMatrixArray(const float* i_pValues, UINT i_Offset, UINT i_Count) = 0;

	virtual HRESULT GetString(LPCSTR* o_pValue) = 0;

	virtual HRESULT SetResource(ID3D11ShaderResourceView* i_pView) = 0;
	virtual HRESULT GetResource(ID3D11ShaderResourceView** o_ppView) = 0;
	virtual HRESULT SetUnorderedAccessView(ID3D11UnorderedAccessView* i_pView) = 0;

protected:
	virtual ~fxEffectVariable() {}
};

//============================================================================
//============================================================================
class fxEffectPass
{
public:
	virtual bool IsValid() = 0;
	virtual HRESULT GetDesc(fxEffectPassDesc* o_pDesc) = 0;
	// Binds the pass's shaders, constant buffers, textures and samplers.
	// i_Flags is unused, as in Effects.
	virtual HRESULT Apply(UINT i_Flags, ID3D11DeviceContext* i_pContext) = 0;
protected:
	virtual ~fxEffectPass() {}
};

//============================================================================
//============================================================================
class fxEffectTechnique
{
public:
	virtual bool IsValid() = 0;
	virtual HRESULT GetDesc(fxEffectTechniqueDesc* o_pDesc) = 0;
	virtual fxEffectPass* GetPassByIndex(UINT i_Index) = 0;
	virtual fxEffectPass* GetPassByName(LPCSTR i_Name) = 0;
protected:
	virtual ~fxEffectTechnique() {}
};

//============================================================================
//============================================================================
class fxEffect
{
public:
	virtual ~fxEffect() {}

	virtual HRESULT GetDesc(fxEffectDesc_* o_pDesc) = 0;

	virtual fxEffectVariable* GetVariableByIndex(UINT i_Index) = 0;
	virtual fxEffectVariable* GetVariableByName(LPCSTR i_Name) = 0;
	virtual fxEffectVariable* GetVariableBySemantic(LPCSTR i_Semantic) = 0;

	virtual fxEffectTechnique* GetTechniqueByIndex(UINT i_Index) = 0;
	virtual fxEffectTechnique* GetTechniqueByName(LPCSTR i_Name) = 0;
};
