/*****************************************************************************
**  shdwOpacityVkTest.cpp
**
**      Hair shadows on Vulkan against DX11: the opacity map renderer
**      (shdwOpacityMapRendererVk, made by g3dSceneRendererCreate::
**      CreateOpacityMapRenderer) and its pass (shdwPassOpacity), drawing a
**      projected light's opacity maps as prjltProjectedLightObject sets
**      them up (gtSceneBuilder::HairShadows), and the hair pass reading
**      them:
**      - every map (and the volume) of every HAIR_SHADOW_TYPE, read back
**        after the light's targets are rendered, over hair, its head and a
**        transparent sphere;
**      - the HDR renderer's image with those maps (hair self-shadowing);
**      - hair settings changed between frames: tessellation and hair lines
**        off, a shadow type change that makes the light's maps again
**        (the old ones destroyed while the hair effect still has them;
**        Plan 7 Review Focus 4), each frame compared.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"
#include "gtTempDir.hpp"

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
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
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif

#include <gtest/gtest.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace
{
	const int c_MapSize = 512;		// the light's shadow map
	const int c_OpacitySize = 256;	// its opacity maps

	const std::pair<HAIR_SHADOW_TYPE, const char*> c_Types[] =
	{
		{ HAIR_SHADOW_OSM4, "OSM4" }, { HAIR_SHADOW_OSM16, "OSM16" }, { HAIR_SHADOW_OSM32, "OSM32" },
		{ HAIR_SHADOW_DOSM4, "DOSM4" }, { HAIR_SHADOW_DOSM16, "DOSM16" }, { HAIR_SHADOW_DOSM32, "DOSM32" },
		{ HAIR_SHADOW_VOLUME4, "VOLUME4" },
	};

	int map_count(HAIR_SHADOW_TYPE i_Type)
	{
		if (i_Type == HAIR_SHADOW_OSM16 || i_Type == HAIR_SHADOW_DOSM16)
			return 4;
		if (i_Type == HAIR_SHADOW_OSM32 || i_Type == HAIR_SHADOW_DOSM32)
			return 8;
		return 1;
	}

	//	Casts and receives shadows (fragments do neither by default)
	void casts_shadow(g3dSceneNode* io_pNode)
	{
		g3dFragment* fragment = const_cast<g3dFragment*>(io_pNode->GetFragment());
		fragment->SetCastsShadow(true);
		fragment->SetReceivesShadow(true);
	}

	//	What a scene made; the frame counter for m_BeforeFrame (reset when
	//	the scene is built, so each backend's run counts from 0)
	struct opacity_state
	{
		g3dProjectedLight*	m_pLight = NULL;
		int					m_Frame = 0;
	};

	struct opacity_setup
	{
		HAIR_SHADOW_TYPE	m_Type = HAIR_SHADOW_OSM4;
		bool				m_bHairShadows = true;	// false: the light has no opacity maps
	};

	//	Hair on a head with a transparent sphere in front, a wall behind,
	//	and a projected light (with a shadow map, and i_Setup's hair
	//	shadows) shining on them; everything casts and receives
	g3dProjectedLight* build_opacity(gtSceneBuilder& io_Builder, const opacity_setup& i_Setup)
	{
		const maMatrix4x4 place = gtScene::Place(0, -0.35f, 0, 0.9f);
		matMaterial* head = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Tinted));
		casts_shadow(io_Builder.Add(gtScene::Sphere(16, 32), head, place * gtScene::Place(0, 0, 0, 0.97f)));
		gtHairShape shape;
		shape.m_nStrands = 64;
		shape.m_nVerticesPerStrand = 8;
		shape.m_Length = 1.0f;
		shape.m_RootRadius = 0.02f;
		shape.m_TipRadius = 0.006f;
		casts_shadow(io_Builder.Hair(shape, place));
		matMaterial* glass = io_Builder.Keep(gtScene::DataMaterial("Phong.fx", [](effShaderData* d)
		{
			gtScene::SetPhong(d, gtScene::e_NonDefault);		// transparency 0.7
		}));
		casts_shadow(io_Builder.Add(gtScene::Sphere(12, 24), glass, gtScene::Place(0.7f, 0.9f, 2.2f, 0.35f)));
		matMaterial* wall = io_Builder.Keep(gtScene::ShaderMaterial("Lambert.fx", gtScene::e_Defaults));
		casts_shadow(io_Builder.Add(gtScene::Plane(), wall, gtScene::Place(0, 0, -1.5f, 4.0f)));
		g3dProjectedLight* light = io_Builder.ProjectedLight(maPoint3d(1.5f, 2.5f, 7.0f), maPoint3d(0, 0, -1.5f),
															 maFloatRGBA(1.0f, 0.95f, 0.85f, 1), c_MapSize);
		if (i_Setup.m_bHairShadows)
			io_Builder.HairShadows(light, i_Setup.m_Type, c_OpacitySize);
		return light;
	}

	gtSceneFunction opacity_scene(const opacity_setup& i_Setup, const std::shared_ptr<opacity_state>& i_pState = nullptr)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path&)
		{
			g3dProjectedLight* light = build_opacity(io_Builder, i_Setup);
			if (i_pState)
			{
				i_pState->m_pLight = light;
				i_pState->m_Frame = 0;
			}
		};
	}

	void hair_prefs(g3dPrefs::g3dRenderPrefs& io_Prefs, HAIR_SHADOW_TYPE i_Type)
	{
		io_Prefs.m_bEnableHair = true;
		io_Prefs.m_bEnableShadows = true;
		io_Prefs.m_HairTransparencyMode = 1;
		io_Prefs.m_HairShadowType = i_Type;
	}

	//	The HDR renderer, hair on, the light's targets rendered each frame
	gtRenderOptions opacity_options(HAIR_SHADOW_TYPE i_Type)
	{
		gtRenderOptions options;
		options.m_CreateRenderer = []() { return g3dSceneRendererCreate::CreateHDRRenderer(); };
		options.m_Prefs = [=](g3dPrefs::g3dRenderPrefs& p) { hair_prefs(p, i_Type); };
		options.m_BeforeFrame = [](gtSceneBuilder& io_Builder) { io_Builder.RenderTargets(); };
		return options;
	}

	//	An L8 volume's texels as RGBA8 (grey, opaque), for gtCompareImages
	std::vector<uint8_t> grey_to_rgba(const std::vector<uint8_t>& i_Grey)
	{
		std::vector<uint8_t> rgba(i_Grey.size() * 4);
		for (size_t t = 0; t < i_Grey.size(); t++)
		{
			rgba[t * 4 + 0] = rgba[t * 4 + 1] = rgba[t * 4 + 2] = i_Grey[t];
			rgba[t * 4 + 3] = 255;
		}
		return rgba;
	}

	//	Every opacity map (RGBA8) and the volume (as RGBA8) of each type
	//	after the light's targets are rendered once, on the backend that is
	//	up, named "<type>/map<i>" and "<type>/volume"
	std::map<std::string, std::vector<uint8_t>> opacity_maps()
	{
		std::map<std::string, std::vector<uint8_t>> maps;
		for (const auto& type : c_Types)
		{
			gtPrefsScope prefs;
			hair_prefs(g3dPrefs::CurrentPrefs(), type.first);
			gtSceneBuilder builder;
			opacity_setup setup;
			setup.m_Type = type.first;
			g3dProjectedLight* light = build_opacity(builder, setup);
			builder.RenderTargets();
			for (int i = 0; i < map_count(type.first); i++)
			{
				maps[std::string(type.second) + "/map" + std::to_string(i)] =
					gtReadTexture(gtBackend::Current(), builder.OpacityMap(light, i), 4);
			}
			if (matTexture* volume = builder.OpacityVolume(light))
				maps[std::string(type.second) + "/volume"] = grey_to_rgba(gtReadTexture(gtBackend::Current(), volume, 1));
		}
		return maps;
	}

	size_t nonzero_texels(const std::vector<uint8_t>& i_Rgba)
	{
		size_t n = 0;
		for (size_t t = 0; t + 3 < i_Rgba.size(); t += 4)
			n += (i_Rgba[t] | i_Rgba[t + 1] | i_Rgba[t + 2] | i_Rgba[t + 3]) != 0;
		return n;
	}

	//	The volume's texels (as RGBA8) that differ from its initial data
	//	(slices 0 and 1 black, 2 and 3 white)
	size_t changed_volume_texels(const std::vector<uint8_t>& i_Rgba)
	{
		size_t n = 0;
		const size_t texels = i_Rgba.size() / 4;
		for (size_t t = 0; t < texels; t++)
			n += i_Rgba[t * 4] != (t < texels / 2 ? 0 : 255);
		return n;
	}
}

//	Every map of every hair shadow type (OSM: 1, 4 or 8 RGBA maps of 4
//	depth layers each; DOSM the same; VOLUME4: one map and the L8 volume),
//	within the image tolerance; the guard: hair (and the sphere) are in
//	DX11's maps of each type (for VOLUME4, in its volume)
TEST(shdwOpacityVkTest, OpacityMapsMatchDX11)
{
	std::map<std::string, std::vector<uint8_t>> dx11, vulkan;
	gtBackend::Run(gtBackend::e_DX11, [&]() { dx11 = opacity_maps(); });
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = opacity_maps(); }))
		return;
	ASSERT_EQ(dx11.size(), vulkan.size());
	std::map<std::string, size_t> drawn;		// texels DX11 drew, by type
	for (const auto& d : dx11)
	{
		SCOPED_TRACE(d.first);
		const std::vector<uint8_t>& v = vulkan[d.first];
		const bool volume = d.first.find("/volume") != std::string::npos;
		const int height = volume ? c_OpacitySize * 4 : c_OpacitySize;
		ASSERT_EQ((size_t)c_OpacitySize * height * 4, d.second.size());
		ASSERT_EQ(d.second.size(), v.size());
		const gtCompareResult result = gtCompareImages(d.second.data(), v.data(), c_OpacitySize, height);
		const size_t set = volume ? changed_volume_texels(d.second) : nonzero_texels(d.second);
		std::cout << "  " << d.first << ": " << result.Describe() << ", " << set
				  << (volume ? " texels changed on DX11" : " texels set on DX11") << std::endl;
		EXPECT_TRUE(result.m_bPass) << result.Describe() << "; images in "
			<< gtSaveComparison("OpacityMapsMatchDX11/" + d.first, d.second.data(), v.data(), c_OpacitySize, height);
		const std::string type = d.first.substr(0, d.first.find('/'));
		if (type != "VOLUME4" || volume)
			drawn[type] += set;
	}
	for (const auto& type : c_Types)
		EXPECT_GT(drawn[type.second], (size_t)c_OpacitySize * c_OpacitySize / 100) << type.second << ": DX11 drew nothing";
}

//	The HDR renderer's image with each type's maps (the hair pass reads
//	them: hair self-shadowing); the guard: on DX11 the maps change it
//	(on 0.1% of the image: the deep types, which also read the light's
//	shadow map, change the strands least, 87 to 145 pixels)
TEST(shdwOpacityVkTest, HairShadowsMatchDX11)
{
	std::vector<gtCase> cases;
	for (const auto& type : c_Types)
	{
		opacity_setup setup;
		setup.m_Type = type.first;
		cases.push_back({ type.second, opacity_scene(setup), opacity_options(type.first) });
	}
	gtExpectSameImages("HairShadowsMatchDX11", cases);
	for (const auto& type : c_Types)
	{
		opacity_setup with, without;
		with.m_Type = type.first;
		without.m_bHairShadows = false;
		gtExpectEffectShows((std::string("HairShadowsMatchDX11/") + type.second).c_str(),
							opacity_scene(with), opacity_options(type.first),
							opacity_scene(without), opacity_options(type.first), 256 * 256 / 1000);
	}
}

//	Review Focus 4: four frames, one setting changed before each of the
//	last three (tessellation off; hair lines off; the shadow type OSM4 ->
//	OSM16, which makes the light's maps again while the hair effect still
//	has the old ones), each frame compared (case "frame<n>" renders n
//	frames and keeps the last)
TEST(shdwOpacityVkTest, HairPrefsChangeBetweenFramesMatchesDX11)
{
	std::vector<gtCase> cases;
	for (int frames = 1; frames <= 4; frames++)
	{
		auto state = std::make_shared<opacity_state>();
		opacity_setup setup;
		gtRenderOptions options = opacity_options(HAIR_SHADOW_OSM4);
		options.m_Frames = frames;
		options.m_Prefs = [](g3dPrefs::g3dRenderPrefs& p)
		{
			hair_prefs(p, HAIR_SHADOW_OSM4);
			p.m_bUseHardwareTessellation = true;
			p.m_bHairLines = true;
		};
		options.m_BeforeFrame = [state](gtSceneBuilder& io_Builder)
		{
			g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();
			switch (state->m_Frame++)
			{
				case 1: p.m_bUseHardwareTessellation = false; break;
				case 2: p.m_bHairLines = false; break;
				case 3:
					p.m_HairShadowType = HAIR_SHADOW_OSM16;
					io_Builder.HairShadows(state->m_pLight, HAIR_SHADOW_OSM16, c_OpacitySize);
					break;
				default: break;
			}
			io_Builder.RenderTargets();
		};
		cases.push_back({ "frame" + std::to_string(frames), opacity_scene(setup, state), options });
	}
	gtExpectSameImages("HairPrefsChangeBetweenFramesMatchesDX11", cases);
}
