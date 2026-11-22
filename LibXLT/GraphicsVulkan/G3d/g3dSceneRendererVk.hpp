/****************************************************************************\
**	g3dSceneRendererVk.hpp
**
**	The g3dSceneRendererVk renders the g3dSceneNodes which describe the
**	graphical scene.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_G3D_SCENERENDERERVK_HPP
#error g3dSceneRendererVk.hpp multiply included
#endif
#define GVK_G3D_SCENERENDERERVK_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif

#include <functional>
#include <vector>

class g3dLayer;
class g3dSceneNode;
class camCamera;

namespace gvk
{

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------

class g3dSceneRendererVk : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g3dSceneRendererVk();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~g3dSceneRendererVk();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
				 const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
				 float i_fSimTime );

	void ReleaseResources(){}

	//--------------------------------------------------------------------
	//	A node filter: in world-space layers, a node it returns false for
	//	isn't drawn (its children still are). None by default. GraphicsOptiX
	//	uses it to draw only what its tracer doesn't (lines, gizmos) over
	//	the traced image.
	//--------------------------------------------------------------------
	typedef std::function<bool(const g3dSceneNode&)> NodeFilter;
	void SetNodeFilter(const NodeFilter& i_Filter) { m_NodeFilter = i_Filter; }

	//--------------------------------------------------------------------
	//	Whether Render clears the target's colour first (the default);
	//	false draws over what the target holds, depth-testing against its
	//	depth
	//--------------------------------------------------------------------
	void SetClearTarget(bool i_bClear) { m_bClearTarget = i_bClear; }

	static void InitStates();
	static void CleanupStates();

private:
	NodeFilter	m_NodeFilter;
	bool		m_bClearTarget = true;
};
}
