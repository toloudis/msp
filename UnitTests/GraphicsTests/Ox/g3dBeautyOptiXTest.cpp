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
**	target; the lens comes from the camera (its real lens with Advanced
**	Lens Properties, else one derived from its depth of field distances)
**	and focuses: a glowing square is sharp in focus and spread, with its
**	light kept, out of focus.
**
**	3d-2, the environment light: a white Lambert floor open to a constant
**	environment of radiance L gets L x albedo, and half of it beside an
**	endless wall; camera misses show the map (the test map's own formula,
**	read as the raster's CartesianToPolar reads it, turned by the angle)
**	with the global environment's background on, the clear colour with it
**	off; the specular map is chosen over the diffuse one; light samples,
**	BRDF samples and both converge to the same image of a map with a sun,
**	and the sun casts a shadow; the GPU-built sampling table matches one
**	built here from the map's formula, and is rebuilt when the colour or
**	the map changes but not the angle; a change of colour, factor, angle,
**	map or background restarts accumulation; without the global
**	environment, the one most fragments have (under a render state) is
**	chosen.
**
**	With MSP_GT_AO_IMAGES set to a folder, the tone-mapped pictures are
**	saved there.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"
#include "gtTempDir.hpp"

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
#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
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

	//	An HDR frame (rgba floats) saved like save_if_asked, linear, scaled so
	//	its brightest channel (at most i_Max) is white
	void save_hdr_if_asked(const std::vector<float>& i_Hdr, int i_Width, int i_Height, const char* i_pName, float i_Max = 1e30f)
	{
		if (!std::getenv("MSP_GT_AO_IMAGES") || i_Hdr.size() != size_t(i_Width) * i_Height * 4)
		{
			return;
		}
		float peak = 1e-6f;
		for (size_t i = 0; i < i_Hdr.size(); ++i)
		{
			if (i % 4 != 3)
			{
				peak = std::max(peak, std::min(i_Hdr[i], i_Max));
			}
		}
		gtPicture picture;
		picture.m_Width = i_Width;
		picture.m_Height = i_Height;
		picture.m_Rgba.resize(size_t(i_Width) * i_Height * 4);
		for (size_t i = 0; i < i_Hdr.size(); ++i)
		{
			picture.m_Rgba[i] = (i % 4 == 3) ? 255 : uint8_t(std::min(1.0f, std::max(0.0f, i_Hdr[i] / peak)) * 255.0f + 0.5f);
		}
		save_if_asked(picture, i_pName);
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
			const double Fv = f0 + (1 - f0) * std::pow(1 - std::min(1.0, NdotV), 5);
			const double Fl = f0 + (1 - f0) * std::pow(1 - NdotL, 5);
			o_Rgb[k] = NdotL * (c[k] * (1 - Fv) * (1 - Fl) * i_Base[k] * (1 - i_Metallic) + c[k] * c_Pi * D * Vis * F);
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

TEST(g3dBeautyOptiX, LensComesFromTheCamera)
{
	gtBackend::RunOptiX([]()
	{
		g3dPrefs::g3dRenderPrefs& prefs = g3dPrefs::CurrentPrefs();
		const bool dof_pref = prefs.m_bEnableDOF;
		prefs.m_bEnableDOF = true;
		camCamera camera;
		const float pixel = 0.001f;
		float radius = 0, focus = 0;

		//	no depth of field on the camera: a pinhole
		EXPECT_FALSE(g3dTraceRendererOptiX::GetLens(camera, pixel, radius, focus));

		camDOFData dof;
		dof.m_bEnableDOF = true;
		dof.m_NearBlurDist = 50;
		dof.m_NearFocalDist = 100;
		dof.m_FarFocalDist = 140;
		dof.m_FarBlurDist = 300;
		dof.m_MaxFarBlur = 0.5f;
		dof.m_MaxCoC = 8;
		camera.SetDOFParams(dof);

		//	Advanced Lens Properties: the real lens (50 mm at f/2.5: a 1 cm radius)
		camera.SetEnableALP(true);
		camera.SetFStop(2.5f);
		camera.SetFocalLength(50.0f);
		camera.SetFocalDistance(250.0f);
		ASSERT_TRUE(g3dTraceRendererOptiX::GetLens(camera, pixel, radius, focus));
		EXPECT_FLOAT_EQ(1.0f, radius);
		EXPECT_FLOAT_EQ(250.0f, focus);

		//	without them: focus mid-range, the far blur distance blurred to
		//	Focus Blur x far limit pixels
		camera.SetEnableALP(false);
		ASSERT_TRUE(g3dTraceRendererOptiX::GetLens(camera, pixel, radius, focus));
		EXPECT_FLOAT_EQ(120.0f, focus);
		const float aperture = 2.0f * radius;
		const float blur_pixels = aperture * std::fabs(300.0f - 120.0f) / 300.0f / (120.0f * pixel);
		EXPECT_NEAR(8.0f * 0.5f, blur_pixels, 1e-3f);

		//	the preference off: a pinhole
		prefs.m_bEnableDOF = false;
		EXPECT_FALSE(g3dTraceRendererOptiX::GetLens(camera, pixel, radius, focus));
		prefs.m_bEnableDOF = dof_pref;
	});
}

TEST(g3dBeautyOptiX, LensFocusesAtItsFocusDistance)
{
	gtBackend::RunOptiX([]()
	{
		g3dPrefs::g3dRenderPrefs& prefs = g3dPrefs::CurrentPrefs();
		const bool dof_pref = prefs.m_bEnableDOF;
		prefs.m_bEnableDOF = true;

		//	a small glowing square 30 below the camera, nothing else
		gtSceneBuilder builder;
		matMaterial* glow = pbr(builder, maFloatRGBA(0, 0, 0, 1), 0.0f, 1.0f);
		gtScene::SetColor(glow, effPbrData::c_Emissive, maFloatRGBA(1, 1, 1, 1));
		builder.Add(gtScene::Plane(), glow, scale_translate(3.0f, 0, 0, 0));
		look_down(builder.Camera(), 0, 0, 30);
		camDOFData dof;
		dof.m_bEnableDOF = true;
		dof.m_MaxFarBlur = 1.0f;
		dof.m_MaxCoC = 5.0f;
		builder.Camera().SetDOFParams(dof);
		builder.Camera().SetEnableALP(true);
		builder.Camera().SetFStop(1.4f);
		builder.Camera().SetFocalLength(50.0f);

		auto render = [&](float i_Focus)
		{
			builder.Camera().SetFocalDistance(i_Focus);
			std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
			renderer->SetBounces(0);
			renderer->SetSamplesPerFrame(128);
			renderer->SetSampleLimit(128);
			View view(builder, *renderer);
			return view.Hdr();
		};
		//	the pixels partly covered by the square: its blurred edge
		auto edge_pixels = [](const std::vector<float>& i_Hdr)
		{
			int count = 0;
			for (int i = 0; i < c_Size * c_Size; ++i)
			{
				const float v = i_Hdr[size_t(i) * 4];
				count += (v > 0.05f && v < 0.95f) ? 1 : 0;
			}
			return count;
		};
		const std::vector<float> sharp = render(30.0f);
		const std::vector<float> blurred = render(8.0f);
		prefs.m_bEnableDOF = dof_pref;
		ASSERT_FALSE(sharp.empty() || blurred.empty());
		//	in focus: only the pixels the square's edge crosses; out of focus
		//	(50 mm at f/1.4, a 3.6 cm aperture, focused at 8: a blur about
		//	20 pixels across at the square) a wide band
		EXPECT_LT(edge_pixels(sharp), 4 * 20 + 8);
		EXPECT_GT(edge_pixels(blurred), 3 * edge_pixels(sharp));
		//	the light is spread, not lost: the image's total stays about the same
		double sum_sharp = 0, sum_blurred = 0;
		for (int i = 0; i < c_Size * c_Size; ++i)
		{
			sum_sharp += sharp[size_t(i) * 4];
			sum_blurred += blurred[size_t(i) * 4];
		}
		EXPECT_NEAR(sum_sharp, sum_blurred, sum_sharp * 0.05);
	});
}

//----------------------------------------------------------------------------
//	3d-2: the environment
//----------------------------------------------------------------------------

namespace
{
	//	gtScene::Plane() turned into the floor y = i_Y (normal +y), i_Half across
	maMatrix4x4 floor_y_matrix(float i_Half, float i_Y = 0.0f)
	{
		return maMatrix4x4(i_Half, 0, 0, 0,  0, 0, -i_Half, 0,  0, i_Half, 0, 0,  0, i_Y, 0, 1);
	}

	//	Looking straight down -y at (x, 0, 0) from i_Height
	void look_down_y(camCamera& io_Camera, float x, float i_Height)
	{
		io_Camera.SetFOV(60.0f);
		io_Camera.SetAspect(1.0f);
		io_Camera.SetClip(1.0f, 5000.0f);
		io_Camera.LookAt(maPoint3d(x, i_Height, 0), maPoint3d(x, 0, 0), maVector3d(0, 0, -1));
	}

	//	A pixel centre's ray: its origin on the near plane and its direction
	void pixel_ray(const camCamera& i_Camera, int x, int y, double o_Origin[3], double o_Direction[3])
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
		double length = 0;
		for (int k = 0; k < 3; ++k)
		{
			o_Origin[k] = ends[0][k];
			o_Direction[k] = ends[1][k] - ends[0][k];
			length += o_Direction[k] * o_Direction[k];
		}
		length = std::sqrt(length);
		for (int k = 0; k < 3; ++k)
		{
			o_Direction[k] /= length;
		}
	}

	//	The floor (y = 0) point (x, z) a pixel's centre sees
	void floor_xz(const camCamera& i_Camera, int x, int y, double& o_X, double& o_Z)
	{
		double o[3], d[3];
		pixel_ray(i_Camera, x, y, o, d);
		const double t = -o[1] / d[1];
		o_X = o[0] + d[0] * t;
		o_Z = o[2] + d[2] * t;
	}

	//	A Lambert.fx material: base colour, no specular (KHR specular 0)
	matMaterial* lambert(gtSceneBuilder& io_Builder, const maFloatRGBA& i_Color)
	{
		matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial("Lambert.fx", gtScene::e_Defaults));
		gtScene::SetColor(material, "g_diffuse", i_Color);
		return material;
	}

	//	The global environment with both maps' colour, factor and angle (degrees)
	void global_environment(gtSceneBuilder& io_Builder, matTexture* i_pDiffuse, matTexture* i_pSpecular, const maFloatRGBA& i_Color,
							float i_Factor, float i_Angle, bool i_bBackground)
	{
		io_Builder.GlobalEnvironment(i_pDiffuse, i_pSpecular, i_bBackground);
		g3dAmbientEnvState state;
		io_Builder.Scene().GetGlobalAmbient(state);
		state.m_DiffuseColor = state.m_SpecularColor = i_Color;
		state.m_DiffuseFactor = state.m_SpecularFactor = i_Factor;
		state.m_DiffuseAngle = state.m_SpecularAngle = i_Angle;
		io_Builder.Scene().SetGlobalAmbient(state);
	}

	double luminance3(const double i_Rgb[3])
	{
		return 0.2126 * i_Rgb[0] + 0.7152 * i_Rgb[1] + 0.0722 * i_Rgb[2];
	}

	//	gtScene::HdrTexture's value at (u, v), and its sun's weight there
	void sky_value(double u, double v, double i_Peak, double o_Rgb[3], double& o_Sun)
	{
		const double t_sky = std::min(v / 0.5, 1.0);
		const double t_ground = std::max((v - 0.5) / 0.5, 0.0);
		const double stripe = 0.85 + 0.15 * std::sin(u * 6.2831853 * 8.0);
		const double sky[3] = { 0.15 + 0.6 * t_sky, 0.3 + 0.55 * t_sky, 0.9 + 0.3 * t_sky };
		const double ground[3] = { (0.35 - 0.25 * t_ground) * stripe, (0.25 - 0.18 * t_ground) * stripe,
								   (0.12 - 0.08 * t_ground) * stripe };
		double blend = std::min(std::max((v - 0.45) / 0.1, 0.0), 1.0);
		blend = blend * blend * (3.0 - 2.0 * blend);
		for (int c = 0; c < 3; ++c)
		{
			o_Rgb[c] = sky[c] + (ground[c] - sky[c]) * blend;
		}
		const double du = u - 0.295, dv = (v - 0.43) * 0.5;
		o_Sun = std::exp(-(du * du + dv * dv) / (0.015 * 0.015));
		o_Rgb[0] += (i_Peak - o_Rgb[0]) * o_Sun;
		o_Rgb[1] += (i_Peak - o_Rgb[1]) * o_Sun;
		o_Rgb[2] += (i_Peak * 0.8 - o_Rgb[2]) * o_Sun;
	}

	//	Support.hlsli's rotateAboutY, then CartesianToPolar
	void map_uv(const double i_D[3], double i_AngleRadians, double& o_U, double& o_V)
	{
		const double s = std::sin(i_AngleRadians), c = std::cos(i_AngleRadians);
		double d[3] = { i_D[2] * s + i_D[0] * c, i_D[1], i_D[2] * c - i_D[0] * s };
		const double length = std::sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
		for (double& x : d)
		{
			x /= length;
		}
		o_U = (std::atan2(d[2], d[0] + 0.001) + c_Pi) / (2 * c_Pi);
		o_V = std::acos(std::min(1.0, std::max(-1.0, d[1]))) / c_Pi;
	}

	double mean_luminance(const std::vector<float>& i_Hdr)
	{
		double sum = 0;
		for (int i = 0; i < c_Size * c_Size; ++i)
		{
			sum += luminance(&i_Hdr[size_t(i) * 4]);
		}
		return sum / (c_Size * c_Size);
	}
}

