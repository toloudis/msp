/****************************************************************************\
**	shdwTestShaderRendererD3D.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "shdwTestShaderRendererD3D.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "GraphicsDX9/eff/effShaderBaseDX9.hpp"
#include "GraphicsDX9/g2d/g2dDX9GlobalWin.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "GraphicsDX9/g3d/g3dDX9TextureUtil.hpp"
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
#include "Core/gf/gfPaths.hpp"
#include "Graphics/mat/matMaterial.hpp"
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

	matRenderTargetTexture* g_pTex = NULL;
	matRenderTargetTexture* g_pTex2 = NULL;

	matTexture* g_DiffuseMap = NULL;
	matTexture* g_SpecularMap = NULL;
	matTexture* g_GlossMap = NULL;
	matTexture* g_ReflectionMap = NULL;


}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwTestShaderRendererD3D::shdwTestShaderRendererD3D()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;

	if (g_pTex != NULL)
		throw("Bad constructor call in shdwTestShaderRendererD3D");
	if (g_pTex2 != NULL)
		throw("Bad constructor call in shdwTestShaderRendererD3D");

	g_pTex = NULL;
	g_pTex2 = NULL;

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwTestShaderRendererD3D::~shdwTestShaderRendererD3D()
{
	ReleaseResources();
}

void shdwTestShaderRendererD3D::ReleaseResources()
{
	// force updates on next render call 
	m_pWindow = NULL;

	delete g_pTex;
	g_pTex = NULL;
	delete g_pTex2;
	g_pTex2 = NULL;

	if (g_DiffuseMap) { matTextureMgr::ReleaseTexture(g_DiffuseMap); g_DiffuseMap = NULL; } 
	if (g_SpecularMap) { matTextureMgr::ReleaseTexture(g_SpecularMap); g_SpecularMap = NULL; } 
	if (g_GlossMap) { matTextureMgr::ReleaseTexture(g_GlossMap); g_GlossMap = NULL; } 
	if (g_ReflectionMap) { matTextureMgr::ReleaseTexture(g_ReflectionMap); g_ReflectionMap = NULL; } 
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwTestShaderRendererD3D::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
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
    
	m_pWindow->MakeCurrent();

	// don't z-text: draw the quads every time.
	g2dDX9Global::g_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	g2dDX9Global::g_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	ID3DXEffect* pEffect;
	for (int i = 0; i < 100; i++)
	{
		pEffect = ShaderSetup(i);

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

void shdwTestShaderRendererD3D::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);

	// test conditions for recreating surfaces:
	if ((w != m_Width) || 
		(h != m_Height) || 
		(g_pTex == NULL) || 
		(g_DiffuseMap == NULL))
	{
		InitMaterials();
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
HRESULT shdwTestShaderRendererD3D::ClearTexture( LPDIRECT3DTEXTURE9 pTexture )
{
    HRESULT hr = S_OK;
    PDIRECT3DSURFACE9 pSurface = NULL;

    hr = pTexture->GetSurfaceLevel( 0, &pSurface );
    if( SUCCEEDED(hr) )
        g2dDX9Global::g_pDevice->ColorFill( pSurface, NULL, D3DCOLOR_ARGB(0, 0, 0, 0) );

    SAFE_RELEASE( pSurface );
    return hr;
}

void shdwTestShaderRendererD3D::HandleChar(itString::CharType ch)
{
	switch( ch )
	{
		case itString::CharType('a'):
			break;
	}
}

void shdwTestShaderRendererD3D::InitMaterials()
{
	if (g_DiffuseMap) matTextureMgr::ReleaseTexture(g_DiffuseMap);
	if (g_SpecularMap) matTextureMgr::ReleaseTexture(g_SpecularMap);
	if (g_GlossMap) matTextureMgr::ReleaseTexture(g_GlossMap);
	if (g_ReflectionMap) matTextureMgr::ReleaseTexture(g_ReflectionMap);

	// load up textures
	fsLocator texLocator = gfPaths::GetPath(gfPaths::e_ExePath);
	texLocator.Push("data");
	g_DiffuseMap = matTextureMgr::LoadTexture(texLocator, itString("Glow_test1_color.dds"));
	g_SpecularMap = matTextureMgr::LoadTexture(texLocator, itString("colors1.png"));
	g_GlossMap = matTextureMgr::LoadTexture(texLocator, itString("green003.png"));
	g_ReflectionMap = matTextureMgr::LoadTexture(texLocator, itString("Sky128.dds"));
}
ID3DXEffect* shdwTestShaderRendererD3D::ShaderSetup(int i)
{
	BOOL hasDiffuseMap = FALSE;//((i/8)%2 == 0) ? TRUE : FALSE;
	BOOL hasSpecularMap = FALSE;//((i/4)%2 == 0) ? TRUE : FALSE;
	BOOL hasGlossMap = FALSE;//((i/2)%2 == 0) ? TRUE : FALSE;
	BOOL hasReflectionMap = FALSE;//((i/1)%2 == 0) ? TRUE : FALSE;

	std::string sname = "Phong";
//	sname += hasDiffuseMap?"1":"0";
//	sname += hasSpecularMap?"1":"0";
//	sname += hasGlossMap?"1":"0";
//	sname += hasReflectionMap?"1":"0";
	sname += ".fx";

//	sname = "simpletest.fx";

	effShaderBaseDX9* pEffBase = (effShaderBaseDX9*)g3dDX9Util::GetEffect(sname);
	ID3DXEffect* pEffect = pEffBase->GetD3DXEffect();

	pEffect->SetTechnique("Default");
    
	pEffect->SetBool("hasDiffuseMap", hasDiffuseMap);
	pEffect->SetBool("hasSpecularMap", hasSpecularMap);
	pEffect->SetBool("hasGlossMap", hasGlossMap);
	pEffect->SetBool("hasReflectionMap", hasReflectionMap);

	// simpletest.fx:
//	pEffect->SetBool("g_bHasDiffuseMap", hasDiffuseMap);
//	pEffect->SetBool("g_bHasSpecularMap", hasSpecularMap);

	pEffect->SetTexture("diffuseMap", hasDiffuseMap?g3dDX9TextureUtil::GetD3DTexture(g_DiffuseMap):NULL);
	pEffect->SetTexture("specularMap", hasSpecularMap?g3dDX9TextureUtil::GetD3DTexture(g_SpecularMap):NULL);
	pEffect->SetTexture("glossMap", hasGlossMap?g3dDX9TextureUtil::GetD3DTexture(g_GlossMap):NULL);
	pEffect->SetTexture("reflectionMap", hasReflectionMap?g3dDX9TextureUtil::GetD3DTexture(g_ReflectionMap):NULL);


//setup light
	struct LightInfo
	{
		maPoint4d m_Position;
		maFloatRGBA m_Diffuse;
		maFloatRGBA m_Specular;
		maVector3d m_Attenuation;

		LightInfo() : m_Position(0,0,0,1), 
			m_Diffuse(1,1,1,1), m_Specular(1,1,1,0), m_Attenuation(1,0,0)
		{
		}
	};
	LightInfo light_info;
	pEffect->SetValue("g_lightInfo", &light_info, sizeof(light_info));

	return pEffect;
}