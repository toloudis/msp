/****************************************************************************\
**	shdwPassSSGI.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwPassSSGI.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effMaskAlphaData.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matPlainTexture.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/shdw/shdwPassDepth.hpp"
#include "GraphicsDX11/G3d/g3dTransparencySortDX11.hpp"

//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D(hr, msg) \
	do { if( (hr) != D3D_OK )				\
	{									\
		g2dDX11Global::PrintDXError(hr);	\
		DBG_ASSERT((hr) == D3D_OK, (msg));\
	} \
	} while(0);
//		DBG_ASSERT3(hr == D3D_OK, "%s(%d) : %s ", __FILE__, __LINE__, (msg));\


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif


namespace
{
	// Stats
	int l_nNumTrianglesRendered = 0;

	matMaterial l_DepthMat;

	g2dPFD l_pfdRGBA32f = g2dPFD(g2dPFD::e_RGBA32f, 32*4);
	int l_bytesRGBA32f = l_pfdRGBA32f.BitsPerPixel()/8;;
	matPlainTexture* l_RndTexture = NULL;

	// lazy evaluate whether to recreate temp surfaces
	// based on whether width/height of window have changed.
	int l_Width = 0, l_Height = 0;

	#define randExc() (maFunctions::FloatRand(0,1))
	//#define randExc() ((float)rand() / ((float)(RAND_MAX)+(float)(1)))

	void MakeRandTex(effShaderBaseDX11* i_pEffect, int numDirs)
	{
//		srand(::timeGetTime());
		// directions
		maVector2d dirs[32];
		if(numDirs > 32)
			numDirs = 32;
		float inc = 2.0f * maConstants::c_fPI / (float)numDirs;
		for(int i=0; i < numDirs; i++)
		{
			float angle = inc*(float)(i);
			dirs[i].SetX( cos(angle));
			dirs[i].SetY( sin(angle));
		}

		// shader vars
		ID3DX11Effect* pEffect = i_pEffect->GetD3DXEffect();
		ID3DX11EffectVariable* pNumDir = pEffect->GetVariableByName("g_NumDir");
		pNumDir->AsScalar()->SetFloat((float)numDirs );
		ID3DX11EffectVariable* pDirs = pEffect->GetVariableByName("g_Dirs");
		pDirs->AsVector()->SetFloatVectorArray((float*)dirs, 0, 32);

		maVector4d f[64*64];
		for(int i=0; i<64*64; i++)
		{
			float angle = 2.0f * maConstants::c_fPI * randExc() / (float)numDirs;
			f[i].SetX(cos(angle));
			f[i].SetY(sin(angle));
			f[i].SetZ(randExc());
			f[i].SetW(0);
		}

		// hw tex
		matTextureMgr::ReleaseTexture(l_RndTexture);
		l_RndTexture = dynamic_cast<matPlainTexture*>(matTextureMgr::CreateTexture(64,64,&l_pfdRGBA32f,false, f));

		ID3D11ShaderResourceView* pTex = g3dDX11TextureUtil::GetD3DTexture(l_RndTexture);
		ID3DX11EffectVariable* pTexVar = pEffect->GetVariableByName("tRandom");
		pTexVar->AsShaderResource()->SetResource(pTex);
	}

	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_Keep = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_Replace = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_Ref_Replace = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_Ref_Equal = NULL;

	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_NoWrite_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_NoWrite_LessE_Replace = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_NoWrite_LessE_Ref_Equal = NULL;

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_NoBlendDepthOnly = NULL;
	g3dBlendStateMgr::BlendState* st_BlendNoAlpha = NULL;

}//namespace

void shdwPassSSGI::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_REPLACE( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS );
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_EQUAL( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_EQUAL );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Test_NoWrite_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );

	ds_Test_Write_LessE_Keep = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, TRUE, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Test_Write_LessE_Replace = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, TRUE, /*0,*/ 0xff, 0xff, OP_REPLACE, OP_KEEP );
	ds_Test_NoWrite_LessE_Replace = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, FALSE, D3D11_COMPARISON_LESS_EQUAL, TRUE, /*0,*/ 0xff, 0xff, OP_REPLACE, OP_KEEP );
	ds_Test_Write_LessE_Ref_Replace = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, TRUE, /*1,*/ 0xff, 0xff, OP_REPLACE, OP_KEEP );

	ds_Test_Write_LessE_Ref_Equal = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, TRUE, /*1,*/ 0xff, 0xff, OP_EQUAL, OP_KEEP );
	ds_Test_NoWrite_LessE_Ref_Equal = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, FALSE, D3D11_COMPARISON_LESS_EQUAL, TRUE, /*1,*/ 0xff, 0xff, OP_EQUAL, OP_KEEP );

	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE );
	st_NoBlendDepthOnly = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		0);
	st_BlendNoAlpha = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE );
}