TEST(g3dBeautyOptiX, ConstantEnvironmentGivesRadianceTimesAlbedo)
{
	gtBackend::RunOptiX([]()
	{
		const maFloatRGBA base(0.6f, 0.5f, 0.4f, 1);
		const float color[3] = { 0.5f, 0.4f, 0.3f }, factor = 2.0f;
		auto render = [&](bool i_bWall, std::vector<double>& o_FloorX)
		{
			gtSceneBuilder builder;
			builder.Add(gtScene::Plane(), lambert(builder, base), floor_y_matrix(1000));
			if (i_bWall)
			{
				builder.Add(gtScene::Plane(), lambert(builder, base), wall_x_matrix(1000));
			}
			global_environment(builder, NULL, NULL, maFloatRGBA(color[0], color[1], color[2], 1), factor, 0, false);
			look_down_y(builder.Camera(), 10, 40);
			std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
			renderer->SetBounces(0);
			renderer->SetSamplesPerFrame(64);
			renderer->SetSampleLimit(64);
			View view(builder, *renderer);
			std::vector<float> hdr = view.Hdr();
			save_if_asked(view.Render(), i_bWall ? "wall" : "open");
			EXPECT_EQ(nullptr, renderer->GetEnvironment().GetMap());
			EXPECT_EQ(0, renderer->GetEnvironment().GetBuildCount()) << "a constant environment needs no table";
			o_FloorX.assign(size_t(c_Size) * c_Size, 0.0);
			for (int y = 0; y < c_Size; ++y)
			{
				for (int x = 0; x < c_Size; ++x)
				{
					double z;
					floor_xz(builder.Camera(), x, y, o_FloorX[size_t(y) * c_Size + x], z);
				}
			}
			return hdr;
		};

		//	open to the sky: every sample gives L x albedo exactly (cosine
		//	samples of a constant light on a Lambert surface)
		std::vector<double> xs;
		const std::vector<float> open = render(false, xs);
		ASSERT_FALSE(open.empty());
		const float* centre = hdr_at(open, c_Size / 2, c_Size / 2);
		for (int c = 0; c < 3; ++c)
		{
			const float expected = color[c] * factor * base.GetPtr()[c];
			EXPECT_NEAR(expected, centre[c], expected * 1e-3f) << "channel " << c;
		}

		//	beside an endless wall (seen from x > 0): half the sky in
		//	cosine-weighted measure, wherever the point is
		const std::vector<float> walled = render(true, xs);
		ASSERT_FALSE(walled.empty());
		double sum = 0;
		int count = 0;
		for (int i = 0; i < c_Size * c_Size; ++i)
		{
			if (xs[i] > 1.0)
			{
				sum += walled[size_t(i) * 4];
				++count;
			}
		}
		EXPECT_GT(count, 1000);
		const double half = 0.5 * color[0] * factor * base.GetPtr()[0];
		EXPECT_NEAR(half, sum / std::max(count, 1), half * 0.02);
	});
}

