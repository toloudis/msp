/****************************************************************************\
**	shdwPassLPVGI.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwPassLPVGI.hpp"

#include "Core/ma/maSampling.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
//#include "Graphics/g3d/g3dGILight.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/g3d/g3dTargetRenderer.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dRenderTargetDX11.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/G3d/g3dTransparencySortDX11.hpp"
#include "GraphicsDX11/mat/matPlainTexture.hpp"
#include "GraphicsDX11/mat/matReflectiveShadowMap.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"


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
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;

	g3dBlendStateMgr::BlendState* st_Blend = NULL;
	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_BlendNoAlpha = NULL;
	g3dBlendStateMgr::BlendState* st_MulColorBlend = NULL;
	g3dBlendStateMgr::BlendState* st_PropagateBlend = NULL;

	int l_RSMSize = 512;
	//matMaterial l_RSMGIMat;
	matMaterial l_InjectionMat;
	int l_nNumTrianglesRendered = 0;
	matTexture* l_pReflectiveMap = NULL;

	// set up global RSM injection variables
	struct SCREEN_VERTEX
	{
		maVector4d pos;
		maPoint2d uv;
	};

	SCREEN_VERTEX* l_VPLVertex = NULL;
	SCREEN_VERTEX* l_RVVertex = NULL;

	ID3D11Buffer*               l_VPLVertexVB = NULL;
	ID3D11Buffer*               l_RVVertexVB = NULL;
	ID3D11InputLayout*          l_VPLVertexLayout = NULL;

	ID3D11Texture3D* l_RVTex0[3] = {NULL, NULL, NULL};	// 3d texture for storing radiance volume
	ID3D11Texture3D* l_RVTex1[3] = {NULL, NULL, NULL};	// allocate ping-pong buff
	ID3D11Texture3D* l_AccumRVTex[3] = {NULL, NULL, NULL};
	ID3D11ShaderResourceView* l_RVShaderResourceView0[3] = {NULL, NULL, NULL};
	ID3D11RenderTargetView*	l_RVRenderTargetView0[3] = {NULL, NULL, NULL};
	ID3D11ShaderResourceView* l_RVShaderResourceView1[3] = {NULL, NULL, NULL};
	ID3D11RenderTargetView*	l_RVRenderTargetView1[3] = {NULL, NULL, NULL};
	ID3D11ShaderResourceView* l_ARVShaderResourceView[3] = {NULL, NULL, NULL};
	ID3D11RenderTargetView*	l_ARVRenderTargetView[3] = {NULL, NULL, NULL};

	int l_LPVCellNum = 32;

	void init_lpv_texture()
	{
		HRESULT op_result;
		CD3D11_TEXTURE3D_DESC desc(DXGI_FORMAT_R32G32B32A32_FLOAT,
			l_LPVCellNum, l_LPVCellNum, l_LPVCellNum, 
			1, BIND_SHADER_RESOURCE | BIND_RENDER_TARGET
			);

		// create d3d texture
		for (int i = 0; i < 3; i++)
		{
			op_result = g2dDX11Global::g_pDevice->CreateTexture3D(
				&desc, NULL, &l_RVTex0[i]);

			if( FAILED(op_result) )
			{
				g2dDX11Global::PrintDXError(op_result);
				if ( E_OUTOFMEMORY == op_result )
					throw g2dOutOfVideoMemoryX();
				DBG_ASSERT(false, "Error creating lpv 3d texture");
			}

			op_result = g2dDX11Global::g_pDevice->CreateTexture3D(
				&desc, NULL, &l_RVTex1[i]);

			if( FAILED(op_result) )
			{
				g2dDX11Global::PrintDXError(op_result);
				if ( E_OUTOFMEMORY == op_result )
					throw g2dOutOfVideoMemoryX();
				DBG_ASSERT(false, "Error creating lpv 3d texture");
			}
	
			op_result = g2dDX11Global::g_pDevice->CreateTexture3D(
				&desc, NULL, &l_AccumRVTex[i]);
		}

		// create SRV:
		for (int i = 0; i < 3; i++)
		{
			op_result = g2dDX11Global::g_pDevice->CreateShaderResourceView( l_RVTex0[i], NULL, &l_RVShaderResourceView0[i] );
			if( !SUCCEEDED(op_result) )
			{
				l_RVShaderResourceView0[i] = NULL;
				g2dDX11Global::PrintDXError(op_result);
				if (op_result == E_OUTOFMEMORY)
				{
					throw g2dOutOfSystemMemoryX();
				}
				else if ( op_result == D3DERR_INVALIDCALL )
				{
					throw g2dGeneralX();
				}
				DBG_ASSERT(SUCCEEDED(op_result), "Error creating render target depth surface");
			}

			op_result = g2dDX11Global::g_pDevice->CreateShaderResourceView( l_RVTex1[i], NULL, &l_RVShaderResourceView1[i] );
			if( !SUCCEEDED(op_result) )
			{
				l_RVShaderResourceView1[i] = NULL;
				g2dDX11Global::PrintDXError(op_result);
				if (op_result == E_OUTOFMEMORY)
				{
					throw g2dOutOfSystemMemoryX();
				}
				else if ( op_result == D3DERR_INVALIDCALL )
				{
					throw g2dGeneralX();
				}
				DBG_ASSERT(SUCCEEDED(op_result), "Error creating render target depth surface");
			}

			op_result = g2dDX11Global::g_pDevice->CreateShaderResourceView( l_AccumRVTex[i], NULL, &l_ARVShaderResourceView[i] );
		}

		// create render target view
		for (int i = 0; i < 3; i++)
		{
			op_result = g2dDX11Global::g_pDevice->CreateRenderTargetView( l_RVTex0[i], NULL, &l_RVRenderTargetView0[i] );
			if( !SUCCEEDED(op_result) )
			{
				l_RVRenderTargetView0[i] = NULL;
				g2dDX11Global::PrintDXError(op_result);
				if (op_result == E_OUTOFMEMORY)
				{
					throw g2dOutOfSystemMemoryX();
				}
				else if ( op_result == D3DERR_INVALIDCALL )
				{
					throw g2dGeneralX();
				}
				DBG_ASSERT(SUCCEEDED(op_result), "Error creating render target main surface");
			}

			op_result = g2dDX11Global::g_pDevice->CreateRenderTargetView( l_RVTex1[i], NULL, &l_RVRenderTargetView1[i] );
			if( !SUCCEEDED(op_result) )
			{
				l_RVRenderTargetView1[i] = NULL;
				g2dDX11Global::PrintDXError(op_result);
				if (op_result == E_OUTOFMEMORY)
				{
					throw g2dOutOfSystemMemoryX();
				}
				else if ( op_result == D3DERR_INVALIDCALL )
				{
					throw g2dGeneralX();
				}
				DBG_ASSERT(SUCCEEDED(op_result), "Error creating render target main surface");
			}

			op_result = g2dDX11Global::g_pDevice->CreateRenderTargetView( l_AccumRVTex[i], NULL, &l_ARVRenderTargetView[i] );
		}
	}
	void init_vplvertex()
	{
		HRESULT hr;

		if (!l_VPLVertexLayout)
		{
			// the dx input layout, corresponds to the struct in the shader
			const D3D11_INPUT_ELEMENT_DESC vpllayout[] =
			{
				{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			void* pShaderBytecode = NULL;
			unsigned long bytecodeLength = 0;
			matShaderMgr::SetUseShaderArray(true);
			effShaderBaseDX11* pDefaultEffect = (effShaderBaseDX11*)matShaderMgr::GetSpecialEffect("LPV_GI.fx");//effShaderArray::GetDefaultEffect();
			DBG_ASSERT(pDefaultEffect, "effects not initialized yet.");
			D3DX11_PASS_DESC passDesc;
			pDefaultEffect->GetD3DXEffect()->GetTechniqueByIndex(0)->GetPassByIndex(0)->GetDesc(&passDesc);
			pShaderBytecode = passDesc.pIAInputSignature;
			bytecodeLength = (unsigned long)passDesc.IAInputSignatureSize;

			hr = g2dDX11Global::g_pDevice->CreateInputLayout(vpllayout, 2, 
				pShaderBytecode, bytecodeLength, &l_VPLVertexLayout);
			DBG_ASSERT(SUCCEEDED(hr), "CreateVertexDeclaration for VPL injection failed.");
		}

		// create fixed vpl vertex buffer
		if (!l_VPLVertex)
		{
			float step = 2.0f / (float)(l_RSMSize);
			float halfStep = step / 2.0f;
			l_VPLVertex = new SCREEN_VERTEX[l_RSMSize * l_RSMSize];
			for (int y = 0; y < l_RSMSize; y++)
			{
				for (int x = 0; x < l_RSMSize; x++)
				{
					int index = y * l_RSMSize + x;
					l_VPLVertex[index].pos = maVector4d( -1.0f + (float)x * step + halfStep, 1.0f - (float)y * step - halfStep, 0.5f, 1.0f );
					//l_VPLVertex[index].pos = maVector4d( -1.0f + (float)x * step + halfStep, -1.0f + (float)y * step + halfStep, 0.5f, 1.0f );
					l_VPLVertex[index].uv = maPoint2d( (float)x * halfStep, (float)y * halfStep);
				}
			}
		}

		if (!l_VPLVertexVB)
		{
			D3D11_BUFFER_DESC vbdesc =
			{
				l_RSMSize * l_RSMSize * sizeof( SCREEN_VERTEX ),
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_VERTEX_BUFFER,
				0,
				0
			};

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = l_VPLVertex;
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;
			hr = ( g2dDX11Global::g_pDevice->CreateBuffer( &vbdesc, &InitData, &l_VPLVertexVB ) );
		}

		// create fixed rv vertex buffer
		if (!l_RVVertex)
		{
			float step = 2.0f / (float)(l_LPVCellNum);
			l_RVVertex = new SCREEN_VERTEX[l_LPVCellNum * l_LPVCellNum];
			float halfStep = step / 2.0f;
			for (int y = 0; y < l_LPVCellNum; y++)
			{
				for (int x = 0; x < l_LPVCellNum; x++)
				{
					int index = y * l_LPVCellNum + x;
					l_RVVertex[index].pos = maVector4d( -1.0f + (float)x * step + halfStep, 1.0f - (float)y * step - halfStep, 0.5f, 1.0f );
					//l_RVVertex[index].pos = maVector4d( -1.0f + (float)x * step + halfStep, -1.0f + (float)y * step + halfStep, 0.5f, 1.0f );
					l_RVVertex[index].uv = maPoint2d( (float)x * halfStep, (float)y * halfStep);
				}
			}
		}

		if (!l_RVVertexVB)
		{
			D3D11_BUFFER_DESC vbdesc =
			{
				l_LPVCellNum * l_LPVCellNum * sizeof( SCREEN_VERTEX ),
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_VERTEX_BUFFER,
				0,
				0
			};

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = l_RVVertex;
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;
			hr = ( g2dDX11Global::g_pDevice->CreateBuffer( &vbdesc, &InitData, &l_RVVertexVB ) );
		}
	}

	void update_lpv_vertex(int i_LPVCellNum)
	{
		if (i_LPVCellNum == l_LPVCellNum)
			return;

		HRESULT hr;

		if (l_RVVertex)
			delete [] l_RVVertex;
		l_RVVertex = NULL;
		SAFE_RELEASE(l_RVVertexVB);

		l_LPVCellNum = i_LPVCellNum;
		l_LPVCellNum = min(l_LPVCellNum, 1024);
		
		// create fixed rv vertex buffer
		if (!l_RVVertex)
		{
			float step = 2.0f / (float)(l_LPVCellNum);
			l_RVVertex = new SCREEN_VERTEX[l_LPVCellNum * l_LPVCellNum];
			float halfStep = step / 2.0f;
			for (int y = 0; y < l_LPVCellNum; y++)
			{
				for (int x = 0; x < l_LPVCellNum; x++)
				{
					int index = y * l_LPVCellNum + x;
					l_RVVertex[index].pos = maVector4d( -1.0f + (float)x * step + halfStep, 1.0f - (float)y * step - halfStep, 0.5f, 1.0f );
					//l_RVVertex[index].pos = maVector4d( -1.0f + (float)x * step + halfStep, -1.0f + (float)y * step + halfStep, 0.5f, 1.0f );
					l_RVVertex[index].uv = maPoint2d( (float)x * halfStep, (float)y * halfStep);
				}
			}
		}

		if (!l_RVVertexVB)
		{
			D3D11_BUFFER_DESC vbdesc =
			{
				l_LPVCellNum * l_LPVCellNum * sizeof( SCREEN_VERTEX ),
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_VERTEX_BUFFER,
				0,
				0
			};

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = l_RVVertex;
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;
			hr = ( g2dDX11Global::g_pDevice->CreateBuffer( &vbdesc, &InitData, &l_RVVertexVB ) );
		}

		for (int i = 0; i < 3; i++)
		{
			SAFE_RELEASE(l_RVShaderResourceView0[i]);
			SAFE_RELEASE(l_RVRenderTargetView0[i]);
			SAFE_RELEASE(l_RVTex0[i]);

			SAFE_RELEASE(l_RVShaderResourceView1[i]);
			SAFE_RELEASE(l_RVRenderTargetView1[i]);
			SAFE_RELEASE(l_RVTex1[i]);

			SAFE_RELEASE(l_ARVShaderResourceView[i]);
			SAFE_RELEASE(l_ARVRenderTargetView[i]);
			SAFE_RELEASE(l_AccumRVTex[i]);
		}
		init_lpv_texture();
	}

	void update_rsm_size(int i_RSMSize, matTexture*& i_rsmap)
	{
		HRESULT hr;

		if (i_RSMSize == l_RSMSize && i_rsmap != NULL)
			return;

		l_RSMSize = i_RSMSize;

		if (i_rsmap)
			matTextureMgr::ReleaseTexture(i_rsmap);
		i_rsmap = matTextureMgr::CreateReflectiveShadowMap( l_RSMSize, l_RSMSize );
		
		if (l_VPLVertex)
			delete [] l_VPLVertex;
		l_VPLVertex = NULL;
		SAFE_RELEASE(l_VPLVertexVB);


		// create fixed vpl vertex buffer
		if (!l_VPLVertex)
		{
			float step = 2.0f / (float)(l_RSMSize);
			float halfStep = step / 2.0f;
			l_VPLVertex = new SCREEN_VERTEX[l_RSMSize * l_RSMSize];
			for (int y = 0; y < l_RSMSize; y++)
			{
				for (int x = 0; x < l_RSMSize; x++)
				{
					int index = y * l_RSMSize + x;
					l_VPLVertex[index].pos = maVector4d( -1.0f + (float)x * step + halfStep, 1.0f - (float)y * step - halfStep, 0.5f, 1.0f );
					//l_VPLVertex[index].pos = maVector4d( -1.0f + (float)x * step + halfStep, -1.0f + (float)y * step + halfStep, 0.5f, 1.0f );
					l_VPLVertex[index].uv = maPoint2d( (float)x * halfStep, (float)y * halfStep);
				}
			}
		}

		if (!l_VPLVertexVB)
		{
			D3D11_BUFFER_DESC vbdesc =
			{
				l_RSMSize * l_RSMSize * sizeof( SCREEN_VERTEX ),
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_VERTEX_BUFFER,
				0,
				0
			};

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = l_VPLVertex;
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;
			hr = ( g2dDX11Global::g_pDevice->CreateBuffer( &vbdesc, &InitData, &l_VPLVertexVB ) );
		}
	}

	void cleanup()
	{
		if (l_pReflectiveMap)
			matTextureMgr::ReleaseTexture(l_pReflectiveMap);
		l_pReflectiveMap = NULL;

		for (int i = 0; i < 3; i++)
		{
			SAFE_RELEASE(l_RVShaderResourceView0[i]);
			SAFE_RELEASE(l_RVRenderTargetView0[i]);
			SAFE_RELEASE(l_RVTex0[i]);

			SAFE_RELEASE(l_RVShaderResourceView1[i]);
			SAFE_RELEASE(l_RVRenderTargetView1[i]);
			SAFE_RELEASE(l_RVTex1[i]);

			SAFE_RELEASE(l_ARVShaderResourceView[i]);
			SAFE_RELEASE(l_ARVRenderTargetView[i]);
			SAFE_RELEASE(l_AccumRVTex[i]);
		}

		SAFE_RELEASE(l_VPLVertexVB);
		SAFE_RELEASE(l_VPLVertexLayout);
		SAFE_RELEASE(l_RVVertexVB);
		SAFE_RELEASE(l_VPLVertexLayout);

		if (l_VPLVertex)
			delete [] l_VPLVertex;
		l_VPLVertex = NULL;

		if (l_RVVertex)
			delete [] l_RVVertex;
		l_RVVertex = NULL;
		/*SAFE_DELETE(l_VPLVertex);
		SAFE_DELETE(l_RVVertex);*/
	}
}//namespace

