/****************************************************************************\
**	shdwPlanarReflectionRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/shdw/shdwPlanarReflectionRendererDX11.hpp"

#include "GraphicsDX11/shdw/shdwHDRRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwShadowLayerRendererDX11.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effReflData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPassBuffers.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/sc/scObject.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDrawStyleUtilDX11.hpp"

//#include c_g2dD3DX11MATH_H

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPlanarReflectionRendererDX11::shdwPlanarReflectionRendererDX11()
:	m_pObject(NULL),
	m_pMaterial(NULL),
	m_Save (true)
{
	// commented out to workaround HDR reflection bug . FIX THIS.
//	if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_HDR)
//		m_pFaceRenderer = new shdwHDRRendererDX11();
//	else
		m_pFaceRenderer = new shdwShadowLayerRendererDX11();
}
shdwPlanarReflectionRendererDX11::shdwPlanarReflectionRendererDX11(scObject* i_Obj, matMaterial* i_Material)
:	m_pObject(i_Obj),
	m_pMaterial(i_Material),
	m_Save (true)
{
	// commented out to workaround HDR reflection bug . FIX THIS.
//	if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_HDR)
//		m_pFaceRenderer = new shdwHDRRendererDX11();
//	else
		m_pFaceRenderer = new shdwShadowLayerRendererDX11();
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPlanarReflectionRendererDX11::~shdwPlanarReflectionRendererDX11()
{
	ReleaseResources();
	delete m_pFaceRenderer;
}

void shdwPlanarReflectionRendererDX11::ReleaseResources()
{
	m_pFaceRenderer->ReleaseResources();
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwPlanarReflectionRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
									const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
									float i_fSimTime )
{//PROFILE("Render");

	if (!g3dPrefs::CurrentPrefs().m_bEnableReflection || g3dPassBuffers::GetDoingFileRefl()  )
	{
		// force a refresh the next time reflection rendering is enabled?
		// m_bFirstRender = true;
		return 0;
	}

	effReflectionMap* pData = &m_pMaterial->ReflectionData();
	DBG_ASSERT(m_pMaterial->GetHasReflection(), "shdwPlanarReflectionRendererDX11 expected material to have effReflData");
	DBG_ASSERT(pData->m_bIsPlanar, "shdwPlanarReflectionRendererDX11 expected planar reflection flag in material");

	int numTriangles = 0; 

	// find all fragments of this material in this object.
	std::list<g3dSceneNode*> nodes;
	RecurseCollectNodes(m_pObject->GetBase(), nodes);
	if (nodes.size() == 0)
		return 0;

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPlanarReflectionRendererDX11::Render" );

	// extract one triangle to define the plane of the reflector.
	g3dSceneNode* node = *(nodes.begin());
	
	g3dFragment* frag = node->GetFragment();
	maPoint3d facepts[3];
	maVector3d facenorms[3];
	maVector3d p, v;

	bool got_face = frag->GetFaceInfo(0, facepts, facenorms);
	DBG_ASSERT(got_face == true, "Renderer could not get plane for planar reflection");
	// point and normal define plane (object space)
	p += facepts[0];
	p += facepts[1];
	p += facepts[2];
	p *= 0.333333f;
	// calculate geometric face normal rather than using vertex normals.
	v = (facepts[1] - facepts[0]).Cross(facepts[2]-facepts[1]);
	v.Normalize();
	
	// bring into world space
	node->GetTotalTransform().Transform(p);
	maMatrix4x4 invtransp = node->GetTotalTransform();
	invtransp.Invert();
	invtransp.Transpose();
	invtransp.TransformDir(v);

	v.Normalize();
	// try to round the normal vector to an axis if it's close.
	if ((v.m_X) > 0.99f)
		v = maVector3d(1,0,0);
	else if ((v.m_X) < -0.99f)
		v = maVector3d(-1,0,0);
	else if ((v.m_Y) > 0.99f)
		v = maVector3d(0,1,0);
	else if ((v.m_Y) < -0.99f)
		v = maVector3d(0,-1,0);
	else if ((v.m_Z) > 0.99f)
		v = maVector3d(0,0,1);
	else if ((v.m_Z) < -0.99f)
		v = maVector3d(0,0,-1);

	float pdotn = v*p;
	// defaults to identity
	maMatrix4x4 mrefl;
	mrefl(0,0) = 1 - 2.0f * v.m_X * v.m_X;
	mrefl(1,1) = 1 - 2.0f * v.m_Y * v.m_Y;
	mrefl(2,2) = 1 - 2.0f * v.m_Z * v.m_Z;
	mrefl(3,3) = 1;
	mrefl(0,1) = -2.0f * v.m_X * v.m_Y;
	mrefl(0,2) = -2.0f * v.m_X * v.m_Z;
	mrefl(1,2) = -2.0f * v.m_Y * v.m_Z;
	mrefl(1,0) = -2.0f * v.m_X * v.m_Y;
	mrefl(2,0) = -2.0f * v.m_X * v.m_Z;
	mrefl(2,1) = -2.0f * v.m_Y * v.m_Z;
	mrefl(0,3) = 2.0f * pdotn * v.m_X;
	mrefl(1,3) = 2.0f * pdotn * v.m_Y;
	mrefl(2,3) = 2.0f * pdotn * v.m_Z;
	mrefl.Transpose();

	camCamera reflCam(i_Camera);

	// replace camera matrix with 
	maMatrix4x4 camMat;
	i_Camera.GetCameraMatrix(camMat);
	reflCam.SetCameraMatrix(mrefl * camMat);

	SetupClipPlane(reflCam, v, p, mrefl*camMat);

	// remember the visiblity state of the node.
	// then hide nodes.
	std::list<bool> was_visible;
	std::list<bool> was_visible_in_refl;
	std::list<g3dSceneNode*>::iterator ni;
	for (ni = nodes.begin(); ni != nodes.end(); ni++)
	{
		was_visible.push_back((*ni)->GetRenderable());
		was_visible_in_refl.push_back((*ni)->GetRenderableInPlanarReflection());
		(*ni)->SetRenderable(false);
		(*ni)->SetRenderableInPlanarReflection(false);
	}

	g3dSingleLightRendering::SetDoPlaneReflectionGen(true);
	numTriangles += m_pFaceRenderer->Render(i_pWindow, reflCam, i_Scene, i_ViewerLayers, i_fSimTime);
	g3dSingleLightRendering::SetDoPlaneReflectionGen(false);

	// restore post-reflection mode polygon winding order
	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	ID3D11ShaderResourceView* pTex = g3dDX11TextureUtil::GetD3DTexture(pData->m_ReflectionMap);
	if (pTex)
		g2dDX11Global::g_pDeviceContext->GenerateMips( pTex );

#ifdef _DEBUG
	if (m_Save)
	{
		fsLocator loc;
		char fname[256];
		sprintf(fname, "rt_%08x.dds", i_pWindow);
		loc.Push(fname);

		matRenderTargetTexture* pMap = dynamic_cast<matRenderTargetTexture*>(i_pWindow);
		matTextureMgr::SaveTextureToFile(pMap, loc);
		m_Save = false;
	}
#endif

	UnsetClipPlane();
	//reflCam.SetCameraMatrix(camMat);

	// restore visibility of nodes
	std::list<bool>::iterator bi, ri;
	for (ni = nodes.begin(), bi = was_visible.begin(), ri = was_visible_in_refl.begin();
		ni != nodes.end(); 
		ni++, bi++, ri++)
	{
		(*ni)->SetRenderable((*bi));
		(*ni)->SetRenderableInPlanarReflection((*ri));
	}
	D3DPERF_EndEvent();
	return numTriangles;
}

//--------------------------------------------------------------------
//	Give this renderer a hint as to what objects not to render, and where
//	to position the camera.
//--------------------------------------------------------------------
void shdwPlanarReflectionRendererDX11::SetSceneObject(scObject* i_Obj, matMaterial* i_Material) 
{
	m_pObject = i_Obj; 
	m_pMaterial = i_Material;
}

//--------------------------------------------------------------------
// collect all fragments that match the given material
//--------------------------------------------------------------------
void shdwPlanarReflectionRendererDX11::RecurseCollectNodes(g3dSceneNode* i_Node, std::list<g3dSceneNode*>& o_Nodes)
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

// v and p come in world space (pre-reflection)
void shdwPlanarReflectionRendererDX11::SetupClipPlane(const camCamera& i_Camera, maVector3d& v, maPoint3d& p,
													 maMatrix4x4& worldToCamera)
{
	g3dSceneGlobal::g_ClipPlane = maVector4d( v, -(v*p) );
}

void shdwPlanarReflectionRendererDX11::UnsetClipPlane()
{
	g3dSceneGlobal::g_ClipPlane = maVector4d( 0,0,0,1 );
}