TEST(g3dBeautyOptiX, BackgroundShowsTheMapTurnedByItsAngle)
{
	gtBackend::RunOptiX([]()
	{
		gtTempDir dir;
		const float peak = 20.0f;
		//	What a camera miss shows without the environment's background:
		//	what the target was cleared to, which a scene without any
		//	environment shows
		float clear[3] = {};
		{
			gtSceneBuilder builder;
			builder.Camera().LookAt(maPoint3d(0, 0, 50), maPoint3d(0, 0, 0), maVector3d(0, 1, 0));
			std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
			ASSERT_TRUE(renderer);
			renderer->SetSampleLimit(1);
			View view(builder, *renderer);
			const std::vector<float> hdr = view.Hdr();
			ASSERT_FALSE(hdr.empty());
			std::memcpy(clear, hdr_at(hdr, 0, 0), sizeof(clear));
		}
		for (float angle : { 0.0f, 90.0f })
		{
			for (bool background : { true, false })
			{
				gtSceneBuilder builder;
				matTexture* sky = builder.Keep(gtScene::HdrTexture(dir.GetPath(), 512, 256, peak));
				ASSERT_NE(nullptr, sky);
				global_environment(builder, NULL, sky, maFloatRGBA(1, 1, 1, 1), 1.0f, angle, background);
				builder.Camera().SetFOV(60.0f);
				builder.Camera().SetAspect(1.0f);
				builder.Camera().SetClip(1.0f, 5000.0f);
				builder.Camera().LookAt(maPoint3d(0, 0, 50), maPoint3d(0, 0, 0), maVector3d(0, 1, 0));
				std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
				ASSERT_TRUE(renderer);
				renderer->SetSampleLimit(1);	// the pixel centres
				View view(builder, *renderer);
				const std::vector<float> hdr = view.Hdr();
				ASSERT_FALSE(hdr.empty());
				if (background && angle == 0.0f)
				{
					save_if_asked(view.Render(), "background");
				}
				int checked = 0;
				for (int y = 2; y < c_Size; y += 5)
				{
					for (int x = 2; x < c_Size; x += 5)
					{
						const float* got = hdr_at(hdr, x, y);
						if (!background)
						{
							for (int c = 0; c < 3; ++c)
							{
								EXPECT_EQ(clear[c], got[c]) << "the clear colour";
							}
							continue;
						}
						double o[3], d[3], u, v, rgb[3], sun;
						pixel_ray(builder.Camera(), x, y, o, d);
						map_uv(d, angle * c_Pi / 180.0, u, v);
						sky_value(u, v, peak, rgb, sun);
						if (sun > 1e-3)
						{
							continue;	// steep: a filtered read differs from the formula
						}
						for (int c = 0; c < 3; ++c)
						{
							EXPECT_NEAR(rgb[c], got[c], rgb[c] * 0.03 + 0.01) << x << ", " << y << " channel " << c << " angle " << angle;
						}
						++checked;
					}
				}
				if (background)
				{
					EXPECT_GT(checked, 100);
				}
			}
		}
	});
}

