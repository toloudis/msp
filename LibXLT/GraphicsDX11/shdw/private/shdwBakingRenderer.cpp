/****************************************************************************\
**	shdwBakingRenderer.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwBakingRenderer.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/eff/effBakeData.hpp"
#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/g2d/g2dRenderTarget.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/G3d/g3dPrefs.hpp"
//#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/bump/bumpBumpRenderer.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/g3d/g3dTransparencySortDX11.hpp"
#include "GraphicsDX11/mat/matDX11GlobalWin.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/shdw/shdwPassAmbient.hpp"
#include "GraphicsDX11/shdw/shdwPassAOVolumes.hpp"
#include "GraphicsDX11/shdw/shdwPassEnvironment.hpp"
#include "GraphicsDX11/shdw/shdwPassLit.hpp"
#include "GraphicsDX11/shdw/shdwPassLPVGI.hpp"
#include "GraphicsDX11/shdw/shdwPassNormals.hpp"
#include "GraphicsDX11/shdw/shdwPassZFill.hpp"
#include "GraphicsDX11/G3d/g3dDrawStyleUtilDX11.hpp"

//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D_ERROR(op_result, error_string) \
			if( op_result != D3D_OK )	\
			{	\
				g2dDX11Global::PrintDXError(op_result);	\
				DBG_ASSERT(false, error_string);	\
			}	\

namespace
{
	// do uv seam flooding. pretty much has to be true for decent bake result.
	bool l_FixupSeams = true;

	// bake mode
	bool l_bIsSaveAndReplace = true;

	// default res if no diffuse texture for fragment.
	int l_TexRes = 512;

	// overlapping uv threshold
	const float l_overlapThres = 2.0f;

	// output texture format
	std::string l_TexFormat("dds");
	
	// color to clear bake texture. alpha=0 is important.
	maFloatRGBA l_ClearInit(0,0,0,0);
	
	// base filename if no diffuse texture for fragment.
	std::string l_BaseName("Texture");

	// temprary bake texture
	matTexture* l_BakeTarget = NULL;

	// Texture reduction (powers of 2) to apply to texture sizes
	// when determining size of baked textures.
	int l_TextureReduce = 0;

	g2dPFD l_hdrPFD(g2dPFD::e_Color, 32);

	g3dBlendStateMgr::BlendState* st_MulBlend = NULL;
	g3dBlendStateMgr::BlendState* st_AddBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Write_NS = NULL;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwBakingRenderer::shdwBakingRenderer()
{
	m_CurrentNode = 0;
	m_pHDRAATarget = NULL;
	m_pPassLPVGI = NULL;
	m_GIScratchBuffer = NULL;
	m_GIDepthBuffer = NULL;
	m_AOBuffer = NULL;
	m_AOPositionBuffer = NULL;
	m_AONormalBuffer = NULL;
	m_AODepthBuffer = NULL;
//	shdwPassNormals::InitializeMaterial();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwBakingRenderer::~shdwBakingRenderer()
{
}

//--------------------------------------------------------------------
//	Init scene for baking, return number of nodes to bake
//--------------------------------------------------------------------
int shdwBakingRenderer::Init(const g2dPFD& i_PFD, const g3dScene* i_pScene, const fsLocator& i_OutputPath,
							 float i_fSimTime, const camCamera& i_Camera, bool i_bIsSaveAndReplace,
							 const std::string& i_OutputFormat, int i_Res, int i_TextureReduce)
{
	D3DPERF_SetMarker( D3DCOLOR_RGBA(255,0,0,255), L"shdwBakingRenderer::Init" );

	shdwPassNormals::InitializeMaterial();

	m_OutputPathLocator = i_OutputPath;
	m_PFD = i_PFD;
	m_CurrentNode = 0;
	l_TexFormat = i_OutputFormat;
	l_TexRes = i_Res;
	l_TextureReduce = i_TextureReduce; // should be member variable?
	l_bIsSaveAndReplace = i_bIsSaveAndReplace;
	m_pScene = const_cast<g3dScene*>(i_pScene);
	m_Camera = i_Camera;

	// Initialize the triangle count
	int nTriangles = 0;

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// gather scene graph elements into sorted lists
	const std::vector<g3dLayer*> &layers = i_pScene->GetLayers();
	g3dSceneRenderUtil::enable_lights();
	m_SceneDatabase.Clear();
	m_SceneDatabase.TraverseLayer(i_fSimTime, layers[0], &i_Camera, false);

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Write_NS );

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	int n0,n1,n2,n3;
	const nodeCacheList& nonShadowNodes = m_SceneDatabase.GetNonShadowNodes();
	n0 = nonShadowNodes.size();
	const nodeCacheList& shadowNodes = m_SceneDatabase.GetShadowNodes();
	n1 = shadowNodes.size();
	const nodeCacheList& nonSolidNodes = m_SceneDatabase.GetNonSolidNodes();
	n2 = nonSolidNodes.size();
	g3dTransparencySortDX11* transparencySort = m_SceneDatabase.GetTransparentNodes();
	const TranspNodeVector& transparentNodes = transparencySort->GetTransparentNodes();
	n3 = transparentNodes.size();

	// Preparing GI params
	m_pScene->GetSSGISettings(m_giParam);
	if (g3dPrefs::CurrentPrefs().m_bGIInvalid)
	{
		g2dPFD depthPFD(g2dPFD::e_Float32, 32);
		m_GIScratchBuffer = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(l_TexRes,l_TexRes,false,&m_PFD));
		m_GIDepthBuffer = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(l_TexRes, l_TexRes,
			false, &depthPFD, false, true, matTextureMgr::e_Framebuffer));
	}

	/*m_pScene->GetSSAOSettings(m_aoParam);
	if (g3dPrefs::CurrentPrefs().m_bAOInvalid)
	{
		g2dPFD depthPFD(g2dPFD::e_Float32, 32);
		m_AOBuffer = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(l_TexRes,l_TexRes,false,&m_PFD));
		m_AOPositionBuffer = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(l_TexRes,l_TexRes,false,&m_PFD));
		m_AONormalBuffer = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(l_TexRes,l_TexRes,false,&m_PFD));
		m_AODepthBuffer = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(l_TexRes, l_TexRes,
			false, &depthPFD, false, true, matTextureMgr::e_Framebuffer));
	}*/

	return n0+n1+n2+n3;
}

