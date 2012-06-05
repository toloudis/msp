/****************************************************************************\
**	g3dTextureRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/G3d/g3dTextureRendererDX11.hpp"

#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
//#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDrawStyleUtilDX11.hpp"

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
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dTextureRendererDX11::g3dTextureRendererDX11()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dTextureRendererDX11::~g3dTextureRendererDX11()
{
	ReleaseResources();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void g3dTextureRendererDX11::ReleaseResources()
{
	// force updates on next render call 
	m_pWindow = NULL;
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int g3dTextureRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dTextureRendererDX11::Render" );

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;

	m_pWindow = i_pWindow;

	int w, h;
	i_pWindow->GetDimensions(w, h);
	if (m_Width != w || m_Height != h)
	{
		m_Width = w;
		m_Height = h;
	}

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	i_pWindow->MakeCurrent();
	
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);
	
	// gather scene graph elements into sorted lists
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();
	g3dDX11Util::SetOverrideMaterial(NULL);

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_FRONT, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	for (int i = 0; i < layers.size(); i++)
	{
		g3dRenderLayer rLayer;
		rLayer.Set(layers[i], &i_Camera, i_pWindow);
		rLayer.PerFrameInit(i_fSimTime);
		l_nNumTrianglesRendered += rLayer.Render(i_fSimTime);
	}

	{
		//matRenderTargetTexture* tempTarget = (matRenderTargetTexture*)(i_pWindow);
		//HRESULT hr;

		//g2dD3D11SurfacePtr srcSurface;
		//g2dD3D11TexturePtr srcTex;
		//hr = tempTarget->GetTextureSurface()->QueryInterface(IID_IDirect3DTexture9, (void**)&srcTex);
		//DBG_ASSERT(srcTex, "Bad texture type passed in to CreateRenderTargetTexture");
		//hr = srcTex->GetSurfaceLevel( 0, &srcSurface );
		//srcTex->Release();

		//// the image destructor will release the surface
		//g2dImageDX11 img(srcSurface);
		//g2dImageSave::Save(fsLocator(itString("test.png")), &img);
	}
	
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
	//dx11 commented out
//	g2dDX11Global::g_pDevice->SetStreamSource(0, NULL, 0, 0);
//	g2dDX11Global::g_pDevice->SetPixelShader( NULL );

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
