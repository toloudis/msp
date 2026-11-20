/*****************************************************************************
**  g3dTraceRendererOptiX.hpp
**
**      g3dTraceRendererOptiX is the OptiX backend's scene renderer: a
**	progressive path tracer for the scene's triangles, with GraphicsVulkan's
**	forward renderer drawing what a tracer shouldn't (lines, gizmos, the
**	camera- and screen-space layers) over the traced image. Its mode picks
**	what it traces:
**	- e_AO: ambient occlusion (optxAO): hits the unoccluded fraction,
**	  encoded as sRGB, as grey; misses the background (the colour the
**	  target was last cleared to);
**	- e_Illumination: the Illumination Only pass (optxLighting): the light
**	  that reaches the surfaces from the scene's lights (projectors'
**	  textures included), with traced soft shadows and diffuse bounces that
**	  carry the surfaces' albedo; linear, as the raster pass writes it;
**	  misses black;
**	- e_ShadowMask: the Shadow Mask pass (optxLighting): the projected
**	  lights' traced shadows; linear; misses white.
**	In e_AO mode, the tracer debug view (g3dRenderPrefs::m_TraceDebugView,
**	or SetDebugView) shows a material quantity of the first hit instead
**	(optxLighting's debug mode), unlit and linear.
**
**	Every mode reads materials (g3dMaterialTableOptiX) and their textures
**	(g3dTextureTableOptiX): cut-out and transparent surfaces let camera,
**	bounce and shadow rays through.
**
**	Render:
**
**	1. Updates its scene BVH (g3dSceneBVHOptiX: GAS from the cache, IAS),
**	   its material table, its light table (lighting: g3dLightTableOptiX)
**	   and the texture table both fill; CUDA waits for Vulkan's last write
**	   to those textures.
**	2. Unless the image has converged, launches GetSamplesPerFrame more
**	   samples per pixel into its accumulation buffer, resolving them into
**	   the shared colour and depth buffers in the same launch (on the OptiX
**	   stream), and signals the frame semaphore (optxFrameShare, double
**	   buffered, so tracing the next frame doesn't wait for Vulkan's copy of
**	   this one).
**	3. Copies the shared colour and depth buffers into the target's colour
**	   and depth images (Vulkan waits for the frame semaphore).
**	4. Draws the rest with g3dSceneRendererVk, without clearing and
**	   skipping the nodes the tracer drew and strand hair (g3dSceneBVHOptiX::
**	   IsRasterOverlay), depth-testing against the traced depth.
**	It returns the triangles traced plus those the raster drew.
**
**	Settings: samples per frame, the sample limit, samples per captured
**	frame, AO rays per sample and bounces come from the render preferences
**	(g3dPrefs::m_Trace*) when the renderer is made, and again whenever the
**	preferences change; the setters below override them until then.
**
**	Accumulation restarts (sample count back to 0) when the scene version,
**	the view-projection matrix (which holds the capture jitter), the
**	target's size, the mode, the seed or a setting that changes the image
**	(AO: the AO radius, the scene's ssaoParams::m_AORadius, AO rays and the
**	background; lighting: the light table's version, bounces, the albedo
**	override, light sampling, and the preferences for shadows and for
**	illumination using normals; every mode: the material table's version,
**	the texture table's (a texture's contents changing), and the debug
**	view) changes, or on Restart. At the sample limit it stops
**	launching: Render only copies the last frame again, and the GPU's
**	tracing is idle while nothing changes. Until then, each Render asks for
**	another frame (g3dProgressive::RequestFrame), so MachStudio keeps
**	drawing while the image converges. While a capture runs
**	(g3dProgressive::IsCapturing), Render launches until the capture
**	samples are reached (or the capture is aborted) and resolves once, so
**	the captured frame is the finished one; a frame in which nothing
**	changed reuses the last one.
**
**	A target whose colour isn't R8G8B8A8 or B8G8R8A8, or is multisampled
**	(or whose depth isn't D24S8 or D32), isn't traced: it is cleared and
**	only the raster part is drawn (logged once).
**
**	Render thread. Must be destroyed before the GAS cache and the Vulkan
**	device.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DTRACERENDEREROPTIX_HPP
#error g3dTraceRendererOptiX.hpp multiply included
#endif
#define GOX_G3DTRACERENDEREROPTIX_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/G3d/g3dSceneRenderer.hpp"
#endif
#ifndef GOX_G3DLIGHTTABLEOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dLightTableOptiX.hpp"
#endif
#ifndef GOX_G3DMATERIALTABLEOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dMaterialTableOptiX.hpp"
#endif
#ifndef GOX_G3DTEXTURETABLEOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dTextureTableOptiX.hpp"
#endif
#ifndef GOX_G3DSCENEBVHOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dSceneBVHOptiX.hpp"
#endif
#ifndef GOX_OPTXAO_HPP
#include "GraphicsOptiX/optx/optxAO.hpp"
#endif
#ifndef GOX_OPTXLIGHTING_HPP
#include "GraphicsOptiX/optx/optxLighting.hpp"
#endif
#ifndef GOX_OPTXFRAMESHARE_HPP
#include "GraphicsOptiX/optx/optxFrameShare.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dSceneRendererVk.hpp"
#endif

#include <cstdint>

namespace gvk
{
	class vkdContext;
	class vkdDevice;
}

namespace gox
{
	class g3dGasCacheOptiX;

	class g3dTraceRendererOptiX : public g3dSceneRenderer
	{
		public:
			enum Mode
			{
				e_AO,
				e_Illumination,
				e_ShadowMask
			};

			//	Defaults of the settings below (the render preferences have
			//	the same ones)
			static const unsigned c_DefaultSamplesPerFrame = 1;
			static const unsigned c_DefaultSampleLimit = 1024;
			static const unsigned c_DefaultAORays = 1;
			static const unsigned c_DefaultBounces = 2;
			static constexpr float c_DefaultAlbedo = 0.8f;

			g3dTraceRendererOptiX(Mode i_Mode, g3dGasCacheOptiX& i_Cache, gvk::vkdDevice& i_Device, gvk::vkdContext& i_Context);
			virtual ~g3dTraceRendererOptiX();
			g3dTraceRendererOptiX(const g3dTraceRendererOptiX&) = delete;
			g3dTraceRendererOptiX& operator=(const g3dTraceRendererOptiX&) = delete;

			Mode GetMode() const { return m_Mode; }

			//--------------------------------------------------------------------
			//	g3dSceneRenderer
			//--------------------------------------------------------------------
			virtual int Render(g2dRenderTarget* i_pTarget, const camCamera& i_Camera, const g3dScene& i_Scene,
							   const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime);
			virtual void ReleaseResources();

			//--------------------------------------------------------------------
			//	Settings: samples per pixel each Render adds (at least 1), the
			//	samples per pixel after which it stops (at least 1), occlusion
			//	rays per AO sample, bounces after the first hit (lighting; 0:
			//	direct light only), an albedo every surface bounces leave with
			//	in place of its material's (lighting, in [0, 1]; for tests;
			//	ClearAlbedo goes back to the materials'), how area lights are
			//	sampled (lighting; for tests), the debug view (an optxDebugView;
			//	-1, the default: the preference's), and the random seed
			//--------------------------------------------------------------------
			void SetSamplesPerFrame(unsigned i_nSamples);
			unsigned GetSamplesPerFrame() const { return m_nSamplesPerFrame; }
			void SetSampleLimit(unsigned i_nSamples);
			unsigned GetSampleLimit() const { return m_nSampleLimit; }
			//	Samples per pixel of a captured frame (0, the default: the limit)
			void SetCaptureSamples(unsigned i_nSamples);
			unsigned GetCaptureSamples() const;
			void SetAORays(unsigned i_nRays);
			unsigned GetAORays() const { return m_nAORays; }
			void SetBounces(unsigned i_nBounces);
			unsigned GetBounces() const { return m_nBounces; }
			void SetAlbedo(float i_Albedo);
			void ClearAlbedo() { m_bAlbedoOverride = false; }
			float GetAlbedo() const { return m_Albedo; }
			void SetDebugView(int i_View) { m_DebugView = i_View; }
			void SetLightSampling(optxLightSampling i_Sampling);
			void SetSeed(unsigned i_Seed);

			//--------------------------------------------------------------------
			//	Restart - the next Render starts accumulating again
			//--------------------------------------------------------------------
			void Restart() { m_bRestart = true; }

			//--------------------------------------------------------------------
			//	For tests and the profiler: samples per pixel accumulated so far,
			//	launches so far, restarts so far, the scene BVH, the light
			//	table, the shared frame buffers
			//--------------------------------------------------------------------
			unsigned GetSampleCount() const { return m_nSamples; }
			int GetLaunchCount() const { return m_nLaunches; }
			int GetRestartCount() const { return m_nRestarts; }
			g3dSceneBVHOptiX& GetSceneBVH() { return m_BVH; }
			g3dLightTableOptiX& GetLightTable() { return m_Lights; }
			g3dMaterialTableOptiX& GetMaterialTable() { return m_Materials; }
			g3dTextureTableOptiX& GetTextureTable() { return m_Textures; }
			optxFrameShare& GetFrameShare() { return m_Share; }

		private:
			//	What restarts accumulation when it changes
			struct Key
			{
				uint64_t	m_SceneVersion = 0;
				uint64_t	m_LightVersion = 0;
				uint64_t	m_MaterialVersion = 0;
				uint64_t	m_TextureVersion = 0;
				unsigned	m_DebugView = 0;
				unsigned	m_bAlbedoOverride = 0;
				double		m_ViewProj[16] = {};
				unsigned	m_Width = 0;
				unsigned	m_Height = 0;
				unsigned	m_Mode = 0;
				float		m_Radius = 0;
				unsigned	m_AORays = 0;
				unsigned	m_Bounces = 0;
				float		m_Albedo = 0;
				unsigned	m_Sampling = 0;
				unsigned	m_bUseCosine = 0;
				unsigned	m_bShadows = 0;
				unsigned	m_Seed = 0;
				float		m_Background[3] = {};

				bool operator==(const Key& i_Other) const;
			};

			//	The tracer settings of the render preferences last applied
			struct PrefSettings
			{
				unsigned	m_SamplesPerFrame = 0;
				unsigned	m_SampleLimit = 0;
				unsigned	m_CaptureSamples = 0;
				unsigned	m_AORays = 0;
				unsigned	m_Bounces = 0;

				bool operator==(const PrefSettings& i_Other) const;
			};

			void apply_prefs();
			bool lighting() const { return m_Mode != e_AO; }
			unsigned debug_view() const;
			bool create_programs(bool i_bLighting);
			int raster_only(g2dRenderTarget* i_pTarget, const camCamera& i_Camera, const g3dScene& i_Scene,
							const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime);
			bool trace(const Key& i_Key, const optxColorOrder i_Order, const optxDepthLayout i_Layout,
					   unsigned i_nGoal, bool i_bAll);

			Mode						m_Mode;
			g3dGasCacheOptiX&			m_Cache;
			gvk::vkdContext&			m_Context;
			g3dSceneBVHOptiX			m_BVH;
			g3dTextureTableOptiX		m_Textures;
			g3dMaterialTableOptiX		m_Materials;
			g3dLightTableOptiX			m_Lights;
			optxAO						m_AO;
			optxLighting				m_Lighting;
			bool						m_bProgramsFailed;
			optxFrameShare				m_Share;
			cuBuffer					m_Accum;
			gvk::g3dSceneRendererVk		m_Overlay;
			PrefSettings				m_Prefs;
			unsigned					m_nSamplesPerFrame;
			unsigned					m_nSampleLimit;
			unsigned					m_nCaptureSamples;
			unsigned					m_nAORays;
			unsigned					m_nBounces;
			float						m_Albedo;
			bool						m_bAlbedoOverride;
			int							m_DebugView;
			optxLightSampling			m_Sampling;
			unsigned					m_Seed;
			Key							m_Key;
			bool						m_bRestart;
			bool						m_bHaveFrame;		// the shared buffers hold a frame for m_Key
			unsigned					m_nSamples;
			int							m_nLaunches;
			int							m_nRestarts;
			bool						m_bLoggedTarget;
			unsigned					m_nLoggedInstances;		// the BVH's sizes last logged
			unsigned					m_nLoggedGas;
	};
}
