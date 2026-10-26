/****************************************************************************\
**  gltfMaterialRead.cpp
**
**      see gltfMaterialRead.hpp
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_MATERIALREAD_HPP
#include "ImportExport/gltf/import/private/gltfMaterialRead.hpp"
#endif

#include <sstream>


namespace
{
	//--------------------------------------------------------------------
	// component i of a glTF factor array, or i_Default if missing
	//--------------------------------------------------------------------
	float get_factor(const std::vector<double>& i_Factor, size_t i, double i_Default)
	{
		return (float)((i < i_Factor.size()) ? i_Factor[i] : i_Default);
	}

	//--------------------------------------------------------------------
	// a number from an extension object, or i_Default if it isn't there
	//--------------------------------------------------------------------
	float get_number(const tinygltf::Value& i_Object, const char* i_Name, float i_Default)
	{
		if (!i_Object.IsObject() || !i_Object.Has(i_Name))
			return i_Default;
		const tinygltf::Value& v = i_Object.Get(i_Name);
		return v.IsNumber() ? (float)v.GetNumberAsDouble() : i_Default;
	}

	//--------------------------------------------------------------------
	// a two-number array from an extension object
	//--------------------------------------------------------------------
	void get_pair(const tinygltf::Value& i_Object, const char* i_Name, float o_Pair[2])
	{
		if (!i_Object.Has(i_Name))
			return;
		const tinygltf::Value& v = i_Object.Get(i_Name);
		if (!v.IsArray() || v.ArrayLen() != 2 || !v.Get(0).IsNumber() || !v.Get(1).IsNumber())
			return;
		o_Pair[0] = (float)v.Get(0).GetNumberAsDouble();
		o_Pair[1] = (float)v.Get(1).GetNumberAsDouble();
	}

	//--------------------------------------------------------------------
	// read_map - a texture reference with its KHR_texture_transform
	//--------------------------------------------------------------------
	gltfTextureRef read_map(int i_Index, int i_TexCoord, const tinygltf::ExtensionMap& i_Extensions)
	{
		gltfTextureRef ref;
		ref.m_Texture = i_Index;
		ref.m_TexCoord = i_TexCoord;

		auto it = i_Extensions.find("KHR_texture_transform");
		if (it != i_Extensions.end() && it->second.IsObject())
		{
			const tinygltf::Value& t = it->second;
			ref.m_bHasTransform = true;
			get_pair(t, "offset", ref.m_Offset);
			ref.m_Rotation = get_number(t, "rotation", 0);
			get_pair(t, "scale", ref.m_Scale);
			if (t.Has("texCoord") && t.Get("texCoord").IsNumber())
				ref.m_TexCoord = t.Get("texCoord").GetNumberAsInt();
		}
		return ref;
	}

	bool same_transform(const gltfTextureRef& a, const gltfTextureRef& b)
	{
		return a.m_bHasTransform == b.m_bHasTransform
			&& a.m_Offset[0] == b.m_Offset[0] && a.m_Offset[1] == b.m_Offset[1]
			&& a.m_Rotation == b.m_Rotation
			&& a.m_Scale[0] == b.m_Scale[0] && a.m_Scale[1] == b.m_Scale[1];
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const gltfTextureRef& gltfMaterialDesc::TransformSource() const
{
	const gltfTextureRef* maps[] = { &m_BaseColorMap, &m_MetallicRoughnessMap, &m_NormalMap, &m_OcclusionMap, &m_EmissiveMap };
	for (const gltfTextureRef* map : maps)
	{
		if (map->m_Texture >= 0)
			return *map;
	}
	return m_BaseColorMap;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace gltfMaterialRead
{
	namespace
	{
		// a material's glTF name or a simple one built from its index
		std::string base_name(const tinygltf::Model& i_Model, int i_MaterialIndex)
		{
			const std::string& name = i_Model.materials[i_MaterialIndex].name;
			if (!name.empty())
				return name;

			std::ostringstream base;
			base << "Material_" << i_MaterialIndex;
			return base.str();
		}
	}

	std::string MaterialKey(const tinygltf::Model& i_Model, int i_MaterialIndex)
	{
		if (i_MaterialIndex < 0 || i_MaterialIndex >= (int)i_Model.materials.size())
			return "DefaultMaterial";

		// Later materials that repeat an earlier name get their index appended
		const std::string base = base_name(i_Model, i_MaterialIndex);
		bool bDuplicate = (base == "DefaultMaterial");
		for (int i = 0; i < i_MaterialIndex && !bDuplicate; i++)
			bDuplicate = (base_name(i_Model, i) == base);

		if (!bDuplicate)
			return base;

		std::ostringstream key;
		key << base << "_" << i_MaterialIndex;
		return key.str();
	}

	gltfMaterialDesc Read(const tinygltf::Material& i_Material)
	{
		gltfMaterialDesc d;
		const tinygltf::PbrMetallicRoughness& pbr = i_Material.pbrMetallicRoughness;

		if (i_Material.alphaMode == "MASK")
			d.m_AlphaMode = 1;
		else if (i_Material.alphaMode == "BLEND")
			d.m_AlphaMode = 2;
		d.m_AlphaCutoff = (float)i_Material.alphaCutoff;

		for (int i = 0; i < 4; i++)
			d.m_BaseColor[i] = get_factor(pbr.baseColorFactor, i, 1.0);

		// Alpha here is bizarre. Different file formats use opacity or transparency.
		// I am going to force things that are alpha==0 to be 1 to handle
		// both usual cases, because who really wants a fully transparent object?
		if (d.m_AlphaMode == 2 && d.m_BaseColor[3] == 0)
			d.m_BaseColor[3] = 1;

		d.m_Metallic = (float)pbr.metallicFactor;
		d.m_Roughness = (float)pbr.roughnessFactor;
		for (int i = 0; i < 3; i++)
			d.m_Emissive[i] = get_factor(i_Material.emissiveFactor, i, 0.0);

		auto strength = i_Material.extensions.find("KHR_materials_emissive_strength");
		if (strength != i_Material.extensions.end())
			d.m_EmissiveStrength = get_number(strength->second, "emissiveStrength", 1);

		d.m_bDoubleSided = i_Material.doubleSided;
		d.m_NormalScale = (float)i_Material.normalTexture.scale;
		d.m_OcclusionStrength = (float)i_Material.occlusionTexture.strength;

		d.m_BaseColorMap = read_map(pbr.baseColorTexture.index, pbr.baseColorTexture.texCoord, pbr.baseColorTexture.extensions);
		d.m_MetallicRoughnessMap = read_map(pbr.metallicRoughnessTexture.index, pbr.metallicRoughnessTexture.texCoord, pbr.metallicRoughnessTexture.extensions);
		d.m_NormalMap = read_map(i_Material.normalTexture.index, i_Material.normalTexture.texCoord, i_Material.normalTexture.extensions);
		d.m_OcclusionMap = read_map(i_Material.occlusionTexture.index, i_Material.occlusionTexture.texCoord, i_Material.occlusionTexture.extensions);
		d.m_EmissiveMap = read_map(i_Material.emissiveTexture.index, i_Material.emissiveTexture.texCoord, i_Material.emissiveTexture.extensions);

		// MachStudio has one UV transform per material and one texture coordinate set
		const gltfTextureRef& source = d.TransformSource();
		const gltfTextureRef* maps[] = { &d.m_BaseColorMap, &d.m_MetallicRoughnessMap, &d.m_NormalMap, &d.m_OcclusionMap, &d.m_EmissiveMap };
		for (const gltfTextureRef* map : maps)
		{
			if (map->m_Texture < 0)
				continue;
			if (!same_transform(*map, source))
				d.m_bTransformsDiffer = true;
			if (map->m_TexCoord != 0)
				d.m_bOtherTexCoord = true;
		}
		return d;
	}

	gltfMaterialDesc ReadDefault()
	{
		return Read(tinygltf::Material());
	}
}
