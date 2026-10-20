/*****************************************************************************
**  shdwAoGiVkTest.cpp
**
**      AO volumes (shdwPassAOVolumes, SSAO with m_bAOInvalid: view-space
**      normals and positions through shdwPassNormals, the volumes drawn
**      from a geometry shader into the reduced-resolution AO buffer with
**      the g_ACos lookup texture, the stencil mask, then the blend or the
**      upsampling blur) and screen-space GI (shdwPassSSGI: peeled depths,
**      multi-horizon GI from the colour buffer, the stencil mask, then the
**      blend or the bilateral blur) against GraphicsDX11's, both through
**      the creator. Then LPV GI (shdwPassLPVGI, m_bGIInvalid: each GI
**      projected light's reflective shadow map from the RSM renderer, its
**      VPLs injected into the radiance volumes from a geometry shader, the
**      propagation, the volumes applied to the scene and blended), the
**      RSM itself (every target, as floats), the cases with no light to
**      inject, and the volumes remade between frames. Last, each AO
**      (SSAO, AO volumes) with each GI (SSGI, LPV), HDR AA off and on.
**
**      Exceptions to gfxcompare's tolerance (max 2/255, at most 0.1% of
**      pixels over 1/255): none.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"
#include "gtTempDir.hpp"

#ifndef MA_CONSTANTS_HPP
#include "Core/Ma/maConstants.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
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
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/Cam/camCamera.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/G3d/g3dProjectedLight.hpp"
#endif
#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/G3d/g3dSceneRenderer.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif
#ifndef G3D_TARGETRENDERER_HPP
#include "Graphics/G3d/g3dTargetRenderer.hpp"
#endif
#ifndef MAT_REFLECTIVE_SHADOWMAP_HPP
#include "GraphicsDX11/Mat/matReflectiveShadowMap.hpp"
#endif
#ifndef GVK_MATREFLECTIVESHADOWMAP_HPP
#include "GraphicsVulkan/Mat/matReflectiveShadowMap.hpp"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace
{
	//	A floor plane facing the camera (z = -1) with four spheres resting
	//	on it, two of them touching, lit by shadow-casting lights. With
	//	i_bMaskOne, sphere 3 doesn't receive occlusion (the stencil mask)
	void spheres_on_floor(gtSceneBuilder& io_Builder, bool i_bMaskOne)
	{
		io_Builder.LightsCastShadows(true);
		matMaterial* floor = io_Builder.Keep(gtScene::ShaderMaterial("Lambert.fx", gtScene::e_Tinted));
		io_Builder.Add(gtScene::Plane(), floor, gtScene::Place(0, 0, -1.0f, 2.6f));

		const gtMesh sphere = gtScene::Sphere(16, 32);
		const char* shaders[2] = { "Blinn.fx", "Lambert.fx" };
		const float x[4] = { -1.3f, -0.25f, 0.55f, 1.45f };
		const float y[4] = { 0.5f, -0.3f, -0.1f, -0.7f };
		for (int i = 0; i < 4; i++)
		{
			matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(shaders[i % 2], gtScene::e_Tinted));
			g3dSceneNode* node = io_Builder.Add(sphere, material, gtScene::Place(x[i], y[i], -0.45f, 0.55f));
			if (i_bMaskOne && i == 3)
				const_cast<g3dFragment*>(node->GetFragment())->SetReceivesOcclusion(false);
		}
		io_Builder.TwoLights();
	}

	gtSceneFunction ao_scene(bool i_bMaskOne = false)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path&)
		{
			spheres_on_floor(io_Builder, i_bMaskOne);
			//	from the side, so the floor around the spheres shows
			io_Builder.Camera().LookAt(maPoint3d(3.0f, 2.5f, 4.5f), maPoint3d(0, 0, -0.7f), maVector3d(0, 1, 0));
			ssaoParams params;
			io_Builder.Scene().GetSSAOSettings(params);
			params.m_AORadius = 1.2f;
			params.m_AORadiusFar = 2.4f;
			params.m_Contrast = 3.0f;
			params.m_Color = maFloatRGBA(0.1f, 0.05f, 0.2f, 1.0f);
			io_Builder.Scene().SetSSAOSettings(params);
		};
	}

	gtRenderOptions ao_prefs(bool i_bAO, int i_Reduce, bool i_bBlur, bool i_bAA)
	{
		gtRenderOptions options;
		options.m_Prefs = [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bEnableSSAO = i_bAO;
			p.m_bAOInvalid = true;
			p.m_AOResolutionReduce = i_Reduce;
			p.m_bEnableSSAOBlur = i_bBlur;
			p.m_bHDRAA = i_bAA;
		};
		return options;
	}

	//	A material shader's material with its own values and its diffuse
	//	colour (g_diffuse) set to i_Color
	matMaterial* coloured(const char* i_pShader, const maFloatRGBA& i_Color)
	{
		matMaterial* material = gtScene::ShaderMaterial(i_pShader, gtScene::e_Defaults);
		std::vector<effParamColor*> colors;
		material->GetShaderParams()->GetAllColorParams(colors);
		bool found = false;
		for (effParamColor* color : colors)
		{
			if (color->GetName() == "g_diffuse")
			{
				color->Property().SetValue(i_Color);
				found = true;
			}
		}
		EXPECT_TRUE(found) << i_pShader << " has no g_diffuse";
		return material;
	}

	//	The plane (facing +Z) scaled to 2.6, turned a quarter about Y
	//	(i_Turn 1 or -1) and moved to x = i_X
	maMatrix4x4 side_wall(float i_X, float i_Turn)
	{
		maMatrix4x4 m;
		m.MakeScale(2.6f, 2.6f, 2.6f);
		m.RotateBy(i_Turn * maConstants::c_fPI_Div_2, maVector3d(0, 1, 0));
		m.TranslateBy(i_X, 0, -0.2f);
		return m;
	}

	//	A white back wall with a red wall on the left and a green one on the
	//	right, two white spheres between them (one in front of the other, a
	//	second depth layer for peeling), lit by shadow-casting lights. With
	//	i_bMaskOne, the back wall doesn't receive GI
	void spheres_between_walls(gtSceneBuilder& io_Builder, bool i_bMaskOne)
	{
		io_Builder.LightsCastShadows(true);
		matMaterial* white = io_Builder.Keep(coloured("Lambert.fx", maFloatRGBA(0.3f, 0.3f, 0.3f, 1.0f)));
		matMaterial* red = io_Builder.Keep(coloured("Lambert.fx", maFloatRGBA(0.4f, 0.04f, 0.02f, 1.0f)));
		matMaterial* green = io_Builder.Keep(coloured("Lambert.fx", maFloatRGBA(0.04f, 0.35f, 0.04f, 1.0f)));
		const gtMesh plane = gtScene::Plane();
		g3dSceneNode* back = io_Builder.Add(plane, white, gtScene::Place(0, 0, -1.5f, 2.6f));
		if (i_bMaskOne)
			const_cast<g3dFragment*>(back->GetFragment())->SetReceivesGI(false);
		//	the side walls: the plane turned to face +X and -X
		io_Builder.Add(plane, red, side_wall(-1.6f, 1.0f));
		io_Builder.Add(plane, green, side_wall(1.6f, -1.0f));

		const gtMesh sphere = gtScene::Sphere(16, 32);
		matMaterial* sphereWhite = io_Builder.Keep(coloured("Blinn.fx", maFloatRGBA(0.3f, 0.3f, 0.3f, 1.0f)));
		io_Builder.Add(sphere, sphereWhite, gtScene::Place(-0.7f, -0.3f, -0.6f, 0.6f));
		io_Builder.Add(sphere, sphereWhite, gtScene::Place(0.5f, -0.1f, 0.3f, 0.5f));
		io_Builder.TwoLights();
		//	TwoLights' point light is outside the green wall: one inside
		io_Builder.PointLight(maPoint3d(-0.6f, 1.2f, 2.0f), maFloatRGBA(0.6f, 0.6f, 0.6f, 1), 25.0f);
	}

	gtSceneFunction gi_scene(bool i_bMaskOne = false)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path&)
		{
			spheres_between_walls(io_Builder, i_bMaskOne);
			ssgiParams params;
			io_Builder.Scene().GetSSGISettings(params);
			params.m_GIRadius = 1.0f;
			params.m_GIRadiusFar = 2.0f;
			params.m_Contrast = 2.0f;
			io_Builder.Scene().SetSSGISettings(params);
		};
	}

	gtRenderOptions gi_prefs(bool i_bGI, bool i_bBlur, bool i_bPeel, bool i_bAA = false)
	{
		gtRenderOptions options;
		options.m_Prefs = [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bEnableSSGI = i_bGI;
			p.m_bGIInvalid = false;
			p.m_bEnableSSGIBlur = i_bBlur;
			p.m_bEnableSSGIDepthPeeling = i_bPeel;
			p.m_SSGINumLayers = i_bPeel ? 3 : 1;
			p.m_bHDRAA = i_bAA;
		};
		return options;
	}
}

TEST(shdwAoGiVkTest, AOVolumesMatchDX11)
{
	gtExpectSameImages("AOVolumesMatchDX11",
	{
		{ "reduce0", ao_scene(), ao_prefs(true, 0, false, false) },
		{ "reduce1", ao_scene(), ao_prefs(true, 1, false, false) },
		//	the HDR AA target: its depth made current with no colour target
		{ "hdraa", ao_scene(), ao_prefs(true, 0, false, true) },
		{ "blur", ao_scene(), ao_prefs(true, 0, true, false) },
		{ "blur_reduce1_hdraa", ao_scene(), ao_prefs(true, 1, true, true) },
		//	one sphere left out by the stencil mask
		{ "masked", ao_scene(true), ao_prefs(true, 0, false, false) },
	});

	gtExpectEffectShows("AOVolumesMatchDX11", ao_scene(), ao_prefs(true, 0, false, false),
						ao_scene(), ao_prefs(false, 0, false, false), 256 * 256 / 50);
	gtExpectEffectShows("AOVolumesMatchDX11/masked", ao_scene(), ao_prefs(true, 0, false, false),
						ao_scene(true), ao_prefs(true, 0, false, false), 256 * 256 / 500);
}

TEST(shdwAoGiVkTest, SSGIMatchDX11)
{
	gtExpectSameImages("SSGIMatchDX11",
	{
		{ "plain", gi_scene(), gi_prefs(true, false, false) },
		{ "blur", gi_scene(), gi_prefs(true, true, false) },
		{ "peeled", gi_scene(), gi_prefs(true, false, true) },
		{ "peeled_blur", gi_scene(), gi_prefs(true, true, true) },
		{ "blur_hdraa", gi_scene(), gi_prefs(true, true, false, true) },
		//	the back wall left out by the stencil mask
		{ "masked", gi_scene(true), gi_prefs(true, false, false) },
	});

	gtExpectEffectShows("SSGIMatchDX11", gi_scene(), gi_prefs(true, false, false),
						gi_scene(), gi_prefs(false, false, false), 256 * 256 / 50);
	gtExpectEffectShows("SSGIMatchDX11/masked", gi_scene(), gi_prefs(true, false, false),
						gi_scene(true), gi_prefs(true, false, false), 256 * 256 / 500);
	//	the second layer only adds GI where the front sphere hides the back
	//	one: a few dozen pixels
	gtExpectEffectShows("SSGIMatchDX11/peeled", gi_scene(), gi_prefs(true, false, true),
						gi_scene(), gi_prefs(true, false, false), 40);
}

namespace
{
	//	LPV GI's light: a GI-enabled projected light, or one of the cases
	//	that give LPV nothing to inject (Review Focus 4)
	enum LpvLight
	{
		e_GILight,			// GI on, casting shadows
		e_NoProjectedLight,	// only the point and directional lights
		e_GIOff,			// a projected light with GI off
		e_PointingAway,		// a GI light looking away from the scene
		e_NoShadows,		// a GI light that casts no shadow
	};

	struct LpvSettings
	{
		int		m_VolumeSize = 32;
		int		m_RSMSize = 512;
		int		m_Iterations = 2;
		float	m_Scale = 1.0f;
		float	m_Falloff = 1.0f;
	};

	void set_lpv(g3dScene& io_Scene, const LpvSettings& i_Settings)
	{
		ssgiParams params;
		io_Scene.GetSSGISettings(params);
		params.m_LPVVolumeSize = i_Settings.m_VolumeSize;
		params.m_LPVRSMSize = i_Settings.m_RSMSize;
		params.m_LPVIteration = i_Settings.m_Iterations;
		params.m_LPVScale = i_Settings.m_Scale;
		params.m_LPVGIFalloff = i_Settings.m_Falloff;
		io_Scene.SetSSGISettings(params);
	}

	//	The walls of spheres_between_walls (a white back wall, red and green
	//	side walls) on a white floor, a white sphere, the usual two lights
	//	and the projected light i_Light says. With i_bMaskFloor, the floor
	//	doesn't receive GI. o_ppLight: the projected light, if any.
	void lpv_room(gtSceneBuilder& io_Builder, LpvLight i_Light, bool i_bMaskFloor, g3dProjectedLight** o_ppLight)
	{
		io_Builder.LightsCastShadows(true);
		matMaterial* white = io_Builder.Keep(coloured("Lambert.fx", maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f)));
		matMaterial* red = io_Builder.Keep(coloured("Lambert.fx", maFloatRGBA(0.8f, 0.08f, 0.04f, 1.0f)));
		matMaterial* green = io_Builder.Keep(coloured("Lambert.fx", maFloatRGBA(0.08f, 0.7f, 0.08f, 1.0f)));
		const gtMesh plane = gtScene::Plane();
		//	everything casts and receives shadows (fragments do neither by
		//	default): the RSM renderer draws only shadow casters
		auto shadowed = [](g3dSceneNode* io_pNode)
		{
			g3dFragment* fragment = const_cast<g3dFragment*>(io_pNode->GetFragment());
			fragment->SetCastsShadow(true);
			fragment->SetReceivesShadow(true);
			return fragment;
		};
		shadowed(io_Builder.Add(plane, white, gtScene::Place(0, 0, -1.5f, 2.6f)));
		shadowed(io_Builder.Add(plane, red, side_wall(-1.6f, 1.0f)));
		shadowed(io_Builder.Add(plane, green, side_wall(1.6f, -1.0f)));
		//	the floor: the plane turned to face +Y
		maMatrix4x4 floor;
		floor.MakeScale(2.6f, 2.6f, 2.6f);
		floor.RotateBy(-maConstants::c_fPI_Div_2, maVector3d(1, 0, 0));
		floor.TranslateBy(0, -1.3f, -0.2f);
		g3dFragment* floor_fragment = shadowed(io_Builder.Add(plane, white, floor));
		if (i_bMaskFloor)
			floor_fragment->SetReceivesGI(false);
		shadowed(io_Builder.Add(gtScene::Sphere(16, 32), white, gtScene::Place(0.2f, -0.7f, -0.4f, 0.6f)));
		io_Builder.TwoLights();

		g3dProjectedLight* light = NULL;
		if (i_Light != e_NoProjectedLight)
		{
			const bool away = (i_Light == e_PointingAway);
			light = io_Builder.ProjectedLight(maPoint3d(0.6f, 1.0f, 2.5f), away ? maPoint3d(1.0f, 2.0f, 8.0f) : maPoint3d(-0.6f, -0.8f, -1.2f),
											  maFloatRGBA(1.0f, 0.95f, 0.9f, 1.0f), i_Light == e_NoShadows ? 0 : 256);
			light->SetGIEnabled(i_Light != e_GIOff);
			if (i_Light == e_NoShadows)
				light->SetCastsShadow(false);
		}
		if (o_ppLight)
			*o_ppLight = light;
	}

	gtSceneFunction lpv_scene(LpvLight i_Light = e_GILight, const LpvSettings& i_Settings = LpvSettings(), bool i_bMaskFloor = false)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path&)
		{
			lpv_room(io_Builder, i_Light, i_bMaskFloor, NULL);
			set_lpv(io_Builder.Scene(), i_Settings);
		};
	}

	gtRenderOptions lpv_prefs(bool i_bLPV, bool i_bAA = false)
	{
		gtRenderOptions options;
		options.m_Prefs = [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bEnableSSGI = i_bLPV;
			p.m_bGIInvalid = true;
			p.m_bHDRAA = i_bAA;
		};
		//	the projected light's shadow map
		options.m_BeforeFrame = [](gtSceneBuilder& io_Builder) { io_Builder.RenderTargets(); };
		return options;
	}

	typedef std::vector<std::vector<float>> RSMTargets;

	//	i_pLight's RSM (i_Size square) rendered as shdwPassLPVGI renders it,
	//	every target read back
	RSMTargets render_rsm(gtSceneBuilder& io_Builder, const g3dProjectedLight* i_pLight, int i_Size)
	{
		RSMTargets targets;
		matTexture* rsm = matTextureMgr::CreateReflectiveShadowMap(i_Size, i_Size);
		EXPECT_TRUE(rsm != NULL);
		if (!rsm)
			return targets;
		std::unique_ptr<g3dSceneRenderer> renderer(g3dSceneRendererCreate::CreateRSMRenderer());
		camCamera camera;
		i_pLight->OrientCamera(camera);
		g2dRenderTarget* target = rsm->GetRenderTargetAPI();
		target->Clear(maFloatRGBA(0, 0, 0, 0));
		g3dTargetRenderer targetRenderer;
		targetRenderer.SetTarget(target);
		targetRenderer.SetScene(&io_Builder.Scene());
		targetRenderer.SetCamera(&camera);
		targetRenderer.SetRenderer(renderer.get());
		targetRenderer.Render(0);

		for (int i = 0; i < matReflectiveShadowMap::e_NumRSMRenderTarget; i++)
		{
			std::vector<uint8_t> bytes;
			if (gtBackend::Current() == gtBackend::e_Vulkan)
			{
				gvk::matReflectiveShadowMap* map = dynamic_cast<gvk::matReflectiveShadowMap*>(rsm);
				EXPECT_TRUE(map != NULL);
				if (map)
					bytes = gtReadVkImage(map->GetTextureSurface(i));
			}
			else
			{
				matReflectiveShadowMap* map = dynamic_cast<matReflectiveShadowMap*>(rsm);
				EXPECT_TRUE(map != NULL);
				if (map)
					bytes = gtReadD3D11Texture(map->GetTextureSurface(i), 16);
			}
			std::vector<float> floats(bytes.size() / sizeof(float));
			if (!floats.empty())
				memcpy(floats.data(), bytes.data(), floats.size() * sizeof(float));
			targets.push_back(floats);
		}
		matTextureMgr::ReleaseTexture(rsm);
		return targets;
	}
}

TEST(shdwAoGiVkTest, RSMMatchesDX11)
{
	const int c_Size = 128;
	const char* names[3] = { "position", "normal", "flux" };
	RSMTargets dx11, vulkan;
	gtTempDir dir;
	auto run = [&](RSMTargets& o_Targets)
	{
		g3dProjectedLight* light = NULL;
		gtRenderOptions options = lpv_prefs(false);
		options.m_AfterRender = [&](gtSceneBuilder& io_Builder) { o_Targets = render_rsm(io_Builder, light, c_Size); };
		gtRenderScene([&](gtSceneBuilder& io_Builder, const std::filesystem::path&)
					  {
						  lpv_room(io_Builder, e_GILight, false, &light);
					  }, dir.GetPath(), options);
	};
	gtBackend::Run(gtBackend::e_DX11, [&]() { run(dx11); });
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { run(vulkan); }))
		return;
	ASSERT_EQ((size_t)matReflectiveShadowMap::e_NumRSMRenderTarget, dx11.size());
	ASSERT_EQ(dx11.size(), vulkan.size());
	for (size_t t = 0; t < dx11.size(); t++)
	{
		ASSERT_EQ((size_t)c_Size * c_Size * 4, dx11[t].size()) << names[t];
		ASSERT_EQ(dx11[t].size(), vulkan[t].size()) << names[t];
		size_t over = 0, first = SIZE_MAX, written = 0;
		for (size_t i = 0; i < dx11[t].size(); i++)
		{
			const float a = dx11[t][i], b = vulkan[t][i];
			written += (a != 0);
			//	1e-5 relative (absolute below 1)
			if (!(std::fabs(a - b) <= 1e-5f * std::max(1.0f, std::max(std::fabs(a), std::fabs(b)))))
			{
				over++;
				first = std::min(first, i);
			}
		}
		EXPECT_EQ(0u, over) << names[t] << ": first at texel " << (first / 4) << " component " << (first % 4)
							<< ": D3D11 " << (first < dx11[t].size() ? dx11[t][first] : 0)
							<< ", Vulkan " << (first < vulkan[t].size() ? vulkan[t][first] : 0);
		//	the light sees the room: most texels are written
		EXPECT_GT(written, dx11[t].size() / 4) << names[t];
	}
}

TEST(shdwAoGiVkTest, LPVGIMatchDX11)
{
	LpvSettings reduced;
	reduced.m_VolumeSize = 16;
	reduced.m_RSMSize = 256;
	LpvSettings strong;
	strong.m_Iterations = 6;
	strong.m_Scale = 3.0f;
	strong.m_Falloff = 0.5f;
	LpvSettings none;
	none.m_Iterations = 0;
	gtExpectSameImages("LPVGIMatchDX11",
	{
		{ "plain", lpv_scene(), lpv_prefs(true) },
		{ "volume16_rsm256", lpv_scene(e_GILight, reduced), lpv_prefs(true) },
		{ "iterations6", lpv_scene(e_GILight, strong), lpv_prefs(true) },
		{ "iterations0", lpv_scene(e_GILight, none), lpv_prefs(true) },
		{ "hdraa", lpv_scene(), lpv_prefs(true, true) },
		//	the floor doesn't receive GI
		{ "masked", lpv_scene(e_GILight, LpvSettings(), true), lpv_prefs(true) },
	});

	gtExpectEffectShows("LPVGIMatchDX11", lpv_scene(), lpv_prefs(true), lpv_scene(), lpv_prefs(false), 256 * 256 / 50);
	gtExpectEffectShows("LPVGIMatchDX11/masked", lpv_scene(), lpv_prefs(true),
						lpv_scene(e_GILight, LpvSettings(), true), lpv_prefs(true), 256 * 256 / 500);
}

TEST(shdwAoGiVkTest, LPVWithoutRSMLightMatchesDX11)
{
	gtExpectSameImages("LPVWithoutRSMLightMatchesDX11",
	{
		{ "no_projected_light", lpv_scene(e_NoProjectedLight), lpv_prefs(true) },
		{ "gi_off", lpv_scene(e_GIOff), lpv_prefs(true) },
		{ "pointing_away", lpv_scene(e_PointingAway), lpv_prefs(true) },
		{ "no_shadows", lpv_scene(e_NoShadows), lpv_prefs(true) },
	});
}

TEST(shdwAoGiVkTest, LPVResizeBetweenFramesIsClean)
{
	//	the GI volume and the RSM are remade between frames, while the frame
	//	before may still read them: each frame compared (a case per frame
	//	count), validation clean
	const LpvSettings sizes[3] = { [] { LpvSettings s; s.m_VolumeSize = 32; s.m_RSMSize = 256; return s; }(),
								   [] { LpvSettings s; s.m_VolumeSize = 16; s.m_RSMSize = 128; return s; }(),
								   [] { LpvSettings s; s.m_VolumeSize = 24; s.m_RSMSize = 512; return s; }() };
	auto resized = [&](int i_Frames) -> gtCase
	{
		std::shared_ptr<int> frame(new int(0));
		gtRenderOptions options = lpv_prefs(true);
		options.m_Frames = i_Frames;
		options.m_BeforeFrame = [=](gtSceneBuilder& io_Builder)
		{
			set_lpv(io_Builder.Scene(), sizes[std::min(*frame, 2)]);
			++*frame;
			io_Builder.RenderTargets();
		};
		gtSceneFunction build = [=](gtSceneBuilder& io_Builder, const std::filesystem::path&)
		{
			*frame = 0;
			lpv_room(io_Builder, e_GILight, false, NULL);
		};
		return { "frame" + std::to_string(i_Frames), build, options };
	};
	gtExpectSameImages("LPVResizeBetweenFramesIsClean", { resized(1), resized(2), resized(3) });
}

namespace
{
	//	LPV's room with ao_scene's SSAO settings and gi_scene's SSGI ones
	void ao_gi_room(gtSceneBuilder& io_Builder, const std::filesystem::path&)
	{
		lpv_room(io_Builder, e_GILight, false, NULL);
		set_lpv(io_Builder.Scene(), LpvSettings());
		ssaoParams ao;
		io_Builder.Scene().GetSSAOSettings(ao);
		ao.m_AORadius = 1.2f;
		ao.m_AORadiusFar = 2.4f;
		ao.m_Contrast = 3.0f;
		ao.m_Color = maFloatRGBA(0.1f, 0.05f, 0.2f, 1.0f);
		io_Builder.Scene().SetSSAOSettings(ao);
		ssgiParams gi;
		io_Builder.Scene().GetSSGISettings(gi);
		gi.m_GIRadius = 1.0f;
		gi.m_GIRadiusFar = 2.0f;
		gi.m_Contrast = 2.0f;
		io_Builder.Scene().SetSSGISettings(gi);
	}

	//	AO (if i_bAO: AO volumes or SSAO) and GI (if i_bGI: LPV or SSGI)
	gtRenderOptions ao_gi_prefs(bool i_bAO, bool i_bAOVolumes, bool i_bGI, bool i_bLPV, bool i_bAA)
	{
		gtRenderOptions options;
		options.m_Prefs = [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bEnableSSAO = i_bAO;
			p.m_bAOInvalid = i_bAOVolumes;
			p.m_bEnableSSGI = i_bGI;
			p.m_bGIInvalid = i_bLPV;
			p.m_bHDRAA = i_bAA;
		};
		//	the projected light's shadow map
		options.m_BeforeFrame = [](gtSceneBuilder& io_Builder) { io_Builder.RenderTargets(); };
		return options;
	}
}

TEST(shdwAoGiVkTest, AOAndGICombinationsMatchDX11)
{
	//	AO and GI share scratch textures and DepthBuffer(), and each pass
	//	clears what the next restores (Review Focus 5)
	std::vector<gtCase> cases;
	for (int aa = 0; aa < 2; aa++)
	{
		for (int volumes = 0; volumes < 2; volumes++)
		{
			for (int lpv = 0; lpv < 2; lpv++)
			{
				const std::string name = std::string(volumes ? "aovolumes" : "ssao") + (lpv ? "_lpv" : "_ssgi") + (aa ? "_hdraa" : "");
				cases.push_back({ name, ao_gi_room, ao_gi_prefs(true, volumes != 0, true, lpv != 0, aa != 0) });
			}
		}
	}
	gtExpectSameImages("AOAndGICombinationsMatchDX11", cases);

	//	the AO shows over each GI
	gtExpectEffectShows("AOAndGICombinationsMatchDX11/ssao_lpv", ao_gi_room, ao_gi_prefs(true, false, true, true, false),
						ao_gi_room, ao_gi_prefs(false, false, true, true, false), 256 * 256 / 100);
	gtExpectEffectShows("AOAndGICombinationsMatchDX11/aovolumes_ssgi", ao_gi_room, ao_gi_prefs(true, true, true, false, false),
						ao_gi_room, ao_gi_prefs(false, true, true, false, false), 256 * 256 / 100);
}
