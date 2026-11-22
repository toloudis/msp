/*****************************************************************************
**  effBlur.cpp
**
**      effBlur is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effBlur.hpp"

#include "Graphics/eff/effBlurData.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTexture.hpp"

effBlur::effBlur(std::unique_ptr<fxEffectDX11> i_pEffect, std::string i_name)
:	effPlainShaderDX11(std::move(i_pEffect), i_name)
{
	m_hSrcSize = m_pEffect->FindConstant("srcSizeInfo");
	m_hDownsampledSize = m_pEffect->FindConstant("downsampledSizeInfo");
	m_hSceneTexture = m_pEffect->FindResource("sceneTexture");
	m_hDownsampledTexture = m_pEffect->FindResource("downsampledTexture");
	m_hHorizontalBlurTexture = m_pEffect->FindResource("horizontalBlurTexture");
}
void effBlur::SetupParams(const effShaderData* i_Data) const
{
	const effBlurData* pData = dynamic_cast<const effBlurData*>(i_Data);
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effBlur");

	m_pEffect->SetResource(m_hSceneTexture, g3dDX11TextureUtil::GetD3DTexture(pData->m_pSceneTexture));
	m_pEffect->SetResource(m_hDownsampledTexture, g3dDX11TextureUtil::GetD3DTexture(pData->m_pDownsampledTexture));
	m_pEffect->SetResource(m_hHorizontalBlurTexture, g3dDX11TextureUtil::GetD3DTexture(pData->m_pHorizontalBlurTexture));

	float vec[4] = {(float)pData->m_pSceneTexture->GetWidth(), (float)pData->m_pSceneTexture->GetHeight(),
			pData->m_FilterWidthX,
			pData->m_FilterWidthY};
	m_pEffect->SetConstant(m_hSrcSize, vec);
	float vecLow[4] = {(float)pData->m_pDownsampledTexture->GetWidth(), (float)pData->m_pDownsampledTexture->GetHeight(),
			pData->m_FilterWidthX,
			pData->m_FilterWidthY};
	m_pEffect->SetConstant(m_hDownsampledSize, vecLow);
}

effShaderData* effBlur::CreateData(const matMaterial* i_Mat)
{
	return new effBlurData();
}




