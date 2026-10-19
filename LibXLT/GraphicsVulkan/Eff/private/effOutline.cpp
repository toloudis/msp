/*****************************************************************************
**  effOutline.cpp
**
**      effOutline is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_EFF_OUTLINE_HPP
#include "GraphicsVulkan/Eff/effOutline.hpp"
#endif

#ifndef EFF_OUTLINEDATA_HPP
#include "Graphics/eff/effOutlineData.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif

namespace gvk
{

effOutline::effOutline(std::unique_ptr<fxEffectVk> i_pEffect, std::string i_name)
:	effPlainShaderVk(std::move(i_pEffect), i_name)
{
	m_hTexture = m_pEffect->FindResource("colorTexture");
	m_hDepthScale = m_pEffect->FindConstant("g_depthScale");
	m_hMinAngle = m_pEffect->FindConstant("g_minAngle");
	m_hMaxAngle = m_pEffect->FindConstant("g_maxAngle");
	m_hThickness = m_pEffect->FindConstant("g_thickness");
	m_hColor = m_pEffect->FindConstant("g_outlineClr");
	m_hViewSize = m_pEffect->FindConstant("g_ViewportDimensions");
	m_hMinWidth = m_pEffect->FindConstant("g_minWidth");
	m_hMaxWidth = m_pEffect->FindConstant("g_maxWidth");

}

void effOutline::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effOutlineData* pData = &(i_Material->GetOutlineData());
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effOutline");

	m_pEffect->SetResource(m_hTexture, g3dVkTextureUtil::GetImage(pData->m_pTexture));

	m_pEffect->SetConstant(m_hDepthScale, (float)pData->m_OutlineDepthScale);
	m_pEffect->SetConstant(m_hMinAngle, (float)pData->m_OutlineMinAngle);
	m_pEffect->SetConstant(m_hMaxAngle, (float)pData->m_OutlineMaxAngle);
	m_pEffect->SetConstant(m_hThickness, (float)pData->m_OutlineThickness);
	m_pEffect->SetConstant(m_hMinWidth, (float)pData->m_OutlineMinWidth);
	m_pEffect->SetConstant(m_hMaxWidth, (float)pData->m_OutlineMaxWidth);
	m_pEffect->SetFloatVector(m_hColor, pData->m_OutlineColor.Ptr());
	m_pEffect->SetFloatVector(m_hViewSize, pData->m_OutlineViewSize.Ptr());
}

effShaderData* effOutline::CreateData(const matMaterial* i_Mat)
{
	return new effOutlineData();
}

}
