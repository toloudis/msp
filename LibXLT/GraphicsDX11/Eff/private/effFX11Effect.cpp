/*****************************************************************************
**  effFX11Effect.cpp
**
**      fxEffect on top of a D3DX11 Effects effect.
\****************************************************************************/

#include "GraphicsDX11/eff/effFX11Effect.hpp"
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"

//============================================================================
//============================================================================
class effFX11Effect::Type : public fxEffectType
{
public:
	explicit Type(ID3DX11EffectType* i_pType) : m_pType(i_pType) {}

	virtual HRESULT GetDesc(fxEffectTypeDesc* o_pDesc)
	{
		D3DX11_EFFECT_TYPE_DESC desc;
		HRESULT hr = m_pType->GetDesc(&desc);
		if (FAILED(hr))
			return hr;
		o_pDesc->TypeName = desc.TypeName;
		o_pDesc->Class = (D3D_SHADER_VARIABLE_CLASS)desc.Class;
		o_pDesc->Type = (D3D_SHADER_VARIABLE_TYPE)desc.Type;
		o_pDesc->Elements = desc.Elements;
		o_pDesc->Members = desc.Members;
		o_pDesc->Rows = desc.Rows;
		o_pDesc->Columns = desc.Columns;
		return S_OK;
	}

private:
	ID3DX11EffectType* m_pType;
};

//============================================================================
//============================================================================
class effFX11Effect::Variable : public fxEffectVariable
{
public:
	Variable(effFX11Effect* i_pOwner, ID3DX11EffectVariable* i_pVariable)
	:	m_pOwner(i_pOwner), m_pVar(i_pVariable) {}

	virtual bool IsValid() { return m_pVar->IsValid() != FALSE; }

	virtual HRESULT GetDesc(fxEffectVariableDesc* o_pDesc)
	{
		D3DX11_EFFECT_VARIABLE_DESC desc;
		HRESULT hr = m_pVar->GetDesc(&desc);
		if (FAILED(hr))
			return hr;
		o_pDesc->Name = desc.Name;
		o_pDesc->Semantic = desc.Semantic;
		o_pDesc->Annotations = desc.Annotations;
		return S_OK;
	}
	virtual fxEffectType* GetType() { return m_pOwner->Wrap(m_pVar->GetType()); }
	virtual fxEffectVariable* GetAnnotationByName(LPCSTR i_Name) { return m_pOwner->Wrap(m_pVar->GetAnnotationByName(i_Name)); }
	virtual fxEffectVariable* GetMemberByName(LPCSTR i_Name) { return m_pOwner->Wrap(m_pVar->GetMemberByName(i_Name)); }
	virtual fxEffectVariable* GetElement(UINT i_Index) { return m_pOwner->Wrap(m_pVar->GetElement(i_Index)); }

	virtual HRESULT SetRawValue(const void* i_pData, UINT i_ByteOffset, UINT i_ByteCount)
	{
		return m_pVar->SetRawValue(i_pData, i_ByteOffset, i_ByteCount);
	}

	virtual HRESULT SetFloat(float i_Value) { return m_pVar->AsScalar()->SetFloat(i_Value); }
	virtual HRESULT GetFloat(float* o_pValue) { return m_pVar->AsScalar()->GetFloat(o_pValue); }
	virtual HRESULT SetInt(int i_Value) { return m_pVar->AsScalar()->SetInt(i_Value); }
	virtual HRESULT GetInt(int* o_pValue) { return m_pVar->AsScalar()->GetInt(o_pValue); }
	virtual HRESULT SetBool(bool i_Value) { return m_pVar->AsScalar()->SetBool(i_Value); }
	virtual HRESULT GetBool(bool* o_pValue) { return m_pVar->AsScalar()->GetBool(o_pValue); }

	virtual HRESULT SetFloatVector(const float* i_pValue) { return m_pVar->AsVector()->SetFloatVector(i_pValue); }
	virtual HRESULT GetFloatVector(float* o_pValue) { return m_pVar->AsVector()->GetFloatVector(o_pValue); }
	virtual HRESULT SetFloatVectorArray(const float* i_pValues, UINT i_Offset, UINT i_Count)
	{
		return m_pVar->AsVector()->SetFloatVectorArray(i_pValues, i_Offset, i_Count);
	}

	virtual HRESULT SetMatrix(const float* i_pValue) { return m_pVar->AsMatrix()->SetMatrix(i_pValue); }
	virtual HRESULT SetMatrixArray(const float* i_pValues, UINT i_Offset, UINT i_Count)
	{
		return m_pVar->AsMatrix()->SetMatrixArray(i_pValues, i_Offset, i_Count);
	}

	virtual HRESULT GetString(LPCSTR* o_pValue) { return m_pVar->AsString()->GetString(o_pValue); }

	virtual HRESULT SetResource(ID3D11ShaderResourceView* i_pView) { return m_pVar->AsShaderResource()->SetResource(i_pView); }
	virtual HRESULT GetResource(ID3D11ShaderResourceView** o_ppView) { return m_pVar->AsShaderResource()->GetResource(o_ppView); }
	virtual HRESULT SetUnorderedAccessView(ID3D11UnorderedAccessView* i_pView)
	{
		return m_pVar->AsUnorderedAccessView()->SetUnorderedAccessView(i_pView);
	}

private:
	effFX11Effect* m_pOwner;
	ID3DX11EffectVariable* m_pVar;
};

