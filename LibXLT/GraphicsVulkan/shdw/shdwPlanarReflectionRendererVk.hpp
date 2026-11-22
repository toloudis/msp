/****************************************************************************\
**	shdwPlanarReflectionRendererVk.hpp
**
**	The shdwPlanarReflectionRendererVk renders the scene from a light's point of
**	view in order to get the depth map info for shadow mapping.
**
**	GraphicsVulkan port of GraphicsDX11's shdwPlanarReflectionRendererDX11:
**	the scene mirrored about a reflector's plane, drawn by the simple
**	renderer (shdwShadowLayerRendererVk) into the reflector material's
**	reflection map, whose mips are then generated.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_SHDW_PLANARREFLECTIONRENDERERVK_HPP
#error shdwPlanarReflectionRendererVk.hpp multiply included
#endif
#define GVK_SHDW_PLANARREFLECTIONRENDERERVK_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif

#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif

#ifndef MA_PLANE_HPP
#include "Core/ma/maPlane.hpp"
#endif

#include <list>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class g3dSceneNode;
class maPlane;
class matMaterial;
class scObject;

namespace gvk
{

class shdwPlanarReflectionRendererVk : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwPlanarReflectionRendererVk();
	shdwPlanarReflectionRendererVk(scObject* i_Obj, matMaterial* i_Material);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~shdwPlanarReflectionRendererVk();

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
	//	Give this renderer a hint as to what objects not to render, and where
	//	to position the camera.
	//--------------------------------------------------------------------
	void SetSceneObject(scObject* i_Obj, matMaterial* i_Material);

protected:
	g3dSceneRenderer* m_pFaceRenderer;
	scObject* m_pObject;
	matMaterial* m_pMaterial;

	//--------------------------------------------------------------------
	// collect all nodes that match the given material
	//--------------------------------------------------------------------
	void RecurseCollectNodes(g3dSceneNode* i_Node, std::list<g3dSceneNode*>& o_Nodes);

//	maMatrix4x4 ClipProjectionMatrix(maMatrix4x4 & matView, maMatrix4x4 & matProj,
//		maPlane & clip_plane);
	void SetupClipPlane(
		const camCamera& i_Camera,
		maVector3d& v,
		maPoint3d& p,
		maMatrix4x4& camMat);
	void UnsetClipPlane();
};

}
