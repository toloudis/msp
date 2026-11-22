/*****************************************************************************
**  gltfMaterialImportTest.cpp
**
**      gltfMaterialImport::ConvertMaterial: a glTF material becomes a
**      Pbr.fx material (the effPbrData bag), with the normal map in the
**      material's normals data and KHR_texture_transform in its UV
**      transform; a primitive without a material gets glTF's default.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_MATERIALIMPORT_HPP
#include "ImportExport/gltf/import/private/gltfMaterialImport.hpp"
#endif
#ifndef EFF_PBRDATA_HPP
#include "Graphics/Eff/effPbrData.hpp"
#endif
#ifndef EFF_NORMALSDATA_HPP
#include "Graphics/Eff/effNormalsData.hpp"
#endif
#ifndef MDL_MATERIALINFO_HPP
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif

#include "gtTempDir.hpp"

#include <gtest/gtest.h>

#include <fstream>
#include <string>

namespace
{
	void WriteFile(const std::filesystem::path& i_Path)
	{
		std::ofstream out(i_Path, std::ios::out | std::ios::binary);
		out << "not really an image";
	}

	bool IsPbr(const mdlMaterialInfo& i_Info)
	{
		return i_Info.GetShaderParams()->GetShaderName().GetLastName() == effPbrData::c_ShaderName;
	}
}

TEST(gltfMaterialImport, PbrMaterialFromGltf)
{
	gtTempDir tmp;
	WriteFile(tmp.GetPath() / "base.png");
	WriteFile(tmp.GetPath() / "normal.png");

	tinygltf::Model model;
	model.images.resize(2);
	model.images[0].uri = "base.png";
	model.images[1].uri = "normal.png";
	model.textures.resize(2);
	model.textures[0].source = 0;
	model.textures[1].source = 1;

	tinygltf::Material m;
	m.pbrMetallicRoughness.baseColorFactor = { 0.2, 0.4, 0.6, 1 };
	m.pbrMetallicRoughness.metallicFactor = 0.3;
	m.pbrMetallicRoughness.roughnessFactor = 0.7;
	m.emissiveFactor = { 1, 0.5, 0 };
	tinygltf::Value::Object strength;
	strength["emissiveStrength"] = tinygltf::Value(4.0);
	m.extensions["KHR_materials_emissive_strength"] = tinygltf::Value(strength);
	m.alphaMode = "MASK";
	m.alphaCutoff = 0.25;
	m.pbrMetallicRoughness.baseColorTexture.index = 0;
	tinygltf::Value::Object transform;
	transform["scale"] = tinygltf::Value(tinygltf::Value::Array{ tinygltf::Value(2.0), tinygltf::Value(2.0) });
	m.pbrMetallicRoughness.baseColorTexture.extensions["KHR_texture_transform"] = tinygltf::Value(transform);
	m.normalTexture.index = 1;
	m.normalTexture.scale = 0.5;
	model.materials.push_back(m);

	const std::string base = (tmp.GetPath() / "base.png").string();
	const std::string normal = (tmp.GetPath() / "normal.png").string();

	mdlMaterialInfo info;
	gltfMaterialImport::ConvertMaterial(&model, 0, "m", info, tmp.GetLocator("scene.gltf"));

	EXPECT_EQ("m", info.GetMaterialName());
	EXPECT_TRUE(IsPbr(info));

	const effPbrData d = effPbrData::FromParams(*info.GetShaderParams());
	EXPECT_FLOAT_EQ(0.2f, d.m_BaseColor.GetRed());
	EXPECT_FLOAT_EQ(0.4f, d.m_BaseColor.GetGreen());
	EXPECT_FLOAT_EQ(0.6f, d.m_BaseColor.GetBlue());
	EXPECT_FLOAT_EQ(1, d.m_BaseColor.GetAlpha());
	EXPECT_FLOAT_EQ(0.3f, d.m_Metallic);
	EXPECT_FLOAT_EQ(0.7f, d.m_Roughness);
	EXPECT_FLOAT_EQ(1, d.m_Emissive.GetRed());
	EXPECT_FLOAT_EQ(0.5f, d.m_Emissive.GetGreen());
	EXPECT_FLOAT_EQ(0, d.m_Emissive.GetBlue());
	EXPECT_FLOAT_EQ(4, d.m_EmissiveStrength);
	EXPECT_EQ(effPbrData::e_Mask, d.m_AlphaMode);
	EXPECT_FLOAT_EQ(0.25f, d.m_AlphaCutoff);
	EXPECT_EQ(base, d.m_BaseColorMap);
	EXPECT_EQ("", d.m_MetallicRoughnessMap);
	EXPECT_EQ("", d.m_ReflectionMap);

	std::string normal_name;
	fsFileUtil::LocatorToANSIFilename(info.GetNormalsParams().m_NameNormalMap, normal_name);
	EXPECT_EQ(normal, normal_name);
	EXPECT_FLOAT_EQ(0.5f, info.GetNormalsParams().m_BumpScale);

	EXPECT_FLOAT_EQ(2, info.GetUVTransform().m_UScale);
	EXPECT_FLOAT_EQ(2, info.GetUVTransform().m_VScale);
	EXPECT_FLOAT_EQ(0, info.GetUVTransform().m_UTrans);
	EXPECT_FLOAT_EQ(0, info.GetUVTransform().m_UVAngle);

	// the glTF default material
	mdlMaterialInfo def;
	gltfMaterialImport::ConvertMaterial(&model, -1, "d", def, tmp.GetLocator("scene.gltf"));
	EXPECT_TRUE(IsPbr(def));
	EXPECT_TRUE(effPbrData::FromParams(*def.GetShaderParams()) == effPbrData());
	EXPECT_TRUE(def.GetNormalsParams().m_NameNormalMap.GetNumNames() == 0);
	EXPECT_FLOAT_EQ(1, def.GetUVTransform().m_UScale);
}
