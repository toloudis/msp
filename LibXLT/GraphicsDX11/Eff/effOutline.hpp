/*****************************************************************************
**  effOutline.hpp
**
**      effOutline is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_OUTLINE_HPP
#error effOutline.hpp multiply included
#endif
#define EFF_OUTLINE_HPP

#ifndef EFF_PLAINSHADERDX11_HPP
#include "GraphicsDX11/eff/effPlainShaderDX11.hpp"
#endif

class matMaterial;

class effOutline : public effPlainShaderDX11
{
public:
	effOutline(std::unique_ptr<fxEffectDX11> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

protected:
	fxEffectDX11::Resource m_hTexture;
	fxEffectDX11::Constant m_hDepthScale;
	fxEffectDX11::Constant m_hMinAngle;
	fxEffectDX11::Constant m_hMaxAngle;
	fxEffectDX11::Constant m_hThickness;
	fxEffectDX11::Constant m_hColor;
	fxEffectDX11::Constant m_hViewSize;
	fxEffectDX11::Constant m_hMinWidth;
	fxEffectDX11::Constant m_hMaxWidth;
};