void shdwPassSSGI::CleanupStates()
{
	SAFE_DELETE( ds_Test_Write_LessE_NS );
	SAFE_DELETE( ds_Disable_NS );
	SAFE_DELETE( ds_Test_Write_LessE_Keep );
	SAFE_DELETE( ds_Test_Write_LessE_Replace );
	SAFE_DELETE( ds_Test_Write_LessE_Ref_Replace );
	SAFE_DELETE( ds_Test_Write_LessE_Ref_Equal );
	SAFE_DELETE( ds_Test_NoWrite_LessE_NS );
	SAFE_DELETE( ds_Test_NoWrite_LessE_Replace );
	SAFE_DELETE( ds_Test_NoWrite_LessE_Ref_Equal );
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( st_NoBlendDepthOnly );
	SAFE_DELETE( st_BlendNoAlpha );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassSSGI::shdwPassSSGI(matRenderTargetTexture* i_NearDepthBuffer,
						   matRenderTargetTexture* i_DepthBuffer,
						   matRenderTargetTexture* i_DepthBuffer2,
						   g2dRenderTarget* i_FullSceneColors,
						   matRenderTargetTexture* i_pGISurface, 
						   matRenderTargetTexture* i_pTmpSurface,
						   const ssgiParams& i_SSGIParams,
						   matRenderTargetTexture* i_FullSceneColorsTex)
:	m_pDepthBuffer(i_DepthBuffer),
	m_pPrevDepthBuffer(i_DepthBuffer2),
	m_pNearDepthBuffer(i_NearDepthBuffer),
	m_GITargetTex(i_pGISurface),
	m_TempTargetTex(i_pTmpSurface),
	m_pCamera(NULL),
	m_Params(i_SSGIParams),
	m_pColorBuffer(i_FullSceneColorsTex)
{
	m_pRenderTarget = i_FullSceneColors;

	// lazy init so that this material can be reused across instantiations.
	if (!l_DepthMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/DepthRender.fx"), matShaderMgr::GetSpecialEffect("DepthRender.fx"));
		l_DepthMat.SetShaderParams(p);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassSSGI::~shdwPassSSGI()
{
	ReleaseResources();
}

//--------------------------------------------------------------------
// free any locally allocated shared (re-entrant?) resources
//--------------------------------------------------------------------
void shdwPassSSGI::CleanUp()
{
	// hw tex
	matTextureMgr::ReleaseTexture(l_RndTexture);
	l_RndTexture = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassSSGI::ReleaseResources()
{
	//CleanupMultisample();
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwPassSSGI::Render( float i_fSimTime )
{//PROFILE("Render");

	camPassBuffersData passBuffersData;
	m_pCamera->GetPassBuffersParams(passBuffersData);
	float top, bottom, left, right;
	m_pCamera->GetSubViewport(top, bottom, left, right);
	if ( passBuffersData.m_GIBuffer )
	{
		g3dDX11Util::BlendBuffers(m_pRenderTarget,passBuffersData.m_GIBuffer,passBuffersData.m_GIBlendOp,passBuffersData.m_GIIntensity,
								  top, bottom, left, right);
		return 0;
	}

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;

	// on entry, i expect a Clear()'ed and BeginScene()'d render target.
	// after i leave, i expect EndScene() and Present() to occur.
	
//	g2dDX11Global::g_pDevice->SetStreamSource(0, NULL, 0, 0);
//	g2dDX11Global::g_pDevice->SetPixelShader( NULL );

	// no objects for GI?

	int n = 0;
	const nodeCacheList& nonShadowNodes = m_SceneInfo->GetNonShadowNodes();
	n += nonShadowNodes.size();
	const nodeCacheList& shadowNodes = m_SceneInfo->GetShadowNodes();
	n += shadowNodes.size();
	const nodeCacheList& nonSolidNodes = m_SceneInfo->GetNonSolidNodes();
	n += nonSolidNodes.size();
//	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
//	{
//		g3dTransparencySortDX11* pTransNodes = m_SceneInfo->GetTransparentNodes();
//		n += pTransNodes->GetTransparentNodes().size();
//	}
	if (n == 0) return 0;

	// copy rendertarget to temp rendertarget
	/*g3dBlendStateMgr::SetBlendState( st_NoBlend );
	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );
	g3dDX11Util::CopyTexToTarget( m_pRenderTarget, m_TempRenderTarget );*/

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassSSGI::Render" );

	// reset d3d state
	g3dDX11Util::release_textures();

	g2dD3D11DepthStencilPtr pDepth = g2dDX11Global::GetDepthTarget();	//save current depth buffer
	
	m_GITargetTex->Clear(maFloatRGBA(0,0,0,0));
	m_GITargetTex->MakeCurrent();

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	int nPeelLayers = 1;

	if (g3dPrefs::CurrentPrefs().m_bEnableSSGIDepthPeeling)
	{
		nPeelLayers = g3dPrefs::CurrentPrefs().m_SSGINumLayers;
		if( nPeelLayers > 4 ) nPeelLayers = 4;
	}

	//assign initial ping pong buffers
	matRenderTargetTexture* pDstDepth = m_pDepthBuffer;
	matRenderTargetTexture* pSrcDepth = NULL;
	shdwPassDepth::eTechnique eTech = shdwPassDepth::eView;	//only first pass is view, the rest are peeled

	for( int l = 0; l < nPeelLayers; l++ )
	{
		//render depths
		shdwPassDepth depthPass( pDstDepth, m_pCamera, false, pSrcDepth, eTech );
		depthPass.SetSceneInfo( m_SceneInfo );
		depthPass.SetOverscanSize( m_Params.m_OverscanPixels );
		//depthPass.SetOverscanSize( 0 );
		l_nNumTrianglesRendered += depthPass.Render(i_fSimTime);
		if (g3dPrefs::CurrentPrefs().m_bEnableHair)
		{
			l_nNumTrianglesRendered += depthPass.RenderHair();			//also render hair
		}
//		if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
//		{
//			l_nNumTrianglesRendered += depthPass.RenderTransparent();
//		}

		//copy depth into component of multi depth buffer
		//this is less efficient than using write masking but to ping pong
		//2 RGBA32F textures wastes more memory than 2 R32F and 1 RGBA32F

		g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

		g3dDX11Util::CopyTextureComponent( pDstDepth, m_pNearDepthBuffer, 0, l );

		g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

		//swap depth targets
		pSrcDepth = (l&1) ? m_pPrevDepthBuffer : m_pDepthBuffer;
		pDstDepth = (l&1) ? m_pDepthBuffer : m_pPrevDepthBuffer;

		eTech = shdwPassDepth::eViewPeelLess;	//switch to a less equal depth compare
	}

	//calculate SSGI
	AccumulateGIPass( m_GITargetTex, nPeelLayers );

	//restore original depth buffer if not AA. the AA buffer has its own depth.
	if ((! g3dPrefs::CurrentPrefs().m_bHDRAA) || (!m_pRenderTarget->GetHasDepthBuffer()))
	{
		g2dDX11Global::SetDepthTarget( pDepth );
	}

	// mask out objects that are not to receive occlusion.
	DrawStencilPass(m_pRenderTarget);

	if (g3dPrefs::CurrentPrefs().m_bEnableSSGIBlur)
	{
		BlurGI(m_pRenderTarget);
	}
	else
	{
		BlendGI(m_pRenderTarget, m_GITargetTex );
	}

	// clear the mask.
	ClearStencil(m_pRenderTarget);
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

void shdwPassSSGI::DrawGIPass(g2dRenderTarget* i_pRenderTarget, matRenderTargetTexture* i_pCurDepth )
{
//	shared_ptr<effShaderParams> p(new effShaderParams());
//	p->SetShaderName(itString("AO/ssaoHorizonBasedAOEngine.fx"));
	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)matShaderMgr::GetSpecialEffect(("ssaoHorizonBasedAOEngine.fx"));
	ID3DX11Effect* pEffect = i_pEffect->GetD3DXEffect();

	//pEffect->SetTechnique("HORIZON_BASED_AO_LD_LOWQUALITY_Pass");
	//pEffect->SetTechnique("HORIZON_BASED_AO_NLD_LOWQUALITY_Pass");
	//pEffect->SetTechnique("HORIZON_BASED_AO_NLD_Pass");
	//pEffect->SetTechnique("HORIZON_BASED_AO_LD_Pass");
	//pEffect->SetTechnique("HORIZON_BASED_AO_NLD_QUALITY_Pass");
	ID3DX11EffectTechnique* pTechnique = pEffect->GetTechniqueByName("HORIZON_BASED_AO_LD_QUALITY_Pass");

	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	// distance cutoff
//	float m_AORadius = 0.35f;
//	float m_RadiusMultiplier  = 1.0f;
	float R[2] = {m_Params.m_GIRadius, m_Params.m_GIRadiusFar};
	float invR[2] = {1.0f/R[0], 1.0f/R[1]};
	float sqrR[2] = {R[0]*R[0], R[1]*R[1]};
	pEffect->GetVariableByName("g_R")->AsVector()->SetFloatVector( R );

	// angle cutoff
//    float m_AngleBias         = 30.0f;
    float angle = m_Params.m_AngleBias * maConstants::c_fAngleToRad;
	pEffect->GetVariableByName("g_AngleBias")->AsScalar()->SetFloat( angle );
	pEffect->GetVariableByName("g_TanAngleBias")->AsScalar()->SetFloat( tan(angle) );

//    float m_Contrast          = 1.4f;
    float contrast = m_Params.m_Contrast / (1.0f - sin(m_Params.m_AngleBias * maConstants::c_fAngleToRad));
//    float contrast = m_Params.m_Contrast / (1.0f - sin(m_Params.m_AngleBias * maConstants::c_fAngleToRad));
	pEffect->GetVariableByName("g_Contrast")->AsScalar()->SetFloat( contrast );

//    float m_NumSteps		    = 8;
	pEffect->GetVariableByName("g_NumSteps")->AsScalar()->SetFloat( (float)p.m_SSGINumSteps );
//    float m_Attenuation       = 1.0f;
	pEffect->GetVariableByName("g_Attenuation")->AsScalar()->SetFloat( m_Params.m_Attenuation );

	static int oldNumDirs = -1;
	if ((l_RndTexture == NULL) || (p.m_SSGINumDirs != oldNumDirs))
	{
		// rebuild random sampling texture
		MakeRandTex(i_pEffect, p.m_SSGINumDirs);
		oldNumDirs = p.m_SSGINumDirs;
	}

	// Recalculate buffer sizes
    int DBWidth    = i_pCurDepth->GetWidth();
    int DBHeight   = i_pCurDepth->GetHeight();
	int TargetWidth, TargetHeight;
	i_pRenderTarget->GetDimensions( TargetWidth, TargetHeight );

//	DBG_ASSERT(m_pCamera->GetAspect() == m_BBWidth/m_BBHeight, "bad aspect ratio");
	// divide by aspect since camera fov is fovx, not fovy.
	float fovx = m_pCamera->GetFOV() * maConstants::c_fAngleToRad;
//	float fovy = m_pCamera->GetFOV() * maConstants::c_fAngleToRad / m_pCamera->GetAspect();
	float FocalLen[2];
	float InvFocalLen[2];
//	FocalLen[0]      = 1.0f / tanf(fovy * 0.5f) *  (m_BBHeight / m_BBWidth);
//	FocalLen[1]      = 1.0f / tanf(fovy * 0.5f);
//	InvFocalLen[0]   = 1.0f / FocalLen[0];
//	InvFocalLen[1]   = 1.0f / FocalLen[1];
	InvFocalLen[0]   = tanf(fovx * 0.5f);
	InvFocalLen[1]   = tanf(fovx * 0.5f)/m_pCamera->GetAspect();
	FocalLen[0]      = 1.0f / InvFocalLen[0];
	FocalLen[1]      = 1.0f / InvFocalLen[1];

	float InvResolution[2];
    InvResolution[0] = 1.0f / TargetWidth;
    InvResolution[1] = 1.0f / TargetHeight;
	float Resolution[2];
    Resolution[0]    = (float)TargetWidth;
    Resolution[1]    = (float)TargetHeight;
	float OverscanRatio[2];
	OverscanRatio[0] = TargetWidth / (float)DBWidth;
	OverscanRatio[1] = TargetHeight / (float)DBHeight;
	
//	D3DXVECTOR4 v0(m_FocalLen[0],m_FocalLen[1],0,0);
//	D3DXVECTOR4 v1(m_InvFocalLen[0],m_InvFocalLen[1],0,0);
//	D3DXVECTOR4 v2(m_InvResolution[0],m_InvResolution[1],0,0);
//	D3DXVECTOR4 v3(m_Resolution[0],m_Resolution[1],0,0);
//	pEffect->SetVector("g_FocalLen", &v0);
//	pEffect->SetVector("g_InvFocalLen", &v1);
//	pEffect->SetVector("g_InvResolution", &v2);
//	pEffect->SetVector("g_Resolution", &v3);

	float nearFar[2] = {m_pCamera->GetNearClip(), m_pCamera->GetFarClip()};

	pEffect->GetVariableByName("g_NearFar")->AsVector()->SetFloatVector( nearFar );
	pEffect->GetVariableByName("g_FocalLen")->AsVector()->SetFloatVector( FocalLen );
	pEffect->GetVariableByName("g_InvFocalLen")->AsVector()->SetFloatVector( InvFocalLen );
	pEffect->GetVariableByName("g_InvResolution")->AsVector()->SetFloatVector( InvResolution );
	pEffect->GetVariableByName("g_Resolution")->AsVector()->SetFloatVector( Resolution );
	pEffect->GetVariableByName("g_OverscanRatio")->AsVector()->SetFloatVector( OverscanRatio );
	pEffect->GetVariableByName("g_SSGITint")->AsVector()->SetFloatVector( m_Params.m_Color.Ptr() );

	pEffect->GetVariableByName("tLinDepth")->AsShaderResource()->SetResource(g3dDX11TextureUtil::GetD3DTexture(i_pCurDepth));
//	pEffect->SetTexture("tNormal", g3dDX11TextureUtil::GetD3DTexture(m_pNormalsBuffer));

//	if (p.m_SSAOParams.m_EnableBlur)
		m_GITargetTex->MakeCurrent();
		int w,h;
		m_GITargetTex->GetDimensions(w,h);
//	else
//		i_pRenderTarget->MakeCurrent();

		// This code has not been tested yet
	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(0);
	pPass->Apply(0, g2dDX11Global::g_pDeviceContext);
    g3dDX11Util::DrawFullScreenQuad( w,h );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

}

void shdwPassSSGI::AccumulateGIPass( g2dRenderTarget* i_pGITarget, int i_nLayers )
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassSSGI::AccumulateGIPass" );

	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)matShaderMgr::GetSpecialEffect(("ssgiMultiHorizonBasedGI.fx"));
	ID3DX11Effect* pEffect = i_pEffect->GetD3DXEffect();

	ID3DX11EffectTechnique* pTechnique = pEffect->GetTechniqueByName("HORIZON_BASED_GI_MULTI_QUALITY_Pass");

	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	// distance cutoff
	float R[2] = {m_Params.m_GIRadius, m_Params.m_GIRadiusFar};
	float invR[2] = {1.0f/R[0], 1.0f/R[1]};
	float sqrR[2] = {R[0]*R[0], R[1]*R[1]};
	pEffect->GetVariableByName("g_R")->AsVector()->SetFloatVector( R );

	// angle cutoff
	float angle = m_Params.m_AngleBias * maConstants::c_fAngleToRad;
	pEffect->GetVariableByName("g_AngleBias")->AsScalar()->SetFloat( angle );
	pEffect->GetVariableByName("g_TanAngleBias")->AsScalar()->SetFloat( tan(angle) );

	float contrast = m_Params.m_Contrast / (1.0f - sin(m_Params.m_AngleBias * maConstants::c_fAngleToRad));
	pEffect->GetVariableByName("g_Contrast")->AsScalar()->SetFloat( contrast );

	pEffect->GetVariableByName("g_NumSteps")->AsScalar()->SetFloat( (float)p.m_SSGINumSteps );
	pEffect->GetVariableByName("g_Attenuation")->AsScalar()->SetFloat( m_Params.m_Attenuation );

	static int oldNumDirs = -1;
	if ((l_RndTexture == NULL) || (p.m_SSGINumDirs != oldNumDirs))
	{
		// rebuild random sampling texture
		MakeRandTex(i_pEffect, p.m_SSGINumDirs);
		oldNumDirs = p.m_SSGINumDirs;
	}

	// Recalculate buffer sizes
	int DBWidth = m_pNearDepthBuffer->GetWidth();
	int DBHeight = m_pNearDepthBuffer->GetHeight();
	int TargetWidth, TargetHeight;
	i_pGITarget->GetDimensions( TargetWidth, TargetHeight );

	//	DBG_ASSERT(m_pCamera->GetAspect() == m_BBWidth/m_BBHeight, "bad aspect ratio");
	// divide by aspect since camera fov is fovx, not fovy.
	float fovx = m_pCamera->GetFOV() * maConstants::c_fAngleToRad;
	float InvFocalLen[2] = {tanf(fovx * 0.5f),tanf(fovx * 0.5f)/m_pCamera->GetAspect()};
	float FocalLen[2] = {1.0f / InvFocalLen[0],1.0f / InvFocalLen[1]};
	float InvResolution[2] = {1.0f / TargetWidth, 1.0f / TargetHeight};
	float Resolution[2] = {(float)TargetWidth, (float)TargetHeight};
	float OverscanRatio[2] = {TargetWidth / (float)DBWidth, TargetHeight / (float)DBHeight};
	float nearFar[2] = {m_pCamera->GetNearClip(), m_pCamera->GetFarClip()};

	pEffect->GetVariableByName("g_NearFar")->AsVector()->SetFloatVector( nearFar );
	pEffect->GetVariableByName("g_FocalLen")->AsVector()->SetFloatVector( FocalLen );
	pEffect->GetVariableByName("g_InvFocalLen")->AsVector()->SetFloatVector( InvFocalLen );
	pEffect->GetVariableByName("g_InvResolution")->AsVector()->SetFloatVector( InvResolution );
	pEffect->GetVariableByName("g_Resolution")->AsVector()->SetFloatVector( Resolution );
	pEffect->GetVariableByName("g_OverscanRatio")->AsVector()->SetFloatVector( OverscanRatio );
	pEffect->GetVariableByName("g_nLayers")->AsScalar()->SetInt( i_nLayers );
	pEffect->GetVariableByName("g_SSGITint")->AsVector()->SetFloatVector( m_Params.m_Color.Ptr() );

	pEffect->GetVariableByName("tDepths")->AsShaderResource()->SetResource( g3dDX11TextureUtil::GetD3DTexture(m_pNearDepthBuffer) );
	pEffect->GetVariableByName("tColors")->AsShaderResource()->SetResource( g3dDX11TextureUtil::GetD3DTexture(m_pColorBuffer) );

	g2dDX11Global::SetDepthTarget(NULL);
	i_pGITarget->MakeCurrent();
	int w,h;
	i_pGITarget->GetDimensions(w,h);

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(0);
	pPass->Apply(0, g2dDX11Global::g_pDeviceContext);
	g3dDX11Util::DrawFullScreenQuad( w,h );

	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	// unbind texture from shader so that it can be set as rendertarget later.
	ID3D11ShaderResourceView* nullPSR[3] = {NULL, NULL, NULL};
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0,3,nullPSR);
	//g2dDX11Global::g_pDeviceContext->PSSetShader(NULL, NULL, 0);

	D3DPERF_EndEvent();
}

void shdwPassSSGI::DrawStencilPass(g2dRenderTarget* i_pTarget)
{

	if (!g2dDX11Global::g_bHasStencil)
	{
		static bool warnOnce = true;
		if (warnOnce)
		{
			DBG_WARNING("SSGI skipping stencil buffer pass");
			warnOnce = false;
		}
		return;
	}
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassSSGI::DrawStencilPass" );

	i_pTarget->MakeCurrent();

	const UINT8 stencilValue = 1;
	g2dDX11Global::g_pDeviceContext->ClearDepthStencilView(
		g2dDX11Global::GetDepthTarget(),
		D3D11_CLEAR_STENCIL,
		1.0f,
		stencilValue);

	// enable stenciling
	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_NoWrite_LessE_Replace );
	
	g3dBlendStateMgr::SetBlendState(st_NoBlendDepthOnly);


	int i,n;

	const nodeCacheList& nonShadowNodes = m_SceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		m_stats.m_nTriangles += RenderNodeStencil(currentNode);
	}
	const nodeCacheList& shadowNodes = m_SceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		m_stats.m_nTriangles += RenderNodeStencil(currentNode);
	}
	const nodeCacheList& nonSolidNodes = m_SceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		m_stats.m_nTriangles += RenderNodeStencil(currentNode);
	}
