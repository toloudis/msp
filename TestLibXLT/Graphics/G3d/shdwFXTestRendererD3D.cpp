/****************************************************************************\
**	shdwFXTestRendererD3D.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "shdwFXTestRendererD3D.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "GraphicsDX9/eff/effShaderBaseDX9.hpp"
#include "GraphicsDX9/g2d/g2dDX9GlobalWin.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "GraphicsDX9/g3d/g3dDX9Util.hpp"
#include "GraphicsDX9/g3d/g3dDrawStyleUtilDX9.hpp"
#include "GraphicsDX9/g3d/g3dFogDX9.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "GraphicsDX9/g3d/g3dLightMgrDX9.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "GraphicsDX9/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX9/g3d/g3dSceneRenderUtil.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "GraphicsDX9/mat/matRenderTargetTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX9/shdw/shdwPassGlow.hpp"
#include "GraphicsDX9/shdw/shdwPassTransparent.hpp"

#include "Core/dbg/dbgLog.hpp"
#include <strsafe.h>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

namespace
{
	// Stats
	int l_nNumTriangles = 0;

	matRenderTargetTexture* g_pTex;
	matRenderTargetTexture* g_pTex2;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwFXTestRendererD3D::shdwFXTestRendererD3D()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;

	if (g_pTex != NULL)
		throw("Bad constructor call in shdwFXTestRendererD3D");
	if (g_pTex2 != NULL)
		throw("Bad constructor call in shdwFXTestRendererD3D");

	g_pTex = NULL;
	g_pTex2 = NULL;

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwFXTestRendererD3D::~shdwFXTestRendererD3D()
{
	ReleaseResources();
}

void shdwFXTestRendererD3D::ReleaseResources()
{
	// force updates on next render call 
	m_pWindow = NULL;

	delete g_pTex;
	g_pTex = NULL;
	delete g_pTex2;
	g_pTex2 = NULL;
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwFXTestRendererD3D::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene,
							   float i_fSimTime )
{//PROFILE("Render");
	m_pWindow = i_pWindow;

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// on entry, i expect a Clear()'ed and BeginScene()'d render target.
	// after i leave, i expect EndScene() and Present() to occur.
	
	// make sure temp surfaces are up-to-date
	CreateSurfaces(i_pWindow);

	// Initialize the triangle count
	l_nNumTriangles = 0;

	/////////////////////////////////////////////////////////////
    UINT uiPassCount, uiPass;
    
	effShaderBaseDX9* pEffBase = (effShaderBaseDX9*)g3dDX9Util::GetEffect("simpletest.fx");
	ID3DXEffect* pEffect = pEffBase->GetD3DXEffect();

	pEffect->SetTechnique("Default");
    
	m_pWindow->MakeCurrent();

	// shuffle the permutations
	static const BOOL bD[4] = {TRUE, FALSE, TRUE, FALSE};
	static const BOOL bS[4] = {TRUE, TRUE, FALSE, FALSE};

	// don't z-text: draw the quads every time.
	g2dDX9Global::g_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	for (int i = 0; i < 200; i++)
	{

		pEffect->SetFloat("diffuseFactor", 0.25);
		pEffect->SetFloat("specularFactor", 0.25);


		pEffect->SetBool("g_bHasDiffuseMap", bD[i%4]);
		pEffect->SetTexture("diffuseMap", (bD[i%4])?g_pTex->GetTextureSurface():NULL);
		pEffect->SetBool("g_bHasSpecularMap", bS[i%4]);
		pEffect->SetTexture("specularMap", (bS[i%4])?g_pTex2->GetTextureSurface():NULL);

//		pEffect->SetTexture("diffuseMap", g_pTex->GetTextureD3D());
//		pEffect->SetTexture("specularMap", g_pTex2->GetTextureD3D());

		pEffect->Begin(&uiPassCount, 0);
		for (uiPass = 0; uiPass < uiPassCount; uiPass++)
		{
			pEffect->BeginPass(uiPass);

				g3dDX9Util::DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );
				l_nNumTriangles += 2;

			pEffect->EndPass();
		}
		pEffect->End();

	}
    g2dDX9Global::g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g2dDX9Global::g_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);


	// Display debug information for number of triangles
//	char num[64];
//	sprintf(num, "tris: %d", l_nNumTriangles);
//	i_pWindow->SetDebugInfo(2, num);

	return l_nNumTriangles;
}

void shdwFXTestRendererD3D::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);

	// test conditions for recreating surfaces:
	if ((w != m_Width) || 
		(h != m_Height) || 
		(g_pTex == NULL) )
	{
		// Crop the scene texture so width and height are evenly divisible by 8.
		// This cropped version of the scene will be used for post processing effects,
		// and keeping everything evenly divisible allows precise control over
		// sampling points within the shaders.
		m_Width = w;
		m_Height = h;
		m_dwCropWidth = w - w % 8;
		m_dwCropHeight = h - h % 8;

		// special knowledge that this render target is a window...
		g2dWindow* pWnd = (g2dWindow*)dynamic_cast<g2dWindow*>(i_pWindow);
		DBG_ASSERT0(pWnd != NULL, "Window passed to renderer render target is not a g2dWindow!");

		delete g_pTex;
		g_pTex = new matRenderTargetTexture(false);
		g_pTex->Make(m_Width, m_Height, pWnd->GetBackBufferPixelFormat(), false);

		delete g_pTex2;
		g_pTex2 = new matRenderTargetTexture(false);
		g_pTex2->Make(m_Width, m_Height, pWnd->GetBackBufferPixelFormat(), false);

		g_pTex->Clear(maFloatRGBA(1,0,0,1));
		g_pTex2->Clear(maFloatRGBA(0,0,1,1));

	}
}

//-----------------------------------------------------------------------------
// Name: ClearTexture()
// Desc: Helper function for RestoreDeviceObjects to clear a texture surface
//-----------------------------------------------------------------------------
HRESULT shdwFXTestRendererD3D::ClearTexture( LPDIRECT3DTEXTURE9 pTexture )
{
    HRESULT hr = S_OK;
    PDIRECT3DSURFACE9 pSurface = NULL;

    hr = pTexture->GetSurfaceLevel( 0, &pSurface );
    if( SUCCEEDED(hr) )
        g2dDX9Global::g_pDevice->ColorFill( pSurface, NULL, D3DCOLOR_ARGB(0, 0, 0, 0) );

    SAFE_RELEASE( pSurface );
    return hr;
}

void shdwFXTestRendererD3D::HandleChar(itString::CharType ch)
{
	switch( ch )
	{
		case itString::CharType('a'):
			break;
	}
}