TEST(g3dBeautyOptiX, SpecularMapIsChosenOverDiffuse)
{
	gtBackend::RunOptiX([]()
	{
		gtTempDir dir;
		gtSceneBuilder builder;
		matTexture* diffuse = builder.Keep(gtScene::HdrTexture(dir.GetPath(), 64, 32, 2.0f));
		matTexture* specular = builder.Keep(gtScene::HdrTexture(dir.GetPath(), 64, 32, 4.0f));
		builder.Add(gtScene::Plane(), lambert(builder, maFloatRGBA(0.5f, 0.5f, 0.5f, 1)), floor_y_matrix(1000));
		look_down_y(builder.Camera(), 0, 40);
		std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSampleLimit(1);
		View view(builder, *renderer);

		global_environment(builder, diffuse, specular, maFloatRGBA(1, 1, 1, 1), 1.0f, 0, false);
		view.Render();
		EXPECT_EQ(specular, renderer->GetEnvironment().GetMap());
		global_environment(builder, diffuse, NULL, maFloatRGBA(1, 1, 1, 1), 1.0f, 0, false);
		view.Render();
		EXPECT_EQ(diffuse, renderer->GetEnvironment().GetMap());

		//	the environment preference off: no environment light
		g3dPrefs::g3dRenderPrefs& prefs = g3dPrefs::CurrentPrefs();
		const bool before = prefs.m_bEnableEnvironment;
		prefs.m_bEnableEnvironment = false;
		view.Render();
		prefs.m_bEnableEnvironment = before;
		EXPECT_EQ(nullptr, renderer->GetEnvironment().GetMap());
		EXPECT_EQ(0u, renderer->GetEnvironment().GetLight().m_Flags);
	});
}

