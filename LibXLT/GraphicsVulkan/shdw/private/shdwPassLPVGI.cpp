/****************************************************************************\
**	shdwPassLPVGI.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSLPVGI_HPP
#include "GraphicsVulkan/shdw/shdwPassLPVGI.hpp"
#endif

#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/g3d/g3dProjectedLight.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#endif
#ifndef G3D_TARGETRENDERER_HPP
#include "Graphics/g3d/g3dTargetRenderer.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/mat/matTextureMgr.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_G3DVKBUFFERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkBufferUtil.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#ifndef GVK_MATREFLECTIVESHADOWMAP_HPP
#include "GraphicsVulkan/Mat/matReflectiveShadowMap.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_FXEFFECTVK_HPP
#include "GraphicsVulkan/Fx/fxEffectVk.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

#include <algorithm>
#include <memory>

namespace gvk
{

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

	std::unique_ptr<g3dVkBuffer>		l_VPLVertexVB;
	std::unique_ptr<g3dVkBuffer>		l_RVVertexVB;
	std::unique_ptr<vkdVertexLayout>	l_VPLVertexLayout;

	//	The radiance volumes (3D images, one per colour channel) and their
	//	ping-pong and accumulation copies; DX11's shader resource views are
	//	the images, its render target views their layered views
	std::unique_ptr<vkdImage> l_RVTex0[3];		// 3d texture for storing radiance volume
	std::unique_ptr<vkdImage> l_RVTex1[3];		// allocate ping-pong buff
	std::unique_ptr<vkdImage> l_AccumRVTex[3];
	vkdImage* l_RVShaderResourceView0[3] = {NULL, NULL, NULL};
	vkdImage* l_RVRenderTargetView0[3] = {NULL, NULL, NULL};
	vkdImage* l_RVShaderResourceView1[3] = {NULL, NULL, NULL};
	vkdImage* l_RVRenderTargetView1[3] = {NULL, NULL, NULL};
	vkdImage* l_ARVShaderResourceView[3] = {NULL, NULL, NULL};
	vkdImage* l_ARVRenderTargetView[3] = {NULL, NULL, NULL};

	int l_LPVCellNum = 32;

	std::unique_ptr<vkdImage> make_volume(const char* i_pName)
	{
		vkdImageDesc desc;
		desc.m_Type = VK_IMAGE_TYPE_3D;
		desc.m_Format = VK_FORMAT_R32G32B32A32_SFLOAT;
		desc.m_Width = desc.m_Height = desc.m_Depth = (uint32_t)l_LPVCellNum;
		desc.m_Usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		desc.m_bLayeredTarget = true;
		return vkdImage::Create(vkdGlobal::Device(), desc, i_pName);
	}

	void init_lpv_texture()
	{
		// create the volumes, their shader views and render target views
		for (int i = 0; i < 3; i++)
		{
			l_RVTex0[i] = make_volume("LPV radiance volume 0");
			l_RVTex1[i] = make_volume("LPV radiance volume 1");
			l_AccumRVTex[i] = make_volume("LPV accumulated radiance volume");

			l_RVShaderResourceView0[i] = l_RVTex0[i].get();
			l_RVShaderResourceView1[i] = l_RVTex1[i].get();
			l_ARVShaderResourceView[i] = l_AccumRVTex[i].get();

			l_RVRenderTargetView0[i] = l_RVTex0[i]->LayeredView();
			l_RVRenderTargetView1[i] = l_RVTex1[i]->LayeredView();
			l_ARVRenderTargetView[i] = l_AccumRVTex[i]->LayeredView();
		}
	}

	void release_lpv_texture()
	{
		for (int i = 0; i < 3; i++)
		{
			l_RVShaderResourceView0[i] = l_RVRenderTargetView0[i] = NULL;
			l_RVShaderResourceView1[i] = l_RVRenderTargetView1[i] = NULL;
			l_ARVShaderResourceView[i] = l_ARVRenderTargetView[i] = NULL;
			l_RVTex0[i].reset();
			l_RVTex1[i].reset();
			l_AccumRVTex[i].reset();
		}
	}

	//	The points of an i_Size x i_Size grid over the target, one per
	//	texel (VPLs of the RSM, cells of a volume slice)
	SCREEN_VERTEX* make_grid(int i_Size)
	{
		float step = 2.0f / (float)(i_Size);
		float halfStep = step / 2.0f;
		SCREEN_VERTEX* vertices = new SCREEN_VERTEX[i_Size * i_Size];
		for (int y = 0; y < i_Size; y++)
		{
			for (int x = 0; x < i_Size; x++)
			{
				int index = y * i_Size + x;
				vertices[index].pos = maVector4d( -1.0f + (float)x * step + halfStep, 1.0f - (float)y * step - halfStep, 0.5f, 1.0f );
				vertices[index].uv = maPoint2d( (float)x * halfStep, (float)y * halfStep);
			}
		}
		return vertices;
	}

	void init_vplvertex()
	{
		if (!l_VPLVertexLayout)
		{
			// the input layout, corresponds to the struct in the shader
			l_VPLVertexLayout.reset(new vkdVertexLayout({ { "SV_POSITION", 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0, 0 },
														  { "TEXCOORD", 0, VK_FORMAT_R32G32_SFLOAT, 0, 16 } },
														{ (uint32_t)sizeof(SCREEN_VERTEX) }));

			matShaderMgr::SetUseShaderArray(true);
			effShaderBaseVk* pDefaultEffect = (effShaderBaseVk*)matShaderMgr::GetSpecialEffect("LPV_GI.fx");
			DBG_ASSERT(pDefaultEffect, "effects not initialized yet.");
		}

		// create fixed vpl vertex buffer
		if (!l_VPLVertex)
		{
			l_VPLVertex = make_grid(l_RSMSize);
		}

		if (!l_VPLVertexVB)
		{
			l_VPLVertexVB.reset(new g3dVkBuffer(g3dVkBuffer::e_Vertex, l_RSMSize * l_RSMSize * sizeof( SCREEN_VERTEX ),
												false, l_VPLVertex, "LPV VPL points"));
		}

		// create fixed rv vertex buffer
		if (!l_RVVertex)
		{
			l_RVVertex = make_grid(l_LPVCellNum);
		}

		if (!l_RVVertexVB)
		{
			l_RVVertexVB.reset(new g3dVkBuffer(g3dVkBuffer::e_Vertex, l_LPVCellNum * l_LPVCellNum * sizeof( SCREEN_VERTEX ),
											   false, l_RVVertex, "LPV volume points"));
		}
	}

	void update_lpv_vertex(int i_LPVCellNum)
	{
		if (i_LPVCellNum == l_LPVCellNum)
			return;

		if (l_RVVertex)
			delete [] l_RVVertex;
		l_RVVertex = NULL;
		l_RVVertexVB.reset();

		l_LPVCellNum = i_LPVCellNum;
		l_LPVCellNum = std::min(l_LPVCellNum, 1024);

		// create fixed rv vertex buffer
		if (!l_RVVertex)
		{
			l_RVVertex = make_grid(l_LPVCellNum);
		}

		if (!l_RVVertexVB)
		{
			l_RVVertexVB.reset(new g3dVkBuffer(g3dVkBuffer::e_Vertex, l_LPVCellNum * l_LPVCellNum * sizeof( SCREEN_VERTEX ),
											   false, l_RVVertex, "LPV volume points"));
		}

		//	the old volumes stay until the GPU is done with them (vkdImage
		//	defers its destruction)
		release_lpv_texture();
		init_lpv_texture();
	}

	void update_rsm_size(int i_RSMSize, matTexture*& i_rsmap)
	{
		if (i_RSMSize == l_RSMSize && i_rsmap != NULL)
			return;

		l_RSMSize = i_RSMSize;

		if (i_rsmap)
			matTextureMgr::ReleaseTexture(i_rsmap);
		i_rsmap = matTextureMgr::CreateReflectiveShadowMap( l_RSMSize, l_RSMSize );

		if (l_VPLVertex)
			delete [] l_VPLVertex;
		l_VPLVertex = NULL;
		l_VPLVertexVB.reset();

		// create fixed vpl vertex buffer
		if (!l_VPLVertex)
		{
			l_VPLVertex = make_grid(l_RSMSize);
		}

		if (!l_VPLVertexVB)
		{
			l_VPLVertexVB.reset(new g3dVkBuffer(g3dVkBuffer::e_Vertex, l_RSMSize * l_RSMSize * sizeof( SCREEN_VERTEX ),
												false, l_VPLVertex, "LPV VPL points"));
		}
	}

	void cleanup()
	{
		if (l_pReflectiveMap)
			matTextureMgr::ReleaseTexture(l_pReflectiveMap);
		l_pReflectiveMap = NULL;

		release_lpv_texture();

		l_VPLVertexVB.reset();
		l_VPLVertexLayout.reset();
		l_RVVertexVB.reset();

		if (l_VPLVertex)
			delete [] l_VPLVertex;
		l_VPLVertex = NULL;

		if (l_RVVertex)
			delete [] l_RVVertex;
		l_RVVertex = NULL;
	}
}//namespace

void shdwPassLPVGI::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );

	const VkColorComponentFlags c_All = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	const VkColorComponentFlags c_RGB = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
	st_Blend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		c_All);
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		c_All);
	st_BlendNoAlpha = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		c_RGB );
	st_MulColorBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_DST_COLOR,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		c_RGB );
	st_PropagateBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		c_All );

	init_lpv_texture();
	init_vplvertex();
}

void shdwPassLPVGI::CleanupStates()
{
	delete ds_Test_Write_LessE_NS; ds_Test_Write_LessE_NS = NULL;
	delete ds_Disable_NS; ds_Disable_NS = NULL;
	delete st_Blend; st_Blend = NULL;
	delete st_NoBlend; st_NoBlend = NULL;
	delete st_BlendNoAlpha; st_BlendNoAlpha = NULL;
	delete st_MulColorBlend; st_MulColorBlend = NULL;
	delete st_PropagateBlend; st_PropagateBlend = NULL;

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
:	m_sceneInfo(NULL),
	m_pCamera(i_pCamera),
	m_pScene(i_pScene),
	m_giParam(i_SSGIParams),
	m_scratchTex(i_Scratch),
	m_depthTex(i_Depth),
	m_pRSMRenderer(NULL),
	m_pRSMTargetRenderer(NULL),
	m_bSwapedBuffer(false),
	m_bDeferRender(i_bIsDeferRender)
{
	m_pRenderTarget = i_Target;

	// lazy init so that this material can be reused across instantiations.
	if (!l_InjectionMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(std::string("Special/LPV_GI.fx"), matShaderMgr::GetSpecialEffect("LPV_GI.fx"));
		l_InjectionMat.SetShaderParams(p);
	}

	m_pRSMRenderer = g3dSceneRendererCreate::CreateRSMRenderer();
	m_pRSMTargetRenderer = new g3dTargetRenderer();
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
	G3D_PROFILE_SCOPE("PassLPVGI");

	camPassBuffersData passBuffersData;
	m_pCamera->GetPassBuffersParams(passBuffersData);
	float top, bottom, left, right;
	m_pCamera->GetSubViewport(top, bottom, left, right);
	if ( passBuffersData.m_GIBuffer )
	{
		g3dVkUtil::BlendBuffers(m_pRenderTarget,passBuffersData.m_GIBuffer,passBuffersData.m_GIBlendOp, passBuffersData.m_GIIntensity,
								  top, bottom, left, right);
		return 0;
	}

	vkdContext& context = vkdGlobal::Context();
	vkdViewport vpOld[vkdContext::c_MaxViewports];
	const uint32_t nViewPorts = context.GetViewports( vpOld );

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;

	SetupGIParam();
	update_lpv_vertex(m_giParam.m_LPVVolumeSize);
	update_rsm_size(m_giParam.m_LPVRSMSize, l_pReflectiveMap);

	// on entry, i expect a Clear()'ed and BeginScene()'d render target.
	// after i leave, i expect EndScene() and Present() to occur.

	// reset d3d state
	g3dVkUtil::release_textures();

	if (m_depthTex != NULL)
	{
		m_depthTex->ClearDepthStencil(1.0f, g2dVkGlobal::HasStencil(), 0);
		m_depthTex->MakeCurrent();
	}
	m_scratchTex->Clear(maFloatRGBA(0,0,0,0));
	m_scratchTex->MakeCurrent();

	g3dBlendStateMgr::SetBlendState(st_NoBlend);
	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );
	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// clear radiance volume firat
	float initColor[4] = {0,0,0,0};
	for (int i = 0; i < 3; i++)
	{
		context.ClearRenderTarget(l_RVRenderTargetView0[i], initColor);
		context.ClearRenderTarget(l_RVRenderTargetView1[i], initColor);
		context.ClearRenderTarget(l_ARVRenderTargetView[i], initColor);
	}

	// Render GI on to scratch texture
	const std::vector<g3dLight*>& lights = g3dLightMgrVk::Implementation()->GetLights( );
	int num_lights = (int)lights.size();
	int TotalVPLNums = 0;
	float MaxLightScale = 0;

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
			m_depthTex->ClearDepthStencil(1.0f, g2dVkGlobal::HasStencil(), 0);
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
	int step = std::max(m_giParam.m_LPVIteration, 0);
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

	// restore some state.
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	// Restore the Old viewport
	context.SetViewports( nViewPorts, vpOld );

	return l_nNumTrianglesRendered;
}

void shdwPassLPVGI::DeferRenderForBaking(const sNodePlusState& i_Node)
{
	vkdContext& context = vkdGlobal::Context();
	vkdViewport vpOld[vkdContext::c_MaxViewports];
	const uint32_t nViewPorts = context.GetViewports( vpOld );

	g3dSceneRenderUtil::SetViewingTransforms(*m_pCamera);
	g3dBlendStateMgr::SetBlendState(st_Blend);
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	ApplyLPVOneNode(m_pScene->GetLayer(0)->GetRootNode(), i_Node);
	BlendGI(m_pRenderTarget);

	context.SetViewports( nViewPorts, vpOld );
}

void shdwPassLPVGI::SetupGIParam()
{


}

void shdwPassLPVGI::BlurGI(g2dRenderTarget* i_pRenderTarget)
{

}

void shdwPassLPVGI::BlendGI(g2dRenderTarget* i_pRenderTarget )
{
	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");

	int technique = pEffect->FindTechnique("SimpleCopy");

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// set render target and source texture
	i_pRenderTarget->MakeCurrent();
	int w,h;
	i_pRenderTarget->GetDimensions(w,h);
	vkdImage* pResView = g3dVkTextureUtil::GetImage(m_scratchTex);

	// turn on stencil - was set up in earlier pass.
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dBlendStateMgr::SetBlendState(st_BlendNoAlpha);

	pEffect->Apply(technique, 0);
	vkdGlobal::Context().SetShaderResources( 0, 1, &pResView );
	g3dVkUtil::DrawFullScreenQuad( w,h );

	// restore stencil state
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// unbind texture from shader so that it can be set as rendertarget later.
	vkdImage* nullPSR[2] = {NULL, NULL};
	vkdGlobal::Context().SetShaderResources(0,2,nullPSR);
}

void shdwPassLPVGI::RenderNodes()
{
	int i,n;

	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	n = (int)nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	n = (int)shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	n = (int)nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
	{
		g3dTransparencySortVk* pTransNodes = m_sceneInfo->GetTransparentNodes();
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
	int nTriangles = 0;
	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	const matMaterial* pMaterialOveride = i_Node.m_pNode->GetMaterial();
	const g3dFragment* pFrag = i_Node.m_pNode->GetFragment();
	const matMaterial* pOriMaterial = ( pMaterialOveride ) ? pMaterialOveride : pFrag->GetMaterial();

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

			if (pOriMaterial->GetShaderParams()->m_pDiffuseMap)
				diffuseMap = pOriMaterial->GetShaderParams()->m_pDiffuseMap->GetTexture();

		}else if(pOriMaterial->GetEffectData() != NULL)
		{
			diffuseColor += pOriMaterial->GetEffectData()->GetEmissive();
			diffuseColor += pOriMaterial->GetEffectData()->GetDiffuse();
		}
	}

	// resolve material/effect
	const matMaterial* pMaterial = &l_InjectionMat;//g3dVkUtil::GetMaterial(i_pNode);
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	fxEffect* pDXEffect = ((effShaderBaseVk*)pEffect)->GetFxEffect();

	pEffect->SetTechnique("GI");

	if (pDXEffect)
	{
		pDXEffect->GetVariableByName("g_emissiveColor")->AsVector()->SetFloatVector(emissiveColor.Ptr());
		pDXEffect->GetVariableByName("g_emissiveIntensity")->AsScalar()->SetFloat(emissiveIntensity);
		pDXEffect->GetVariableByName("g_diffuseColor")->AsVector()->SetFloatVector(diffuseColor.Ptr());
		pDXEffect->GetVariableByName("hasDiffuseMap")->AsScalar()->SetBool((diffuseMap) ? true : false);
		pDXEffect->GetVariableByName("g_diffuseMap")->AsShaderResource()->SetResource(
			g3dVkTextureUtil::GetImage(diffuseMap));
		pDXEffect->GetVariableByName("g_bIsReceivesGI")->AsScalar()->SetBool(pFrag->GetReceivesGI());
	}

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	return nTriangles;
}

void shdwPassLPVGI::RenderRSMPerLight(const g3dProjectedLight* i_pProjLight)
{
	camCamera projCamera;
	i_pProjLight->OrientCamera(projCamera);

	g2dRenderTarget* pTarget = l_pReflectiveMap->GetRenderTargetAPI();
	pTarget->Clear(maFloatRGBA(0,0,0,0));
	m_pRSMTargetRenderer->SetTarget(pTarget);
	m_pRSMTargetRenderer->SetScene(const_cast<g3dScene*>(m_pScene));
	m_pRSMTargetRenderer->SetCamera(&projCamera);
	m_pRSMTargetRenderer->SetRenderer(m_pRSMRenderer);

	m_pRSMTargetRenderer->Render(0);
}

void shdwPassLPVGI::DownSampleRSM()
{
}

void shdwPassLPVGI::InjectRSM(const g3dSceneNode* i_rootNode, const g3dProjectedLight* i_pProjLight,
							  int i_totalVPLNum, float i_maxLightScale)
{
	vkdContext& context = vkdGlobal::Context();

	// Setup the viewport to match the backbuffer
	vkdViewport vp;
	vp.m_Width = (float)l_LPVCellNum;
	vp.m_Height = (float)l_LPVCellNum;
	vp.m_MinDepth = 0.0f;
	vp.m_MaxDepth = 1.0f;
	vp.m_X = 0;
	vp.m_Y = 0;
	context.SetViewports( 1, &vp );

	context.SetRenderTargets( 3, l_RVRenderTargetView0, NULL );

	// Setup shader variables
	const matMaterial* pMaterial = &l_InjectionMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	fxEffect* pDXEffect = ((effShaderBaseVk*)pEffect)->GetFxEffect();
	fxEffectTechnique* pEffectTechnique = pDXEffect->GetTechniqueByName("Default");

	maAxisBox worldBox = i_rootNode->GetWorldBox();
	float volumeOrigin[4] = {worldBox.GetMinX(), worldBox.GetMinY(), worldBox.GetMinZ(), 1.0f};
	float invVolumeWidth[4] = {1.0f/worldBox.GetDiffX(), 1.0f/worldBox.GetDiffY(), 1.0f/worldBox.GetDiffZ(), 1.0f};
	int cells = l_LPVCellNum * l_LPVCellNum;
	float pointWeight = (float) cells / ((float) l_RSMSize * l_RSMSize);

	pDXEffect->GetVariableByName("g_VolumeOrigin")->AsVector()->SetFloatVector(volumeOrigin);
	pDXEffect->GetVariableByName("g_InvVolumeWidth")->AsVector()->SetFloatVector(invVolumeWidth);
	pDXEffect->GetVariableByName("g_LightPos")->AsVector()->SetFloatVector(i_pProjLight->GetPosition().Ptr());
	pDXEffect->GetVariableByName("g_PointWeight")->AsScalar()->SetFloat(pointWeight);
	pDXEffect->GetVariableByName("g_TexelNum")->AsScalar()->SetInt(l_LPVCellNum);
	pDXEffect->GetVariableByName("g_TexelSize")->AsScalar()->SetFloat(1.0f / (float)(l_LPVCellNum));
	maFloatRGBA lightColor = i_pProjLight->GetIntensity() * i_pProjLight->GetIntensityFactor();
	pDXEffect->GetVariableByName("g_LightColor")->AsVector()->SetFloatVector(lightColor.Ptr());

	// apply texture
	matReflectiveShadowMap* rsmMap = dynamic_cast<matReflectiveShadowMap*>(l_pReflectiveMap);
	pDXEffect->GetVariableByName("projRSMPosMap")->AsShaderResource()->SetResource(rsmMap->GetShaderView(0));
	pDXEffect->GetVariableByName("projRSMNormalMap")->AsShaderResource()->SetResource(rsmMap->GetShaderView(1));
	pDXEffect->GetVariableByName("projRSMFluxMap")->AsShaderResource()->SetResource(rsmMap->GetShaderView(2));

	pEffectTechnique->GetPassByIndex(0)->Apply(0);

	// draw vertice
	drawVolumeVertex(l_VPLVertexVB.get(), l_RSMSize * l_RSMSize);

	vkdImage* nullTex[3] = { NULL, NULL, NULL };
	context.SetShaderResources(0, 3, nullTex);
}

void shdwPassLPVGI::LightPropogation(int i_NumIteration, const g3dSceneNode* i_rootNode)
{
	vkdContext& context = vkdGlobal::Context();
	vkdImage** currentSRV = NULL;
	vkdImage** renderedSRV = NULL;
	vkdImage** currentRTV = NULL;

	const matMaterial* pMaterial = &l_InjectionMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	fxEffect* pDXEffect = ((effShaderBaseVk*)pEffect)->GetFxEffect();
	fxEffectTechnique* pEffectTechnique = pDXEffect->GetTechniqueByName("RVPropagation");
	fxEffectTechnique* pEffectTechniqueAccum = pDXEffect->GetTechniqueByName("AccumRV");

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

		vkdViewport vp;
		vp.m_Width = (float)l_LPVCellNum;
		vp.m_Height = (float)l_LPVCellNum;
		vp.m_MinDepth = 0.0f;
		vp.m_MaxDepth = 1.0f;
		vp.m_X = 0;
		vp.m_Y = 0;
		context.SetViewports( 1, &vp );

		context.SetRenderTargets( 3, currentRTV, NULL );

		// clear radiance volume firat
		float initColor[4] = {0,0,0,0};
		for (int i = 0; i < 3; i++)
		{
			context.ClearRenderTarget(currentRTV[i], initColor);
		}

		// Setup shader variables
		pDXEffect->GetVariableByName("RVVolumeMapR")->AsShaderResource()->SetResource(currentSRV[0]);
		pDXEffect->GetVariableByName("RVVolumeMapG")->AsShaderResource()->SetResource(currentSRV[1]);
		pDXEffect->GetVariableByName("RVVolumeMapB")->AsShaderResource()->SetResource(currentSRV[2]);
		pDXEffect->GetVariableByName("g_TexelSize")->AsScalar()->SetFloat(1.0f / (float)(l_LPVCellNum));
		pDXEffect->GetVariableByName("g_TexelNum")->AsScalar()->SetInt(l_LPVCellNum);
		pDXEffect->GetVariableByName("g_CellWidth")->AsVector()->SetFloatVector(CellWidth);
		pDXEffect->GetVariableByName("g_GIFalloff")->AsScalar()->SetFloat(m_giParam.m_LPVGIFalloff);

		pEffectTechnique->GetPassByIndex(0)->Apply(0);

		g3dBlendStateMgr::SetBlendState(st_NoBlend);
		drawVolumeVertex(l_RVVertexVB.get(), l_LPVCellNum * l_LPVCellNum);

		vkdImage* nullTex[3] = { NULL, NULL, NULL };
		context.SetShaderResources(0, 3, nullTex);

		// Accumlate RV result
		context.SetRenderTargets( 3, l_ARVRenderTargetView, NULL );
		pDXEffect->GetVariableByName("g_TexelNum")->AsScalar()->SetInt(l_LPVCellNum);
		pDXEffect->GetVariableByName("RVVolumeMapR")->AsShaderResource()->SetResource(renderedSRV[0]);
		pDXEffect->GetVariableByName("RVVolumeMapG")->AsShaderResource()->SetResource(renderedSRV[1]);
		pDXEffect->GetVariableByName("RVVolumeMapB")->AsShaderResource()->SetResource(renderedSRV[2]);
		pEffectTechniqueAccum->GetPassByIndex(0)->Apply(0);

		g3dBlendStateMgr::SetBlendState(st_Blend);
		drawVolumeVertex(l_RVVertexVB.get(), l_LPVCellNum * l_LPVCellNum);

		context.SetShaderResources(0, 3, nullTex);
	}
}

void shdwPassLPVGI::ApplyLPV(const g3dSceneNode* i_rootNode)
{
	if (m_depthTex != NULL)
	{
		m_depthTex->ClearDepthStencil(1.0f, g2dVkGlobal::HasStencil(), 0);
		m_depthTex->MakeCurrent();
	}

	m_scratchTex->Clear(maFloatRGBA(0,0,0,0));
	m_scratchTex->MakeCurrent();

	const matMaterial* pMaterial = &l_InjectionMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	fxEffect* pDXEffect = ((effShaderBaseVk*)pEffect)->GetFxEffect();

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

	vkdImage* nullTex[3] = { NULL, NULL, NULL };
	vkdGlobal::Context().SetShaderResources(0, 3, nullTex);
}

void shdwPassLPVGI::ApplyLPVOneNode(const g3dSceneNode* i_rootNode, const sNodePlusState& i_Node)
{
	if (m_depthTex != NULL)
	{
		m_depthTex->ClearDepthStencil(1.0f, g2dVkGlobal::HasStencil(), 0);
		m_depthTex->MakeCurrent();
	}

	m_scratchTex->Clear(maFloatRGBA(0,0,0,0));
	m_scratchTex->MakeCurrent();

	const matMaterial* pMaterial = &l_InjectionMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	fxEffect* pDXEffect = ((effShaderBaseVk*)pEffect)->GetFxEffect();

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

	vkdImage* nullTex[3] = { NULL, NULL, NULL };
	vkdGlobal::Context().SetShaderResources(0, 3, nullTex);
}

void shdwPassLPVGI::drawVolumeVertex(g3dVkBuffer* i_buffer, int i_numVertex)
{
	// draw vertice
	vkdContext& context = vkdGlobal::Context();
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceSize offset = 0;
	i_buffer->GetBinding(context, buffer, offset);

	context.SetVertexLayout( l_VPLVertexLayout.get() );
	context.SetVertexBuffer( 0, buffer, offset );
	context.SetTopology( VK_PRIMITIVE_TOPOLOGY_POINT_LIST );
	context.Draw( i_numVertex, 0 );
	G3D_PROFILE_DRAWS(1);
}

}
