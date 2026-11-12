/*****************************************************************************
**  g3dAORendererOptiX.cpp
**
**      See g3dAORendererOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DAORENDEREROPTIX_HPP
#include "GraphicsOptiX/G3d/g3dAORendererOptiX.hpp"
#endif
#ifndef GOX_G3DGASCACHEOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dGasCacheOptiX.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef G3D_PROGRESSIVE_HPP
#include "Graphics/G3d/g3dProgressive.hpp"
#endif
#ifndef G3D_THREADCONTROL_HPP
#include "Graphics/G3d/g3dThreadControl.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>

namespace gox
{
	namespace
	{
		//	The raster's world view-projection (g3dSceneRenderUtil::
		//	SetViewingTransforms for world space: camera, then projection
		//	mirrored in x for D3D's left-handed coordinates), row vectors
		void view_projection(const camCamera& i_Camera, double o_ViewProj[16])
		{
			maMatrix4x4 camera, projection;
			i_Camera.GetCameraMatrix(camera);
			i_Camera.GetProjectionMatrix(projection);
			projection.ScaleBy(-1.0f, 1.0f, 1.0f);
			for (int r = 0; r < 4; ++r)
			{
				for (int c = 0; c < 4; ++c)
				{
					double sum = 0;
					for (int k = 0; k < 4; ++k)
					{
						sum += double(camera(r, k)) * double(projection(k, c));
					}
					o_ViewProj[r * 4 + c] = sum;
				}
			}
		}

		//	Gauss-Jordan with partial pivoting; false if singular
		bool invert(const double i_M[16], double o_Inverse[16])
		{
			double a[4][8];
			for (int r = 0; r < 4; ++r)
			{
				for (int c = 0; c < 4; ++c)
				{
					a[r][c] = i_M[r * 4 + c];
					a[r][c + 4] = (r == c) ? 1.0 : 0.0;
				}
			}
			for (int c = 0; c < 4; ++c)
			{
				int pivot = c;
				for (int r = c + 1; r < 4; ++r)
				{
					if (std::fabs(a[r][c]) > std::fabs(a[pivot][c]))
					{
						pivot = r;
					}
				}
				if (std::fabs(a[pivot][c]) < 1e-300)
				{
					return false;
				}
				if (pivot != c)
				{
					for (int k = 0; k < 8; ++k)
					{
						std::swap(a[c][k], a[pivot][k]);
					}
				}
				const double scale = 1.0 / a[c][c];
				for (int k = 0; k < 8; ++k)
				{
					a[c][k] *= scale;
				}
				for (int r = 0; r < 4; ++r)
				{
					if (r != c && a[r][c] != 0.0)
					{
						const double f = a[r][c];
						for (int k = 0; k < 8; ++k)
						{
							a[r][k] -= f * a[c][k];
						}
					}
				}
			}
			for (int r = 0; r < 4; ++r)
			{
				for (int c = 0; c < 4; ++c)
				{
					o_Inverse[r * 4 + c] = a[r][c + 4];
				}
			}
			return true;
		}

		float srgb_to_linear(float c)
		{
			return c <= 0.04045f ? c / 12.92f : std::pow((c + 0.055f) / 1.055f, 2.4f);
		}
	}

	bool g3dAORendererOptiX::Key::operator==(const Key& i_Other) const
	{
		return m_SceneVersion == i_Other.m_SceneVersion && m_Width == i_Other.m_Width && m_Height == i_Other.m_Height &&
			   m_Radius == i_Other.m_Radius && m_AORays == i_Other.m_AORays && m_Seed == i_Other.m_Seed &&
			   std::memcmp(m_ViewProj, i_Other.m_ViewProj, sizeof(m_ViewProj)) == 0 &&
			   std::memcmp(m_Background, i_Other.m_Background, sizeof(m_Background)) == 0;
	}

	g3dAORendererOptiX::g3dAORendererOptiX(g3dGasCacheOptiX& i_Cache, gvk::vkdDevice& i_Device, gvk::vkdContext& i_Context)
		: m_Context(i_Context)
		, m_BVH(i_Cache)
		, m_bAOFailed(false)
		, m_Share(i_Device, i_Context, i_Cache.GetMemoryCache(), i_Cache.GetVulkanTimeline())
		, m_nSamplesPerFrame(c_DefaultSamplesPerFrame)
		, m_nSampleLimit(c_DefaultSampleLimit)
		, m_nCaptureSamples(0)
		, m_nAORays(c_DefaultAORays)
		, m_Seed(0)
		, m_bRestart(true)
		, m_bHaveFrame(false)
		, m_nSamples(0)
		, m_nLaunches(0)
		, m_nRestarts(0)
		, m_bLoggedTarget(false)
		, m_nLoggedInstances(0)
		, m_nLoggedGas(0)
	{
		m_Overlay.SetClearTarget(false);
		m_Overlay.SetNodeFilter([](const g3dSceneNode& i_Node) { return !g3dSceneBVHOptiX::IsTraced(i_Node); });
	}

	g3dAORendererOptiX::~g3dAORendererOptiX()
	{
		if (optxContext::Exists())
		{
			optxContext::MakeCurrent();
		}
	}

	void g3dAORendererOptiX::ReleaseResources()
	{
		optxContext::MakeCurrent();
		m_Share.Release();
		m_Accum.Free();
		m_BVH.Release();
		m_bHaveFrame = false;
		m_bRestart = true;
	}

	void g3dAORendererOptiX::SetSamplesPerFrame(unsigned i_nSamples)
	{
		m_nSamplesPerFrame = std::max(1u, i_nSamples);
	}

	void g3dAORendererOptiX::SetSampleLimit(unsigned i_nSamples)
	{
		m_nSampleLimit = std::max(1u, i_nSamples);
	}

	void g3dAORendererOptiX::SetCaptureSamples(unsigned i_nSamples)
	{
		m_nCaptureSamples = i_nSamples;
	}

	unsigned g3dAORendererOptiX::GetCaptureSamples() const
	{
		return m_nCaptureSamples ? m_nCaptureSamples : m_nSampleLimit;
	}

	void g3dAORendererOptiX::SetAORays(unsigned i_nRays)
	{
		m_nAORays = i_nRays;
	}

	void g3dAORendererOptiX::SetSeed(unsigned i_Seed)
	{
		m_Seed = i_Seed;
	}

	int g3dAORendererOptiX::raster_only(g2dRenderTarget* i_pTarget, const camCamera& i_Camera, const g3dScene& i_Scene,
										const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime)
	{
		m_Overlay.SetClearTarget(true);
		const int triangles = m_Overlay.Render(i_pTarget, i_Camera, i_Scene, i_ViewerLayers, i_fSimTime);
		m_Overlay.SetClearTarget(false);
		return triangles;
	}

	int g3dAORendererOptiX::Render(g2dRenderTarget* i_pTarget, const camCamera& i_Camera, const g3dScene& i_Scene,
								   const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime)
	{
		G3D_PROFILE_SCOPE("AORendererOptiX");
		optxContext::MakeCurrent();

		//	The target's images, as vkdContext has them once it is current
		i_pTarget->MakeCurrent();
		gvk::vkdImage* colors[8] = {};
		gvk::vkdImage* depth = NULL;
		m_Context.GetRenderTargets(colors, &depth);
		gvk::vkdImage* color = colors[0];
		if (!color)
		{
			return 0;	// the frame is skipped (a window with no area)
		}
		optxColorOrder order = e_optxColorRGBA;
		optxDepthLayout layout = e_optxDepthD24;
		if (!optxFrameShare::GetColorOrder(color->Format(), order) || color->Samples() != VK_SAMPLE_COUNT_1_BIT ||
			(depth && (!optxFrameShare::GetDepthLayout(depth->Format(), layout) || depth->Samples() != VK_SAMPLE_COUNT_1_BIT)))
		{
			if (!m_bLoggedTarget)
			{
				DBG_LOG("GraphicsOptiX: a render target of format " << int(color->Format()) << " (" << int(color->Samples())
						<< " samples) can't show traced frames; drawing only the raster part");
				m_bLoggedTarget = true;
			}
			return raster_only(i_pTarget, i_Camera, i_Scene, i_ViewerLayers, i_fSimTime);
		}
		if (!m_AO.IsCreated())
		{
			std::string error;
			if (m_bAOFailed || !m_AO.Create(error))
			{
				if (!m_bAOFailed)
				{
					DBG_LOG("GraphicsOptiX: " << error);
					m_bAOFailed = true;
				}
				return raster_only(i_pTarget, i_Camera, i_Scene, i_ViewerLayers, i_fSimTime);
			}
		}

		{
			G3D_PROFILE_SCOPE("SceneBVH");
			m_BVH.Update(i_Scene, i_ViewerLayers, g3dPrefs::CurrentPrefs().m_bLowResolution);
		}
		if (m_BVH.GetInstanceCount() != m_nLoggedInstances || m_BVH.GetGasCount() != m_nLoggedGas)
		{
			m_nLoggedInstances = m_BVH.GetInstanceCount();
			m_nLoggedGas = m_BVH.GetGasCount();
			DBG_LOG("GraphicsOptiX: scene BVH of " << m_nLoggedInstances << " instances, " << m_nLoggedGas << " GAS ("
					<< (m_BVH.GetGasBytes() >> 20) << " MB), " << m_BVH.GetTriangleCount() << " triangles");
		}

		Key key;
		key.m_SceneVersion = m_BVH.GetSceneVersion();
		view_projection(i_Camera, key.m_ViewProj);
		key.m_Width = color->Width();
		key.m_Height = color->Height();
		ssaoParams ao_settings;
		i_Scene.GetSSAOSettings(ao_settings);
		key.m_Radius = ao_settings.m_AORadius;
		key.m_AORays = m_nAORays;
		key.m_Seed = m_Seed;
		//	what g3dTargetRenderer cleared the target to
		const float* background = color->LastClearColor();
		key.m_Background[0] = background[0];
		key.m_Background[1] = background[1];
		key.m_Background[2] = background[2];
		if (m_bRestart || !(key == m_Key))
		{
			m_Key = key;
			m_bRestart = false;
			m_bHaveFrame = false;
			m_nSamples = 0;
			++m_nRestarts;
		}

		//	Capturing, the frame is finished before it is shown; else a few
		//	samples, and another frame asked for until the limit
		const bool capturing = g3dProgressive::IsCapturing();
		const unsigned goal = capturing ? GetCaptureSamples() : m_nSampleLimit;
		if (m_nSamples < goal)
		{
			if (!trace(m_Key, order, layout, goal, capturing))
			{
				m_bHaveFrame = false;
				return raster_only(i_pTarget, i_Camera, i_Scene, i_ViewerLayers, i_fSimTime);
			}
			if (!capturing && m_nSamples < goal)
			{
				g3dProgressive::RequestFrame();
			}
		}

		if (m_bHaveFrame)
		{
			G3D_PROFILE_SCOPE("Composite");
			m_Share.CopyTo(color, depth);
		}

		int triangles = m_BVH.GetTriangleCount();
		{
			G3D_PROFILE_SCOPE("RasterOverlay");
			triangles += m_Overlay.Render(i_pTarget, i_Camera, i_Scene, i_ViewerLayers, i_fSimTime);
		}
		return triangles;
	}

	//	Step 2 of Render: more samples toward i_nGoal (one launch, or with
	//	i_bAll launches until it is reached or the capture is aborted), then
	//	resolved into the shared buffers
	bool g3dAORendererOptiX::trace(const Key& i_Key, const optxColorOrder i_Order, const optxDepthLayout i_Layout,
								   unsigned i_nGoal, bool i_bAll)
	{
		const size_t accum_bytes = size_t(i_Key.m_Width) * i_Key.m_Height * 4 * sizeof(float);
		if (!m_Share.Resize(i_Key.m_Width, i_Key.m_Height) || !m_Accum.Reserve(accum_bytes))
		{
			return false;
		}

		optxAOParams params = {};
		params.m_Handle = m_BVH.GetHandle();
		params.m_Instances = m_BVH.GetInstanceTable();
		params.m_Accum = m_Accum.Get();
		params.m_Color = m_Share.Color();
		params.m_Depth = m_Share.Depth();
		params.m_Width = i_Key.m_Width;
		params.m_Height = i_Key.m_Height;
		double inverse[16];
		if (!invert(i_Key.m_ViewProj, inverse))
		{
			return false;
		}
		for (int i = 0; i < 16; ++i)
		{
			params.m_ViewProj[i] = float(i_Key.m_ViewProj[i]);
			params.m_InvViewProj[i] = float(inverse[i]);
		}
		for (int c = 0; c < 3; ++c)
		{
			params.m_Background[c] = srgb_to_linear(i_Key.m_Background[c]);
		}
		params.m_Radius = i_Key.m_Radius;
		//	Off the surface by a little of the scene's size
		const maAxisBox& bounds = m_BVH.GetBounds();
		float size = 1.0f;
		if (!bounds.IsEmpty())
		{
			const float dx = bounds.GetMaxX() - bounds.GetMinX();
			const float dy = bounds.GetMaxY() - bounds.GetMinY();
			const float dz = bounds.GetMaxZ() - bounds.GetMinZ();
			size = std::sqrt(dx * dx + dy * dy + dz * dz);
		}
		params.m_Epsilon = std::max(size * 1e-5f, 1e-5f);
		params.m_AORays = i_Key.m_AORays;
		params.m_ColorOrder = i_Order;
		params.m_DepthLayout = i_Layout;
		params.m_Seed = i_Key.m_Seed;

		if (!m_Share.BeginWrite())
		{
			return false;
		}
		do
		{
			G3D_PROFILE_SCOPE("AOLaunch");
			params.m_FirstSample = m_nSamples;
			params.m_Samples = std::min(m_nSamplesPerFrame, i_nGoal - m_nSamples);
			if (!m_AO.Accumulate(params, m_BVH.GetSbtRecordCount()))
			{
				return false;
			}
			m_nSamples += params.m_Samples;
			++m_nLaunches;
		}
		while (i_bAll && m_nSamples < i_nGoal && !g3dThreadControl::ShouldRenderThreadAbort());
		params.m_FirstSample = m_nSamples;
		params.m_Samples = 0;
		{
			G3D_PROFILE_SCOPE("AOResolve");
			if (!m_AO.Resolve(params))
			{
				return false;
			}
		}
		if (!m_Share.EndWrite())
		{
			return false;
		}
		m_bHaveFrame = true;
		return true;
	}
}
