/*****************************************************************************
**  gltfMaterialReadTest.cpp
**
**      Tests of gltfMaterialRead: a glTF material read into a plain
**      description (factors, alpha, maps, KHR_materials_emissive_strength,
**      KHR_texture_transform) with glTF's defaults for what is absent.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_MATERIALREAD_HPP
#include "ImportExport/gltf/import/private/gltfMaterialRead.hpp"
#endif

#include <gtest/gtest.h>

namespace
{
	void ExpectNoMap(const gltfTextureRef& i_Map)
	{
		EXPECT_EQ(-1, i_Map.m_Texture);
		EXPECT_FALSE(i_Map.m_bHasTransform);
	}

	void ExpectDefaults(const gltfMaterialDesc& i_Desc)
	{
		for (int i = 0; i < 4; i++)
			EXPECT_FLOAT_EQ(1, i_Desc.m_BaseColor[i]);
		EXPECT_FLOAT_EQ(1, i_Desc.m_Metallic);
		EXPECT_FLOAT_EQ(1, i_Desc.m_Roughness);
		for (int i = 0; i < 3; i++)
			EXPECT_FLOAT_EQ(0, i_Desc.m_Emissive[i]);
		EXPECT_FLOAT_EQ(1, i_Desc.m_EmissiveStrength);
		EXPECT_EQ(0, i_Desc.m_AlphaMode);
		EXPECT_FLOAT_EQ(0.5f, i_Desc.m_AlphaCutoff);
		EXPECT_FALSE(i_Desc.m_bDoubleSided);
		EXPECT_FLOAT_EQ(1, i_Desc.m_NormalScale);
		EXPECT_FLOAT_EQ(1, i_Desc.m_OcclusionStrength);
		ExpectNoMap(i_Desc.m_BaseColorMap);
		ExpectNoMap(i_Desc.m_MetallicRoughnessMap);
		ExpectNoMap(i_Desc.m_NormalMap);
		ExpectNoMap(i_Desc.m_OcclusionMap);
		ExpectNoMap(i_Desc.m_EmissiveMap);
		EXPECT_FALSE(i_Desc.m_bTransformsDiffer);
		EXPECT_FALSE(i_Desc.m_bOtherTexCoord);
	}

	// extensions["KHR_texture_transform"] with the given values
	tinygltf::Value TextureTransform(double i_OffsetU, double i_OffsetV, double i_Rotation, double i_ScaleU, double i_ScaleV)
	{
		tinygltf::Value::Object t;
		t["offset"] = tinygltf::Value(tinygltf::Value::Array{ tinygltf::Value(i_OffsetU), tinygltf::Value(i_OffsetV) });
		t["rotation"] = tinygltf::Value(i_Rotation);
		t["scale"] = tinygltf::Value(tinygltf::Value::Array{ tinygltf::Value(i_ScaleU), tinygltf::Value(i_ScaleV) });
		return tinygltf::Value(t);
	}
}

TEST(gltfMaterialRead, Defaults)
{
	ExpectDefaults(gltfMaterialRead::ReadDefault());
	ExpectDefaults(gltfMaterialRead::Read(tinygltf::Material()));
}

TEST(gltfMaterialRead, FactorsAndAlpha)
{
	tinygltf::Material m;
	m.pbrMetallicRoughness.baseColorFactor = { 0.2, 0.4, 0.6, 0.5 };
	m.pbrMetallicRoughness.metallicFactor = 0.3;
	m.pbrMetallicRoughness.roughnessFactor = 0.7;
	m.emissiveFactor = { 0.1, 0.2, 0.3 };
	m.alphaMode = "MASK";
	m.alphaCutoff = 0.25;
	m.doubleSided = true;

	gltfMaterialDesc d = gltfMaterialRead::Read(m);
	EXPECT_FLOAT_EQ(0.2f, d.m_BaseColor[0]);
	EXPECT_FLOAT_EQ(0.4f, d.m_BaseColor[1]);
	EXPECT_FLOAT_EQ(0.6f, d.m_BaseColor[2]);
	EXPECT_FLOAT_EQ(0.5f, d.m_BaseColor[3]);
	EXPECT_FLOAT_EQ(0.3f, d.m_Metallic);
	EXPECT_FLOAT_EQ(0.7f, d.m_Roughness);
	EXPECT_FLOAT_EQ(0.1f, d.m_Emissive[0]);
	EXPECT_FLOAT_EQ(0.2f, d.m_Emissive[1]);
	EXPECT_FLOAT_EQ(0.3f, d.m_Emissive[2]);
	EXPECT_EQ(1, d.m_AlphaMode);
	EXPECT_FLOAT_EQ(0.25f, d.m_AlphaCutoff);
	EXPECT_TRUE(d.m_bDoubleSided);

	m.alphaMode = "BLEND";
	m.pbrMetallicRoughness.baseColorFactor[3] = 0;
	d = gltfMaterialRead::Read(m);
	EXPECT_EQ(2, d.m_AlphaMode);
	EXPECT_FLOAT_EQ(1, d.m_BaseColor[3]);

	m.alphaMode = "SOMETHING";
	EXPECT_EQ(0, gltfMaterialRead::Read(m).m_AlphaMode);
}

TEST(gltfMaterialRead, Maps)
{
	tinygltf::Material m;
	m.pbrMetallicRoughness.baseColorTexture.index = 0;
	m.pbrMetallicRoughness.metallicRoughnessTexture.index = 1;
	m.normalTexture.index = 2;
	m.normalTexture.scale = 0.5;
	m.occlusionTexture.index = 3;
	m.occlusionTexture.strength = 0.8;
	m.emissiveTexture.index = 4;

	const gltfMaterialDesc d = gltfMaterialRead::Read(m);
	EXPECT_EQ(0, d.m_BaseColorMap.m_Texture);
	EXPECT_EQ(1, d.m_MetallicRoughnessMap.m_Texture);
	EXPECT_EQ(2, d.m_NormalMap.m_Texture);
	EXPECT_EQ(3, d.m_OcclusionMap.m_Texture);
	EXPECT_EQ(4, d.m_EmissiveMap.m_Texture);
	EXPECT_FLOAT_EQ(0.5f, d.m_NormalScale);
	EXPECT_FLOAT_EQ(0.8f, d.m_OcclusionStrength);
	EXPECT_FALSE(d.m_bTransformsDiffer);
	EXPECT_FALSE(d.m_bOtherTexCoord);
}

TEST(gltfMaterialRead, EmissiveStrength)
{
	tinygltf::Material m;
	tinygltf::Value::Object strength;
	strength["emissiveStrength"] = tinygltf::Value(4.0);
	m.extensions["KHR_materials_emissive_strength"] = tinygltf::Value(strength);

	EXPECT_FLOAT_EQ(4, gltfMaterialRead::Read(m).m_EmissiveStrength);
}

TEST(gltfMaterialRead, TextureTransform)
{
	tinygltf::Material m;
	m.pbrMetallicRoughness.baseColorTexture.index = 0;
	m.pbrMetallicRoughness.baseColorTexture.extensions["KHR_texture_transform"] = TextureTransform(0.1, 0.2, 0.5, 2, 3);

	const gltfMaterialDesc d = gltfMaterialRead::Read(m);
	const gltfTextureRef& t = d.m_BaseColorMap;
	EXPECT_TRUE(t.m_bHasTransform);
	EXPECT_FLOAT_EQ(0.1f, t.m_Offset[0]);
	EXPECT_FLOAT_EQ(0.2f, t.m_Offset[1]);
	EXPECT_FLOAT_EQ(0.5f, t.m_Rotation);
	EXPECT_FLOAT_EQ(2, t.m_Scale[0]);
	EXPECT_FLOAT_EQ(3, t.m_Scale[1]);
	EXPECT_EQ(0, t.m_TexCoord);
	EXPECT_EQ(&d.m_BaseColorMap, &d.TransformSource());
	EXPECT_FALSE(d.m_bTransformsDiffer);
	EXPECT_FALSE(d.m_bOtherTexCoord);

	// without a base color map, the first map present is the source
	tinygltf::Material e;
	e.emissiveTexture.index = 1;
	e.emissiveTexture.extensions["KHR_texture_transform"] = TextureTransform(0, 0, 0, 4, 4);
	const gltfMaterialDesc de = gltfMaterialRead::Read(e);
	EXPECT_EQ(&de.m_EmissiveMap, &de.TransformSource());
	EXPECT_FLOAT_EQ(4, de.TransformSource().m_Scale[0]);
}

TEST(gltfMaterialRead, TransformsThatDiffer)
{
	tinygltf::Material m;
	m.pbrMetallicRoughness.baseColorTexture.index = 0;
	m.pbrMetallicRoughness.baseColorTexture.extensions["KHR_texture_transform"] = TextureTransform(0.1, 0.2, 0.5, 2, 3);
	m.emissiveTexture.index = 1;

	// emissive map without a transform
	EXPECT_TRUE(gltfMaterialRead::Read(m).m_bTransformsDiffer);

	// emissive map with another one
	m.emissiveTexture.extensions["KHR_texture_transform"] = TextureTransform(0.1, 0.2, 0.5, 2, 4);
	EXPECT_TRUE(gltfMaterialRead::Read(m).m_bTransformsDiffer);

	// the same transform on both
	m.emissiveTexture.extensions["KHR_texture_transform"] = TextureTransform(0.1, 0.2, 0.5, 2, 3);
	EXPECT_FALSE(gltfMaterialRead::Read(m).m_bTransformsDiffer);
}

TEST(gltfMaterialRead, OtherTexCoord)
{
	tinygltf::Material m;
	m.normalTexture.index = 0;
	m.normalTexture.texCoord = 1;
	gltfMaterialDesc d = gltfMaterialRead::Read(m);
	EXPECT_TRUE(d.m_bOtherTexCoord);
	EXPECT_EQ(1, d.m_NormalMap.m_TexCoord);

	// KHR_texture_transform's texCoord wins over the texture info's
	tinygltf::Material k;
	k.pbrMetallicRoughness.baseColorTexture.index = 0;
	tinygltf::Value::Object t;
	t["texCoord"] = tinygltf::Value(1);
	k.pbrMetallicRoughness.baseColorTexture.extensions["KHR_texture_transform"] = tinygltf::Value(t);
	d = gltfMaterialRead::Read(k);
	EXPECT_TRUE(d.m_bOtherTexCoord);
	EXPECT_EQ(1, d.m_BaseColorMap.m_TexCoord);

	// a set other than 0 on a texture that isn't there doesn't count
	tinygltf::Material n;
	n.occlusionTexture.texCoord = 1;
	EXPECT_FALSE(gltfMaterialRead::Read(n).m_bOtherTexCoord);
}
