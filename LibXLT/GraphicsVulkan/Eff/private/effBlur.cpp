/*****************************************************************************
**  effBlur.cpp
**
**      effBlur is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_EFF_BLUR_HPP
#include "GraphicsVulkan/Eff/effBlur.hpp"
#endif

#ifndef EFF_BLURDATA_HPP
#include "Graphics/eff/effBlurData.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/mat/matTexture.hpp"
#endif

namespace gvk
{

effBlur::effBlur(std::unique_ptr<fxEffectVk> i_pEffect, std::string i_name)
:	effPlainShaderVk(std::move(i_pEffect), i_name)
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

	m_pEffect->SetResource(m_hSceneTexture, g3dVkTextureUtil::GetImage(pData->m_pSceneTexture));
	m_pEffect->SetResource(m_hDownsampledTexture, g3dVkTextureUtil::GetImage(pData->m_pDownsampledTexture));
	m_pEffect->SetResource(m_hHorizontalBlurTexture, g3dVkTextureUtil::GetImage(pData->m_pHorizontalBlurTexture));

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

}