//--------------------------------------------------------------------
//	CleanUp after baking
//--------------------------------------------------------------------
int shdwBakingRenderer::CleanUp()
{
	if (m_pPassLPVGI)
	{
		delete m_pPassLPVGI;
		m_pPassLPVGI = NULL;
	}

	if (m_GIScratchBuffer)
	{
		matTextureMgr::ReleaseTexture(m_GIScratchBuffer);
		m_GIScratchBuffer = NULL;
	}

	if (m_GIDepthBuffer)
	{
		matTextureMgr::ReleaseTexture(m_GIDepthBuffer);
		m_GIScratchBuffer = NULL;
	}

	//if (m_AOBuffer)
	//{
	//	matTextureMgr::ReleaseTexture(m_AOBuffer);
	//	m_AOBuffer = NULL;
	//}
	//if (m_AOPositionBuffer)
	//{
	//	matTextureMgr::ReleaseTexture(m_AOPositionBuffer);
	//	m_AOPositionBuffer = NULL;
	//}
	//if (m_AONormalBuffer)
	//{
	//	matTextureMgr::ReleaseTexture(m_AONormalBuffer);
	//	m_AONormalBuffer = NULL;
	//}
	//if (m_AODepthBuffer)
	//{
	//	matTextureMgr::ReleaseTexture(m_AODepthBuffer);
	//	m_AODepthBuffer = NULL;
	//}

	return 0;
}

