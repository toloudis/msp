/*****************************************************************************
**  g3dLightingOptiXTest.cpp
**
**      Part 3 of the OptiX plan (docs/plans/2026-11-19-optix-lights-
**	materials-beauty.md), with the whole GraphicsOptiX layer up
**	(gtBackend::RunOptiX), rendering into an RGBA8 render-target texture
**	through g3dTargetRenderer:
**
**	3a: the backend makes no target maps (and Vulkan does); strand hair
**	isn't drawn over the traced image.
**
**	3b: the traced Illumination Only and Shadow Mask passes
**	(g3dTraceRendererOptiX's lighting modes), with bounces at 0 unless a
**	test says otherwise: a point light's falloff and colour are
**	Lighting.hlsli's at the pixel centres (linear values, as the raster
**	pass writes them); a spot projected light's cone edge is where its
**	angle puts it; an occluder casts a shadow, and none when the light
**	isn't a shadow source or the occluder doesn't cast; the shadow mask
**	is black in the shadow and white elsewhere; light linking keeps a
**	light from one object only; accumulation restarts when a light moves
**	or a tracer setting changes (directly or through the preferences);
**	bounces brighten a floor beside a lit wall and never darken a pixel;
**	and a large disk light converges to the same image by light samples,
**	bounce rays and both (multiple importance sampling).
**
**	With MSP_GT_AO_IMAGES set to a folder, the pictures are saved there.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"

#ifndef GOX_G3DTRACERENDEREROPTIX_HPP
#include "GraphicsOptiX/G3d/g3dTraceRendererOptiX.hpp"
#endif
#ifndef GOX_G3DSCENEBVHOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dSceneBVHOptiX.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_POINTLIGHT_HPP
#include "Graphics/G3d/g3dPointLight.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_RENDERSTATE_HPP
#include "Graphics/G3d/g3dRenderState.hpp"
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
#include <cstring>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <vector>

using namespace gox;

namespace
{
	const int c_Size = 64;
	const float c_Clear[4] = { 0, 0, 0, 0 };

	void save_if_asked(const gtPicture& i_Picture, const char* i_pName)
	{
		const char* dir = std::getenv("MSP_GT_AO_IMAGES");
		if (!dir || !i_Picture.Valid())
		{
			return;
		}
		const int scale = 4, w = i_Picture.m_Width * scale, h = i_Picture.m_Height * scale;
		std::vector<uint8_t> big(size_t(w) * h * 4);
		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				std::memcpy(&big[(size_t(y) * w + x) * 4],
							&i_Picture.m_Rgba[(size_t(y / scale) * i_Picture.m_Width + x / scale) * 4], 4);
			}
		}
		std::filesystem::create_directories(std::filesystem::u8path(dir));
		const std::string file = std::string(dir) + "/" + ::testing::UnitTest::GetInstance()->current_test_info()->name() +
								 "_" + i_pName + ".png";
		stbi_write_png(file.c_str(), w, h, 4, big.data(), w * 4);
	}

	//	The renderer the creator gives for Illumination Only or Shadow Mask
	std::unique_ptr<g3dTraceRendererOptiX> lighting_renderer(bool i_bMask = false)
	{
		g3dSceneRenderer* renderer = i_bMask ? g3dSceneRendererCreate::CreateShadowMaskRenderer()
											 : g3dSceneRendererCreate::CreateIlluminationRenderer();
		g3dTraceRendererOptiX* traced = dynamic_cast<g3dTraceRendererOptiX*>(renderer);
		EXPECT_NE(nullptr, traced) << "the type isn't the OptiX renderer";
		if (!traced)
		{
			delete renderer;
		}
		return std::unique_ptr<g3dTraceRendererOptiX>(traced);
	}

	void look_down(camCamera& io_Camera, float x, float y, float i_Height)
	{
		io_Camera.SetFOV(60.0f);
		io_Camera.SetAspect(1.0f);
		io_Camera.SetClip(1.0f, 5000.0f);
		io_Camera.LookAt(maPoint3d(x, y, i_Height), maPoint3d(x, y, 0), maVector3d(0, 1, 0));
	}

	maMatrix4x4 scale_translate(float s, float tx, float ty, float tz)
	{
		return maMatrix4x4(s, 0, 0, 0,  0, s, 0, 0,  0, 0, s, 0,  tx, ty, tz, 1);
	}

	//	gtScene::Plane() turned into the plane x = 0, i_Half across
	maMatrix4x4 wall_x_matrix(float i_Half)
	{
		return maMatrix4x4(0, 0, i_Half, 0,  0, i_Half, 0, 0,  i_Half, 0, 0, 0,  0, 0, 0, 1);
	}

	//	The raster's world view-projection, in double, row vectors
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

	void transform(const double i_M[16], const double i_P[3], double o_H[4])
	{
		for (int c = 0; c < 4; ++c)
		{
			o_H[c] = i_P[0] * i_M[c] + i_P[1] * i_M[4 + c] + i_P[2] * i_M[8 + c] + i_M[12 + c];
		}
	}

	void invert(const double i_M[16], double o_Inverse[16])
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
			for (int k = 0; k < 8; ++k)
			{
				std::swap(a[c][k], a[pivot][k]);
			}
			const double scale = 1.0 / a[c][c];
			for (int k = 0; k < 8; ++k)
			{
				a[c][k] *= scale;
			}
			for (int r = 0; r < 4; ++r)
			{
				if (r != c)
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
	}

	//	Where a world point lands in the target
	bool pixel_of(const camCamera& i_Camera, const maPoint3d& i_Point, int& o_X, int& o_Y)
	{
		double vp[16], clip[4];
		view_projection(i_Camera, vp);
		const double p[3] = { i_Point.GetX(), i_Point.GetY(), i_Point.GetZ() };
		transform(vp, p, clip);
		if (clip[3] <= 0)
		{
			return false;
		}
		o_X = int(std::floor((clip[0] / clip[3] + 1.0) * 0.5 * c_Size));
		o_Y = int(std::floor((1.0 - clip[1] / clip[3]) * 0.5 * c_Size));
		return o_X >= 0 && o_X < c_Size && o_Y >= 0 && o_Y < c_Size;
	}

	//	The floor point (z = 0) a pixel's centre sees
	maPoint3d floor_point(const camCamera& i_Camera, int x, int y)
	{
		double vp[16], inverse[16];
		view_projection(i_Camera, vp);
		invert(vp, inverse);
		const double ndc[2] = { (x + 0.5) / c_Size * 2 - 1, 1 - (y + 0.5) / c_Size * 2 };
		double ends[2][3];
		for (int e = 0; e < 2; ++e)
		{
			const double p[3] = { ndc[0], ndc[1], double(e) };
			double h[4];
			transform(inverse, p, h);
			for (int k = 0; k < 3; ++k)
			{
				ends[e][k] = h[k] / h[3];
			}
		}
		const double t = -ends[0][2] / (ends[1][2] - ends[0][2]);
		return maPoint3d(float(ends[0][0] + t * (ends[1][0] - ends[0][0])), float(ends[0][1] + t * (ends[1][1] - ends[0][1])),
						 0.0f);
	}

	const uint8_t* texel(const gtPicture& i_Picture, int x, int y)
	{
		return &i_Picture.m_Rgba[(size_t(y) * i_Picture.m_Width + x) * 4];
	}

	//	The red byte where a floor point lands (-1 if off screen)
	int red_at(const gtPicture& i_Picture, const camCamera& i_Camera, float x, float y)
	{
		int px = 0, py = 0;
		if (!pixel_of(i_Camera, maPoint3d(x, y, 0), px, py))
		{
			ADD_FAILURE() << "(" << x << ", " << y << ") is off screen";
			return -1;
		}
		return texel(i_Picture, px, py)[0];
	}

	int expected_byte(float i_Value)
	{
		return int(std::min(1.0f, std::max(0.0f, i_Value)) * 255.0f + 0.5f);
	}

	//	A floor (z = 0, 2000 across) that receives shadows
	g3dSceneNode* add_floor(gtSceneBuilder& io_Builder, float i_Half = 1000.0f, float x = 0, float y = 0)
	{
		g3dSceneNode* node = io_Builder.Add(gtScene::Plane(), io_Builder.Keep(gtScene::DataMaterial("default")),
											scale_translate(i_Half, x, y, 0));
		node->GetFragment()->SetCastsShadow(true);
		node->GetFragment()->SetReceivesShadow(true);
		return node;
	}

	//	A square occluder i_Half across at height z, seen only by shadow
	//	rays (a shadow hull), so the camera sees the floor under it
	g3dSceneNode* add_occluder(gtSceneBuilder& io_Builder, float i_Half, float z)
	{
		g3dSceneNode* node = io_Builder.Add(gtScene::Plane(), io_Builder.Keep(gtScene::DataMaterial("default")),
											scale_translate(i_Half, 0, 0, z));
		node->GetFragment()->SetCastsShadow(true);
		node->GetFragment()->SetShadowHull(true);
		return node;
	}

	//	One view of a builder's scene through a renderer
	struct View
	{
		gtTarget			m_Target;
		g3dTargetRenderer	m_View;
		gtSceneBuilder&		m_Builder;

		View(gtSceneBuilder& io_Builder, g3dSceneRenderer* i_pRenderer)
			: m_Target(gtBackend::e_Vulkan, c_Size, c_Size, c_Clear)
			, m_View(m_Target.Target(), i_pRenderer, &io_Builder.Scene(), &io_Builder.Camera())
			, m_Builder(io_Builder)
		{
		}

		gtPicture Render()
		{
			m_View.Render(1.0f, m_Builder.ViewerLayers());
			gtPicture picture = m_Target.Read();
			EXPECT_TRUE(picture.Valid());
			return picture;
		}
	};
}

//----------------------------------------------------------------------------
//	3a
//----------------------------------------------------------------------------

TEST(g3dLightingOptiXTest, NoShadowOrReflectionMapsUnderOptiX)
{
	gtBackend::RunOptiX([]()
	{
		EXPECT_FALSE(g3dSceneRendererCreate::NeedsShadowMaps());
		EXPECT_FALSE(g3dSceneRendererCreate::NeedsOpacityMaps());
		EXPECT_FALSE(g3dSceneRendererCreate::NeedsReflectionMaps());
		EXPECT_TRUE(g3dSceneRendererCreate::UsesTracerSettings());
	});
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		EXPECT_TRUE(g3dSceneRendererCreate::NeedsShadowMaps());
		EXPECT_TRUE(g3dSceneRendererCreate::NeedsOpacityMaps());
		EXPECT_TRUE(g3dSceneRendererCreate::NeedsReflectionMaps());
		EXPECT_FALSE(g3dSceneRendererCreate::UsesTracerSettings());
	});
}

TEST(g3dLightingOptiXTest, StrandHairIsNotDrawn)
{
	gtBackend::RunOptiX([]()
	{
		auto render = [](bool i_bHair)
		{
			gtSceneBuilder builder;
			add_floor(builder);
			builder.PointLight(maPoint3d(0, 0, 20), maFloatRGBA(0.5f, 0.5f, 0.5f, 1));
			look_down(builder.Camera(), 0, 0, 50);
			if (i_bHair)
			{
				g3dSceneNode* hair = builder.Hair(gtHairShape(), gtScene::Place(0, 0, 10, 8));
				EXPECT_FALSE(g3dSceneBVHOptiX::IsRasterOverlay(*hair));
				EXPECT_FALSE(g3dSceneBVHOptiX::IsTraced(*hair));
			}
			std::unique_ptr<g3dTraceRendererOptiX> renderer = lighting_renderer();
			renderer->SetSampleLimit(1);
			View view(builder, renderer.get());
			return view.Render();
		};
		const gtPicture without = render(false);
		const gtPicture with = render(true);
		save_if_asked(with, "with_hair");
		ASSERT_TRUE(without.Valid() && with.Valid());
		EXPECT_EQ(without.m_Rgba, with.m_Rgba) << "strand hair was drawn";
	});
}

//----------------------------------------------------------------------------
//	3b
//----------------------------------------------------------------------------

TEST(g3dLightingOptiXTest, PointLightMatchesTheRasterFormula)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		add_floor(builder);
		const float height = 10.0f;
		const maFloatRGBA color(0.8f, 0.6f, 0.4f, 1);
		g3dPointLight* light = builder.PointLight(maPoint3d(0, 0, height), color);
		light->SetFalloff0(0.2f);
		light->SetFalloff1(0.01f);
		light->SetFalloff2(0.008f);
		light->SetFalloff3(0.0f);
		light->SetFalloffStart(2.0f);
		look_down(builder.Camera(), 0, 0, 50);

		std::unique_ptr<g3dTraceRendererOptiX> renderer = lighting_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetBounces(0);
		renderer->SetSampleLimit(1);	// sample 0 is the pixel centre
		View view(builder, renderer.get());
		const gtPicture picture = view.Render();
		save_if_asked(picture, "point");
		ASSERT_TRUE(picture.Valid());

		int checked = 0;
		for (int y = 4; y < c_Size; y += 9)
		{
			for (int x = 4; x < c_Size; x += 9)
			{
				const maPoint3d p = floor_point(builder.Camera(), x, y);
				const float dx = p.GetX(), dy = p.GetY(), d = std::sqrt(dx * dx + dy * dy + height * height);
				const float past = d - 2.0f;
				const float attenuation = 1.0f / (0.2f + 0.01f * past + 0.008f * past * past);
				const float cosine = height / d;
				const uint8_t* got = texel(picture, x, y);
				EXPECT_NEAR(expected_byte(color.GetRed() * attenuation * cosine), got[0], 1) << x << ", " << y;
				EXPECT_NEAR(expected_byte(color.GetGreen() * attenuation * cosine), got[1], 1) << x << ", " << y;
				EXPECT_NEAR(expected_byte(color.GetBlue() * attenuation * cosine), got[2], 1) << x << ", " << y;
				++checked;
			}
		}
		EXPECT_GT(checked, 40);
	});
}

TEST(g3dLightingOptiXTest, SpotConeEdgeIsWhereItsAngleSays)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		add_floor(builder);
		//	60 degrees: the edge 30 degrees off the axis of the light's
		//	shadow camera, which sits the light's scale (1) behind it: at
		//	21 tan 30 = 12.1 (pixels are 0.9 across here)
		g3dProjectedLight* light = builder.ProjectedLight(maPoint3d(0, 0, 20), maPoint3d(0, 0, 0),
														  maFloatRGBA(0.5f, 0.5f, 0.5f, 1), 0);
		light->SetRange(100.0f);
		look_down(builder.Camera(), 0, 0, 50);

		std::unique_ptr<g3dTraceRendererOptiX> renderer = lighting_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetBounces(0);
		renderer->SetSampleLimit(1);
		View view(builder, renderer.get());
		const gtPicture picture = view.Render();
		save_if_asked(picture, "spot");
		ASSERT_TRUE(picture.Valid());

		for (float angle = 0; angle < 6.28f; angle += 0.785f)
		{
			const float c = std::cos(angle), s = std::sin(angle);
			const float inside = 10.5f, outside = 13.8f;
			const float expected = 0.5f * 20.0f / std::sqrt(400.0f + inside * inside);
			EXPECT_NEAR(expected_byte(expected), red_at(picture, builder.Camera(), inside * c, inside * s), 2) << angle;
			EXPECT_EQ(0, red_at(picture, builder.Camera(), outside * c, outside * s)) << angle;
		}
	});
}

TEST(g3dLightingOptiXTest, OccluderCastsAShadow)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		add_floor(builder);
		g3dSceneNode* occluder = add_occluder(builder, 2.0f, 10.0f);	// its shadow: 4 across the middle
		g3dProjectedLight* light = builder.ProjectedLight(maPoint3d(0, 0, 20), maPoint3d(0, 0, 0),
														  maFloatRGBA(0.6f, 0.6f, 0.6f, 1), 0);
		light->SetRange(100.0f);
		light->SetAngle(90.0f);
		light->SetInnerAngle(90.0f);
		look_down(builder.Camera(), 0, 0, 50);

		std::unique_ptr<g3dTraceRendererOptiX> renderer = lighting_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetBounces(0);
		renderer->SetSampleLimit(1);
		View view(builder, renderer.get());
		const gtPicture shadowed = view.Render();
		save_if_asked(shadowed, "shadow");
		ASSERT_TRUE(shadowed.Valid());
		EXPECT_EQ(0, red_at(shadowed, builder.Camera(), 0, 0));
		EXPECT_EQ(0, red_at(shadowed, builder.Camera(), 2.5f, -2.5f));
		EXPECT_NEAR(expected_byte(0.6f * 20.0f / std::sqrt(464.0f)), red_at(shadowed, builder.Camera(), 8, 0), 2);

		//	not a shadow source: no shadow
		light->SetShadowSource(false);
		const gtPicture unshadowed = view.Render();
		save_if_asked(unshadowed, "not_a_source");
		EXPECT_NEAR(expected_byte(0.6f), red_at(unshadowed, builder.Camera(), 0, 0), 2);
		light->SetShadowSource(true);

		//	the occluder doesn't cast: no shadow
		occluder->GetFragment()->SetCastsShadow(false);
		const gtPicture not_cast = view.Render();
		save_if_asked(not_cast, "not_cast");
		EXPECT_NEAR(expected_byte(0.6f), red_at(not_cast, builder.Camera(), 0, 0), 2);
		occluder->GetFragment()->SetCastsShadow(true);

		//	the shadow mask: black in the shadow, white elsewhere
		std::unique_ptr<g3dTraceRendererOptiX> mask = lighting_renderer(true);
		ASSERT_TRUE(mask);
		mask->SetSampleLimit(1);
		View mask_view(builder, mask.get());
		const gtPicture masked = mask_view.Render();
		save_if_asked(masked, "mask");
		ASSERT_TRUE(masked.Valid());
		EXPECT_EQ(0, red_at(masked, builder.Camera(), 0, 0));
		EXPECT_EQ(255, red_at(masked, builder.Camera(), 8, 0));
		EXPECT_EQ(255, texel(masked, 0, 0)[0]);
	});
}

TEST(g3dLightingOptiXTest, LightLinkingExcludesOneObject)
{
	gtBackend::RunOptiX([]()
	{
		g3dRenderState state;	// outlives the builder's nodes
		gtSceneBuilder builder;
		add_floor(builder, 8.0f, -10.0f, 0.0f);
		g3dSceneNode* right = add_floor(builder, 8.0f, 10.0f, 0.0f);
		builder.PointLight(maPoint3d(0, 0, 20), maFloatRGBA(0.5f, 0.5f, 0.5f, 1));
		g3dPointLight* other = builder.PointLight(maPoint3d(0, 0, 20), maFloatRGBA(0.5f, 0.5f, 0.5f, 1));
		other->SetEnable(false);
		//	the right floor is linked to the other (off) light alone
		state.m_Lights.push_back(other);
		right->SetRenderState(&state);
		look_down(builder.Camera(), 0, 0, 50);

		std::unique_ptr<g3dTraceRendererOptiX> renderer = lighting_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetBounces(0);
		renderer->SetSampleLimit(1);
		View view(builder, renderer.get());
		const gtPicture picture = view.Render();
		save_if_asked(picture, "linking");
		ASSERT_TRUE(picture.Valid());
		EXPECT_GT(red_at(picture, builder.Camera(), -10, 0), 50);
		EXPECT_EQ(0, red_at(picture, builder.Camera(), 10, 0));
		EXPECT_EQ(1u, renderer->GetSceneBVH().GetLightSets().size());

		//	no render state: every light reaches it again
		right->SetRenderState(NULL);
		const gtPicture unlinked = view.Render();
		EXPECT_GT(red_at(unlinked, builder.Camera(), 10, 0), 50);
		EXPECT_EQ(0u, renderer->GetSceneBVH().GetLightSets().size());
	});
}

TEST(g3dLightingOptiXTest, RestartsWhenALightMovesOrASettingChanges)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		add_floor(builder);
		g3dPointLight* light = builder.PointLight(maPoint3d(0, 0, 20), maFloatRGBA(0.5f, 0.5f, 0.5f, 1));
		look_down(builder.Camera(), 0, 0, 50);
		std::unique_ptr<g3dTraceRendererOptiX> renderer = lighting_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSamplesPerFrame(1);
		renderer->SetSampleLimit(2);
		View view(builder, renderer.get());
		view.Render();
		view.Render();
		view.Render();	// converged: nothing launched
		EXPECT_EQ(2, renderer->GetLaunchCount());
		int restarts = renderer->GetRestartCount();
		auto expect_restart = [&](const char* i_pWhat)
		{
			SCOPED_TRACE(i_pWhat);
			view.Render();
			EXPECT_EQ(++restarts, renderer->GetRestartCount());
			EXPECT_EQ(1u, renderer->GetSampleCount());
			view.Render();
			EXPECT_EQ(restarts, renderer->GetRestartCount());
		};

		light->SetPosition(maPoint3d(1, 0, 20));
		expect_restart("light moved");
		light->SetIntensity(maFloatRGBA(0.4f, 0.5f, 0.5f, 1));
		expect_restart("light colour");
		renderer->SetBounces(3);
		expect_restart("bounces");
		renderer->SetAlbedo(0.5f);
		expect_restart("albedo");

		//	the preferences' tracer settings reach the renderer when they change
		g3dPrefs::g3dRenderPrefs& prefs = g3dPrefs::CurrentPrefs();
		const int bounces = prefs.m_TraceBounces;
		prefs.m_TraceBounces = 5;
		expect_restart("bounces preference");
		EXPECT_EQ(5u, renderer->GetBounces());
		prefs.m_TraceBounces = bounces;
		const bool shadows = prefs.m_bEnableShadows;
		prefs.m_bEnableShadows = !shadows;
		expect_restart("shadows preference");
		prefs.m_bEnableShadows = shadows;
	});
}

TEST(g3dLightingOptiXTest, BouncesBrightenAndNeverDarken)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		add_floor(builder);
		builder.Add(gtScene::Plane(), builder.Keep(gtScene::DataMaterial("default")), wall_x_matrix(1000));
		builder.PointLight(maPoint3d(6, 0, 6), maFloatRGBA(0.5f, 0.5f, 0.5f, 1));
		look_down(builder.Camera(), 10, 0, 50);

		auto render = [&](unsigned i_nBounces)
		{
			std::unique_ptr<g3dTraceRendererOptiX> renderer = lighting_renderer();
			renderer->SetBounces(i_nBounces);
			renderer->SetSamplesPerFrame(64);
			renderer->SetSampleLimit(64);
			renderer->SetSeed(11);
			View view(builder, renderer.get());
			return view.Render();
		};
		const gtPicture direct = render(0);
		const gtPicture bounced = render(2);
		const gtPicture more = render(4);
		save_if_asked(direct, "direct");
		save_if_asked(bounced, "bounced");
		ASSERT_TRUE(direct.Valid() && bounced.Valid() && more.Valid());

		//	the same seed gives the same direct light, and bounces only add
		int darker = 0;
		for (size_t i = 0; i < direct.m_Rgba.size(); ++i)
		{
			darker += bounced.m_Rgba[i] < direct.m_Rgba[i];
			darker += more.m_Rgba[i] < bounced.m_Rgba[i];
		}
		EXPECT_EQ(0, darker) << "texels darker with more bounces";

		//	the floor beside the lit wall gets brighter
		double sum_direct = 0, sum_bounced = 0;
		int count = 0;
		for (float x = 1; x <= 4; x += 1)
		{
			for (float y = -8; y <= 8; y += 2)
			{
				sum_direct += red_at(direct, builder.Camera(), x, y);
				sum_bounced += red_at(bounced, builder.Camera(), x, y);
				++count;
			}
		}
		EXPECT_GT(sum_bounced / count, sum_direct / count + 5.0) << "bounces added little light";
	});
}

TEST(g3dLightingOptiXTest, AreaLightSamplingStrategiesAgree)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		add_floor(builder);
		g3dProjectedLight* light = builder.ProjectedLight(maPoint3d(0, 0, 10), maPoint3d(0, 0, 0),
														  maFloatRGBA(0.7f, 0.7f, 0.7f, 1), 0);
		light->SetRange(100.0f);
		light->SetAngle(120.0f);
		light->SetInnerAngle(120.0f);
		light->SetLightSize(6.0f);	// a disk of radius 3
		look_down(builder.Camera(), 0, 0, 30);

		auto render = [&](optxLightSampling i_Sampling)
		{
			std::unique_ptr<g3dTraceRendererOptiX> renderer = lighting_renderer();
			renderer->SetBounces(0);
			renderer->SetLightSampling(i_Sampling);
			renderer->SetSamplesPerFrame(256);
			renderer->SetSampleLimit(256);
			View view(builder, renderer.get());
			return view.Render();
		};
		const gtPicture lights = render(e_optxSampleLights);
		const gtPicture bounces = render(e_optxSampleBounces);
		const gtPicture both = render(e_optxSampleBoth);
		save_if_asked(lights, "lights");
		save_if_asked(bounces, "bounces");
		save_if_asked(both, "both");
		ASSERT_TRUE(lights.Valid() && bounces.Valid() && both.Valid());

		auto mean = [&](const gtPicture& i_Picture)
		{
			double sum = 0;
			int count = 0;
			for (int y = 0; y < c_Size; ++y)
			{
				for (int x = 0; x < c_Size; ++x)
				{
					const maPoint3d p = floor_point(builder.Camera(), x, y);
					if (p.GetX() * p.GetX() + p.GetY() * p.GetY() < 36.0f)
					{
						sum += texel(i_Picture, x, y)[0];
						++count;
					}
				}
			}
			EXPECT_GT(count, 100);
			return count ? sum / count : 0.0;
		};
		const double by_lights = mean(lights), by_bounces = mean(bounces), by_both = mean(both);
		EXPECT_GT(by_lights, 100.0);
		EXPECT_NEAR(by_lights, by_both, by_lights * 0.015);
		EXPECT_NEAR(by_lights, by_bounces, by_lights * 0.03);
	});
}
