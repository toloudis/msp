/*****************************************************************************
**  effDOF.cpp
**
**      effDOF is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_EFF_DOF_HPP
#include "GraphicsVulkan/Eff/effDOF.hpp"
#endif

#ifndef EFF_DOFDATA_HPP
#include "Graphics/eff/effDOFData.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/mat/matTexture.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif

namespace gvk
{

effDOF::effDOF(std::unique_ptr<fxEffectVk> i_pEffect, std::string i_name)
:	effPlainShaderVk(std::move(i_pEffect), i_name)
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

	m_pEffect->SetResource(m_hSharpTexture, g3dVkTextureUtil::GetImage(pData->m_pSharpTexture));
	m_pEffect->SetResource(m_hBlurryTexture, g3dVkTextureUtil::GetImage(pData->m_pBlurryTexture));
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

}
