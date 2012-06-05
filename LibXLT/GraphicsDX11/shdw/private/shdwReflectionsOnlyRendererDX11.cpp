/****************************************************************************\
**	shdwReflectionsOnlyRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwReflectionsOnlyRendererDX11.hpp"

#include "Core/ma/maPlane.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/Eff/effTexturedData.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPassBuffers.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/G3d/g3dTransparencySortDX11.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/shdw/shdwFrameBufferMgr.hpp"
#include "GraphicsDX11/shdw/shdwPassAlphaFill.hpp"
#include "GraphicsDX11/shdw/shdwPassNormals.hpp"
#include "GraphicsDX11/shdw/shdwPassZFill.hpp"
#include "GraphicsDX11/G2d/g2dWindowDX11.hpp"

//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D_ERROR(op_result, error_string) \
			if( op_result != D3D_OK )	\
			{	\
				g2dDX11Global::PrintDXError(op_result);	\
				DBG_ASSERT(false, error_string);	\
			}	\

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif


namespace
{
	// Stats
	int l_nNumTrianglesRendered = 0;
	g2dPFD l_hdrPFD(g2dPFD::e_RGBA16f, 64);

	g3dBlendStateMgr::BlendState* st_Initial = NULL;
	g3dBlendStateMgr::BlendState* st_CopyRestore = NULL;
	g3dBlendStateMgr::BlendState* st_AddBlend = NULL;
	g3dBlendStateMgr::BlendState* st_MulBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwReflectionsOnlyRendererDX11::shdwReflectionsOnlyRendererDX11()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwReflectionsOnlyRendererDX11::~shdwReflectionsOnlyRendererDX11()
{
	ReleaseResources();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwReflectionsOnlyRendererDX11::ReleaseResources()
{
	// force updates on next render call 
	m_pWindow = NULL;

	m_pFrameBuffer->ReleaseSurfaces();
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwReflectionsOnlyRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwReflectionsOnlyRendererDX11::Render" );

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;

	m_pWindow = dynamic_cast<g2dWindowDX11*>(i_pWindow);
	DBG_ASSERT( m_pWindow, "Render requires a g2dWindowDX11 render target" );

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// on entry, i expect a Clear()'ed and BeginScene()'d render target.
	// after i leave, i expect EndScene() and Present() to occur.
	
	// make sure temp surfaces are up-to-date
	CreateSurfaces(i_pWindow);
#ifdef NEW_AA_CODE
	matRenderTargetTexture* currentTarget = m_pFrameBuffer->HDRRenderTarget();
	m_pFrameBuffer->HDRRenderTargetTex()->SetDepthBuffer( m_pWindow->GetDepthStencilBuffer() );
#else
	g2dRenderTarget* currentTarget = NULL;
	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		// this target has its own depth buffer with it!
		currentTarget = m_pFrameBuffer->HDRAATarget();
	}
	else
	{
		// this target uses the backbuffer's depth buffer!
		currentTarget = m_pFrameBuffer->HDRRenderTargetTex()->GetRenderTargetAPI();
	}

	// init and clear main render surface
	i_pWindow->MakeCurrent();
	i_pWindow->ClearDepthStencil();
#endif

// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// gather scene graph elements into sorted lists
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();
	g3dSceneRenderUtil::enable_lights();
	//g3dSceneRenderUtil::update_world_data( layers[0]->GetRootNode() );
	m_SceneDatabase.Clear();
	m_SceneDatabase.TraverseLayer(i_fSimTime, layers[0], &i_Camera);

	/*const nodeCacheList& shadowNodes = m_SceneDatabase.GetShadowNodes();
	const nodeCacheList& nonShadowNodes = m_SceneDatabase.GetNonShadowNodes();
	const nodeCacheList& nonSolidNodes = m_SceneDatabase.GetNonSolidNodes();
	g3dRenderStateTraverser* stateTraverser = m_SceneDatabase.GetRenderStateCache();*/

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );


	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	//const g3dSceneNode* pNode = NULL;
	//g3dSceneNode::DrawStyle draw_style;
	//int i,n;

	// blend for ambient pass
	
	g3dBlendStateMgr::SetBlendState(st_Initial);

	// simple stupid draw loop
	g3dSingleLightRendering::SetDoAmbientEnvironmentPass(true);
	g3dDX11Util::AllowAdditiveChanges(false);

	int i,n;

	currentTarget->MakeCurrent();
	currentTarget->Clear(maFloatRGBA(0,0,0,0), true);

	g3dSingleLightRendering::SetDoIsolateReflections(true);

	const nodeCacheList& nonShadowNodes = m_SceneDatabase.GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		l_nNumTrianglesRendered += RenderNode(currentNode, m_SceneDatabase.GetRenderStateCache());
	}
	const nodeCacheList& shadowNodes = m_SceneDatabase.GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		l_nNumTrianglesRendered += RenderNode(currentNode, m_SceneDatabase.GetRenderStateCache());
	}
	const nodeCacheList& nonSolidNodes = m_SceneDatabase.GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		l_nNumTrianglesRendered += RenderNode(currentNode, m_SceneDatabase.GetRenderStateCache());
	}
	// transparent nodes
	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
	{
		g3dTransparencySortDX11* pTransNodes = m_SceneDatabase.GetTransparentNodes();
		const TranspNodeVector& tNodes = pTransNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tNodes.end();
		for (it = tNodes.begin(); it != end; ++it)
		{
			sNodePlusState NS;
			NS.m_pNode = it->m_pSceneNode;
			NS.m_StateCache = it->m_RenderStateCache;

			bool doRender = true;

			g3dFragment * frag = (g3dFragment*)NS.m_pNode->GetFragment();
			if ( frag )
			{
				matMaterial * mat = frag->GetMaterial();
				if ( mat )
				{
					doRender = !mat->GetBelongsToLightShaft();
				}
			}

			if ( doRender )
			{
				l_nNumTrianglesRendered += RenderNode(NS, m_SceneDatabase.GetRenderStateCache());
			}			
		}
	}

	g3dSingleLightRendering::SetDoIsolateReflections(false);

	// restore some state.
	g3dDX11Util::AllowAdditiveChanges(true);
	g3dSingleLightRendering::SetDoAmbientEnvironmentPass(false);
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();

	/*shdwPassAlphaFill alphaFillPass(currentTarget, (m_pFrameBuffer->HDRAATarget() != NULL), true); 
	alphaFillPass.SetSceneInfo(&m_SceneDatabase);
	l_nNumTrianglesRendered += alphaFillPass.Render(i_fSimTime);*/

	camPassBuffersData passBuffersData;
	i_Camera.GetPassBuffersParams(passBuffersData);
	float top, bottom, left, right;
	i_Camera.GetSubViewport(top, bottom, left, right);
	if ( passBuffersData.m_ReflBuffer && g3dPrefs::CurrentPrefs().m_bEnableReflection && g3dPassBuffers::GetDoingFileRefl() )
	{
		g3dDX11Util::BlendBuffers(currentTarget,passBuffersData.m_ReflBuffer,passBuffersData.m_ReflBlendOp, passBuffersData.m_ReflIntensity,
								  top, bottom, left, right);
	}

