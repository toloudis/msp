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

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

class matMaterial;

class effBlur : public effShaderBaseDX11 
{
public:
	effBlur(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupParams(const effShaderData* i_Data) const;

protected:
	ID3DX11EffectShaderResourceVariable* m_hSceneTexture;
	ID3DX11EffectShaderResourceVariable* m_hDownsampledTexture;
	ID3DX11EffectShaderResourceVariable* m_hHorizontalBlurTexture;

	ID3DX11EffectVectorVariable* m_hSrcSize;
	ID3DX11EffectVectorVariable* m_hDownsampledSize;
};