//============================================================================
//============================================================================
class effFX11Effect::Pass : public fxEffectPass
{
public:
	explicit Pass(ID3DX11EffectPass* i_pPass) : m_pPass(i_pPass) {}

	virtual bool IsValid() { return m_pPass->IsValid() != FALSE; }
	virtual HRESULT GetDesc(fxEffectPassDesc* o_pDesc)
	{
		D3DX11_PASS_DESC desc;
		HRESULT hr = m_pPass->GetDesc(&desc);
		if (FAILED(hr))
			return hr;
		o_pDesc->Name = desc.Name;
		o_pDesc->pIAInputSignature = desc.pIAInputSignature;
		o_pDesc->IAInputSignatureSize = desc.IAInputSignatureSize;
		return S_OK;
	}
	virtual HRESULT Apply(UINT i_Flags, ID3D11DeviceContext* i_pContext)
	{
		return m_pPass->Apply(i_Flags, i_pContext);
	}

private:
	ID3DX11EffectPass* m_pPass;
};

//============================================================================
//============================================================================
class effFX11Effect::Technique : public fxEffectTechnique
{
public:
	Technique(effFX11Effect* i_pOwner, ID3DX11EffectTechnique* i_pTechnique)
	:	m_pOwner(i_pOwner), m_pTech(i_pTechnique) {}

	virtual bool IsValid() { return m_pTech->IsValid() != FALSE; }
	virtual HRESULT GetDesc(fxEffectTechniqueDesc* o_pDesc)
	{
		D3DX11_TECHNIQUE_DESC desc;
		HRESULT hr = m_pTech->GetDesc(&desc);
		if (FAILED(hr))
			return hr;
		o_pDesc->Name = desc.Name;
		o_pDesc->Passes = desc.Passes;
		return S_OK;
	}
	virtual fxEffectPass* GetPassByIndex(UINT i_Index) { return m_pOwner->Wrap(m_pTech->GetPassByIndex(i_Index)); }
	virtual fxEffectPass* GetPassByName(LPCSTR i_Name) { return m_pOwner->Wrap(m_pTech->GetPassByName(i_Name)); }

private:
	effFX11Effect* m_pOwner;
	ID3DX11EffectTechnique* m_pTech;
};

//============================================================================
//============================================================================
effFX11Effect::effFX11Effect(ID3DX11Effect* i_pEffect)
:	m_pEffect(i_pEffect)
{
}

effFX11Effect::~effFX11Effect()
{
}

HRESULT effFX11Effect::GetDesc(fxEffectDesc_* o_pDesc)
{
	D3DX11_EFFECT_DESC desc;
	HRESULT hr = m_pEffect->GetDesc(&desc);
	if (FAILED(hr))
		return hr;
	o_pDesc->GlobalVariables = desc.GlobalVariables;
	o_pDesc->Techniques = desc.Techniques;
	return S_OK;
}

fxEffectVariable* effFX11Effect::GetVariableByIndex(UINT i_Index) { return Wrap(m_pEffect->GetVariableByIndex(i_Index)); }
fxEffectVariable* effFX11Effect::GetVariableByName(LPCSTR i_Name) { return Wrap(m_pEffect->GetVariableByName(i_Name)); }
fxEffectVariable* effFX11Effect::GetVariableBySemantic(LPCSTR i_Semantic) { return Wrap(m_pEffect->GetVariableBySemantic(i_Semantic)); }
fxEffectTechnique* effFX11Effect::GetTechniqueByIndex(UINT i_Index) { return Wrap(m_pEffect->GetTechniqueByIndex(i_Index)); }
fxEffectTechnique* effFX11Effect::GetTechniqueByName(LPCSTR i_Name) { return Wrap(m_pEffect->GetTechniqueByName(i_Name)); }

fxEffectVariable* effFX11Effect::Wrap(ID3DX11EffectVariable* i_pVariable)
{
	std::unique_ptr<Variable>& p = m_Variables[i_pVariable];
	if (!p)
		p.reset(new Variable(this, i_pVariable));
	return p.get();
}

fxEffectTechnique* effFX11Effect::Wrap(ID3DX11EffectTechnique* i_pTechnique)
{
	std::unique_ptr<Technique>& p = m_Techniques[i_pTechnique];
	if (!p)
		p.reset(new Technique(this, i_pTechnique));
	return p.get();
}

fxEffectPass* effFX11Effect::Wrap(ID3DX11EffectPass* i_pPass)
{
	std::unique_ptr<Pass>& p = m_Passes[i_pPass];
	if (!p)
		p.reset(new Pass(i_pPass));
	return p.get();
}

fxEffectType* effFX11Effect::Wrap(ID3DX11EffectType* i_pType)
{
	std::unique_ptr<Type>& p = m_Types[i_pType];
	if (!p)
		p.reset(new Type(i_pType));
	return p.get();
}