TEST(g3dBeautyOptiX, EnvironmentSamplingStrategiesAgree)
{
	gtBackend::RunOptiX([]()
	{
		gtTempDir dir;
		gtSceneBuilder builder;
		matTexture* sky = builder.Keep(gtScene::HdrTexture(dir.GetPath(), 512, 256, 50.0f));
		ASSERT_NE(nullptr, sky);
		builder.Add(gtScene::Plane(), pbr(builder, maFloatRGBA(0.6f, 0.6f, 0.6f, 1), 0.0f, 0.4f), floor_y_matrix(1000));
		global_environment(builder, NULL, sky, maFloatRGBA(1, 1, 1, 1), 1.0f, 30.0f, false);
		look_down_y(builder.Camera(), 0, 30);

		auto render = [&](optxLightSampling i_Sampling)
		{
			std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
			renderer->SetBounces(0);
			renderer->SetLightSampling(i_Sampling);
			renderer->SetSamplesPerFrame(256);
			renderer->SetSampleLimit(512);
			View view(builder, *renderer);
			view.Render();
			std::vector<float> hdr = view.Hdr();
			EXPECT_EQ(1, renderer->GetEnvironment().GetBuildCount());
			return hdr;
		};
		const std::vector<float> lights = render(e_optxSampleLights);
		const std::vector<float> brdf = render(e_optxSampleBounces);
		const std::vector<float> both = render(e_optxSampleBoth);
		ASSERT_FALSE(lights.empty() || brdf.empty() || both.empty());
		const double by_lights = mean_luminance(lights), by_brdf = mean_luminance(brdf), by_both = mean_luminance(both);
		EXPECT_GT(by_lights, 0.1);
		EXPECT_NEAR(by_lights, by_both, by_lights * 0.02);
		EXPECT_NEAR(by_lights, by_brdf, by_lights * 0.05);
	});
}