#ifdef NEW_AA_CODE
	if( !currentTarget->CopyWithResolve( m_pFrameBuffer->HDRRenderTargetTex(), false, true ))
	{
		DBG_TRACE( "HDR Target Texture Copy/Resolve failed!" );
	}

	CopyToBackBuf(m_pFrameBuffer->HDRRenderTargetTex());
#else
	if (m_pFrameBuffer->HDRAATarget())
	{
		m_pFrameBuffer->HDRAATarget()->Resolve(m_pFrameBuffer->HDRRenderTargetTex());
	}

	CopyToBackBuf(m_pFrameBuffer->HDRRenderTargetTex());

	// prepare zbuffer for viewer layers to follow.
	shdwPassZFill depthPrePass;
	depthPrePass.SetSceneInfo(&m_SceneDatabase);
	l_nNumTrianglesRendered += depthPrePass.Render(i_fSimTime);
#endif

	// Also add in the extra layers that are specific to this viewer
	for (int i = 0; i < i_ViewerLayers.size(); i++)
	{
		g3dRenderLayer rLayer;
		rLayer.Set(i_ViewerLayers[i], &i_Camera, i_pWindow);
		rLayer.PerFrameInit(i_fSimTime);
		l_nNumTrianglesRendered += rLayer.Render(i_fSimTime);
	}

	// reset d3d state
	g3dDX11Util::release_textures();

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

	// Display debug information for number of triangles
