/****************************************************************************\
**  gltfMaterialImport.cpp
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/gltf/import/private/gltfMaterialImport.hpp"
#include "ImportExport/gltf/import/private/gltfAccessorUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Graphics/Eff/effBlinnData.hpp"
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
		// Alpha here is bizarre. Different file formats use opacity or transparency.
		// I am going to force things that are alpha==0 to be 1 to handle
		// both usual cases, because who really wants a fully transparent object?
		//--------------------------------------------------------------------
		void fix_alpha(double &io_Alpha)
		{
			if (io_Alpha == 0) io_Alpha = 1;
			//DBG_LOG("  Material Opacity: " << io_Alpha);
		}

		//--------------------------------------------------------------------
		// get component i of a glTF factor array, or i_Default if missing
		//--------------------------------------------------------------------
		double get_factor(const std::vector<double>& i_Factor, size_t i, double i_Default)
		{
			return (i < i_Factor.size()) ? i_Factor[i] : i_Default;
		}

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
				// Images embedded in a .glb or as data URIs have no file our
				// texture manager can load by name.
				DBG_WARNING("glTF image " << tex.source << " (" << image.name << ") is embedded; embedded textures are not supported yet");
				return "";
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
		// get material name or assign simple one
		//--------------------------------------------------------------------
		std::string get_material_name(const tinygltf::Material &i_Material)
		{
			// Give material some sort of name, no attempt to 
			// make it unique though.
			if (!i_Material.name.empty())
				return i_Material.name;
			else
				return "Unnamed";
		}

		//--------------------------------------------------------------------
		// convert_material - convert glTF metallic-roughness material to our
		// Blinn material. This is an approximation:
		//  base color -> diffuse color and texture
		//  metallic   -> specular color (blend of 4% grey and base color)
		//                and environment reflectivity
		//  roughness  -> shininess (1 - roughness)
		//  BLEND alpha -> transparency; MASK is treated as opaque
		//--------------------------------------------------------------------
		void convert_material(const tinygltf::Model* i_pModel, const tinygltf::Material &i_Material,
							  mdlMaterialInfo& o_MatInfo,
							  const fsLocator& i_ContainingFile)
		{
			o_MatInfo.SetMaterialName(get_material_name(i_Material));

			shared_ptr<effBlinnData> eff_data(new effBlinnData());

			const tinygltf::PbrMetallicRoughness & pbr = i_Material.pbrMetallicRoughness;
			const double r = get_factor(pbr.baseColorFactor, 0, 1.0);
			const double g = get_factor(pbr.baseColorFactor, 1, 1.0);
			const double b = get_factor(pbr.baseColorFactor, 2, 1.0);
			const double metallic = (std::min)((std::max)(pbr.metallicFactor, 0.0), 1.0);
			const double roughness = (std::min)((std::max)(pbr.roughnessFactor, 0.0), 1.0);

			// Only BLEND materials are see-through. MASK (alpha cutoff) is not
			// supported by our shaders, so it is drawn opaque.
			double alpha = 1;
			if (i_Material.alphaMode == "BLEND")
			{
				alpha = get_factor(pbr.baseColorFactor, 3, 1.0);
				fix_alpha(alpha);
			}
			eff_data->m_Transparency = (float)alpha;
			eff_data->m_ColorDiffuse.Set(r, g, b, alpha);

			// Always forcing ambient, could be controlled by a setting.
			eff_data->m_ColorAmbient.Set(1,1,1,1);
			eff_data->m_ColorEmissive.Set(get_factor(i_Material.emissiveFactor, 0, 0.0),
										  get_factor(i_Material.emissiveFactor, 1, 0.0),
										  get_factor(i_Material.emissiveFactor, 2, 0.0), 1);

			// Dielectrics reflect ~4% untinted, metals reflect their base color
			const double dielectric_spec = 0.04;
			eff_data->m_ColorSpecular.Set(dielectric_spec + (r - dielectric_spec) * metallic,
										  dielectric_spec + (g - dielectric_spec) * metallic,
										  dielectric_spec + (b - dielectric_spec) * metallic, 1);
			// Blinn.fx shininess is 0..1, larger is a tighter highlight
			eff_data->m_SpecularPower = (float)(1.0 - roughness);
			eff_data->m_DiffuseRoughness = (float)roughness;
			eff_data->m_Reflectivity = (float)metallic;

			// look for textures:
			// The base color factor multiplies the texture (Blinn.fx does the same)
			eff_data->m_NameDiffuse = get_texture_path(i_pModel, pbr.baseColorTexture.index, i_ContainingFile);
			eff_data->m_NameNormalMap = get_texture_path(i_pModel, i_Material.normalTexture.index, i_ContainingFile);
			if (!eff_data->m_NameNormalMap.empty())
				eff_data->m_BumpMapScale = (float)i_Material.normalTexture.scale;

			shared_ptr<effShaderParams> eff_params(new effShaderParams()); 
			eff_params->SetShaderName(itString("Blinn.fx"));
			eff_data->AddToParams(*eff_params);

			o_MatInfo.SetShaderParams(eff_params);
		}

		//--------------------------------------------------------------------
		// add_material - convert glTF material and add it to the table
		//--------------------------------------------------------------------
		void add_material(const tinygltf::Model* i_pModel, const tinygltf::Material &i_Material,
						  const std::string& i_Key,
						  mdlMatInfoTable& io_MaterialTable,
						  std::vector<matMaterial*>& o_Materials,
						  const bool i_bCreateMaterials,
						  const fsLocator& i_ContainingFile)
		{
			shared_ptr<mdlMatInfo> material_info(new mdlMatInfo());

			convert_material(i_pModel, i_Material, material_info->m_Info, i_ContainingFile);

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
	// GetMaterialKey - key into the material table for a glTF material index
	//--------------------------------------------------------------------
	std::string GetMaterialKey(int i_MaterialIndex)
	{
		if (i_MaterialIndex < 0)
			return "gltf#default";

		std::ostringstream key;
		key << "gltf#" << i_MaterialIndex;
		return key.str();
	}

	//--------------------------------------------------------------------
	// Get materials used by the primitives of a mesh
	//--------------------------------------------------------------------
	void GetNodeMaterials(const tinygltf::Model* i_pModel, tinygltf::Mesh* i_pMesh,
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

			std::string key = GetMaterialKey(matIndex);
			if (io_MaterialTable.find(key) != io_MaterialTable.end())
				continue;

			// This material has not been added to the material table yet,
			// create material information for it now.
			if (matIndex >= 0)
			{
				add_material(i_pModel, i_pModel->materials[matIndex], key,
					io_MaterialTable, o_Materials, i_bCreateMaterials, i_ContainingFile);
			}
			else
			{
				// glTF default material: white, fully metallic and rough
				tinygltf::Material default_material;
				default_material.name = "Default";
				add_material(i_pModel, default_material, key,
					io_MaterialTable, o_Materials, i_bCreateMaterials, i_ContainingFile);
			}
		}

	}


}	// end of namespace

