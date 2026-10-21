/*****************************************************************************
**  shdwPreviewRenderersVkTest.cpp
**
**      The preview renderers MachStudio's viewport offers (the render
**      types in g3dSceneRendererTypes) on Vulkan against DX11, each made by
**      the creator (g3dSceneRendererCreate::Create<Type>Renderer) and drawn
**      into a render-target texture, with HDR AA off and on (the frame
**      buffer's multisampled target and its resolve, NEW_AA_CODE):
**      - the depth renderer (shdwDepthRendererVk): spheres at five depths
**        and a transparent one;
**      - the normal renderer (shdwNormalRendererVk): a bump-mapped sphere
**        (a soft normal map) and a plain one;
**      - the materials renderer (shdwMaterialsRendererVk with
**        shdwPassMaterials): one untextured sphere per material shader;
**      - the glow renderer (shdwGlowRendererVk): a glowing sphere and one
**        that doesn't glow;
**      - the reflection-only renderer (shdwReflectionsOnlyRendererVk): a
**        planar reflective floor and a cube-mapped sphere;
**      - the shadow mask and illumination renderers
**        (shdwShadowsOnlyRendererVk, masking and not): shdwShadowVkTest's
**        spheres before a wall under a shadowed projected light, with a
**        shadow-casting point light or not, and with the projected light's
**        shadows off;
**      - the AO preview renderer (shdwAOPreviewRendererVk):
**        shdwAoGiVkTest's spheres on a floor, SSAO and AO volumes, and a
**        sphere that doesn't receive occlusion;
**      - the GI preview renderer (shdwGIPreviewRendererVk): shdwAoGiVkTest's
**        LPV room, SSGI and LPV, and LPV with HDR AA;
**      - the UV renderer (shdwUVRendererVk), as fgmtUVEditor uses it: a
**        textured sphere, and a bump-mapped plane with UVs outside 0-1;
**      - the texture renderer (g3dTextureRendererVk), as rmpTextureMgr
**        draws its ramps: a Ramp.fx square in a screen-space layer, each
**        gradient shape, with and without waves;
**      - a soft cutout (a Blinn transparency map over a sphere) through the
**        materials, normal, illumination, reflection-only and depth
**        renderers, alpha included.
**
**      Each case is also checked on DX11 alone: the renderer's image
**      differs from the default renderer's (gtExpectEffectShows), so a
**      Vulkan renderer that fell back to another one can't pass.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"

#ifndef MA_CONSTANTS_HPP
#include "Core/Ma/maConstants.hpp"
#endif
#ifndef EFF_GLOWDATA_HPP
#include "Graphics/Eff/effGlowData.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif
#ifndef EFF_RAMPDATA_HPP
#include "Graphics/Eff/effRampData.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/G3d/g3dLayer.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/G3d/g3dProjectedLight.hpp"
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
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif

#include <gtest/gtest.h>

#include <functional>
#include <string>
#include <vector>

namespace
{
	//	The material shaders shdwMaterialsVkTest checks are registered on
	//	both backends (its c_Expected)
	const char* const c_Materials[] =
	{
		"Anisotropic.fx", "Blinn.fx", "BlinnReflection.fx", "BlinnSkin.fx", "CarPaint.fx", "Cartoon.fx",
		"Default.fx", "FC3DBlinn.fx", "FC3DPhong.fx", "FC3DPhongTransparent.fx", "FC3DSpecularFresnel.fx",
		"FC3DSubSurfaceScatter_wBlinnSpecular.fx", "Lambert.fx", "Phong.fx", "PhongReflection.fx",
		"Phong_wBump.fx", "Simple.fx", "Skin.fx", "SpecularFresnel.fx", "SubSurfaceScatter.fx",
		"SubSurfaceScatter_wBlinnSpecular.fx", "testtess.fx",
	};

	typedef std::function<g3dSceneRenderer*()> create_fn;

	//	The renderer, HDR AA as given; i_Prefs edits the prefs further
	gtRenderOptions preview(const create_fn& i_Create, bool i_bHDRAA,
							const std::function<void(g3dPrefs::g3dRenderPrefs&)>& i_Prefs = nullptr, int i_Frames = 1)
	{
		gtRenderOptions options;
		options.m_CreateRenderer = i_Create;
		options.m_Frames = i_Frames;
		options.m_Prefs = [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bHDRAA = i_bHDRAA;
			if (i_Prefs)
				i_Prefs(p);
		};
		return options;
	}

	//	The same scene and prefs through the default renderer
	gtRenderOptions with_default(gtRenderOptions i_Options)
	{
		i_Options.m_CreateRenderer = nullptr;
		return i_Options;
	}

	//	The cases compared; then each is checked on DX11 to differ from the
	//	default renderer's image
	void expect_cases(const char* i_pTest, const std::vector<gtCase>& i_Cases)
	{
		gtExpectSameImages(i_pTest, i_Cases);
		for (const gtCase& c : i_Cases)
		{
			gtExpectEffectShows((std::string(i_pTest) + "/" + c.m_Name).c_str(), c.m_Build, c.m_Options,
								c.m_Build, with_default(c.m_Options), 256 * 256 / 50);
		}
	}

	//	Each scene with HDR AA off and on, as expect_cases; i_BeforeFrame
	//	runs before each frame
	void expect_renderer(const char* i_pTest, const create_fn& i_Create,
						 const std::vector<std::pair<std::string, gtSceneFunction>>& i_Scenes,
						 const std::function<void(g3dPrefs::g3dRenderPrefs&)>& i_Prefs = nullptr, int i_Frames = 1,
						 const std::function<void(gtSceneBuilder&)>& i_BeforeFrame = nullptr)
	{
		std::vector<gtCase> cases;
		for (const auto& scene : i_Scenes)
		{
			for (bool aa : { false, true })
			{
				gtRenderOptions options = preview(i_Create, aa, i_Prefs, i_Frames);
				options.m_BeforeFrame = i_BeforeFrame;
				cases.push_back({ scene.first + (aa ? "/hdr_aa" : "/no_aa"), scene.second, options });
			}
		}
		expect_cases(i_pTest, cases);
	}

	//	Spheres at five depths across the view, and a transparent (Phong
	//	data) one in front; the far clip is near the scene, so the depths
	//	spread over the grey range
	void depth_scene(gtSceneBuilder& io_Builder, const std::filesystem::path&)
	{
		io_Builder.Camera().SetClip(0.5f, 20.0f);
		const gtMesh sphere = gtScene::Sphere(16, 32);
		for (int i = 0; i < 5; i++)
		{
			matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Tinted));
			io_Builder.Add(sphere, material, gtScene::Place(-2.0f + 1.0f * i, 0.6f, 2.0f - 3.0f * i, 0.6f));
		}
		matMaterial* glass = io_Builder.Keep(gtScene::DataMaterial("Phong.fx", [](effShaderData* io_pData)
		{
			gtScene::SetPhong(io_pData, gtScene::e_Tinted);
		}));
		io_Builder.Add(sphere, glass, gtScene::Place(0.3f, -0.9f, 1.0f, 0.7f));
		io_Builder.TwoLights();
	}

	//	A sphere with a soft normal map (the material's NormalsData) and a
	//	plain one
	void normal_scene(gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		const gtMesh sphere = gtScene::Sphere(24, 48);
		matTexture* normals = io_Builder.Keep(gtScene::NormalTexture(i_Dir, 128, 4));
		matMaterial* bumped = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Tinted));
		bumped->NormalsData().m_pNormalMap = normals;
		bumped->NormalsData().m_BumpScale = 1.0f;
		io_Builder.Add(sphere, bumped, gtScene::Place(-1.1f, 0, 0, 1.0f));
		matMaterial* plain = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Tinted));
		io_Builder.Add(sphere, plain, gtScene::Place(1.1f, 0, 0, 1.0f));
		io_Builder.TwoLights();
	}

	//	One untextured sphere per material shader, in a 6 x 4 grid
	void materials_scene(gtSceneBuilder& io_Builder, const std::filesystem::path&)
	{
		const gtMesh sphere = gtScene::Sphere(12, 24);
		int i = 0;
		for (const char* shader : c_Materials)
		{
			matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(shader, gtScene::e_Defaults));
			io_Builder.Add(sphere, material, gtScene::Place(-2.25f + 0.9f * (i % 6), 1.35f - 0.9f * (i / 6), 0, 0.4f));
			i++;
		}
		io_Builder.TwoLights();
	}

	//	Spheres with constant glow and lit (specular) glow, both through a
	//	soft mask, and one that doesn't glow; the lights cast shadows (the
	//	lit glow is drawn for those only)
	void glow_scene(gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		io_Builder.LightsCastShadows(true);
		const gtMesh sphere = gtScene::Sphere(24, 48);
		matTexture* mask = io_Builder.Keep(gtScene::CheckerTexture(i_Dir, 64, 64, 4));
		for (int i = 0; i < 3; i++)
		{
			matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Tinted));
			if (i < 2)
			{
				material->SetHasGlow(true);
				effGlowData& glow = material->GlowData();
				glow.m_GlowAmount = 4.0f;
				glow.m_GlowScale = maVector4d(1.0f, 0.6f, 0.3f, 1.0f);
				glow.m_GlowSize = 4.0f;
				glow.m_bConstantGlow = i == 0;
				glow.m_pGlowMask = mask;
			}
			io_Builder.Add(sphere, material, gtScene::Place(-1.6f + 1.6f * i, 0, 0, 0.7f));
		}
		io_Builder.TwoLights();
	}

	//	shdwReflectionVkTest's reflectors: a BlinnReflection material that
	//	shows its map well
	matMaterial* reflector(gtSceneBuilder& io_Builder)
	{
		matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial("BlinnReflection.fx", gtScene::e_Defaults));
		gtScene::SetColor(material, "g_diffuse", maFloatRGBA(0.1f, 0.1f, 0.12f, 1));
		gtScene::SetFloat(material, "fresnelBias", 0.6f);
		gtScene::SetFloat(material, "IOR", 1.5f);
		gtScene::SetFloat(material, "refrFactor", 0.8f);
		return material;
	}

	maMatrix4x4 floor_at(float i_Y, float i_Scale)
	{
		maMatrix4x4 m;
		m.MakeRotate(maVector3d(0, 0, 1), maVector3d(0, 1, 0));
		m.ScaleBy(i_Scale, i_Scale, i_Scale);
		m.TranslateBy(0, i_Y, 0);
		return m;
	}

	//	A planar reflective floor and a cube-mapped sphere among tinted
	//	spheres, lit by shadow-casting lights and the global environment
	//	(not drawn as the background); the reflections are rendered before
	//	each frame (reflection_options)
	void reflection_scene(gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		io_Builder.LightsCastShadows(true);
		io_Builder.Camera().LookAt(maPoint3d(0, 1.5f, 6.0f), maPoint3d(0, -0.4f, 0), maVector3d(0, 1, 0));
		g3dSceneNode* floor = io_Builder.Add(gtScene::Plane(), reflector(io_Builder), floor_at(-1.0f, 3.0f));
		const gtMesh sphere = gtScene::Sphere(16, 32);
		g3dSceneNode* mirror = io_Builder.Add(sphere, reflector(io_Builder), gtScene::Place(0, -0.2f, 0, 0.7f));
		const char* shaders[3] = { "Blinn.fx", "Phong.fx", "Lambert.fx" };
		const float x[3] = { -1.6f, 1.6f, 0.2f };
		const float z[3] = { -0.4f, -0.2f, -1.6f };
		for (int i = 0; i < 3; i++)
		{
			matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(shaders[i], gtScene::e_Tinted));
			io_Builder.Add(sphere, material, gtScene::Place(x[i], -0.4f, z[i], 0.55f));
		}
		io_Builder.TwoLights();
		matTexture* diffuse = io_Builder.Keep(gtScene::HdrTexture(i_Dir, 128, 64, 2.0f));
		matTexture* specular = io_Builder.Keep(gtScene::HdrTexture(i_Dir, 256, 128, 2.0f));
		io_Builder.GlobalEnvironment(diffuse, specular, false);
		io_Builder.Reflective(floor, true, false);
		io_Builder.Reflective(mirror, false, true);
	}

	//	The reflection-only renderer's options: two frames, the reflections
	//	rendered before each
	gtRenderOptions reflection_options(bool i_bHDRAA)
	{
		gtRenderOptions options = preview([]() { return g3dSceneRendererCreate::CreateReflectionOnlyRenderer(); }, i_bHDRAA, nullptr, 2);
		options.m_BeforeFrame = [](gtSceneBuilder& io_Builder) { io_Builder.RenderTargets(); };
		return options;
	}
}

namespace
{
	//	A cobweb-like material: a soft white-on-black grid as Blinn's
	//	transparency map (strands opaque, holes clear; binding it sets
	//	hasTransparencyMap, its ExistVar) over a tinted sphere,
	//	lit; the test scene's cobweb is the one thing whose alpha shows in
	//	the materials, normal, illumination and reflection-only passes
	void cutout_scene(gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		io_Builder.LightsCastShadows(true);
		matTexture* diffuse = io_Builder.Keep(gtScene::CheckerTexture(i_Dir, 64, 64, 4));
		matTexture* web = io_Builder.Keep(gtScene::WebTexture(i_Dir, 128, 8));
		matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Tinted, { diffuse }));
		gtScene::SetTexture(material, "transparencyMap", web);
		io_Builder.Add(gtScene::Plane(), material, gtScene::Place(0, 0, 1.0f, 1.6f));
		matMaterial* behind = io_Builder.Keep(gtScene::ShaderMaterial("Phong.fx", gtScene::e_Tinted));
		io_Builder.Add(gtScene::Sphere(16, 32), behind, gtScene::Place(0.4f, 0, -1.0f, 1.0f));
		io_Builder.TwoLights();
	}
}

//	Plan 6 Task 10: the capture passes of the test scene whose cobweb
//	differed more than the beauty pass (alpha and alpha-tested edges; the
//	cobweb's texture is hard-edged, so that was the anisotropic filtering
//	difference): a soft cutout through each of those renderers matches
TEST(shdwPreviewRenderersVkTest, CutoutMatchesDX11)
{
	const std::pair<const char*, create_fn> renderers[] =
	{
		{ "materials", []() { return g3dSceneRendererCreate::CreateMaterialsRenderer(); } },
		{ "normal", []() { return g3dSceneRendererCreate::CreateNormalRenderer(); } },
		{ "illumination", []() { return g3dSceneRendererCreate::CreateIlluminationRenderer(); } },
		{ "reflection_only", []() { return g3dSceneRendererCreate::CreateReflectionOnlyRenderer(); } },
		{ "depth", []() { return g3dSceneRendererCreate::CreateDepthRenderer(); } },
	};
	std::vector<gtCase> cases;
	for (const auto& r : renderers)
	{
		for (bool aa : { false, true })
		{
			cases.push_back({ std::string(r.first) + (aa ? "/hdr_aa" : "/no_aa"), cutout_scene,
							  preview(r.second, aa, [](g3dPrefs::g3dRenderPrefs& p) { p.m_bEnableTransparent = true; }) });
		}
	}
	expect_cases("CutoutMatchesDX11", cases);
}

TEST(shdwPreviewRenderersVkTest, DepthRendererMatchesDX11)
{
	expect_renderer("DepthRendererMatchesDX11", []() { return g3dSceneRendererCreate::CreateDepthRenderer(); },
					{ { "spheres", depth_scene } },
					[](g3dPrefs::g3dRenderPrefs& p) { p.m_bEnableTransparent = true; });
}

TEST(shdwPreviewRenderersVkTest, NormalRendererMatchesDX11)
{
	expect_renderer("NormalRendererMatchesDX11", []() { return g3dSceneRendererCreate::CreateNormalRenderer(); },
					{ { "spheres", normal_scene } });
}

TEST(shdwPreviewRenderersVkTest, MaterialsRendererMatchesDX11)
{
	expect_renderer("MaterialsRendererMatchesDX11", []() { return g3dSceneRendererCreate::CreateMaterialsRenderer(); },
					{ { "materials", materials_scene } });
}

TEST(shdwPreviewRenderersVkTest, GlowRendererMatchesDX11)
{
	expect_renderer("GlowRendererMatchesDX11", []() { return g3dSceneRendererCreate::CreateGlowRenderer(); },
					{ { "spheres", glow_scene } },
					[](g3dPrefs::g3dRenderPrefs& p) { p.m_bEnableGlow = true; });
}

TEST(shdwPreviewRenderersVkTest, ReflectionOnlyRendererMatchesDX11)
{
	std::vector<gtCase> cases;
	for (bool aa : { false, true })
		cases.push_back({ aa ? "reflectors/hdr_aa" : "reflectors/no_aa", reflection_scene, reflection_options(aa) });
	expect_cases("ReflectionOnlyRendererMatchesDX11", cases);
}

namespace
{
	//	Casts and receives shadows (fragments do neither by default)
	g3dFragment* casts_shadow(g3dSceneNode* io_pNode)
	{
		g3dFragment* fragment = const_cast<g3dFragment*>(io_pNode->GetFragment());
		fragment->SetCastsShadow(true);
		fragment->SetReceivesShadow(true);
		return fragment;
	}

	//	shdwShadowVkTest's scene: three spheres in front of a wall, lit by a
	//	projected light from the front with a shadow map (i_bProjectedShadows
	//	false: the light casts no shadow); with i_bPointLight, also a
	//	shadow-casting point light on the left
	gtSceneFunction shadow_scene(bool i_bPointLight, bool i_bProjectedShadows)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path&)
		{
			const gtMesh sphere = gtScene::Sphere(16, 32);
			matMaterial* wall = io_Builder.Keep(gtScene::ShaderMaterial("Lambert.fx", gtScene::e_Defaults));
			casts_shadow(io_Builder.Add(gtScene::Plane(), wall, gtScene::Place(0, 0, -1.5f, 4.0f)));
			const gtScene::Values values[3] = { gtScene::e_Defaults, gtScene::e_Tinted, gtScene::e_Defaults };
			const char* shaders[3] = { "Blinn.fx", "Phong.fx", "Lambert.fx" };
			for (int i = 0; i < 3; i++)
			{
				matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(shaders[i], values[i]));
				casts_shadow(io_Builder.Add(sphere, material, gtScene::Place(-1.3f + 1.3f * i, 0.4f - 0.4f * i, 0, 0.5f)));
			}
			g3dProjectedLight* light = io_Builder.ProjectedLight(maPoint3d(1.5f, 2.5f, 7.0f), maPoint3d(0, 0, -1.5f),
																 maFloatRGBA(1.0f, 0.95f, 0.85f, 1), 1024);
			if (!i_bProjectedShadows)
				light->SetCastsShadow(false);
			if (i_bPointLight)
			{
				io_Builder.LightsCastShadows(true);
				io_Builder.PointLight(maPoint3d(-3.0f, 1.0f, 3.0f), maFloatRGBA(0.7f, 0.7f, 0.8f, 1), 25.0f);
			}
		};
	}

	//	The shadow mask or illumination renderer on the shadow scene: one
	//	light, two, and the projected light's shadows off; the shadow map
	//	rendered before each frame
	void expect_shadows_only(const char* i_pTest, const create_fn& i_Create)
	{
		expect_renderer(i_pTest, i_Create,
						{ { "projected", shadow_scene(false, true) },
						  { "projected_point", shadow_scene(true, true) },
						  { "unshadowed_projected_point", shadow_scene(true, false) } },
						[](g3dPrefs::g3dRenderPrefs& p) { p.m_bProjLightsOn = true; }, 1,
						[](gtSceneBuilder& io_Builder) { io_Builder.RenderTargets(); });
	}

	//	shdwAoGiVkTest's AO scene: four spheres on a floor, lit by
	//	shadow-casting lights, seen from the side; with i_bMaskOne, sphere 3
	//	doesn't receive occlusion
	gtSceneFunction ao_scene(bool i_bMaskOne)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path&)
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

	//	The AO preview renderer, SSAO or AO volumes (m_bAOInvalid)
	gtRenderOptions ao_preview(bool i_bVolumes)
	{
		return preview([]() { return g3dSceneRendererCreate::CreateAmbientOcclusionRenderer(); }, false,
					   [=](g3dPrefs::g3dRenderPrefs& p)
					   {
						   p.m_bEnableSSAO = true;
						   p.m_bAOInvalid = i_bVolumes;
					   });
	}

	//	A Lambert material with its diffuse colour (g_diffuse) set
	matMaterial* coloured(const maFloatRGBA& i_Color)
	{
		matMaterial* material = gtScene::ShaderMaterial("Lambert.fx", gtScene::e_Defaults);
		gtScene::SetColor(material, "g_diffuse", i_Color);
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

	//	shdwAoGiVkTest's LPV room: a white back wall and floor, red and
	//	green side walls and a white sphere, the usual two lights and a
	//	GI-enabled projected light casting shadows; SSGI settings as its
	//	SSGI scene's
	void gi_room(gtSceneBuilder& io_Builder, const std::filesystem::path&)
	{
		io_Builder.LightsCastShadows(true);
		matMaterial* white = io_Builder.Keep(coloured(maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f)));
		matMaterial* red = io_Builder.Keep(coloured(maFloatRGBA(0.8f, 0.08f, 0.04f, 1.0f)));
		matMaterial* green = io_Builder.Keep(coloured(maFloatRGBA(0.08f, 0.7f, 0.08f, 1.0f)));
		const gtMesh plane = gtScene::Plane();
		casts_shadow(io_Builder.Add(plane, white, gtScene::Place(0, 0, -1.5f, 2.6f)));
		casts_shadow(io_Builder.Add(plane, red, side_wall(-1.6f, 1.0f)));
		casts_shadow(io_Builder.Add(plane, green, side_wall(1.6f, -1.0f)));
		//	the floor: the plane turned to face +Y
		maMatrix4x4 floor;
		floor.MakeScale(2.6f, 2.6f, 2.6f);
		floor.RotateBy(-maConstants::c_fPI_Div_2, maVector3d(1, 0, 0));
		floor.TranslateBy(0, -1.3f, -0.2f);
		casts_shadow(io_Builder.Add(plane, white, floor));
		casts_shadow(io_Builder.Add(gtScene::Sphere(16, 32), white, gtScene::Place(0.2f, -0.7f, -0.4f, 0.6f)));
		io_Builder.TwoLights();
		g3dProjectedLight* light = io_Builder.ProjectedLight(maPoint3d(0.6f, 1.0f, 2.5f), maPoint3d(-0.6f, -0.8f, -1.2f),
															 maFloatRGBA(1.0f, 0.95f, 0.9f, 1.0f), 256);
		light->SetGIEnabled(true);

		ssgiParams params;
		io_Builder.Scene().GetSSGISettings(params);
		params.m_GIRadius = 1.0f;
		params.m_GIRadiusFar = 2.0f;
		params.m_Contrast = 2.0f;
		params.m_LPVVolumeSize = 32;
		params.m_LPVRSMSize = 512;
		params.m_LPVIteration = 2;
		params.m_LPVScale = 1.0f;
		params.m_LPVGIFalloff = 1.0f;
		io_Builder.Scene().SetSSGISettings(params);
	}

	//	The GI preview renderer, SSGI or LPV (m_bGIInvalid); the projected
	//	light's shadow map rendered before each frame
	gtRenderOptions gi_preview(bool i_bLPV, bool i_bHDRAA)
	{
		gtRenderOptions options = preview([]() { return g3dSceneRendererCreate::CreateGlobalIlluminationRenderer(); }, i_bHDRAA,
										  [=](g3dPrefs::g3dRenderPrefs& p)
										  {
											  p.m_bEnableSSGI = true;
											  p.m_bGIInvalid = i_bLPV;
										  });
		options.m_BeforeFrame = [](gtSceneBuilder& io_Builder) { io_Builder.RenderTargets(); };
		return options;
	}
}

TEST(shdwPreviewRenderersVkTest, ShadowMaskRendererMatchesDX11)
{
	expect_shadows_only("ShadowMaskRendererMatchesDX11", []() { return g3dSceneRendererCreate::CreateShadowMaskRenderer(); });
}

TEST(shdwPreviewRenderersVkTest, IlluminationRendererMatchesDX11)
{
	expect_shadows_only("IlluminationRendererMatchesDX11", []() { return g3dSceneRendererCreate::CreateIlluminationRenderer(); });
}

TEST(shdwPreviewRenderersVkTest, AOPreviewRendererMatchesDX11)
{
	//	Exception table: SSAO max 3/255 (measured: one pixel, on the floor's
	//	silhouette against the background, 253 on DX11 and 250 on Vulkan,
	//	the same on every run). The preview shows the AO term alone on
	//	white, so one occlusion sample that lands the other side of the
	//	edge shows at full strength; the HDR renderer's SSAO cases
	//	(shdwAoGiVkTest) multiply it into the lit image and stay within
	//	1/255.
	gtRenderOptions ssao = ao_preview(false);
	ssao.m_MaxErr = 3 / 255.0f;
	expect_cases("AOPreviewRendererMatchesDX11",
	{
		{ "ssao", ao_scene(false), ssao },
		{ "ao_volumes", ao_scene(false), ao_preview(true) },
		//	sphere 3 left out by the stencil mask
		{ "ao_volumes_masked", ao_scene(true), ao_preview(true) },
	});
}

TEST(shdwPreviewRenderersVkTest, GIPreviewRendererMatchesDX11)
{
	const std::vector<gtCase> cases =
	{
		{ "ssgi", gi_room, gi_preview(false, false) },
		{ "lpv", gi_room, gi_preview(true, false) },
		{ "lpv_hdr_aa", gi_room, gi_preview(true, true) },
	};
	expect_cases("GIPreviewRendererMatchesDX11", cases);

	//	the GI shows: with GI off the preview is black (measured: SSGI
	//	brightens about 21000 pixels by more than 8/255, LPV about 19000)
	for (const gtCase& c : cases)
	{
		gtRenderOptions off = c.m_Options;
		const auto prefs = off.m_Prefs;
		off.m_Prefs = [prefs](g3dPrefs::g3dRenderPrefs& p)
		{
			prefs(p);
			p.m_bEnableSSGI = false;
		};
		gtExpectEffectShows(("GIPreviewRendererMatchesDX11/" + c.m_Name + "/gi_off").c_str(), c.m_Build, c.m_Options,
							c.m_Build, off, 256 * 256 / 50);
	}
}

namespace
{
	//	fgmtUVEditor's view: an orthographic camera whose sub-viewport shows
	//	-1..1 (the UV renderer draws the UV square there)
	void uv_camera(gtSceneBuilder& io_Builder)
	{
		camCamera& camera = io_Builder.Camera();
		camera.SetOrthographic(true);
		camera.SetClip(0, 1);
		camera.SetAspect(1);
		camera.SetSubViewport(1, -1, -1, 1);
	}

	//	The UV renderer draws the layer's root fragment (fgmtUVEditor makes
	//	the edited node the root): i_Mesh with i_pMaterial is added, and the
	//	root takes its fragment
	void uv_root(gtSceneBuilder& io_Builder, const gtMesh& i_Mesh, matMaterial* i_pMaterial)
	{
		g3dSceneNode* node = io_Builder.Add(i_Mesh, i_pMaterial, maMatrix4x4());
		io_Builder.Layer().GetRootNode()->SetFragment(const_cast<g3dFragment*>(node->GetFragment()));
		uv_camera(io_Builder);
	}

	//	A sphere with a soft checker texture
	void uv_sphere(gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		matTexture* checker = io_Builder.Keep(gtScene::CheckerTexture(i_Dir, 128, 128, 8));
		matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Tinted, { checker }));
		uv_root(io_Builder, gtScene::Sphere(12, 24), material);
	}

	//	A bump-mapped plane (a soft normal map) cut into quads, its UVs
	//	running from -0.5 to 1.5 (the baking transform clamps them to the
	//	square's edges)
	void uv_plane(gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		gtMesh plane = gtScene::Grid(6);
		for (maPoint2d& uv : plane.m_UVs)
			uv = maPoint2d(uv.GetX() * 2.0f - 0.5f, uv.GetY() * 2.0f - 0.5f);
		matTexture* checker = io_Builder.Keep(gtScene::CheckerTexture(i_Dir, 128, 128, 8));
		matTexture* normals = io_Builder.Keep(gtScene::NormalTexture(i_Dir, 128, 4));
		matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Tinted, { checker }));
		material->NormalsData().m_pNormalMap = normals;
		material->NormalsData().m_BumpScale = 1.0f;
		uv_root(io_Builder, plane, material);
	}

	//	rmpTextureMgr's ramp: a 2 x 2 square with a Ramp.fx material
	//	(effRampData, no specular) in a screen-space, z-sorted layer, drawn
	//	by the texture renderer into a render-target texture; the gradient
	//	is gtScene::RampTexture (the 1D RGBA32F texture rmpTextureMgr fills),
	//	without noise (rmpTextureMgr's noise map is never made)
	gtSceneFunction ramp_scene(int i_Shape, float i_UWave, float i_VWave)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path&)
		{
			io_Builder.Layer().SetSortMethod(g3dLayer::e_ZSort);
			io_Builder.Layer().SetModelSpace(g3dLayer::e_Screen);
			matTexture* gradient = io_Builder.Keep(gtScene::RampTexture(512));
			matMaterial* material = io_Builder.Keep(gtScene::DataMaterial("Ramp.fx", [=](effShaderData* io_pData)
			{
				effRampData* ramp = dynamic_cast<effRampData*>(io_pData);
				EXPECT_TRUE(ramp != NULL);
				if (!ramp)
					return;
				ramp->m_Texture = gradient;
				ramp->m_NoiseTexture = NULL;
				ramp->m_RampShape = i_Shape;
				ramp->m_RampInterpolation = 1;
				ramp->m_UWave = i_UWave;
				ramp->m_UWaveFreq = 0.5f;
				ramp->m_VWave = i_VWave;
				ramp->m_VWaveFreq = 0.5f;
				ramp->m_Noise = 0;
				ramp->m_NoiseFreq = 0.5f;
			}));
			material->SetHasSpecular(false);
			io_Builder.Add(gtScene::Plane(), material, maMatrix4x4());
		};
	}

	gtRenderOptions renderer_options(const create_fn& i_Create)
	{
		gtRenderOptions options;
		options.m_CreateRenderer = i_Create;
		return options;
	}
}

TEST(shdwPreviewRenderersVkTest, UVRendererMatchesDX11)
{
	const gtRenderOptions uv = renderer_options([]() { return g3dSceneRendererCreate::CreateUVRenderer(); });
	expect_cases("UVRendererMatchesDX11",
	{
		{ "sphere", uv_sphere, uv },
		{ "bump_plane", uv_plane, uv },
	});
}

TEST(shdwPreviewRenderersVkTest, TextureRendererMatchesDX11)
{
	const gtRenderOptions texture = renderer_options([]() { return g3dSceneRendererCreate::CreateTextureRenderer(); });
	expect_cases("TextureRendererMatchesDX11",
	{
		{ "linear_u", ramp_scene(0, 0, 0), texture },
		{ "linear_v_wave", ramp_scene(1, 0.1f, 0.05f), texture },
		{ "circular", ramp_scene(2, 0, 0), texture },
		{ "square_wave", ramp_scene(3, 0.05f, 0.1f), texture },
	});
}