void shdwPassLPVGI::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );

	st_Blend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL);
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL);
	st_BlendNoAlpha = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE );
	st_MulColorBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_DEST_COLOR,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE );
	st_PropagateBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );

	init_lpv_texture();
	init_vplvertex();
}

void shdwPassLPVGI::CleanupStates()
{
	SAFE_DELETE( ds_Test_Write_LessE_NS );
	SAFE_DELETE( ds_Disable_NS );
	SAFE_DELETE( st_Blend );
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( st_BlendNoAlpha );
	SAFE_DELETE( st_MulColorBlend );
	SAFE_DELETE( st_PropagateBlend );

	cleanup();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassLPVGI::shdwPassLPVGI(const ssgiParams& i_SSGIParams,
							 matRenderTargetTexture* i_Scratch, 
							 matRenderTargetTexture* i_Depth,
							 g2dRenderTarget* i_Target,
							 const camCamera* i_pCamera,
							 const g3dScene* i_pScene,
							 bool i_bIsDeferRender)
:	m_pCamera(i_pCamera),
	m_pScene(i_pScene),
	m_scratchTex(i_Scratch),
	m_depthTex(i_Depth),
	m_giParam(i_SSGIParams),
	m_pRSMRenderer(NULL),
	m_pRSMTargetRenderer(NULL),
	m_bSwapedBuffer(false),
	m_bDeferRender(i_bIsDeferRender)
{
	m_pRenderTarget = i_Target;

	// lazy init so that this material can be reused across instantiations.
	/*if (!l_RSMGIMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/RSMGI.fx"), matShaderMgr::GetSpecialEffect("RSMGI.fx"));
		l_RSMGIMat.SetShaderParams(p);
	}*/

	if (!l_InjectionMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/LPV_GI.fx"), matShaderMgr::GetSpecialEffect("LPV_GI.fx"));
		l_InjectionMat.SetShaderParams(p);
	}

	m_pRSMRenderer = g3dSceneRendererCreate::CreateRSMRenderer();
	m_pRSMTargetRenderer = new g3dTargetRenderer();

	//l_pReflectiveMap = matTextureMgr::CreateReflectiveShadowMap( l_RSMSize, l_RSMSize );
	//m_pDownSampledRSM = matTextureMgr::CreateReflectiveShadowMap( l_RSMSize / 2, l_RSMSize / 2);
	/*if (l_FixedGIPos.size() == 0 || l_FixedGIPos.size() != l_FixedGINum)
	{
		maSampling::GetSamplesSphereGSS(l_FixedGINum, l_FixedGIPos);
	}*/
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassLPVGI::~shdwPassLPVGI()
{
	ReleaseResources();
}

void shdwPassLPVGI::SetSceneInfo(shdwPassTraversal* i_Traversal) 
{
	m_sceneInfo = i_Traversal;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassLPVGI::ReleaseResources()
{
	//CleanupMultisample();

	if (m_pRSMRenderer)
		delete m_pRSMRenderer;
	m_pRSMRenderer = NULL;

	if (m_pRSMTargetRenderer)
		delete m_pRSMTargetRenderer;
	m_pRSMTargetRenderer = NULL;
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwPassLPVGI::Render( float i_fSimTime )
{//PROFILE("Render");

	camPassBuffersData passBuffersData;
	m_pCamera->GetPassBuffersParams(passBuffersData);
	float top, bottom, left, right;
	m_pCamera->GetSubViewport(top, bottom, left, right);
	if ( passBuffersData.m_GIBuffer )
	{
		g3dDX11Util::BlendBuffers(m_pRenderTarget,passBuffersData.m_GIBuffer,passBuffersData.m_GIBlendOp, passBuffersData.m_GIIntensity,
								  top, bottom, left, right);
		return 0;
	}

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassLPVGI::Render" );

	D3D11_VIEWPORT vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
	UINT nViewPorts = 1;
	g2dDX11Global::g_pDeviceContext->RSGetViewports( &nViewPorts, vpOld );

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;

	SetupGIParam();
	update_lpv_vertex(m_giParam.m_LPVVolumeSize);
	update_rsm_size(m_giParam.m_LPVRSMSize, l_pReflectiveMap);

	// on entry, i expect a Clear()'ed and BeginScene()'d render target.
	// after i leave, i expect EndScene() and Present() to occur.
	
	// reset d3d state
	g3dDX11Util::release_textures();

	if (m_depthTex != NULL)
	{
		m_depthTex->ClearDepthStencil(1.0f, g2dDX11Global::g_bHasStencil, 0);
		m_depthTex->MakeCurrent();
	}
	m_scratchTex->Clear(maFloatRGBA(0,0,0,0));
	m_scratchTex->MakeCurrent();

	g3dBlendStateMgr::SetBlendState(st_NoBlend);	
	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );
	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	// clear radiance volume firat
	float initColor[4] = {0,0,0,0};
	for (int i = 0; i < 3; i++)
	{
		g2dDX11Global::g_pDeviceContext->ClearRenderTargetView(l_RVRenderTargetView0[i], initColor);
		g2dDX11Global::g_pDeviceContext->ClearRenderTargetView(l_RVRenderTargetView1[i], initColor);
		g2dDX11Global::g_pDeviceContext->ClearRenderTargetView(l_ARVRenderTargetView[i], initColor);
	}

	// Render GI on to scratch texture
	const std::vector<g3dLight*>& lights = g3dLightMgrDX11::Implementation()->GetLights( );
	int num_lights = lights.size();
	int TotalVPLNums = 0;
	float MaxLightScale = 0;

	// pass through all lights and determine how many lights are on
	//for (int il = 0; il < num_lights; il++)
	//{
	//	g3dLight* pLight = lights[il];
	//	if ( !pLight->IsEnabled() ) 
	//		continue;

	//	g3dProjectedLight* proj_light = dynamic_cast<g3dProjectedLight*>(pLight);
	//	// masking mode only uses proj lights?
	//	if ((proj_light == NULL))
	//		continue;

	//	if (proj_light->GetScale() >= MaxLightScale)
	//		MaxLightScale = proj_light->GetScale();

	//	if (!proj_light->GetGIEnabled())
	//		continue;

	//	TotalVPLNums += l_RSMSize * l_RSMSize;
	//	
	//}
	//
	for (int il = 0; il < num_lights; il++)
	{
		g3dLight* pLight = lights[il];
		if ( !pLight->IsEnabled() ) 
			continue;

		g3dProjectedLight* proj_light = dynamic_cast<g3dProjectedLight*>(pLight);
		// masking mode only uses proj lights?
		if ((proj_light == NULL))
			continue;

		if (!proj_light->GetGIEnabled())
			continue;

		// create RSM per light
		// cache the blend state before drawing rsm
		if (m_depthTex != NULL)
		{
			m_depthTex->ClearDepthStencil(1.0f, g2dDX11Global::g_bHasStencil, 0);
			m_depthTex->MakeCurrent();
		}
		g3dBlendStateMgr::BlendState st_OldBlend;
		g3dBlendStateMgr::GetCurrentBlendState( st_OldBlend );
		RenderRSMPerLight(proj_light);
		g3dBlendStateMgr::SetBlendState(&st_OldBlend);

		// downsample RSM, select most important VPL
		DownSampleRSM();

		// Inject RSM to radiance volume
		g3dBlendStateMgr::SetBlendState(st_Blend);	
		g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );
		InjectRSM(m_pScene->GetLayer(0)->GetRootNode(), proj_light, TotalVPLNums, MaxLightScale);
	
		g3dBlendStateMgr::SetBlendState(st_BlendNoAlpha);
	}

	// Propogate Light
	g3dBlendStateMgr::SetBlendState(st_NoBlend);
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );
	int step = max(m_giParam.m_LPVIteration, 0);
	LightPropogation(step, m_pScene->GetLayer(0)->GetRootNode());

	// Apply LPV result into the scene
	// Reset the viewing transform
	g3dSceneRenderUtil::SetViewingTransforms(*m_pCamera);
	g3dBlendStateMgr::SetBlendState(st_NoBlend);
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	if (!m_bDeferRender)
	{
		ApplyLPV(m_pScene->GetLayer(0)->GetRootNode());
		BlendGI(m_pRenderTarget);
	}

	//matTextureMgr::SaveTextureToFile(m_scratchTex, fsLocator(itString("C:\\Projects\\test.png")));
	// restore some state.
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();

	// Restore the Old viewport
	g2dDX11Global::g_pDeviceContext->RSSetViewports( nViewPorts, vpOld );

	D3DPERF_EndEvent();
	return l_nNumTrianglesRendered;
}

void shdwPassLPVGI::DeferRenderForBaking(const sNodePlusState& i_Node)
{
	D3D11_VIEWPORT vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
	UINT nViewPorts = 1;
	g2dDX11Global::g_pDeviceContext->RSGetViewports( &nViewPorts, vpOld );

	g3dSceneRenderUtil::SetViewingTransforms(*m_pCamera);
	g3dBlendStateMgr::SetBlendState(st_Blend);
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	ApplyLPVOneNode(m_pScene->GetLayer(0)->GetRootNode(), i_Node);
	BlendGI(m_pRenderTarget);

	g2dDX11Global::g_pDeviceContext->RSSetViewports( nViewPorts, vpOld );
}

void shdwPassLPVGI::SetupGIParam()
{
	

}

void shdwPassLPVGI::BlurGI(g2dRenderTarget* i_pRenderTarget)
{
		
}

void shdwPassLPVGI::BlendGI(g2dRenderTarget* i_pRenderTarget )
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassLPVGI::BlendGI" );

	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)matShaderMgr::GetSpecialEffect(("HDRLighting.fx"));
	ID3DX11Effect* pEffect = i_pEffect->GetD3DXEffect();

	ID3DX11EffectTechnique* pTechnique = pEffect->GetTechniqueByName("SimpleCopy");

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	// set render target and source texture
	i_pRenderTarget->MakeCurrent();
	int w,h;
	i_pRenderTarget->GetDimensions(w,h);
	ID3D11ShaderResourceView* pResView = g3dDX11TextureUtil::GetD3DTexture(m_scratchTex);

	// turn on stencil - was set up in earlier pass.
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

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

