/*****************************************************************************
**  effTraceMaterialVkTest.cpp
**
**      effTraceMaterial (Graphics/Eff), the material model the OptiX tracer
**	reads (docs/plans/2026-11-20-optix-materials.md, step 16). The
**	conversion uses no GPU, but materials' parameter bags are built by
**	their effects, so the tests run with the Vulkan layer up:
**	- Pbr.fx converts one to one (its occlusion map dropped);
**	- a material of each rule family (Blinn, Phong, Lambert, Simple,
**	  Cartoon, Skin, CarPaint; the built-in Phong data; strand hair not
**	  traced) gets the values its rule says;
**	- textures, the UV transform, the normal map and opacity;
**	- dropped data (displacement, glow, other layers, the shader's own
**	  effects) is listed and gives no values;
**	- the roughness formulas.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtScene.hpp"
#include "gtTempDir.hpp"

#ifndef EFF_TRACEMATERIAL_HPP
#include "Graphics/Eff/effTraceMaterial.hpp"
#endif
#ifndef EFF_PBRDATA_HPP
#include "Graphics/Eff/effPbrData.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <string>

namespace
{
	void expect_rgb(const maFloatRGBA& i_Expected, const maFloatRGBA& i_Got, const char* i_pWhat)
	{
		EXPECT_FLOAT_EQ(i_Expected.GetRed(), i_Got.GetRed()) << i_pWhat;
		EXPECT_FLOAT_EQ(i_Expected.GetGreen(), i_Got.GetGreen()) << i_pWhat;
		EXPECT_FLOAT_EQ(i_Expected.GetBlue(), i_Got.GetBlue()) << i_pWhat;
	}

	bool dropped(const effTraceMaterial& i_Material, const std::string& i_Start)
	{
		for (const std::string& what : i_Material.m_Dropped)
		{
			if (what.compare(0, i_Start.size(), i_Start) == 0)
			{
				return true;
			}
		}
		return false;
	}
}

TEST(effTraceMaterial, RoughnessFormulas)
{
	EXPECT_FLOAT_EQ(0.7f, effTraceMaterial::RoughnessFromShininess(0.3f));
	EXPECT_FLOAT_EQ(1.0f, effTraceMaterial::RoughnessFromShininess(-1.0f));
	EXPECT_FLOAT_EQ(0.0f, effTraceMaterial::RoughnessFromShininess(2.0f));
	//	GGX alpha = sqrt(2 / (n + 2)), roughness = sqrt(alpha)
	EXPECT_FLOAT_EQ(std::sqrt(std::sqrt(2.0f / 22.0f)), effTraceMaterial::RoughnessFromExponent(20.0f));
	EXPECT_FLOAT_EQ(1.0f, effTraceMaterial::RoughnessFromExponent(0.0f));

	effTraceMaterial m;
	m.m_Roughness = 0.25f;
	EXPECT_FLOAT_EQ(0.25f, m.RoughnessForGloss(0.5f));
	m.m_GlossMode = effTraceMaterial::e_GlossBlinn;
	m.m_Shininess = 0.8f;
	EXPECT_FLOAT_EQ(0.6f, m.RoughnessForGloss(0.5f));
	m.m_GlossMode = effTraceMaterial::e_GlossPhong;
	m.m_Shininess = 40.0f;
	EXPECT_FLOAT_EQ(effTraceMaterial::RoughnessFromExponent(20.0f), m.RoughnessForGloss(0.5f));
}

TEST(effTraceMaterial, PbrConvertsOneToOne)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		gtTempDir dir;
		gtSceneBuilder b;
		matTexture* base = b.Keep(gtScene::CheckerTexture(dir.GetPath(), 16, 16, 4));
		matTexture* mr = b.Keep(gtScene::CheckerTexture(dir.GetPath(), 16, 16, 2));
		matTexture* occlusion = b.Keep(gtScene::CheckerTexture(dir.GetPath(), 8, 8, 2));
		matMaterial* m = b.Keep(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
		gtScene::SetColor(m, effPbrData::c_BaseColor, maFloatRGBA(0.2f, 0.4f, 0.6f, 1));
		gtScene::SetFloat(m, effPbrData::c_Metallic, 0.3f);
		gtScene::SetFloat(m, effPbrData::c_Roughness, 0.7f);
		gtScene::SetColor(m, effPbrData::c_Emissive, maFloatRGBA(1.0f, 0.5f, 0.25f, 1));
		gtScene::SetFloat(m, effPbrData::c_EmissiveStrength, 2.0f);
		gtScene::SetTexture(m, effPbrData::c_BaseColorMap, base);
		gtScene::SetTexture(m, effPbrData::c_MetallicRoughnessMap, mr);
		gtScene::SetTexture(m, effPbrData::c_OcclusionMap, occlusion);

		const effTraceMaterial t = effTraceMaterial::Of(*m);
		EXPECT_TRUE(t.m_bTraced);
		EXPECT_EQ("Pbr.fx", t.m_Shader);
		expect_rgb(maFloatRGBA(0.2f, 0.4f, 0.6f, 1), t.m_BaseColor, "base colour");
		EXPECT_EQ(base, t.m_pBaseColorMap);
		EXPECT_FLOAT_EQ(0.3f, t.m_Metallic);
		EXPECT_FLOAT_EQ(0.7f, t.m_Roughness);
		EXPECT_EQ(mr, t.m_pMetallicRoughnessMap);
		EXPECT_EQ(effTraceMaterial::e_GlossNone, t.m_GlossMode);
		expect_rgb(maFloatRGBA(1.0f, 0.5f, 0.25f, 1), t.m_Emission, "emission");
		EXPECT_FLOAT_EQ(2.0f, t.m_EmissionStrength);
		EXPECT_FLOAT_EQ(1.0f, t.m_Specular);
		EXPECT_FLOAT_EQ(1.5f, t.m_Ior);
		EXPECT_TRUE(dropped(t, "the occlusion map"));
		const std::vector<matTexture*> textures = t.GetTextures();
		EXPECT_EQ(2u, textures.size());
		EXPECT_EQ(textures.end(), std::find(textures.begin(), textures.end(), occlusion)) << "the occlusion map is dropped";
	});
}

TEST(effTraceMaterial, BlinnBySemantic)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		gtTempDir dir;
		gtSceneBuilder b;
		matTexture* diffuse = b.Keep(gtScene::CheckerTexture(dir.GetPath(), 16, 16, 4));
		matTexture* gloss = b.Keep(gtScene::CheckerTexture(dir.GetPath(), 16, 16, 2));
		matMaterial* m = b.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Defaults));
		gtScene::SetColor(m, "g_diffuse", maFloatRGBA(0.5f, 0.25f, 0.125f, 1));
		gtScene::SetTexture(m, "diffuseMap", diffuse);
		gtScene::SetColor(m, "g_specular", maFloatRGBA(0.9f, 0.8f, 0.7f, 1));
		gtScene::SetFloat(m, "g_specularPower", 0.5f);
		gtScene::SetFloat(m, "g_shininess", 0.3f);
		gtScene::SetColor(m, "g_emissive", maFloatRGBA(0.1f, 0.2f, 0.3f, 1));
		gtScene::SetFloat(m, "g_emissiveIntensity", 3.0f);

		effTraceMaterial t = effTraceMaterial::Of(*m);
		expect_rgb(maFloatRGBA(0.5f, 0.25f, 0.125f, 1), t.m_BaseColor, "base colour");
		EXPECT_EQ(diffuse, t.m_pBaseColorMap);
		EXPECT_FLOAT_EQ(0.0f, t.m_Metallic);
		EXPECT_FLOAT_EQ(0.7f, t.m_Roughness);
		EXPECT_FLOAT_EQ(0.5f, t.m_Specular);
		expect_rgb(maFloatRGBA(0.9f, 0.8f, 0.7f, 1), t.m_SpecularColor, "specular colour");
		expect_rgb(maFloatRGBA(0.1f, 0.2f, 0.3f, 1), t.m_Emission, "emission");
		EXPECT_FLOAT_EQ(3.0f, t.m_EmissionStrength);
		EXPECT_FLOAT_EQ(1.5f, t.m_Ior) << "Blinn's g_IOR isn't an index";
		EXPECT_EQ(effTraceMaterial::e_GlossNone, t.m_GlossMode);

		//	a gloss map: its red scales the shininess
		gtScene::SetTexture(m, "glossMap", gloss);
		t = effTraceMaterial::Of(*m);
		EXPECT_EQ(gloss, t.m_pGlossMap);
		EXPECT_EQ(effTraceMaterial::e_GlossBlinn, t.m_GlossMode);
		EXPECT_FLOAT_EQ(0.3f, t.m_Shininess);
		EXPECT_FLOAT_EQ(0.85f, t.RoughnessForGloss(0.5f));
	});
}

TEST(effTraceMaterial, ReflectiveBlinnReflectsThroughKhrSpecular)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		gtTempDir dir;
		gtSceneBuilder b;
		matTexture* cube = b.Keep(gtScene::CheckerTexture(dir.GetPath(), 16, 16, 4));
		matTexture* factor = b.Keep(gtScene::CheckerTexture(dir.GetPath(), 16, 16, 2));
		matMaterial* m = b.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Defaults));
		gtScene::SetFloat(m, "g_shininess", 0.3f);		// a broad highlight: roughness 0.7
		gtScene::SetFloat(m, "g_reflectivity", 0.8f);
		gtScene::SetFloat(m, "fresnelBias", 0.25f);
		gtScene::SetFloat(m, "reflBlur", 1.5f);

		//	no reflection map: the highlight's lobe, unchanged
		effTraceMaterial t = effTraceMaterial::Of(*m);
		EXPECT_FLOAT_EQ(0.7f, t.m_Roughness);
		EXPECT_FLOAT_EQ(1.0f, t.m_ReflectionRoughness);

		//	with one: F0 = reflectivity x bias, F90 = reflectivity, as sharp as the map
		gtScene::SetTexture(m, "cubeMap", cube);
		gtScene::SetTexture(m, "reflectFactorMap", factor);
		t = effTraceMaterial::Of(*m);
		EXPECT_FLOAT_EQ(0.8f, t.m_Specular) << "F90";
		const float r = (t.m_Ior - 1) / (t.m_Ior + 1);
		EXPECT_NEAR(0.2f, r * r * t.m_SpecularColor.GetRed() * t.m_Specular, 1e-5f) << "F0";
		EXPECT_FLOAT_EQ(t.m_SpecularColor.GetRed(), t.m_SpecularColor.GetGreen());
		EXPECT_FLOAT_EQ(t.m_SpecularColor.GetRed(), t.m_SpecularColor.GetBlue());
		EXPECT_FLOAT_EQ(0.25f, t.m_Roughness) << "reflBlur / 6";
		EXPECT_FLOAT_EQ(0.25f, t.m_ReflectionRoughness);
		EXPECT_EQ(factor, t.m_pSpecularColorMap);
	});
}

TEST(effTraceMaterial, PhongLambertSimpleCartoon)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		gtSceneBuilder b;
		matMaterial* phong = b.Keep(gtScene::ShaderMaterial("Phong.fx", gtScene::e_Defaults));
		gtScene::SetFloat(phong, "g_shininess", 30.0f);
		gtScene::SetFloat(phong, "g_specularPower", 2.0f);
		effTraceMaterial t = effTraceMaterial::Of(*phong);
		EXPECT_FLOAT_EQ(effTraceMaterial::RoughnessFromExponent(30.0f), t.m_Roughness);
		EXPECT_FLOAT_EQ(1.0f, t.m_Specular) << "the specular factor is clamped to 1";

		matMaterial* lambert = b.Keep(gtScene::ShaderMaterial("Lambert.fx", gtScene::e_Defaults));
		gtScene::SetColor(lambert, "g_emissive", maFloatRGBA(0.25f, 0.5f, 0.75f, 1));
		t = effTraceMaterial::Of(*lambert);
		EXPECT_FLOAT_EQ(0.0f, t.m_Specular);
		EXPECT_FLOAT_EQ(1.0f, t.m_Roughness);
		expect_rgb(maFloatRGBA(0.25f, 0.5f, 0.75f, 1), t.m_Emission, "Lambert emission");

		for (const char* shader : { "Simple.fx", "Cartoon.fx" })
		{
			matMaterial* m = b.Keep(gtScene::ShaderMaterial(shader, gtScene::e_Defaults));
			gtScene::SetColor(m, "g_diffuse", maFloatRGBA(0.3f, 0.6f, 0.9f, 1));
			t = effTraceMaterial::Of(*m);
			expect_rgb(maFloatRGBA(0.3f, 0.6f, 0.9f, 1), t.m_BaseColor, shader);
			EXPECT_FLOAT_EQ(0.0f, t.m_Specular) << shader;
			expect_rgb(maFloatRGBA(0, 0, 0, 1), t.m_Emission, shader);
		}
		EXPECT_TRUE(dropped(t, "toon shading"));
	});
}

TEST(effTraceMaterial, SkinCarPaintAndHair)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		gtTempDir dir;
		gtSceneBuilder b;
		matTexture* normals = b.Keep(gtScene::NormalTexture(dir.GetPath(), 16, 2));
		matMaterial* skin = b.Keep(gtScene::ShaderMaterial("Skin.fx", gtScene::e_Defaults));
		gtScene::SetColor(skin, "g_specColor", maFloatRGBA(0.4f, 0.5f, 0.6f, 1));
		gtScene::SetFloat(skin, "g_specPower", 0.2f);
		gtScene::SetFloat(skin, "g_specGloss", 15.0f);
		gtScene::SetTexture(skin, "normalTex", normals);
		gtScene::SetFloat(skin, "g_bumpMapScale", 2.0f);
		effTraceMaterial t = effTraceMaterial::Of(*skin);
		expect_rgb(maFloatRGBA(0.4f, 0.5f, 0.6f, 1), t.m_SpecularColor, "skin specular");
		EXPECT_FLOAT_EQ(0.2f, t.m_Specular);
		EXPECT_FLOAT_EQ(effTraceMaterial::RoughnessFromExponent(15.0f), t.m_Roughness);
		EXPECT_EQ(normals, t.m_pNormalMap);
		EXPECT_FLOAT_EQ(2.0f, t.m_NormalScale);
		EXPECT_TRUE(dropped(t, "subsurface scattering"));

		matMaterial* paint = b.Keep(gtScene::ShaderMaterial("CarPaint.fx", gtScene::e_Defaults));
		gtScene::SetFloat(paint, "g_shininess", 0.75f);
		t = effTraceMaterial::Of(*paint);
		EXPECT_FLOAT_EQ(0.25f, t.m_Roughness);
		EXPECT_TRUE(dropped(t, "flakes"));

		matMaterial* hair = b.Keep(gtScene::ShaderMaterial("Hair_SH.fx", gtScene::e_Defaults));
		EXPECT_FALSE(effTraceMaterial::Of(*hair).m_bTraced);
	});
}

TEST(effTraceMaterial, BuiltInPhongData)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		gtSceneBuilder b;
		matMaterial* m = b.Keep(gtScene::DataMaterial("default", [](effShaderData* io_pData)
		{
			effPhongData* phong = dynamic_cast<effPhongData*>(io_pData);
			ASSERT_TRUE(phong);
			phong->m_ColorDiffuse = maFloatRGBA(0.6f, 0.3f, 0.1f, 1);
			phong->m_ColorSpecular = maFloatRGBA(0.2f, 0.2f, 0.2f, 1);
			phong->m_ColorEmissive = maFloatRGBA(0.0f, 0.1f, 0.0f, 1);
			phong->m_SpecularPower = 50.0f;
		}));
		const effTraceMaterial t = effTraceMaterial::Of(*m);
		EXPECT_EQ("default", t.m_Shader);
		expect_rgb(maFloatRGBA(0.6f, 0.3f, 0.1f, 1), t.m_BaseColor, "base colour");
		expect_rgb(maFloatRGBA(0.2f, 0.2f, 0.2f, 1), t.m_SpecularColor, "specular");
		expect_rgb(maFloatRGBA(0.0f, 0.1f, 0.0f, 1), t.m_Emission, "emission");
		EXPECT_FLOAT_EQ(effTraceMaterial::RoughnessFromExponent(50.0f), t.m_Roughness);
	});
}

TEST(effTraceMaterial, UVTransformNormalMapAndOpacity)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		gtTempDir dir;
		gtSceneBuilder b;
		matTexture* normals = b.Keep(gtScene::NormalTexture(dir.GetPath(), 16, 2));
		matTexture* base = b.Keep(gtScene::CheckerTexture(dir.GetPath(), 16, 16, 4, true));
		matMaterial* m = b.Keep(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
		gtScene::SetTexture(m, effPbrData::c_BaseColorMap, base);
		gtScene::SetInt(m, effPbrData::c_AlphaMode, effPbrData::e_Mask);
		gtScene::SetFloat(m, effPbrData::c_AlphaCutoff, 0.4f);
		m->NormalsData().m_pNormalMap = normals;
		m->NormalsData().m_BumpScale = 0.5f;
		effUVTransform& uv = m->UVTransform(0);
		uv.m_UScale = 2.0f;
		uv.m_VScale = 3.0f;
		uv.m_UTrans = 0.25f;
		uv.m_VTrans = -0.5f;
		uv.m_UVAngle = 30.0f;

		const effTraceMaterial t = effTraceMaterial::Of(*m);
		EXPECT_EQ(normals, t.m_pNormalMap);
		EXPECT_FLOAT_EQ(0.5f, t.m_NormalScale);
		EXPECT_EQ(base, t.m_Opacity.m_pMap);
		EXPECT_TRUE(t.m_Opacity.m_bMapAlpha);
		EXPECT_TRUE(t.m_Opacity.m_bCutOut);
		EXPECT_FLOAT_EQ(0.4f, t.m_Opacity.m_Cutoff);

		//	the 2x3 transform is the raster's: (u, v, 0, 1) times the
		//	row-vector matrix MakeUVTransform transposes
		maMatrix4x4 matrix = uv.MakeUVTransform();
		matrix.Transpose();
		const float points[3][2] = { { 0.0f, 0.0f }, { 0.3f, 0.7f }, { -1.5f, 2.0f } };
		for (const float* point : points)
		{
			const float u = point[0], v = point[1];
			const float expected_u = u * matrix(0, 0) + v * matrix(1, 0) + matrix(3, 0);
			const float expected_v = u * matrix(0, 1) + v * matrix(1, 1) + matrix(3, 1);
			EXPECT_NEAR(expected_u, t.m_UVTransform[0] * u + t.m_UVTransform[1] * v + t.m_UVTransform[2], 1e-5f);
			EXPECT_NEAR(expected_v, t.m_UVTransform[3] * u + t.m_UVTransform[4] * v + t.m_UVTransform[5], 1e-5f);
		}
		//	with no rotation it is scale, then offset
		uv.m_UVAngle = 0.0f;
		const effTraceMaterial straight = effTraceMaterial::Of(*m);
		EXPECT_NEAR(2.0f * 0.5f + 0.25f, straight.m_UVTransform[0] * 0.5f + straight.m_UVTransform[1] * 0.5f + straight.m_UVTransform[2], 1e-6f);
		EXPECT_NEAR(3.0f * 0.5f - 0.5f, straight.m_UVTransform[3] * 0.5f + straight.m_UVTransform[4] * 0.5f + straight.m_UVTransform[5], 1e-6f);
	});
}

TEST(effTraceMaterial, DroppedDataIsListed)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		gtSceneBuilder b;
		matMaterial* m = b.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Defaults));
		effTraceMaterial t = effTraceMaterial::Of(*m);
		EXPECT_FALSE(dropped(t, "material layers"));
		EXPECT_FALSE(dropped(t, "glow"));
		m->AddMaterialLayer();
		t = effTraceMaterial::Of(*m);
		EXPECT_TRUE(dropped(t, "material layers after the first"));
		m->RemoveExtraMaterialLayers();
		t = effTraceMaterial::Of(*m);
		EXPECT_FALSE(dropped(t, "material layers"));
	});
}
