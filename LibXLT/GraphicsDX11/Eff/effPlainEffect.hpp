/*****************************************************************************
**  effPlainEffect.hpp
**
**      fxEffect (GraphicsDX11/Fx/fxEffectApi.hpp) on top of a converted
**      plain-HLSL effect (fxEffectDX11), so the material shader classes can
**      drive it exactly as they drove the Effects version.
**
**      Variables come from the effect's manifest, in declaration order, with
**      their semantics and SAS annotations; their storage is the constant
**      buffer slot or resource slot shader reflection found for them. A
**      variable that no entry point reads still exists (as it did in
**      Effects) and keeps what is written to it on the CPU only.
\****************************************************************************/

#ifdef EFF_PLAINEFFECT_HPP
#error effPlainEffect.hpp multiply included
#endif
#define EFF_PLAINEFFECT_HPP

#ifndef FX_EFFECTAPI_HPP
#include "GraphicsDX11/Fx/fxEffectApi.hpp"
#endif

#include <map>
#include <memory>
#include <string>
#include <vector>

class fxEffectDX11;

class effPlainEffect : public fxEffect
{
public:
	// i_Source: where the effect came from, e.g. "Materials/Phong.effect.json"
	explicit effPlainEffect(std::unique_ptr<fxEffectDX11> i_pEffect,
		const std::string& i_Source = std::string());
	virtual ~effPlainEffect();

	fxEffectDX11* GetEffectDX11() const { return m_pEffect.get(); }
	// A material (from Materials/), as opposed to a special or post effect
	// the renderer drives directly.
	bool IsMaterial() const { return m_Source.compare(0, 10, "Materials/") == 0; }

	virtual HRESULT GetDesc(fxEffectDesc_* o_pDesc);
	virtual fxEffectVariable* GetVariableByIndex(UINT i_Index);
	virtual fxEffectVariable* GetVariableByName(LPCSTR i_Name);
	virtual fxEffectVariable* GetVariableBySemantic(LPCSTR i_Semantic);
	virtual fxEffectTechnique* GetTechniqueByIndex(UINT i_Index);
	virtual fxEffectTechnique* GetTechniqueByName(LPCSTR i_Name);

	class Variable;
	class Technique;
	class Pass;
	class Type;

	Variable* GetInvalidVariable() { return m_pInvalidVariable.get(); }
	Pass* GetInvalidPass() { return m_pInvalidPass.get(); }

private:
	std::unique_ptr<fxEffectDX11> m_pEffect;
	std::string m_Source;
	std::vector<std::unique_ptr<Variable>> m_Variables;
	std::map<std::string, Variable*> m_VariableNames;
	std::vector<std::unique_ptr<Technique>> m_Techniques;
	std::unique_ptr<Variable> m_pInvalidVariable;
	std::unique_ptr<Technique> m_pInvalidTechnique;
	std::unique_ptr<Pass> m_pInvalidPass;
};