void shdwPassLPVGI::RenderNodes()
{
	int i,n;

	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
	{
		g3dTransparencySortDX11* pTransNodes = m_sceneInfo->GetTransparentNodes();
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
				m_stats.m_nTriangles += RenderNode( NS );
			}
		}
	}
}

int shdwPassLPVGI::RenderNode(const sNodePlusState& i_Node)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(0,0,255,255), L"shdwPassLPVGI::RenderNode" );
	int nTriangles = 0;
	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);

	const matMaterial* pMaterialOveride = i_Node.m_pNode->GetMaterial();
	const g3dFragment* pFrag = i_Node.m_pNode->GetFragment();
	const matMaterial* pOriMaterial = ( pMaterialOveride ) ? pMaterialOveride : pFrag->GetMaterial();

//	g3dFragment* pF = (g3dFragment*)i_pNode->GetFragment();
//	bool ds = pF->GetDoubleSided();
//	pF->SetDoubleSided(false);
	maFloatRGBA diffuseColor(0,0,0,0);
	maFloatRGBA emissiveColor(0,0,0,0);
	matTexture* diffuseMap = NULL;
	float emissiveIntensity = 1.0f;
	
	if ( pOriMaterial )
	{
		if (pOriMaterial->GetShaderParams() != NULL)
		{

			effParamColor* c_emissive = pOriMaterial->GetShaderParams()->FindColorParam("g_emissive");
			if (c_emissive)
			{
				emissiveColor = c_emissive->GetProperty().GetValue();
			}

			effParamFloat* c_emissiveIntensity = pOriMaterial->GetShaderParams()->FindFloatParam("g_emissiveIntensity");
			if (c_emissiveIntensity)
			{
				emissiveIntensity = c_emissiveIntensity->GetProperty().GetValue();
			}

			if (pOriMaterial->GetShaderParams()->m_pDiffuseColor)
				diffuseColor = pOriMaterial->GetShaderParams()->m_pDiffuseColor->GetProperty().GetValue();
			/*effParamColor* c_diffuse = pOriMaterial->GetShaderParams()->FindColorParam("g_diffuse");
			if (c_diffuse)
			{
				diffuseColor = c_diffuse->GetProperty().GetValue();
			}*/

			if (pOriMaterial->GetShaderParams()->m_pDiffuseMap)
				diffuseMap = pOriMaterial->GetShaderParams()->m_pDiffuseMap->GetTexture();
			
			/*effParamTexture* c_diffuseMap = pOriMaterial->GetShaderParams()->FindTextureParam("diffuseMap");
			if (c_diffuseMap)
			{
				diffuseMap = c_diffuseMap->GetTexture();
			}*/

			
		}else if(pOriMaterial->GetEffectData() != NULL)
		{
			diffuseColor += pOriMaterial->GetEffectData()->GetEmissive();
			diffuseColor += pOriMaterial->GetEffectData()->GetDiffuse();
		}
	}

	// resolve material/effect
	const matMaterial* pMaterial = &l_InjectionMat;//g3dDX11Util::GetMaterial(i_pNode);
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	ID3DX11Effect* pDXEffect = ((effShaderBaseDX11*)pEffect)->GetD3DXEffect();

	pEffect->SetTechnique("GI");

	if (pDXEffect)
	{
		pDXEffect->GetVariableByName("g_emissiveColor")->AsVector()->SetFloatVector(emissiveColor.Ptr());
		pDXEffect->GetVariableByName("g_emissiveIntensity")->AsScalar()->SetFloat(emissiveIntensity);
		pDXEffect->GetVariableByName("g_diffuseColor")->AsVector()->SetFloatVector(diffuseColor.Ptr());
		pDXEffect->GetVariableByName("hasDiffuseMap")->AsScalar()->SetBool((diffuseMap) ? true : false);
		pDXEffect->GetVariableByName("g_diffuseMap")->AsShaderResource()->SetResource(
			g3dDX11TextureUtil::GetD3DTexture(diffuseMap));
		pDXEffect->GetVariableByName("g_bIsReceivesGI")->AsScalar()->SetBool(pFrag->GetReceivesGI());
	}

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

