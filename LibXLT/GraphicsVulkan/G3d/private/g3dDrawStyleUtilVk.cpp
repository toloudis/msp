/*****************************************************************************
**  g3dDrawStyleUtilVk.cpp
**
**	Sets D3D states for rendering in different draw styles
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif

#ifndef EFF_SOLIDDATA_HPP
#include "Graphics/eff/effSolidData.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/eff/effPhongData.hpp"
#endif
#ifndef EFF_UNLITDATA_HPP
#include "Graphics/eff/effUnlitData.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif

namespace gvk
{

namespace
{
	matMaterial* l_pWireframeMaterial = NULL;
	g3dSceneNode::DrawStyle l_DrawStyle = g3dSceneNode::e_Solid;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g3dDrawStyleUtilVk::Initialize()
{
	// Solid shader just uses a constant color for all vertices
	l_pWireframeMaterial = new matMaterial(); // "default");//("Solid.fx");

	effPhongData* pSolidData = dynamic_cast<effPhongData*>(l_pWireframeMaterial->GetEffectData());
	if (pSolidData)
	{
		pSolidData->m_ColorAmbient.Set	(0.6f, 0.6f, 0.6f, 1);
		pSolidData->m_ColorDiffuse.Set	(0.4f, 0.4f, 0.4f, 1);
		pSolidData->m_ColorSpecular.Set	(0.0f, 0.0f, 0.0f, 1);
		pSolidData->m_Transparency = 1;
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g3dDrawStyleUtilVk::DeInitialize()
{
	// Clear out shader
	delete l_pWireframeMaterial;
	l_pWireframeMaterial = NULL;
}

//------------------------------------------------------------------------
// SetDrawStyle - set D3D states for the given draw style
//------------------------------------------------------------------------
void g3dDrawStyleUtilVk::SetDrawStyle(g3dSceneNode::DrawStyle i_DrawStyle)
{
	if (l_DrawStyle != i_DrawStyle && i_DrawStyle != g3dSceneNode::e_Inherit)
	{
		l_DrawStyle = i_DrawStyle;
		switch (i_DrawStyle)
		{
		case g3dSceneNode::e_Solid:
			g3dVkUtil::SetOverrideMaterial(NULL);
			break;
		case g3dSceneNode::e_Wireframe:
			g3dVkUtil::SetOverrideMaterial(l_pWireframeMaterial);
			break;
		case g3dSceneNode::e_LitWireframe:
			g3dVkUtil::SetOverrideMaterial(NULL);
			break;
		}
	}
}

//------------------------------------------------------------------------
// GetD3DDrawStyle - get D3D Fill for the given draw style
//------------------------------------------------------------------------
VkPolygonMode g3dDrawStyleUtilVk::GetD3DDrawStyle()
{
	return l_DrawStyle == g3dSceneNode::e_Solid ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE;
}

//------------------------------------------------------------------------
// RestoreNormalDrawStyle - return D3D settings to regular rendering.
//	Equivalent to calling SetDrawStyle(g3dSceneNode::e_Solid);
//------------------------------------------------------------------------
void g3dDrawStyleUtilVk::RestoreNormalDrawStyle()
{
	SetDrawStyle(g3dSceneNode::e_Solid);
}

}