//--------------------------------------------------------------------
//	Bake a node and return true if there are any left to bake.
//--------------------------------------------------------------------
bool shdwBakingRenderer::BakeNextNode(std::map<const g3dFragment*, std::string> &io_TextureNameMap)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwBakingRenderer::BakeNextNode" );

	bool moretodo = true;

	int n0,n1,n2,n3;
	const nodeCacheList& nonShadowNodes = m_SceneDatabase.GetNonShadowNodes();
	n0 = nonShadowNodes.size();
	const nodeCacheList& shadowNodes = m_SceneDatabase.GetShadowNodes();
	n1 = shadowNodes.size();
	const nodeCacheList& nonSolidNodes = m_SceneDatabase.GetNonSolidNodes();
	n2 = nonSolidNodes.size();
	g3dTransparencySortDX11* transparencySort = m_SceneDatabase.GetTransparentNodes();
	const TranspNodeVector& transparentNodes = transparencySort->GetTransparentNodes();
	n3 = transparentNodes.size();

	//force no culling
	g3dDX11Util::SetCullEnable( false );

	g3dRenderStateTraverser* renderStateCache = m_SceneDatabase.GetRenderStateCache();
	g3dSingleLightRendering::SetDoBaking(true);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Write_NS );

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		// this target has its own depth buffer with it!
		InitMultisample();
	}

	int i = m_CurrentNode;
	if (m_CurrentNode < n0)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		BakeNonShadowNode(currentNode, renderStateCache, io_TextureNameMap);
	}
	else if (m_CurrentNode < n0+n1)
	{
		i -= n0;
		const sNodePlusState& currentNode = shadowNodes[i];
		BakeShadowNode(currentNode, renderStateCache, io_TextureNameMap);
	}
	else if (m_CurrentNode < n0+n1+n2)
	{
		i -= n0+n1;
		const sNodePlusState& currentNode = nonSolidNodes[i];
		BakeNonSolidNode(currentNode, renderStateCache, io_TextureNameMap);
	}
	else if (m_CurrentNode < n0+n1+n2+n3)
	{
		i -= n0+n1+n2;
		sNodePlusState transpNode;
		transpNode.m_pNode = transparentNodes[i].m_pSceneNode;
		transpNode.m_StateCache = transparentNodes[i].m_RenderStateCache;
		BakeShadowNode(transpNode, renderStateCache, io_TextureNameMap);
	}
	else
	{
		// index exceeded nodes to render! we're done!
		moretodo = false;
	}

	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		CleanupMultisample();
	}

	g3dSingleLightRendering::SetDoBaking(false);

	//restore culling
	g3dDX11Util::SetCullEnable( true );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );
	D3DPERF_EndEvent();

	m_CurrentNode++;
	return moretodo;
}

//--------------------------------------------------------------------
//	Set the output directory
//--------------------------------------------------------------------
void shdwBakingRenderer::SetOutputDirectory(const fsLocator& i_OutputDir)
{
	m_OutputPathLocator = i_OutputDir;
}

//--------------------------------------------------------------------
//	Bake a node and return num triangles rendered.
//--------------------------------------------------------------------
int shdwBakingRenderer::BakeNonShadowNode(const sNodePlusState& i_Node, 
										  g3dRenderStateTraverser* i_pRenderStateCache,
										  std::map<const g3dFragment*, std::string> &io_TextureNameMap)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwBakingRenderer::BakeNonShadowNode" );
	const g3dSceneNode* pNode = i_Node.m_pNode;
	g3dFragment* pFragment = const_cast<g3dFragment*>(pNode->GetFragment());
	/*if (l_bIsSaveAndReplace)
		pFragment->SetUseBakedTexture(true);*/
	/*if (!pFragment->GetReceivesBake())
		return 0;*/

	int texx=l_TexRes,texy=l_TexRes;
	std::string name;
	GetTextureInfo(pNode, pFragment, texx, texy, name);

	matRenderTargetTexture* pTexture = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(texx,texy,false,&m_PFD));
	g2dRenderTarget* pTarget = pTexture->GetRenderTargetAPI();
	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		pTarget = m_pHDRAATarget;
	}

	pTarget->BeginScene();
	pTarget->MakeCurrent();
	pTarget->Clear(l_ClearInit);

	int nTriangles = 0;
	if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_HDR)
	{
		//	nTriangles += shdwPassAmbient::RenderNode(i_Node, i_pRenderStateCache);
		g3dDX11Util::AllowAdditiveChanges(false);

		if (g3dPrefs::CurrentPrefs().m_bEnableEnvironment)
			nTriangles += shdwPassEnvironment::RenderNode(i_Node, i_pRenderStateCache);

		g3dDX11Util::AllowAdditiveChanges(true);
	}
	else if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_Normals)
	{
		g3dBlendStateMgr::SetBlendState(st_MulBlend);
		nTriangles += shdwPassNormals::RenderNode(i_Node, true);
	}

	g3dBlendStateMgr::SetBlendState(st_AddBlend);

	// non-shadow node means no "lit" pass.

	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		m_pHDRAATarget->Resolve(pTexture);
	}

	FixupSeams(pTexture, texx, texy);

	pTarget->EndScene();

	fsLocator f = m_OutputPathLocator;
	f.Push(name.c_str());
	matTextureMgr::SaveTextureToFile(pTexture, f);

	matTextureMgr::ReleaseTexture(pTexture);

	D3DPERF_EndEvent();
	return nTriangles;
}