//	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
//	{
//		g3dTransparencySortDX11* pTransNodes = m_SceneInfo->GetTransparentNodes();
//		const TranspNodeVector& tNodes = pTransNodes->GetTransparentNodes();
//		TranspNodeVector::const_iterator it, end = tNodes.end();
//
//		for (it = tNodes.begin(); it != end; ++it)
//		{
//			sNodePlusState NS;
//			NS.m_pNode = it->m_pSceneNode;
//			NS.m_StateCache = it->m_RenderStateCache;
//
//			bool doRender = true;
//
//			/*g3dFragment * frag = (g3dFragment*)NS.m_pNode->GetFragment();
//			if ( frag )
//			{
//				matMaterial * mat = frag->GetMaterial();
//				if ( mat )
//				{
//					doRender = !mat->GetBelongsToLightShaft();
//				}
//			}*/
//
//			if ( doRender )
//			{
//				m_stats.m_nTriangles += RenderNodeStencil( NS );
//			}
//		}
//	}

	// restore some state.
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	// restore depth write state
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );

	D3DPERF_EndEvent();
}

// clear the mask.
void shdwPassSSGI::ClearStencil(g2dRenderTarget* i_pTarget)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassSSGI::ClearStencil" );

	// disable stencil test and restore some state. should I clear it here? probably.
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	// can i get away with not clearing it?
//	HRESULT op_result = g2dDX11Global::g_pDevice->Clear(	0,
//		NULL,
//		D3DCLEAR_STENCIL,
//		0, 
//		1.0f,
//		stencilValue);

	D3DPERF_EndEvent();
}