//	char num[64];
//	sprintf(num, "tris: %d", l_nNumTriangles);
//	i_pWindow->SetDebugInfo(2, num);
	D3DPERF_EndEvent();
	return l_nNumTrianglesRendered;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwReflectionsOnlyRendererDX11::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	m_pFrameBuffer->CreateSurfaces(i_pWindow, 0);

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);
	m_Width = w;
	m_Height = h;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwReflectionsOnlyRendererDX11::CopyToBackBuf(matRenderTargetTexture* pTex, bool bDoBlend, bool isRGB)
{
	g3dBlendStateMgr::SetBlendState(st_Initial);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

// Draw the high dynamic range scene texture to the low dynamic range
    // back buffer. 
	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)matShaderMgr::GetSpecialEffect(("HDRLighting.fx"));
	ID3DX11Effect* pEffect = i_pEffect->GetD3DXEffect();

	ID3DX11EffectTechnique* pTechnique = pEffect->GetTechniqueByName(isRGB?"SimpleCopyLDR":"SimpleCopyLumLDR");
    
	m_pWindow->MakeCurrent();
	int w,h;
	m_pWindow->GetDimensions(w,h);

	ID3D11ShaderResourceView* aRes = pTex->GetSurface();

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	D3DX11_TECHNIQUE_DESC techDesc;
	pTechnique->GetDesc( &techDesc );
	for( UINT uiPass = 0; uiPass < techDesc.Passes; ++uiPass)
	{
		pTechnique->GetPassByIndex(uiPass)->Apply(0, g2dDX11Global::g_pDeviceContext);
		g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, &aRes);
		g3dDX11Util::DrawFullScreenQuad( w,h );
	}

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_CopyRestore);

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

}

//--------------------------------------------------------------------
//	Sometimes we need to access the pre-buffer pixels that don't get 
//	drawn to a window.
//--------------------------------------------------------------------
g2dRenderTarget* shdwReflectionsOnlyRendererDX11::GetRawBuffer()
{
	return m_pFrameBuffer->HDRRenderTargetTex();
}


g2dPFD::PixelFormat shdwReflectionsOnlyRendererDX11::GetRawBufferPFD()
{
	return g2dPFD::e_RGBA16f;
}

int shdwReflectionsOnlyRendererDX11::RenderNode(const sNodePlusState& i_Node, g3dRenderStateTraverser* i_pRenderStateCache)
{
	int nTriangles = 0;
	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);
	i_pRenderStateCache->SetRenderState( i_Node.m_StateCache, 
		g3dSingleLightRendering::GetDoSingleLightRendering() );

	// rendering with a null environment will isolate the reflections only, since
	// the environment pass includes reflections.
	g3dAmbientEnvState nullEnv;
	nTriangles += g3dSceneRenderUtil::DrawNodeEnvironment(i_Node.m_pNode, nullEnv);

	return nTriangles;
}

void shdwReflectionsOnlyRendererDX11::InitStates()
{
	st_Initial = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );
	st_CopyRestore = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );
	st_AddBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );
	st_MulBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwReflectionsOnlyRendererDX11::CleanupStates()
{
	SAFE_DELETE( st_Initial );
	SAFE_DELETE( st_CopyRestore );
	SAFE_DELETE( st_AddBlend );
	SAFE_DELETE( st_MulBlend );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
	SAFE_DELETE( ds_Disable_NS );
}