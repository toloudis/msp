/*****************************************************************************
**  g3dTraceRendererOptiX.cpp
**
**      See g3dTraceRendererOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DTRACERENDEREROPTIX_HPP
#include "GraphicsOptiX/G3d/g3dTraceRendererOptiX.hpp"
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
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
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
		void view_projection(const camCamera& i_Camera, double o_ViewProj[16], double o_View[16])
		{
			maMatrix4x4 camera, projection;
			i_Camera.GetCameraMatrix(camera);
			i_Camera.GetProjectionMatrix(projection);
			for (int r = 0; r < 4; ++r)
			{
				for (int c = 0; c < 4; ++c)
				{
					o_View[r * 4 + c] = camera(r, c);
				}
			}
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

		//	The smallest n with 2^n >= v (at most 31)
		unsigned ceil_log2(unsigned v)
		{
			unsigned n = 0;
			while (n < 31 && (1u << n) < v)
			{
				++n;
			}
			return n;
		}

		float srgb_to_linear(float c)
		{
			return c <= 0.04045f ? c / 12.92f : std::pow((c + 0.055f) / 1.055f, 2.4f);
		}

		//	(x, y, z, 1) * M / w, row vectors
		void unproject(const double i_M[16], double x, double y, double z, double o_P[3])
		{
			double p[4];
			for (int c = 0; c < 4; ++c)
			{
				p[c] = x * i_M[c] + y * i_M[4 + c] + z * i_M[8 + c] + i_M[12 + c];
			}
			for (int c = 0; c < 3; ++c)
			{
				o_P[c] = p[c] / p[3];
			}
		}

		//	A camera ray through NDC (x, y), as the programs make it: its
		//	origin on the near plane and its direction
		void camera_ray(const double i_InvViewProj[16], double x, double y, double o_Origin[3], double o_Direction[3])
		{
			double mid[3];
			unproject(i_InvViewProj, x, y, 0.0, o_Origin);
			unproject(i_InvViewProj, x, y, 0.5, mid);
			double length = 0.0;
			for (int c = 0; c < 3; ++c)
			{
				o_Direction[c] = mid[c] - o_Origin[c];
				length += o_Direction[c] * o_Direction[c];
			}
			length = std::sqrt(length);
			for (int c = 0; c < 3; ++c)
			{
				o_Direction[c] /= length > 0.0 ? length : 1.0;
			}
		}

		//	A pixel's ray cone at the centre of the view: the distance between
		//	two neighbouring pixels' ray origins (an orthographic pixel's
		//	width) and the angle between their directions (a perspective
		//	pixel's)
		void pixel_cone(const double i_InvViewProj[16], unsigned i_Height, float& o_Width, float& o_Spread)
		{
			const double step = 2.0 / std::max(1u, i_Height);
			double o0[3], d0[3], o1[3], d1[3];
			camera_ray(i_InvViewProj, 0.0, 0.0, o0, d0);
			camera_ray(i_InvViewProj, 0.0, step, o1, d1);
			double distance = 0.0, cosine = 0.0;
			for (int c = 0; c < 3; ++c)
			{
				distance += (o1[c] - o0[c]) * (o1[c] - o0[c]);
				cosine += d0[c] * d1[c];
			}
			o_Width = float(std::sqrt(distance));
			o_Spread = float(std::acos(std::min(1.0, std::max(-1.0, cosine))));
		}
	}

	bool g3dTraceRendererOptiX::GetLens(const camCamera& i_Camera, float i_PixelAngle, float& o_Radius, float& o_Focus)
	{
		o_Radius = 0.0f;
		o_Focus = 0.0f;
		camDOFData dof;
		i_Camera.GetDOFParams(dof);
		if (!g3dPrefs::CurrentPrefs().m_bEnableDOF || dof.m_MaxFarBlur == -1 || !(i_PixelAngle > 1e-7f))
		{
			return false;
		}
		if (i_Camera.GetEnableALP())
		{
			//	A real lens: focal length in millimetres, the scene in centimetres
			const float stop = i_Camera.GetFStop(), length = i_Camera.GetFocalLength(), focus = i_Camera.GetFocalDistance();
			if (stop > 0.0f && length > 0.0f && focus > 0.0f)
			{
				o_Radius = length / (2.0f * stop) / 10.0f;
				o_Focus = focus;
			}
		}
		else
		{
			//	Derived from the distances: a point at depth d seen through an
			//	aperture A focused at s blurs to A |d - s| / d at the focus
			//	plane, where a pixel is s times its angle across
			float focus = 0.0f;
			if (dof.m_NearFocalDist > 0.0f && dof.m_FarFocalDist > 0.0f)
			{
				focus = 0.5f * (dof.m_NearFocalDist + dof.m_FarFocalDist);
			}
			else
			{
				focus = std::max(dof.m_NearFocalDist, dof.m_FarFocalDist);
			}
			if (focus > 0.0f && dof.m_MaxCoC > 0.0f)
			{
				float pixels = 0.0f, depth = 0.0f;
				if (dof.m_FarBlurDist > focus && dof.m_MaxFarBlur > 0.0f)
				{
					pixels = dof.m_MaxCoC * std::min(1.0f, dof.m_MaxFarBlur);
					depth = dof.m_FarBlurDist;
				}
				else if (dof.m_NearBlurDist > 0.0f && dof.m_NearBlurDist < focus)
				{
					pixels = dof.m_MaxCoC;
					depth = dof.m_NearBlurDist;
				}
				if (pixels > 0.0f)
				{
					const float aperture = pixels * focus * i_PixelAngle * depth / std::fabs(depth - focus);
					o_Radius = 0.5f * aperture;
					o_Focus = focus;
				}
			}
		}
		if (!(o_Radius > 0.0f) || !(o_Focus > 0.0f))
		{
			o_Radius = o_Focus = 0.0f;
			return false;
		}
		return true;
	}

	bool g3dTraceRendererOptiX::Key::operator==(const Key& i_Other) const
	{
		return m_SceneVersion == i_Other.m_SceneVersion && m_LightVersion == i_Other.m_LightVersion &&
			   m_MaterialVersion == i_Other.m_MaterialVersion && m_TextureVersion == i_Other.m_TextureVersion &&
			   m_EnvironmentVersion == i_Other.m_EnvironmentVersion &&
			   m_DebugView == i_Other.m_DebugView && m_bAlbedoOverride == i_Other.m_bAlbedoOverride &&
			   m_Width == i_Other.m_Width && m_Height == i_Other.m_Height && m_Mode == i_Other.m_Mode &&
			   m_Radius == i_Other.m_Radius && m_AORays == i_Other.m_AORays && m_Bounces == i_Other.m_Bounces &&
			   m_Albedo == i_Other.m_Albedo && m_Sampling == i_Other.m_Sampling && m_bUseCosine == i_Other.m_bUseCosine &&
			   m_bShadows == i_Other.m_bShadows && m_Seed == i_Other.m_Seed &&
			   m_Selection == i_Other.m_Selection && m_MaxRadiance == i_Other.m_MaxRadiance &&
			   m_LensRadius == i_Other.m_LensRadius && m_Focus == i_Other.m_Focus && m_Denoise == i_Other.m_Denoise &&
			   m_Filter == i_Other.m_Filter && m_FilterWidth == i_Other.m_FilterWidth &&
			   m_Log2Samples == i_Other.m_Log2Samples && m_Order == i_Other.m_Order &&
			   m_ImageWidth == i_Other.m_ImageWidth && m_ImageHeight == i_Other.m_ImageHeight &&
			   m_TargetX == i_Other.m_TargetX && m_TargetY == i_Other.m_TargetY && m_RegionX == i_Other.m_RegionX &&
			   m_RegionY == i_Other.m_RegionY && m_RegionWidth == i_Other.m_RegionWidth &&
			   m_RegionHeight == i_Other.m_RegionHeight &&
			   std::memcmp(m_ViewProj, i_Other.m_ViewProj, sizeof(m_ViewProj)) == 0 &&
			   std::memcmp(m_Background, i_Other.m_Background, sizeof(m_Background)) == 0;
	}

	bool g3dTraceRendererOptiX::Frame::operator==(const Frame& i_Other) const
	{
		return m_fTime == i_Other.m_fTime && std::memcmp(m_WholeViewProj, i_Other.m_WholeViewProj, sizeof(m_WholeViewProj)) == 0;
	}

	bool g3dTraceRendererOptiX::PrefSettings::operator==(const PrefSettings& i_Other) const
	{
		return m_SamplesPerFrame == i_Other.m_SamplesPerFrame && m_SampleLimit == i_Other.m_SampleLimit &&
			   m_CaptureSamples == i_Other.m_CaptureSamples && m_AORays == i_Other.m_AORays &&
			   m_Bounces == i_Other.m_Bounces && m_MaxRadiance == i_Other.m_MaxRadiance && m_Denoise == i_Other.m_Denoise;
	}

	g3dTraceRendererOptiX::g3dTraceRendererOptiX(Mode i_Mode, g3dGasCacheOptiX& i_Cache, gvk::vkdDevice& i_Device,
												 gvk::vkdContext& i_Context)
		: m_Mode(i_Mode)
		, m_Cache(i_Cache)
		, m_Context(i_Context)
		, m_BVH(i_Cache)
		, m_Textures(i_Cache.GetImageCache())
		, m_bProgramsFailed(false)
		, m_Share(i_Device, i_Context, i_Cache.GetMemoryCache(), i_Cache.GetVulkanTimeline())
		, m_nSamplesPerFrame(c_DefaultSamplesPerFrame)
		, m_nSampleLimit(c_DefaultSampleLimit)
		, m_nCaptureSamples(0)
		, m_nAORays(c_DefaultAORays)
		, m_nBounces(c_DefaultBounces)
		, m_Albedo(c_DefaultAlbedo)
		, m_bAlbedoOverride(false)
		, m_DebugView(-1)
		, m_Sampling(e_optxSampleBoth)
		, m_Selection(e_optxSelectOne)
		, m_MaxRadiance(0.0f)
		, m_Denoise(g3dPrefs::g3dRenderPrefs::e_TraceDenoiseOff)
		, m_Seed(0)
		, m_Order(e_optxOrderBlueNoise)
		, m_bRestart(true)
		, m_bHaveFrame(false)
		, m_bFrameDenoised(false)
		, m_bMeasuredFrame(false)
		, m_nSamples(0)
		, m_nLaunches(0)
		, m_nRestarts(0)
		, m_bLoggedTarget(false)
		, m_nLoggedInstances(0)
		, m_nLoggedGas(0)
	{
		m_Overlay.SetClearTarget(false);
		m_Overlay.SetNodeFilter([](const g3dSceneNode& i_Node) { return g3dSceneBVHOptiX::IsRasterOverlay(i_Node); });
		apply_prefs();
	}

	g3dTraceRendererOptiX::~g3dTraceRendererOptiX()
	{
		if (optxContext::Exists())
		{
			optxContext::MakeCurrent();
		}
	}

	void g3dTraceRendererOptiX::ReleaseResources()
	{
		optxContext::MakeCurrent();
		m_Share.Release();
		m_Accum.Free();
		m_Denoiser.Release();
		m_DenoiseInput.Free();
		m_FrameBuffer.ReleaseSurfaces();
		m_BVH.Release();
		m_Lights.Release();
		m_Environment.Release();
		m_Materials.Release();
		m_Textures.Release();
		m_bHaveFrame = false;
		m_bFrameDenoised = false;
		m_bMeasuredFrame = false;
		m_bRestart = true;
	}

	//	The preferences' tracer settings, if they changed since last applied
	//	(so a setter's value stands until the user changes a preference)
	void g3dTraceRendererOptiX::apply_prefs()
	{
		const g3dPrefs::g3dRenderPrefs& prefs = g3dPrefs::CurrentPrefs();
		PrefSettings settings;
		settings.m_SamplesPerFrame = unsigned(std::max(1, prefs.m_TraceSamplesPerFrame));
		settings.m_SampleLimit = unsigned(std::max(1, prefs.m_TraceSampleLimit));
		settings.m_CaptureSamples = unsigned(std::max(0, prefs.m_TraceCaptureSamples));
		settings.m_AORays = unsigned(std::max(0, prefs.m_TraceAORays));
		settings.m_Bounces = unsigned(std::max(0, prefs.m_TraceBounces));
		settings.m_MaxRadiance = std::max(0.0f, prefs.m_TraceMaxRadiance);
		settings.m_Denoise = prefs.m_TraceDenoise;
		if (settings == m_Prefs)
		{
			return;
		}
		m_Prefs = settings;
		SetSamplesPerFrame(settings.m_SamplesPerFrame);
		SetSampleLimit(settings.m_SampleLimit);
		SetCaptureSamples(settings.m_CaptureSamples);
		SetAORays(settings.m_AORays);
		SetBounces(settings.m_Bounces);
		SetMaxRadiance(settings.m_MaxRadiance);
		SetDenoise(settings.m_Denoise);
	}

	void g3dTraceRendererOptiX::SetDenoise(int i_Mode)
	{
		const bool known = i_Mode >= 0 && i_Mode < g3dPrefs::g3dRenderPrefs::e_TraceDenoiseCount;
		m_Denoise = known ? i_Mode : g3dPrefs::g3dRenderPrefs::e_TraceDenoiseOff;
	}

	bool g3dTraceRendererOptiX::wants_denoise(const Key& i_Key, bool i_bCapturing) const
	{
		if (!hdr(i_Key.m_DebugView) || m_Denoiser.HasFailed())
		{
			return false;
		}
		switch (i_Key.m_Denoise)
		{
			case g3dPrefs::g3dRenderPrefs::e_TraceDenoiseViewport:
				return !i_bCapturing;
			case g3dPrefs::g3dRenderPrefs::e_TraceDenoiseCapture:
				return i_bCapturing;
			case g3dPrefs::g3dRenderPrefs::e_TraceDenoiseBoth:
				return true;
			default:
				return false;
		}
	}

	std::vector<float> g3dTraceRendererOptiX::ReadDenoiseInput() const
	{
		if (!m_bFrameDenoised || !m_DenoiseInput.Get())
		{
			return std::vector<float>();
		}
		std::vector<float> values(optxDenoiser::InputBytes(m_Key.m_RegionWidth, m_Key.m_RegionHeight) / sizeof(float));
		if (!m_DenoiseInput.Download(values.data(), values.size() * sizeof(float)))
		{
			return std::vector<float>();
		}
		return values;
	}

	void g3dTraceRendererOptiX::SetSamplesPerFrame(unsigned i_nSamples)
	{
		m_nSamplesPerFrame = std::max(1u, i_nSamples);
	}

	void g3dTraceRendererOptiX::SetSampleLimit(unsigned i_nSamples)
	{
		m_nSampleLimit = std::max(1u, i_nSamples);
	}

	void g3dTraceRendererOptiX::SetCaptureSamples(unsigned i_nSamples)
	{
		m_nCaptureSamples = i_nSamples;
	}

	unsigned g3dTraceRendererOptiX::GetCaptureSamples() const
	{
		return m_nCaptureSamples ? m_nCaptureSamples : m_nSampleLimit;
	}

	void g3dTraceRendererOptiX::SetAORays(unsigned i_nRays)
	{
		m_nAORays = i_nRays;
	}

	void g3dTraceRendererOptiX::SetBounces(unsigned i_nBounces)
	{
		m_nBounces = i_nBounces;
	}

	void g3dTraceRendererOptiX::SetAlbedo(float i_Albedo)
	{
		m_Albedo = std::min(1.0f, std::max(0.0f, i_Albedo));
		m_bAlbedoOverride = true;
	}

	void g3dTraceRendererOptiX::SetLightSampling(optxLightSampling i_Sampling)
	{
		m_Sampling = i_Sampling;
	}

	void g3dTraceRendererOptiX::SetSeed(unsigned i_Seed)
	{
		m_Seed = i_Seed;
	}

	//	The debug view the e_Beauty mode shows (0: none)
	unsigned g3dTraceRendererOptiX::debug_view() const
	{
		if (m_Mode != e_Beauty)
		{
			return 0;
		}
		const int view = m_DebugView >= 0 ? m_DebugView : g3dPrefs::CurrentPrefs().m_TraceDebugView;
		return (view > 0 && view < g3dPrefs::g3dRenderPrefs::e_TraceDebugCount) ? unsigned(view) : 0u;
	}

	bool g3dTraceRendererOptiX::create_programs(bool i_bLighting)
	{
		if (i_bLighting ? m_Lighting.IsCreated() : m_AO.IsCreated())
		{
			return true;
		}
		if (m_bProgramsFailed)
		{
			return false;
		}
		std::string error;
		if (!(i_bLighting ? m_Lighting.Create(error) : m_AO.Create(error)))
		{
			DBG_LOG("GraphicsOptiX: " << error);
			m_bProgramsFailed = true;
			return false;
		}
		return true;
	}

	int g3dTraceRendererOptiX::raster_only(g2dRenderTarget* i_pTarget, const camCamera& i_Camera, const g3dScene& i_Scene,
										   const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime)
	{
		m_Overlay.SetClearTarget(true);
		const int triangles = m_Overlay.Render(i_pTarget, i_Camera, i_Scene, i_ViewerLayers, i_fSimTime);
		m_Overlay.SetClearTarget(false);
		return triangles;
	}

	//	Step 1 of Render: the material, light and environment tables (and
	//	the texture table they fill) for the BVH's materials and light sets
	void g3dTraceRendererOptiX::update_tables(const g3dScene& i_Scene, bool i_bEnvironment)
	{
		G3D_PROFILE_SCOPE("MaterialTables");
		m_Textures.Begin();
		m_Materials.Update(m_BVH.GetMaterials(), m_Textures);
		if (lighting())
		{
			m_Lights.Update(m_BVH.GetLightSets(), m_Textures);
		}
		if (i_bEnvironment)
		{
			m_Environment.Update(i_Scene, m_BVH, m_Textures);
		}
		m_Textures.End();
	}

	unsigned g3dTraceRendererOptiX::Log2Samples(unsigned i_nGoal)
	{
		return std::min(ceil_log2(i_nGoal), c_MaxLog2Samples);
	}

	//	The samples per pixel to reach and what they are weighted by: in a
	//	view, the sample limit and a box of width 1; capturing, the capture
	//	samples times the capture's n x n supersampling, weighted by its
	//	filter (with no supersampling, a box of width 1, as the raster
	//	capture doesn't filter then)
	g3dTraceRendererOptiX::Sampling g3dTraceRendererOptiX::sampling_for(bool i_bCapturing) const
	{
		Sampling sampling;
		sampling.m_nGoal = m_nSampleLimit;
		if (i_bCapturing)
		{
			const g3dProgressive::CaptureSampling capture = g3dProgressive::GetCaptureSampling();
			const unsigned n = unsigned(std::max(1, capture.m_nSupersampling));
			sampling.m_nGoal = GetCaptureSamples() * n * n;
			if (n > 1 && capture.m_Filter >= 0 && capture.m_Filter < g3dProgressive::e_FilterCount && capture.m_FilterWidth > 0.0f)
			{
				static_assert(int(g3dProgressive::e_FilterCount) == int(e_optxFilterCount),
							  "optxPixelFilterKind mirrors g3dProgressive::FilterKind");
				sampling.m_Filter.m_Kind = unsigned(capture.m_Filter);
				sampling.m_Filter.m_Width = capture.m_FilterWidth;
			}
		}
		sampling.m_nGoal = std::max(1u, sampling.m_nGoal);
		sampling.m_Log2Samples = Log2Samples(sampling.m_nGoal);
		return sampling;
	}

	bool g3dTraceRendererOptiX::GetImageWindow(const camCamera& i_Camera, unsigned i_Width, unsigned i_Height,
											   ImageWindow& o_Window, double o_WholeViewProj[16])
	{
		o_Window = ImageWindow();
		o_Window.m_Width = o_Window.m_ImageWidth = i_Width;
		o_Window.m_Height = o_Window.m_ImageHeight = i_Height;
		double view[16];
		float top, bottom, left, right;
		i_Camera.GetSubViewport(top, bottom, left, right);
		if (top == -1.0f && bottom == 1.0f && left == -1.0f && right == 1.0f)
		{
			view_projection(i_Camera, o_WholeViewProj, view);
			return true;
		}
		//	The tile's corners through its own inverse view-projection and the
		//	whole view's view-projection (the same camera without the
		//	sub-viewport) give where it lies in the whole image
		camCamera whole(i_Camera);
		whole.SetSubViewport(-1.0f, 1.0f, -1.0f, 1.0f);
		view_projection(whole, o_WholeViewProj, view);
		double tile[16], inverse[16];
		view_projection(i_Camera, tile, view);
		if (!invert(tile, inverse))
		{
			return false;
		}
		double corners[2][2];
		const double ndc[2][2] = { { -1.0, 1.0 }, { 1.0, -1.0 } };		// the tile's top left and bottom right
		for (int i = 0; i < 2; ++i)
		{
			double p[3], q[3];
			unproject(inverse, ndc[i][0], ndc[i][1], 0.5, p);
			unproject(o_WholeViewProj, p[0], p[1], p[2], q);
			corners[i][0] = q[0];
			corners[i][1] = q[1];
		}
		const double span_x = std::fabs(corners[1][0] - corners[0][0]) * 0.5;
		const double span_y = std::fabs(corners[1][1] - corners[0][1]) * 0.5;
		if (!(span_x > 1e-6) || !(span_y > 1e-6) || span_x > 1.0 + 1e-6 || span_y > 1.0 + 1e-6)
		{
			return false;
		}
		const double image_width = std::floor(i_Width / span_x + 0.5);
		const double image_height = std::floor(i_Height / span_y + 0.5);
		const double x = std::floor((std::min(corners[0][0], corners[1][0]) + 1.0) * 0.5 * image_width + 0.5);
		const double y = std::floor((1.0 - std::max(corners[0][1], corners[1][1])) * 0.5 * image_height + 0.5);
		if (x < 0.0 || y < 0.0 || x + i_Width > image_width || y + i_Height > image_height || image_width > 65536.0 ||
			image_height > 65536.0)
		{
			return false;
		}
		o_Window.m_X = unsigned(x);
		o_Window.m_Y = unsigned(y);
		o_Window.m_ImageWidth = unsigned(image_width);
		o_Window.m_ImageHeight = unsigned(image_height);
		return true;
	}

	//	The pixels traced for a target: the target itself, or for a tile of a
	//	bigger image that is denoised, the tile with the denoiser's overlap
	//	around it (clipped to the image), so its edges see their
	//	neighbours; in whole-image pixels
	void g3dTraceRendererOptiX::traced_region(const ImageWindow& i_Window, bool i_bDenoise, Key& io_Key)
	{
		io_Key.m_RegionX = i_Window.m_X;
		io_Key.m_RegionY = i_Window.m_Y;
		io_Key.m_RegionWidth = i_Window.m_Width;
		io_Key.m_RegionHeight = i_Window.m_Height;
		unsigned overlap = 0;
		if (!i_bDenoise || !i_Window.IsTile() || !m_Denoiser.GetOverlap(overlap) || overlap == 0)
		{
			return;
		}
		const unsigned x0 = i_Window.m_X > overlap ? i_Window.m_X - overlap : 0;
		const unsigned y0 = i_Window.m_Y > overlap ? i_Window.m_Y - overlap : 0;
		const unsigned x1 = std::min(i_Window.m_ImageWidth, i_Window.m_X + i_Window.m_Width + overlap);
		const unsigned y1 = std::min(i_Window.m_ImageHeight, i_Window.m_Y + i_Window.m_Height + overlap);
		io_Key.m_RegionX = x0;
		io_Key.m_RegionY = y0;
		io_Key.m_RegionWidth = x1 - x0;
		io_Key.m_RegionHeight = y1 - y0;
	}

	//	What the image depends on, for the mode (see the header's restart
	//	list); o_View: the camera matrix
	g3dTraceRendererOptiX::Key g3dTraceRendererOptiX::make_key(const camCamera& i_Camera, const gvk::vkdImage& i_Color,
															   unsigned i_DebugView, const g3dScene& i_Scene,
															   const Sampling& i_Sampling, const ImageWindow& i_Window,
															   double o_View[16]) const
	{
		const g3dPrefs::g3dRenderPrefs& prefs = g3dPrefs::CurrentPrefs();
		const bool hdr_image = hdr(i_DebugView);
		Key key;
		key.m_SceneVersion = m_BVH.GetSceneVersion();
		view_projection(i_Camera, key.m_ViewProj, o_View);
		key.m_Width = i_Color.Width();
		key.m_Height = i_Color.Height();
		key.m_Mode = m_Mode;
		key.m_Seed = m_Seed;
		key.m_DebugView = i_DebugView;
		key.m_MaterialVersion = m_Materials.GetVersion();
		key.m_TextureVersion = m_Textures.GetVersion();
		key.m_Filter = i_Sampling.m_Filter.m_Kind;
		key.m_FilterWidth = i_Sampling.m_Filter.m_Width;
		key.m_Log2Samples = i_Sampling.m_Log2Samples;
		key.m_Order = m_Order;
		key.m_ImageWidth = i_Window.m_ImageWidth;
		key.m_ImageHeight = i_Window.m_ImageHeight;
		key.m_TargetX = i_Window.m_X;
		key.m_TargetY = i_Window.m_Y;
		if (i_DebugView)
		{
			key.m_Background[0] = key.m_Background[1] = key.m_Background[2] = 0.0f;
		}
		else if (lighting())
		{
			key.m_LightVersion = m_Lights.GetVersion();
			key.m_bShadows = prefs.m_bEnableShadows ? 1 : 0;
			if (m_Mode == e_Illumination || path_traced())
			{
				key.m_Bounces = m_nBounces;
				key.m_bAlbedoOverride = m_bAlbedoOverride ? 1 : 0;
				key.m_Albedo = m_bAlbedoOverride ? m_Albedo : 0.0f;
				key.m_Sampling = m_Sampling;
			}
			if (m_Mode == e_Illumination)
			{
				key.m_bUseCosine = prefs.m_bIlluminationUsesNormals ? 1 : 0;
			}
			if (path_traced())
			{
				key.m_Selection = m_Selection;
				key.m_MaxRadiance = m_MaxRadiance;
				key.m_Denoise = m_Denoise;
				key.m_EnvironmentVersion = hdr_image ? m_Environment.GetVersion() : 0;
				//	what g3dTargetRenderer cleared the target to (Reflection
				//	Only: black, as the raster pass clears its scene)
				const float* background = i_Color.LastClearColor();
				for (int c = 0; c < 3; ++c)
				{
					key.m_Background[c] = m_Mode == e_ReflectionOnly ? 0.0f : srgb_to_linear(background[c]);
				}
				//	The lens, from the pixel's angle at the view's centre
				double inverse[16];
				float width = 0.0f, spread = 0.0f;
				if (invert(key.m_ViewProj, inverse))
				{
					pixel_cone(inverse, key.m_Height, width, spread);
				}
				GetLens(i_Camera, spread, key.m_LensRadius, key.m_Focus);
			}
			else
			{
				//	What the raster passes clear to: black, or white for the mask
				const float background = (m_Mode == e_ShadowMask) ? 1.0f : 0.0f;
				key.m_Background[0] = key.m_Background[1] = key.m_Background[2] = background;
			}
		}
		else
		{
			ssaoParams ao_settings;
			i_Scene.GetSSAOSettings(ao_settings);
			key.m_Radius = ao_settings.m_AORadius;
			key.m_AORays = m_nAORays;
			//	what g3dTargetRenderer cleared the target to
			const float* background = i_Color.LastClearColor();
			key.m_Background[0] = background[0];
			key.m_Background[1] = background[1];
			key.m_Background[2] = background[2];
		}
		return key;
	}

	int g3dTraceRendererOptiX::Render(g2dRenderTarget* i_pTarget, const camCamera& i_Camera, const g3dScene& i_Scene,
									  const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime)
	{
		G3D_PROFILE_SCOPE("TraceRendererOptiX");
		optxContext::MakeCurrent();
		apply_prefs();

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
		const unsigned debug = debug_view();
		const bool hdr_image = hdr(debug);
		optxColorOrder order = e_optxColorRGBA;
		optxDepthLayout layout = e_optxDepthD24;
		//	The beauty pass's own image goes through the post chain, which
		//	draws into any colour target
		const bool color_ok = hdr_image || optxFrameShare::GetColorOrder(color->Format(), order);
		if (!color_ok || color->Samples() != VK_SAMPLE_COUNT_1_BIT ||
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
		const bool lighting_program = lighting() || debug != 0;
		if (!create_programs(lighting_program))
		{
			return raster_only(i_pTarget, i_Camera, i_Scene, i_ViewerLayers, i_fSimTime);
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
		update_tables(i_Scene, hdr_image);

		//	Capturing, the frame is finished before it is shown; else a few
		//	samples, and another frame asked for until the limit
		const bool capturing = g3dProgressive::IsCapturing();
		const Sampling sampling = sampling_for(capturing);
		ImageWindow window;
		Frame frame;
		frame.m_fTime = i_fSimTime;
		if (!GetImageWindow(i_Camera, color->Width(), color->Height(), window, frame.m_WholeViewProj))
		{
			//	a sub-viewport that isn't a tile of a frame: traced as its own image
			window = ImageWindow();
			window.m_Width = window.m_ImageWidth = color->Width();
			window.m_Height = window.m_ImageHeight = color->Height();
		}
		double view[16];
		Key key = make_key(i_Camera, *color, debug, i_Scene, sampling, window, view);
		//	Whether a frame resolved now is denoised, and so the traced region
		const bool denoise = wants_denoise(key, capturing);
		traced_region(window, denoise && lighting_program, key);
		if (m_bRestart || !(key == m_Key))
		{
			m_Key = key;
			m_bRestart = false;
			m_bHaveFrame = false;
			m_nSamples = 0;
			++m_nRestarts;
		}

		//	A finished frame resolved again (no new samples) when it is now
		//	to be denoised, or no longer (a capture starting or ending)
		const unsigned goal = sampling.m_nGoal;
		if (m_nSamples < goal || (m_bHaveFrame && denoise != m_bFrameDenoised))
		{
			//	The denoiser's exposure: measured for each whole image; a tile
			//	takes the first measured tile's of the same frame
			const bool measure = !window.IsTile() || !m_bMeasuredFrame || !(frame == m_MeasuredFrame);
			if (!trace(m_Key, view, order, layout, goal, capturing, denoise, measure))
			{
				m_bHaveFrame = false;
				return raster_only(i_pTarget, i_Camera, i_Scene, i_ViewerLayers, i_fSimTime);
			}
			if (m_bFrameDenoised && measure)
			{
				m_MeasuredFrame = frame;
				m_bMeasuredFrame = true;
			}
			if (!capturing && m_nSamples < goal)
			{
				g3dProgressive::RequestFrame();
			}
		}

		int triangles = m_BVH.GetTriangleCount();
		if (m_bHaveFrame && m_Share.GetColorLayout() == optxFrameShare::e_ColorHalf)
		{
			triangles += post_process(i_pTarget, depth, i_Camera, i_Scene, i_fSimTime);
		}
		else if (m_bHaveFrame)
		{
			G3D_PROFILE_SCOPE("Composite");
			m_Share.CopyTo(color, depth);
		}

		{
			G3D_PROFILE_SCOPE("RasterOverlay");
			triangles += m_Overlay.Render(i_pTarget, i_Camera, i_Scene, i_ViewerLayers, i_fSimTime);
		}
		return triangles;
	}

	//	The launch parameters both programs share (each has its own struct)
	//	for the key's frame: where its pixels lie in the whole image, the
	//	sequence and the filter
	template <class Params>
	void g3dTraceRendererOptiX::fill_common(const Key& i_Key, const double i_Inverse[16], float i_Epsilon, Params& o_Params) const
	{
		o_Params.m_Handle = m_BVH.GetHandle();
		o_Params.m_Instances = m_BVH.GetInstanceTable();
		o_Params.m_Materials = m_Materials.GetMaterials();
		o_Params.m_Textures = m_Textures.GetTextures();
		o_Params.m_Accum = m_Accum.Get();
		for (int i = 0; i < 16; ++i)
		{
			o_Params.m_ViewProj[i] = float(i_Key.m_ViewProj[i]);
			o_Params.m_InvViewProj[i] = float(i_Inverse[i]);
		}
		o_Params.m_Epsilon = i_Epsilon;
		o_Params.m_Seed = i_Key.m_Seed;
		o_Params.m_Sequence.m_Order = i_Key.m_Order;
		o_Params.m_Sequence.m_Log2Resolution = ceil_log2(std::max(i_Key.m_ImageWidth, i_Key.m_ImageHeight));
		o_Params.m_Sequence.m_Log2Samples = i_Key.m_Log2Samples;
		o_Params.m_Sequence.m_Seed = i_Key.m_Seed;
		o_Params.m_Filter.m_Kind = i_Key.m_Filter;
		o_Params.m_Filter.m_Width = i_Key.m_FilterWidth;
	}

	//	Step 2 of Render: more samples toward i_nGoal (one launch, or with
	//	i_bAll launches until it is reached or the capture is aborted; at
	//	the goal already, one launch that adds none), then resolved into
	//	the shared buffers: by the last launch itself, or (an aborted
	//	capture) by a resolve launch. With i_bDenoise (the beauty pass and
	//	Reflection Only), the resolve writes the denoiser's input over the
	//	traced region and the denoiser writes the frame into the shared
	//	colour (measuring the exposure first with i_bMeasure); if it can't,
	//	the image is resolved again undenoised.
	bool g3dTraceRendererOptiX::trace(const Key& i_Key, const double i_View[16], const optxColorOrder i_Order,
									  const optxDepthLayout i_Layout, unsigned i_nGoal, bool i_bAll, bool i_bDenoise,
									  bool i_bMeasure)
	{
		//	The beauty pass and Reflection Only: three planes of accumulation,
		//	an HDR frame; the other modes two planes; over the traced region
		const bool beauty = hdr(i_Key.m_DebugView);
		const size_t accum_bytes = size_t(i_Key.m_RegionWidth) * i_Key.m_RegionHeight * 4 * sizeof(float) * (beauty ? 3 : 2);
		const optxFrameShare::ColorLayout color_layout = beauty ? optxFrameShare::e_ColorHalf : optxFrameShare::e_Color8;
		if (!m_Share.Resize(i_Key.m_Width, i_Key.m_Height, color_layout) || !m_Accum.Reserve(accum_bytes))
		{
			return false;
		}
		//	The denoiser and its input, if this frame is denoised (a failure
		//	to make them shows it undenoised)
		bool denoise = false;
		if (i_bDenoise && beauty)
		{
			denoise = m_Denoiser.Prepare(i_Key.m_RegionWidth, i_Key.m_RegionHeight) &&
					  m_DenoiseInput.Reserve(optxDenoiser::InputBytes(i_Key.m_RegionWidth, i_Key.m_RegionHeight));
		}
		double inverse[16];
		if (!invert(i_Key.m_ViewProj, inverse))
		{
			return false;
		}
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
		const float epsilon = std::max(size * 1e-5f, 1e-5f);
		const bool lighting_program = lighting() || i_Key.m_DebugView != 0;

		//	Each program has its own parameters; only the mode's are used
		optxAOParams ao = {};
		optxLightingParams lit = {};
		if (lighting_program)
		{
			fill_common(i_Key, inverse, epsilon, lit);
			//	The launch covers the traced region; the frame lies inside it
			lit.m_Width = i_Key.m_RegionWidth;
			lit.m_Height = i_Key.m_RegionHeight;
			lit.m_OutX = i_Key.m_TargetX - i_Key.m_RegionX;
			lit.m_OutY = i_Key.m_TargetY - i_Key.m_RegionY;
			lit.m_OutWidth = i_Key.m_Width;
			lit.m_OutHeight = i_Key.m_Height;
			lit.m_ImageX = i_Key.m_RegionX;
			lit.m_ImageY = i_Key.m_RegionY;
			pixel_cone(inverse, i_Key.m_Height, lit.m_ConeWidth, lit.m_ConeSpread);
			lit.m_bAlbedoOverride = i_Key.m_bAlbedoOverride;
			lit.m_DebugView = i_Key.m_DebugView;
			lit.m_Lights = m_Lights.GetLights();
			lit.m_LightSets = m_Lights.GetSets();
			lit.m_LightIndices = m_Lights.GetIndices();
			lit.m_NumLights = m_Lights.GetLightCount();
			for (int c = 0; c < 3; ++c)
			{
				lit.m_Background[c] = i_Key.m_Background[c];
			}
			lit.m_Albedo = i_Key.m_Albedo;
			lit.m_Mode = i_Key.m_DebugView ? e_optxLightingDebug
										   : (m_Mode == e_ShadowMask) ? e_optxLightingShadowMask
										   : (m_Mode == e_Beauty) ? e_optxLightingBeauty
										   : (m_Mode == e_ReflectionOnly) ? e_optxLightingReflection : e_optxLightingIllumination;
			lit.m_LightSelection = i_Key.m_Selection;
			lit.m_MaxRadiance = i_Key.m_MaxRadiance;
			if (i_Key.m_LensRadius > 0.0f)
			{
				set_lens(i_Key, inverse, i_View, lit);
			}
			lit.m_Bounces = i_Key.m_Bounces;
			lit.m_Sampling = i_Key.m_Sampling;
			lit.m_bUseCosine = i_Key.m_bUseCosine;
			lit.m_bShadows = i_Key.m_bShadows;
			lit.m_ColorOrder = i_Order;
			lit.m_DepthLayout = i_Layout;
			lit.m_DenoiseInput = denoise ? m_DenoiseInput.Get() : 0;
		}
		else
		{
			fill_common(i_Key, inverse, epsilon, ao);
			ao.m_Width = i_Key.m_Width;
			ao.m_Height = i_Key.m_Height;
			ao.m_ImageX = i_Key.m_TargetX;
			ao.m_ImageY = i_Key.m_TargetY;
			for (int c = 0; c < 3; ++c)
			{
				ao.m_Background[c] = srgb_to_linear(i_Key.m_Background[c]);
			}
			ao.m_Radius = i_Key.m_Radius;
			ao.m_AORays = i_Key.m_AORays;
			ao.m_ColorOrder = i_Order;
			ao.m_DepthLayout = i_Layout;
		}

		//	The textures' last Vulkan writes (uploads, renders) land first
		if (!m_Cache.WaitForVulkan(m_Textures.GetWaitValue()) || !m_Share.BeginWrite())
		{
			return false;
		}
		if (beauty)
		{
			//	The environment's sampling table, built if its map or colour
			//	changed (queued before the launches that read it); without
			//	one it is sampled cosine-weighted
			m_Environment.Prepare(m_Lighting, m_Textures);
			lit.m_Environment = m_Environment.GetLight();
		}
		ao.m_Color = lit.m_Color = m_Share.Color();	// the pair BeginWrite picked
		ao.m_Depth = lit.m_Depth = m_Share.Depth();
		bool resolved = false;
		do
		{
			G3D_PROFILE_SCOPE("TraceLaunch");
			const unsigned first = m_nSamples;
			const unsigned samples = std::min(m_nSamplesPerFrame, i_nGoal - m_nSamples);
			const unsigned resolve = (!i_bAll || m_nSamples + samples >= i_nGoal) ? 1 : 0;
			bool launched;
			if (lighting_program)
			{
				lit.m_FirstSample = first;
				lit.m_Samples = samples;
				lit.m_Resolve = resolve;
				launched = m_Lighting.Accumulate(lit, m_BVH.GetSbtRecordCount());
			}
			else
			{
				ao.m_FirstSample = first;
				ao.m_Samples = samples;
				ao.m_Resolve = resolve;
				launched = m_AO.Accumulate(ao, m_BVH.GetSbtRecordCount());
			}
			if (!launched)
			{
				return false;
			}
			m_nSamples += samples;
			++m_nLaunches;
			resolved = resolve != 0;
		}
		while (i_bAll && m_nSamples < i_nGoal && !g3dThreadControl::ShouldRenderThreadAbort());
		if (!resolved)
		{
			G3D_PROFILE_SCOPE("TraceResolve");
			bool done;
			if (lighting_program)
			{
				lit.m_FirstSample = m_nSamples;
				lit.m_Samples = 0;
				lit.m_Resolve = 0;
				done = m_Lighting.Resolve(lit);
			}
			else
			{
				ao.m_FirstSample = m_nSamples;
				ao.m_Samples = 0;
				ao.m_Resolve = 0;
				done = m_AO.Resolve(ao);
			}
			if (!done)
			{
				return false;
			}
		}
		if (denoise)
		{
			G3D_PROFILE_SCOPE("Denoise");
			optxDenoiser::Window frame;
			frame.m_X = lit.m_OutX;
			frame.m_Y = lit.m_OutY;
			frame.m_Width = i_Key.m_Width;
			frame.m_Height = i_Key.m_Height;
			if (!m_Denoiser.Run(m_DenoiseInput.Get(), m_Share.Color(), frame, i_bMeasure))
			{
				//	Reported by the denoiser: the image resolved undenoised
				lit.m_DenoiseInput = 0;
				lit.m_FirstSample = m_nSamples;
				lit.m_Samples = 0;
				lit.m_Resolve = 0;
				if (!m_Lighting.Resolve(lit))
				{
					return false;
				}
				denoise = false;
			}
		}
		if (!m_Share.EndWrite())
		{
			return false;
		}
		m_bHaveFrame = true;
		m_bFrameDenoised = denoise;
		return true;
	}

	//	The thin lens's launch parameters: centred on the eye, across the
	//	camera's x and y axes (the camera matrix's first two columns),
	//	focused along the view direction (the centre pixel's ray)
	void g3dTraceRendererOptiX::set_lens(const Key& i_Key, const double i_Inverse[16], const double i_View[16],
										 optxLightingParams& o_Params)
	{
		double eye[3], forward[3], origin[3];
		camera_ray(i_Inverse, 0.0, 0.0, origin, forward);
		for (int c = 0; c < 3; ++c)
		{
			eye[c] = -(i_View[12] * i_View[c * 4] + i_View[13] * i_View[c * 4 + 1] + i_View[14] * i_View[c * 4 + 2]);
		}
		double right[3] = { i_View[0], i_View[4], i_View[8] };
		double up[3] = { i_View[1], i_View[5], i_View[9] };
		for (double* axis : { right, up })
		{
			const double length = std::sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
			for (int c = 0; c < 3; ++c)
			{
				axis[c] /= length > 0.0 ? length : 1.0;
			}
		}
		for (int c = 0; c < 3; ++c)
		{
			o_Params.m_Eye[c] = float(eye[c]);
			o_Params.m_Forward[c] = float(forward[c]);
			o_Params.m_LensRight[c] = float(right[c]);
			o_Params.m_LensUp[c] = float(up[c]);
		}
		o_Params.m_LensRadius = i_Key.m_LensRadius;
		o_Params.m_FocusDistance = i_Key.m_Focus;
	}

	//	Step 3 of Render for the beauty pass and Reflection Only: the HDR frame into the post
	//	chain's scene texture and the traced depth into the target's, then
	//	the post chain (without its depth of field) into the target. Returns
	//	the triangles the post chain drew.
	int g3dTraceRendererOptiX::post_process(g2dRenderTarget* i_pTarget, gvk::vkdImage* i_pDepth, const camCamera& i_Camera,
											const g3dScene& i_Scene, float i_fSimTime)
	{
		m_FrameBuffer.CreateSurfaces(i_pTarget, 0);
		gvk::matRenderTargetTexture* scene = m_FrameBuffer.HDRRenderTargetTex();
		if (!scene || !scene->GetImage())
		{
			return 0;
		}
		{
			G3D_PROFILE_SCOPE("Composite");
			m_Share.CopyTo(scene->GetImage(), i_pDepth);
		}
		gvk::shdwPostProcessVk::Options options;
		options.m_bAATarget = false;
		options.m_bOverlayLayers = false;	// the raster overlay draws every layer's untraced nodes
		options.m_bDof = false;				// the lens focused the traced image
		const int triangles = m_Post.Run(i_pTarget, m_FrameBuffer, i_Camera, i_Scene, i_fSimTime, options);
		i_pTarget->MakeCurrent();
		return triangles;
	}
}