//--------------------------------------------------------------------
//	Bake a node and return num triangles rendered.
//--------------------------------------------------------------------
int shdwBakingRenderer::BakeShadowNode(const sNodePlusState& i_Node, 
									   g3dRenderStateTraverser* i_pRenderStateCache,
									   std::map<const g3dFragment*, std::string> &io_TextureNameMap)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwBakingRenderer::BakeShadowNode" );

	g3dSceneNode* pNode = const_cast<g3dSceneNode*>(i_Node.m_pNode);
	g3dFragment* pFragment = const_cast<g3dFragment*>(pNode->GetFragment());
	float overlapFactor = pFragment->GetUVOverlapFactor();
	if (overlapFactor >= l_overlapThres)
	{
		pNode->SetActiveInRenderLayer(false);
		DBG_WARNING("UV overlapping has detected, cannot bake this fragment:" << i_Node.m_pNode->GetName());
		return 0;
	}
	/*if (l_bIsSaveAndReplace)
		pFragment->SetUseBakedTexture(true);*/
	/*if (!pFragment->GetReceivesBake())
		return 0;*/

	int texx=l_TexRes,texy=l_TexRes;
	std::string name;
	GetTextureInfo(pNode, pFragment, texx, texy, name);

	matRenderTargetTexture* pTexture = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(texx,texy,false,&m_PFD));
	g2dRenderTarget* pTarget = pTexture->GetRenderTargetAPI();
	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		pTarget = m_pHDRAATarget;
	}

	if (g3dPrefs::CurrentPrefs().m_bGIInvalid && g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_HDR)
	{
		if (!m_pPassLPVGI)
		{
			m_pPassLPVGI = new shdwPassLPVGI(m_giParam, m_GIScratchBuffer, m_GIDepthBuffer,
				pTarget, &m_Camera, m_pScene, true);
			m_pPassLPVGI->SetSceneInfo(&m_SceneDatabase);
			m_pPassLPVGI->Render(0.0f);
		}
	}

	g3dSceneRenderUtil::SetViewingTransforms(m_Camera);
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Write_NS );
	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	//g2dRenderTarget* pTarget = io_pTexture->GetRenderTargetAPI();
	pTarget->BeginScene();
	pTarget->MakeCurrent();
	pTarget->Clear(l_ClearInit);

	shdwPassZFill depthPrePass(true);
	depthPrePass.SetSceneInfo(&m_SceneDatabase);
	depthPrePass.RenderOneNode(0.0f, i_Node);

	int nTriangles = 0;
	if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_HDR)
	{
		//	nTriangles += shdwPassAmbient::RenderNode(i_Node, i_pRenderStateCache);
		g3dDX11Util::AllowAdditiveChanges(false);
		if (g3dPrefs::CurrentPrefs().m_bEnableEnvironment)
			nTriangles += shdwPassEnvironment::RenderNode(i_Node, i_pRenderStateCache);

		g3dDX11Util::AllowAdditiveChanges(false);

		if (g3dPrefs::CurrentPrefs().m_bEnableLitPass)
			nTriangles += shdwPassLit::RenderNode(i_Node, g3dLightMgrDX11::Implementation()->GetLights( ), i_pRenderStateCache);

		if (g3dPrefs::CurrentPrefs().m_bGIInvalid)
		{
			m_pPassLPVGI->SetRenderTarget(pTarget);
			m_pPassLPVGI->DeferRenderForBaking(i_Node);
		}

		/*if (g3dPrefs::CurrentPrefs().m_bAOInvalid)
		{
			shdwPassAOVolumes aoPass(pTarget, m_AOBuffer, m_AOPositionBuffer, 
				m_AONormalBuffer, 
				m_AODepthBuffer, 
				&m_Camera, m_aoParam);
			aoPass.SetSceneInfo(&m_SceneDatabase);
			aoPass.BakeRender(0.0f, i_Node);
		}*/
		g3dDX11Util::AllowAdditiveChanges(true);
	}
	else if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_Normals)
	{
		g3dBlendStateMgr::SetBlendState(st_MulBlend);
		nTriangles += shdwPassNormals::RenderNode(i_Node, true);
	}

	g3dBlendStateMgr::SetBlendState(st_AddBlend);

	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		m_pHDRAATarget->Resolve(pTexture);
	}

	FixupSeams(pTexture, texx, texy);

	pTarget->EndScene();

	fsLocator f = m_OutputPathLocator;
	f.Push(name.c_str());
	matTextureMgr::SaveTextureToFile(pTexture, f);
	
	matTextureMgr::ReleaseTexture(pTexture);

	D3DPERF_EndEvent();
	return nTriangles;
}

