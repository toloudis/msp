/****************************************************************************\
**	shdwPassVelocity.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSVELOCITY_HPP
#include "GraphicsVulkan/shdw/shdwPassVelocity.hpp"
#endif

#ifndef MA_PLANE_HPP
#include "Core/ma/maPlane.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef EFF_MASKALPHADATA_HPP
#include "Graphics/eff/effMaskAlphaData.hpp"
#endif
#ifndef G2D_WINDOW_HPP
#include "Graphics/g2d/g2dWindow.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

namespace
{
	matMaterial l_DrawVelocityMat;

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;

	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
}; // namespace

void shdwPassVelocity::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwPassVelocity::CleanupStates()
{
	delete st_NoBlend;

	delete ds_Test_Write_LessE_NS;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassVelocity::shdwPassVelocity()
	: m_sceneInfo(NULL),
	  m_pCamera(NULL),
	  m_Scene(NULL),
	  m_pVelocityStateManager(NULL)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassVelocity::shdwPassVelocity(g2dRenderTarget* i_pVelocityTarget,
								   VelocityStateManager* i_pVelocityStateManager,
								   const camCamera* i_pCamera,
								   const g3dScene* i_Scene)
	: m_sceneInfo(NULL),
	  m_pCamera(i_pCamera),
	  m_Scene(i_Scene),
	  m_pVelocityStateManager(i_pVelocityStateManager)
{
	m_pRenderTarget = i_pVelocityTarget;
	m_lastTime = 0;

	// lazy init so that this material can be reused across instantiations.
	if (!l_DrawVelocityMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/VelocityRender.fx"), matShaderMgr::GetSpecialEffect("VelocityRender.fx"));
		l_DrawVelocityMat.SetShaderParams(p);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassVelocity::~shdwPassVelocity()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassVelocity::SetOldTime(float time)
{
	m_lastTime = time;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int shdwPassVelocity::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassVelocity");
	m_stats.Reset();

	// Assign shader
	effShaderBaseVk* pEffBase = (effShaderBaseVk*)g3dVkUtil::GetEffect("VelocityRender.fx");
	fxEffect* pEffect = pEffBase->GetFxEffect();

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_time;

	// Clear surfaces
	m_pRenderTarget->Clear(maFloatRGBA(0,0,0,1));

	// Write to velocity surface
	m_pRenderTarget->MakeCurrent();

	m_pRenderTarget->ClearDepthStencil();

	// Previous and current viewproj matrix management
	if ( m_lastTime != i_time )
	{
		m_pVelocityStateManager->SetOldCamera(  *(m_pVelocityStateManager->GetCurrCamera())  );
	}

	// ASSUMING that consecutive frames should use the same viewport:
	float top, bottom, left, right;
	m_pCamera->GetSubViewport(top,bottom,left,right);
	m_pVelocityStateManager->GetOldCamera()->SetSubViewport( top, bottom, left, right );

	g3dSceneRenderUtil::SetViewingTransforms( *m_pCamera );

	m_pVelocityStateManager->SetCurrCamera(  *m_pCamera  );

	// Only render if not the first frame
	//if ( g3dPrefs::CurrentPrefs().m_nDoRenderCaptureIteration != 0 )
	{

		// gather scene graph elements into sorted lists
		const std::vector<g3dLayer*> &layers = m_Scene->GetLayers();
		g3dSceneRenderUtil::enable_lights();
		m_sceneInfo->Clear();
		m_sceneInfo->TraverseLayer(i_time, layers[0], m_pCamera);

		// Z Buffering, lights, and blend for ambient pass

		g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

		g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

		// Motion blur are disabled, these new blend states have never been tested.
		g3dBlendStateMgr::SetBlendState(st_NoBlend);

		// Force our material for all renderers
		g3dVkUtil::SetOverrideMaterial(&l_DrawVelocityMat);

		// Render the layers
		const g3dSceneNode* pNode = NULL;
		int i,n;
		const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
		n = nonShadowNodes.size();
		for (i = 0; i < n; i++)
		{
			pNode = nonShadowNodes[i].m_pNode;
			SetupOldMatrix(pNode,pEffect);
			SelectStoringTechnique(pNode,pEffBase);
			m_stats.m_nTriangles += RenderNode(pNode);
		}
		const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
		n = shadowNodes.size();
		for (i = 0; i < n; i++)
		{
			pNode = shadowNodes[i].m_pNode;
			SetupOldMatrix(pNode,pEffect);
			SelectStoringTechnique(pNode,pEffBase);
			m_stats.m_nTriangles += RenderNode(pNode);
		}
		const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
		n = nonSolidNodes.size();
		for (i = 0; i < n; i++)
		{
			pNode = nonSolidNodes[i].m_pNode;
			SetupOldMatrix(pNode,pEffect);
			SelectStoringTechnique(pNode,pEffBase);
			m_stats.m_nTriangles += RenderNode(pNode);
		}

		g3dDrawStyleUtilVk::RestoreNormalDrawStyle();
		g3dVkUtil::SetOverrideMaterial(NULL);

	}

	m_pVelocityStateManager->InspectCams();


	return m_stats.m_nTriangles;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int shdwPassVelocity::RenderNode(const g3dSceneNode* i_pNode)
{
	int nTriangles = 0;
	if ( g3dPrefs::CurrentPrefs().m_nDoRenderCaptureIteration != 0 )
	{
		// resolve material/effect
		const matMaterial* pMaterial = g3dVkUtil::GetMaterial(i_pNode);//m_pAlphaMaskMat;
		matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

		// Set draw style
		g3dDrawStyleUtilVk::SetDrawStyle(i_pNode->GetDrawStyle());

		// set shader globals
		g3dVkUtil::SetupShaderGlobals(pEffect);

		// geometry data
		g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);

		bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
		g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
		nTriangles = g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
	}

	return nTriangles;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
std::string get_key_str( g3dSceneNode* pNode )
{
	std::string key = "";

	if ( pNode )
	{
		key = pNode->GetName();
		if ( pNode->GetFragment() )
		{
			key.append("_" + pNode->GetFragment()->GetFragmentName());
			if ( pNode->GetFragment()->GetMaterial() )
			{
				key.append("_" + pNode->GetFragment()->GetMaterial()->GetName() );
			}
		}
	}
	return key;
}

//------------------------------------------------------------------------
//	Check if an object has been processed
//------------------------------------------------------------------------
int shdwPassVelocity::InsertObject(std::string i_Name)
{
	for(std::map<std::string, int>::const_iterator it = m_ObjectNames.begin(); it != m_ObjectNames.end(); ++it)
	{
		if ( it->first == i_Name )
		{
			int val = it->second + 1;
			m_ObjectNames[i_Name] = val;
			return val;
		}
	}
	m_ObjectNames.insert(std::pair<std::string,int>(i_Name,1));
	return 1;
}

//------------------------------------------------------------------------
//	Clear processed objects
//------------------------------------------------------------------------
void shdwPassVelocity::ClearObjects()
{
	m_ObjectNames.clear();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassVelocity::SetupOldMatrix( const g3dSceneNode* pNode, fxEffect* pEffect )
{
	std::string key = get_key_str((g3dSceneNode*)pNode);

	int obj_id = InsertObject( key );
	char buffer[33];
	_itoa(obj_id,buffer,10);

	key = key + "_" + buffer;

	maMatrix4x4 g_world_old = m_pVelocityStateManager->GetPrevWorldTransform(key);
	g_world_old.Transpose();
	pEffect->GetVariableByName("g_world_old")->AsMatrix()->SetMatrix(g_world_old.Ptr());

	maMatrix4x4 g_vp_old = m_pVelocityStateManager->GetViewProjectionTransform_Old();
	g_vp_old.Transpose();
	pEffect->GetVariableByName("g_vp_old")->AsMatrix()->SetMatrix(g_vp_old.Ptr());

	maMatrix4x4 g_wvp_old = m_pVelocityStateManager->GetPrevWorldTransform(key) * m_pVelocityStateManager->GetViewProjectionTransform_Old();
	g_wvp_old.Transpose();
	pEffect->GetVariableByName("g_wvp_old")->AsMatrix()->SetMatrix(g_wvp_old.Ptr());

	m_pVelocityStateManager->InsertCurrWorldTransform(key,pNode->GetTotalTransform());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassVelocity::SelectStoringTechnique( const g3dSceneNode* pNode, effShaderBaseVk* pEffect )
{
	if ( pNode->GetFragment()->GetHasVelocityBuffer() )
	{
		pEffect->SetTechnique("StoreMorphableObjectVelocity");
	}
	else
	{
		pEffect->SetTechnique("StoreNonMorphableObjectVelocity");
	}
}

}
