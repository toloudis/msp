/*****************************************************************************
**  g3dDrawStyleUtilDX11.hpp
**
**	Sets D3D states for rendering in different draw styles
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_DRAWSTYLEUTILDX11_HPP
#error g3dDrawStyleUtilDX11.hpp multiply included
#endif
#define G3D_DRAWSTYLEUTILDX11_HPP

#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

namespace g3dDrawStyleUtilDX11
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Initialize();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeInitialize();

	//------------------------------------------------------------------------
	// SetDrawStyle - set mode for the given draw style
	//------------------------------------------------------------------------
	void SetDrawStyle(g3dSceneNode::DrawStyle i_DrawStyle);

	//------------------------------------------------------------------------
	// GetD3DDrawStyle - get mode for the given draw style
	//------------------------------------------------------------------------
	D3D11_FILL_MODE GetD3DDrawStyle();

	//------------------------------------------------------------------------
	// RestoreNormalDrawStyle - return D3D settings to regular rendering.
	//	Equivalent to calling SetDrawStyle(g3dSceneNode::e_Solid);
	//------------------------------------------------------------------------
	void RestoreNormalDrawStyle();

}
