/*****************************************************************************
**  effBlur.hpp
**
**      effBlur is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_BLUR_HPP
#error effBlur.hpp multiply included
#endif
#define EFF_BLUR_HPP

#ifndef EFF_PLAINSHADERDX11_HPP
#include "GraphicsDX11/eff/effPlainShaderDX11.hpp"
#endif

class matMaterial;

class effBlur : public effPlainShaderDX11
{
public:
	effBlur(std::unique_ptr<fxEffectDX11> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupParams(const effShaderData* i_Data) const;

protected:
	fxEffectDX11::Resource m_hSceneTexture;
	fxEffectDX11::Resource m_hDownsampledTexture;
	fxEffectDX11::Resource m_hHorizontalBlurTexture;

	fxEffectDX11::Constant m_hSrcSize;
	fxEffectDX11::Constant m_hDownsampledSize;
};