void shdwPassLPVGI::RenderRSMPerLight(const g3dProjectedLight* i_pProjLight)
{
	//if (m_giParam.m_RSMSize != l_RSMSize)
	//{	
	//	if (l_pReflectiveMap)
	//		matTextureMgr::ReleaseTexture(l_pReflectiveMap);
	//	l_pReflectiveMap = NULL;
	//	l_pReflectiveMap = matTextureMgr::CreateReflectiveShadowMap( m_giParam.m_RSMSize, m_giParam.m_RSMSize );	
	//	l_RSMSize = m_giParam.m_RSMSize;

	//	/*if (m_pDownSampledRSM)
	//		matTextureMgr::ReleaseTexture(m_pDownSampledRSM);
	//	m_pDownSampledRSM = NULL;
	//	m_pDownSampledRSM = matTextureMgr::CreateReflectiveShadowMap(m_giParam.m_RSMSize / 2, m_giParam.m_RSMSize / 2);*/
	//}
	camCamera projCamera;
	i_pProjLight->OrientCamera(projCamera);

	g2dRenderTarget* pTarget = l_pReflectiveMap->GetRenderTargetAPI();
	pTarget->Clear(maFloatRGBA(0,0,0,0));
	m_pRSMTargetRenderer->SetTarget(pTarget);
	m_pRSMTargetRenderer->SetScene(const_cast<g3dScene*>(m_pScene));
	m_pRSMTargetRenderer->SetCamera(&projCamera);
	m_pRSMTargetRenderer->SetRenderer(m_pRSMRenderer);

	m_pRSMTargetRenderer->Render(0);
	//m_pRSMTargetRenderer->SetTarget();
}

