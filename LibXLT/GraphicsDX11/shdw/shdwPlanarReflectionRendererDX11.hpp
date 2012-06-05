/****************************************************************************\
**	shdwPlanarReflectionRendererDX11.hpp
**
**	The shdwPlanarReflectionRendererDX11 renders the scene from a light's point of 
**	view in order to get the depth map info for shadow mapping.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_PLANARREFLECTIONRENDERERDX11_HPP
#error shdwPlanarReflectionRendererDX11.hpp multiply included
#endif
#define SHDW_PLANARREFLECTIONRENDERERDX11_HPP

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

class shdwPlanarReflectionRendererDX11 : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwPlanarReflectionRendererDX11();
	shdwPlanarReflectionRendererDX11(scObject* i_Obj, matMaterial* i_Material);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~shdwPlanarReflectionRendererDX11();

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
	bool m_Save;

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