void shdwPassSSGI::BlurGI(g2dRenderTarget* i_pRenderTarget)
{
	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)matShaderMgr::GetSpecialEffect(("ssaoBilateralBlurEngine.fx"));
	ID3DX11Effect* pEffect = i_pEffect->GetD3DXEffect();

	ID3DX11EffectTechnique* pTechnique = pEffect->GetTechniqueByName("BlurPass");

    // Update shader variables g_Resolution and g_InvResolution
	int width, height;
	i_pRenderTarget->GetDimensions(width, height);
	int DBWidth    = m_pNearDepthBuffer->GetWidth();
	int DBHeight   = m_pNearDepthBuffer->GetHeight();
	float invResolution[2];
    invResolution[0] = 1.0f / (float)width;
    invResolution[1] = 1.0f / (float)height;
	float resolution[2];
    resolution[0]    = (float)width;
    resolution[1]    = (float)height;
	float OverscanRatio[2];
	OverscanRatio[0] = width / (float)DBWidth;
	OverscanRatio[1] = height / (float)DBHeight;

	pEffect->GetVariableByName("g_InvResolution")->AsVector()->SetFloatVector( invResolution );
	pEffect->GetVariableByName("g_Resolution")->AsVector()->SetFloatVector( resolution );
	pEffect->GetVariableByName("g_OverscanRatio")->AsVector()->SetFloatVector( OverscanRatio );

    float m_EdgeThreshold  = 0.1f;

	// the blur params.
	float radius     = m_Params.m_BlurWidth;
    float sigma      = (radius+1) / 2;
    float inv_sigma2 = 1.0f / (2*sigma*sigma);

	// Blur Pass X : render from gi target into temp target

	/*if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{*/
		//m_pNearDepthBuffer->MakeDepthCurrent();
		m_pNearDepthBuffer->MakeCurrent();
	//}
	m_TempTargetTex->MakeCurrent();
	int w,h;
	m_TempTargetTex->GetDimensions(w,h);
	m_TempTargetTex->Clear(maFloatRGBA(0,0,0,0));
    
	pEffect->GetVariableByName("g_BlurFalloff")->AsScalar()->SetFloat( inv_sigma2 );
	pEffect->GetVariableByName("g_BlurRadius")->AsScalar()->SetFloat( radius );

	pEffect->GetVariableByName("g_EdgeThreshold")->AsScalar()->SetFloat( m_EdgeThreshold );
    float sharpness = (m_Params.m_BlurSharpness) * (m_Params.m_BlurSharpness);
	pEffect->GetVariableByName("g_Sharpness")->AsScalar()->SetFloat( sharpness );

	pEffect->GetVariableByName("tDepth")->AsShaderResource()->SetResource( g3dDX11TextureUtil::GetD3DTexture(m_pNearDepthBuffer) );
	pEffect->GetVariableByName("tColor")->AsShaderResource()->SetResource( NULL );

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	pEffect->GetVariableByName("tSource")->AsShaderResource()->SetResource( g3dDX11TextureUtil::GetD3DTexture(m_GITargetTex) );

	ID3DX11EffectPass* pPass0 = pTechnique->GetPassByIndex(0);
	pPass0->Apply(0, g2dDX11Global::g_pDeviceContext);
	g3dDX11Util::DrawFullScreenQuad( w,h );

	//g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_BlendNoAlpha);

	// turn on stencil - was set up in earlier pass.
