/*****************************************************************************
**  effDOF.hpp
**
**      effDOF is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_DOF_HPP
#error effDOF.hpp multiply included
#endif
#define EFF_DOF_HPP

#ifndef EFF_PLAINSHADERDX11_HPP
#include "GraphicsDX11/eff/effPlainShaderDX11.hpp"
#endif

class matMaterial;

class effDOF : public effPlainShaderDX11
{
public:
	effDOF(std::unique_ptr<fxEffectDX11> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupParams(const effShaderData* i_Data) const;

protected:
	fxEffectDX11::Resource m_hSharpTexture;
	fxEffectDX11::Resource m_hBlurryTexture;
	fxEffectDX11::Constant m_hSharpResolutionData;
	fxEffectDX11::Constant m_hBlurryResolutionData;
	fxEffectDX11::Constant m_hMaxCoC;
};
