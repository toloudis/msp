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

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

class matMaterial;

class effGlow : public effShaderBaseDX11 
{
public:
	effGlow(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

protected:
	ID3DX11EffectShaderResourceVariable* m_hTexture;
	ID3DX11EffectScalarVariable* m_hGlowAmount;
	ID3DX11EffectVectorVariable* m_hGlowScale;
	ID3DX11EffectScalarVariable* m_hGlowSize;
	ID3DX11EffectScalarVariable* m_hConstantGlow;
	ID3DX11EffectVectorVariable* m_hSrcSizeInfo;
};
