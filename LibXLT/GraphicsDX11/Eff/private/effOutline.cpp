/*****************************************************************************
**  effOutline.cpp
**
**      effOutline is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effOutline.hpp"

#include "Graphics/eff/effOutlineData.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "Graphics/mat/matMaterial.hpp"

effOutline::effOutline(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name)
:	effShaderBaseDX11(i_Directory, i_pEffect, i_name)
{
	MapParameter("colorTexture", m_hTexture );
	MapParameter("g_depthScale", m_hDepthScale);
	MapParameter("g_minAngle", m_hMinAngle);
	MapParameter("g_maxAngle", m_hMaxAngle);
	MapParameter("g_thickness", m_hThickness);
	MapParameter("g_outlineClr", m_hColor);
	MapParameter("g_ViewportDimensions", m_hViewSize);
	MapParameter("g_minWidth", m_hMinWidth);
	MapParameter("g_maxWidth", m_hMaxWidth);

}

void effOutline::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effOutlineData* pData = &(i_Material->GetOutlineData());
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effOutline");

	m_hTexture->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_pTexture));

	m_hDepthScale->SetFloat(pData->m_OutlineDepthScale);
	m_hMinAngle->SetFloat(pData->m_OutlineMinAngle);
	m_hMaxAngle->SetFloat(pData->m_OutlineMaxAngle);
	m_hThickness->SetFloat(pData->m_OutlineThickness);
	m_hMinWidth->SetFloat(pData->m_OutlineMinWidth);
	m_hMaxWidth->SetFloat(pData->m_OutlineMaxWidth);
	m_hColor->SetFloatVector( pData->m_OutlineColor.Ptr() );
	m_hViewSize->SetFloatVector( pData->m_OutlineViewSize.Ptr() );
}

effShaderData* effOutline::CreateData(const matMaterial* i_Mat)
{
	return new effOutlineData();
}




