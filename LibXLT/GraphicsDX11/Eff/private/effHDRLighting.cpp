/*****************************************************************************
**  effHDRLighting.cpp
**
**      effPhong is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/eff/effHDRLighting.hpp"

effHDRLighting::effHDRLighting(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name)
	:	effShaderBaseDX11(i_Directory, i_pEffect, i_name) 
{
	//ID3DX11EffectTechnique* m_TFinalScenePass;

	this->MapParameter("g_fixedLuminance", m_HFixedLuminance);
	this->MapParameter("g_bEnableToneMap", m_HEnableToneMap);
	this->MapParameter("g_fMiddleGray", m_HMiddleGrey);
	this->MapParameter("g_fWhiteCutoff", m_HWhiteCutoff);
	this->MapParameter("g_fStarScale", m_HStarScale);
	this->MapParameter("g_fBloomScale", m_HBloomScale);
}

void effHDRLighting::ToneMapSetEnable(bool i_Enable)
{
	m_HEnableToneMap->SetBool(i_Enable);
}
void effHDRLighting::ToneMapSetLuminance(float i_Lum)
{
	m_HFixedLuminance->SetFloat(i_Lum);
}
void effHDRLighting::ToneMapSetMiddleGrey(float i_Gray)
{
	m_HMiddleGrey->SetFloat(i_Gray);
}
void effHDRLighting::ToneMapSetWhiteCutoff(float i_WhiteCutoff)
{
	m_HWhiteCutoff->SetFloat(i_WhiteCutoff);
}
void effHDRLighting::ToneMapSetStarScale(float i_StarScale)
{
	m_HStarScale->SetFloat(i_StarScale);
}
void effHDRLighting::ToneMapSetBloomScale(float i_BloomScale)
{
	m_HBloomScale->SetFloat(i_BloomScale);
}
void effHDRLighting::SetToneMapping()
{
	this->m_CurrentTechnique = m_TFinalScenePass;
}
