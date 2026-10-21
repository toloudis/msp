/*****************************************************************************
**  hairVkTest.cpp
**
**      Hair on Vulkan against DX11: hair fragments (hairModelFragVk, made by
**      mdlFragCreate::CreateHairFragment) drawn by the hair renderer
**      (HairRendererVk) through Hair_SH.fx, on a synthetic hair object
**      (gtSceneBuilder::Hair) over a head sphere:
**      - the forward and simple renderers, with hardware tessellation and
**        hair lines on and off (Hair_SH's four passes), fractional
**        tessellation with interpolated strands, and a point light;
**      - tessellation and interpolation counts at and past their limits
**        (the Vulkan-only isoline factors and the tessellator, Plan 7
**        Review Focus 1);
**      - odd shapes: one strand, two control points per strand, more than
**        65,535 control points (Review Focus 2);
**      - a hair object without strands, on Vulkan only (DX11 asserts making
**        its empty buffers; Review Focus 3);
**      - morphed hair, which DX11 never uploads again (Review Focus 5);
**      - the renderers with their own hair branches (HAIR_SUPPORTED):
**        depth, normal, materials, shadow mask and illumination, and the
**        depth map renderer's shadow map.
**
**      Each case is also checked on DX11 alone: the hair changes the image
**      (gtExpectEffectShows), so a Vulkan renderer that draws no hair can't
**      pass.
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

#ifndef G3D_SCENERENDERERDX11_HPP
#include "GraphicsDX11/g3d/g3dSceneRendererDX11.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dSceneRendererVk.hpp"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace
{
	typedef std::function<g3dSceneRenderer*()> create_fn;
	typedef std::function<void(g3dPrefs::g3dRenderPrefs&)> prefs_fn;

	//	The forward renderer of the backend that is up
	g3dSceneRenderer* forward_renderer()
	{
		if (gtBackend::Current() == gtBackend::e_DX11)
			return new g3dSceneRendererDX11();
		return new gvk::g3dSceneRendererVk();
	}

	g3dSceneRenderer* simple_renderer()
	{
		return g3dSceneRendererCreate::CreateSimpleRenderer();
	}

	//	i_Create's renderer with hair on; i_Prefs edits the prefs further
	gtRenderOptions hair_options(const create_fn& i_Create, const prefs_fn& i_Prefs = nullptr)
	{
		gtRenderOptions options;
		options.m_CreateRenderer = i_Create;
		options.m_Prefs = [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bEnableHair = true;
			if (i_Prefs)
				i_Prefs(p);
		};
		return options;
	}

	//	Hardware tessellation and hair lines as given (Hair_SH's passes)
	prefs_fn pass_prefs(bool i_bTessellate, bool i_bLines)
	{
		return [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bUseHardwareTessellation = i_bTessellate;
			p.m_bHairLines = i_bLines;
		};
	}

	const maMatrix4x4 c_HairPlace = gtScene::Place(0, -0.35f, 0, 0.9f);

	//	The default hair: 64 strands of 8 control points
	gtHairShape hair_shape()
	{
		gtHairShape shape;
		shape.m_nStrands = 64;
		shape.m_nVerticesPerStrand = 8;
		shape.m_Length = 1.0f;
		shape.m_RootRadius = 0.02f;
		shape.m_TipRadius = 0.006f;
		return shape;
	}

	struct hair_setup
	{
		gtHairShape	m_Shape = hair_shape();
		bool		m_bHair = true;			// false: the head alone
		bool		m_bHead = true;			// false: the hair alone
		bool		m_bNearClip = false;	// the camera's far clip near the scene (the depth preview's grey range)
		bool		m_bPointLight = false;	// a point light instead of TwoLights
		bool		m_bShadows = false;		// a wall behind, the projected light with a shadow map, all casting
		//	runs on the hair node after the scene is built
		std::function<void(g3dSceneNode*)>	m_Edit;
	};

	//	Casts and receives shadows (fragments do neither by default)
	void casts_shadow(g3dSceneNode* io_pNode)
	{
		g3dFragment* fragment = const_cast<g3dFragment*>(io_pNode->GetFragment());
		fragment->SetCastsShadow(true);
		fragment->SetReceivesShadow(true);
	}

	const int c_MapSize = 512;

	//	The head and its hair; with m_bShadows, shdwShadowVkTest's wall and
	//	projected light (returned; else NULL)
	g3dProjectedLight* build_hair(gtSceneBuilder& io_Builder, const hair_setup& i_Setup)
	{
		if (i_Setup.m_bNearClip)
			io_Builder.Camera().SetClip(0.5f, 20.0f);
		g3dSceneNode* head_node = NULL;
		if (i_Setup.m_bHead)
		{
			matMaterial* head = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Tinted));
			head_node = io_Builder.Add(gtScene::Sphere(16, 32), head, c_HairPlace * gtScene::Place(0, 0, 0, 0.97f));
		}
		g3dSceneNode* hair_node = i_Setup.m_bHair ? io_Builder.Hair(i_Setup.m_Shape, c_HairPlace) : NULL;
		if (hair_node && i_Setup.m_Edit)
			i_Setup.m_Edit(hair_node);
		if (i_Setup.m_bShadows)
		{
			matMaterial* wall = io_Builder.Keep(gtScene::ShaderMaterial("Lambert.fx", gtScene::e_Defaults));
			casts_shadow(io_Builder.Add(gtScene::Plane(), wall, gtScene::Place(0, 0, -1.5f, 4.0f)));
			if (head_node)
				casts_shadow(head_node);
			if (hair_node)
				casts_shadow(hair_node);
			return io_Builder.ProjectedLight(maPoint3d(1.5f, 2.5f, 7.0f), maPoint3d(0, 0, -1.5f),
											 maFloatRGBA(1.0f, 0.95f, 0.85f, 1), c_MapSize);
		}
		if (i_Setup.m_bPointLight)
			io_Builder.PointLight(maPoint3d(-2.0f, 2.0f, 3.0f), maFloatRGBA(1.0f, 0.9f, 0.8f, 1), 25.0f);
		else
			io_Builder.TwoLights();
		return NULL;
	}

	gtSceneFunction hair_scene(const hair_setup& i_Setup)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path&)
		{
			build_hair(io_Builder, i_Setup);
		};
	}

	gtSceneFunction bald_scene(hair_setup i_Setup)
	{
		i_Setup.m_bHair = false;
		return hair_scene(i_Setup);
	}

	struct hair_case
	{
		std::string		m_Name;
		hair_setup		m_Setup;
		gtRenderOptions	m_Options;
		//	DX11 draws no hair with this renderer (the simple renderer's
		//	traversal sets hair nodes aside and nothing draws them): the
		//	guard checks that instead
		bool			m_bNoHairOnDX11 = false;
	};

	//	On DX11, i_Case's image with the hair is the image without it
	void expect_no_hair_on_dx11(const char* i_pName, const hair_case& i_Case)
	{
		gtPicture with, without;
		gtBackend::Run(gtBackend::e_DX11, [&]()
		{
			gtTempDir dir;
			with = gtRenderScene(hair_scene(i_Case.m_Setup), dir.GetPath(), i_Case.m_Options);
			without = gtRenderScene(bald_scene(i_Case.m_Setup), dir.GetPath(), i_Case.m_Options);
		});
		ASSERT_TRUE(with.Valid()) << i_pName;
		EXPECT_TRUE(with.m_Rgba == without.m_Rgba) << i_pName << ": DX11 drew hair";
	}

	//	The cases compared; then each is checked on DX11: the hair changes
	//	the image by more than 8/255 in at least i_MinPixels pixels (or, for
	//	m_bNoHairOnDX11, doesn't change it)
	void expect_cases(const char* i_pTest, const std::vector<hair_case>& i_Cases, int i_MinPixels = 256 * 256 / 200)
	{
		std::vector<gtCase> cases;
		for (const hair_case& c : i_Cases)
			cases.push_back({ c.m_Name, hair_scene(c.m_Setup), c.m_Options });
		gtExpectSameImages(i_pTest, cases);
		for (const hair_case& c : i_Cases)
		{
			const std::string name = std::string(i_pTest) + "/" + c.m_Name;
			if (c.m_bNoHairOnDX11)
			{
				expect_no_hair_on_dx11(name.c_str(), c);
				continue;
			}
			gtExpectEffectShows(name.c_str(), hair_scene(c.m_Setup), c.m_Options,
								bald_scene(c.m_Setup), c.m_Options, i_MinPixels);
		}
	}
}

//	Step 1 guard (decision 1, rule b): DX11 draws the synthetic hair, so
//	there is something for Vulkan to match
TEST(hairVkTest, HairShowsOnDX11)
{
	hair_setup setup;
	setup.m_Shape = gtHairShape();
	setup.m_Shape.m_nStrands = 64;
	setup.m_Shape.m_nVerticesPerStrand = 8;
	const gtRenderOptions options = hair_options(forward_renderer);
	gtExpectEffectShows("HairShowsOnDX11", hair_scene(setup), options, bald_scene(setup), options, 256 * 256 / 100);
}

//	The forward and simple renderers: Hair_SH's four passes (hardware
//	tessellation x hair lines), fractional tessellation with interpolated
//	strands, and a point light
TEST(hairVkTest, HairMatchesDX11)
{
	const std::pair<const char*, create_fn> renderers[] =
	{
		{ "forward", forward_renderer },
		{ "simple", simple_renderer },
	};
	std::vector<hair_case> cases;
	for (const auto& r : renderers)
	{
		const std::string name = r.first;
		const size_t first = cases.size();
		for (bool tess : { false, true })
		{
			for (bool lines : { false, true })
			{
				cases.push_back({ name + (tess ? "/tess" : "/no_tess") + (lines ? "_lines" : ""), hair_setup(),
								  hair_options(r.second, pass_prefs(tess, lines)) });
			}
		}
		cases.push_back({ name + "/tess_2.5_interp_3", hair_setup(), hair_options(r.second, [](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bUseHardwareTessellation = true;
			p.m_HairTessellation = 2.5f;
			p.m_HairInterpolationCount = 3;
		}) });
		hair_setup point;
		point.m_bPointLight = true;
		cases.push_back({ name + "/point_light", point, hair_options(r.second) });
		//	the simple renderer (shdwShadowLayerRenderer) draws no hair on DX11
		for (size_t i = first; i < cases.size(); i++)
			cases[i].m_bNoHairOnDX11 = name == "simple";
	}
	expect_cases("HairMatchesDX11", cases);
}

//	Review Focus 1: tessellation 0 to 100 (the hull shader clamps to
//	[1, 63], the tessellator rounds to fractional odd) and interpolation
//	counts 0 to 100, hardware tessellated, through the forward renderer
TEST(hairVkTest, HairTessLimitsMatchDX11)
{
	std::vector<hair_case> cases;
	for (float tess : { 0.0f, 0.5f, 1.0f, 2.5f, 63.9f, 64.0f, 100.0f })
	{
		char name[32];
		snprintf(name, sizeof(name), "tess_%g", tess);
		cases.push_back({ name, hair_setup(), hair_options(forward_renderer, [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bUseHardwareTessellation = true;
			p.m_HairTessellation = tess;
			p.m_HairInterpolationCount = 1;
		}) });
	}
	for (int interp : { 0, 1, 64, 100 })
	{
		cases.push_back({ "interp_" + std::to_string(interp), hair_setup(),
						  hair_options(forward_renderer, [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bUseHardwareTessellation = true;
			p.m_HairTessellation = 2.5f;
			p.m_HairInterpolationCount = interp;
		}) });
	}
	expect_cases("HairTessLimitsMatchDX11", cases, 256 * 256 / 400);
}

//	Review Focus 2: one strand; two control points per strand (one
//	segment: every patch but each strand's first is culled); more than
//	65,535 control points; each with and without hardware tessellation
TEST(hairVkTest, HairOddShapesMatchDX11)
{
	struct shape { const char* m_pName; int m_nStrands; int m_nVertices; };
	const shape shapes[] =
	{
		{ "one_strand", 1, 8 },
		{ "two_points", 64, 2 },
		{ "65600_points", 8200, 8 },
	};
	std::vector<hair_case> cases;
	for (const shape& s : shapes)
	{
		hair_setup setup;
		setup.m_Shape.m_nStrands = s.m_nStrands;
		setup.m_Shape.m_nVerticesPerStrand = s.m_nVertices;
		if (s.m_nStrands == 1)
		{
			setup.m_bHead = false;					// the one strand grows behind the head
			setup.m_Shape.m_RootRadius = 0.05f;		// wide enough to see
			setup.m_Shape.m_TipRadius = 0.03f;
		}
		for (bool tess : { false, true })
		{
			gtRenderOptions options = hair_options(forward_renderer, pass_prefs(tess, false));
			options.m_bCheckCoverage = s.m_nStrands > 1;
			cases.push_back({ std::string(s.m_pName) + (tess ? "/tess" : "/no_tess"), setup, options });
		}
	}
	expect_cases("HairOddShapesMatchDX11", cases, 10);
}

//	Review Focus 3, Vulkan only (DX11 asserts making a zero-size buffer): a
//	hair object without strands draws nothing (the image is the head's
//	alone), with and without hardware tessellation, and validation stays
//	clean (gtBackend checks it)
TEST(hairVkTest, EmptyHairDrawsNothing)
{
	const gtRenderOptions options[2] = { hair_options(forward_renderer, pass_prefs(false, false)),
										 hair_options(forward_renderer, pass_prefs(true, false)) };
	hair_setup empty;
	empty.m_Shape.m_nStrands = 0;
	gtPicture with[2], without[2];
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]()
	{
		for (int i = 0; i < 2; i++)
		{
			gtTempDir dir;
			with[i] = gtRenderScene(hair_scene(empty), dir.GetPath(), options[i]);
			without[i] = gtRenderScene(bald_scene(empty), dir.GetPath(), options[i]);
		}
	}))
		return;
	for (int i = 0; i < 2; i++)
	{
		SCOPED_TRACE(i ? "tess" : "no_tess");
		ASSERT_TRUE(with[i].Valid());
		ASSERT_TRUE(without[i].Valid());
		EXPECT_TRUE(with[i].m_Rgba == without[i].m_Rgba);
	}
}

//	Review Focus 5: UpdateVertices changes the fragment's hair info, but
//	DX11 made its buffers immutable and never uploads them again, so the
//	strands don't move; Vulkan draws the same (DX11's behaviour, not a fix)
TEST(hairVkTest, UpdatedHairMatchesDX11)
{
	hair_setup moved;
	moved.m_Edit = [](g3dSceneNode* io_pNode)
	{
		g3dFragment* fragment = const_cast<g3dFragment*>(io_pNode->GetFragment());
		const int count = hair_shape().m_nStrands * hair_shape().m_nVerticesPerStrand;
		std::vector<maPoint3d> points(count);
		for (int i = 0; i < count; i++)
			points[i] = maPoint3d(0.02f * (i % 8), 1.5f + 0.01f * (i / 8), 0);
		fragment->UpdateVertices(count, points.data(), NULL);
	};
	std::vector<hair_case> cases;
	for (bool tess : { false, true })
		cases.push_back({ tess ? "tess" : "no_tess", moved, hair_options(forward_renderer, pass_prefs(tess, false)) });
	expect_cases("UpdatedHairMatchesDX11", cases);

	//	and on DX11 the morphed hair draws as the unmorphed hair
	gtPicture before, after;
	gtBackend::Run(gtBackend::e_DX11, [&]()
	{
		gtTempDir dir;
		before = gtRenderScene(hair_scene(hair_setup()), dir.GetPath(), cases[1].m_Options);
		after = gtRenderScene(hair_scene(moved), dir.GetPath(), cases[1].m_Options);
	});
	ASSERT_TRUE(before.Valid());
	EXPECT_TRUE(before.m_Rgba == after.m_Rgba);
}

namespace
{
	//	The shadow map of the shadow scene's projected light after
	//	RenderTargets (the depth map renderer), on the backend that is up
	std::vector<float> hair_map_depths(bool i_bHair)
	{
		gtPrefsScope prefs;
		g3dPrefs::CurrentPrefs().m_bEnableHair = true;
		gtSceneBuilder builder;
		hair_setup setup;
		setup.m_bShadows = true;
		setup.m_bHair = i_bHair;
		g3dProjectedLight* light = build_hair(builder, setup);
		builder.RenderTargets();
		const std::vector<uint8_t> bytes = gtReadTexture(gtBackend::Current(), light->GetShadowMap(), 4);
		std::vector<float> depths(bytes.size() / 4);
		if (!depths.empty())
			memcpy(depths.data(), bytes.data(), bytes.size());
		return depths;
	}
}

//	The renderers with their own hair branches (HAIR_SUPPORTED): the depth,
//	normal and materials previews, the shadow mask and illumination
//	renderers (hair lit by a shadowed projected light), and the depth map
//	renderer's shadow map
TEST(hairVkTest, HairInOtherRenderersMatchesDX11)
{
	const std::pair<const char*, create_fn> renderers[] =
	{
		{ "depth", []() { return g3dSceneRendererCreate::CreateDepthRenderer(); } },
		{ "normal", []() { return g3dSceneRendererCreate::CreateNormalRenderer(); } },
		{ "materials", []() { return g3dSceneRendererCreate::CreateMaterialsRenderer(); } },
		{ "shadow_mask", []() { return g3dSceneRendererCreate::CreateShadowMaskRenderer(); } },
		{ "illumination", []() { return g3dSceneRendererCreate::CreateIlluminationRenderer(); } },
	};
	std::vector<hair_case> cases;
	for (const auto& r : renderers)
	{
		for (bool tess : { false, true })
		{
			hair_setup setup;
			setup.m_bShadows = true;
			setup.m_bNearClip = std::string(r.first) == "depth";
			gtRenderOptions options = hair_options(r.second, pass_prefs(tess, false));
			options.m_BeforeFrame = [](gtSceneBuilder& io_Builder) { io_Builder.RenderTargets(); };
			cases.push_back({ std::string(r.first) + (tess ? "/tess" : "/no_tess"), setup, options });
		}
	}
	expect_cases("HairInOtherRenderersMatchesDX11", cases);

	//	the depth map renderer: the hair's depths in the shadow map
	std::vector<float> dx11, dx11_bald, vulkan;
	gtBackend::Run(gtBackend::e_DX11, [&]()
	{
		dx11 = hair_map_depths(true);
		dx11_bald = hair_map_depths(false);
	});
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = hair_map_depths(true); }))
		return;
	ASSERT_EQ((size_t)c_MapSize * c_MapSize, dx11.size());
	ASSERT_EQ(dx11.size(), vulkan.size());
	ASSERT_EQ(dx11.size(), dx11_bald.size());
	size_t differing = 0, hair = 0;
	float max_err = 0;
	for (size_t t = 0; t < dx11.size(); t++)
	{
		const float err = std::fabs(dx11[t] - vulkan[t]);
		max_err = std::max(max_err, err);
		differing += !(err <= 1e-6f);		// NaN counts
		hair += dx11[t] != dx11_bald[t];
	}
	std::cout << "  shadow map: max error " << max_err << ", " << differing << " texels over 1e-6, "
			  << hair << " texels hair on DX11" << std::endl;
	EXPECT_EQ(0u, differing);
	EXPECT_GT(hair, dx11.size() / 200);		// the guard: the hair is in DX11's map
}