void shdwPassLPVGI::DownSampleRSM()
{
}

void shdwPassLPVGI::InjectRSM(const g3dSceneNode* i_rootNode, const g3dProjectedLight* i_pProjLight,
							  int i_totalVPLNum, float i_maxLightScale)
{
	

	// Setup the viewport to match the backbuffer
	D3D11_VIEWPORT vp;
	vp.Width = (float)l_LPVCellNum;
	vp.Height = (float)l_LPVCellNum;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g2dDX11Global::g_pDeviceContext->RSSetViewports( 1, &vp );

	g2dDX11Global::g_pDeviceContext->OMSetRenderTargets( 3, l_RVRenderTargetView0, NULL ); 

	// Setup shader variables
	const matMaterial* pMaterial = &l_InjectionMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	ID3DX11Effect* pDXEffect = ((effShaderBaseDX11*)pEffect)->GetD3DXEffect();
	ID3DX11EffectTechnique* pEffectTechnique = pDXEffect->GetTechniqueByName("Default");

	maAxisBox worldBox = i_rootNode->GetWorldBox();
	float volumeOrigin[4] = {worldBox.GetMinX(), worldBox.GetMinY(), worldBox.GetMinZ(), 1.0f};
	float invVolumeWidth[4] = {1.0f/worldBox.GetDiffX(), 1.0f/worldBox.GetDiffY(), 1.0f/worldBox.GetDiffZ(), 1.0f};
	int cells = l_LPVCellNum * l_LPVCellNum;
	//float pointWeight = (float) cells / ((float) l_RSMSize * l_RSMSize) * pow(i_pProjLight->GetScale() / i_maxLightScale, 2.0f);
	float pointWeight = (float) cells / ((float) l_RSMSize * l_RSMSize);
	//float pointWeight = (float) 1.0f / ((float) l_RSMSize) * pow(i_pProjLight->GetScale() / i_maxLightScale, 1.0f);
	
	pDXEffect->GetVariableByName("g_VolumeOrigin")->AsVector()->SetFloatVector(volumeOrigin);
	pDXEffect->GetVariableByName("g_InvVolumeWidth")->AsVector()->SetFloatVector(invVolumeWidth);
	//pDXEffect->GetVariableByName("g_Cells")->AsScalar()->SetInt(cells);
	pDXEffect->GetVariableByName("g_LightPos")->AsVector()->SetFloatVector(i_pProjLight->GetPosition().Ptr());
	pDXEffect->GetVariableByName("g_PointWeight")->AsScalar()->SetFloat(pointWeight);
	pDXEffect->GetVariableByName("g_TexelNum")->AsScalar()->SetInt(l_LPVCellNum);
	pDXEffect->GetVariableByName("g_TexelSize")->AsScalar()->SetFloat(1.0f / (float)(l_LPVCellNum));
	//pDXEffect->GetVariableByName("g_RSMSize")->AsScalar()->SetInt(l_RSMSize);
	maFloatRGBA lightColor = i_pProjLight->GetIntensity() * i_pProjLight->GetIntensityFactor();
	pDXEffect->GetVariableByName("g_LightColor")->AsVector()->SetFloatVector(lightColor.Ptr());
	
	// apply texture
	matReflectiveShadowMap* rsmMap = dynamic_cast<matReflectiveShadowMap*>(l_pReflectiveMap);
	pDXEffect->GetVariableByName("projRSMPosMap")->AsShaderResource()->SetResource(rsmMap->GetShaderView(0));
	pDXEffect->GetVariableByName("projRSMNormalMap")->AsShaderResource()->SetResource(rsmMap->GetShaderView(1));
	pDXEffect->GetVariableByName("projRSMFluxMap")->AsShaderResource()->SetResource(rsmMap->GetShaderView(2));

	pEffectTechnique->GetPassByIndex(0)->Apply(0, g2dDX11Global::g_pDeviceContext);

	// draw vertice
	UINT strides = sizeof( SCREEN_VERTEX );
	UINT offsets = 0;
	ID3D11Buffer* pBuffers[1] = { l_VPLVertexVB };

	drawVolumeVertex(l_VPLVertexVB, l_RSMSize * l_RSMSize);

	ID3D11ShaderResourceView* nullTex[3] = { NULL, NULL, NULL };
	g2dDX11Global::g_pDeviceContext->VSSetShaderResources(0, 3, nullTex);
	g2dDX11Global::g_pDeviceContext->GSSetShaderResources(0, 3, nullTex);
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 3, nullTex);

	// hack to restore GS state
	g2dDX11Global::g_pDeviceContext->GSSetShader(NULL, NULL, 0);

	/*D3DX11SaveTextureToFile(g2dDX11Global::g_pDeviceContext, l_RVTex0[0], D3DX11_IFF_DDS, L"C:\\Projects\\RVVolumeR.dds");
	D3DX11SaveTextureToFile(g2dDX11Global::g_pDeviceContext, l_RVTex0[1], D3DX11_IFF_DDS, L"C:\\Projects\\RVVolumeG.dds");
	D3DX11SaveTextureToFile(g2dDX11Global::g_pDeviceContext, l_RVTex0[2], D3DX11_IFF_DDS, L"C:\\Projects\\RVVolumeB.dds");*/
}

