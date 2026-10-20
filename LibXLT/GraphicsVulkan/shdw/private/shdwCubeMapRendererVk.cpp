/****************************************************************************\
**	shdwCubeMapRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_SHDW_CUBEMAPRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwCubeMapRendererVk.hpp"
#endif

#ifndef GVK_SHDW_SHADOWLAYERRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwShadowLayerRendererVk.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef EFF_REFLDATA_HPP
#include "Graphics/eff/effReflData.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/g3d/g3dLayer.hpp"
#endif
#ifndef G3D_PASSBUFFERS_HPP
#include "Graphics/g3d/g3dPassBuffers.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef GVK_MATCUBERENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matCubeRenderTargetTexture.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef SC_OBJECT_HPP
#include "Graphics/sc/scObject.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

#include <algorithm>

namespace gvk
{

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwCubeMapRendererVk::shdwCubeMapRendererVk()
:	m_pObject(NULL),
	m_pMaterial(NULL),
	m_bRenderPerFrame (false),
	m_bFirstRender (true)
{
	// commented out to workaround HDR reflection bug . FIX THIS.
//	if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_HDR)
//		m_pFaceRenderer = new shdwHDRRendererVk();
//	else


		m_pFaceRenderer = new shdwShadowLayerRendererVk();
}
shdwCubeMapRendererVk::shdwCubeMapRendererVk(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame)
:	m_pObject(i_Obj),
	m_pMaterial(i_Material),
	m_bRenderPerFrame (i_RenderPerFrame),
	m_bFirstRender (true)
{
	// commented out to workaround HDR reflection bug . FIX THIS.
//	if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_HDR)
//		m_pFaceRenderer = new shdwHDRRendererVk();
//	else


		m_pFaceRenderer = new shdwShadowLayerRendererVk();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwCubeMapRendererVk::~shdwCubeMapRendererVk()
{
	ReleaseResources();
	delete m_pFaceRenderer;
}

void shdwCubeMapRendererVk::ReleaseResources()
{
	m_pFaceRenderer->ReleaseResources();
}

bool shdwCubeMapRendererVk::IsEnabled()
{
	// we will always render once (m_bFirstRender)
	// and if m_bRenderPerFrame, then never disable.

	if (!m_bRenderPerFrame && !m_bFirstRender)
		return false;

	// if m_bRenderPerFrame, then we will render
	return true;
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwCubeMapRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
									const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
									float i_fSimTime )
{//PROFILE("Render");
	G3D_PROFILE_SCOPE("CubeMapRenderer");

	if (!g3dPrefs::CurrentPrefs().m_bEnableReflection || g3dPassBuffers::GetDoingFileRefl() )
	{
		// force a refresh the next time reflection rendering is enabled?
		//m_bFirstRender = true;
		return 0;
	}

	effReflectionMap* pData = &m_pMaterial->ReflectionData();
	DBG_ASSERT(m_pMaterial->GetHasReflection(), "shdwCubeMapRendererVk expected material to have effReflData");
	DBG_ASSERT(!pData->m_bIsPlanar, "shdwCubeMapRendererVk expected planar flag false");

	matCubeRenderTargetTexture* pCubeMap = dynamic_cast<matCubeRenderTargetTexture*>(i_pWindow);
	DBG_ASSERT(pCubeMap, "g3dCubeMapRenderer not using cube map target");

	int numTriangles = 0;

	maPoint3d target;

	// find all fragments of this material in this object.
	std::list<g3dSceneNode*> nodes;
	RecurseCollectNodes(m_pObject->GetBase(), nodes);

	// remember the visiblity state of the node.
	// then hide nodes and add to bounding box.
	std::list<bool> was_visible;
	std::list<bool> was_visible_in_refl;
	maAxisBox box;
	std::list<g3dSceneNode*>::iterator ni;
	for (ni = nodes.begin(); ni != nodes.end(); ni++)
	{
		was_visible.push_back((*ni)->GetRenderable());
		was_visible_in_refl.push_back((*ni)->GetRenderableInCubeMapReflection());

		(*ni)->SetRenderable(false);
		(*ni)->SetRenderableInCubeMapReflection(false);

		box.Union((*ni)->GetWorldBox());
	}

	// aggregate bounding box center is camera position.
	maPoint3d pos = box.GetCenter();

	// get the overall scene world bounds
	maAxisBox sceneBounds;
	int n = i_Scene.GetNumLayers();
	for (int i = 0; i < n; i++)
	{
		// do i need to call update_world_data here??
//		g3dSceneRenderUtil::update_world_data( i_Scene.GetLayer(i)->GetRootNode() );
		g3dSceneNode* node = i_Scene.GetLayer(i)->GetRootNode();
		if (node)
			sceneBounds.Union(node->GetWorldBox());
	}
	// now find the largest distance from the camera position to the edge of the bounds and add a small buffer.
	float farbounds[6] = {
		(sceneBounds.GetMaxX()-pos.m_X),
		(pos.m_X-sceneBounds.GetMinX()),
		(sceneBounds.GetMaxY()-pos.m_Y),
		(pos.m_Y-sceneBounds.GetMinY()),
		(sceneBounds.GetMaxZ()-pos.m_Z),
		(pos.m_Z-sceneBounds.GetMinZ())
	};
	// since pos is centered, if the given near plane is outside the bounds,
	// then we can tighten it to the bounds extent.
	float nearbounds[6] = {
		std::min(pData->m_NearPlane, (box.GetMaxX()-pos.m_X)),
		std::min(pData->m_NearPlane, (pos.m_X-box.GetMinX())),
		std::min(pData->m_NearPlane, (box.GetMaxY()-pos.m_Y)),
		std::min(pData->m_NearPlane, (pos.m_Y-box.GetMinY())),
		std::min(pData->m_NearPlane, (box.GetMaxZ()-pos.m_Z)),
		std::min(pData->m_NearPlane, (pos.m_Z-box.GetMinZ()))
	};
	static const maVector3d directions[] =
	{
		// eyeVec, upVec
		maVector3d(1,0,0),	maVector3d(0,1,0),
		maVector3d(-1,0,0),	maVector3d(0,1,0),
		maVector3d(0,1,0),	maVector3d(0,0,-1),
		maVector3d(0,-1,0),	maVector3d(0,0,1),
		maVector3d(0,0,1),	maVector3d(0,1,0),
		maVector3d(0,0,-1),	maVector3d(0,1,0),
	};

	camCamera camera;
	// The projection matrix has a FOV of 90 degrees and asp ratio of 1
	camera.SetClip(0.1f, 10000.0f);
	camera.SetAspect(1.0f);
	camera.SetFOV(90);

	// RENDER 6 FACES!!!

	for (int i = 0; i < 6; i++)
	{
		target = pos + directions[i*2];
		camera.LookAt(pos,
			target,
			directions[i*2+1]);

		camera.SetClip(nearbounds[i] + 0.01f, farbounds[i] + 1.0f);

		pCubeMap->SetFaceTarget(i);

		g3dSingleLightRendering::SetDoCubeReflectionGen(true);
		numTriangles += m_pFaceRenderer->Render(pCubeMap, camera, i_Scene, i_ViewerLayers, i_fSimTime);
		g3dSingleLightRendering::SetDoCubeReflectionGen(false);
	}

	vkdImage* pTex = g3dVkTextureUtil::GetImage(pData->m_ReflectionMap);
	if (pTex)
		pTex->GenerateMips( vkdGlobal::Context() );

//	(DX11's _DEBUG dump of the first map's faces to face<n>.png in the
//	working directory is left out)

	// restore visibility of nodes
	std::list<bool>::iterator bi, ri;
	for (ni = nodes.begin(), bi = was_visible.begin(), ri = was_visible_in_refl.begin();
		ni != nodes.end();
		ni++, bi++, ri++)
	{
		(*ni)->SetRenderable((*bi));
		(*ni)->SetRenderableInCubeMapReflection((*ri));
	}

	m_bFirstRender = false;
	return numTriangles;
}

//--------------------------------------------------------------------
//	Give this renderer a hint as to what objects not to render, and where
//	to position the camera.
//--------------------------------------------------------------------
void shdwCubeMapRendererVk::SetSceneObject(scObject* i_Obj, matMaterial* i_Material)
{
	m_pObject = i_Obj;
	m_pMaterial = i_Material;
}

//--------------------------------------------------------------------
// collect all fragments that match the given material
//--------------------------------------------------------------------
void shdwCubeMapRendererVk::RecurseCollectNodes(g3dSceneNode* i_Node, std::list<g3dSceneNode*>& o_Nodes)
{
	g3dFragment* fragment = i_Node->GetFragment();
	if (fragment)
	{
		if (fragment->GetMaterial() == m_pMaterial)
			o_Nodes.push_back(i_Node);
	}

	int nChildren = i_Node->GetChildren().size();
	for (int i = 0; i < nChildren; i++)
	{
		RecurseCollectNodes(i_Node->GetChild(i), o_Nodes);
	}
}

}
