/****************************************************************************\
**	shdwSinglePassRendererDX11.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwSinglePassRendererDX11.hpp"

//#include "GraphicsDX11/shdw/shdwPassAmbient.hpp"
//#include "GraphicsDX11/shdw/shdwPassDOF.hpp"
//#include "GraphicsDX11/shdw/shdwPassEnvironment.hpp"
//#include "GraphicsDX11/shdw/shdwPassGlow.hpp"
//#include "GraphicsDX11/shdw/shdwPassLit.hpp"
//#include "GraphicsDX11/shdw/shdwPassTransparent.hpp"
//#include "GraphicsDX11/shdw/shdwPassOutline.hpp"
//#include "GraphicsDX11/shdw/shdwPassZFill.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effBlurData.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/sc/scBillboard.hpp"
#include "GraphicsDX11/eff/effShaderArray.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matPlainTexture.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"

//#include "Core/dbg/dbgLog.hpp"
//#include "profile.h"

//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D_ERROR(op_result, error_string) \
			if( op_result != D3D_OK )	\
			{	\
				g2dDX10Global::PrintDXError(op_result);	\
				DBG_ASSERT0(false, error_string);	\
			}	\

namespace
{

int DrawNode(g3dSceneNode* i_pNode)
{
	g3dDrawStyleUtilDX11::SetDrawStyle(i_pNode->GetDrawStyle());
	g3dRasterizerStateMgr::SetRasterizerState( 
		i_pNode->GetFragment()->GetDoubleSided() ? D3D11_CULL_NONE : D3D11_CULL_FRONT, 
		g3dDrawStyleUtilDX11::GetD3DDrawStyle());

	// resolve material/effect
	const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

	// use default technique
	pEffect->SetTechnique(matShaderEffect::e_Default);

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

	// shading data
	pEffect->SetupMaterial(pMaterial);

	pEffect->SetupAmbientLighting(i_pNode->GetWorldBox());

	// I think we actually always want to set up the ambient pass,
	// even if that just means we set it up with somethinng with
	// NULL pointers and booleans set to false
	static const g3dAmbientEnvState l_NoAmbientEnvState;
	pEffect->SetupAmbientPass(l_NoAmbientEnvState);

	int nTriangles = g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
	
	return nTriangles;
}

int RecurseDrawNodes(g3dSceneNode* i_Node)
{
	if (!i_Node->GetRenderable())
		return 0;

	int npoly = 0;

	g3dFragment* frag = i_Node->GetFragment();
	if (frag)
	{
		npoly += DrawNode( i_Node );
	}

	for (int i = 0; i < i_Node->GetNumChildren(); i++)
	{
		npoly += RecurseDrawNodes(i_Node->GetChild(i));
	}
	return npoly;
}
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwSinglePassRendererDX11::shdwSinglePassRendererDX11()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwSinglePassRendererDX11::~shdwSinglePassRendererDX11()
{
	ReleaseResources();
}

void shdwSinglePassRendererDX11::ReleaseResources()
{
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy plus additional 
//	viewer specific layers.
//	Returns the number of triangles rendered
//--------------------------------------------------------------------
int shdwSinglePassRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
			 const g3dScene &i_Scene,
			 const std::vector<g3dLayer*>& i_ViewerLayers,
			 float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwSinglePassRendererDX11::Render" );

//	g2dDX10Global::g_pDevice->ClearDepthStencilView( , D3D10_CLEAR_DEPTH, 1, 0);
	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	int npoly = 0;
	int n = i_Scene.GetNumLayers();
	for (int i = 0; i < n; i++)
	{
		// Set the camera and projection transform
		g3dSceneRenderUtil::SetViewingTransforms(i_Camera, i_Scene.GetLayer(i)->GetModelSpace());

		npoly += RecurseDrawNodes(i_Scene.GetLayer(i)->GetRootNode());

		g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();
	}


	D3DPERF_EndEvent();
	return npoly;
}




