/*****************************************************************************
**  g3dSamplingOptiXTest.cpp
**
**      Part 3e of the OptiX plan (docs/plans/2026-11-21-optix-capture-
**	sampling.md), with the whole GraphicsOptiX layer up (gtBackend::
**	RunOptiX), through the beauty pass's linear HDR frame:
**	- capture: under a capture's n x n supersampling the tracer takes the
**	  capture samples times n x n in one frame (one restart, one denoise);
**	  with a box of width 1 that is the view's sampling, sample for sample;
**	- the filter: a wide Gaussian blurs an emissive edge across its width,
**	  Catmull-Rom's negative lobe makes it overshoot;
**	- tiles: a frame traced as four tiles through the camera's
**	  sub-viewport (as quadrant capture does) equals the frame traced
**	  whole, undenoised; denoised, the tiles' seams differ from the whole
**	  frame no more than their insides do, and the denoiser measures the
**	  exposure once for the frame;
**	- the sequence: at few samples the blue-noise ordered sequence's error
**	  has less low-frequency energy than per-pixel scrambling's, and
**	  Owen-scrambled Sobol converges faster than the hash generator at an
**	  edge.
**
**	With MSP_GT_AO_IMAGES set to a folder, HDR frames are saved there.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"
#ifndef GOX_G3DTRACERENDEREROPTIX_HPP
#include "GraphicsOptiX/G3d/g3dTraceRendererOptiX.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef EFF_PBRDATA_HPP
#include "Graphics/Eff/effPbrData.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_PROGRESSIVE_HPP
#include "Graphics/G3d/g3dProgressive.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/G3d/g3dProjectedLight.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif
#ifndef G3D_TARGETRENDERER_HPP
#include "Graphics/G3d/g3dTargetRenderer.hpp"
#endif

#include <gtest/gtest.h>
#include <stb_image_write.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace gox;

namespace
{
	typedef g3dPrefs::g3dRenderPrefs Prefs;
	const int c_Size = 64;
	const float c_Clear[4] = { 0, 0, 0, 0 };

	void save_hdr_if_asked(const std::vector<float>& i_Hdr, int i_Width, int i_Height, const char* i_pName)
	{
		const char* dir = std::getenv("MSP_GT_AO_IMAGES");
		if (!dir || i_Hdr.size() != size_t(i_Width) * i_Height * 4)
		{
			return;
		}
		std::vector<uint8_t> rgba(i_Hdr.size());
		for (size_t i = 0; i < i_Hdr.size(); ++i)
		{
			const float v = (i % 4 == 3) ? 1.0f : std::max(0.0f, i_Hdr[i]) / (1.0f + std::max(0.0f, i_Hdr[i]));
			rgba[i] = uint8_t(std::min(1.0f, v) * 255.0f + 0.5f);
		}
		std::filesystem::create_directories(std::filesystem::u8path(dir));
		const std::string file = std::string(dir) + "/" + ::testing::UnitTest::GetInstance()->current_test_info()->name() +
								 "_" + i_pName + ".png";
		stbi_write_png(file.c_str(), i_Width, i_Height, 4, rgba.data(), i_Width * 4);
	}

	maMatrix4x4 scale_translate(float s, float tx, float ty, float tz)
	{
		return maMatrix4x4(s, 0, 0, 0,  0, s, 0, 0,  0, 0, s, 0,  tx, ty, tz, 1);
	}

	void look_down(camCamera& io_Camera, float i_Height)
	{
		io_Camera.SetFOV(60.0f);
		io_Camera.SetAspect(1.0f);
		io_Camera.SetClip(1.0f, 5000.0f);
		io_Camera.LookAt(maPoint3d(0, 0, i_Height), maPoint3d(0, 0, 0), maVector3d(0, 1, 0));
	}

	matMaterial* pbr(gtSceneBuilder& io_Builder, const maFloatRGBA& i_Base, float i_Metallic, float i_Roughness)
	{
		matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
		gtScene::SetColor(material, effPbrData::c_BaseColor, i_Base);
		gtScene::SetFloat(material, effPbrData::c_Metallic, i_Metallic);
		gtScene::SetFloat(material, effPbrData::c_Roughness, i_Roughness);
		return material;
	}

	matMaterial* glowing(gtSceneBuilder& io_Builder, float i_Strength)
	{
		matMaterial* material = pbr(io_Builder, maFloatRGBA(0, 0, 0, 1), 0.0f, 1.0f);
		gtScene::SetColor(material, effPbrData::c_Emissive, maFloatRGBA(1, 1, 1, 1));
		gtScene::SetFloat(material, effPbrData::c_EmissiveStrength, i_Strength);
		return material;
	}

	std::unique_ptr<g3dTraceRendererOptiX> beauty_renderer()
	{
		g3dSceneRenderer* renderer = g3dSceneRendererCreate::CreateDefaultRenderer();
		g3dTraceRendererOptiX* traced = dynamic_cast<g3dTraceRendererOptiX*>(renderer);
		EXPECT_NE(nullptr, traced) << "the type isn't the OptiX renderer";
		if (!traced)
		{
			delete renderer;
			return nullptr;
		}
		EXPECT_TRUE(traced->SamplesCaptures());
		return std::unique_ptr<g3dTraceRendererOptiX>(traced);
	}

	//	A view of the builder's scene through a renderer, into a target of
	//	its own size; Hdr renders and reads the traced HDR frame
	struct View
	{
		gtTarget			m_Target;
		g3dTargetRenderer	m_View;
		gtSceneBuilder&		m_Builder;
		g3dTraceRendererOptiX&	m_Renderer;
		int					m_Width;
		int					m_Height;

		View(gtSceneBuilder& io_Builder, g3dTraceRendererOptiX& i_Renderer, int i_Width = c_Size, int i_Height = c_Size)
			: m_Target(gtBackend::e_Vulkan, i_Width, i_Height, c_Clear)
			, m_View(m_Target.Target(), &i_Renderer, &io_Builder.Scene(), &io_Builder.Camera())
			, m_Builder(io_Builder)
			, m_Renderer(i_Renderer)
			, m_Width(i_Width)
			, m_Height(i_Height)
		{
		}

		std::vector<float> Hdr()
		{
			m_View.Render(1.0f, m_Builder.ViewerLayers());
			std::vector<float> hdr = m_Renderer.GetFrameShare().ReadColor();
			EXPECT_EQ(size_t(m_Width) * m_Height * 4, hdr.size()) << "no HDR frame";
			return hdr;
		}

		//	Renders until the renderer has i_nSamples
		std::vector<float> HdrAt(unsigned i_nSamples)
		{
			std::vector<float> hdr;
			do
			{
				hdr = Hdr();
			}
			while (m_Renderer.GetSampleCount() < i_nSamples && !hdr.empty());
			return hdr;
		}
	};

	double luminance(const float* i_Rgb)
	{
		return 0.2126 * i_Rgb[0] + 0.7152 * i_Rgb[1] + 0.0722 * i_Rgb[2];
	}

	std::vector<double> luminances(const std::vector<float>& i_Hdr)
	{
		std::vector<double> result(i_Hdr.size() / 4);
		for (size_t i = 0; i < result.size(); ++i)
		{
			result[i] = luminance(&i_Hdr[i * 4]);
		}
		return result;
	}

	//	The mean of a column's luminance over the middle rows
	double column(const std::vector<float>& i_Hdr, int x)
	{
		double sum = 0;
		for (int y = c_Size / 4; y < 3 * c_Size / 4; ++y)
		{
			sum += luminance(&i_Hdr[(size_t(y) * c_Size + x) * 4]);
		}
		return sum / (c_Size / 2);
	}

	g3dProgressive::CaptureSampling capture_sampling(int n, g3dProgressive::FilterKind i_Filter, float i_Width)
	{
		g3dProgressive::CaptureSampling sampling;
		sampling.m_nSupersampling = n;
		sampling.m_Filter = i_Filter;
		sampling.m_FilterWidth = i_Width;
		return sampling;
	}

	//	Soft shadows of a raised square under a disk light, and bounce light
	void soft_shadow_scene(gtSceneBuilder& io_Builder)
	{
		g3dSceneNode* floor = io_Builder.Add(gtScene::Plane(), pbr(io_Builder, maFloatRGBA(0.7f, 0.6f, 0.5f, 1), 0.0f, 0.6f),
											 scale_translate(1000.0f, 0, 0, 0));
		floor->GetFragment()->SetCastsShadow(true);
		floor->GetFragment()->SetReceivesShadow(true);
		g3dSceneNode* square = io_Builder.Add(gtScene::Plane(), pbr(io_Builder, maFloatRGBA(0.3f, 0.5f, 0.7f, 1), 0.0f, 0.4f),
											  scale_translate(4, 2, 1, 4));
		square->GetFragment()->SetCastsShadow(true);
		g3dProjectedLight* light = io_Builder.ProjectedLight(maPoint3d(0, 0, 14), maPoint3d(0, 0, 0),
															 maFloatRGBA(0.9f, 0.9f, 0.9f, 1), 0);
		light->SetRange(100.0f);
		light->SetAngle(120.0f);
		light->SetInnerAngle(120.0f);
		light->SetLightSize(8.0f);
		look_down(io_Builder.Camera(), 30);
	}

	//	A glowing plane over half the view (x < 0), nothing else: a sharp edge
	void edge_scene(gtSceneBuilder& io_Builder)
	{
		io_Builder.Add(gtScene::Plane(), glowing(io_Builder, 1.0f), scale_translate(50.0f, -50.0f, 0, 0));
		look_down(io_Builder.Camera(), 30);
	}

	//	The same half plane turned 20 degrees about the view's axis, so its
	//	edge crosses pixels at every fraction (an antialiasing problem)
	void slanted_edge_scene(gtSceneBuilder& io_Builder)
	{
		const float s = 50.0f, c = std::cos(0.349066f), n = std::sin(0.349066f);
		io_Builder.Add(gtScene::Plane(), glowing(io_Builder, 1.0f),
					   maMatrix4x4(s * c, s * n, 0, 0,  -s * n, s * c, 0, 0,  0, 0, s, 0,  -s * c, -s * n, 0, 1));
		look_down(io_Builder.Camera(), 30);
	}

	//	The quadrant capture's sub-viewport for tile i of 2 x 2
	//	(cptrRenderUtil::ConfigureViewport)
	void set_quadrant(camCamera& io_Camera, int i)
	{
		const float top = -1.0f + (i / 2), left = -1.0f + (i % 2);
		io_Camera.SetSubViewport(top, top + 1.0f, left, left + 1.0f);
	}

	//	A frame traced as four 32 x 32 tiles, put together at the places
	//	GetImageWindow gives (checked to be the four quadrants)
	std::vector<float> tiled(gtSceneBuilder& io_Builder, g3dTraceRendererOptiX& io_Renderer, unsigned i_nSamples)
	{
		const int half = c_Size / 2;
		std::vector<float> image(size_t(c_Size) * c_Size * 4, -1.0f);
		std::set<std::pair<unsigned, unsigned>> places;
		View view(io_Builder, io_Renderer, half, half);
		for (int i = 0; i < 4; ++i)
		{
			set_quadrant(io_Builder.Camera(), i);
			g3dTraceRendererOptiX::ImageWindow window;
			double whole[16];
			EXPECT_TRUE(g3dTraceRendererOptiX::GetImageWindow(io_Builder.Camera(), half, half, window, whole));
			EXPECT_EQ(unsigned(c_Size), window.m_ImageWidth);
			EXPECT_EQ(unsigned(c_Size), window.m_ImageHeight);
			EXPECT_TRUE(window.IsTile());
			places.insert(std::make_pair(window.m_X, window.m_Y));
			const std::vector<float> tile = view.HdrAt(i_nSamples);
			if (tile.size() != size_t(half) * half * 4 || window.m_X + half > unsigned(c_Size) || window.m_Y + half > unsigned(c_Size))
			{
				ADD_FAILURE() << "tile " << i << " wasn't traced";
				continue;
			}
			for (int y = 0; y < half; ++y)
			{
				std::copy(tile.begin() + size_t(y) * half * 4, tile.begin() + size_t(y + 1) * half * 4,
						  image.begin() + ((size_t(window.m_Y) + y) * c_Size + window.m_X) * 4);
			}
		}
		io_Builder.Camera().SetSubViewport(-1, 1, -1, 1);
		EXPECT_EQ(4u, places.size()) << "the tiles aren't the four quadrants";
		return image;
	}

	//	The image's error against a reference, luminance per pixel
	std::vector<double> error(const std::vector<float>& i_Image, const std::vector<float>& i_Reference)
	{
		const std::vector<double> a = luminances(i_Image), b = luminances(i_Reference);
		std::vector<double> e(a.size());
		for (size_t i = 0; i < e.size(); ++i)
		{
			e[i] = a[i] - b[i];
		}
		return e;
	}

	double mean_abs(const std::vector<double>& i_Values)
	{
		double sum = 0;
		for (double v : i_Values)
		{
			sum += std::fabs(v);
		}
		return i_Values.empty() ? 0.0 : sum / i_Values.size();
	}

	//	The share of an error image's energy at low frequencies: the energy
	//	of its 5 x 5 box blur (wrapping) over its own energy. White noise
	//	keeps about 1/25 of it; blue noise much less.
	double low_frequency_share(const std::vector<double>& i_Error)
	{
		double total = 0, low = 0;
		for (int y = 0; y < c_Size; ++y)
		{
			for (int x = 0; x < c_Size; ++x)
			{
				double sum = 0;
				for (int dy = -2; dy <= 2; ++dy)
				{
					for (int dx = -2; dx <= 2; ++dx)
					{
						sum += i_Error[size_t((y + dy + c_Size) % c_Size) * c_Size + (x + dx + c_Size) % c_Size];
					}
				}
				low += (sum / 25) * (sum / 25);
				const double e = i_Error[size_t(y) * c_Size + x];
				total += e * e;
			}
		}
		return total > 0 ? low / total : 0.0;
	}
}

TEST(g3dSamplingOptiX, CaptureTakesTheSupersamplingInOneFrame)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		soft_shadow_scene(builder);
		std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSamplesPerFrame(4);
		renderer->SetSampleLimit(64);
		renderer->SetCaptureSamples(2);
		renderer->SetDenoise(Prefs::e_TraceDenoiseCapture);
		View view(builder, *renderer);
		view.HdrAt(64);
		const int restarts = renderer->GetRestartCount();
		const int denoises = renderer->GetDenoiser().GetRunCount();
		{
			g3dProgressive::CaptureScope capturing(capture_sampling(4, g3dProgressive::e_FilterGaussian, 2.0f));
			EXPECT_EQ(4, g3dProgressive::GetCaptureSampling().m_nSupersampling);
			view.Hdr();
			EXPECT_EQ(32u, renderer->GetSampleCount()) << "2 capture samples x 4 x 4";
			EXPECT_EQ(restarts + 1, renderer->GetRestartCount());
			EXPECT_EQ(denoises + 1, renderer->GetDenoiser().GetRunCount()) << "denoised once, at the end";
			EXPECT_TRUE(renderer->IsFrameDenoised());
		}
		EXPECT_EQ(1, g3dProgressive::GetCaptureSampling().m_nSupersampling) << "the scope puts the sampling back";
		{
			//	no supersampling: the capture samples alone (and no filter)
			g3dProgressive::CaptureScope capturing(capture_sampling(1, g3dProgressive::e_FilterGaussian, 2.0f));
			view.Hdr();
			EXPECT_EQ(2u, renderer->GetSampleCount());
		}
	});
}

TEST(g3dSamplingOptiX, BoxOfWidthOneIsTheViewsSampling)
{
	gtBackend::RunOptiX([]()
	{
		//	16 capture samples x 2 x 2 with a box of width 1 is the view's 64
		//	samples: the converged view is the captured frame, unrestarted
		gtSceneBuilder builder;
		soft_shadow_scene(builder);
		std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSamplesPerFrame(16);
		renderer->SetSampleLimit(64);
		renderer->SetCaptureSamples(16);
		View view(builder, *renderer);
		const std::vector<float> in_view = view.HdrAt(64);
		const int restarts = renderer->GetRestartCount();
		g3dProgressive::CaptureScope capturing(capture_sampling(2, g3dProgressive::e_FilterBox, 1.0f));
		const std::vector<float> captured = view.Hdr();
		EXPECT_EQ(restarts, renderer->GetRestartCount());
		EXPECT_EQ(64u, renderer->GetSampleCount());
		EXPECT_EQ(in_view, captured);
	});
}

TEST(g3dSamplingOptiX, CaptureFilterShapesAnEdge)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		edge_scene(builder);
		auto capture = [&](g3dProgressive::FilterKind i_Filter, float i_Width)
		{
			std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
			EXPECT_TRUE(renderer);
			if (!renderer)
			{
				return std::vector<float>();
			}
			renderer->SetBounces(0);
			renderer->SetSamplesPerFrame(256);
			renderer->SetCaptureSamples(16);
			View view(builder, *renderer);
			g3dProgressive::CaptureScope capturing(capture_sampling(4, i_Filter, i_Width));
			std::vector<float> hdr = view.Hdr();
			EXPECT_EQ(256u, renderer->GetSampleCount());
			return hdr;
		};
		const std::vector<float> box = capture(g3dProgressive::e_FilterBox, 1.0f);
		const std::vector<float> gaussian = capture(g3dProgressive::e_FilterGaussian, 6.0f);
		const std::vector<float> catmull = capture(g3dProgressive::e_FilterCatmullRom, 4.0f);
		ASSERT_FALSE(box.empty() || gaussian.empty() || catmull.empty());
		save_hdr_if_asked(box, c_Size, c_Size, "box");
		save_hdr_if_asked(gaussian, c_Size, c_Size, "gaussian");
		save_hdr_if_asked(catmull, c_Size, c_Size, "catmullrom");

		//	The glowing half (1) and the dark half (0), and the edge between
		//	columns 31 and 32
		const bool bright_left = column(box, 2) > column(box, c_Size - 3);
		auto at = [&](const std::vector<float>& i_Hdr, int i_FromEdge)
		{
			//	the column i_FromEdge pixels into the dark side (> 0) or the
			//	glowing side (< 0): its centre |i_FromEdge| - 0.5 pixels off
			//	the edge
			const int dark_first = bright_left ? c_Size / 2 : c_Size / 2 - 1;
			const int toward_dark = bright_left ? 1 : -1;
			const int x = i_FromEdge > 0 ? dark_first + toward_dark * (i_FromEdge - 1)
										 : dark_first - toward_dark * (-i_FromEdge);
			return column(i_Hdr, x);
		};
		const double bright = column(box, bright_left ? 2 : c_Size - 3);
		EXPECT_NEAR(1.0, bright, 0.02);
		EXPECT_NEAR(bright, at(box, -2), bright * 0.01) << "a box of width 1 keeps the edge sharp";
		EXPECT_NEAR(0.0, at(box, 2), 0.01);
		EXPECT_GT(at(gaussian, 2), 0.05 * bright) << "a 6-pixel Gaussian spreads the edge";
		EXPECT_LT(at(gaussian, -2), 0.95 * bright);
		//	(1.048 at a pixel whose centre is half a pixel inside, computed
		//	from the filter; Mitchell's lobe lies past half its width, where
		//	neither capture samples)
		EXPECT_GT(at(catmull, -1), 1.02 * bright) << "Catmull-Rom's negative lobe overshoots";
	});
}

TEST(g3dSamplingOptiX, TilesEqualTheWholeFrame)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		soft_shadow_scene(builder);
		std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSamplesPerFrame(16);
		renderer->SetSampleLimit(16);
		renderer->SetSeed(5);
		std::vector<float> whole;
		{
			View view(builder, *renderer);
			whole = view.HdrAt(16);
		}
		const std::vector<float> tiles = tiled(builder, *renderer, 16);
		ASSERT_EQ(whole.size(), tiles.size());
		save_hdr_if_asked(whole, c_Size, c_Size, "whole");
		save_hdr_if_asked(tiles, c_Size, c_Size, "tiles");
		//	The same samples, pixel for pixel: only float differences between
		//	the two projections' rays
		int close = 0;
		const std::vector<double> e = error(tiles, whole);
		for (double v : e)
		{
			close += std::fabs(v) < 1e-3 ? 1 : 0;
		}
		EXPECT_GT(close, int(e.size() * 98 / 100)) << "of " << e.size() << " pixels";
		EXPECT_LT(mean_abs(e), 2e-3);
	});
}

TEST(g3dSamplingOptiX, DenoisedTilesHaveNoSeams)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		soft_shadow_scene(builder);
		std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSamplesPerFrame(4);
		renderer->SetSampleLimit(4);
		renderer->SetSeed(5);
		renderer->SetDenoise(Prefs::e_TraceDenoiseBoth);
		//	The tiles first: the first measures the frame's exposure, the
		//	others take it (as the whole frame later does too: same view and
		//	time, but a whole image is always measured)
		const std::vector<float> tiles = tiled(builder, *renderer, 4);
		EXPECT_TRUE(renderer->IsFrameDenoised());
		EXPECT_EQ(1, renderer->GetDenoiser().GetMeasureCount()) << "one exposure for the frame's tiles";
		EXPECT_EQ(4, renderer->GetDenoiser().GetRunCount());
		std::vector<float> whole;
		{
			View view(builder, *renderer);
			whole = view.HdrAt(4);
			EXPECT_TRUE(renderer->IsFrameDenoised());
		}
		EXPECT_EQ(2, renderer->GetDenoiser().GetMeasureCount());
		ASSERT_EQ(whole.size(), tiles.size());
		save_hdr_if_asked(whole, c_Size, c_Size, "whole");
		save_hdr_if_asked(tiles, c_Size, c_Size, "tiles");
		const std::vector<double> e = error(tiles, whole);
		std::vector<double> seam, inside;
		for (int y = 0; y < c_Size; ++y)
		{
			for (int x = 0; x < c_Size; ++x)
			{
				const bool near_seam = std::abs(2 * x + 1 - c_Size) <= 4 || std::abs(2 * y + 1 - c_Size) <= 4;
				(near_seam ? seam : inside).push_back(e[size_t(y) * c_Size + x]);
			}
		}
		EXPECT_LE(mean_abs(seam), 2.0 * mean_abs(inside) + 1e-3)
			<< "seams " << mean_abs(seam) << ", inside " << mean_abs(inside);
	});
}

TEST(g3dSamplingOptiX, BlueNoiseErrorHasLessLowFrequencyEnergy)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		soft_shadow_scene(builder);
		auto render = [&](unsigned i_nSamples, optxSequenceOrder i_Order)
		{
			std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
			EXPECT_TRUE(renderer);
			if (!renderer)
			{
				return std::vector<float>();
			}
			renderer->SetBounces(1);
			renderer->SetSamplesPerFrame(std::min(i_nSamples, 256u));
			renderer->SetSampleLimit(i_nSamples);
			renderer->SetSequenceOrder(i_Order);
			renderer->SetSeed(3);
			View view(builder, *renderer);
			return view.HdrAt(i_nSamples);
		};
		const std::vector<float> converged = render(2048, e_optxOrderBlueNoise);
		ASSERT_FALSE(converged.empty());
		for (unsigned samples : { 1u, 4u })
		{
			const std::vector<float> blue = render(samples, e_optxOrderBlueNoise);
			const std::vector<float> per_pixel = render(samples, e_optxOrderPerPixel);
			ASSERT_FALSE(blue.empty() || per_pixel.empty());
			const std::string name = std::to_string(samples);
			save_hdr_if_asked(blue, c_Size, c_Size, ("blue" + name).c_str());
			save_hdr_if_asked(per_pixel, c_Size, c_Size, ("perpixel" + name).c_str());
			const double blue_low = low_frequency_share(error(blue, converged));
			const double white_low = low_frequency_share(error(per_pixel, converged));
			EXPECT_LT(blue_low, 0.75 * white_low) << samples << " samples: blue " << blue_low << ", per pixel " << white_low;
		}
	});
}

TEST(g3dSamplingOptiX, SobolConvergesFasterThanTheHashAtAnEdge)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		slanted_edge_scene(builder);
		auto render = [&](unsigned i_nSamples, optxSequenceOrder i_Order)
		{
			std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
			EXPECT_TRUE(renderer);
			if (!renderer)
			{
				return std::vector<float>();
			}
			renderer->SetBounces(0);
			renderer->SetSamplesPerFrame(std::min(i_nSamples, 256u));
			renderer->SetSampleLimit(i_nSamples);
			renderer->SetSequenceOrder(i_Order);
			View view(builder, *renderer);
			return view.HdrAt(i_nSamples);
		};
		const std::vector<float> converged = render(4096, e_optxOrderBlueNoise);
		ASSERT_FALSE(converged.empty());
		for (unsigned samples : { 16u, 64u })
		{
			const double sobol = mean_abs(error(render(samples, e_optxOrderBlueNoise), converged));
			const double per_pixel = mean_abs(error(render(samples, e_optxOrderPerPixel), converged));
			const double hash = mean_abs(error(render(samples, e_optxOrderHash), converged));
			EXPECT_LT(sobol, 0.8 * hash) << samples << " samples: Sobol " << sobol << ", hash " << hash;
			EXPECT_LT(per_pixel, 0.8 * hash) << samples << " samples: per pixel " << per_pixel << ", hash " << hash;
		}
	});
}
