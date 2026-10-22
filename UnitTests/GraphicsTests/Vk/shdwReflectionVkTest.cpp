/*****************************************************************************
**  shdwReflectionVkTest.cpp
**
**      The simple renderer (shdwShadowLayerRendererVk, from
**      g3dSceneRendererCreate::CreateSimpleRenderer) and the reflection
**      renderers built on it on Vulkan against DX11:
**      - the simple renderer's image of lit spheres under the global
**        environment, with overlay and viewer layers, single-light
**        rendering off, and matte;
**      - a planar reflection (a reflective floor under spheres) and a
**        cube map reflection (a reflective sphere among others), made as
**        mtrlScriptObject makes them (gtSceneBuilder::Reflective) and
**        rendered by their target renderers before the view
**        (RenderTargets), then the view drawn by the HDR renderer; every
**        layer and mip of the reflection maps is compared too, for a cube
**        map made once and one made every frame (a sphere moves between
**        the two frames, so the two differ);
**      - several reflectors at once: a planar floor, two cube-mapped
**        spheres that see each other and a planar reflector behind the
**        camera. Each backend's GPU time per reflection renderer is
**        printed (no assertion) when the profiling markers are compiled
**        in (Debug, or Release with MspGfxProfiling=true).
**
**      Checks on DX11 alone make sure the maps show the scene and the
**      reflections show in the view (they didn't from the plain-HLSL
**      conversion until fxMaterialInstance left the renderer's variables
**      to the renderer: restoring a material's parameter block undid the
**      reflection flags SetupMaterial had set).
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"
#include "gtTempDir.hpp"

#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef EFF_REFLDATA_HPP
#include "Graphics/Eff/effReflData.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace
{
	const float c_Pi = 3.14159265f;

	//	Plan 4b's twelve spheres, lit by shadow-casting lights
	void spheres(gtSceneBuilder& io_Builder)
	{
		io_Builder.LightsCastShadows(true);
		io_Builder.LitSpheres(true, true);
	}

	//	The global environment (diffuse and specular lat-long maps), not
	//	drawn as the background; a dim sun, so the lit surfaces don't
	//	saturate (the simple renderer doesn't tone map) and hide what they
	//	reflect
	void environment(gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		matTexture* diffuse = io_Builder.Keep(gtScene::HdrTexture(i_Dir, 128, 64, 2.0f));
		matTexture* specular = io_Builder.Keep(gtScene::HdrTexture(i_Dir, 256, 128, 2.0f));
		io_Builder.GlobalEnvironment(diffuse, specular, false);
	}

	gtRenderOptions simple_renderer()
	{
		gtRenderOptions options;
		options.m_CreateRenderer = []() { return g3dSceneRendererCreate::CreateSimpleRenderer(); };
		return options;
	}

	//	A horizontal square at height i_Y facing up (Plane faces +Z)
	maMatrix4x4 floor_at(float i_Y, float i_Scale)
	{
		maMatrix4x4 m;
		m.MakeRotate(maVector3d(0, 0, 1), maVector3d(0, 1, 0));
		m.ScaleBy(i_Scale, i_Scale, i_Scale);
		m.TranslateBy(0, i_Y, 0);
		return m;
	}

	//	A BlinnReflection material set to show its reflection map well: dark
	//	diffuse; a strong Fresnel
	//	bias (planar maps); IOR 1.5 and refraction (cube maps: with the
	//	default IOR of 1 and no refraction the cube term is zero)
	matMaterial* reflector(gtSceneBuilder& io_Builder)
	{
		matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial("BlinnReflection.fx", gtScene::e_Defaults));
		gtScene::SetColor(material, "g_diffuse", maFloatRGBA(0.1f, 0.1f, 0.12f, 1));
		gtScene::SetFloat(material, "fresnelBias", 0.6f);
		gtScene::SetFloat(material, "IOR", 1.5f);
		gtScene::SetFloat(material, "refrFactor", 0.8f);
		return material;
	}

	//	A reflection test's state, shared by its scene function and render
	//	options (both are copied into the case): the reflection maps, a node
	//	moved before the second frame, and the maps' texels after the run
	struct reflection_state
	{
		struct map
		{
			matTexture*	m_pTexture;
			int			m_Size;
			int			m_Layers;		// 1 (planar) or 6 (cube)
			bool		m_bShows;		// the map shows the scene
		};
		std::vector<map>	m_Maps;
		g3dSceneNode*		m_pMover = NULL;
		int					m_Frame = 0;
		//	[backend][map][layer * mips + mip]
		std::vector<std::vector<std::vector<uint8_t>>>	m_Levels[2];

		//	For a new run (the other backend's levels are kept)
		void Reset()
		{
			m_Maps.clear();
			m_pMover = NULL;
			m_Frame = 0;
		}
	};
	typedef std::shared_ptr<reflection_state> state_ptr;

	int mip_count(int i_Size)
	{
		int mips = 1;
		while (i_Size > 1)
		{
			i_Size /= 2;
			mips++;
		}
		return mips;
	}

	//	gtSceneBuilder::Reflective, with the map recorded in io_State;
	//	i_bShows: the map shows the scene (not for a planar reflector facing
	//	away from it, whose mirrored camera looks away)
	void reflect(gtSceneBuilder& io_Builder, const state_ptr& io_State, g3dSceneNode* i_pNode, bool i_bPlanar, bool i_bPerFrame,
				 bool i_bShows = true)
	{
		io_Builder.Reflective(i_pNode, i_bPlanar, i_bPerFrame);
		const effReflectionMap& data = i_pNode->GetFragment()->GetMaterial()->GetReflectionData();
		io_State->m_Maps.push_back({ data.m_ReflectionMap, data.m_ReflMapResolution, i_bPlanar ? 1 : 6, i_bShows });
	}

	//	Two frames, the reflections rendered before each (RenderTargets; in a
	//	profiler frame if i_bProfile), io_State's mover moved before the
	//	second, and every reflection map read back after the run
	gtRenderOptions reflection_options(state_ptr io_State, bool i_bProfile = false)
	{
		gtRenderOptions options;
		options.m_Frames = 2;
		options.m_BeforeFrame = [io_State, i_bProfile](gtSceneBuilder& io_Builder)
		{
			if (io_State->m_Frame++ == 1 && io_State->m_pMover)
			{
				io_State->m_pMover->SetTransform(gtScene::Place(-0.6f, 1.4f, 0.6f, 0.55f));
				io_Builder.Scene().UpdateWorldData();
			}
			if (i_bProfile)
				g3dProfiler::BeginFrame(1.25f);
			io_Builder.RenderTargets();
			if (i_bProfile)
				g3dProfiler::EndFrame();
		};
		options.m_AfterRender = [io_State](gtSceneBuilder&)
		{
			std::vector<std::vector<std::vector<uint8_t>>>& maps = io_State->m_Levels[gtBackend::Current() == gtBackend::e_Vulkan];
			maps.clear();
			for (const reflection_state::map& map : io_State->m_Maps)
			{
				maps.emplace_back();
				for (int layer = 0; layer < map.m_Layers; layer++)
					for (int mip = 0; mip < mip_count(map.m_Size); mip++)
						maps.back().push_back(gtReadTexture(gtBackend::Current(), map.m_pTexture, 4, mip, layer));
			}
		};
		return options;
	}

	//	Every layer and mip of every reflection map matches between the
	//	backends, and the maps that should show the scene do on DX11: a
	//	planar map in its one layer, a cube map in at least three faces.
	void expect_same_maps(const std::string& i_Name, const reflection_state& i_State)
	{
		SCOPED_TRACE(i_Name);
		ASSERT_EQ(i_State.m_Maps.size(), i_State.m_Levels[0].size());
		ASSERT_EQ(i_State.m_Levels[0].size(), i_State.m_Levels[1].size());
		for (size_t m = 0; m < i_State.m_Maps.size(); m++)
		{
			const reflection_state::map& map = i_State.m_Maps[m];
			const int mips = mip_count(map.m_Size);
			ASSERT_EQ((size_t)map.m_Layers * mips, i_State.m_Levels[0][m].size());
			ASSERT_EQ(i_State.m_Levels[0][m].size(), i_State.m_Levels[1][m].size());
			int failed = 0, covered_layers = 0;
			float max_err = 0;
			for (int layer = 0; layer < map.m_Layers; layer++)
			{
				for (int mip = 0; mip < mips; mip++)
				{
					const int size = std::max(1, map.m_Size >> mip);
					const std::vector<uint8_t>& dx11 = i_State.m_Levels[0][m][layer * mips + mip];
					const std::vector<uint8_t>& vulkan = i_State.m_Levels[1][m][layer * mips + mip];
					ASSERT_EQ((size_t)size * size * 4, dx11.size());
					ASSERT_EQ(dx11.size(), vulkan.size());
					if (mip == 0)
					{
						int covered = 0;
						for (size_t i = 0; i < dx11.size(); i += 4)
							covered += (dx11[i] | dx11[i + 1] | dx11[i + 2]) != 0;
						covered_layers += covered > size * size / 50;
					}
					const gtCompareResult result = gtCompareImages(dx11.data(), vulkan.data(), size, size);
					max_err = std::max(max_err, result.m_MaxErr);
					if (!result.m_bPass)
					{
						std::ostringstream name;
						name << i_Name << "/map" << m << "_layer" << layer << "_mip" << mip;
						//	images for the first few only
						ADD_FAILURE() << name.str() << ": " << result.Describe() << (failed < 4 ? " (images in "
							+ gtSaveComparison(name.str(), dx11.data(), vulkan.data(), size, size) + ")" : std::string());
						failed++;
					}
				}
			}
			std::cout << "  " << i_Name << " map " << m << ": " << (map.m_Layers * mips - failed) << "/" << map.m_Layers * mips
					  << " levels match (max " << max_err * 255.0f << "/255), " << covered_layers << " of "
					  << map.m_Layers << " layers show the scene" << std::endl;
			if (map.m_bShows)
				EXPECT_GE(covered_layers, map.m_Layers == 1 ? 1 : 3) << "map " << m << " shows almost nothing on DX11";
		}
	}

	//	Three tinted spheres standing on a reflective (planar) floor at
	//	y = -1, seen from above and in front, lit by shadow-casting lights
	//	and the global environment (the environment pass draws the
	//	reflection term)
	gtSceneFunction planar_scene(state_ptr io_State, bool i_bReflective = true)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			io_State->Reset();
			io_Builder.LightsCastShadows(true);
			io_Builder.Camera().LookAt(maPoint3d(0, 1.5f, 6.0f), maPoint3d(0, -0.4f, 0), maVector3d(0, 1, 0));
			g3dSceneNode* floor = io_Builder.Add(gtScene::Plane(), reflector(io_Builder), floor_at(-1.0f, 3.0f));
			const gtMesh sphere = gtScene::Sphere(16, 32);
			const char* shaders[3] = { "Blinn.fx", "Phong.fx", "Lambert.fx" };
			for (int i = 0; i < 3; i++)
			{
				matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(shaders[i], gtScene::e_Tinted));
				io_Builder.Add(sphere, material, gtScene::Place(-1.4f + 1.4f * i, -0.4f + 0.2f * i, -0.3f * i, 0.6f));
			}
			io_Builder.TwoLights();
			environment(io_Builder, i_Dir);
			if (i_bReflective)
				reflect(io_Builder, io_State, floor, true, false);
		};
	}

	//	A reflective (cube-mapped) sphere among four tinted ones, lit by
	//	shadow-casting lights and the global environment; the one above
	//	moves before the second frame
	gtSceneFunction cube_scene(state_ptr io_State, bool i_bPerFrame, bool i_bReflective = true)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			io_State->Reset();
			io_Builder.LightsCastShadows(true);
			const gtMesh sphere = gtScene::Sphere(16, 32);
			g3dSceneNode* mirror = io_Builder.Add(sphere, reflector(io_Builder), gtScene::Place(0, 0, 0, 0.9f));
			const char* shaders[4] = { "Blinn.fx", "Phong.fx", "Lambert.fx", "Blinn.fx" };
			const float x[4] = { -1.9f, 1.9f, 0.0f, 0.3f };
			const float y[4] = { 0.0f, 0.3f, 1.6f, -1.6f };
			const float z[4] = { -0.5f, -0.2f, -0.8f, 0.5f };
			for (int i = 0; i < 4; i++)
			{
				matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(shaders[i],
					i % 2 ? gtScene::e_Tinted : gtScene::e_Defaults));
				g3dSceneNode* node = io_Builder.Add(sphere, material, gtScene::Place(x[i], y[i], z[i], 0.55f));
				if (i == 2)
					io_State->m_pMover = node;
			}
			io_Builder.TwoLights();
			environment(io_Builder, i_Dir);
			if (i_bReflective)
				reflect(io_Builder, io_State, mirror, false, i_bPerFrame);
		};
	}

	//	A planar floor, two cube-mapped spheres (per frame) that see each
	//	other, tinted spheres, and a planar reflector behind the camera
	//	(out of view, facing the scene: its mirrored camera looks away, so
	//	its map stays empty; the cube maps see it), under the global
	//	environment
	gtSceneFunction several_reflectors(state_ptr io_State)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			io_State->Reset();
			io_Builder.LightsCastShadows(true);
			io_Builder.Camera().LookAt(maPoint3d(0, 1.5f, 6.0f), maPoint3d(0, -0.4f, 0), maVector3d(0, 1, 0));
			g3dSceneNode* floor = io_Builder.Add(gtScene::Plane(), reflector(io_Builder), floor_at(-1.0f, 3.0f));
			maMatrix4x4 behind;
			behind.MakeRotateY(c_Pi);
			behind.ScaleBy(3.0f, 3.0f, 3.0f);
			behind.TranslateBy(0, 0, 8.0f);
			g3dSceneNode* back = io_Builder.Add(gtScene::Plane(), reflector(io_Builder), behind);
			const gtMesh sphere = gtScene::Sphere(16, 32);
			g3dSceneNode* left = io_Builder.Add(sphere, reflector(io_Builder), gtScene::Place(-0.8f, -0.3f, 0, 0.6f));
			g3dSceneNode* right = io_Builder.Add(sphere, reflector(io_Builder), gtScene::Place(0.8f, -0.3f, 0, 0.6f));
			const char* shaders[2] = { "Blinn.fx", "Lambert.fx" };
			for (int i = 0; i < 2; i++)
			{
				matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(shaders[i], gtScene::e_Tinted));
				io_Builder.Add(sphere, material, gtScene::Place(-2.0f + 4.0f * i, -0.5f, -1.0f, 0.5f));
			}
			io_Builder.TwoLights();
			environment(io_Builder, i_Dir);
			reflect(io_Builder, io_State, floor, true, false);
			reflect(io_Builder, io_State, left, false, true);
			reflect(io_Builder, io_State, right, false, true);
			reflect(io_Builder, io_State, back, true, false, false);
		};
	}

	//	The profiler's rows for the reflection renderers, from its CSV
	std::string reflection_timings(const std::string& i_Csv)
	{
		std::ifstream in(i_Csv);
		std::ostringstream out;
		std::string line;
		while (std::getline(in, line))
		{
			if (line.find("ReflectionRenderer,") != std::string::npos || line.find("CubeMapRenderer,") != std::string::npos)
				out << "    " << line << "\n";
		}
		return out.str();
	}

	//	draws per "frame,path" of a profiler CSV
	std::map<std::string, std::string> profile_draws(const std::string& i_Csv)
	{
		std::ifstream in(i_Csv);
		std::map<std::string, std::string> draws;
		std::string line;
		std::getline(in, line);	// header
		while (std::getline(in, line))
		{
			std::vector<std::string> fields;
			std::istringstream row(line);
			for (std::string field; std::getline(row, field, ',');)
				fields.push_back(field);
			if (fields.size() == 8)
				draws[fields[1] + "," + fields[3]] = fields[7];
		}
		return draws;
	}
}

TEST(shdwReflectionVkTest, SimpleRendererMatchesDX11)
{
	gtRenderOptions multi_light = simple_renderer();
	multi_light.m_bSingleLight = false;
	gtRenderOptions matte = simple_renderer();
	matte.m_Prefs = [](g3dPrefs::g3dRenderPrefs& io_Prefs) { io_Prefs.m_bRenderMatte = true; };
	auto lit = [](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		spheres(io_Builder);
		environment(io_Builder, i_Dir);
	};
	gtExpectSameImages("SimpleRendererMatchesDX11",
	{
		{ "environment", lit, simple_renderer() },
		{ "overlay_layers", [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
			{
				lit(io_Builder, i_Dir);
				const gtMesh sphere = gtScene::Sphere(16, 32);
				matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Tinted));
				//	camera space looks down +Z
				io_Builder.AddLayer(sphere, material, gtScene::Place(-0.6f, -0.6f, 3.0f, 0.3f));
				io_Builder.AddViewerLayer(sphere, material, gtScene::Place(0.6f, 0.6f, 3.0f, 0.3f));
			}, simple_renderer() },
		{ "multi_light", lit, multi_light },
		{ "matte", lit, matte },
	});
}

TEST(shdwReflectionVkTest, PlanarReflectionMatchDX11)
{
	state_ptr state(new reflection_state);
	gtExpectSameImage("PlanarReflectionMatchDX11", planar_scene(state), reflection_options(state));
	expect_same_maps("PlanarReflectionMatchDX11", *state);

	//	the reflection shows in the view on DX11
	state_ptr plain(new reflection_state);
	gtRenderOptions plain_options = reflection_options(plain);
	plain_options.m_AfterRender = nullptr;
	gtExpectEffectShows("PlanarReflectionMatchDX11", planar_scene(state), reflection_options(state),
						planar_scene(plain, false), plain_options, 256 * 256 / 100);
}

TEST(shdwReflectionVkTest, CubeMapReflectionMatchDX11)
{
	state_ptr once(new reflection_state), per_frame(new reflection_state);
	gtExpectSameImages("CubeMapReflectionMatchDX11",
	{
		{ "once", cube_scene(once, false), reflection_options(once) },
		{ "per_frame", cube_scene(per_frame, true), reflection_options(per_frame) },
	});
	expect_same_maps("CubeMapReflectionMatchDX11/once", *once);
	expect_same_maps("CubeMapReflectionMatchDX11/per_frame", *per_frame);

	//	a map made once keeps the first frame's scene: the moved sphere
	//	shows only in the map made per frame
	EXPECT_NE(once->m_Levels[0], per_frame->m_Levels[0]);

	//	the reflection shows in the view on DX11
	state_ptr plain(new reflection_state);
	gtRenderOptions plain_options = reflection_options(plain);
	plain_options.m_AfterRender = nullptr;
	gtExpectEffectShows("CubeMapReflectionMatchDX11", cube_scene(per_frame, true), reflection_options(per_frame),
						cube_scene(plain, true, false), plain_options, 256 * 256 / 200);
}

TEST(shdwReflectionVkTest, SeveralReflectorsMatchDX11)
{
	state_ptr state(new reflection_state);
	gtExpectSameImage("SeveralReflectorsMatchDX11", several_reflectors(state), reflection_options(state));
	expect_same_maps("SeveralReflectorsMatchDX11", *state);

#ifdef MSP_GFX_PROFILING
	//	each backend's GPU time per reflection renderer, two frames
	gtTempDir dir;
	const char* names[2] = { "dx11", "vulkan" };
	const gtBackend::Kind kinds[2] = { gtBackend::e_DX11, gtBackend::e_Vulkan };
	for (int i = 0; i < 2; i++)
	{
		const std::string csv = (dir.GetPath() / (std::string(names[i]) + ".csv")).string();
		if (!gtBackend::Run(kinds[i], [&]()
		{
			g3dProfiler::Start(csv, names[i]);
			gtRenderScene(several_reflectors(state), dir.GetPath(), reflection_options(state, true));
			g3dProfiler::Stop();
		}))
			continue;
		std::cout << "  " << names[i] << " reflection renderer times (backend,frame,sim_time,path,depth,cpu_ms,gpu_ms,draws):\n"
				  << reflection_timings(csv);
	}

	//	the backends count draws at the same points (bump draws, full-screen
	//	quads, ...), so every scope reports the same number
	const std::map<std::string, std::string> dx11 = profile_draws((dir.GetPath() / "dx11.csv").string());
	const std::map<std::string, std::string> vulkan = profile_draws((dir.GetPath() / "vulkan.csv").string());
	if (!dx11.empty() && !vulkan.empty())
	{
		EXPECT_EQ(dx11.size(), vulkan.size());
		for (const auto& scope : dx11)
		{
			const auto it = vulkan.find(scope.first);
			if (it == vulkan.end())
				ADD_FAILURE() << "no Vulkan row for " << scope.first;
			else
				EXPECT_EQ(scope.second, it->second) << "draws of " << scope.first;
		}
	}
#else
	std::cout << "  reflection renderer timings off: this build has no profiling markers (MspGfxProfiling=true)" << std::endl;
#endif
}
