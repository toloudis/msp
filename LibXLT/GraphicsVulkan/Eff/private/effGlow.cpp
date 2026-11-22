/*****************************************************************************
**  effGlow.cpp
**
**      effGlow is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_EFF_GLOW_HPP
#include "GraphicsVulkan/Eff/effGlow.hpp"
#endif

#ifndef EFF_GLOWDATA_HPP
#include "Graphics/eff/effGlowData.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif

namespace gvk
{

effGlow::effGlow(std::unique_ptr<fxEffectVk> i_pEffect, std::string i_name)
:	effPlainShaderVk(std::move(i_pEffect), i_name)
{
	m_hTexture = m_pEffect->FindResource("glowTexture");
	m_hGlowAmount = m_pEffect->FindConstant("glowAmount");
	m_hGlowScale = m_pEffect->FindConstant("glowScale");
	m_hGlowSize = m_pEffect->FindConstant("glowSize");
	m_hConstantGlow = m_pEffect->FindConstant("bConstantGlow");
	m_hSrcSizeInfo = m_pEffect->FindConstant("srcSizeInfo");
}

void effGlow::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effGlowData* pData = &(i_Material->GetGlowData());
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effGlow");

	m_pEffect->SetResource(m_hTexture, g3dVkTextureUtil::GetImage(pData->m_pTexture));
	m_pEffect->SetConstant(m_hGlowAmount, (float)pData->m_GlowAmount);
	m_pEffect->SetFloatVector(m_hGlowScale, pData->m_GlowScale.Ptr());
	m_pEffect->SetConstant(m_hGlowSize, (float)pData->m_GlowSize);
	m_pEffect->SetConstant(m_hConstantGlow, (int)(pData->m_bConstantGlow ? 1 : 0));	// HLSL bool is 4 bytes
	m_pEffect->SetFloatVector(m_hSrcSizeInfo, pData->m_SrcSizeInfo.Ptr());
}

effShaderData* effGlow::CreateData(const matMaterial* i_Mat)
{
	return new effGlowData();
}

}
