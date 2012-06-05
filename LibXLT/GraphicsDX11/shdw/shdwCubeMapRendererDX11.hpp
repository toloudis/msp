/****************************************************************************\
**	shdwCubeMapRendererDX11.hpp
**
**	The shdwCubeMapRendererDX11 renders the scene from a light's point of 
**	view in order to get the depth map info for shadow mapping.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_CUBEMAPRENDERERDX11_HPP
#error shdwCubeMapRendererDX11.hpp multiply included
#endif
#define SHDW_CUBEMAPRENDERERDX11_HPP

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

class shdwCubeMapRendererDX11 : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwCubeMapRendererDX11();
	shdwCubeMapRendererDX11(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~shdwCubeMapRendererDX11();

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
	bool m_Save;
	bool m_bRenderPerFrame;
	bool m_bFirstRender;

	//--------------------------------------------------------------------
	// collect all nodes that match the given material
	//--------------------------------------------------------------------
	void RecurseCollectNodes(g3dSceneNode* i_Node, std::list<g3dSceneNode*>& o_Nodes);
};

