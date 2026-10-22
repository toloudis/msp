/*****************************************************************************
**  shdwDofFogVkTest.cpp
**
**      Depth of field (shdwPassDOF and the HDR renderer's RenderDOF: the
**      blurriness written into the HDR target's alpha, the blurred copy,
**      g3dRenderDOF) and fog (shdwPassFog: view-space depths, then the
**      fog effect over a copy of the HDR target) against GraphicsDX11's,
**      both through the creator. Review Focus 1 (DOF at its limits) and 2
**      (fog over the cleared background, and with SSAO, which also builds
**      and clears DepthBuffer()).
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

#ifndef CAM_CAMERA_HPP
#include "Graphics/Cam/camCamera.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
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
	//	Five spheres at view depths 3, 4.5, 6, 7.5 and 9 (the camera is at
	//	z = 6 looking down -Z), spread across the view, in front of a
	//	backdrop at depth 10, lit by shadow-casting lights; i_bBackdrop
	//	false leaves the cleared background behind them
	void spheres_in_depth(gtSceneBuilder& io_Builder, bool i_bBackdrop = true)
	{
		io_Builder.LightsCastShadows(true);
		if (i_bBackdrop)
		{
			matMaterial* back = io_Builder.Keep(gtScene::ShaderMaterial("Lambert.fx", gtScene::e_Tinted));
			io_Builder.Add(gtScene::Plane(), back, gtScene::Place(0, 0, -4.0f, 5.0f));
		}

		const gtMesh sphere = gtScene::Sphere(16, 32);
		const char* shaders[2] = { "Blinn.fx", "Lambert.fx" };
		const float x[5] = { -1.0f, -0.6f, 0.15f, 1.0f, 1.9f };
		const float y[5] = { 0.45f, -0.6f, 0.5f, -0.5f, 0.6f };
		const float z[5] = { 3.0f, 1.5f, 0.0f, -1.5f, -3.0f };
		for (int i = 0; i < 5; i++)
		{
			matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(shaders[i % 2], gtScene::e_Tinted));
			io_Builder.Add(sphere, material, gtScene::Place(x[i], y[i], z[i], 0.45f + 0.1f * i));
		}
		io_Builder.TwoLights();
	}

	//	The camera's depth of field, as cmraCameraObject sets it
	camDOFData dof(float i_NearBlur, float i_NearFocal, float i_FarFocal, float i_FarBlur,
				   float i_MaxFarBlur = 1.0f, float i_MaxCoC = 5.0f)
	{
		camDOFData data;
		data.m_NearBlurDist = i_NearBlur;
		data.m_NearFocalDist = i_NearFocal;
		data.m_FarFocalDist = i_FarFocal;
		data.m_FarBlurDist = i_FarBlur;
		data.m_MaxFarBlur = i_MaxFarBlur;
		data.m_MaxCoC = i_MaxCoC;
		data.m_bEnableDOF = true;
		return data;
	}

	//	Near blur from depth 4.5 in, far blur from 7.5 out: the spheres at
	//	3 and 9 are the blurriest, the one at 6 is in focus
	camDOFData usual_dof()
	{
		return dof(2.5f, 5.0f, 7.0f, 10.0f);
	}

	gtSceneFunction dof_scene(const camDOFData& i_DOF)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path&)
		{
			spheres_in_depth(io_Builder);
			io_Builder.Camera().SetDOFParams(i_DOF);
		};
	}

	gtRenderOptions dof_prefs(bool i_bDOF = true, int i_DebugMode = 0, bool i_bAA = true)
	{
		gtRenderOptions options;
		options.m_Prefs = [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bEnableDOF = i_bDOF;
			p.m_HDRDebugMode = i_DebugMode;
			p.m_bHDRAA = i_bAA;
		};
		return options;
	}

	//	The scene's fog. Depth fog from 4 to 12 (the spheres and the
	//	backdrop are inside it), altitude fog from -1.5 to 1.5 along the
	//	fog's orientation, a blue-grey colour
	fogParams fog(g3dType::FogMode i_Mode, bool i_bUseWorld = false)
	{
		fogParams params;
		params.m_nMode = i_Mode;
		params.m_Color = maFloatRGBA(0.55f, 0.65f, 0.8f, 1.0f);
		params.m_bUseWorld = i_bUseWorld;
		params.m_fStart = 4.0f;
		params.m_fEnd = 12.0f;
		params.m_fDensity = (i_Mode == g3dType::e_FogModeExp2) ? 1.4f : 2.0f;
		params.m_fAltitudeStart = -1.5f;
		params.m_fAltitudeEnd = 1.5f;
		params.m_fAltitudeDensity = (i_Mode == g3dType::e_FogModeExp2) ? 1.1f : 1.5f;
		return params;
	}

	//	The camera above the scene, looking down at it (the altitude fog
	//	then runs across the view at a slant), rolled when i_bRoll so its up
	//	vector (the fog's orientation with m_bUseWorld) isn't the world's
	void tilt_camera(gtSceneBuilder& io_Builder, bool i_bRoll)
	{
		const maVector3d up = i_bRoll ? maVector3d(0.45f, 1, 0) : maVector3d(0, 1, 0);
		io_Builder.Camera().LookAt(maPoint3d(0.0f, 2.5f, 6.0f), maPoint3d(0, 0, 0), up);
	}

	gtSceneFunction fog_scene(const fogParams& i_Fog, bool i_bBackdrop = true, bool i_bRoll = false)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path&)
		{
			spheres_in_depth(io_Builder, i_bBackdrop);
			tilt_camera(io_Builder, i_bRoll);
			io_Builder.Scene().SetFog(i_Fog);
		};
	}

	//	The fog scene with SSAO settings that suit its size
	gtSceneFunction fog_ssao_scene(const fogParams& i_Fog)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			fog_scene(i_Fog)(io_Builder, i_Dir);
			ssaoParams params;
			io_Builder.Scene().GetSSAOSettings(params);
			params.m_AORadius = 0.8f;
			params.m_AORadiusFar = 1.6f;
			io_Builder.Scene().SetSSAOSettings(params);
		};
	}

	gtRenderOptions ssao_prefs(bool i_bPeel, bool i_bAA)
	{
		gtRenderOptions options;
		options.m_Prefs = [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bEnableSSAO = true;
			p.m_bEnableSSAOBlur = true;
			p.m_bEnableSSAODepthPeeling = i_bPeel;
			p.m_SSAONumLayers = i_bPeel ? 3 : 1;
			p.m_bHDRAA = i_bAA;
		};
		return options;
	}

	gtSceneFunction no_fog_scene(bool i_bBackdrop = true)
	{
		return fog_scene(fogParams(), i_bBackdrop);
	}
}

TEST(shdwDofFogVkTest, DOFMatchDX11)
{
	gtExpectSameImages("DOFMatchDX11",
	{
		{ "near_and_far", dof_scene(usual_dof()), dof_prefs() },
		{ "near_and_far_noaa", dof_scene(usual_dof()), dof_prefs(true, 0, false) },
		{ "half_far_blur", dof_scene(dof(2.5f, 5.0f, 7.0f, 10.0f, 0.5f, 3.0f)), dof_prefs() },
	});

	gtExpectEffectShows("DOFMatchDX11", dof_scene(usual_dof()), dof_prefs(),
						dof_scene(usual_dof()), dof_prefs(false), 256 * 256 / 50);
}

//	Review Focus 1
TEST(shdwDofFogVkTest, DOFLimitsMatchDX11)
{
	gtExpectSameImages("DOFLimitsMatchDX11",
	{
		//	every distance negative: all the scene is past the far blur
		{ "behind_camera", dof_scene(dof(-4.0f, -3.0f, -2.0f, -1.0f)), dof_prefs() },
		//	one focal plane, and blur distances equal to it (zero ranges)
		{ "near_equals_far", dof_scene(dof(2.5f, 6.0f, 6.0f, 10.0f)), dof_prefs() },
		{ "zero_ranges", dof_scene(dof(6.0f, 6.0f, 6.0f, 6.0f)), dof_prefs() },
		//	no far blur; no blur at all
		{ "max_far_blur_0", dof_scene(dof(2.5f, 5.0f, 7.0f, 10.0f, 0.0f)), dof_prefs() },
		{ "max_coc_0", dof_scene(dof(2.5f, 5.0f, 7.0f, 10.0f, 1.0f, 0.0f)), dof_prefs() },
		//	DOF skipped
		{ "max_far_blur_minus_1", dof_scene(camDOFData()), dof_prefs() },
	});
}

TEST(shdwDofFogVkTest, DOFDebugModeMatchesDX11)
{
	gtRenderOptions mode4 = dof_prefs(true, 4);
	gtExpectSameImages("DOFDebugModeMatchesDX11",
	{
		{ "mode4", dof_scene(usual_dof()), mode4 },
		{ "mode4_noaa", dof_scene(usual_dof()), dof_prefs(true, 4, false) },
	});

	gtExpectEffectShows("DOFDebugModeMatchesDX11", dof_scene(usual_dof()), mode4,
						dof_scene(usual_dof()), dof_prefs(false, 4), 256 * 256 / 50);
}

TEST(shdwDofFogVkTest, FogMatchDX11)
{
	gtExpectSameImages("FogMatchDX11",
	{
		{ "linear", fog_scene(fog(g3dType::e_FogModeLinear)), gtRenderOptions() },
		{ "exp", fog_scene(fog(g3dType::e_FogModeExp)), gtRenderOptions() },
		{ "exp2", fog_scene(fog(g3dType::e_FogModeExp2)), gtRenderOptions() },
		//	oriented along the rolled camera's up vector
		{ "linear_world", fog_scene(fog(g3dType::e_FogModeLinear, true), true, true), gtRenderOptions() },
		{ "exp_world", fog_scene(fog(g3dType::e_FogModeExp, true), true, true), gtRenderOptions() },
	});

	const g3dType::FogMode modes[3] = { g3dType::e_FogModeLinear, g3dType::e_FogModeExp, g3dType::e_FogModeExp2 };
	for (g3dType::FogMode mode : modes)
	{
		gtExpectEffectShows(("FogMatchDX11/mode" + std::to_string((int)mode)).c_str(), fog_scene(fog(mode)), gtRenderOptions(),
							no_fog_scene(), gtRenderOptions(), 256 * 256 / 20);
	}
}

//	Review Focus 2: the fog's depth buffer is cleared to 1, and nothing is
//	drawn over the background
TEST(shdwDofFogVkTest, FogOverBackgroundMatchesDX11)
{
	gtExpectSameImages("FogOverBackgroundMatchesDX11",
	{
		{ "linear", fog_scene(fog(g3dType::e_FogModeLinear), false), gtRenderOptions() },
		{ "exp", fog_scene(fog(g3dType::e_FogModeExp), false), gtRenderOptions() },
		{ "exp_noaa", fog_scene(fog(g3dType::e_FogModeExp), false), dof_prefs(false, 0, false) },
	});

	gtExpectEffectShows("FogOverBackgroundMatchesDX11", fog_scene(fog(g3dType::e_FogModeExp), false), gtRenderOptions(),
						no_fog_scene(false), gtRenderOptions(), 256 * 256 / 20);
}

//	Review Focus 2: SSAO builds its depths in DepthBuffer() too
TEST(shdwDofFogVkTest, FogWithSSAOMatchesDX11)
{
	gtExpectSameImages("FogWithSSAOMatchesDX11",
	{
		{ "ssao", fog_ssao_scene(fog(g3dType::e_FogModeLinear)), ssao_prefs(false, true) },
		{ "ssao_peeled", fog_ssao_scene(fog(g3dType::e_FogModeExp)), ssao_prefs(true, true) },
		{ "ssao_noaa", fog_ssao_scene(fog(g3dType::e_FogModeExp2)), ssao_prefs(false, false) },
	});

	gtExpectEffectShows("FogWithSSAOMatchesDX11", fog_ssao_scene(fog(g3dType::e_FogModeLinear)), ssao_prefs(false, true),
						fog_ssao_scene(fogParams()), ssao_prefs(false, true), 256 * 256 / 20);
}
