/*****************************************************************************
**  g3dBeautyOptiXTest.cpp
**
**      Part 3d of the OptiX plan (docs/plans/2026-11-20-optix-beauty.md),
**	with the whole GraphicsOptiX layer up (gtBackend::RunOptiX): the
**	path-traced beauty pass (g3dTraceRendererOptiX's e_Beauty mode, the
**	default renderer type), its linear HDR frame read back from the frame
**	share (before the post chain's tone mapping):
**
**	3d-1: a Pbr.fx floor under a point light, with bounces at 0, gets
**	Pbr.hlsl's direct light (PbrDirect, computed here) at the pixel
**	centres; a glossy floor under a large disk light converges to the same
**	image by light samples, BRDF samples and both (multiple importance
**	sampling); choosing one light per hit and summing every light converge
**	to the same image, with light linking keeping a light from one object;
**	an emissive plane is seen with its emission (values above 1 survive the
**	RGBA16F frame) and lights a floor through bounces; the radiance limit
**	leaves direct light alone and darkens bounced light, and changing it
**	restarts accumulation; the post chain draws the HDR frame into the
**	target.
**
**	With MSP_GT_AO_IMAGES set to a folder, the tone-mapped pictures are
**	saved there.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
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
#include <memory>
#include <string>
#include <vector>

using namespace gox;

namespace
{
	const int c_Size = 64;
	const float c_Clear[4] = { 0, 0, 0, 0 };
	const double c_Pi = 3.14159265358979323846;

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

	//	The renderer the creator gives for the default type
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
		EXPECT_EQ(g3dTraceRendererOptiX::e_Beauty, traced->GetMode());
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

	//	The floor point (z = i_Z) a pixel's centre sees
	maPoint3d floor_point(const camCamera& i_Camera, int x, int y, double i_Z = 0)
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
		const double t = (i_Z - ends[0][2]) / (ends[1][2] - ends[0][2]);
		return maPoint3d(float(ends[0][0] + t * (ends[1][0] - ends[0][0])), float(ends[0][1] + t * (ends[1][1] - ends[0][1])),
						 float(i_Z));
	}

	//	A Pbr.fx material
	matMaterial* pbr(gtSceneBuilder& io_Builder, const maFloatRGBA& i_Base, float i_Metallic, float i_Roughness)
	{
		matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
		gtScene::SetColor(material, effPbrData::c_BaseColor, i_Base);
		gtScene::SetFloat(material, effPbrData::c_Metallic, i_Metallic);
		gtScene::SetFloat(material, effPbrData::c_Roughness, i_Roughness);
		return material;
	}

	//	A floor (z = 0) that receives shadows
	g3dSceneNode* add_floor(gtSceneBuilder& io_Builder, matMaterial* i_pMaterial, float i_Half = 1000.0f, float x = 0, float y = 0)
	{
		g3dSceneNode* node = io_Builder.Add(gtScene::Plane(), i_pMaterial, scale_translate(i_Half, x, y, 0));
		node->GetFragment()->SetCastsShadow(true);
		node->GetFragment()->SetReceivesShadow(true);
		return node;
	}

	//	One view of a builder's scene through a renderer: the tone-mapped
	//	target, and the traced HDR frame (rgba floats per pixel)
	struct View
	{
		gtTarget			m_Target;
		g3dTargetRenderer	m_View;
		gtSceneBuilder&		m_Builder;
		g3dTraceRendererOptiX&	m_Renderer;

		View(gtSceneBuilder& io_Builder, g3dTraceRendererOptiX& i_Renderer)
			: m_Target(gtBackend::e_Vulkan, c_Size, c_Size, c_Clear)
			, m_View(m_Target.Target(), &i_Renderer, &io_Builder.Scene(), &io_Builder.Camera())
			, m_Builder(io_Builder)
			, m_Renderer(i_Renderer)
		{
		}

		gtPicture Render()
		{
			m_View.Render(1.0f, m_Builder.ViewerLayers());
			gtPicture picture = m_Target.Read();
			EXPECT_TRUE(picture.Valid());
			return picture;
		}

		std::vector<float> Hdr()
		{
			Render();
			std::vector<float> hdr = m_Renderer.GetFrameShare().ReadColor();
			EXPECT_EQ(size_t(c_Size) * c_Size * 4, hdr.size()) << "no HDR frame";
			return hdr;
		}
	};

	const float* hdr_at(const std::vector<float>& i_Hdr, int x, int y)
	{
		return &i_Hdr[(size_t(y) * c_Size + x) * 4];
	}

	double luminance(const float* i_Rgb)
	{
		return 0.2126 * i_Rgb[0] + 0.7152 * i_Rgb[1] + 0.0722 * i_Rgb[2];
	}

	//	The mean luminance over the floor pixels a predicate keeps
	template <class Keep>
	double mean_where(const std::vector<float>& i_Hdr, const camCamera& i_Camera, Keep i_Keep, int* o_pCount = nullptr)
	{
		double sum = 0;
		int count = 0;
		for (int y = 0; y < c_Size; ++y)
		{
			for (int x = 0; x < c_Size; ++x)
			{
				const maPoint3d p = floor_point(i_Camera, x, y);
				if (i_Keep(p.GetX(), p.GetY()))
				{
					sum += luminance(hdr_at(i_Hdr, x, y));
					++count;
				}
			}
		}
		if (o_pCount)
		{
			*o_pCount = count;
		}
		return count ? sum / count : 0.0;
	}

	//	Pbr.hlsl's PbrDirect for a light of colour c (diffuse and specular)
	//	from direction l, seen from v, on normal n
	void pbr_direct(const double i_Base[3], double i_Metallic, double i_Roughness, const double n[3], const double v[3],
					const double l[3], const double c[3], double o_Rgb[3])
	{
		auto dot = [](const double* a, const double* b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; };
		double h[3] = { l[0] + v[0], l[1] + v[1], l[2] + v[2] };
		const double hl = 1.0 / std::sqrt(std::max(dot(h, h), 1e-8));
		for (double& x : h)
		{
			x *= hl;
		}
		const double roughness = std::min(1.0, std::max(0.045, i_Roughness));
		const double a = roughness * roughness, a2 = a * a;
		const double NdotL = std::min(1.0, std::max(0.0, dot(n, l)));
		const double NdotV = std::max(dot(n, v), 1e-4);
		const double NdotH = std::min(1.0, std::max(0.0, dot(n, h)));
		const double VdotH = std::min(1.0, std::max(0.0, dot(v, h)));
		const double d = NdotH * NdotH * (a2 - 1) + 1;
		const double D = a2 / (c_Pi * d * d);
		const double gv = NdotL * std::sqrt(NdotV * NdotV * (1 - a2) + a2);
		const double gl = NdotV * std::sqrt(NdotL * NdotL * (1 - a2) + a2);
		const double Vis = 0.5 / std::max(gv + gl, 1e-5);
		for (int k = 0; k < 3; ++k)
		{
			const double f0 = 0.04 + (i_Base[k] - 0.04) * i_Metallic;
			const double F = f0 + (1 - f0) * std::pow(1 - VdotH, 5);
			o_Rgb[k] = NdotL * (c[k] * (1 - F) * i_Base[k] * (1 - i_Metallic) + c[k] * c_Pi * D * Vis * F);
		}
	}
}

//----------------------------------------------------------------------------
//	3d-1
//----------------------------------------------------------------------------

TEST(g3dBeautyOptiX, DirectLightIsPbrHlsls)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		const double base[3] = { 0.6, 0.5, 0.4 };
		const double metallic = 0.3, roughness = 0.5;
		add_floor(builder, pbr(builder, maFloatRGBA(float(base[0]), float(base[1]), float(base[2]), 1), float(metallic),
							   float(roughness)));
		const maPoint3d at(3, -2, 10);
		const double color[3] = { 0.8, 0.7, 0.6 };
		g3dPointLight* light = builder.PointLight(at, maFloatRGBA(float(color[0]), float(color[1]), float(color[2]), 1));
		light->SetFalloff0(1.0f);
		light->SetFalloff1(0.0f);
		light->SetFalloff2(0.0f);
		light->SetFalloff3(0.0f);
		light->SetFalloffStart(0.0f);
		const double eye[3] = { 0, 0, 50 };
		look_down(builder.Camera(), 0, 0, float(eye[2]));

		std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetBounces(0);
		renderer->SetSampleLimit(1);	// sample 0 is the pixel centre
		View view(builder, *renderer);
		const std::vector<float> hdr = view.Hdr();
		save_if_asked(view.Render(), "direct");
		ASSERT_EQ(size_t(c_Size) * c_Size * 4, hdr.size());

		int checked = 0;
		for (int y = 3; y < c_Size; y += 6)
		{
			for (int x = 3; x < c_Size; x += 6)
			{
				const maPoint3d p = floor_point(builder.Camera(), x, y);
				const double n[3] = { 0, 0, 1 };
				double v[3] = { eye[0] - p.GetX(), eye[1] - p.GetY(), eye[2] - p.GetZ() };
				double l[3] = { at.GetX() - p.GetX(), at.GetY() - p.GetY(), at.GetZ() - p.GetZ() };
				for (double* w : { v, l })
				{
					const double len = std::sqrt(w[0] * w[0] + w[1] * w[1] + w[2] * w[2]);
					w[0] /= len;
					w[1] /= len;
					w[2] /= len;
				}
				double expected[3];
				pbr_direct(base, metallic, roughness, n, v, l, color, expected);
				const float* got = hdr_at(hdr, x, y);
				for (int c = 0; c < 3; ++c)
				{
					EXPECT_NEAR(expected[c], got[c], expected[c] * 0.01 + 2e-3) << x << ", " << y << " channel " << c;
				}
				EXPECT_NEAR(1.0f, got[3], 1e-3) << "coverage";
				++checked;
			}
		}
		EXPECT_GT(checked, 90);
	});
}

TEST(g3dBeautyOptiX, AreaLightSamplingStrategiesAgree)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		add_floor(builder, pbr(builder, maFloatRGBA(0.5f, 0.5f, 0.5f, 1), 0.0f, 0.3f));
		g3dProjectedLight* light = builder.ProjectedLight(maPoint3d(0, 0, 10), maPoint3d(0, 0, 0),
														  maFloatRGBA(0.7f, 0.7f, 0.7f, 1), 0);
		light->SetRange(100.0f);
		light->SetAngle(120.0f);
		light->SetInnerAngle(120.0f);
		light->SetLightSize(6.0f);	// a disk of radius 3
		look_down(builder.Camera(), 0, 0, 30);

		auto render = [&](optxLightSampling i_Sampling)
		{
			std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
			renderer->SetBounces(0);
			renderer->SetLightSampling(i_Sampling);
			renderer->SetSamplesPerFrame(256);
			renderer->SetSampleLimit(256);
			View view(builder, *renderer);
			return view.Hdr();
		};
		const std::vector<float> lights = render(e_optxSampleLights);
		const std::vector<float> brdf = render(e_optxSampleBounces);
		const std::vector<float> both = render(e_optxSampleBoth);
		ASSERT_FALSE(lights.empty() || brdf.empty() || both.empty());

		auto inside = [](float x, float y) { return x * x + y * y < 36.0f; };
		int count = 0;
		const double by_lights = mean_where(lights, builder.Camera(), inside, &count);
		const double by_brdf = mean_where(brdf, builder.Camera(), inside);
		const double by_both = mean_where(both, builder.Camera(), inside);
		EXPECT_GT(count, 100);
		EXPECT_GT(by_lights, 0.1);
		EXPECT_NEAR(by_lights, by_both, by_lights * 0.02);
		EXPECT_NEAR(by_lights, by_brdf, by_lights * 0.04);
	});
}

TEST(g3dBeautyOptiX, OneLightAndEveryLightAgree)
{
	gtBackend::RunOptiX([]()
	{
		g3dRenderState state;	// outlives the builder's nodes
		gtSceneBuilder builder;
		matMaterial* material = pbr(builder, maFloatRGBA(0.7f, 0.6f, 0.5f, 1), 0.0f, 0.6f);
		add_floor(builder, material, 8.0f, -9.0f, 0.0f);
		g3dSceneNode* right = add_floor(builder, material, 8.0f, 9.0f, 0.0f);
		const float positions[8][3] = { { -12, -6, 4 }, { -6, 6, 6 }, { 0, 0, 3 }, { 6, -6, 5 },
										{ 12, 6, 4 }, { -14, 8, 8 }, { 14, -8, 7 }, { 3, 9, 2 } };
		std::vector<g3dPointLight*> lights;
		for (int i = 0; i < 8; ++i)
		{
			g3dPointLight* light = builder.PointLight(maPoint3d(positions[i][0], positions[i][1], positions[i][2]),
													  maFloatRGBA(0.3f + 0.1f * (i % 3), 0.4f, 0.5f - 0.05f * i, 1));
			light->SetFalloff0(0.5f);
			light->SetFalloff1(0.0f);
			light->SetFalloff2(0.02f);
			light->SetFalloff3(0.0f);
			light->SetFalloffStart(0.0f);
			lights.push_back(light);
		}
		//	the right floor is lit by every light but the one over it (the 5th)
		for (int i = 0; i < 8; ++i)
		{
			if (i != 4)
			{
				state.m_Lights.push_back(lights[i]);
			}
		}
		right->SetRenderState(&state);
		look_down(builder.Camera(), 0, 0, 40);

		auto render = [&](optxLightSelection i_Selection)
		{
			std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
			renderer->SetBounces(0);
			renderer->SetLightSelection(i_Selection);
			renderer->SetSamplesPerFrame(256);
			renderer->SetSampleLimit(256);
			View view(builder, *renderer);
			return view.Hdr();
		};
		const std::vector<float> one = render(e_optxSelectOne);
		const std::vector<float> all = render(e_optxSelectAll);
		ASSERT_FALSE(one.empty() || all.empty());
		auto left_floor = [](float x, float y) { return x > -16.0f && x < -2.0f && std::fabs(y) < 7.0f; };
		auto right_floor = [](float x, float y) { return x > 2.0f && x < 16.0f && std::fabs(y) < 7.0f; };
		const double one_left = mean_where(one, builder.Camera(), left_floor);
		const double all_left = mean_where(all, builder.Camera(), left_floor);
		const double one_right = mean_where(one, builder.Camera(), right_floor);
		const double all_right = mean_where(all, builder.Camera(), right_floor);
		EXPECT_GT(all_left, 0.05);
		EXPECT_NEAR(all_left, one_left, all_left * 0.03);
		EXPECT_NEAR(all_right, one_right, all_right * 0.03);

		//	under the excluded light, the right floor is darker than the left
		//	is under its mirror image... and linking holds with one light chosen
		auto under_fifth = [](float x, float y) { return (x - 12.0f) * (x - 12.0f) + (y - 6.0f) * (y - 6.0f) < 2.0f; };
		auto under_first = [](float x, float y) { return (x + 12.0f) * (x + 12.0f) + (y + 6.0f) * (y + 6.0f) < 2.0f; };
		EXPECT_LT(mean_where(one, builder.Camera(), under_fifth), mean_where(one, builder.Camera(), under_first));
	});
}

TEST(g3dBeautyOptiX, EmissionIsSeenAndLightsTheScene)
{
	gtBackend::RunOptiX([]()
	{
		//	seen directly, no lights: the emission itself, above 1
		{
			gtSceneBuilder builder;
			matMaterial* glow = pbr(builder, maFloatRGBA(0.2f, 0.2f, 0.2f, 1), 0.0f, 1.0f);
			gtScene::SetColor(glow, effPbrData::c_Emissive, maFloatRGBA(1.0f, 0.5f, 0.25f, 1));
			gtScene::SetFloat(glow, effPbrData::c_EmissiveStrength, 4.0f);
			add_floor(builder, glow);
			look_down(builder.Camera(), 0, 0, 20);
			std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
			ASSERT_TRUE(renderer);
			renderer->SetBounces(0);
			renderer->SetSampleLimit(1);
			View view(builder, *renderer);
			const std::vector<float> hdr = view.Hdr();
			ASSERT_FALSE(hdr.empty());
			const float* centre = hdr_at(hdr, c_Size / 2, c_Size / 2);
			EXPECT_NEAR(4.0f, centre[0], 4.0f * 2e-3f);
			EXPECT_NEAR(2.0f, centre[1], 2.0f * 2e-3f);
			EXPECT_NEAR(1.0f, centre[2], 1.0f * 2e-3f);
		}

		//	an emissive wall lights the floor beside it only through bounces
		gtSceneBuilder builder;
		add_floor(builder, pbr(builder, maFloatRGBA(0.8f, 0.8f, 0.8f, 1), 0.0f, 1.0f));
		matMaterial* wall = pbr(builder, maFloatRGBA(0.0f, 0.0f, 0.0f, 1), 0.0f, 1.0f);
		gtScene::SetColor(wall, effPbrData::c_Emissive, maFloatRGBA(1, 1, 1, 1));
		gtScene::SetFloat(wall, effPbrData::c_EmissiveStrength, 2.0f);
		builder.Add(gtScene::Plane(), wall, wall_x_matrix(1000));
		look_down(builder.Camera(), 10, 0, 40);
		auto render = [&](unsigned i_nBounces)
		{
			std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
			renderer->SetBounces(i_nBounces);
			renderer->SetSamplesPerFrame(64);
			renderer->SetSampleLimit(64);
			View view(builder, *renderer);
			return view.Hdr();
		};
		const std::vector<float> direct = render(0);
		const std::vector<float> bounced = render(1);
		ASSERT_FALSE(direct.empty() || bounced.empty());
		auto beside = [](float x, float y) { return x > 1.0f && x < 4.0f && std::fabs(y) < 8.0f; };
		int count = 0;
		EXPECT_NEAR(0.0, mean_where(direct, builder.Camera(), beside, &count), 1e-6) << "emissive surfaces aren't light sampled";
		EXPECT_GT(count, 20);
		//	a white floor at the foot of a wall of radiance 2 gets about half
		//	of it (the wall fills half its sky), times its albedo
		const double lit = mean_where(bounced, builder.Camera(), beside);
		EXPECT_GT(lit, 0.3);
		EXPECT_LT(lit, 0.9);
	});
}

TEST(g3dBeautyOptiX, MaxRadianceLimitsBouncedLightOnly)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		add_floor(builder, pbr(builder, maFloatRGBA(0.8f, 0.8f, 0.8f, 1), 0.0f, 0.8f));
		builder.Add(gtScene::Plane(), pbr(builder, maFloatRGBA(0.8f, 0.8f, 0.8f, 1), 0.0f, 0.8f), wall_x_matrix(1000));
		g3dPointLight* light = builder.PointLight(maPoint3d(6, 0, 6), maFloatRGBA(0.8f, 0.8f, 0.8f, 1));
		light->SetFalloff0(1.0f);
		light->SetFalloff1(0.0f);
		light->SetFalloff2(0.0f);
		light->SetFalloff3(0.0f);
		look_down(builder.Camera(), 10, 0, 50);

		auto render = [&](unsigned i_nBounces, float i_Max)
		{
			std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
			renderer->SetBounces(i_nBounces);
			renderer->SetMaxRadiance(i_Max);
			renderer->SetSamplesPerFrame(32);
			renderer->SetSampleLimit(32);
			renderer->SetSeed(5);
			View view(builder, *renderer);
			return view.Hdr();
		};
		//	direct light: the same with or without the limit
		const std::vector<float> direct = render(0, 0.0f);
		const std::vector<float> direct_limited = render(0, 0.001f);
		ASSERT_FALSE(direct.empty() || direct_limited.empty());
		EXPECT_EQ(direct, direct_limited);

		//	bounced light: darker with a low limit
		const std::vector<float> bounced = render(2, 0.0f);
		const std::vector<float> limited = render(2, 0.001f);
		auto floor_region = [](float x, float y) { return x > 1.0f && x < 20.0f && std::fabs(y) < 10.0f; };
		EXPECT_LT(mean_where(limited, builder.Camera(), floor_region) + 0.005, mean_where(bounced, builder.Camera(), floor_region));
		EXPECT_GE(mean_where(limited, builder.Camera(), floor_region) + 1e-6, mean_where(direct, builder.Camera(), floor_region));
	});
}

TEST(g3dBeautyOptiX, RestartsWhenTheLimitChanges)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		add_floor(builder, pbr(builder, maFloatRGBA(0.5f, 0.5f, 0.5f, 1), 0.0f, 0.5f));
		builder.PointLight(maPoint3d(0, 0, 10), maFloatRGBA(0.5f, 0.5f, 0.5f, 1));
		look_down(builder.Camera(), 0, 0, 30);
		std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSampleLimit(4);
		View view(builder, *renderer);
		view.Render();
		view.Render();
		const int restarts = renderer->GetRestartCount();
		EXPECT_EQ(2u, renderer->GetSampleCount());
		renderer->SetMaxRadiance(5.0f);
		view.Render();
		EXPECT_EQ(restarts + 1, renderer->GetRestartCount());

		//	through the preferences
		g3dPrefs::g3dRenderPrefs& prefs = g3dPrefs::CurrentPrefs();
		const float before = prefs.m_TraceMaxRadiance;
		prefs.m_TraceMaxRadiance = 7.0f;
		view.Render();
		prefs.m_TraceMaxRadiance = before;
		EXPECT_EQ(restarts + 2, renderer->GetRestartCount());
		EXPECT_EQ(7.0f, renderer->GetMaxRadiance());
	});
}

TEST(g3dBeautyOptiX, PostChainDrawsTheHdrFrame)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		add_floor(builder, pbr(builder, maFloatRGBA(0.7f, 0.7f, 0.7f, 1), 0.0f, 0.6f));
		builder.PointLight(maPoint3d(0, 0, 10), maFloatRGBA(1, 1, 1, 1));
		look_down(builder.Camera(), 0, 0, 30);
		std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSampleLimit(4);
		renderer->SetSamplesPerFrame(4);
		View view(builder, *renderer);
		const gtPicture picture = view.Render();
		save_if_asked(picture, "tone_mapped");
		ASSERT_TRUE(picture.Valid());
		EXPECT_EQ(optxFrameShare::e_ColorHalf, renderer->GetFrameShare().GetColorLayout());
		//	the lit floor shows, brightest under the light
		const uint8_t* centre = &picture.m_Rgba[(size_t(c_Size / 2) * c_Size + c_Size / 2) * 4];
		const uint8_t* corner = &picture.m_Rgba[(size_t(2) * c_Size + 2) * 4];
		EXPECT_GT(centre[0], 20);
		EXPECT_GT(centre[0], corner[0]);
	});
}
