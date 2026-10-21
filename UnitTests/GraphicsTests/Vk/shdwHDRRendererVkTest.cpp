/*****************************************************************************
**  shdwHDRRendererVkTest.cpp
**
**      The HDR renderer (shdwHDRRendererVk) against GraphicsDX11's
**      (shdwHDRRendererDX11), both made by the creator
**      (g3dSceneRendererCreate::CreateDefaultRenderer) as MachStudio's
**      viewport makes them, rendering the same synthetic scene, compared
**      with gfxcompare's tolerance (alpha included): lit spheres (Plan
**      4b's, and a moderate set that tone mapping doesn't saturate), the
**      global environment with and without its background, bloom and star
**      from a bright sun, alpha, HDR anti-aliasing on and off, each HDR
**      debug buffer, the ambient-only branch (single-light rendering off),
**      every optional pass off, no lights and many lights, overlay and
**      viewer layers, one renderer used at several sizes in turn, and
**      the camera's render-pass buffers (beauty, reflection with a file
**      reflection, AO in place of AO volumes or SSAO; Add and Multiply).
**      Also (Vulkan only) sharing the HDR renderer's buffers with every
**      type, as MachStudio's capture thread and render view do.
**
**      The lights cast shadows, as MachStudio's do (the lit pass draws only
**      those). Plan 4b's built-in Phong row is drawn by the transparent pass
**      (its data materials count as transparent whatever their transparency).
**
**      Exceptions to gfxcompare's tolerance (max 2/255, at most 0.1% of
**      pixels over 1/255):
**
**        case                       measured                reason
**        DebugModes/mode6           max 2/255, 0.21% over   the bright-pass
**            buffer is a quarter-size texture shown 4x enlarged: each texel
**            that differs (by 2/255, beside the sun, where the threshold
**            magnifies half-float rounding) counts as 16 pixels; allowed 0.3%
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"
#include "gtTempDir.hpp"

#ifndef G3D_LIGHTMGR_HPP
#include "Graphics/G3d/g3dLightMgr.hpp"
#endif
#ifndef G3D_PASSBUFFERS_HPP
#include "Graphics/G3d/g3dPassBuffers.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/G3d/g3dLayer.hpp"
#endif
#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/G3d/g3dSceneRenderer.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/Cam/camCamera.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef GVK_SHDW_SCENERENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwSceneRendererVk.hpp"
#endif

#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace
{
	//	The star types used (GlareDef's EGLARELIBTYPE)
	const int c_StarCamera = 1;			// GLT_CAMERA
	const int c_StarCrossScreen = 4;	// GLT_FILTER_CROSSSCREEN

	//	Plan 4b's twelve spheres, lit by shadow-casting lights
	void spheres(gtSceneBuilder& io_Builder, bool i_bLights = true)
	{
		io_Builder.LightsCastShadows(true);
		io_Builder.LitSpheres(i_bLights, true);
	}

	//	Rows Blinn, Phong and Lambert; columns the shader's own values and
	//	tinted colours; lit by shadow-casting lights
	void moderate(gtSceneBuilder& io_Builder, bool i_bLights = true)
	{
		io_Builder.LightsCastShadows(true);
		const gtMesh sphere = gtScene::Sphere(16, 32);
		const gtScene::Values values[2] = { gtScene::e_Defaults, gtScene::e_Tinted };
		const char* shaders[3] = { "Blinn.fx", "Phong.fx", "Lambert.fx" };
		for (int row = 0; row < 3; row++)
		{
			for (int column = 0; column < 2; column++)
			{
				matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(shaders[row], values[column]));
				io_Builder.Add(sphere, material, gtScene::Place(-0.8f + 1.6f * column, 1.4f - 1.4f * row, 0, 0.6f));
			}
		}
		if (i_bLights)
			io_Builder.TwoLights();
	}

	//	i_Peak: the sun's value in the diffuse map (also the background's)
	//	and the specular map
	void environment(gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir, bool i_bBackground, float i_Peak = 8.0f)
	{
		matTexture* diffuse = io_Builder.Keep(gtScene::HdrTexture(i_Dir, 128, 64, i_Peak));
		matTexture* specular = io_Builder.Keep(gtScene::HdrTexture(i_Dir, 256, 128, i_Peak));
		io_Builder.GlobalEnvironment(diffuse, specular, i_bBackground);
	}

	//	The moderate spheres under the global environment
	gtSceneFunction environment_scene(bool i_bBackground, float i_Peak = 8.0f)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			moderate(io_Builder);
			environment(io_Builder, i_Dir, i_bBackground, i_Peak);
		};
	}

	//	A bright sun in view, with i_StarType's glare
	gtSceneFunction bloom_scene(int i_StarType)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			environment_scene(true, 50.0f)(io_Builder, i_Dir);
			camHDRData hdr;
			io_Builder.Camera().GetHDRParams(hdr);
			hdr.m_StarType = i_StarType;
			io_Builder.Camera().SetHDRParams(hdr);
		};
	}

	void lit_spheres(gtSceneBuilder& io_Builder, const std::filesystem::path&)
	{
		spheres(io_Builder);
	}

	void moderate_spheres(gtSceneBuilder& io_Builder, const std::filesystem::path&)
	{
		moderate(io_Builder);
	}

	gtRenderOptions prefs(const std::function<void(g3dPrefs::g3dRenderPrefs&)>& i_Prefs)
	{
		gtRenderOptions options;
		options.m_Prefs = i_Prefs;
		return options;
	}
}

TEST(shdwHDRRendererVkTest, LitSpheresMatchDX11)
{
	gtExpectSameImages("LitSpheresMatchDX11",
	{
		{ "plan4b", lit_spheres, gtRenderOptions() },
		{ "moderate", moderate_spheres, gtRenderOptions() },
	});
}

TEST(shdwHDRRendererVkTest, GlobalEnvironmentMatchDX11)
{
	gtExpectSameImage("GlobalEnvironmentMatchDX11", environment_scene(false));
}

TEST(shdwHDRRendererVkTest, EnvironmentBackgroundMatchDX11)
{
	gtExpectSameImage("EnvironmentBackgroundMatchDX11", environment_scene(true));
}

TEST(shdwHDRRendererVkTest, BloomAndStarMatchDX11)
{
	gtRenderOptions three_frames;
	three_frames.m_Frames = 3;		// luminance adaptation
	gtExpectSameImages("BloomAndStarMatchDX11",
	{
		{ "no_star", bloom_scene(0), three_frames },
		{ "camera", bloom_scene(c_StarCamera), three_frames },
		{ "cross_screen", bloom_scene(c_StarCrossScreen), three_frames },
	});
}

TEST(shdwHDRRendererVkTest, AlphaMatchesDX11)
{
	//	the spheres over the transparent clear, one of them invisible with
	//	its mask black (a hole in the alpha)
	gtExpectSameImage("AlphaMatchesDX11", [](gtSceneBuilder& io_Builder, const std::filesystem::path&)
	{
		moderate(io_Builder);
		io_Builder.Layer().GetRootNode()->GetChildren()[4]->SetActiveInRenderLayer(false);
	});
}

TEST(shdwHDRRendererVkTest, HDRAAMatchDX11)
{
	//	(HDR anti-aliasing is on by default: MSAA, resolve and depth copy)
	gtExpectSameImages("HDRAAMatchDX11",
	{
		{ "on", environment_scene(true), prefs([](g3dPrefs::g3dRenderPrefs& p) { p.m_bHDRAA = true; }) },
		{ "off", environment_scene(true), prefs([](g3dPrefs::g3dRenderPrefs& p) { p.m_bHDRAA = false; }) },
	});
}

TEST(shdwHDRRendererVkTest, DebugModesMatchDX11)
{
	//	each intermediate buffer shown (mode 4 needs DOF, Plan 5b; mode 10
	//	SSAO, Task 5); some are mostly black by design
	std::vector<gtCase> cases;
	for (int mode : { 1, 2, 3, 5, 6, 7, 8, 9 })
	{
		gtRenderOptions options = prefs([mode](g3dPrefs::g3dRenderPrefs& p) { p.m_HDRDebugMode = mode; });
		options.m_bCheckCoverage = false;
		if (mode == 6)
			options.m_MaxPctOver = 0.3f;		// see the exception table
		cases.push_back({ "mode" + std::to_string(mode), bloom_scene(c_StarCrossScreen), options });
	}
	gtExpectSameImages("DebugModesMatchDX11", cases);
}

TEST(shdwHDRRendererVkTest, AmbientOnlyBranchMatchesDX11)
{
	//	single-light rendering off: the ambient pass instead of the
	//	environment and lit passes (material swatches render this way)
	gtRenderOptions options;
	options.m_bSingleLight = false;
	gtExpectSameImages("AmbientOnlyBranchMatchesDX11",
	{
		{ "spheres", moderate_spheres, options },
		{ "environment", environment_scene(true), options },
	});
}

TEST(shdwHDRRendererVkTest, PassesOffMatchDX11)
{
	//	every optional pass off leaves the alpha fill and black colours (the
	//	alpha is still compared); with the lit pass back on, colours too
	auto off = [](g3dPrefs::g3dRenderPrefs& p)
	{
		p.m_bEnableEnvironment = false;
		p.m_bEnableLitPass = false;
		p.m_bEnableTransparent = false;
		p.m_bEnableGlow = false;
		p.m_bEnableOutline = false;
		p.m_bEnableSSAO = false;
		p.m_bEnableReflection = false;
	};
	gtRenderOptions all_off = prefs(off);
	all_off.m_bCheckCoverage = false;
	gtExpectSameImages("PassesOffMatchDX11",
	{
		{ "all", environment_scene(true), all_off },
		{ "all_but_lit", environment_scene(true), prefs([off](g3dPrefs::g3dRenderPrefs& p) { off(p); p.m_bEnableLitPass = true; }) },
	});
}

TEST(shdwHDRRendererVkTest, NoLightsAndManyLightsMatchDX11)
{
	gtRenderOptions no_lights;
	no_lights.m_bCheckCoverage = false;		// only the ambient light
	gtExpectSameImages("NoLightsAndManyLightsMatchDX11",
	{
		{ "none", [](gtSceneBuilder& io_Builder, const std::filesystem::path&) { moderate(io_Builder, false); }, no_lights },
		{ "twelve", [](gtSceneBuilder& io_Builder, const std::filesystem::path&)
		{
			moderate(io_Builder, false);
			for (int i = 0; i < 12; i++)
			{
				const float a = i * 6.2831853f / 12;
				io_Builder.PointLight(maPoint3d(3.5f * cosf(a), 3.5f * sinf(a), 2.0f + (i % 3)),
									  maFloatRGBA(0.15f + 0.05f * (i % 4), 0.2f, 0.25f - 0.05f * (i % 3), 1), 12.0f);
			}
		}, gtRenderOptions() },
	});
}

TEST(shdwHDRRendererVkTest, OverlayLayersMatchDX11)
{
	gtExpectSameImage("OverlayLayersMatchDX11", [](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		moderate(io_Builder);
		//	camera space looks down +Z: a sphere in the scene's second
		//	(overlay) layer, in front of the scene, and one in a viewer layer
		matMaterial* overlay = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_NonDefault));
		io_Builder.AddLayer(gtScene::Sphere(16, 32), overlay, gtScene::Place(-0.6f, -0.5f, 3.0f, 0.4f));
		matMaterial* viewer = io_Builder.Keep(gtScene::OpaquePhong("Phong.fx", gtScene::e_NonDefault));
		io_Builder.AddViewerLayer(gtScene::Sphere(16, 32), viewer, gtScene::Place(0.7f, 0.6f, 4.0f, 0.5f));
	});
}

TEST(shdwHDRRendererVkTest, ResizeBetweenFramesMatchesDX11)
{
	//	one renderer at 256x256, 200x150 and 256x256 again: its frame
	//	buffers are remade at each size. A 0x0 view is skipped, as a
	//	minimized MachStudio view isn't rendered (no 0x0 target exists).
	struct Size { int m_Width, m_Height; };
	const Size sizes[] = { { 256, 256 }, { 200, 150 }, { 0, 0 }, { 256, 256 } };

	gtTempDir dir;
	auto render_all = [&](std::vector<gtPicture>& o_Pictures)
	{
		gtPrefsScope prefs;
		gtSceneBuilder builder;
		g3dLightMgr::SetAmbient(maFloatRGBA(0.1f, 0.15f, 0.2f, 1));
		environment_scene(true)(builder, dir.GetPath());
		std::unique_ptr<g3dSceneRenderer> renderer(g3dSceneRendererCreate::CreateDefaultRenderer());
		const float clear[4] = { 0, 0, 0, 0 };
		for (const Size& size : sizes)
		{
			if (size.m_Width == 0 || size.m_Height == 0)
				continue;
			gtTarget target(gtBackend::Current(), size.m_Width, size.m_Height, clear);
			builder.Camera().SetAspect((float)size.m_Width / (float)size.m_Height);
			renderer->Render(target.Target(), builder.Camera(), builder.Scene(), builder.ViewerLayers(), 1.25f);
			o_Pictures.push_back(target.Read());
		}
		renderer->ReleaseResources();
	};
	std::vector<gtPicture> dx11, vulkan;
	gtBackend::Run(gtBackend::e_DX11, [&]() { render_all(dx11); });
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { render_all(vulkan); }))
		return;
	ASSERT_EQ(dx11.size(), 3u);
	ASSERT_EQ(vulkan.size(), dx11.size());
	for (size_t i = 0; i < dx11.size(); i++)
	{
		const std::string name = "ResizeBetweenFramesMatchesDX11/" + std::to_string(i) + "_" +
			std::to_string(dx11[i].m_Width) + "x" + std::to_string(dx11[i].m_Height);
		SCOPED_TRACE(name);
		ASSERT_TRUE(dx11[i].Valid() && vulkan[i].Valid());
		ASSERT_EQ(dx11[i].m_Width, vulkan[i].m_Width);
		ASSERT_EQ(dx11[i].m_Height, vulkan[i].m_Height);
		const gtCompareResult result = gtCompareImages(dx11[i].m_Rgba.data(), vulkan[i].m_Rgba.data(), dx11[i].m_Width, dx11[i].m_Height);
		std::cout << "  " << name << ": " << result.Describe() << std::endl;
		if (!result.m_bPass)
		{
			ADD_FAILURE() << result.Describe() << " (images in "
				<< gtSaveComparison(name, dx11[i].m_Rgba.data(), vulkan[i].m_Rgba.data(), dx11[i].m_Width, dx11[i].m_Height) << ")";
		}
	}
}


namespace
{
	//	A render-pass buffer: blended with i_Op at i_Intensity, or none
	struct PassBuffer
	{
		bool	m_bOn = false;
		int		m_Op = g3dPassBuffers::e_ADD;
		float	m_Intensity = 1.0f;
	};

	PassBuffer pass_buffer(int i_Op, float i_Intensity)
	{
		PassBuffer buffer;
		buffer.m_bOn = true;
		buffer.m_Op = i_Op;
		buffer.m_Intensity = i_Intensity;
		return buffer;
	}

	struct PassBuffers
	{
		PassBuffer	m_Beauty, m_Refl, m_AO;
		bool		m_bFileRefl = false;	// g3dPassBuffers::SetDoingFileRefl
	};

	//	The moderate spheres above a planar reflector (BlinnReflection, dark,
	//	with shdwReflectionVkTest's strong Fresnel bias) under the global
	//	environment (its pass draws the reflection term), the
	//	camera given i_Buffers' pass buffers as cmraCameraObject loads them
	//	(soft checker images: each buffer its own)
	gtSceneFunction pass_buffer_scene(const PassBuffers& i_Buffers)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			moderate(io_Builder);
			io_Builder.Camera().LookAt(maPoint3d(0, 1.0f, 7.0f), maPoint3d(0, -0.6f, 0), maVector3d(0, 1, 0));
			maMatrix4x4 floor;
			floor.MakeRotate(maVector3d(0, 0, 1), maVector3d(0, 1, 0));
			floor.ScaleBy(3.0f, 3.0f, 3.0f);
			floor.TranslateBy(0, -2.1f, 0);
			matMaterial* reflector = io_Builder.Keep(gtScene::ShaderMaterial("BlinnReflection.fx", gtScene::e_Defaults));
			gtScene::SetColor(reflector, "g_diffuse", maFloatRGBA(0.1f, 0.1f, 0.12f, 1));
			gtScene::SetFloat(reflector, "fresnelBias", 0.6f);
			io_Builder.Reflective(io_Builder.Add(gtScene::Plane(), reflector, floor), true, false);
			environment(io_Builder, i_Dir, false);

			camPassBuffersData data;
			int cells = 4;
			auto image = [&](const PassBuffer& i_Buffer, matTexture*& o_pTexture, int& o_Op, float& o_Intensity)
			{
				if (!i_Buffer.m_bOn)
					return;
				o_pTexture = io_Builder.Keep(gtScene::CheckerTexture(i_Dir, 128, 128, cells++));
				o_Op = i_Buffer.m_Op;
				o_Intensity = i_Buffer.m_Intensity;
			};
			image(i_Buffers.m_Beauty, data.m_BeautyBuffer, data.m_BeautyBlendOp, data.m_BeautyIntensity);
			image(i_Buffers.m_Refl, data.m_ReflBuffer, data.m_ReflBlendOp, data.m_ReflIntensity);
			image(i_Buffers.m_AO, data.m_AOBuffer, data.m_AOBlendOp, data.m_AOIntensity);
			io_Builder.Camera().SetPassBuffersParams(data);
			//	gtPrefsScope restores it
			g3dPassBuffers::SetDoingFileRefl(i_Buffers.m_bFileRefl);
		};
	}

	//	The reflection rendered before the frame; AO volumes (i_bAOVolumes)
	//	or SSAO if i_bAO
	gtRenderOptions pass_buffer_prefs(bool i_bAO = false, bool i_bAOVolumes = true, bool i_bAA = false)
	{
		gtRenderOptions options;
		options.m_Prefs = [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bEnableReflection = true;
			p.m_bEnableSSAO = i_bAO;
			p.m_bAOInvalid = i_bAOVolumes;
			p.m_bHDRAA = i_bAA;
		};
		options.m_BeforeFrame = [](gtSceneBuilder& io_Builder) { io_Builder.RenderTargets(); };
		return options;
	}
}

TEST(shdwHDRRendererVkTest, PassBuffersMatchDX11)
{
	//	the blend ops MachStudio offers (cmraDriverRenderPass: Add and
	//	Multiply; Replace's tag is commented out, and BlendBuffers sets no
	//	blend state for it)
	const int ops[2] = { g3dPassBuffers::e_ADD, g3dPassBuffers::e_MUL };
	const char* opNames[2] = { "add", "mul" };
	std::vector<gtCase> cases;
	for (int i = 0; i < 2; i++)
	{
		const std::string op = opNames[i];
		PassBuffers beauty;
		beauty.m_Beauty = pass_buffer(ops[i], 0.6f);
		cases.push_back({ "beauty_" + op, pass_buffer_scene(beauty), pass_buffer_prefs() });

		//	a file reflection: the reflection buffer in place of the
		//	planar reflection (not rendered, and no map in the material)
		PassBuffers refl;
		refl.m_Refl = pass_buffer(ops[i], 0.8f);
		refl.m_bFileRefl = true;
		cases.push_back({ "refl_file_" + op, pass_buffer_scene(refl), pass_buffer_prefs() });

		//	the AO buffer in place of the AO volumes pass, and of SSAO
		PassBuffers ao;
		ao.m_AO = pass_buffer(ops[i], 0.7f);
		cases.push_back({ "ao_volumes_" + op, pass_buffer_scene(ao), pass_buffer_prefs(true, true) });
		cases.push_back({ "ssao_" + op, pass_buffer_scene(ao), pass_buffer_prefs(true, false) });
	}
	//	the reflection buffer without a file reflection: left out, and the
	//	planar reflection rendered
	PassBuffers reflNotFile;
	reflNotFile.m_Refl = pass_buffer(g3dPassBuffers::e_ADD, 0.8f);
	cases.push_back({ "refl_not_file", pass_buffer_scene(reflNotFile), pass_buffer_prefs() });
	//	a file reflection with no reflection buffer
	PassBuffers fileOnly;
	fileOnly.m_bFileRefl = true;
	cases.push_back({ "file_refl_no_buffer", pass_buffer_scene(fileOnly), pass_buffer_prefs() });
	//	all three, AO volumes and HDR AA on
	PassBuffers all;
	all.m_Beauty = pass_buffer(g3dPassBuffers::e_MUL, 1.2f);
	all.m_Refl = pass_buffer(g3dPassBuffers::e_ADD, 0.5f);
	all.m_AO = pass_buffer(g3dPassBuffers::e_MUL, 1.0f);
	all.m_bFileRefl = true;
	cases.push_back({ "all_hdraa", pass_buffer_scene(all), pass_buffer_prefs(true, true, true) });
	gtExpectSameImages("PassBuffersMatchDX11", cases);

	PassBuffers none, fileRefl;
	fileRefl.m_bFileRefl = true;
	PassBuffers beauty, refl, ao;
	beauty.m_Beauty = pass_buffer(g3dPassBuffers::e_ADD, 0.6f);
	refl.m_Refl = pass_buffer(g3dPassBuffers::e_ADD, 0.8f);
	refl.m_bFileRefl = true;
	ao.m_AO = pass_buffer(g3dPassBuffers::e_MUL, 0.7f);
	const int c_Min = 256 * 256 / 50;
	gtExpectEffectShows("PassBuffersMatchDX11/beauty", pass_buffer_scene(beauty), pass_buffer_prefs(),
						pass_buffer_scene(none), pass_buffer_prefs(), c_Min);
	gtExpectEffectShows("PassBuffersMatchDX11/refl_file", pass_buffer_scene(refl), pass_buffer_prefs(),
						pass_buffer_scene(fileRefl), pass_buffer_prefs(), c_Min);
	//	a file reflection drops the planar reflection
	gtExpectEffectShows("PassBuffersMatchDX11/file_refl", pass_buffer_scene(none), pass_buffer_prefs(),
						pass_buffer_scene(fileRefl), pass_buffer_prefs(), 256 * 256 / 500);
	gtExpectEffectShows("PassBuffersMatchDX11/ao_volumes", pass_buffer_scene(ao), pass_buffer_prefs(true, true),
						pass_buffer_scene(none), pass_buffer_prefs(true, true), c_Min);
}

//	MachStudio's capture thread (cptrRenderThread) and render view
//	(tma3dRenderView) share the HDR renderer's buffers with each renderer
//	they make; every type the creator makes is a shdw renderer, as on DX11,
//	but the opacity map renderer, whose forward stand-in (Plan 7) has no
//	buffers and is left alone. Each partner renders once with the shared
//	buffers. A partner of any other class asserts, as DX11's.
TEST(shdwHDRRendererVkTest, ShareBuffersWithEveryType)
{
	const g3dSceneRendererTypes::RendererType types[] =
	{
		g3dSceneRendererTypes::e_Default, g3dSceneRendererTypes::e_HDR,
		g3dSceneRendererTypes::e_AmbientOcclusion, g3dSceneRendererTypes::e_Depth,
		g3dSceneRendererTypes::e_ShadowMask, g3dSceneRendererTypes::e_Materials,
		g3dSceneRendererTypes::e_VelocityMap, g3dSceneRendererTypes::e_Normals,
		g3dSceneRendererTypes::e_IlluminationOnly, g3dSceneRendererTypes::e_DirtyMatte,
		g3dSceneRendererTypes::e_Wireframe, g3dSceneRendererTypes::e_ReflectionOnly,
		g3dSceneRendererTypes::e_GlobalIllumination, g3dSceneRendererTypes::e_Glow,
	};
	gtTempDir dir;
	gtBackend::Run(gtBackend::e_Vulkan, [&]()
	{
		gtPrefsScope prefs_scope;
		std::unique_ptr<g3dSceneRenderer> hdr(g3dSceneRendererCreate::CreateHDRRenderer());
		gvk::shdwSceneRendererVk* hdrShdw = dynamic_cast<gvk::shdwSceneRendererVk*>(hdr.get());
		ASSERT_TRUE(hdrShdw != NULL);

		for (g3dSceneRendererTypes::RendererType type : types)
		{
			SCOPED_TRACE(type);
			gtRenderOptions options;
			options.m_CreateRenderer = [&]()
			{
				g3dSceneRenderer* partner = g3dSceneRendererCreate::CreateRenderer(type);
				gvk::shdwSceneRendererVk* shdw = dynamic_cast<gvk::shdwSceneRendererVk*>(partner);
				EXPECT_TRUE(shdw != NULL);
				hdr->ShareBuffers(partner);
				if (shdw)
					EXPECT_EQ(shdw->GetFrameBuffer(), hdrShdw->GetFrameBuffer());
				return partner;
			};
			gtRenderScene(lit_spheres, dir.GetPath(), options);
		}

		{	//	the opacity map renderer's stand-in
			SCOPED_TRACE("opacity map");
			gtRenderOptions options;
			options.m_CreateRenderer = [&]()
			{
				g3dSceneRenderer* partner = g3dSceneRendererCreate::CreateOpacityMapRenderer(NULL);
				EXPECT_TRUE(dynamic_cast<gvk::shdwSceneRendererVk*>(partner) == NULL);
				hdr->ShareBuffers(partner);
				return partner;
			};
			gtRenderScene(lit_spheres, dir.GetPath(), options);
		}

	#if ENV_BUILD == ENV_DEBUGBUILD
		{	//	not a shdw renderer: asserts (DBG_ASSERT is compiled out of Release)
			static int s_Asserts;
			s_Asserts = 0;
			dbgMsg::AssertHandler previous = dbgMsg::SetAssertHandler([](const std::string&) { ++s_Asserts; });
			std::unique_ptr<g3dSceneRenderer> cube(g3dSceneRendererCreate::CreateCubeMapRenderer(NULL, NULL, false));
			hdr->ShareBuffers(cube.get());
			dbgMsg::SetAssertHandler(previous);
			EXPECT_EQ(1, s_Asserts);
		}
	#endif
	});
}