TEST(g3dBeautyOptiX, SunCastsAShadow)
{
	gtBackend::RunOptiX([]()
	{
		gtTempDir dir;
		gtSceneBuilder builder;
		matTexture* sky = builder.Keep(gtScene::HdrTexture(dir.GetPath(), 512, 256, 1000.0f));
		ASSERT_NE(nullptr, sky);
		builder.Add(gtScene::Plane(), lambert(builder, maFloatRGBA(0.7f, 0.7f, 0.7f, 1)), floor_y_matrix(1000));
		//	a square roof, 4 across, 3 above the floor
		builder.Add(gtScene::Plane(), lambert(builder, maFloatRGBA(0.7f, 0.7f, 0.7f, 1)), floor_y_matrix(2.0f, 3.0f));
		global_environment(builder, NULL, sky, maFloatRGBA(1, 1, 1, 1), 1.0f, 0, false);
		look_down_y(builder.Camera(), 0, 30);
		std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetBounces(0);
		renderer->SetSamplesPerFrame(128);
		renderer->SetSampleLimit(128);
		View view(builder, *renderer);
		const std::vector<float> hdr = view.Hdr();
		save_hdr_if_asked(hdr, c_Size, c_Size, "sun_shadow_linear");
		ASSERT_FALSE(hdr.empty());

		//	the sun's direction: the inverse of CartesianToPolar at the map's
		//	sun centre (u, v) = (0.295, 0.43), angle 0
		const double theta = 0.43 * c_Pi, phi = 0.295 * 2 * c_Pi - c_Pi;
		const double sun[3] = { std::sin(theta) * std::cos(phi), std::cos(theta), std::sin(theta) * std::sin(phi) };
		ASSERT_GT(sun[1], 0.1) << "the sun is above the floor";
		//	the roof's umbra on the floor (a little nearer than the sun's centre
		//	casts it: the disc's higher half lights the floor more strongly),
		//	and a lit point as far on the other side
		const double reach = 0.8 * 3.0;
		const double shadow[2] = { -sun[0] / sun[1] * reach, -sun[2] / sun[1] * reach };
		const double lit[2] = { -shadow[0], -shadow[1] };
		auto value_at = [&](const double i_P[2])
		{
			double sum = 0;
			int count = 0;
			for (int y = 0; y < c_Size; ++y)
			{
				for (int x = 0; x < c_Size; ++x)
				{
					double px, pz;
					floor_xz(builder.Camera(), x, y, px, pz);
					if ((px - i_P[0]) * (px - i_P[0]) + (pz - i_P[1]) * (pz - i_P[1]) < 1.5 * 1.5)
					{
						sum += luminance(hdr_at(hdr, x, y));
						++count;
					}
				}
			}
			EXPECT_GT(count, 10);
			return count ? sum / count : 0.0;
		};
		//	The sky gives the floor about 60% of its light, the sun the rest; in
		//	the umbra only the sky remains
		const double in_shadow = value_at(shadow), in_sun = value_at(lit);
		EXPECT_LT(in_shadow, 0.75 * in_sun) << "shadow at " << shadow[0] << ", " << shadow[1];
		EXPECT_GT(in_shadow, 0.4 * in_sun) << "the sky still lights the shadow";
	});
}

