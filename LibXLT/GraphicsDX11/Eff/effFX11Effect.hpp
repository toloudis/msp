/*****************************************************************************
**  effFX11Effect.hpp
**
**      fxEffect (GraphicsDX11/Fx/fxEffectApi.hpp) on top of a D3DX11 Effects
**      effect, for shaders that have not been converted to plain HLSL yet.
**      Every call forwards to the matching ID3DX11Effect call.
\****************************************************************************/

#ifdef EFF_FX11EFFECT_HPP
#error effFX11Effect.hpp multiply included
#endif
#define EFF_FX11EFFECT_HPP

#ifndef FX_EFFECTAPI_HPP
#include "GraphicsDX11/Fx/fxEffectApi.hpp"
#endif

#include <map>
#include <memory>

struct ID3DX11Effect;
struct ID3DX11EffectVariable;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectPass;
struct ID3DX11EffectType;

class effFX11Effect : public fxEffect
{
public:
	// Takes no reference; the caller keeps owning i_pEffect.
	explicit effFX11Effect(ID3DX11Effect* i_pEffect);
	virtual ~effFX11Effect();

	ID3DX11Effect* GetD3DXEffect() const { return m_pEffect; }

	virtual HRESULT GetDesc(fxEffectDesc_* o_pDesc);
	virtual fxEffectVariable* GetVariableByIndex(UINT i_Index);
	virtual fxEffectVariable* GetVariableByName(LPCSTR i_Name);
	virtual fxEffectVariable* GetVariableBySemantic(LPCSTR i_Semantic);
	virtual fxEffectTechnique* GetTechniqueByIndex(UINT i_Index);
	virtual fxEffectTechnique* GetTechniqueByName(LPCSTR i_Name);

	// Wrappers for objects Effects returned; created once, owned here.
	fxEffectVariable* Wrap(ID3DX11EffectVariable* i_pVariable);
	fxEffectTechnique* Wrap(ID3DX11EffectTechnique* i_pTechnique);
	fxEffectPass* Wrap(ID3DX11EffectPass* i_pPass);
	fxEffectType* Wrap(ID3DX11EffectType* i_pType);

private:
	class Variable;
	class Technique;
	class Pass;
	class Type;

	ID3DX11Effect* m_pEffect;
	std::map<ID3DX11EffectVariable*, std::unique_ptr<Variable>> m_Variables;
	std::map<ID3DX11EffectTechnique*, std::unique_ptr<Technique>> m_Techniques;
	std::map<ID3DX11EffectPass*, std::unique_ptr<Pass>> m_Passes;
	std::map<ID3DX11EffectType*, std::unique_ptr<Type>> m_Types;
};
