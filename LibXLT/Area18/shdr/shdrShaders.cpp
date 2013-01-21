/****************************************************************************\
**	shdrVS.cpp
**
**		see .hpp
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Area18/shdr/shdrShaders.hpp"

#include "Area18/shdr/shdrDefaultVS.hpp"
#include "Area18/shdr/shdrColorHeadlightPS.hpp"
#include "Area18/shdr/shdrColorPS.hpp"
#include "Area18/shdr/shdrLambertPS.h"
//#include "Area18/shdr/shdrConvolution2dCS.hpp"
//#include "Area18/shdr/shdrMaterialPS.hpp"
//#include "Area18/shdr/shdrVolumeVizPS.hpp"
//#include "Area18/shdr/shdrVolumeVizIsoPS.hpp"
//#include "Area18/shdr/shdrVolumeVizMipPS.hpp"
//#include "Area18/shdr/shdrVolumeVizVS.hpp"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrShaders::shdrShaders(oglContext* i_pDevice)
{
	m_DefaultVS.reset(new shdrDefaultVS(i_pDevice));
	m_ColorPS.reset(new shdrColorPS(i_pDevice));
	m_ColorHeadlightPS.reset(new shdrColorHeadlightPS(i_pDevice));
	//m_LambertPS.reset(new shdrLambertPS(i_pDevice));

//	m_pCSVerticalFilter.reset(new shdrCS(i_pDevice, "ConvolutionCS.hlsl", "CSVerticalFilter"));
//	m_pCSHorizFilter.reset(new shdrCS(i_pDevice, "ConvolutionCS.hlsl", "CSHorizFilter"));
//	m_pPSTextureToTexture.reset(new shdrPS(i_pDevice, "TextureToTexture.glsl"));
//	m_pCSConvolve.reset(new shdrConvolution2dCS(i_pDevice));
//	m_pPSVolumeViz.reset(new shdrVolumeVizPS(i_pDevice));
//	m_pPSVolumeVizIso.reset(new shdrVolumeVizIsoPS(i_pDevice));
//	m_pPSVolumeVizMip.reset(new shdrVolumeVizMipPS(i_pDevice));
//	m_pVSVolumeViz.reset(new shdrVolumeVizVS(i_pDevice));

//	m_pMatPhong.reset(new shdrMaterialPS(i_pDevice, "PhongPS.hlsl", "PS"));
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrShaders::~shdrShaders()
{
}
