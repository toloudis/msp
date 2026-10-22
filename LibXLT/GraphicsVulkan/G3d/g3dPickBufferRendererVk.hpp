/****************************************************************************\
**	g3dPickBufferRendererVk.hpp
**
**	GraphicsVulkan port of GraphicsDX11's g3dPickBufferRendererDX11: renders
**	the scene with solid colors so that you can tell which object is visible
**	at each point (pick3dPickBuffer draws one pixel into a 1x1 RGBA32F
**	render-target texture and GetPickInfo reads it back).
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G3D_PICKBUFFERRENDERERVK_HPP
#error g3dPickBufferRendererVk.hpp multiply included
#endif
#define GVK_G3D_PICKBUFFERRENDERERVK_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class effMaskAlphaData;
class g3dSceneNode;
class g3dLayer;
class maFloatRGBA;
class matMaterial;

namespace gvk
{
	class g3dPickBufferRendererVk : public g3dPickRenderer
	{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		g3dPickBufferRendererVk();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~g3dPickBufferRendererVk();

		//--------------------------------------------------------------------
		//	Render renders the scene node hierarchy.
		//	Returns the number of triangles rendered
		//--------------------------------------------------------------------
		int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
					 const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
					 float i_fSimTime );

		void ReleaseResources(){}

		//--------------------------------------------------------------------
		//	set a 1x1 viewport before clearing.
		//--------------------------------------------------------------------
		virtual void SetPickViewport(g2dRenderTarget* i_pTarget);

		//--------------------------------------------------------------------
		//	restore viewport
		//--------------------------------------------------------------------
		virtual void RestoreViewport();

		//--------------------------------------------------------------------
		//	return info about the picked object.
		//--------------------------------------------------------------------
		virtual void GetPickInfo(g2dRenderTarget* i_pWindow, g3dPickInfo& o_PickInfo);

		static void InitStates();
		static void CleanupStates();

	private:
		void world_space_render( g3dSceneNode* i_pNode,
								 const maPoint3d &i_CameraPos,
								 bool i_bRenderLowRes );
		void setup_layer(g3dLayer& i_Layer);
		void render_layer(g3dLayer& i_Layer, const camCamera& i_Camera);

		matMaterial* m_pColorCodeMat;
		effMaskAlphaData* m_pMaskAlphaData;
		uint32_t	m_FragmentCount;

		int DrawNode(const g3dSceneNode* i_pNode);

		// save current viewports to be restored after pick render.
		vkdViewport m_OldViewports[vkdContext::c_MaxViewports];
		uint32_t	m_nOldViewports;
	};
}
