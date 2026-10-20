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
**      the creator.
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

#include <gtest/gtest.h>

#include <filesystem>
#include <functional>
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
