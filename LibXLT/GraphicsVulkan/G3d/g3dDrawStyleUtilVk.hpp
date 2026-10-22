/*****************************************************************************
**  g3dDrawStyleUtilVk.hpp
**
**	Sets D3D states for rendering in different draw styles
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_G3D_DRAWSTYLEUTILVK_HPP
#error g3dDrawStyleUtilVk.hpp multiply included
#endif
#define GVK_G3D_DRAWSTYLEUTILVK_HPP

#include <volk.h>

#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif


namespace gvk
{

namespace g3dDrawStyleUtilVk
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
	VkPolygonMode GetD3DDrawStyle();

	//------------------------------------------------------------------------
	// RestoreNormalDrawStyle - return D3D settings to regular rendering.
	//	Equivalent to calling SetDrawStyle(g3dSceneNode::e_Solid);
	//------------------------------------------------------------------------
	void RestoreNormalDrawStyle();

}
}