void shdwPassLPVGI::LightPropogation(int i_NumIteration, const g3dSceneNode* i_rootNode)
{
	ID3D11ShaderResourceView** currentSRV = NULL;
	ID3D11ShaderResourceView** renderedSRV = NULL;
	ID3D11RenderTargetView** currentRTV = NULL;

	const matMaterial* pMaterial = &l_InjectionMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	ID3DX11Effect* pDXEffect = ((effShaderBaseDX11*)pEffect)->GetD3DXEffect();
	ID3DX11EffectTechnique* pEffectTechnique = pDXEffect->GetTechniqueByName("RVPropagation");
	ID3DX11EffectTechnique* pEffectTechniqueAccum = pDXEffect->GetTechniqueByName("AccumRV");

	maAxisBox worldBox = i_rootNode->GetWorldBox();
	float CellWidth[3] = {worldBox.GetDiffX() / l_LPVCellNum, 
							worldBox.GetDiffY() / l_LPVCellNum, 
							worldBox.GetDiffZ() / l_LPVCellNum};

	for (int i = 0; i < i_NumIteration; i++)
	{
		if (i % 2)
		{
			currentSRV = l_RVShaderResourceView1;
			currentRTV = l_RVRenderTargetView0;
			renderedSRV = l_RVShaderResourceView0;
			m_bSwapedBuffer = false;
		}
		else
		{
			currentSRV = l_RVShaderResourceView0;
			currentRTV = l_RVRenderTargetView1;
			renderedSRV = l_RVShaderResourceView1;
			m_bSwapedBuffer = true;
		}
		
		D3D11_VIEWPORT vp;
		vp.Width = (float)l_LPVCellNum;
		vp.Height = (float)l_LPVCellNum;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		g2dDX11Global::g_pDeviceContext->RSSetViewports( 1, &vp );

		g2dDX11Global::g_pDeviceContext->OMSetRenderTargets( 3, currentRTV, NULL );

		// clear radiance volume firat
		float initColor[4] = {0,0,0,0};
		for (int i = 0; i < 3; i++)
		{
			g2dDX11Global::g_pDeviceContext->ClearRenderTargetView(currentRTV[i], initColor);
		}

		// Setup shader variables
		pDXEffect->GetVariableByName("RVVolumeMapR")->AsShaderResource()->SetResource(currentSRV[0]);
		pDXEffect->GetVariableByName("RVVolumeMapG")->AsShaderResource()->SetResource(currentSRV[1]);
		pDXEffect->GetVariableByName("RVVolumeMapB")->AsShaderResource()->SetResource(currentSRV[2]);
		pDXEffect->GetVariableByName("g_TexelSize")->AsScalar()->SetFloat(1.0f / (float)(l_LPVCellNum));
		pDXEffect->GetVariableByName("g_TexelNum")->AsScalar()->SetInt(l_LPVCellNum);
		pDXEffect->GetVariableByName("g_CellWidth")->AsVector()->SetFloatVector(CellWidth);
		pDXEffect->GetVariableByName("g_GIFalloff")->AsScalar()->SetFloat(m_giParam.m_LPVGIFalloff);

		pEffectTechnique->GetPassByIndex(0)->Apply(0, g2dDX11Global::g_pDeviceContext);

		g3dBlendStateMgr::SetBlendState(st_NoBlend);
		drawVolumeVertex(l_RVVertexVB, l_LPVCellNum * l_LPVCellNum);

		ID3D11ShaderResourceView* nullTex[3] = { NULL, NULL, NULL };
		g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 3, nullTex);

		// Accumlate RV result
		g2dDX11Global::g_pDeviceContext->OMSetRenderTargets( 3, l_ARVRenderTargetView, NULL );
		pDXEffect->GetVariableByName("g_TexelNum")->AsScalar()->SetInt(l_LPVCellNum);
		pDXEffect->GetVariableByName("RVVolumeMapR")->AsShaderResource()->SetResource(renderedSRV[0]);
		pDXEffect->GetVariableByName("RVVolumeMapG")->AsShaderResource()->SetResource(renderedSRV[1]);
		pDXEffect->GetVariableByName("RVVolumeMapB")->AsShaderResource()->SetResource(renderedSRV[2]);
		pEffectTechniqueAccum->GetPassByIndex(0)->Apply(0, g2dDX11Global::g_pDeviceContext);

		g3dBlendStateMgr::SetBlendState(st_Blend);
		drawVolumeVertex(l_RVVertexVB, l_LPVCellNum * l_LPVCellNum);

		//ID3D11ShaderResourceView* nullTex[3] = { NULL, NULL, NULL };
		g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 3, nullTex);
	}

	//D3DX11SaveTextureToFile(g2dDX11Global::g_pDeviceContext, l_RVTex0[1], D3DX11_IFF_DDS, L"C:\\Projects\\RVVolume_Propagate1.dds");
	//D3DX11SaveTextureToFile(g2dDX11Global::g_pDeviceContext, l_RVTex1[1], D3DX11_IFF_DDS, L"C:\\Projects\\RVVolume_Propagate2.dds");

	// hack to restore GS state
	g2dDX11Global::g_pDeviceContext->GSSetShader(NULL, NULL, 0);
}

