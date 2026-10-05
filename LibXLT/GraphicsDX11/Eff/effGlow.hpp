/*****************************************************************************
**  effGlow.hpp
**
**      effGlow is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_GLOW_HPP
#error effGlow.hpp multiply included
#endif
#define EFF_GLOW_HPP

#ifndef EFF_PLAINSHADERDX11_HPP
#include "GraphicsDX11/eff/effPlainShaderDX11.hpp"
#endif

class matMaterial;

class effGlow : public effPlainShaderDX11
{
public:
	effGlow(std::unique_ptr<fxEffectDX11> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

protected:
	fxEffectDX11::Resource m_hTexture;
	fxEffectDX11::Constant m_hGlowAmount;
	fxEffectDX11::Constant m_hGlowScale;
	fxEffectDX11::Constant m_hGlowSize;
	fxEffectDX11::Constant m_hConstantGlow;
	fxEffectDX11::Constant m_hSrcSizeInfo;
};