//--------------------------------------------------------------------
//	Bake a node and return num triangles rendered.
//--------------------------------------------------------------------
int shdwBakingRenderer::BakeNonSolidNode(const sNodePlusState& i_Node, 
										 g3dRenderStateTraverser* i_pRenderStateCache,
										 std::map<const g3dFragment*, std::string> &io_TextureNameMap)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwBakingRenderer::BakeNonSolidNode" );

	const g3dSceneNode* pNode = i_Node.m_pNode;
	g3dFragment* pFragment = const_cast<g3dFragment*>(pNode->GetFragment());
	/*if (l_bIsSaveAndReplace)
		pFragment->SetUseBakedTexture(true);*/
	/*if (!pFragment->GetReceivesBake())
		return 0;*/

	int texx=l_TexRes,texy=l_TexRes;
	std::string name;
	GetTextureInfo(pNode, pFragment, texx, texy, name);

	matRenderTargetTexture* pTexture = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(texx,texy,false,&m_PFD));
	g2dRenderTarget* pTarget = pTexture->GetRenderTargetAPI();
	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		pTarget = m_pHDRAATarget;
	}
	pTarget->BeginScene();
	pTarget->MakeCurrent();
	pTarget->Clear(l_ClearInit);

	int nTriangles = 0;
	if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_HDR)
	{
		//	nTriangles += shdwPassAmbient::RenderNode(i_Node, i_pRenderStateCache);
		g3dDX11Util::AllowAdditiveChanges(false);
		if (g3dPrefs::CurrentPrefs().m_bEnableEnvironment)
			nTriangles += shdwPassEnvironment::RenderNode(i_Node, i_pRenderStateCache);

		g3dDX11Util::AllowAdditiveChanges(true);
		// non-shadow node means no "lit" pass.
	}
	else if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_Normals)
	{
		g3dBlendStateMgr::SetBlendState(st_MulBlend);
		nTriangles += shdwPassNormals::RenderNode(i_Node, true);
	}
	g3dBlendStateMgr::SetBlendState(st_AddBlend);
	

	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		m_pHDRAATarget->Resolve(pTexture);
	}

	FixupSeams(pTexture, texx, texy);

	pTarget->EndScene();

	fsLocator f = m_OutputPathLocator;
	f.Push(name.c_str());
	matTextureMgr::SaveTextureToFile(pTexture, f);

	matTextureMgr::ReleaseTexture(pTexture);

	D3DPERF_EndEvent();
	return nTriangles;
}

