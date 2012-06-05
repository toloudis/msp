/****************************************************************************\
**	shdwRayTraceRenderer.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwRayTraceRenderer.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effBlurData.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/sc/scBillboard.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dWindowPrimaryDX11.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matPlainTexture.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"

#include "GraphicsDX11/shdw/private/rtUtil.hpp"

//#include "profile.h"

//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D_ERROR(op_result, error_string) \
			if( op_result != D3D_OK )	\
			{	\
				g2dDX11Global::PrintDXError(op_result);	\
				DBG_ASSERT0(false, error_string);	\
			}	\

namespace
{

}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwRayTraceRenderer::shdwRayTraceRenderer()
{
	RTInit();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwRayTraceRenderer::~shdwRayTraceRenderer()
{
	ReleaseResources();
	RTCleanUp();
}

void shdwRayTraceRenderer::ReleaseResources()
{
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy plus additional 
//	viewer specific layers.
//	Returns the number of triangles rendered
//--------------------------------------------------------------------
int shdwRayTraceRenderer::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
			 const g3dScene &i_Scene,
			 const std::vector<g3dLayer*>& i_ViewerLayers,
			 float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwRayTraceRenderer::Render" );

//	g2dDX11Global::g_pDevice->ClearDepthStencilView( , D3D11_CLEAR_DEPTH, 1, 0);
	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;
	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	RT(i_pWindow, &i_Camera, &i_Scene);

	D3DPERF_EndEvent();
	return 0;
}





