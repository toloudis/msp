/*****************************************************************************
**  g3dAORendererOptiXTest.cpp
**
**      The OptiX AO renderer (g3dAORendererOptiX), with the whole
**	GraphicsOptiX layer up (gtBackend::RunOptiX), rendering into an RGBA8
**	render-target texture through g3dTargetRenderer, as MachStudio's views
**	do: the creator gives it for the default, HDR and AO types; a quad
**	under an open sky is white on the background; a floor beside a tall
**	wall is half occluded (the analytic value for cosine-weighted rays);
**	the same seed gives the same image; each restart key restarts
**	accumulation, and a converged frame launches nothing; it asks for
**	frames until it converges, and a capture gets the finished frame; the
**	raster draws what isn't traced over the traced image, depth-tested
**	against it; the shared
**	colour and depth buffers reach the target intact; and traced depth
**	agrees with the forward renderer's depth for the same quad.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"

#ifndef GOX_G3DAORENDEREROPTIX_HPP
#include "GraphicsOptiX/G3d/g3dAORendererOptiX.hpp"
#endif
#ifndef GOX_CULOADER_HPP
#include "GraphicsOptiX/cu/cuLoader.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef GVK_G2DDEPTHSTENCILBUFFERVK_HPP
#include "GraphicsVulkan/G2d/g2dDepthStencilBufferVk.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dSceneRendererVk.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_PROGRESSIVE_HPP
#include "Graphics/G3d/g3dProgressive.hpp"
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
#include <string>
#include <memory>
#include <vector>

using namespace gox;

namespace
{
	const int c_Size = 64;
	const float c_Clear[4] = { 0, 0, 0, 0 };

	//	With MSP_GT_AO_IMAGES set to a folder, a test's picture is saved
	//	there as <test>_<i_pName>.png, scaled up 4x, to look at
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
		const std::string file = std::string(dir) + "/" +
								 ::testing::UnitTest::GetInstance()->current_test_info()->name() + "_" + i_pName + ".png";
		stbi_write_png(file.c_str(), w, h, 4, big.data(), w * 4);
	}

	//	The renderer the creator gives for the AO type
	std::unique_ptr<g3dAORendererOptiX> ao_renderer()
	{
		g3dSceneRenderer* renderer = g3dSceneRendererCreate::CreateAmbientOcclusionRenderer();
		g3dAORendererOptiX* ao = dynamic_cast<g3dAORendererOptiX*>(renderer);
		EXPECT_NE(nullptr, ao) << "the AO type isn't the OptiX renderer";
		if (!ao)
		{
			delete renderer;
		}
		return std::unique_ptr<g3dAORendererOptiX>(ao);
	}

	//	The builder's camera i_Height above (x, y, 0), looking down -z
	void look_down(camCamera& io_Camera, float x, float y, float i_Height)
	{
		io_Camera.SetFOV(60.0f);
		io_Camera.SetAspect(1.0f);
		io_Camera.SetClip(1.0f, 5000.0f);
		io_Camera.LookAt(maPoint3d(x, y, i_Height), maPoint3d(x, y, 0), maVector3d(0, 1, 0));
	}

	//	Row-vector matrix: scale, then translate
	maMatrix4x4 scale_translate(float s, float tx, float ty, float tz)
	{
		return maMatrix4x4(s, 0, 0, 0,  0, s, 0, 0,  0, 0, s, 0,  tx, ty, tz, 1);
	}

	//	gtScene::Plane() (the square -1..1 in x and y at z = 0) turned into
	//	the plane x = 0, i_Half across (y from -i_Half to i_Half, z too)
	maMatrix4x4 wall_x_matrix(float i_Half)
	{
		return maMatrix4x4(0, 0, i_Half, 0,  0, i_Half, 0, 0,  i_Half, 0, 0, 0,  0, 0, 0, 1);
	}

	//	... and into the plane y = 0
	maMatrix4x4 wall_y_matrix(float i_Half)
	{
		return maMatrix4x4(i_Half, 0, 0, 0,  0, 0, i_Half, 0,  0, i_Half, 0, 0,  0, 0, 0, 1);
	}

	//	The raster's world view-projection, in double (camera, then the
	//	projection mirrored in x), row vectors, row-major
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

	//	(p, 1) * M for row-major M
	void transform(const double i_M[16], const double i_P[3], double o_H[4])
	{
		for (int c = 0; c < 4; ++c)
		{
			o_H[c] = i_P[0] * i_M[c] + i_P[1] * i_M[4 + c] + i_P[2] * i_M[8 + c] + i_M[12 + c];
		}
	}

	//	Gauss-Jordan with partial pivoting
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

	//	Where a world point lands in a c_Size x c_Size target, through the
	//	raster's view-projection
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

	//	The exact depth (D24) at each pixel centre of a c_Size x c_Size
	//	target where the plane through the origin with normal i_Normal is
	//	seen, in double
	std::vector<int> exact_plane_depth(const camCamera& i_Camera, const double i_Normal[3])
	{
		double vp[16], inverse[16];
		view_projection(i_Camera, vp);
		invert(vp, inverse);
		std::vector<int> depth(c_Size * c_Size);
		for (int i = 0; i < c_Size * c_Size; ++i)
		{
			const double ndc[2] = { ((i % c_Size) + 0.5) / c_Size * 2 - 1, 1 - ((i / c_Size) + 0.5) / c_Size * 2 };
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
			double d[3], np = 0, nd = 0;
			for (int k = 0; k < 3; ++k)
			{
				d[k] = ends[1][k] - ends[0][k];
				np += i_Normal[k] * ends[0][k];
				nd += i_Normal[k] * d[k];
			}
			const double t = -np / nd;
			const double q[3] = { ends[0][0] + t * d[0], ends[0][1] + t * d[1], ends[0][2] + t * d[2] };
			double h[4];
			transform(vp, q, h);
			depth[i] = int(std::lround(h[2] / h[3] * 16777215.0));
		}
		return depth;
	}

	float srgb_to_linear(uint8_t i_Value)
	{
		const float c = i_Value / 255.0f;
		return c <= 0.04045f ? c / 12.92f : std::pow((c + 0.055f) / 1.055f, 2.4f);
	}

	const uint8_t* texel(const gtPicture& i_Picture, int x, int y)
	{
		return &i_Picture.m_Rgba[(size_t(y) * i_Picture.m_Width + x) * 4];
	}

	//	The target's depth image read back (32 bits a texel)
	std::vector<uint32_t> read_depth(const gtTarget& i_Target)
	{
		shared_ptr<g2dDepthStencilBuffer> buffer = i_Target.Target()->GetDepthStencilBuffer();
		gvk::g2dDepthStencilBufferVk* depth = dynamic_cast<gvk::g2dDepthStencilBufferVk*>(buffer.get());
		EXPECT_NE(nullptr, depth);
		if (!depth)
		{
			return std::vector<uint32_t>();
		}
		const std::vector<uint8_t> bytes = gtReadVkImage(depth->GetDepthView());
		std::vector<uint32_t> texels(bytes.size() / 4);
		std::memcpy(texels.data(), bytes.data(), texels.size() * 4);
		return texels;
	}

	//	A floor (z = 0, 2000 across) beside a wall (x = 0, 2000 across), and
	//	an AO radius of 1000: from a floor point a little way from the wall,
	//	the wall hides half the hemisphere (every direction towards it, out
	//	to tiny angles), so cosine-weighted occlusion rays escape half the
	//	time. With i_bCorner, a second wall (y = 0) makes inside corners: a
	//	quarter escapes. The camera looks down at x = y = 20.
	void floor_and_wall(gtSceneBuilder& io_Builder, bool i_bCorner = false)
	{
		//	occlusion rays see only shadow casters (MachStudio sets the flag
		//	per object; fragments start without it)
		matMaterial* material = io_Builder.Keep(gtScene::DataMaterial("default"));
		io_Builder.Add(gtScene::Plane(), material, scale_translate(1000, 0, 0, 0))->GetFragment()->SetCastsShadow(true);
		io_Builder.Add(gtScene::Plane(), material, wall_x_matrix(1000))->GetFragment()->SetCastsShadow(true);
		if (i_bCorner)
		{
			io_Builder.Add(gtScene::Plane(), material, wall_y_matrix(1000))->GetFragment()->SetCastsShadow(true);
		}
		ssaoParams settings;
		io_Builder.Scene().GetSSAOSettings(settings);
		settings.m_AORadius = 1000.0f;
		io_Builder.Scene().SetSSAOSettings(settings);
		look_down(io_Builder.Camera(), 20, 20, 50);
	}

	//	The mean linear AO of the floor points 5 to 40 from both walls (x and
	//	y), as the picture shows them (0.5 or 0.25 within a few thousandths:
	//	the walls' finite height and the radius)
	double floor_ao(const camCamera& i_Camera, const gtPicture& i_Picture)
	{
		double sum = 0;
		int count = 0;
		for (float x = 5; x <= 40; x += 5)
		{
			for (float y = 5; y <= 40; y += 5)
			{
				int px = 0, py = 0;
				if (pixel_of(i_Camera, maPoint3d(x, y, 0), px, py))
				{
					sum += srgb_to_linear(texel(i_Picture, px, py)[0]);
					++count;
				}
			}
		}
		EXPECT_GT(count, 40);
		return count ? sum / count : 0.0;
	}

	//	floor_and_wall's scene, AO with 4 x 64 rays a pixel
	gtPicture render_floor(bool i_bCorner)
	{
		gtSceneBuilder builder;
		floor_and_wall(builder, i_bCorner);
		gtTarget target(gtBackend::e_Vulkan, c_Size, c_Size, c_Clear);
		std::unique_ptr<g3dAORendererOptiX> renderer = ao_renderer();
		if (!renderer)
		{
			return gtPicture();
		}
		renderer->SetAORays(4);
		renderer->SetSamplesPerFrame(64);
		renderer->SetSampleLimit(64);
		g3dTargetRenderer view(target.Target(), renderer.get(), &builder.Scene(), &builder.Camera());
		view.Render(1.0f, builder.ViewerLayers());
		gtPicture picture = target.Read();
		EXPECT_TRUE(picture.Valid());
		save_if_asked(picture, i_bCorner ? "corner" : "wall");
		const double ao = picture.Valid() ? floor_ao(builder.Camera(), picture) : 0.0;
		EXPECT_NEAR(i_bCorner ? 0.25 : 0.5, ao, 0.02);
		return picture;
	}
}

TEST(g3dAORendererOptiXTest, CreatorGivesIt)
{
	gtBackend::RunOptiX([]()
	{
		for (g3dSceneRenderer* renderer : { g3dSceneRendererCreate::CreateDefaultRenderer(),
											g3dSceneRendererCreate::CreateHDRRenderer(),
											g3dSceneRendererCreate::CreateAmbientOcclusionRenderer() })
		{
			EXPECT_NE(nullptr, dynamic_cast<g3dAORendererOptiX*>(renderer));
			delete renderer;
		}
		g3dSceneRenderer* simple = g3dSceneRendererCreate::CreateSimpleRenderer();
		EXPECT_EQ(nullptr, dynamic_cast<g3dAORendererOptiX*>(simple));
		delete simple;
	});
}

TEST(g3dAORendererOptiXTest, OpenSkyIsWhiteOnTheBackground)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		builder.Add(gtScene::Plane(), builder.Keep(gtScene::DataMaterial("default")), scale_translate(10, 0, 0, 0));
		look_down(builder.Camera(), 0, 0, 50);
		gtTarget target(gtBackend::e_Vulkan, c_Size, c_Size, c_Clear);
		std::unique_ptr<g3dAORendererOptiX> renderer = ao_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetAORays(4);
		renderer->SetSamplesPerFrame(4);
		renderer->SetSampleLimit(4);
		g3dTargetRenderer view(target.Target(), renderer.get(), &builder.Scene(), &builder.Camera());
		view.SetBackgroundColor(g2dRGBColor(10, 20, 30));
		view.Render(1.0f, builder.ViewerLayers());
		const gtPicture picture = target.Read();
		ASSERT_TRUE(picture.Valid());
		save_if_asked(picture, "sky");

		const uint8_t* centre = texel(picture, c_Size / 2, c_Size / 2);
		EXPECT_EQ(255, centre[0]);
		EXPECT_EQ(255, centre[1]);
		EXPECT_EQ(255, centre[2]);
		EXPECT_EQ(255, centre[3]);
		const uint8_t* corner = texel(picture, 0, 0);
		EXPECT_NEAR(10, corner[0], 1);
		EXPECT_NEAR(20, corner[1], 1);
		EXPECT_NEAR(30, corner[2], 1);
		EXPECT_EQ(255, corner[3]);
		EXPECT_EQ(4u, renderer->GetSampleCount());
		EXPECT_EQ(2u, renderer->GetSceneBVH().GetTriangleCount());
	});
}

TEST(g3dAORendererOptiXTest, WallHalvesTheOcclusion)
{
	gtBackend::RunOptiX([]() { render_floor(false); });
}

TEST(g3dAORendererOptiXTest, InsideCornerLeavesAQuarter)
{
	gtBackend::RunOptiX([]() { render_floor(true); });
}

TEST(g3dAORendererOptiXTest, SameSeedSameImage)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		floor_and_wall(builder);
		auto render = [&](unsigned i_Seed)
		{
			gtTarget target(gtBackend::e_Vulkan, c_Size, c_Size, c_Clear);
			std::unique_ptr<g3dAORendererOptiX> renderer = ao_renderer();
			renderer->SetAORays(1);
			renderer->SetSamplesPerFrame(2);
			renderer->SetSampleLimit(2);
			renderer->SetSeed(i_Seed);
			g3dTargetRenderer view(target.Target(), renderer.get(), &builder.Scene(), &builder.Camera());
			view.Render(1.0f, builder.ViewerLayers());
			return target.Read();
		};
		const gtPicture first = render(7);
		const gtPicture second = render(7);
		const gtPicture other = render(8);
		save_if_asked(first, "seed7_2spp");
		ASSERT_TRUE(first.Valid());
		EXPECT_EQ(first.m_Rgba, second.m_Rgba);
		EXPECT_NE(first.m_Rgba, other.m_Rgba);
	});
}

TEST(g3dAORendererOptiXTest, RestartKeys)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		floor_and_wall(builder);
		std::unique_ptr<gtTarget> target(new gtTarget(gtBackend::e_Vulkan, c_Size, c_Size, c_Clear));
		std::unique_ptr<g3dAORendererOptiX> renderer = ao_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSamplesPerFrame(1);
		renderer->SetSampleLimit(3);
		g3dTargetRenderer view(target->Target(), renderer.get(), &builder.Scene(), &builder.Camera());
		auto render = [&]() { view.Render(1.0f, builder.ViewerLayers()); };

		render();
		EXPECT_EQ(1, renderer->GetRestartCount());
		EXPECT_EQ(1u, renderer->GetSampleCount());
		render();
		render();
		EXPECT_EQ(3u, renderer->GetSampleCount());
		EXPECT_EQ(3, renderer->GetLaunchCount());
		//	converged: nothing launched, nothing restarted
		render();
		render();
		EXPECT_EQ(3, renderer->GetLaunchCount());
		EXPECT_EQ(1, renderer->GetRestartCount());
		EXPECT_EQ(3u, renderer->GetSampleCount());

		int restarts = renderer->GetRestartCount();
		auto expect_restart = [&](const char* i_pWhat)
		{
			SCOPED_TRACE(i_pWhat);
			render();
			EXPECT_EQ(++restarts, renderer->GetRestartCount());
			EXPECT_EQ(1u, renderer->GetSampleCount());
			render();	// and not again while nothing changes
			EXPECT_EQ(restarts, renderer->GetRestartCount());
			EXPECT_EQ(2u, renderer->GetSampleCount());
		};

		look_down(builder.Camera(), 21, 20, 50);
		expect_restart("camera");

		ssaoParams settings;
		builder.Scene().GetSSAOSettings(settings);
		settings.m_AORadius = 500.0f;
		builder.Scene().SetSSAOSettings(settings);
		expect_restart("AO radius");

		view.SetBackgroundColor(g2dRGBColor(40, 50, 60));
		expect_restart("background");

		renderer->SetAORays(2);
		expect_restart("AO rays");

		renderer->SetSeed(3);
		expect_restart("seed");

		renderer->Restart();
		expect_restart("Restart");

		builder.Add(gtScene::Plane(), builder.Keep(gtScene::DataMaterial("default")), scale_translate(1, 30, 0, 1));
		expect_restart("scene");

		target.reset(new gtTarget(gtBackend::e_Vulkan, c_Size / 2, c_Size, c_Clear));
		view.SetTarget(target->Target());
		expect_restart("size");
	});
}

TEST(g3dAORendererOptiXTest, AsksForFramesUntilConverged)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		floor_and_wall(builder);
		gtTarget target(gtBackend::e_Vulkan, c_Size, c_Size, c_Clear);
		std::unique_ptr<g3dAORendererOptiX> renderer = ao_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSamplesPerFrame(1);
		renderer->SetSampleLimit(3);
		g3dTargetRenderer view(target.Target(), renderer.get(), &builder.Scene(), &builder.Camera());
		for (int frame = 1; frame <= 4; ++frame)
		{
			SCOPED_TRACE(frame);
			g3dProgressive::ClearFrameRequest();	// as the render loop does when it starts a render
			view.Render(1.0f, builder.ViewerLayers());
			EXPECT_EQ(frame < 3, g3dProgressive::IsFrameRequested());
		}
		g3dProgressive::ClearFrameRequest();
	});
}

TEST(g3dAORendererOptiXTest, CaptureFinishesTheFrame)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		floor_and_wall(builder);
		gtTarget target(gtBackend::e_Vulkan, c_Size, c_Size, c_Clear);
		std::unique_ptr<g3dAORendererOptiX> renderer = ao_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSamplesPerFrame(2);
		renderer->SetSampleLimit(4);
		renderer->SetCaptureSamples(6);
		g3dTargetRenderer view(target.Target(), renderer.get(), &builder.Scene(), &builder.Camera());
		g3dProgressive::ClearFrameRequest();
		{
			g3dProgressive::CaptureScope capturing;
			view.Render(1.0f, builder.ViewerLayers());
			EXPECT_EQ(6u, renderer->GetSampleCount());
			EXPECT_EQ(3, renderer->GetLaunchCount());
			EXPECT_FALSE(g3dProgressive::IsFrameRequested());
			EXPECT_EQ(1u, renderer->GetFrameShare().GetFrameValue());	// resolved once
			//	the next frame, with nothing changed, is the same one
			view.Render(1.0f, builder.ViewerLayers());
			EXPECT_EQ(3, renderer->GetLaunchCount());
			EXPECT_EQ(1, renderer->GetRestartCount());
			//	the camera moved: a whole frame again
			look_down(builder.Camera(), 22, 20, 50);
			view.Render(1.0f, builder.ViewerLayers());
			EXPECT_EQ(6u, renderer->GetSampleCount());
			EXPECT_EQ(6, renderer->GetLaunchCount());
		}
		EXPECT_FALSE(g3dProgressive::IsCapturing());
	});
}

TEST(g3dAORendererOptiXTest, RasterDrawsTheRestOverTheTrace)
{
	gtBackend::RunOptiX([]()
	{
		//	A floor (traced), and a sphere in a viewer layer (never traced;
		//	camera space looks down +z): in front of the floor, or behind it,
		//	where the traced depth hides it
		auto render = [](int i_Sphere)
		{
			gtSceneBuilder builder;
			builder.Add(gtScene::Plane(), builder.Keep(gtScene::DataMaterial("default")), scale_translate(1000, 0, 0, 0));
			look_down(builder.Camera(), 0, 0, 50);
			matMaterial* material = builder.Keep(gtScene::OpaquePhong("Phong.fx", gtScene::e_NonDefault));
			if (i_Sphere == 1)
			{
				builder.AddViewerLayer(gtScene::Sphere(16, 32), material, gtScene::Place(0, 0, 10, 2));
			}
			else if (i_Sphere == 2)
			{
				builder.AddViewerLayer(gtScene::Sphere(16, 32), material, gtScene::Place(0, 0, 200, 40));
			}
			gtTarget target(gtBackend::e_Vulkan, c_Size, c_Size, c_Clear);
			std::unique_ptr<g3dAORendererOptiX> renderer = ao_renderer();
			renderer->SetSampleLimit(1);
			g3dTargetRenderer view(target.Target(), renderer.get(), &builder.Scene(), &builder.Camera());
			view.Render(1.0f, builder.ViewerLayers());
			EXPECT_EQ(2u, renderer->GetSceneBVH().GetTriangleCount());	// the floor alone
			return target.Read();
		};
		const gtPicture floor = render(0);
		const gtPicture in_front = render(1);
		const gtPicture behind = render(2);
		save_if_asked(floor, "floor");
		save_if_asked(in_front, "in_front");
		save_if_asked(behind, "behind");
		ASSERT_TRUE(floor.Valid() && in_front.Valid() && behind.Valid());
		const int mid = c_Size / 2;
		EXPECT_NE(0, std::memcmp(texel(floor, mid, mid), texel(in_front, mid, mid), 4)) << "the sphere in front isn't drawn";
		EXPECT_EQ(0, std::memcmp(texel(floor, 2, 2), texel(in_front, 2, 2), 4));
		EXPECT_EQ(floor.m_Rgba, behind.m_Rgba) << "the sphere behind the floor shows";
	});
}

TEST(g3dAORendererOptiXTest, SharedBuffersReachTheTarget)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		floor_and_wall(builder);
		gtTarget target(gtBackend::e_Vulkan, c_Size, c_Size, c_Clear);
		std::unique_ptr<g3dAORendererOptiX> renderer = ao_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSamplesPerFrame(2);
		renderer->SetSampleLimit(2);
		g3dTargetRenderer view(target.Target(), renderer.get(), &builder.Scene(), &builder.Camera());
		view.Render(1.0f, builder.ViewerLayers());
		const gtPicture picture = target.Read();
		const std::vector<uint32_t> depth = read_depth(target);
		ASSERT_TRUE(picture.Valid());
		ASSERT_EQ(size_t(c_Size * c_Size), depth.size());

		optxFrameShare& share = renderer->GetFrameShare();
		const size_t bytes = size_t(c_Size) * c_Size * 4;
		std::vector<uint8_t> color(bytes);
		std::vector<uint32_t> shared_depth(c_Size * c_Size);
		ASSERT_TRUE(optxContext::Synchronize());
		ASSERT_EQ(CUDA_SUCCESS, cu().cuMemcpyDtoH(color.data(), share.Color(), bytes));
		ASSERT_EQ(CUDA_SUCCESS, cu().cuMemcpyDtoH(shared_depth.data(), share.Depth(), bytes));
		EXPECT_EQ(color, picture.m_Rgba);
		int differ = 0, covered = 0;
		for (size_t i = 0; i < depth.size(); ++i)
		{
			differ += (depth[i] & 0xFFFFFF) != (shared_depth[i] & 0xFFFFFF);
			covered += (shared_depth[i] & 0xFFFFFF) < 0xFFFFFF;
		}
		EXPECT_EQ(0, differ);
		EXPECT_EQ(c_Size * c_Size, covered);	// the floor fills the view
		EXPECT_EQ(1u, share.GetFrameValue());
	});
}

TEST(g3dAORendererOptiXTest, TracedDepthMatchesRasterDepth)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		//	A quad tilted about y, then about x, so depth varies across it both
		//	ways (a flip in x or y would show)
		const float c = std::cos(0.5f), s = std::sin(0.5f), cb = std::cos(0.3f), sb = std::sin(0.3f);
		const maMatrix4x4 tilt(10 * c, 10 * s * sb, -10 * s * cb, 0,
							   0, 10 * cb, 10 * sb, 0,
							   10 * s, -10 * c * sb, 10 * c * cb, 0,
							   0, 0, 0, 1);
		builder.Add(gtScene::Plane(), builder.Keep(gtScene::DataMaterial("default")), tilt);
		look_down(builder.Camera(), 0, 0, 30);

		gtTarget raster_target(gtBackend::e_Vulkan, c_Size, c_Size, c_Clear);
		gvk::g3dSceneRendererVk raster;
		g3dTargetRenderer raster_view(raster_target.Target(), &raster, &builder.Scene(), &builder.Camera());
		raster_target.Target()->ClearDepthStencil(1.0f);
		raster_view.Render(1.0f, builder.ViewerLayers());
		const std::vector<uint32_t> raster_depth = read_depth(raster_target);

		gtTarget traced_target(gtBackend::e_Vulkan, c_Size, c_Size, c_Clear);
		std::unique_ptr<g3dAORendererOptiX> renderer = ao_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetSampleLimit(1);
		g3dTargetRenderer traced_view(traced_target.Target(), renderer.get(), &builder.Scene(), &builder.Camera());
		traced_view.Render(1.0f, builder.ViewerLayers());
		const std::vector<uint32_t> traced_depth = read_depth(traced_target);

		ASSERT_EQ(raster_depth.size(), traced_depth.size());
		const double normal[3] = { s, -c * sb, c * cb };
		const std::vector<int> exact = exact_plane_depth(builder.Camera(), normal);

		//	Pixels both cover: the traced depth against the exact one, and
		//	against the raster's, which has float error of its own (vertex
		//	transforms and interpolation: up to 11 D24 steps from exact here)
		int both = 0, coverage_differs = 0, max_exact = 0, max_raster = 0;
		double sum_exact = 0;
		for (size_t i = 0; i < raster_depth.size(); ++i)
		{
			const int r = int(raster_depth[i] & 0xFFFFFF), t = int(traced_depth[i] & 0xFFFFFF);
			const bool r_hit = r < 0xFFFFFF, t_hit = t < 0xFFFFFF;
			if (r_hit && t_hit)
			{
				++both;
				max_exact = std::max(max_exact, std::abs(t - exact[i]));
				sum_exact += std::abs(t - exact[i]);
				max_raster = std::max(max_raster, std::abs(t - r));
			}
			else if (r_hit != t_hit)
			{
				++coverage_differs;	// rasterization's edge rule against a ray at the centre
			}
		}
		ASSERT_GT(both, c_Size * c_Size / 8);
		EXPECT_LE(max_exact, 4) << "traced depth from exact, in D24 steps";
		EXPECT_LE(sum_exact / both, 1.0) << "mean traced depth from exact, in D24 steps";
		EXPECT_LE(max_raster, 16) << "traced depth from raster depth, in D24 steps";
		EXPECT_LE(coverage_differs, 2 * c_Size);
	});
}

//	Not a check: a picture to look at (run by name with MSP_GT_AO_IMAGES
//	set). Spheres on a floor, from above at an angle, 256 samples a pixel.
TEST(g3dAORendererOptiXTest, DISABLED_Showcase)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		matMaterial* material = builder.Keep(gtScene::DataMaterial("default"));
		builder.Add(gtScene::Plane(), material, scale_translate(20, 0, 0, 0))->GetFragment()->SetCastsShadow(true);
		const gtMesh sphere = gtScene::Sphere(32, 64);
		const float spots[5][3] = { { 0, 0, 2 }, { -4.5f, 1, 1 }, { 4, -1.5f, 1.5f }, { 1.5f, 4, 0.75f }, { -2, -4, 1.2f } };
		for (const float* spot : spots)
		{
			builder.Add(sphere, material, gtScene::Place(spot[0], spot[1], spot[2], spot[2]))->GetFragment()->SetCastsShadow(true);
		}
		ssaoParams settings;
		builder.Scene().GetSSAOSettings(settings);
		settings.m_AORadius = 6.0f;
		builder.Scene().SetSSAOSettings(settings);
		builder.Camera().SetFOV(50.0f);
		builder.Camera().SetAspect(1.0f);
		builder.Camera().SetClip(0.5f, 500.0f);
		builder.Camera().LookAt(maPoint3d(0, -16, 12), maPoint3d(0, 0, 0.5f), maVector3d(0, 0, 1));

		gtTarget target(gtBackend::e_Vulkan, 256, 256, c_Clear);
		std::unique_ptr<g3dAORendererOptiX> renderer = ao_renderer();
		ASSERT_TRUE(renderer);
		renderer->SetAORays(4);
		renderer->SetSamplesPerFrame(256);
		renderer->SetSampleLimit(256);
		g3dTargetRenderer view(target.Target(), renderer.get(), &builder.Scene(), &builder.Camera());
		view.SetBackgroundColor(g2dRGBColor(60, 70, 90));
		view.Render(1.0f, builder.ViewerLayers());
		gtPicture picture = target.Read();
		const char* dir = std::getenv("MSP_GT_AO_IMAGES");
		if (dir && picture.Valid())
		{
			std::filesystem::create_directories(std::filesystem::u8path(dir));
			stbi_write_png((std::string(dir) + "/Showcase.png").c_str(), 256, 256, 4, picture.m_Rgba.data(), 256 * 4);
		}
	});
}
