/****************************************************************************\
**  gltfMaterialRead.hpp
**
**      gltfMaterialRead reads a glTF material into a plain description:
**	its factors, alpha, maps and the extensions we take
**	(KHR_materials_emissive_strength, KHR_texture_transform), with glTF's
**	defaults for what is absent. It needs no Graphics types, so it is
**	tested without a GPU; gltfMaterialImport builds the material from it.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GLTF_MATERIALREAD_HPP
#error gltfMaterialRead.hpp multiply included
#endif
#define GLTF_MATERIALREAD_HPP

#include "tiny_gltf.h"

//----------------------------------------------------------------------------
//	A map of a material: which glTF texture, and how its coordinates are made
//----------------------------------------------------------------------------
struct gltfTextureRef
{
	int m_Texture = -1;						// glTF texture index, -1 for none
	int m_TexCoord = 0;						// as the file asks (KHR_texture_transform's texCoord wins)
	bool m_bHasTransform = false;			// KHR_texture_transform present
	float m_Offset[2] = { 0, 0 }, m_Rotation = 0, m_Scale[2] = { 1, 1 };
};

//----------------------------------------------------------------------------
//	A glTF metallic-roughness material
//----------------------------------------------------------------------------
struct gltfMaterialDesc
{
	float m_BaseColor[4] = { 1, 1, 1, 1 };
	float m_Metallic = 1, m_Roughness = 1;
	float m_Emissive[3] = { 0, 0, 0 };
	float m_EmissiveStrength = 1;			// KHR_materials_emissive_strength, else 1
	int m_AlphaMode = 0;					// 0 OPAQUE, 1 MASK, 2 BLEND (effPbrData::AlphaMode's values)
	float m_AlphaCutoff = 0.5f;
	bool m_bDoubleSided = false;
	float m_NormalScale = 1, m_OcclusionStrength = 1;
	gltfTextureRef m_BaseColorMap, m_MetallicRoughnessMap, m_NormalMap, m_OcclusionMap, m_EmissiveMap;
	bool m_bTransformsDiffer = false;		// the maps present don't all have the base color map's transform
	bool m_bOtherTexCoord = false;			// a map present asks for a set other than 0

	// the map whose transform the material takes: the base color map, else
	// the first map present (the base color map when there are none)
	const gltfTextureRef& TransformSource() const;
};

namespace gltfMaterialRead
{
	gltfMaterialDesc Read(const tinygltf::Material& i_Material);

	// the glTF default material (a primitive without one)
	gltfMaterialDesc ReadDefault();

	// the material's unique name: its glTF name, "Material_<index>" when it
	// has none, with "_<index>" appended when an earlier material has the
	// same; "DefaultMaterial" for an index out of range
	std::string MaterialKey(const tinygltf::Model& i_Model, int i_MaterialIndex);
}
