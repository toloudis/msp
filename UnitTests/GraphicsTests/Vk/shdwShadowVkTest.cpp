/*****************************************************************************
**  shdwShadowVkTest.cpp
**
**      Shadow maps on Vulkan against DX11: a projected light's shadow map
**      (matTextureMgr::CreateShadowMap, an R32_FLOAT colour target and a
**      depth buffer) rendered by the depth map renderer the creator makes
**      (g3dSceneRendererCreate::CreateDepthMapRenderer) through a
**      g3dTargetRenderer from the light's shadow camera, as
**      prjltProjectedLightObject sets it up; the map's depths read back
**      and compared texel by texel, and the HDR renderer's image of a
**      scene the light shadows, compared with gfxcompare's tolerance.
**
**      The scene: spheres in front of a wall, lit by a projected light
**      from the front, so their shadows fall on the wall. Shadow dithering
**      (a fragment's GetShadowDithering: the depth map shader adds its
**      bias to the alpha threshold, dropping translucent texels) is
**      checked on the depths with a translucent sphere; the images keep
**      the spheres opaque (translucent materials are drawn by the
**      transparent pass, ported in Task 4) and only run the dithered path.
**      A check on DX11 alone makes sure the shadows show in the images.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/G3d/g3dProjectedLight.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace
{
	const int c_MapSize = 1024;

	//	The dither bias (DepthMap.hlsl's documented default): added to the
	//	alpha threshold, so a translucent caster (alpha 0.7) is dropped
	const float c_DitherBias = 0.85f;

	//	Casts and receives shadows (fragments do neither by default; a
	//	MachStudio object's fragment properties set both)
	void casts_shadow(g3dSceneNode* io_pNode, bool i_bDither)
	{
		g3dFragment* fragment = const_cast<g3dFragment*>(io_pNode->GetFragment());
		fragment->SetCastsShadow(true);
		fragment->SetReceivesShadow(true);
		fragment->SetShadowDithering(i_bDither, c_DitherBias);
	}

	//	Three spheres in front of a wall, lit by a shadowed projected light
	//	from the front; i_bDither: the spheres' shadows dithered;
	//	i_bTranslucent: the middle sphere a translucent Phong (depths only)
	g3dProjectedLight* shadow_scene(gtSceneBuilder& io_Builder, int i_MapSize, bool i_bDither, bool i_bTranslucent)
	{
		const gtMesh sphere = gtScene::Sphere(16, 32);
		matMaterial* wall = io_Builder.Keep(gtScene::ShaderMaterial("Lambert.fx", gtScene::e_Defaults));
		casts_shadow(io_Builder.Add(gtScene::Plane(), wall, gtScene::Place(0, 0, -1.5f, 4.0f)), false);
		const gtScene::Values values[3] = { gtScene::e_Defaults, gtScene::e_Tinted, gtScene::e_Defaults };
		const char* shaders[3] = { "Blinn.fx", "Phong.fx", "Lambert.fx" };
		for (int i = 0; i < 3; i++)
		{
			matMaterial* material = (i == 1 && i_bTranslucent)
				? gtScene::DataMaterial("Phong.fx", [](effShaderData* i_pData) { gtScene::SetPhong(i_pData, gtScene::e_NonDefault); })
				: gtScene::ShaderMaterial(shaders[i], values[i]);
			io_Builder.Keep(material);
			casts_shadow(io_Builder.Add(sphere, material, gtScene::Place(-1.3f + 1.3f * i, 0.4f - 0.4f * i, 0, 0.5f)), i_bDither);
		}
		return io_Builder.ProjectedLight(maPoint3d(1.5f, 2.5f, 7.0f), maPoint3d(0, 0, -1.5f), maFloatRGBA(1.0f, 0.95f, 0.85f, 1), i_MapSize);
	}

	gtSceneFunction shadow_scene_function(int i_MapSize, bool i_bDither)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path&)
		{
			shadow_scene(io_Builder, i_MapSize, i_bDither, false);
		};
	}

	gtRenderOptions render_targets()
	{
		gtRenderOptions options;
		options.m_BeforeFrame = [](gtSceneBuilder& io_Builder) { io_Builder.RenderTargets(); };
		return options;
	}

	//	The shadow map's depths after RenderTargets, on the backend that is up
	std::vector<float> map_depths(bool i_bDither, bool i_bTranslucent)
	{
		gtPrefsScope prefs;
		gtSceneBuilder builder;
		g3dProjectedLight* light = shadow_scene(builder, c_MapSize, i_bDither, i_bTranslucent);
		builder.RenderTargets();
		const std::vector<uint8_t> bytes = gtReadTexture(gtBackend::Current(), light->GetShadowMap(), 4);
		std::vector<float> depths(bytes.size() / 4);
		if (!depths.empty())
			memcpy(depths.data(), bytes.data(), bytes.size());
		return depths;
	}
}

TEST(shdwShadowVkTest, ShadowMapDepthsMatchDX11)
{
	const char* names[3] = { "plain", "dithered", "dithered_translucent" };
	const bool dither[3] = { false, true, true };
	const bool translucent[3] = { false, false, true };
	std::vector<float> dx11[3], vulkan[3];
	gtBackend::Run(gtBackend::e_DX11, [&]()
	{
		for (int i = 0; i < 3; i++)
			dx11[i] = map_depths(dither[i], translucent[i]);
	});
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]()
	{
		for (int i = 0; i < 3; i++)
			vulkan[i] = map_depths(dither[i], translucent[i]);
	}))
		return;

	for (int i = 0; i < 3; i++)
	{
		SCOPED_TRACE(names[i]);
		ASSERT_EQ((size_t)c_MapSize * c_MapSize, dx11[i].size());
		ASSERT_EQ(dx11[i].size(), vulkan[i].size());
		size_t differing = 0, covered = 0;
		float max_err = 0;
		for (size_t t = 0; t < dx11[i].size(); t++)
		{
			const float err = std::fabs(dx11[i][t] - vulkan[i][t]);
			max_err = std::max(max_err, err);
			differing += !(err <= 1e-6f);		// NaN counts
			covered += dx11[i][t] < 1.0f;
		}
		std::cout << "  " << names[i] << ": max error " << max_err << ", " << differing << " texels over 1e-6, "
				  << covered << " covered" << std::endl;
		EXPECT_EQ(0u, differing);
		//	the scene fills a good part of the map (the cleared texels hold 1)
		EXPECT_GT(covered, dx11[i].size() / 20);
	}
	//	the translucent sphere's shadow is dithered: its depths differ from
	//	the opaque one's
	EXPECT_NE(dx11[1], dx11[2]);
}

TEST(shdwShadowVkTest, ProjectedLightShadowsMatchDX11)
{
	gtRenderOptions cull_off = render_targets();
	cull_off.m_Prefs = [](g3dPrefs::g3dRenderPrefs& io_Prefs) { io_Prefs.m_bProjLightFrustumCull = false; };
	gtRenderOptions cull_on = render_targets();
	cull_on.m_Prefs = [](g3dPrefs::g3dRenderPrefs& io_Prefs) { io_Prefs.m_bProjLightFrustumCull = true; };
	gtExpectSameImages("ProjectedLightShadowsMatchDX11",
	{
		{ "plain", shadow_scene_function(c_MapSize, false), cull_off },
		{ "dithered", shadow_scene_function(c_MapSize, true), cull_off },
		{ "frustum_cull", shadow_scene_function(c_MapSize, false), cull_on },
	});

	//	the shadows show: on DX11, the image differs from the light's without
	//	a shadow map in a good part of the wall
	gtPicture shadowed, unshadowed;
	gtBackend::Run(gtBackend::e_DX11, [&]()
	{
		shadowed = gtRenderScene(shadow_scene_function(c_MapSize, false), std::filesystem::path(), cull_off);
		unshadowed = gtRenderScene(shadow_scene_function(0, false), std::filesystem::path(), cull_off);
	});
	ASSERT_TRUE(shadowed.Valid() && unshadowed.Valid());
	int differing = 0;
	for (size_t i = 0; i < shadowed.m_Rgba.size(); i += 4)
	{
		int diff = 0;
		for (int c = 0; c < 3; c++)
			diff = std::max(diff, std::abs((int)shadowed.m_Rgba[i + c] - (int)unshadowed.m_Rgba[i + c]));
		differing += diff > 8;
	}
	std::cout << "  " << differing << " pixels shadowed" << std::endl;
	EXPECT_GT(differing, shadowed.m_Width * shadowed.m_Height / 50) << "images (shadowed as dx11, unshadowed as vulkan) in "
		<< gtSaveComparison("ProjectedLightShadowsMatchDX11/shadows_show", shadowed.m_Rgba.data(), unshadowed.m_Rgba.data(), shadowed.m_Width, shadowed.m_Height);
}

TEST(shdwShadowVkTest, ShadowMapNeverRenderedMatchesDX11)
{
	//	the map is made and given to the light, but RenderTargets isn't run:
	//	DX11's new map reads as zeros (Vulkan clears its map to match), so
	//	everything the light reaches is in shadow and the image is mostly
	//	the ambient term (dark: no coverage check)
	gtRenderOptions options;
	options.m_bCheckCoverage = false;
	gtExpectSameImage("ShadowMapNeverRenderedMatchesDX11", shadow_scene_function(c_MapSize, false), options);
}
