/****************************************************************************\
**	shdwCubeMapRendererVk.hpp
**
**	The shdwCubeMapRendererVk renders the scene from a light's point of
**	view in order to get the depth map info for shadow mapping.
**
**	GraphicsVulkan port of GraphicsDX11's shdwCubeMapRendererDX11: the
**	scene seen from a reflector's centre, one cube face at a time, drawn by
**	the simple renderer (shdwShadowLayerRendererVk) into the reflector
**	material's cube map, whose mips are then generated; once, or every
**	frame.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_SHDW_CUBEMAPRENDERERVK_HPP
#error shdwCubeMapRendererVk.hpp multiply included
#endif
#define GVK_SHDW_CUBEMAPRENDERERVK_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif

#include <list>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class g3dSceneNode;
class matMaterial;
class scObject;

namespace gvk
{

class shdwCubeMapRendererVk : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwCubeMapRendererVk();
	shdwCubeMapRendererVk(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~shdwCubeMapRendererVk();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	virtual int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
				 const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
				 float i_fSimTime );

	//--------------------------------------------------------------------
	//	Let a renderer free up any memory it is holding on to
	//--------------------------------------------------------------------
	virtual void ReleaseResources();

	//--------------------------------------------------------------------
	//	A renderer can be enabled or disabled from having Render() called.
	//--------------------------------------------------------------------
	virtual bool IsEnabled();

	//--------------------------------------------------------------------
	//	Give this renderer a hint as to what objects not to render, and where
	//	to position the camera.
	//--------------------------------------------------------------------
	void SetSceneObject(scObject* i_Obj, matMaterial* i_Material);

	//--------------------------------------------------------------------
	//	Tell this renderer whether to render on every frame or disable itself
	//	after one frame.
	//--------------------------------------------------------------------
	void SetPerFrameRendering(bool i_RenderPerFrame) {m_bRenderPerFrame = i_RenderPerFrame;}

protected:
	g3dSceneRenderer* m_pFaceRenderer;
	scObject* m_pObject;
	matMaterial* m_pMaterial;
	bool m_bRenderPerFrame;
	bool m_bFirstRender;

	//--------------------------------------------------------------------
	// collect all nodes that match the given material
	//--------------------------------------------------------------------
	void RecurseCollectNodes(g3dSceneNode* i_Node, std::list<g3dSceneNode*>& o_Nodes);
};

}
