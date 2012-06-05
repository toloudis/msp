/****************************************************************************\
**	shdwPassVelocity.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwPassVelocity.hpp"

#include "Core/ma/maPlane.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effMaskAlphaData.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

namespace
{
	matMaterial l_DrawVelocityMat;

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;

	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
}; // namespace

void shdwPassVelocity::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL);

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
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
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwVelocityRendererDX11::Render" );
	m_stats.Reset();
	
	// Assign shader
	effShaderBaseDX11* pEffBase = (effShaderBaseDX11*)g3dDX11Util::GetEffect("VelocityRender.fx");
	ID3DX11Effect* pEffect = pEffBase->GetD3DXEffect();

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

		g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

		// Motion blur are disabled, these new blend states have never been tested.
		g3dBlendStateMgr::SetBlendState(st_NoBlend);

		// Force our material for all renderers
		g3dDX11Util::SetOverrideMaterial(&l_DrawVelocityMat);

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

		g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();	
		g3dDX11Util::SetOverrideMaterial(NULL);

	}

	m_pVelocityStateManager->InspectCams();

	D3DPERF_EndEvent();

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
		const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;
		matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

		// Set draw style
		g3dDrawStyleUtilDX11::SetDrawStyle(i_pNode->GetDrawStyle());

		// set shader globals
		g3dDX11Util::SetupShaderGlobals(pEffect);

		// geometry data
		g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

		bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
		g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
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
void shdwPassVelocity::SetupOldMatrix( const g3dSceneNode* pNode, ID3DX11Effect* pEffect ) 
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
void shdwPassVelocity::SelectStoringTechnique( const g3dSceneNode* pNode, effShaderBaseDX11* pEffect ) 
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