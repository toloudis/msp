/****************************************************************************\
**  gltfMaterialImport.cpp
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/gltf/import/private/gltfMaterialImport.hpp"
#include "ImportExport/gltf/import/private/gltfAccessorUtil.hpp"
#include "ImportExport/gltf/import/private/gltfImageExtract.hpp"
#include "ImportExport/gltf/import/private/gltfMaterialRead.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/Eff/effNormalsData.hpp"
#include "Graphics/Eff/effPbrData.hpp"
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/Mat/matShaderEffect.hpp"
#endif
#include "Graphics/Mat/matTexturePathUtil.hpp"
#include "Graphics/mdl/mdlMatInfo.hpp"

#include <algorithm>
#include <set>
#include <sstream>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace gltfMaterialImport
{

	namespace
	{
		//--------------------------------------------------------------------
		// get_texture_path - full path of the image file behind a glTF
		// texture index, resolved relative to the glTF file.
		// Returns "" if there is no usable external image file.
		//--------------------------------------------------------------------
		std::string get_texture_path(const tinygltf::Model* i_pModel, int i_TextureIndex,
									 const fsLocator& i_ContainingFile)
		{
			if (i_TextureIndex < 0 || i_TextureIndex >= (int)i_pModel->textures.size())
				return "";

			const tinygltf::Texture& tex = i_pModel->textures[i_TextureIndex];
			if (tex.source < 0 || tex.source >= (int)i_pModel->images.size())
				return "";

			const tinygltf::Image& image = i_pModel->images[tex.source];
			if (image.uri.empty() || tinygltf::IsDataURI(image.uri))
			{
				// Images embedded in a .glb or as data URIs are written out
				// to files, since our texture manager loads textures by name.
				return gltfImageExtract::ExtractEmbeddedImage(*i_pModel, tex.source, i_ContainingFile);
			}

			// uris are URL-encoded (e.g. spaces as %20)
			std::string texture_path;
			if (!tinygltf::URIDecode(image.uri, &texture_path, NULL))
				texture_path = image.uri;

			// Switch all slashes to the format fsFileUtil expects
			std::replace(texture_path.begin(), texture_path.end(), '/', '\\');

			fsLocator tex_loc;
			fsFileUtil::ANSIFilenameToLocator(texture_path, tex_loc);

			// Resolve single filenames and relative paths here:
			const bool bAllowSingleFilenameTextures = false;
			const bool bResolveAbsolutePaths = true;
			matTexturePathUtil::ResolveFullPath(tex_loc, i_ContainingFile,
				bAllowSingleFilenameTextures, bResolveAbsolutePaths);

			// Use fullpath to texture now
			std::string full_path;
			fsFileUtil::LocatorToANSIFilename(tex_loc, full_path);
			return full_path;
		}

		//--------------------------------------------------------------------
		// add_material - convert glTF material and add it to the table
		//--------------------------------------------------------------------
		void add_material(const tinygltf::Model* i_pModel, int i_MaterialIndex,
						  const std::string& i_Key,
						  mdlMatInfoTable& io_MaterialTable,
						  std::vector<matMaterial*>& o_Materials,
						  const bool i_bCreateMaterials,
						  const fsLocator& i_ContainingFile)
		{
			shared_ptr<mdlMatInfo> material_info(new mdlMatInfo());

			ConvertMaterial(i_pModel, i_MaterialIndex, i_Key, material_info->m_Info, i_ContainingFile);

			io_MaterialTable[i_Key] = material_info;

			if (i_bCreateMaterials)
			{
				// Create materials and load textures
				//material_info->LoadTextures(i_TextureFinder, o_Textures);
				material_info->CreateMaterial();
				o_Materials.push_back(material_info->m_pMaterial);
			}
		}
	}	// end of local namespace

	//--------------------------------------------------------------------
	// ConvertMaterial - a glTF metallic-roughness material as a Pbr.fx
	// material, plus the settings every material has
	//--------------------------------------------------------------------
	void ConvertMaterial(const tinygltf::Model* i_pModel, int i_MaterialIndex,
						 const std::string& i_Name,
						 mdlMaterialInfo& o_MatInfo,
						 const fsLocator& i_ContainingFile)
	{
		o_MatInfo.SetMaterialName(i_Name);

		const bool bDefault = (i_MaterialIndex < 0 || i_MaterialIndex >= (int)i_pModel->materials.size());
		const gltfMaterialDesc desc = bDefault ? gltfMaterialRead::ReadDefault()
											   : gltfMaterialRead::Read(i_pModel->materials[i_MaterialIndex]);

		effPbrData pbr_data;
		pbr_data.m_BaseColor.Set(desc.m_BaseColor[0], desc.m_BaseColor[1], desc.m_BaseColor[2], desc.m_BaseColor[3]);
		pbr_data.m_Metallic = desc.m_Metallic;
		pbr_data.m_Roughness = desc.m_Roughness;
		pbr_data.m_OcclusionStrength = desc.m_OcclusionStrength;
		pbr_data.m_Emissive.Set(desc.m_Emissive[0], desc.m_Emissive[1], desc.m_Emissive[2], 1);
		pbr_data.m_EmissiveStrength = desc.m_EmissiveStrength;
		pbr_data.m_AlphaMode = (effPbrData::AlphaMode)desc.m_AlphaMode;
		pbr_data.m_AlphaCutoff = desc.m_AlphaCutoff;
		pbr_data.m_BaseColorMap = get_texture_path(i_pModel, desc.m_BaseColorMap.m_Texture, i_ContainingFile);
		pbr_data.m_MetallicRoughnessMap = get_texture_path(i_pModel, desc.m_MetallicRoughnessMap.m_Texture, i_ContainingFile);
		pbr_data.m_OcclusionMap = get_texture_path(i_pModel, desc.m_OcclusionMap.m_Texture, i_ContainingFile);
		pbr_data.m_EmissiveMap = get_texture_path(i_pModel, desc.m_EmissiveMap.m_Texture, i_ContainingFile);

		shared_ptr<effShaderParams> eff_params(new effShaderParams());
		eff_params->SetShaderName(std::string(effPbrData::c_ShaderName));
		pbr_data.AddToParams(*eff_params);
		o_MatInfo.SetShaderParams(eff_params);

		// The normal map is the material's normals data, as for every material type
		const std::string normal_map = get_texture_path(i_pModel, desc.m_NormalMap.m_Texture, i_ContainingFile);
		if (!normal_map.empty())
		{
			effNormalsData& normals = o_MatInfo.NormalsParams();
			fsFileUtil::ANSIFilenameToLocator(normal_map, normals.m_NameNormalMap);
			normals.m_BumpScale = desc.m_NormalScale;
		}

		// One UV transform per material: the base color map's (else the first map's)
		const gltfTextureRef& source = desc.TransformSource();
		if (source.m_bHasTransform)
		{
			o_MatInfo.UVTransform() = effUVTransform::FromKhrTextureTransform(source.m_Offset[0], source.m_Offset[1],
				source.m_Rotation, source.m_Scale[0], source.m_Scale[1]);
		}
		if (desc.m_bTransformsDiffer)
			DBG_WARNING("glTF material " << i_Name << ": its textures have different KHR_texture_transforms; all use the base color map's (else the first map's)");
		if (desc.m_bOtherTexCoord)
			DBG_WARNING("glTF material " << i_Name << ": a texture uses a texture coordinate set other than 0; set 0 is used");
	}

	//--------------------------------------------------------------------
	// GetMaterialKey - unique material name for a glTF material index
	//--------------------------------------------------------------------
	std::string GetMaterialKey(const tinygltf::Model* i_pModel, int i_MaterialIndex)
	{
		return gltfMaterialRead::MaterialKey(*i_pModel, i_MaterialIndex);
	}

	//--------------------------------------------------------------------
	// Get materials used by the primitives of a mesh
	//--------------------------------------------------------------------
	void GetNodeMaterials(const tinygltf::Model* i_pModel, const tinygltf::Mesh* i_pMesh,
		mdlMatInfoTable& io_MaterialTable,
		//const fsResourceFinder& i_TextureFinder,
		std::vector<matMaterial*>& o_Materials,
		//std::vector<matTexture*>& o_Textures,
		const bool i_bCreateMaterials,
		const fsLocator& i_ContainingFile)
	{
		// loop over all primitives in mesh
		// each primitive references a material
		// 
		for (int p = 0; p < (int)i_pMesh->primitives.size(); ++p)
		{
			const tinygltf::Primitive& prim = i_pMesh->primitives[p];

			// Points and lines are not imported, don't create their materials
			if (!gltfAccessorUtil::IsTriangleMode(prim.mode))
				continue;

			int matIndex = prim.material;
			if (matIndex >= (int)i_pModel->materials.size())
			{
				DBG_WARNING("glTF primitive refers to missing material " << matIndex);
				matIndex = -1;
			}

			std::string key = GetMaterialKey(i_pModel, matIndex);
			if (io_MaterialTable.find(key) != io_MaterialTable.end())
				continue;

			// This material has not been added to the material table yet,
			// create material information for it now.
			// (index -1 is the glTF default material: white, fully metallic and rough)
			add_material(i_pModel, matIndex, key,
				io_MaterialTable, o_Materials, i_bCreateMaterials, i_ContainingFile);
		}

	}


}	// end of namespace

