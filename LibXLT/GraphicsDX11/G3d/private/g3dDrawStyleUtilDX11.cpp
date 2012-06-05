/*****************************************************************************
**  g3dDrawStyleUtilDX11.cpp
**
**	Sets D3D states for rendering in different draw styles
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"

#include "Graphics/eff/effSolidData.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/eff/effUnlitData.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"

namespace
{
	matMaterial* l_pWireframeMaterial = NULL;
	g3dSceneNode::DrawStyle l_DrawStyle = g3dSceneNode::e_Solid;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g3dDrawStyleUtilDX11::Initialize()
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
void g3dDrawStyleUtilDX11::DeInitialize()
{
	// Clear out shader
	delete l_pWireframeMaterial;
	l_pWireframeMaterial = NULL;
}

//------------------------------------------------------------------------
// SetDrawStyle - set D3D states for the given draw style
//------------------------------------------------------------------------
void g3dDrawStyleUtilDX11::SetDrawStyle(g3dSceneNode::DrawStyle i_DrawStyle)
{
	if (l_DrawStyle != i_DrawStyle && i_DrawStyle != g3dSceneNode::e_Inherit)
	{
		l_DrawStyle = i_DrawStyle;
		switch (i_DrawStyle)
		{
		case g3dSceneNode::e_Solid:
			g3dDX11Util::SetOverrideMaterial(NULL);
			break;
		case g3dSceneNode::e_Wireframe:
			g3dDX11Util::SetOverrideMaterial(l_pWireframeMaterial);
			break;
		case g3dSceneNode::e_LitWireframe:
			g3dDX11Util::SetOverrideMaterial(NULL);
			break;
		}
	}
}

//------------------------------------------------------------------------
// GetD3DDrawStyle - get D3D Fill for the given draw style
//------------------------------------------------------------------------
D3D11_FILL_MODE g3dDrawStyleUtilDX11::GetD3DDrawStyle()
{
	return l_DrawStyle == g3dSceneNode::e_Solid ? D3D11_FILL_SOLID : D3D11_FILL_WIREFRAME;
}

//------------------------------------------------------------------------
// RestoreNormalDrawStyle - return D3D settings to regular rendering.
//	Equivalent to calling SetDrawStyle(g3dSceneNode::e_Solid);
//------------------------------------------------------------------------
void g3dDrawStyleUtilDX11::RestoreNormalDrawStyle()
{
	SetDrawStyle(g3dSceneNode::e_Solid);
}

