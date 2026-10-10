/*****************************************************************************
**  effLightGlow.hpp
**
**      effLightGlow is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_LIGHTGLOW_HPP
#error effLightGlow.hpp multiply included
#endif
#define EFF_LIGHTGLOW_HPP

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

class matMaterial;

class effLightGlow : public effShaderBaseDX11 
{
public:
	effLightGlow(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

	//====================================================================
	// Returns false because this effect doesn't need single light passes.
	//====================================================================
	virtual bool DoesLighting() const;

protected:
	fxEffectVariable* m_hHasDiffuseMap;
	fxEffectVariable* m_hDiffuseMap;

	fxEffectVariable* m_hEdgeFuzzCutoff;
	fxEffectVariable* m_hDistFalloffStart;
	fxEffectVariable* m_hDistFalloffEnd;
	fxEffectVariable* m_hGlowAlpha;
	fxEffectVariable* m_hUseShadow;
};

