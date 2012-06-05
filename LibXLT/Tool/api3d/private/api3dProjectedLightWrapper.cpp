/****************************************************************************\
**	api3dProjectedLightWrapper.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dProjectedLightWrapper.hpp"

#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dTargetRenderer.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dTargetRendererMgr.hpp"


//--------------------------------------------------------------------
// The wrapper manages this light, but does not own the light itself.
//	This constructor will create a depth map for it in the given
//	resolution and it will create the renderers needed to keep
//	the shadows up to date.
//	If the given scene is NULL, then api3dScene is used.
//--------------------------------------------------------------------
api3dProjectedLightWrapper::api3dProjectedLightWrapper(g3dProjectedLight& i_Light, 
							int i_DepthMapResolution,
							g3dScene *i_pScene)
: m_Light(i_Light),
  m_pTexture(NULL),
  m_pShadowMap(NULL),
  m_pMapRenderer(NULL),
  m_pScene(i_pScene ? i_pScene : api3dScene::GetScene())
{
	// simple renderer for shadow maps 
	m_pDepthRenderer = g3dSceneRendererCreate::CreateDepthMapRenderer();
	
	update_shadow_map(i_DepthMapResolution);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
api3dProjectedLightWrapper::~api3dProjectedLightWrapper()
{
	if (m_pTexture)
		matTextureMgr::ReleaseTexture(m_pTexture);
	if (m_pShadowMap)
		matTextureMgr::ReleaseTexture(m_pShadowMap);
	if (m_pMapRenderer)
	{
		api3dTargetRendererMgr::RemoveTargetRenderer(m_pMapRenderer);
		delete m_pMapRenderer;
	}
	delete m_pDepthRenderer;
}

//--------------------------------------------------------------------
// Pass ownership of the projected texture to this wrapper. This
//	isn't necessary for the projected light, you could manage the
//	texture and assign it to the g3dProjectedLight yourself, but 
//	this is just for convenience.
//--------------------------------------------------------------------
void api3dProjectedLightWrapper::SetTexture(matTexture* i_pTexture)
{
	if (m_pTexture)
		matTextureMgr::ReleaseTexture(m_pTexture);

	// Assign texture to projected light
	m_pTexture = i_pTexture;
	m_Light.SetTexture(m_pTexture);
}

//--------------------------------------------------------------------
// Reallocate depth map to given size, which probably 
//	should be a power of 2.
//--------------------------------------------------------------------
void api3dProjectedLightWrapper::ResizeDepthMap(int i_Size)
{
	update_shadow_map(i_Size);
}


//--------------------------------------------------------------------
// Call this in the think/render process to make sure that the 
//	camera used for rendering depth maps matches the current light's
//	properties.
//--------------------------------------------------------------------
void api3dProjectedLightWrapper::OrientCamera()
{
	m_Light.OrientCamera(m_ShadowCamera);
}

//--------------------------------------------------------------------
// The depth map renderer is added to the api3dTargetRenderMgr,
// so this call is not necessary unless you want to force an
// update to the depth map on your own.
//--------------------------------------------------------------------
void api3dProjectedLightWrapper::RenderDepthMap(float i_Time)
{
	m_pMapRenderer->Render(i_Time);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void api3dProjectedLightWrapper::update_shadow_map(int i_Size)
{
	if (m_pShadowMap)
		matTextureMgr::ReleaseTexture(m_pShadowMap);

	// make the render target texture
	m_pShadowMap = matTextureMgr::CreateShadowMap( i_Size, i_Size );

	// Assign textures to projected light
	m_Light.SetShadowMap(m_pShadowMap);

	// Set up Target renderer for updating the depth map
	if (m_pMapRenderer)
	{
		api3dTargetRendererMgr::RemoveTargetRenderer(m_pMapRenderer);
		delete m_pMapRenderer;
	}
	
	g2dRenderTarget* pTarget = m_pShadowMap->GetRenderTargetAPI();
	m_pMapRenderer = new g3dTargetRenderer(pTarget, m_pDepthRenderer, m_pScene, &m_ShadowCamera );
	m_pMapRenderer->SetBackgroundColor(g2dRGBColor(0xff, 0xff, 0xff));
	api3dTargetRendererMgr::AddTargetRenderer(m_pMapRenderer);
}
