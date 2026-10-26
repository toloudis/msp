/*****************************************************************************
**  effPbrVkTest.cpp
**
**      The Pbr material shader (Pbr.fx, glTF 2.0 metallic-roughness) on
**      Vulkan, where it has no DX11 counterpart:
**      - it is listed in the material picker and loads by name, with
**        hardware tessellation as Blinn's;
**      - its manifest's parameters are effPbrData's names, types and glTF
**        defaults;
**      - it draws in the HDR (default), materials preview and glow
**        renderers with no validation message: defaults, a mirror
**        (roughness 0, metallic 1), black metal, a dielectric, MASK with a
**        cut-out base color map, BLEND, and an emissive glowing sphere, lit
**        by shadow-casting lights and a shadowed projected light; then the
**        mirror and the black metal alone through the HDR renderer (a NaN
**        from the BRDF spreads through bloom and blanks the picture);
**      - its alpha mode decides whether a material is transparent;
**      - the material editor's generic path: the controls its manifest
**        gives (the alpha mode a combo box, the base color map's ExistVar),
**        and a material saved and read back with every value.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"
#include "gtTempDir.hpp"

#ifndef EFF_GLOWDATA_HPP
#include "Graphics/Eff/effGlowData.hpp"
#endif
#ifndef EFF_OPACITY_HPP
#include "Graphics/Eff/effOpacity.hpp"
#endif
#ifndef EFF_PBRDATA_HPP
#include "Graphics/Eff/effPbrData.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/Mat/matShaderEffect.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/Mat/matShaderMgr.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif
#ifndef MDL_MATERIALINFO_HPP
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#endif
#ifndef MTR_MATERIALSAVER_HPP
#include "Graphics/mtr/mtrMaterialSaver.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_FXEFFECTAPI_HPP
#include "GraphicsVulkan/Fx/fxEffectApi.hpp"
#endif
#ifndef CH_BINREADER_HPP
#include "Core/ch/chBinReader.hpp"
#endif
#ifndef CH_BINWRITER_HPP
#include "Core/ch/chBinWriter.hpp"
#endif
#ifndef GF_FILEBIN_HPP
#include "Core/gf/gfFileBin.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef FS_RESOURCEFINDERDIR_HPP
#include "Core/fs/fsResourceFinderDir.hpp"
#endif
#ifndef PRTY_COMBOBOXUIINFO_HPP
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PRTY_RANGEDFLOATUIINFO_HPP
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/it/itStringUtil.hpp"
#endif

#include <gtest/gtest.h>

#include <functional>
#include <string>
#include <typeinfo>
#include <vector>

namespace
{
	//	A Pbr material with the manifest's defaults, edited by i_Edit
	typedef std::function<void(matMaterial*, gtSceneBuilder&, const std::filesystem::path&)> edit_fn;

	struct PbrCase
	{
		const char*	m_pName;
		edit_fn		m_Edit;
	};

	const std::vector<PbrCase>& cases()
	{
		static const std::vector<PbrCase> c_Cases =
		{
			{ "defaults", [](matMaterial*, gtSceneBuilder&, const std::filesystem::path&) {} },
			{ "mirror", [](matMaterial* m, gtSceneBuilder&, const std::filesystem::path&)
				{
					gtScene::SetFloat(m, effPbrData::c_Roughness, 0.0f);
					gtScene::SetFloat(m, effPbrData::c_Metallic, 1.0f);
				} },
			{ "black metal", [](matMaterial* m, gtSceneBuilder&, const std::filesystem::path&)
				{
					gtScene::SetColor(m, effPbrData::c_BaseColor, maFloatRGBA(0, 0, 0, 1));
					gtScene::SetFloat(m, effPbrData::c_Metallic, 1.0f);
				} },
			{ "dielectric", [](matMaterial* m, gtSceneBuilder&, const std::filesystem::path&)
				{
					gtScene::SetColor(m, effPbrData::c_BaseColor, maFloatRGBA(0.8f, 0.2f, 0.2f, 1));
					gtScene::SetFloat(m, effPbrData::c_Metallic, 0.0f);
					gtScene::SetFloat(m, effPbrData::c_Roughness, 0.5f);
				} },
			{ "mask", [](matMaterial* m, gtSceneBuilder& b, const std::filesystem::path& dir)
				{
					gtScene::SetInt(m, effPbrData::c_AlphaMode, effPbrData::e_Mask);
					gtScene::SetFloat(m, effPbrData::c_Metallic, 0.0f);
					gtScene::SetTexture(m, effPbrData::c_BaseColorMap, b.Keep(gtScene::CheckerTexture(dir, 64, 64, 8, true)));
				} },
			{ "blend", [](matMaterial* m, gtSceneBuilder&, const std::filesystem::path&)
				{
					gtScene::SetInt(m, effPbrData::c_AlphaMode, effPbrData::e_Blend);
					gtScene::SetColor(m, effPbrData::c_BaseColor, maFloatRGBA(0.3f, 0.6f, 0.9f, 0.5f));
					gtScene::SetFloat(m, effPbrData::c_Metallic, 0.0f);
				} },
			{ "blend map", [](matMaterial* m, gtSceneBuilder& b, const std::filesystem::path& dir)
				{
					gtScene::SetInt(m, effPbrData::c_AlphaMode, effPbrData::e_Blend);
					gtScene::SetColor(m, effPbrData::c_BaseColor, maFloatRGBA(0.8f, 0.8f, 0.8f, 0.5f));
					gtScene::SetFloat(m, effPbrData::c_Metallic, 0.0f);
					gtScene::SetTexture(m, effPbrData::c_BaseColorMap, b.Keep(gtScene::CheckerTexture(dir, 64, 64, 8, true)));
				} },
			{ "emissive", [](matMaterial* m, gtSceneBuilder&, const std::filesystem::path&)
				{
					gtScene::SetColor(m, effPbrData::c_Emissive, maFloatRGBA(1.0f, 0.5f, 0.0f, 1));
					gtScene::SetFloat(m, effPbrData::c_EmissiveStrength, 4.0f);
					// Lit glow (the Glow technique's specular term), modest:
					// a white metal's specular is as bright as its colour,
					// and a large glow size floods the frame
					gtScene::SetFloat(m, effPbrData::c_Metallic, 0.0f);
					gtScene::SetFloat(m, effPbrData::c_Roughness, 0.6f);
					m->SetHasGlow(true);
					effGlowData& glow = m->GlowData();
					glow.m_GlowAmount = 1.0f;
					glow.m_GlowScale = maVector4d(1.0f, 0.6f, 0.3f, 1.0f);
					glow.m_GlowSize = 0.25f;
					glow.m_bConstantGlow = false;
				} },
		};
		return c_Cases;
	}

	//	Spheres with the cases named (all if empty) in a row, lit by
	//	shadow-casting lights and a shadowed projected light
	gtSceneFunction pbr_scene(const std::vector<std::string>& i_Names)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			io_Builder.LightsCastShadows(true);
			const gtMesh sphere = gtScene::Sphere(16, 32);
			std::vector<const PbrCase*> chosen;
			for (const PbrCase& c : cases())
			{
				if (i_Names.empty() || std::find(i_Names.begin(), i_Names.end(), c.m_pName) != i_Names.end())
					chosen.push_back(&c);
			}
			const float spacing = 0.65f;
			const float left = -0.5f * spacing * (float)(chosen.size() - 1);
			for (size_t i = 0; i < chosen.size(); i++)
			{
				matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
				chosen[i]->m_Edit(material, io_Builder, i_Dir);
				io_Builder.Add(sphere, material, gtScene::Place(left + spacing * (float)i, 0, 0, chosen.size() > 2 ? 0.3f : 1.0f));
			}
			io_Builder.TwoLights();
			io_Builder.ProjectedLight(maPoint3d(1.5f, 2.5f, 7.0f), maPoint3d(0, 0, 0), maFloatRGBA(1.0f, 0.95f, 0.85f, 1), 512);
		};
	}

	gtRenderOptions options(const std::function<g3dSceneRenderer*()>& i_Create, bool i_bGlow)
	{
		gtRenderOptions o;
		o.m_CreateRenderer = i_Create;
		o.m_Frames = 2;
		o.m_BeforeFrame = [](gtSceneBuilder& io_Builder) { io_Builder.RenderTargets(); };
		if (i_bGlow)
			o.m_Prefs = [](g3dPrefs::g3dRenderPrefs& p) { p.m_bEnableGlow = true; };
		return o;
	}

	//	The fraction of pixels that aren't (0, 0, 0, 0)
	float coverage(const gtPicture& i_Picture)
	{
		size_t covered = 0;
		for (size_t i = 0; i + 3 < i_Picture.m_Rgba.size(); i += 4)
		{
			if (i_Picture.m_Rgba[i] || i_Picture.m_Rgba[i + 1] || i_Picture.m_Rgba[i + 2] || i_Picture.m_Rgba[i + 3])
				covered++;
		}
		return i_Picture.m_Rgba.empty() ? 0.0f : (float)covered / (float)(i_Picture.m_Rgba.size() / 4);
	}

	void expect_drawn(const char* i_pWhat, const gtSceneFunction& i_Scene, const gtRenderOptions& i_Options, bool i_bCheckCoverage)
	{
		SCOPED_TRACE(i_pWhat);
		gtTempDir dir;
		const gtPicture picture = gtRenderScene(i_Scene, dir.GetPath(), i_Options);
		ASSERT_TRUE(picture.Valid());
		if (i_bCheckCoverage)
			EXPECT_GE(coverage(picture), 0.05f);
	}
}

TEST(effPbrVkTest, ListedAndLoadable)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		const matShaderInfo* found = NULL;
		for (const matShaderInfo& info : matShaderMgr::GetMaterialShaders())
		{
			if (itStringUtil::GetStdString(info.m_Name) == effPbrData::c_ShaderName)
				found = &info;
		}
		ASSERT_TRUE(found != NULL) << "Pbr.fx not in GetMaterialShaders()";
		EXPECT_EQ(found->m_UIName, "PBR");

		matShaderEffect* effect = matShaderMgr::GetEffect(fsLocator(itString(effPbrData::c_ShaderName)));
		ASSERT_TRUE(effect != NULL);
		EXPECT_TRUE(effect->HasHardwareTessellation());
	});
}

TEST(effPbrVkTest, ParamsMatchEffPbrData)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		std::unique_ptr<matMaterial> material(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
		const effShaderParams* params = material->GetShaderParams(0).get();
		ASSERT_TRUE(params != NULL);

		EXPECT_TRUE(effPbrData::FromParams(*params) == effPbrData());

		for (const char* name : { effPbrData::c_BaseColor, effPbrData::c_Emissive })
			EXPECT_TRUE(params->FindColorParam(name) != NULL) << name;
		for (const char* name : { effPbrData::c_Metallic, effPbrData::c_Roughness, effPbrData::c_OcclusionStrength,
								  effPbrData::c_EmissiveStrength, effPbrData::c_AlphaCutoff, effPbrData::c_ReflectionMapAngle })
			EXPECT_TRUE(params->FindFloatParam(name) != NULL) << name;
		for (const char* name : { effPbrData::c_BaseColorMap, effPbrData::c_MetallicRoughnessMap, effPbrData::c_OcclusionMap,
								  effPbrData::c_EmissiveMap, effPbrData::c_ReflectionMap })
			EXPECT_TRUE(params->FindTextureParam(name) != NULL) << name;
		EXPECT_TRUE(params->FindIntParam(effPbrData::c_AlphaMode) != NULL);

		// The base color is the material's diffuse color
		EXPECT_TRUE(params->m_pDiffuseColor == params->FindColorParam(effPbrData::c_BaseColor));
		EXPECT_TRUE(params->m_pDiffuseMap == params->FindTextureParam(effPbrData::c_BaseColorMap));
	});
}

TEST(effPbrVkTest, AlphaModeDecidesTransparency)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		// matMaterial::GetHasTransparency reads the parameters each time
		std::unique_ptr<matMaterial> pbr(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
		EXPECT_FALSE(pbr->GetHasTransparency()) << "OPAQUE";
		gtScene::SetInt(pbr.get(), effPbrData::c_AlphaMode, effPbrData::e_Mask);
		EXPECT_FALSE(pbr->GetHasTransparency()) << "MASK";
		// BLEND is transparent whatever the alpha: here the base color's
		// alpha is 1 and there are no maps
		gtScene::SetInt(pbr.get(), effPbrData::c_AlphaMode, effPbrData::e_Blend);
		EXPECT_TRUE(pbr->GetHasTransparency()) << "BLEND";
		gtScene::SetInt(pbr.get(), effPbrData::c_AlphaMode, effPbrData::e_Opaque);
		EXPECT_FALSE(pbr->GetHasTransparency()) << "OPAQUE again";

		std::unique_ptr<matMaterial> blinn(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Defaults));
		EXPECT_FALSE(blinn->GetHasTransparency()) << "Blinn";
	});
}

TEST(effPbrVkTest, OpacityFollowsAlphaMode)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		gtTempDir dir;
		matTexture* checker = gtScene::CheckerTexture(dir.GetPath(), 64, 64, 8, true);
		matTexture* web = gtScene::WebTexture(dir.GetPath(), 128, 8);
		ASSERT_TRUE(checker && web);
		{
			std::unique_ptr<matMaterial> pbr(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
			gtScene::SetColor(pbr.get(), effPbrData::c_BaseColor, maFloatRGBA(1, 1, 1, 0.6f));
			gtScene::SetTexture(pbr.get(), effPbrData::c_BaseColorMap, checker);
			gtScene::SetFloat(pbr.get(), effPbrData::c_AlphaCutoff, 0.4f);

			effOpacity o = effOpacity::Of(*pbr);
			EXPECT_EQ(NULL, o.m_pMap) << "OPAQUE";
			EXPECT_EQ(1.0f, o.m_Value) << "OPAQUE";
			EXPECT_FALSE(o.m_bCutOut) << "OPAQUE";

			gtScene::SetInt(pbr.get(), effPbrData::c_AlphaMode, effPbrData::e_Mask);
			o = effOpacity::Of(*pbr);
			EXPECT_EQ(checker, o.m_pMap) << "MASK";
			EXPECT_TRUE(o.m_bMapAlpha) << "MASK";
			EXPECT_FLOAT_EQ(0.6f, o.m_Value) << "MASK";
			EXPECT_TRUE(o.m_bCutOut) << "MASK";
			EXPECT_FLOAT_EQ(0.4f, o.m_Cutoff) << "MASK";

			gtScene::SetInt(pbr.get(), effPbrData::c_AlphaMode, effPbrData::e_Blend);
			o = effOpacity::Of(*pbr);
			EXPECT_EQ(checker, o.m_pMap) << "BLEND";
			EXPECT_TRUE(o.m_bMapAlpha) << "BLEND";
			EXPECT_FLOAT_EQ(0.6f, o.m_Value) << "BLEND";
			EXPECT_FALSE(o.m_bCutOut) << "BLEND";

			gtScene::SetInt(pbr.get(), effPbrData::c_AlphaMode, effPbrData::e_Mask);
			gtScene::SetTexture(pbr.get(), effPbrData::c_BaseColorMap, NULL);
			o = effOpacity::Of(*pbr);
			EXPECT_EQ(NULL, o.m_pMap) << "MASK without the map";
			EXPECT_FLOAT_EQ(0.6f, o.m_Value) << "MASK without the map";
			EXPECT_TRUE(o.m_bCutOut) << "MASK without the map";
		}
		{
			std::unique_ptr<matMaterial> blinn(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Defaults));
			effOpacity o = effOpacity::Of(*blinn);
			EXPECT_EQ(NULL, o.m_pMap) << "Blinn defaults";
			EXPECT_EQ(1.0f, o.m_Value) << "Blinn defaults";

			gtScene::SetTexture(blinn.get(), "transparencyMap", web);
			gtScene::SetFloat(blinn.get(), "g_transparency", 0.5f);
			o = effOpacity::Of(*blinn);
			EXPECT_EQ(web, o.m_pMap) << "Blinn transparency";
			EXPECT_FALSE(o.m_bMapAlpha) << "Blinn transparency";
			EXPECT_FLOAT_EQ(0.5f, o.m_Value) << "Blinn transparency";
			EXPECT_FALSE(o.m_bCutOut) << "Blinn transparency";
		}
		{
			std::unique_ptr<matMaterial> phong(gtScene::OpaquePhong("Phong.fx", gtScene::e_Defaults));
			effOpacity o = effOpacity::Of(*phong);
			EXPECT_EQ(NULL, o.m_pMap) << "Phong";
			EXPECT_EQ(1.0f, o.m_Value) << "Phong";
		}
		matTextureMgr::ReleaseTexture(web);
		matTextureMgr::ReleaseTexture(checker);
	});
}

TEST(effPbrVkTest, DrawsInEveryRendererWithoutValidationMessages)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		const gtSceneFunction all = pbr_scene({});
		expect_drawn("hdr", all, options(nullptr, false), true);
		expect_drawn("materials", all, options([]() { return g3dSceneRendererCreate::CreateMaterialsRenderer(); }, false), true);
		expect_drawn("glow", all, options([]() { return g3dSceneRendererCreate::CreateGlowRenderer(); }, true), false);
		expect_drawn("mirror", pbr_scene({ "mirror" }), options(nullptr, false), true);
		expect_drawn("black metal", pbr_scene({ "black metal" }), options(nullptr, false), true);
		gtRenderOptions aa = options(nullptr, false);
		aa.m_Prefs = [](g3dPrefs::g3dRenderPrefs& p) { p.m_bHDRAA = true; p.m_bEnableTransparent = true; };
		expect_drawn("blend map, hdr aa", pbr_scene({ "blend map" }), aa, true);	});
}

//============================================================================
//	Plan 2b: a PBR material's alpha in the passes that draw it with their own
//	shader. Each case renders two scenes on Vulkan that must be the same
//	image: a MASK plane against an opaque plane with the cut-out cells
//	removed from its mesh, or a BLEND PBR plane against a Blinn plane with
//	the same mask as its transparency map. On a mismatch both are saved
//	under GraphicsTestsImages/<test>/<case> (the PBR one as dx11.png, its
//	twin as vulkan.png).
//============================================================================
namespace
{
	typedef std::function<matMaterial*(gtSceneBuilder&, const std::filesystem::path&)> material_fn;

	enum Map { e_NoMap, e_ClearMap, e_SolidMap };

	//	A Pbr material: alpha mode, base color alpha, cutoff, and the base
	//	color map (the checker with or without its cleared cells)
	material_fn pbr(int i_Mode, float i_Alpha, float i_Cutoff, Map i_Map)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			matMaterial* m = io_Builder.Keep(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
			gtScene::SetInt(m, effPbrData::c_AlphaMode, i_Mode);
			gtScene::SetColor(m, effPbrData::c_BaseColor, maFloatRGBA(0.8f, 0.8f, 0.8f, i_Alpha));
			gtScene::SetFloat(m, effPbrData::c_Metallic, 0.0f);
			gtScene::SetFloat(m, effPbrData::c_AlphaCutoff, i_Cutoff);
			if (i_Map != e_NoMap)
				gtScene::SetTexture(m, effPbrData::c_BaseColorMap,
									io_Builder.Keep(gtScene::CheckerTexture(i_Dir, 64, 64, 8, i_Map == e_ClearMap)));
			return m;
		};
	}

	//	A Blinn material drawn like pbr(BLEND, i_Alpha, -, e_ClearMap) in the
	//	passes that don't run the material's shader: the checker as its
	//	diffuse map, the checker's alpha as its transparency map
	material_fn blinn_twin(float i_Alpha)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			matMaterial* m = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Defaults));
			gtScene::SetColor(m, "g_diffuse", maFloatRGBA(0.8f, 0.8f, 0.8f, 1.0f));
			gtScene::SetTexture(m, "diffuseMap", io_Builder.Keep(gtScene::CheckerTexture(i_Dir, 64, 64, 8, true)));
			gtScene::SetTexture(m, "transparencyMap", io_Builder.Keep(gtScene::CheckerMaskTexture(i_Dir, 64, 64, 8)));
			gtScene::SetFloat(m, "g_transparency", i_Alpha);
			return m;
		};
	}

	enum Plane { e_Full, e_Holes, e_OutOfView };

	//	The plane (Grid(8), its holed twin, or moved out of view so the
	//	node order stays: the materials renderer colours nodes by it) in
	//	front of a Phong sphere, two lights
	gtSceneFunction plane_scene(Plane i_Plane, const material_fn& i_Material)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			matMaterial* material = i_Material(io_Builder, i_Dir);
			io_Builder.Add(i_Plane == e_Holes ? gtScene::GridWithHoles(8) : gtScene::Grid(8), material,
						   gtScene::Place(i_Plane == e_OutOfView ? 100.0f : 0.0f, 0, 1.0f, 1.6f));
			matMaterial* behind = io_Builder.Keep(gtScene::ShaderMaterial("Phong.fx", gtScene::e_Tinted));
			io_Builder.Add(gtScene::Sphere(16, 32), behind, gtScene::Place(0.4f, 0, -1.0f, 1.0f));
			io_Builder.TwoLights();
		};
	}

	//	plane_scene with shadows: the plane (shadow dithering i_bDither,
	//	bias 0.4) and the sphere behind it cast and receive shadows, a wall
	//	behind them receives them, from the two lights and a projected light
	//	with a shadow map (the depth map renderer's). The plane's checker is
	//	magnified (mip 0), as in plane_scene: mips are made premultiplied, as
	//	WIC makes them for DX11, so a cleared cell's colour fades at its edges
	gtSceneFunction shadow_scene(Plane i_Plane, const material_fn& i_Material, bool i_bDither = false)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			io_Builder.LightsCastShadows(true);
			auto shadowed = [](g3dSceneNode* io_pNode)
			{
				g3dFragment* fragment = const_cast<g3dFragment*>(io_pNode->GetFragment());
				fragment->SetCastsShadow(true);
				fragment->SetReceivesShadow(true);
				return fragment;
			};
			matMaterial* material = i_Material(io_Builder, i_Dir);
			g3dFragment* plane = shadowed(io_Builder.Add(i_Plane == e_Holes ? gtScene::GridWithHoles(8) : gtScene::Grid(8), material,
														 gtScene::Place(i_Plane == e_OutOfView ? 100.0f : -0.3f, 0.2f, 1.0f, 1.6f)));
			if (i_bDither)
				plane->SetShadowDithering(true, 0.4f);
			matMaterial* grey = io_Builder.Keep(gtScene::ShaderMaterial("Phong.fx", gtScene::e_Tinted));
			shadowed(io_Builder.Add(gtScene::Sphere(16, 32), grey, gtScene::Place(0.6f, -0.3f, -0.5f, 0.6f)));
			shadowed(io_Builder.Add(gtScene::Plane(), grey, gtScene::Place(0, 0, -1.5f, 3.0f)));
			io_Builder.TwoLights();
			io_Builder.ProjectedLight(maPoint3d(-1.5f, 1.5f, 6.0f), maPoint3d(0, 0, 0), maFloatRGBA(1.0f, 0.95f, 0.85f, 1), 512);
		};
	}

	struct twin_renderer
	{
		const char*								m_pName;
		std::function<g3dSceneRenderer*()>		m_Create;
	};

	const std::vector<twin_renderer>& preview_renderers()
	{
		static const std::vector<twin_renderer> c_Renderers =
		{
			{ "depth", []() { return g3dSceneRendererCreate::CreateDepthRenderer(); } },
			{ "normal", []() { return g3dSceneRendererCreate::CreateNormalRenderer(); } },
			{ "materials", []() { return g3dSceneRendererCreate::CreateMaterialsRenderer(); } },
		};
		return c_Renderers;
	}

	gtRenderOptions twin_options(const std::function<g3dSceneRenderer*()>& i_Create, bool i_bHDRAA)
	{
		gtRenderOptions o;
		o.m_CreateRenderer = i_Create;
		o.m_Prefs = [=](g3dPrefs::g3dRenderPrefs& p) { p.m_bEnableTransparent = true; p.m_bHDRAA = i_bHDRAA; };
		return o;
	}

	//	i_A and i_B rendered with i_Options: the same image (i_bSame), or
	//	clearly not (the check that a case isn't vacuous)
	void expect_twins(const std::string& i_Name, const gtSceneFunction& i_A, const gtSceneFunction& i_B,
					  const gtRenderOptions& i_Options, bool i_bSame = true)
	{
		SCOPED_TRACE(i_Name);
		gtTempDir dir;
		const gtPicture a = gtRenderScene(i_A, dir.GetPath(), i_Options);
		const gtPicture b = gtRenderScene(i_B, dir.GetPath(), i_Options);
		ASSERT_TRUE(a.Valid() && b.Valid());
		const gtCompareResult result = gtCompareImages(a.m_Rgba.data(), b.m_Rgba.data(), a.m_Width, a.m_Height,
													   i_Options.m_MaxErr, 1 / 255.0f, i_Options.m_MaxPctOver);
		if (result.m_bPass != i_bSame)
		{
			const std::string saved = gtSaveComparison(::testing::UnitTest::GetInstance()->current_test_info()->name()
													   + std::string("/") + i_Name, a.m_Rgba.data(), b.m_Rgba.data(),
													   a.m_Width, a.m_Height);
			ADD_FAILURE() << (i_bSame ? "differ: " : "the same: ") << result.Describe() << "; saved to " << saved;
		}
	}
}

TEST(effPbrVkTest, MaskMatchesHoledGrid)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		using P = effPbrData;
		//	m_bEdges: against the holed mesh, whose edges and the alpha cut can
		//	disagree where a pixel centre is within float error of a cell
		//	boundary (measured: 2 pixels, 0.003%); any error is allowed on at
		//	most 0.1% of the pixels. "mask factor" uses cutoff 0.3 so that
		//	0.6 x the bilinear map alpha crosses it at the cell boundary (with
		//	0.5 the cut moves a third of a texel into the kept cells, as glTF
		//	says it should, and the mesh twin no longer matches).
		struct Case { const char* m_pName; gtSceneFunction m_Pbr, m_Twin; bool m_bEdges; };
		const std::vector<Case> cases =
		{
			{ "mask", plane_scene(e_Full, pbr(P::e_Mask, 1.0f, 0.5f, e_ClearMap)),
					  plane_scene(e_Holes, pbr(P::e_Opaque, 1.0f, 0.5f, e_ClearMap)), true },
			{ "mask factor", plane_scene(e_Full, pbr(P::e_Mask, 0.6f, 0.3f, e_ClearMap)),
							 plane_scene(e_Holes, pbr(P::e_Opaque, 0.6f, 0.3f, e_ClearMap)), true },
			{ "below cutoff", plane_scene(e_Full, pbr(P::e_Mask, 0.4f, 0.5f, e_NoMap)),
							  plane_scene(e_OutOfView, pbr(P::e_Mask, 0.4f, 0.5f, e_NoMap)), false },
			{ "cutoff above", plane_scene(e_Full, pbr(P::e_Mask, 0.6f, 0.7f, e_ClearMap)),
							  plane_scene(e_OutOfView, pbr(P::e_Mask, 0.6f, 0.7f, e_ClearMap)), false },
			{ "opaque ignores alpha", plane_scene(e_Full, pbr(P::e_Opaque, 0.2f, 0.5f, e_ClearMap)),
									  plane_scene(e_Full, pbr(P::e_Opaque, 0.2f, 0.5f, e_SolidMap)), false },
		};
		std::vector<twin_renderer> renderers = preview_renderers();
		renderers.insert(renderers.begin(), { "hdr", nullptr });
		for (const twin_renderer& r : renderers)
		{
			const gtRenderOptions options = twin_options(r.m_Create, false);
			for (const Case& c : cases)
			{
				gtRenderOptions o = options;
				if (c.m_bEdges)
					o.m_MaxErr = 1.0f;
				expect_twins(std::string(r.m_pName) + "/" + c.m_pName, c.m_Pbr, c.m_Twin, o);
			}
			// not vacuous: the holes show
			expect_twins(std::string(r.m_pName) + "/holes show", cases[0].m_Pbr,
						 plane_scene(e_Full, pbr(P::e_Opaque, 1.0f, 0.5f, e_ClearMap)), options, false);
		}
	});
}

TEST(effPbrVkTest, BlendMatchesBlinnInThePasses)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		const gtSceneFunction pbr_plane = plane_scene(e_Full, pbr(effPbrData::e_Blend, 0.5f, 0.5f, e_ClearMap));
		const gtSceneFunction blinn_plane = plane_scene(e_Full, blinn_twin(0.5f));
		for (const twin_renderer& r : preview_renderers())
		{
			for (bool aa : { false, true })
			{
				const std::string name = std::string(r.m_pName) + (aa ? "/hdr_aa" : "/no_aa");
				const gtRenderOptions options = twin_options(r.m_Create, aa);
				expect_twins(name, pbr_plane, blinn_plane, options);
				// not vacuous: the PBR plane's alpha shows
				expect_twins(name + "/alpha shows", pbr_plane,
							 plane_scene(e_Full, pbr(effPbrData::e_Opaque, 1.0f, 0.5f, e_ClearMap)), options, false);
			}
		}
	});
}

//	Plan 2b Task 4: shadow maps (the depth map renderer) cut a MASK
//	material's holes, and dithering doesn't thin what MASK keeps
TEST(effPbrVkTest, MaskShadowsMatchHoledGrid)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		using P = effPbrData;
		//	m_bBeautyOnly: the shadow mask and illumination renderers write the
		//	alpha they test as the surface's opacity, so a MASK surface's kept
		//	pixels are as opaque as its base alpha there (soft at the cut edges,
		//	0.6 with "dithered"), not fully opaque as the mesh twin's; its cut
		//	and its shadows are checked in the HDR renderer
		struct Case { const char* m_pName; gtSceneFunction m_Pbr, m_Twin; bool m_bEdges; bool m_bBeautyOnly; };
		const std::vector<Case> cases =
		{
			{ "mask", shadow_scene(e_Full, pbr(P::e_Mask, 1.0f, 0.5f, e_ClearMap)),
					  shadow_scene(e_Holes, pbr(P::e_Opaque, 1.0f, 0.5f, e_ClearMap)), true, true },
			{ "below cutoff", shadow_scene(e_Full, pbr(P::e_Mask, 0.4f, 0.5f, e_NoMap)),
							  shadow_scene(e_OutOfView, pbr(P::e_Mask, 0.4f, 0.5f, e_NoMap)), false, false },
			// (no "opaque ignores alpha" here: with shadow-casting lights an
			// opaque material's diffuse map alpha already darkens its lit
			// colour, Blinn's as PBR's; Task 3's MaskMatchesHoledGrid checks
			// it without shadows)
			// with dithering on, the kept alpha 0.6 is under 0.3 + the 0.4 bias
			{ "dithered", shadow_scene(e_Full, pbr(P::e_Mask, 0.6f, 0.3f, e_ClearMap), true),
						  shadow_scene(e_Holes, pbr(P::e_Opaque, 0.6f, 0.3f, e_ClearMap), true), true, true },
		};
		const std::vector<twin_renderer> renderers =
		{
			{ "hdr", nullptr },
			{ "shadow_mask", []() { return g3dSceneRendererCreate::CreateShadowMaskRenderer(); } },
			{ "illumination", []() { return g3dSceneRendererCreate::CreateIlluminationRenderer(); } },
		};
		for (const twin_renderer& r : renderers)
		{
			gtRenderOptions options = twin_options(r.m_Create, false);
			options.m_Frames = 2;
			options.m_BeforeFrame = [](gtSceneBuilder& io_Builder) { io_Builder.RenderTargets(); };
			for (const Case& c : cases)
			{
				if (c.m_bBeautyOnly && r.m_Create)
					continue;
				gtRenderOptions o = options;
				if (c.m_bEdges)
					o.m_MaxErr = 1.0f;
				expect_twins(std::string(r.m_pName) + "/" + c.m_pName, c.m_Pbr, c.m_Twin, o);
			}
			// not vacuous: the holes show, in the plane and its shadow
			expect_twins(std::string(r.m_pName) + "/holes show", cases[0].m_Pbr,
						 shadow_scene(e_Full, pbr(P::e_Opaque, 1.0f, 0.5f, e_ClearMap)), options, false);
		}
	});
}

//============================================================================
//	Plan 3: the material editor's generic path for a PBR material. The
//	effect's manifest gives the controls (BuildPrtyObject: the bag's
//	m_pPrtyUI UI infos, which pqt/pwx turn into widgets) and the .mab saves
//	the bag as an SHDT chunk; nothing in either is PBR-specific.
//============================================================================
namespace
{
	//	The UI info in i_pParams' editor for the property labelled i_Label
	const prtyPropertyUIInfo* ui_info(const effShaderParams* i_pParams, const std::string& i_Label)
	{
		for (const shared_ptr<prtyPropertyUIInfo>& info : i_pParams->m_pPrtyUI->GetList())
		{
			if (info->GetProperty(0)->GetPropertyName() == i_Label)
				return info.get();
		}
		return NULL;
	}

	//	The ExistVar i_pName as the material's bindings set it in its effect
	bool bound_bool(matMaterial* i_pMaterial, const char* i_pName)
	{
		shared_ptr<effShaderParams> params = i_pMaterial->GetShaderParams(0);
		gvk::effShaderBaseVk* effect = dynamic_cast<gvk::effShaderBaseVk*>(params->GetShader());
		EXPECT_TRUE(effect != NULL);
		if (!effect)
			return false;
		params->m_pShaderBindings->Bind();
		bool value = false;
		EXPECT_TRUE(effect->GetFxEffect()->GetVariableByName(i_pName)->AsScalar()->GetBool(&value)) << i_pName;
		return value;
	}

	fsLocator locator_of(const std::filesystem::path& i_Path)
	{
		fsLocator locator;
		fsFileUtil::UnicodeStringToLocator(itString(i_Path.wstring().c_str()), locator);
		return locator;
	}
}

TEST(effPbrVkTest, EditorControlsFromManifest)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		std::unique_ptr<matMaterial> pbr(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
		std::unique_ptr<matMaterial> blinn(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Defaults));
		const effShaderParams* params = pbr->GetShaderParams(0).get();
		const effShaderParams* blinn_params = blinn->GetShaderParams(0).get();
		ASSERT_TRUE(params->m_pPrtyUI != NULL);
		ASSERT_TRUE(blinn_params->m_pPrtyUI != NULL);

		//	The UI info classes Blinn's controls get: Base Color is a
		//	ColorPicker, Base Color Map a TextureFilePicker, the maps
		//	FilePickers and the numbers Sliders, as Blinn's Diffuse Color,
		//	Diffuse Map, Emissive Map and Roughness
		const prtyPropertyUIInfo* colour = ui_info(blinn_params, "Diffuse Color");
		const prtyPropertyUIInfo* texture = ui_info(blinn_params, "Diffuse Map");
		const prtyPropertyUIInfo* file = ui_info(blinn_params, "Emissive Map");
		const prtyPropertyUIInfo* slider = ui_info(blinn_params, "Roughness");
		ASSERT_TRUE(colour && texture && file && slider) << "Blinn's labels changed";

		enum Kind { e_Colour, e_Texture, e_File, e_Slider, e_Combo };
		struct Control { const char* m_pLabel; const char* m_pCategory; Kind m_Kind; float m_Max; };
		const std::vector<Control> expected =
		{
			{ "Base Color", "Base", e_Colour, 0 },
			{ "Base Color Map", "Base", e_Texture, 0 },
			{ "Metallic", "Metal/Rough", e_Slider, 1 },
			{ "Roughness", "Metal/Rough", e_Slider, 1 },
			{ "Metallic (B) Roughness (G) Map", "Metal/Rough", e_File, 0 },
			{ "Occlusion Map (R)", "Occlusion", e_File, 0 },
			{ "Occlusion Strength", "Occlusion", e_Slider, 1 },
			{ "Emissive Color", "Emissive", e_Colour, 0 },
			{ "Emissive Map", "Emissive", e_File, 0 },
			{ "Emissive Strength", "Emissive", e_Slider, 100 },
			{ "Alpha Mode", "Alpha", e_Combo, 0 },
			{ "Alpha Cutoff", "Alpha", e_Slider, 1 },
			{ "Reflection Map", "Reflection", e_File, 0 },
			{ "Map Angle", "Reflection", e_Slider, 360 },
		};
		const PropertyUIIList& list = params->m_pPrtyUI->GetList();
		ASSERT_EQ(expected.size(), list.size());
		size_t i = 0;
		for (const shared_ptr<prtyPropertyUIInfo>& info : list)
		{
			const Control& c = expected[i++];
			SCOPED_TRACE(c.m_pLabel);
			EXPECT_EQ(c.m_pLabel, info->GetProperty(0)->GetPropertyName());
			EXPECT_EQ(c.m_pCategory, info->GetCategory());
			const prtyPropertyUIInfo& got = *info;
			switch (c.m_Kind)
			{
				case e_Colour:	EXPECT_EQ(typeid(*colour), typeid(got)); break;
				case e_Texture:	EXPECT_EQ(typeid(*texture), typeid(got)); break;
				case e_File:	EXPECT_EQ(typeid(*file), typeid(got)); break;
				case e_Slider:
				{
					EXPECT_EQ(typeid(*slider), typeid(got));
					const prtyRangedFloatUIInfo* ranged = dynamic_cast<const prtyRangedFloatUIInfo*>(&got);
					ASSERT_TRUE(ranged != NULL);
					EXPECT_EQ(0.0f, ranged->GetMinimum());
					EXPECT_EQ(c.m_Max, ranged->GetMaximum());
					break;
				}
				case e_Combo:
				{
					const prtyComboBoxUIInfo* combo = dynamic_cast<const prtyComboBoxUIInfo*>(&got);
					ASSERT_TRUE(combo != NULL) << typeid(got).name();
					// index = the value the shader reads (effPbrData::AlphaMode)
					EXPECT_EQ(std::vector<std::string>({ "Opaque", "Mask", "Blend" }), combo->m_List);
					EXPECT_EQ(effPbrData::e_Opaque, 0);
					EXPECT_EQ(effPbrData::e_Mask, 1);
					EXPECT_EQ(effPbrData::e_Blend, 2);
					// and it edits the alpha mode parameter
					EXPECT_EQ(&params->FindIntParam(effPbrData::c_AlphaMode)->Property(), got.GetProperty(0));
					break;
				}
			}
		}

		//	ExistVar: a base color map picked by file name (the property the
		//	editor's chooser sets, then loaded as the editor and .mab loading
		//	do) turns hasBaseColorMap on; clearing the name turns it off
		gtTempDir dir;
		matTexture* checker = gtScene::CheckerTexture(dir.GetPath(), 64, 64, 8, true);
		ASSERT_TRUE(checker != NULL);
		const std::filesystem::path png = dir.GetPath() / "checker_64x64_8_clear.png";
		ASSERT_TRUE(std::filesystem::exists(png));
		effParamTexture* map = params->FindTextureParam(effPbrData::c_BaseColorMap);
		ASSERT_TRUE(map != NULL);
		const fsResourceFinderDir finder(locator_of(dir.GetPath()));
		EXPECT_FALSE(bound_bool(pbr.get(), "hasBaseColorMap")) << "no map";
		map->Property().SetValue(locator_of(png));
		EXPECT_TRUE(map->Load(finder));
		matTexture* loaded = map->GetTexture();
		EXPECT_TRUE(loaded != NULL);
		EXPECT_TRUE(bound_bool(pbr.get(), "hasBaseColorMap")) << "map picked";
		map->Property().SetValue(fsLocator());
		EXPECT_TRUE(map->Load(finder));
		EXPECT_EQ(NULL, map->GetTexture());
		EXPECT_FALSE(bound_bool(pbr.get(), "hasBaseColorMap")) << "map cleared";
		if (loaded)
			matTextureMgr::ReleaseTexture(loaded);
		matTextureMgr::ReleaseTexture(checker);
	});
}

TEST(effPbrVkTest, SavedAndReloaded)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		gtTempDir dir;
		//	Real files for the maps: reading resolves texture paths
		matTexture* checker = gtScene::CheckerTexture(dir.GetPath(), 64, 64, 8, true);
		ASSERT_TRUE(checker != NULL);
		matTextureMgr::ReleaseTexture(checker);
		const std::filesystem::path png = dir.GetPath() / "checker_64x64_8_clear.png";
		auto map_file = [&](const char* i_pName)
		{
			const std::filesystem::path path = dir.GetPath() / i_pName;
			std::filesystem::copy_file(png, path);
			std::string ansi;
			fsFileUtil::LocatorToANSIFilename(locator_of(path), ansi);
			return ansi;
		};

		//	Every field off its default (effPbrDataTest's NonDefault)
		effPbrData data;
		data.m_BaseColor.Set(0.2f, 0.4f, 0.6f, 0.5f);
		data.m_BaseColorMap = map_file("base.png");
		data.m_Metallic = 0.3f;
		data.m_Roughness = 0.7f;
		data.m_MetallicRoughnessMap = map_file("mr.png");
		data.m_OcclusionMap = map_file("ao.png");
		data.m_OcclusionStrength = 0.8f;
		data.m_Emissive.Set(0.1f, 0.2f, 0.3f, 1.0f);
		data.m_EmissiveMap = map_file("emissive.png");
		data.m_EmissiveStrength = 3.0f;
		data.m_AlphaMode = effPbrData::e_Mask;
		data.m_AlphaCutoff = 0.25f;
		data.m_ReflectionMap = map_file("refl.png");
		data.m_ReflectionMapAngle = 90.0f;

		//	The editor's bag (built by the effect), set to those values
		std::unique_ptr<matMaterial> pbr(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
		shared_ptr<effShaderParams> written = pbr->GetShaderParams(0);
		{
			effShaderParams values;
			data.AddToParams(values);
			written->SetMatchingParams(&values);
		}
		ASSERT_TRUE(effPbrData::FromParams(*written) == data) << "the values weren't set";

		//	Saved as every material is (the MATR chunk: shader name and the
		//	SHDT bag) and read back
		const fsLocator file = locator_of(dir.GetPath() / "material.bin");
		fsFileUtil::CreateFile(file);
		{
			mdlMaterialInfo info;
			info.SetShaderParams(written);
			gfFileBin write_file(file, fsFileStream::e_WriteOnly);
			chBinWriter writer(write_file);
			mtrMaterialSaver::WriteMaterialData(writer, info);
		}
		mdlMaterialInfo read_info;
		{
			gfFileBin read_file(file, fsFileStream::e_ReadOnly);
			chBinReader reader(read_file);
			chDefs::Name name;
			chDefs::Version version;
			chDefs::Size size;
			ASSERT_TRUE(reader.ReadChunkHeader(name, version, size));
			ASSERT_EQ(mtrMaterialSaver::GetChunkName(), name);
			mtrMaterialSaver::ReadMaterialData(reader, version, size, read_info);
			reader.FinishChunk();
		}
		shared_ptr<effShaderParams> read = read_info.GetShaderParams();
		ASSERT_TRUE(read != NULL);
		EXPECT_EQ(itString(effPbrData::c_ShaderName), read->GetShaderName().GetLastName());
		const effPbrData back = effPbrData::FromParams(*read);
		EXPECT_EQ(effPbrData::e_Mask, back.m_AlphaMode);
		EXPECT_EQ(data.m_BaseColorMap, back.m_BaseColorMap);
		EXPECT_EQ(data.m_ReflectionMap, back.m_ReflectionMap);
		EXPECT_TRUE(back == data);

		//	The loaded material's editor (the effect builds the read bag's
		//	UI) shows the saved values, not the manifest's defaults
		matShaderEffect* effect = read->GetShader();
		ASSERT_TRUE(effect != NULL);
		effect->BuildPrtyObject(read.get());
		ASSERT_TRUE(read->m_pPrtyUI != NULL);
		EXPECT_EQ(written->m_pPrtyUI->GetList().size(), read->m_pPrtyUI->GetList().size());
		EXPECT_TRUE(effPbrData::FromParams(*read) == data);
		const prtyPropertyUIInfo* mode = ui_info(read.get(), "Alpha Mode");
		ASSERT_TRUE(mode != NULL);
		EXPECT_EQ(&read->FindIntParam(effPbrData::c_AlphaMode)->Property(), mode->GetProperty(0));
		EXPECT_EQ(effPbrData::e_Mask, read->FindIntParam(effPbrData::c_AlphaMode)->GetProperty().GetValue());
		const prtyPropertyUIInfo* metallic = ui_info(read.get(), "Metallic");
		ASSERT_TRUE(metallic != NULL);
		EXPECT_EQ(&read->FindFloatParam(effPbrData::c_Metallic)->Property(), metallic->GetProperty(0));
		EXPECT_FLOAT_EQ(0.3f, read->FindFloatParam(effPbrData::c_Metallic)->GetProperty().GetValue());
	});
}
