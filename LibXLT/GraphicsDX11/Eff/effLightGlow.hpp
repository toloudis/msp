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
	effLightGlow(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

	//====================================================================
	// Returns false because this effect doesn't need single light passes.
	//====================================================================
	virtual bool DoesLighting() const;

protected:
	ID3DX11EffectScalarVariable* m_hHasDiffuseMap;
	ID3DX11EffectShaderResourceVariable* m_hDiffuseMap;

	ID3DX11EffectScalarVariable* m_hEdgeFuzzCutoff;
	ID3DX11EffectScalarVariable* m_hDistFalloffStart;
	ID3DX11EffectScalarVariable* m_hDistFalloffEnd;
	ID3DX11EffectScalarVariable* m_hGlowAlpha;
	ID3DX11EffectScalarVariable* m_hUseShadow;
};