//--------------------------------------------------------------------
//	Generate baking texture's filename and resolution 
//--------------------------------------------------------------------
void shdwBakingRenderer::GetTextureInfo(const g3dSceneNode* i_pNode, 
										const g3dFragment* i_pFrag, 
										int& o_TexX, 
										int& o_TexY, 
										std::string& o_Name)
{
	// Get material for fragment
	const matMaterial *pMaterial = i_pFrag->GetMaterial();
	if (i_pNode->GetMaterial())
		pMaterial = i_pNode->GetMaterial();
	
	o_TexX = l_TexRes;
	o_TexY = l_TexRes;

	std::string baseName(i_pNode->GetBakeName());
	// output name is independant with texture names
	//if (pMaterial && pMaterial->GetEffectData())
	//{
	//	// Set texture res from the diffuse texture from the material
	//	std::vector<matTexture*> textures;
	//	pMaterial->GetEffectData()->GetTextures(textures);
	//	if (!textures.empty())
	//	{
	//		o_TexX = textures[0]->GetWidth();
	//		o_TexY = textures[0]->GetHeight();
	//	}

	//	std::vector<std::string> names;
	//	pMaterial->GetEffectData()->GetTextureNames(names);
	//	if (!names.empty())
	//	{
	//		baseName = names[0];
	//		// strip extension
	//		baseName = baseName.substr(0, baseName.length()-4);
	//	}
	//}

	// Make sure the texture size is a square power of two.
	// Reduce the size by a power of two for each texture reduction level.
	
	//float tex_size = (o_TexX + o_TexY) / 2.0f; // start with average dimension
	//const float c_Log2 = ::logf(2.0);
	//float pow_of_2 = ::logf(tex_size) / c_Log2;
	//pow_of_2 -= l_TextureReduce;
	//const int c_MinimumPowerOfTwo = 7; // don't let textures get too small (2^7 == 128)
	//if (pow_of_2 < c_MinimumPowerOfTwo)
	//	o_TexX = o_TexY = (1 << c_MinimumPowerOfTwo);
	//else
	//	o_TexX = o_TexY = (1 << (int) pow_of_2);

	// The texture name map can be used by the caller to control
	// which textures should be written, or it can be used to gather
	// the unique filenames that were generated.
	//std::map<const g3dFragment*, std::string>::iterator it = io_TextureNameMap.find(i_pFrag);
	//if (it == io_TextureNameMap.end())
	{
		// Generate uniqeu filename and return its value in the map
		//fsFileUtil::GenerateFileName(m_OutputPathLocator, baseName, std::string("_baked.") + l_TexFormat, o_Name);
		
		std::string name = baseName;
		if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_HDR)
			name.append(std::string("_Baked_Color.") + l_TexFormat);
		else if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_Normals)
			name.append(std::string("_Baked_Normals.") + l_TexFormat);

		//io_TextureNameMap[i_pFrag] = name;
		o_Name = name;
	}
	//else
	//{
	//	// Use the filename given to us by the map
	//	o_Name = it->second;
	//}
}

//--------------------------------------------------------------------
//	Duplicate pixels around uv seams
//--------------------------------------------------------------------
void shdwBakingRenderer::FixupSeams(matTexture* io_pTexture, int i_Width, int i_Height)
{
	if (l_FixupSeams)
	{
		D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwBakingRenderer::FixupSeams" );
//		Sync();
		matTexture* TMP = NULL;
		TMP = matTextureMgr::CreateRenderTargetTexture(i_Width,i_Height,false,&m_PFD);
		g3dDX11Util::FixBakedTextureSeams(io_pTexture, TMP, 3, g3dDX11Util::e_Blend);
		matTextureMgr::ReleaseTexture(TMP);
		D3DPERF_EndEvent();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwBakingRenderer::InitMultisample()
{
	// don't init if already initted! 
	if (m_pHDRAATarget == NULL)
	{
		m_pHDRAATarget = new matRenderTargetTexture( false, false );
		m_pHDRAATarget->Make( l_TexRes, l_TexRes, l_hdrPFD, true, false, g2dResourceCounterDX11::eRenderTarget, true );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwBakingRenderer::CleanupMultisample()
{
	// release the multisample surface.
	delete m_pHDRAATarget;
	m_pHDRAATarget = NULL;
}

void shdwBakingRenderer::InitStates()
{
	st_MulBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE );
	st_AddBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Write_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

void shdwBakingRenderer::CleanupStates()
{
	SAFE_DELETE( st_MulBlend );
	SAFE_DELETE( st_AddBlend );
	SAFE_DELETE( ds_Disable_NS );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
	SAFE_DELETE( ds_Test_LessE_NS );
	SAFE_DELETE( ds_Write_NS );
}