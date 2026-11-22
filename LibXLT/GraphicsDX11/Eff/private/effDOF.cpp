/*****************************************************************************
**  effDOF.cpp
**
**      effDOF is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effDOF.hpp"

#include "Graphics/eff/effDOFData.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"

effDOF::effDOF(std::unique_ptr<fxEffectDX11> i_pEffect, std::string i_name)
:	effPlainShaderDX11(std::move(i_pEffect), i_name)
{
	m_hSharpTexture = m_pEffect->FindResource("tSource");
	m_hBlurryTexture = m_pEffect->FindResource("tSourceLow");
	m_hSharpResolutionData = m_pEffect->FindConstant("pixelSizeHigh");
	m_hBlurryResolutionData = m_pEffect->FindConstant("pixelSizeLow");
	m_hMaxCoC = m_pEffect->FindConstant("maxCoC");
}
void effDOF::SetupParams(const effShaderData* i_Data) const
{
	const effDOFData* pData = dynamic_cast<const effDOFData*>(i_Data);
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effDOF");

	m_pEffect->SetResource(m_hSharpTexture, g3dDX11TextureUtil::GetD3DTexture(pData->m_pSharpTexture));
	m_pEffect->SetResource(m_hBlurryTexture, g3dDX11TextureUtil::GetD3DTexture(pData->m_pBlurryTexture));
	float vec[2] = {pData->m_FilterWidthX, pData->m_FilterWidthY};
	m_pEffect->SetConstant(m_hSharpResolutionData, vec);
	float vecBlur[2] = {pData->m_FilterWidthX, pData->m_FilterWidthY};
	m_pEffect->SetConstant(m_hBlurryResolutionData, vecBlur);
	m_pEffect->SetConstant(m_hMaxCoC, pData->m_MaxCoC);
}

effShaderData* effDOF::CreateData(const matMaterial* i_Mat)
{
	return new effDOFData();
}