void shdwPassLPVGI::ApplyLPV(const g3dSceneNode* i_rootNode)
{
	if (m_depthTex != NULL)
	{
		m_depthTex->ClearDepthStencil(1.0f, g2dDX11Global::g_bHasStencil, 0);
		m_depthTex->MakeCurrent();
	}

	m_scratchTex->Clear(maFloatRGBA(0,0,0,0));
	m_scratchTex->MakeCurrent();

	const matMaterial* pMaterial = &l_InjectionMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	ID3DX11Effect* pDXEffect = ((effShaderBaseDX11*)pEffect)->GetD3DXEffect();

	ID3D11ShaderResourceView** currentSRV = NULL;
	if (m_bSwapedBuffer)
	{
		currentSRV = l_RVShaderResourceView1;
	}else
	{
		currentSRV = l_RVShaderResourceView0;
	}

	/*pDXEffect->GetVariableByName("RVVolumeMapR")->AsShaderResource()->SetResource(currentSRV[0]);
	pDXEffect->GetVariableByName("RVVolumeMapG")->AsShaderResource()->SetResource(currentSRV[1]);
	pDXEffect->GetVariableByName("RVVolumeMapB")->AsShaderResource()->SetResource(currentSRV[2]);*/
	pDXEffect->GetVariableByName("RVVolumeMapR")->AsShaderResource()->SetResource(l_ARVShaderResourceView[0]);
	pDXEffect->GetVariableByName("RVVolumeMapG")->AsShaderResource()->SetResource(l_ARVShaderResourceView[1]);
	pDXEffect->GetVariableByName("RVVolumeMapB")->AsShaderResource()->SetResource(l_ARVShaderResourceView[2]);

	maAxisBox worldBox = i_rootNode->GetWorldBox();
	float volumeOrigin[4] = {worldBox.GetMinX(), worldBox.GetMinY(), worldBox.GetMinZ(), 1.0f};
	float invVolumeWidth[4] = {1.0f/worldBox.GetDiffX(), 1.0f/worldBox.GetDiffY(), 1.0f/worldBox.GetDiffZ(), 1.0f};
	
	pDXEffect->GetVariableByName("g_VolumeOrigin")->AsVector()->SetFloatVector(volumeOrigin);
	pDXEffect->GetVariableByName("g_InvVolumeWidth")->AsVector()->SetFloatVector(invVolumeWidth);
	pDXEffect->GetVariableByName("g_GIScale")->AsScalar()->SetFloat(m_giParam.m_LPVScale);
	pDXEffect->GetVariableByName("g_TexelNum")->AsScalar()->SetInt(l_LPVCellNum);
	pDXEffect->GetVariableByName("g_TexelSize")->AsScalar()->SetFloat(1.0f / 1.0f / (float)(l_LPVCellNum));

	RenderNodes();

	//matTextureMgr::SaveTextureToFile(m_scratchTex, fsLocator(itString("C:\\Projects\\GI.png")));

	ID3D11ShaderResourceView* nullTex[3] = { NULL, NULL, NULL };
	g2dDX11Global::g_pDeviceContext->VSSetShaderResources(0, 3, nullTex);
	g2dDX11Global::g_pDeviceContext->GSSetShaderResources(0, 3, nullTex);
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 3, nullTex);
}