TEST(g3dBeautyOptiX, EnvironmentTableIsTheMapsAndRebuiltWhenItChanges)
{
	gtBackend::RunOptiX([]()
	{
		gtTempDir dir;
		const float peak = 30.0f;
		gtSceneBuilder builder;
		matTexture* sky = builder.Keep(gtScene::HdrTexture(dir.GetPath(), 512, 256, peak));
		matTexture* other = builder.Keep(gtScene::HdrTexture(dir.GetPath(), 1024, 512, peak));
		ASSERT_TRUE(sky && other);
		builder.Add(gtScene::Plane(), lambert(builder, maFloatRGBA(0.5f, 0.5f, 0.5f, 1)), floor_y_matrix(1000));
		global_environment(builder, NULL, sky, maFloatRGBA(1, 1, 1, 1), 1.0f, 0, false);
		look_down_y(builder.Camera(), 0, 40);
		std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSampleLimit(64);
		View view(builder, *renderer);
		view.Render();
		const g3dEnvironmentOptiX& environment = renderer->GetEnvironment();
		EXPECT_EQ(1, environment.GetBuildCount());
		const optxEnvironmentLight& light = environment.GetLight();
		const unsigned W = light.m_TableWidth, H = light.m_TableHeight;
		ASSERT_EQ(512u, W);
		ASSERT_EQ(256u, H);
		EXPECT_EQ(1u, light.m_Taps);
		const std::vector<float> table = environment.ReadTable();
		ASSERT_EQ(size_t(optxEnvTableFloats(W, H)), table.size());

		//	the table built here from the map's formula: luminance x sin theta
		//	at each texel's centre, then the CDFs
		std::vector<double> cells(size_t(W) * H);
		std::vector<double> sums(H, 0.0);
		double total = 0;
		for (unsigned j = 0; j < H; ++j)
		{
			for (unsigned i = 0; i < W; ++i)
			{
				double rgb[3], sun;
				sky_value((i + 0.5) / W, (j + 0.5) / H, peak, rgb, sun);
				const double f = luminance3(rgb) * std::sin((j + 0.5) / H * c_Pi);
				cells[size_t(j) * W + i] = f;
				sums[j] += f;
			}
			total += sums[j];
		}
		int checked = 0;
		for (unsigned j = 0; j < H; j += 7)
		{
			for (unsigned i = 0; i < W; i += 11)
			{
				const double f = cells[size_t(j) * W + i];
				EXPECT_NEAR(f, table[size_t(j) * W + i], f * 0.03 + 1e-4) << i << ", " << j;
				++checked;
			}
		}
		EXPECT_GT(checked, 1000);
		EXPECT_NEAR(total, table[optxEnvTotalAt(W, H)], total * 0.01);
		for (unsigned j = 0; j < H; j += 13)
		{
			EXPECT_NEAR(sums[j], table[optxEnvRowSumsAt(W, H) + j], sums[j] * 0.02 + 1e-4) << "row " << j;
			const float* row = &table[optxEnvRowCdfAt(W, H) + size_t(j) * (W + 1)];
			EXPECT_EQ(0.0f, row[0]);
			EXPECT_EQ(1.0f, row[W]);
			double running = 0;
			for (unsigned i = 0; i < W; ++i)
			{
				EXPECT_LE(row[i], row[i + 1]);
				running += cells[size_t(j) * W + i];
				if (i % 37 == 0)
				{
					EXPECT_NEAR(running / sums[j], row[i + 1], 3e-3) << "row " << j << " entry " << i;
				}
			}
		}
		const float* marginal = &table[optxEnvMarginalAt(W, H)];
		EXPECT_EQ(0.0f, marginal[0]);
		EXPECT_EQ(1.0f, marginal[H]);
		double running = 0;
		for (unsigned j = 0; j < H; ++j)
		{
			running += sums[j];
			EXPECT_NEAR(running / total, marginal[j + 1], 3e-3) << "row " << j;
		}

		//	rebuilt for a new colour or map, not for a new angle or nothing
		auto settle = [&]()
		{
			view.Render();
			view.Render();
		};
		settle();
		EXPECT_EQ(1, environment.GetBuildCount());
		global_environment(builder, NULL, sky, maFloatRGBA(1, 1, 1, 1), 1.0f, 45.0f, false);
		settle();
		EXPECT_EQ(1, environment.GetBuildCount()) << "the table is in map coordinates";
		global_environment(builder, NULL, sky, maFloatRGBA(1, 0.5f, 1, 1), 1.0f, 45.0f, false);
		settle();
		EXPECT_EQ(2, environment.GetBuildCount());
		global_environment(builder, NULL, other, maFloatRGBA(1, 0.5f, 1, 1), 1.0f, 45.0f, false);
		settle();
		EXPECT_EQ(3, environment.GetBuildCount());
		//	a 1024 x 512 map: a 512 x 256 table (level 1), 2 x 2 samples per cell at level 0
		EXPECT_EQ(512u, environment.GetLight().m_TableWidth);
		EXPECT_EQ(2u, environment.GetLight().m_Taps);
		EXPECT_EQ(1.0f, environment.GetLight().m_TableLevel);
		EXPECT_EQ(0.0f, environment.GetLight().m_TapLevel);
	});
}

