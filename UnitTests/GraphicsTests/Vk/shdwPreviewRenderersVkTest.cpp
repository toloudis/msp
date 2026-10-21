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
**        planar reflective floor and a cube-mapped sphere.
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

#ifndef EFF_GLOWDATA_HPP
#include "Graphics/Eff/effGlowData.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
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

	//	Each scene with HDR AA off and on, compared; then each case is
	//	checked on DX11 to differ from the default renderer's image
	void expect_renderer(const char* i_pTest, const create_fn& i_Create,
						 const std::vector<std::pair<std::string, gtSceneFunction>>& i_Scenes,
						 const std::function<void(g3dPrefs::g3dRenderPrefs&)>& i_Prefs = nullptr, int i_Frames = 1)
	{
		std::vector<gtCase> cases;
		for (const auto& scene : i_Scenes)
		{
			for (bool aa : { false, true })
				cases.push_back({ scene.first + (aa ? "/hdr_aa" : "/no_aa"), scene.second, preview(i_Create, aa, i_Prefs, i_Frames) });
		}
		gtExpectSameImages(i_pTest, cases);
		for (const gtCase& c : cases)
		{
			gtExpectEffectShows((std::string(i_pTest) + "/" + c.m_Name).c_str(), c.m_Build, c.m_Options,
								c.m_Build, with_default(c.m_Options), 256 * 256 / 50);
		}
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
		matTexture* mask = io_Builder.Keep(gtScene::CheckerTexture(i_Dir, 64, 64, 4, 8));
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
	gtExpectSameImages("ReflectionOnlyRendererMatchesDX11", cases);
	for (const gtCase& c : cases)
	{
		gtExpectEffectShows(("ReflectionOnlyRendererMatchesDX11/" + c.m_Name).c_str(), c.m_Build, c.m_Options,
							c.m_Build, with_default(c.m_Options), 256 * 256 / 50);
	}
}