void shdwPassLPVGI::ApplyLPVOneNode(const g3dSceneNode* i_rootNode, const sNodePlusState& i_Node)
{
	if (m_depthTex != NULL)
	{
		m_depthTex->ClearDepthStencil(1.0f, g2dDX11Global::g_bHasStencil, 0);
		m_depthTex->MakeCurrent();
	}

	m_scratchTex->Clear(maFloatRGBA(0,0,0,0));
	m_scratchTex->MakeCurrent();

	const matMaterial* pMaterial = &l_InjectionMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	ID3DX11Effect* pDXEffect = ((effShaderBaseDX11*)pEffect)->GetD3DXEffect();

	ID3D11ShaderResourceView** currentSRV = NULL;
	if (m_bSwapedBuffer)
	{
		currentSRV = l_RVShaderResourceView1;
	}else
	{
		currentSRV = l_RVShaderResourceView0;
	}

	/*pDXEffect->GetVariableByName("RVVolumeMapR")->AsShaderResource()->SetResource(currentSRV[0]);
	pDXEffect->GetVariableByName("RVVolumeMapG")->AsShaderResource()->SetResource(currentSRV[1]);
	pDXEffect->GetVariableByName("RVVolumeMapB")->AsShaderResource()->SetResource(currentSRV[2]);*/
	pDXEffect->GetVariableByName("RVVolumeMapR")->AsShaderResource()->SetResource(l_ARVShaderResourceView[0]);
	pDXEffect->GetVariableByName("RVVolumeMapG")->AsShaderResource()->SetResource(l_ARVShaderResourceView[1]);
	pDXEffect->GetVariableByName("RVVolumeMapB")->AsShaderResource()->SetResource(l_ARVShaderResourceView[2]);

	maAxisBox worldBox = i_rootNode->GetWorldBox();
	float volumeOrigin[4] = {worldBox.GetMinX(), worldBox.GetMinY(), worldBox.GetMinZ(), 1.0f};
	float invVolumeWidth[4] = {1.0f/worldBox.GetDiffX(), 1.0f/worldBox.GetDiffY(), 1.0f/worldBox.GetDiffZ(), 1.0f};
	
	pDXEffect->GetVariableByName("g_VolumeOrigin")->AsVector()->SetFloatVector(volumeOrigin);
	pDXEffect->GetVariableByName("g_InvVolumeWidth")->AsVector()->SetFloatVector(invVolumeWidth);
	pDXEffect->GetVariableByName("g_GIScale")->AsScalar()->SetFloat(m_giParam.m_LPVScale);
	pDXEffect->GetVariableByName("g_TexelNum")->AsScalar()->SetInt(l_LPVCellNum);
	pDXEffect->GetVariableByName("g_TexelSize")->AsScalar()->SetFloat(1.0f / 1.0f / (float)(l_LPVCellNum));

	RenderNode(i_Node);

	//matTextureMgr::SaveTextureToFile(m_scratchTex, fsLocator(itString("C:\\Projects\\GI.png")));

	ID3D11ShaderResourceView* nullTex[3] = { NULL, NULL, NULL };
	g2dDX11Global::g_pDeviceContext->VSSetShaderResources(0, 3, nullTex);
	g2dDX11Global::g_pDeviceContext->GSSetShaderResources(0, 3, nullTex);
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 3, nullTex);
}

void shdwPassLPVGI::drawVolumeVertex(ID3D11Buffer* i_buffer, int i_numVertex)
{
	// draw vertice
	UINT strides = sizeof( SCREEN_VERTEX );
	UINT offsets = 0;
	ID3D11Buffer* pBuffers[1] = { i_buffer };

	g2dDX11Global::g_pDeviceContext->IASetInputLayout( l_VPLVertexLayout );
	g2dDX11Global::g_pDeviceContext->IASetVertexBuffers( 0, 1, pBuffers, &strides, &offsets );
	g2dDX11Global::g_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	g2dDX11Global::g_pDeviceContext->Draw( i_numVertex, 0 );
}