TEST(g3dBeautyOptiX, EnvironmentChangesRestart)
{
	gtBackend::RunOptiX([]()
	{
		gtTempDir dir;
		gtSceneBuilder builder;
		matTexture* sky = builder.Keep(gtScene::HdrTexture(dir.GetPath(), 64, 32, 5.0f));
		matTexture* other = builder.Keep(gtScene::HdrTexture(dir.GetPath(), 64, 32, 6.0f));
		builder.Add(gtScene::Plane(), lambert(builder, maFloatRGBA(0.5f, 0.5f, 0.5f, 1)), floor_y_matrix(1000));
		global_environment(builder, NULL, sky, maFloatRGBA(1, 1, 1, 1), 1.0f, 0, false);
		look_down_y(builder.Camera(), 0, 40);
		std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSampleLimit(64);
		View view(builder, *renderer);
		view.Render();
		view.Render();
		int restarts = renderer->GetRestartCount();
		EXPECT_EQ(2u, renderer->GetSampleCount());

		auto expect_restart = [&](const char* i_pWhat)
		{
			view.Render();
			EXPECT_EQ(restarts + 1, renderer->GetRestartCount()) << i_pWhat;
			restarts = renderer->GetRestartCount();
			view.Render();
			EXPECT_EQ(restarts, renderer->GetRestartCount()) << i_pWhat << ", unchanged";
		};
		global_environment(builder, NULL, sky, maFloatRGBA(1, 0.8f, 1, 1), 1.0f, 0, false);
		expect_restart("colour");
		global_environment(builder, NULL, sky, maFloatRGBA(1, 0.8f, 1, 1), 1.5f, 0, false);
		expect_restart("factor");
		global_environment(builder, NULL, sky, maFloatRGBA(1, 0.8f, 1, 1), 1.5f, 20.0f, false);
		expect_restart("angle");
		global_environment(builder, NULL, other, maFloatRGBA(1, 0.8f, 1, 1), 1.5f, 20.0f, false);
		expect_restart("map");
		global_environment(builder, NULL, other, maFloatRGBA(1, 0.8f, 1, 1), 1.5f, 20.0f, true);
		expect_restart("background");
	});
}

TEST(g3dBeautyOptiX, MostCommonEnvironmentWithoutTheGlobalOne)
{
	gtBackend::RunOptiX([]()
	{
		g3dRenderState state;		// outlive the builder's nodes
		g3dAmbientEnvState first, second;
		first.m_DiffuseFactor = 0.3f;
		second.m_DiffuseFactor = 0.7f;
		gtSceneBuilder builder;
		matMaterial* material = lambert(builder, maFloatRGBA(0.5f, 0.5f, 0.5f, 1));
		g3dSceneNode* a = builder.Add(gtScene::Plane(), material, scale_translate(2.0f, -6, 0, 0));
		g3dSceneNode* b = builder.Add(gtScene::Plane(), material, scale_translate(2.0f, 0, 0, 0));
		g3dSceneNode* c = builder.Add(gtScene::Plane(), material, scale_translate(2.0f, 6, 0, 0));
		a->SetEnvironment(&first);
		b->SetEnvironment(&second);
		c->SetEnvironment(&second);
		look_down(builder.Camera(), 0, 0, 30);
		std::unique_ptr<g3dTraceRendererOptiX> renderer = beauty_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSampleLimit(1);
		View view(builder, *renderer);

		//	outside every render state the traverser's default applies
		view.Render();
		EXPECT_EQ(1u, renderer->GetSceneBVH().GetEnvironmentCount());
		EXPECT_EQ(0.0f, renderer->GetSceneBVH().GetEnvironment().m_DiffuseFactor);
		EXPECT_EQ(0u, renderer->GetEnvironment().GetLight().m_Flags);

		//	under one: two of the three have the second
		a->SetRenderState(&state);
		b->SetRenderState(&state);
		c->SetRenderState(&state);
		view.Render();
		EXPECT_EQ(2u, renderer->GetSceneBVH().GetEnvironmentCount());
		EXPECT_EQ(0.7f, renderer->GetSceneBVH().GetEnvironment().m_DiffuseFactor);
		EXPECT_FLOAT_EQ(0.7f, renderer->GetEnvironment().GetLight().m_Color[0]) << "the diffuse colour (white) x factor, no maps";
		EXPECT_NE(0u, renderer->GetEnvironment().GetLight().m_Flags & e_optxEnvironmentOn);
		a->SetRenderState(NULL);
		b->SetRenderState(NULL);
		c->SetRenderState(NULL);
	});
}
