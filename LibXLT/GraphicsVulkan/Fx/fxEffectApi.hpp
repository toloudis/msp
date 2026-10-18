/*****************************************************************************
**  fxEffectApi.hpp
**
**      The Effects-shaped interface over a loaded plain-HLSL effect that
**      GraphicsDX11's material shader classes and renderers drive
**      (GraphicsDX11/Fx/fxEffectApi.hpp), for GraphicsVulkan: the same
**      classes, methods and names, in namespace gvk, with D3D types
**      replaced so code ports by changing types only:
**
**      - HRESULT -> bool (true where D3D code tested SUCCEEDED)
**      - LPCSTR -> const char*, UINT -> uint32_t, BYTE/SIZE_T dropped
**      - D3D_SHADER_VARIABLE_CLASS / _TYPE -> fxVarClass / fxVarType (same values)
**      - ID3D11ShaderResourceView* / ID3D11UnorderedAccessView* -> vkdImage*
**      - fxEffectPass::Apply(flags) binds through vkdGlobal::Context()
**      - fxEffectPassDesc has no input signature: Vulkan matches vertex
**        inputs to the layout by semantic when the pipeline is created
**
**      As in Effects, a lookup never returns null: a missing variable,
**      technique or pass is an object whose IsValid() is false and whose
**      setters do nothing. Objects are owned by the effect.
\****************************************************************************/
#ifdef GVK_FXEFFECTAPI_HPP
#error fxEffectApi.hpp multiply included
#endif
#define GVK_FXEFFECTAPI_HPP

#ifndef GVK_VKDREFLECT_HPP
#include "GraphicsVulkan/vkd/vkdReflect.hpp"
#endif

#include <cstdint>

namespace gvk
{
class vkdImage;
class fxEffectVariable;

//============================================================================
// Descriptions, with the field names of the matching D3DX11_*_DESC structs.
// Strings stay valid for the lifetime of the effect.
//============================================================================
struct fxEffectDesc_
{
	uint32_t GlobalVariables = 0;
	uint32_t Techniques = 0;
};

struct fxEffectVariableDesc
{
	const char* Name = nullptr;
	const char* Semantic = nullptr;		// null when the variable has none
	uint32_t Annotations = 0;
};

struct fxEffectTypeDesc
{
	const char* TypeName = nullptr;
	fxVarClass Class = fx_SVC_SCALAR;
	fxVarType Type = fx_SVT_VOID;
	uint32_t Elements = 0;				// 0 if not an array
	uint32_t Members = 0;				// struct members
	uint32_t Rows = 0;
	uint32_t Columns = 0;
};

struct fxEffectTechniqueDesc
{
	const char* Name = nullptr;
	uint32_t Passes = 0;
};

struct fxEffectPassDesc
{
	const char* Name = nullptr;
};

//============================================================================
//============================================================================
class fxEffectType
{
public:
	virtual bool GetDesc(fxEffectTypeDesc* o_pDesc) = 0;
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

	virtual bool GetDesc(fxEffectVariableDesc* o_pDesc) = 0;
	virtual fxEffectType* GetType() = 0;
	virtual fxEffectVariable* GetAnnotationByName(const char* i_Name) = 0;
	virtual fxEffectVariable* GetMemberByName(const char* i_Name) = 0;
	virtual fxEffectVariable* GetElement(uint32_t i_Index) = 0;

	// Bytes copied into the variable's constant buffer storage, from
	// i_ByteOffset into the variable, as Effects' SetRawValue.
	virtual bool SetRawValue(const void* i_pData, uint32_t i_ByteOffset, uint32_t i_ByteCount) = 0;

	// Scalars convert between float, int and bool as Effects did.
	virtual bool SetFloat(float i_Value) = 0;
	virtual bool GetFloat(float* o_pValue) = 0;
	virtual bool SetInt(int i_Value) = 0;
	virtual bool GetInt(int* o_pValue) = 0;
	virtual bool SetBool(bool i_Value) = 0;
	virtual bool GetBool(bool* o_pValue) = 0;

	// As many floats as the vector has components.
	virtual bool SetFloatVector(const float* i_pValue) = 0;
	virtual bool GetFloatVector(float* o_pValue) = 0;
	// Packed source (a float2[] as 2 floats per element).
	virtual bool SetFloatVectorArray(const float* i_pValues, uint32_t i_Offset, uint32_t i_Count) = 0;

	// Row-major 4x4 source, as Effects' SetMatrix.
	virtual bool SetMatrix(const float* i_pValue) = 0;
	virtual bool SetMatrixArray(const float* i_pValues, uint32_t i_Offset, uint32_t i_Count) = 0;

	virtual bool GetString(const char** o_pValue) = 0;

	virtual bool SetResource(vkdImage* i_pImage) = 0;
	virtual bool GetResource(vkdImage** o_ppImage) = 0;
	virtual bool SetUnorderedAccessView(vkdImage* i_pImage) = 0;

protected:
	virtual ~fxEffectVariable() {}
};

//============================================================================
//============================================================================
class fxEffectPass
{
public:
	virtual bool IsValid() = 0;
	virtual bool GetDesc(fxEffectPassDesc* o_pDesc) = 0;
	// Binds the pass's shaders, constant buffers, textures and samplers.
	// i_Flags is unused, as in Effects.
	virtual bool Apply(uint32_t i_Flags) = 0;
protected:
	virtual ~fxEffectPass() {}
};

//============================================================================
//============================================================================
class fxEffectTechnique
{
public:
	virtual bool IsValid() = 0;
	virtual bool GetDesc(fxEffectTechniqueDesc* o_pDesc) = 0;
	virtual fxEffectPass* GetPassByIndex(uint32_t i_Index) = 0;
	virtual fxEffectPass* GetPassByName(const char* i_Name) = 0;
protected:
	virtual ~fxEffectTechnique() {}
};

//============================================================================
//============================================================================
class fxEffect
{
public:
	virtual ~fxEffect() {}

	virtual bool GetDesc(fxEffectDesc_* o_pDesc) = 0;

	virtual fxEffectVariable* GetVariableByIndex(uint32_t i_Index) = 0;
	virtual fxEffectVariable* GetVariableByName(const char* i_Name) = 0;
	virtual fxEffectVariable* GetVariableBySemantic(const char* i_Semantic) = 0;

	virtual fxEffectTechnique* GetTechniqueByIndex(uint32_t i_Index) = 0;
	virtual fxEffectTechnique* GetTechniqueByName(const char* i_Name) = 0;
};
}
