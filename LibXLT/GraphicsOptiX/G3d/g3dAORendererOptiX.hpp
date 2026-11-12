/*****************************************************************************
**  g3dAORendererOptiX.hpp
**
**      g3dAORendererOptiX is the OptiX backend's scene renderer: a
**	progressive ambient occlusion path tracer for the scene's triangles,
**	with GraphicsVulkan's forward renderer drawing what a tracer shouldn't
**	(lines, gizmos, the camera- and screen-space layers) over the traced
**	image. Render:
**
**	1. Updates its scene BVH (g3dSceneBVHOptiX: GAS from the cache, IAS).
**	2. Unless the image has converged, launches GetSamplesPerFrame more AO
**	   samples per pixel into its accumulation buffer, then resolves them
**	   into the shared colour buffer and writes depth (optxAO, on the OptiX
**	   stream), and signals the frame semaphore (optxFrameShare).
**	3. Copies the shared colour and depth buffers into the target's colour
**	   and depth images (Vulkan waits for the frame semaphore).
**	4. Draws the rest with g3dSceneRendererVk, without clearing and
**	   skipping the nodes the tracer drew (g3dSceneBVHOptiX::IsTraced),
**	   depth-testing against the traced depth.
**	It returns the triangles traced plus those the raster drew.
**
**	Accumulation restarts (sample count back to 0) when the scene version,
**	the view-projection matrix (which holds the capture jitter), the
**	target's size, the AO radius (the scene's ssaoParams::m_AORadius) or
**	rays per sample, the seed, or the background (the colour the target
**	was last cleared to: vkdImage::LastClearColor) changes, or on Restart.
**	At GetSampleLimit samples it stops launching: Render only copies the
**	last frame again, and the GPU's tracing is idle while nothing changes.
**	Until then, each Render asks for another frame (g3dProgressive::
**	RequestFrame), so MachStudio keeps drawing while the image converges.
**	While a capture runs (g3dProgressive::IsCapturing), Render launches
**	until GetCaptureSamples are reached (or the capture is aborted) and
**	resolves once, so the captured frame is the finished one; a frame in
**	which nothing changed reuses the last one.
**	Misses get the background; hits the unoccluded fraction, encoded as
**	sRGB, as grey.
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
#ifdef GOX_G3DAORENDEREROPTIX_HPP
#error g3dAORendererOptiX.hpp multiply included
#endif
#define GOX_G3DAORENDEREROPTIX_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/G3d/g3dSceneRenderer.hpp"
#endif
#ifndef GOX_G3DSCENEBVHOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dSceneBVHOptiX.hpp"
#endif
#ifndef GOX_OPTXAO_HPP
#include "GraphicsOptiX/optx/optxAO.hpp"
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

	class g3dAORendererOptiX : public g3dSceneRenderer
	{
		public:
			//	Defaults of the settings below
			static const unsigned c_DefaultSamplesPerFrame = 1;
			static const unsigned c_DefaultSampleLimit = 1024;
			static const unsigned c_DefaultAORays = 1;

			g3dAORendererOptiX(g3dGasCacheOptiX& i_Cache, gvk::vkdDevice& i_Device, gvk::vkdContext& i_Context);
			virtual ~g3dAORendererOptiX();
			g3dAORendererOptiX(const g3dAORendererOptiX&) = delete;
			g3dAORendererOptiX& operator=(const g3dAORendererOptiX&) = delete;

			//--------------------------------------------------------------------
			//	g3dSceneRenderer
			//--------------------------------------------------------------------
			virtual int Render(g2dRenderTarget* i_pTarget, const camCamera& i_Camera, const g3dScene& i_Scene,
							   const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime);
			virtual void ReleaseResources();

			//--------------------------------------------------------------------
			//	Settings: samples per pixel each Render adds (at least 1), the
			//	samples per pixel after which it stops (at least 1), occlusion
			//	rays per sample (a change restarts), and the random seed (a
			//	change restarts)
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
			void SetSeed(unsigned i_Seed);

			//--------------------------------------------------------------------
			//	Restart - the next Render starts accumulating again
			//--------------------------------------------------------------------
			void Restart() { m_bRestart = true; }

			//--------------------------------------------------------------------
			//	For tests and the profiler: samples per pixel accumulated so far,
			//	AO launches so far, restarts so far, the scene BVH, the shared
			//	frame buffers
			//--------------------------------------------------------------------
			unsigned GetSampleCount() const { return m_nSamples; }
			int GetLaunchCount() const { return m_nLaunches; }
			int GetRestartCount() const { return m_nRestarts; }
			g3dSceneBVHOptiX& GetSceneBVH() { return m_BVH; }
			optxFrameShare& GetFrameShare() { return m_Share; }

		private:
			//	What restarts accumulation when it changes
			struct Key
			{
				uint64_t	m_SceneVersion = 0;
				double		m_ViewProj[16] = {};
				unsigned	m_Width = 0;
				unsigned	m_Height = 0;
				float		m_Radius = 0;
				unsigned	m_AORays = 0;
				unsigned	m_Seed = 0;
				float		m_Background[3] = {};

				bool operator==(const Key& i_Other) const;
			};

			int raster_only(g2dRenderTarget* i_pTarget, const camCamera& i_Camera, const g3dScene& i_Scene,
							const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime);
			bool trace(const Key& i_Key, const optxColorOrder i_Order, const optxDepthLayout i_Layout,
					   unsigned i_nGoal, bool i_bAll);

			gvk::vkdContext&			m_Context;
			g3dSceneBVHOptiX			m_BVH;
			optxAO						m_AO;
			bool						m_bAOFailed;
			optxFrameShare				m_Share;
			cuBuffer					m_Accum;
			gvk::g3dSceneRendererVk		m_Overlay;
			unsigned					m_nSamplesPerFrame;
			unsigned					m_nSampleLimit;
			unsigned					m_nCaptureSamples;
			unsigned					m_nAORays;
			unsigned					m_Seed;
			Key							m_Key;
			bool						m_bRestart;
			bool						m_bHaveFrame;		// the shared buffers hold a frame for m_Key
			unsigned					m_nSamples;
			int							m_nLaunches;
			int							m_nRestarts;
			bool						m_bLoggedTarget;
	};
}
