/****************************************************************************\
**	api3dProjectedLightWrapper.hpp
**
**		An object to keep track of data needed for a projected light,
**	especially for rendering the depth map for the shadows.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_PROJECTEDLIGHTWRAPPER_HPP
#error api3dProjectedLightWrapper.hpp multiply included
#endif
#define API3D_PROJECTEDLIGHTWRAPPER_HPP

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class g3dProjectedLight;
class g3dTargetRenderer;
class g3dScene;
class g3dSceneRenderer;
class matTexture;


//============================================================================
//============================================================================
class api3dProjectedLightWrapper 
{
	public:
		//--------------------------------------------------------------------
		// The wrapper manages this light, but does not own the light itself.
		//	This constructor will create a depth map for it in the given
		//	resolution and it will create the renderers needed to keep
		//	the shadows up to date.
		//	If the given scene is NULL, then api3dScene is used.
		//--------------------------------------------------------------------
		api3dProjectedLightWrapper(g3dProjectedLight& i_Light, 
									int i_DepthMapResolution = 512,
									g3dScene *i_pScene = NULL);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~api3dProjectedLightWrapper();

		//--------------------------------------------------------------------
		// Pass ownership of the projected texture to this wrapper. This
		//	isn't necessary for the projected light, you could manage the
		//	texture and assign it to the g3dProjectedLight yourself, but 
		//	this is just for convenience.
		//--------------------------------------------------------------------
		void SetTexture(matTexture* i_pTexture);

		//--------------------------------------------------------------------
		// Reallocate depth map to given size, which probably 
		//	should be a power of 2.
		//--------------------------------------------------------------------
		void ResizeDepthMap(int i_Size);

		//--------------------------------------------------------------------
		// Call this in the think/render process to make sure that the 
		//	camera used for rendering depth maps matches the current light's
		//	properties.
		//--------------------------------------------------------------------
		void OrientCamera();

		//--------------------------------------------------------------------
		// The depth map renderer is added to the api3dTargetRenderMgr,
		// so this call is not necessary unless you want to force an
		// update to the depth map on your own.
		//--------------------------------------------------------------------
		void RenderDepthMap(float i_Time);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_shadow_map(int i_Size);

		g3dProjectedLight& m_Light;

		// Textures for projection
		matTexture* m_pTexture;
		matTexture* m_pShadowMap;

		// for rendering depth map for the projected light
		g3dTargetRenderer* m_pMapRenderer;
		g3dSceneRenderer* m_pDepthRenderer;
		camCamera m_ShadowCamera;
		g3dScene* m_pScene;

};
