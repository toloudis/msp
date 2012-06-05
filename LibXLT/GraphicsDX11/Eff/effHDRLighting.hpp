/*****************************************************************************
**  effHDRLighting.hpp
**
**      effHDRLighting is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_HDRLIGHTING_HPP
#error effHDRLighting.hpp multiply included
#endif
#define EFF_HDRLIGHTING_HPP

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

class effHDRLighting : public effShaderBaseDX11 
{
public:
	effHDRLighting(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_Name);

	void ToneMapSetEnable(bool i_Enable);
	void ToneMapSetLuminance(float i_Lum);
	void ToneMapSetMiddleGrey(float i_Gray);
	void ToneMapSetWhiteCutoff(float i_WhiteCutoff);
	void ToneMapSetStarScale(float i_StarScale);
	void ToneMapSetBloomScale(float i_BloomScale);

	void SetToneMapping();

private:
	ID3DX11EffectTechnique* m_TFinalScenePass;

	ID3DX11EffectScalarVariable* m_HFixedLuminance;
	ID3DX11EffectScalarVariable* m_HEnableToneMap;
	ID3DX11EffectScalarVariable* m_HMiddleGrey;
	ID3DX11EffectScalarVariable* m_HWhiteCutoff;
	ID3DX11EffectScalarVariable* m_HStarScale;
	ID3DX11EffectScalarVariable* m_HBloomScale;
};