//	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_Ref_Equal, 1 );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_NoWrite_LessE_Ref_Equal, 1 );

    // Blur Pass Y : render from temp target into main render target
	i_pRenderTarget->MakeCurrent();
	i_pRenderTarget->GetDimensions(w,h);
	pEffect->GetVariableByName("tSource")->AsShaderResource()->SetResource( g3dDX11TextureUtil::GetD3DTexture(m_TempTargetTex) );

	ID3DX11EffectPass* pPass1 = pTechnique->GetPassByIndex(1);
	pPass1->Apply(0, g2dDX11Global::g_pDeviceContext);
	g3dDX11Util::DrawFullScreenQuad( w,h );

	// restore stencil state
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_NoWrite_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	// unbind texture from shader so that it can be set as rendertarget later.
	ID3D11ShaderResourceView* nullPSR[3] = {NULL, NULL, NULL};
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0,3,nullPSR);
}

void shdwPassSSGI::BlendGI(g2dRenderTarget* i_pRenderTarget, matRenderTargetTexture* i_pGISrc )
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassSSGI::BlendGI" );

	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)matShaderMgr::GetSpecialEffect(("HDRLighting.fx"));
	ID3DX11Effect* pEffect = i_pEffect->GetD3DXEffect();

	ID3DX11EffectTechnique* pTechnique = pEffect->GetTechniqueByName("SimpleCopy");

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	// set render target and source texture
	i_pRenderTarget->MakeCurrent();
	int w,h;
	i_pRenderTarget->GetDimensions(w,h);
	ID3D11ShaderResourceView* pResView = g3dDX11TextureUtil::GetD3DTexture(m_GITargetTex);

	// turn on stencil - was set up in earlier pass.
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_NoWrite_LessE_Ref_Equal, 1 );

	g3dBlendStateMgr::SetBlendState(st_BlendNoAlpha);

	ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(0);
	pPass->Apply(0, g2dDX11Global::g_pDeviceContext);
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources( 0, 1, &pResView );
	g3dDX11Util::DrawFullScreenQuad( w,h );

	// restore stencil state
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	// unbind texture from shader so that it can be set as rendertarget later.
	ID3D11ShaderResourceView* nullPSR[2] = {NULL, NULL};
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0,2,nullPSR);

	D3DPERF_EndEvent();
}

int shdwPassSSGI::RenderNodeStencil(const sNodePlusState& i_Node)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassSSGI::RenderNodeStencil" );

	DBG_ASSERT(i_Node.m_pNode->GetFragment() != NULL, "null fragment in SSGI");

	// Set the comparison reference value
	// this is where we decide if the fragment will receive occlusion.
	// either way, we still have to draw the frag so it fills depth buffer for the depthstencil test later.
	bool UseRef = i_Node.m_pNode->GetFragment()->GetReceivesGI();
	g3dDepthStencilStateMgr::SetDepthStencilState( UseRef ? ds_Test_Write_LessE_Ref_Replace : ds_Test_Write_LessE_Replace, UseRef ? 1 : 0 );

	// set the minimal state necessary to draw depth.
	int nTriangles = 0;
	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);

	// resolve material/effect
	const matMaterial* pMaterial = &l_DepthMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

//	pEffect->SetTechnique("ViewSpaceDepthRemap");	
	pEffect->SetTechnique("ViewSpaceDepth");	

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	D3DPERF_EndEvent();
	return nTriangles;
}

