/*****************************************************************************
**  shdwScreenPassesVkTest.cpp
**
**      The HDR renderer's screen-space and post passes against
**      GraphicsDX11's, both through the creator: SSAO (shdwPassSSAO: blur,
**      depth peeling, overscan, and the depth buffer it builds, HDR debug
**      mode 10), glow (shdwPassGlow: lit specular glow and constant glow,
**      with a glow mask), outlines (shdwPassOutline: depth silhouettes and
**      normal creases), the post effects (shdwPassPostShader through
**      g3dPostProcessing), matte rendering (m_bRenderMatte: alpha out,
**      bloom and star as grey, glow skipped), and all of them at once with
**      transparency (Review Focus 2: scratch targets read and written
**      within one frame).
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
#include "gtTempDir.hpp"

#ifndef G3D_POSTPROCESSING_HPP
#include "Graphics/G3d/g3dPostProcessing.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/Mat/matShaderMgr.hpp"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <functional>
#include <string>
#include <vector>

namespace
{
	//	A backdrop plane and four spheres resting against it, two of them
	//	overlapping (a second depth layer for SSAO's peeling), lit by shadow-
	//	casting lights. i_pEdit, if given, edits each sphere's material.
	void spheres_on_backdrop(gtSceneBuilder& io_Builder, const std::function<void(int, matMaterial*)>& i_Edit = nullptr)
	{
		io_Builder.LightsCastShadows(true);
		matMaterial* back = io_Builder.Keep(gtScene::ShaderMaterial("Lambert.fx", gtScene::e_Tinted));
		io_Builder.Add(gtScene::Plane(), back, gtScene::Place(0, 0, -1.0f, 2.6f));

		const gtMesh sphere = gtScene::Sphere(16, 32);
		const char* shaders[2] = { "Blinn.fx", "Lambert.fx" };
		const float x[4] = { -1.3f, -0.2f, 0.35f, 1.4f };
		const float y[4] = { 0.5f, -0.3f, 0.1f, -0.7f };
		const float z[4] = { -0.45f, -0.4f, 0.4f, -0.5f };
		for (int i = 0; i < 4; i++)
		{
			matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(shaders[i % 2], gtScene::e_Tinted));
			if (i_Edit)
				i_Edit(i, material);
			io_Builder.Add(sphere, material, gtScene::Place(x[i], y[i], z[i], 0.55f));
		}
		io_Builder.TwoLights();
	}

	//	The scene's SSAO settings: a radius that suits the scene's size, and
	//	i_Overscan border pixels
	void ssao_settings(gtSceneBuilder& io_Builder, int i_Overscan)
	{
		ssaoParams params;
		io_Builder.Scene().GetSSAOSettings(params);
		params.m_AORadius = 0.8f;
		params.m_AORadiusFar = 1.6f;
		params.m_OverscanPixels = i_Overscan;
		io_Builder.Scene().SetSSAOSettings(params);
	}

	gtSceneFunction ssao_scene(int i_Overscan)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path&)
		{
			spheres_on_backdrop(io_Builder);
			ssao_settings(io_Builder, i_Overscan);
		};
	}

	gtRenderOptions ssao_prefs(bool i_bBlur, bool i_bPeel, int i_DebugMode = 0)
	{
		gtRenderOptions options;
		options.m_Prefs = [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bEnableSSAO = true;
			p.m_bEnableSSAOBlur = i_bBlur;
			p.m_bEnableSSAODepthPeeling = i_bPeel;
			p.m_SSAONumLayers = i_bPeel ? 3 : 1;
			p.m_HDRDebugMode = i_DebugMode;
		};
		return options;
	}

	//	Glow on sphere i of the backdrop scene: lit (specular) glow, or
	//	constant glow; masked by a checker texture
	void set_glow(matMaterial* io_pMaterial, matTexture* i_pMask, bool i_bConstant)
	{
		io_pMaterial->SetHasGlow(true);
		effGlowData& glow = io_pMaterial->GlowData();
		glow.m_GlowAmount = 4.0f;
		glow.m_GlowScale = maVector4d(1.0f, 0.6f, 0.3f, 1.0f);
		glow.m_GlowSize = 4.0f;
		glow.m_bConstantGlow = i_bConstant;
		glow.m_pGlowMask = i_pMask;
	}

	gtSceneFunction glow_scene(bool i_bConstant)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			matTexture* mask = io_Builder.Keep(gtScene::CheckerTexture(i_Dir, 64, 64, 4));
			spheres_on_backdrop(io_Builder, [=](int i_Index, matMaterial* io_pMaterial)
			{
				if (i_Index == 0 || i_Index == 2)
					set_glow(io_pMaterial, mask, i_bConstant);
			});
		};
	}

	//	Normal outlines: drawn where neighbouring normals are i_MinAngle to
	//	150 degrees apart (a sphere's rim against the cleared buffer's +Z),
	//	i_Width to twice that wide. Depth outlines (m_bUseDepths, on by
	//	default) are left out: shdwPassOutline asks DepthRender.fx for
	//	"WorldSpaceNormal", which it lacks, and both backends assert there.
	void set_outline(matMaterial* io_pMaterial, float i_MinAngle, float i_Width)
	{
		io_pMaterial->SetHasOutline(true);
		effOutlineData& outline = io_pMaterial->OutlineData();
		outline.m_bUseDepths = false;
		outline.m_bUseNormals = true;
		outline.m_OutlineColor = maFloatRGBA(1.0f, 0.9f, 0.2f, 1.0f);
		outline.m_OutlineThickness = 2.0f;
		outline.m_OutlineMinWidth = i_Width;
		outline.m_OutlineMaxWidth = 2 * i_Width;
		outline.m_OutlineMinAngle = i_MinAngle;
		outline.m_OutlineMaxAngle = 150.0f;
	}

	//	Outlines on spheres 1 to 3, each with its own angle and width
	void outline_scene(gtSceneBuilder& io_Builder, const std::filesystem::path&)
	{
		spheres_on_backdrop(io_Builder, [](int i_Index, matMaterial* io_pMaterial)
		{
			if (i_Index == 1)
				set_outline(io_pMaterial, 30.0f, 2.0f);
			if (i_Index == 2)
				set_outline(io_pMaterial, 60.0f, 1.0f);
			if (i_Index == 3)
				set_outline(io_pMaterial, 20.0f, 3.0f);
		});
	}

	//	The post effect PostEffect/<i_Name>.fx made active, as
	//	pfxPostEffectObject does; texture parameters (GradientMap's
	//	gradient) get a smooth ramp. gtPrefsScope restores
	//	g3dPostProcessing after the render.
	void set_post_effect(gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir, const std::string& i_Name)
	{
		const itString file((i_Name + ".fx").c_str());
		matShaderEffect* effect = matShaderMgr::GetPostEffect(fsLocator(file));
		ASSERT_TRUE(effect != NULL) << i_Name;
		shared_ptr<effShaderParams> params(new effShaderParams());
		params->SetShaderName(file, effect);
		effect->BuildPrtyObject(params.get());
		std::vector<effParamTexture*> textures;
		params->GetAllTextureParams(textures);
		if (!textures.empty())
		{
			matTexture* gradient = io_Builder.Keep(gtScene::RampTexture(512));
			for (size_t i = 0; i < textures.size(); i++)
				textures[i]->SetTexture(gradient);
			if (effParamBool* has = dynamic_cast<effParamBool*>(params->FindParam("hasGradientMap")))
				has->Property().SetValue(true);
		}
		g3dPostProcessing::SetPostEffect(params);
		g3dPostProcessing::SetActive(true);
	}

	gtSceneFunction post_scene(const std::string& i_Name)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			spheres_on_backdrop(io_Builder);
			set_post_effect(io_Builder, i_Dir, i_Name);
		};
	}

	//	A translucent Phong sphere in front of the others
	void add_translucent(gtSceneBuilder& io_Builder)
	{
		matMaterial* material = io_Builder.Keep(gtScene::DataMaterial("Phong.fx", [](effShaderData* io_pData)
		{
			gtScene::SetPhong(io_pData, gtScene::e_Tinted);
			if (effPhongData* phong = dynamic_cast<effPhongData*>(io_pData))
			{
				phong->m_ColorDiffuse = maFloatRGBA(0.2f, 0.5f, 0.6f, 1);
				phong->m_Transparency = 0.5f;
			}
		}));
		io_Builder.Add(gtScene::Sphere(16, 32), material, gtScene::Place(-0.5f, -0.1f, 0.9f, 0.5f));
	}

	//	Glow on spheres 0 and 2, outlines on 1 and 3, SSAO settings, a post
	//	effect and a translucent sphere
	void everything_scene(gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		matTexture* mask = io_Builder.Keep(gtScene::CheckerTexture(i_Dir, 64, 64, 4));
		spheres_on_backdrop(io_Builder, [=](int i_Index, matMaterial* io_pMaterial)
		{
			if (i_Index == 0)
				set_glow(io_pMaterial, mask, false);
			if (i_Index == 2)
				set_glow(io_pMaterial, mask, true);
			if (i_Index == 1)
				set_outline(io_pMaterial, 30.0f, 2.0f);
			if (i_Index == 3)
				set_outline(io_pMaterial, 20.0f, 3.0f);
		});
		add_translucent(io_Builder);
		ssao_settings(io_Builder, 16);
		set_post_effect(io_Builder, i_Dir, "Sepia");
	}

	gtRenderOptions everything_prefs(bool i_bMatte, bool i_bAA)
	{
		gtRenderOptions options;
		options.m_Prefs = [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bEnableSSAO = true;
			p.m_bEnableSSAOBlur = true;
			p.m_bEnableSSAODepthPeeling = true;
			p.m_SSAONumLayers = 2;
			p.m_bEnableGlow = true;
			p.m_bEnableOutline = true;
			p.m_bEnableTransparent = true;
			p.m_bRenderMatte = i_bMatte;
			p.m_bHDRAA = i_bAA;
		};
		return options;
	}
}

TEST(shdwScreenPassesVkTest, SSAOMatchDX11)
{
	//	mode 10 shows the peeled view-space depths (SSAO's multi-depth
	//	buffer): the camera moves close so most of them are under 1
	gtSceneFunction close = [](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		ssao_scene(0)(io_Builder, i_Dir);
		io_Builder.Camera().LookAt(maPoint3d(0.0f, 0.0f, 1.2f), maPoint3d(0, 0, 0), maVector3d(0, 1, 0));
	};
	gtRenderOptions mode10 = ssao_prefs(false, true, 10);
	mode10.m_bCheckCoverage = false;
	gtExpectSameImages("SSAOMatchDX11",
	{
		{ "plain", ssao_scene(0), ssao_prefs(false, false) },
		{ "blur", ssao_scene(0), ssao_prefs(true, false) },
		{ "peeled", ssao_scene(0), ssao_prefs(false, true) },
		{ "peeled_blur", ssao_scene(0), ssao_prefs(true, true) },
		{ "overscan16", ssao_scene(16), ssao_prefs(true, false) },
		{ "overscan16_peeled", ssao_scene(16), ssao_prefs(false, true) },
		{ "debug_mode10", close, mode10 },
	});

	gtRenderOptions off = ssao_prefs(false, false);
	off.m_Prefs = nullptr;
	gtExpectEffectShows("SSAOMatchDX11", ssao_scene(0), ssao_prefs(true, false), ssao_scene(0), off, 256 * 256 / 50);
	gtRenderOptions mode10_layer1 = ssao_prefs(false, false, 10);
	gtExpectEffectShows("SSAOMatchDX11/debug_mode10_peeled", close, mode10, close, mode10_layer1, 256 * 256 / 100);
}

TEST(shdwScreenPassesVkTest, GlowMatchDX11)
{
	gtRenderOptions options;
	options.m_Prefs = [](g3dPrefs::g3dRenderPrefs& p) { p.m_bEnableGlow = true; };
	gtExpectSameImages("GlowMatchDX11",
	{
		{ "specular", glow_scene(false), options },
		{ "constant", glow_scene(true), options },
	});

	gtRenderOptions off;
	off.m_Prefs = [](g3dPrefs::g3dRenderPrefs& p) { p.m_bEnableGlow = false; };
	gtExpectEffectShows("GlowMatchDX11/specular", glow_scene(false), options, glow_scene(false), off, 256 * 256 / 100);
	gtExpectEffectShows("GlowMatchDX11/constant", glow_scene(true), options, glow_scene(true), off, 256 * 256 / 100);
}

TEST(shdwScreenPassesVkTest, OutlineMatchDX11)
{
	gtRenderOptions options;
	options.m_Prefs = [](g3dPrefs::g3dRenderPrefs& p) { p.m_bEnableOutline = true; };
	gtExpectSameImage("OutlineMatchDX11", outline_scene, options);

	gtRenderOptions off;
	off.m_Prefs = [](g3dPrefs::g3dRenderPrefs& p) { p.m_bEnableOutline = false; };
	gtExpectEffectShows("OutlineMatchDX11", outline_scene, options, outline_scene, off, 256);
}

TEST(shdwScreenPassesVkTest, PostEffectsMatchDX11)
{
	const char* effects[4] = { "BlackAndWhite", "GradientMap", "Sepia", "Sketch" };
	std::vector<gtCase> cases;
	for (const char* effect : effects)
		cases.push_back({ effect, post_scene(effect), gtRenderOptions() });
	gtExpectSameImages("PostEffectsMatchDX11", cases);

	for (const char* effect : effects)
	{
		gtExpectEffectShows((std::string("PostEffectsMatchDX11/") + effect).c_str(), post_scene(effect), gtRenderOptions(),
							[](gtSceneBuilder& io_Builder, const std::filesystem::path&) { spheres_on_backdrop(io_Builder); },
							gtRenderOptions(), 256 * 256 / 20);
	}
}

TEST(shdwScreenPassesVkTest, MatteMatchesDX11)
{
	gtRenderOptions options;
	options.m_Prefs = [](g3dPrefs::g3dRenderPrefs& p)
	{
		p.m_bRenderMatte = true;
		p.m_bEnableGlow = true;
	};
	gtExpectSameImage("MatteMatchesDX11", glow_scene(false), options);

	gtRenderOptions off;
	off.m_Prefs = [](g3dPrefs::g3dRenderPrefs& p) { p.m_bEnableGlow = true; };
	gtExpectEffectShows("MatteMatchesDX11", glow_scene(false), options, glow_scene(false), off, 256 * 256 / 20);
}

TEST(shdwScreenPassesVkTest, EveryPostEffectAtOnceMatchesDX11)
{
	gtExpectSameImages("EveryPostEffectAtOnceMatchesDX11",
	{
		{ "all", everything_scene, everything_prefs(false, false) },
		{ "all_aa", everything_scene, everything_prefs(false, true) },
		{ "all_matte", everything_scene, everything_prefs(true, false) },
	});

	gtExpectEffectShows("EveryPostEffectAtOnceMatchesDX11", everything_scene, everything_prefs(false, false),
						[](gtSceneBuilder& io_Builder, const std::filesystem::path&) { spheres_on_backdrop(io_Builder); },
						gtRenderOptions(), 256 * 256 / 20);
}
