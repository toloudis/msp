/****************************************************************************\
**  gltfMaterialImport.cpp
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/gltf/import/private/gltfMaterialImport.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Graphics/Eff/effBlinnData.hpp"
#include "Graphics/Mat/matTexturePathUtil.hpp"
#include "Graphics/mdl/mdlMatInfo.hpp"

#include <algorithm>
#include <set>


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
		// get_texture_from_property - returns "" if no texture found
		//--------------------------------------------------------------------
		std::string get_texture(const tinygltf::Model* i_pModel, int index) {
			const tinygltf::Texture& tex = i_pModel->textures[index];
			int source = tex.source;
			const tinygltf::Image& image = i_pModel->images[source];
			return image.uri;
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
		// convert_material - convert FBX material to our material format
		//--------------------------------------------------------------------
		void convert_material(const tinygltf::Model* i_pModel, const tinygltf::Material &i_Material,
							  mdlMaterialInfo& o_MatInfo,
							  const fsLocator& i_ContainingFile)
		{
			o_MatInfo.SetMaterialName(get_material_name(i_Material));

			shared_ptr<effBlinnData> eff_data(new effBlinnData());

			eff_data->m_ColorDiffuse.Set(1, 1, 1, 1);
			eff_data->m_Transparency = 1;
			if (i_Material.alphaMode != "OPAQUE") {
				eff_data->m_Transparency = i_Material.alphaCutoff;
			}

			// Always forcing ambient and emissive, could be controlled by a setting.
			eff_data->m_ColorAmbient.Set(1,1,1,1);
			eff_data->m_ColorEmissive.Set(i_Material.emissiveFactor[0], i_Material.emissiveFactor[1], i_Material.emissiveFactor[2],1);

			// Default the specular values, could be altered if we find a phong material
			eff_data->m_ColorSpecular.Set(0,0,0,1);
			eff_data->m_SpecularPower = 1.0f;

			// now dig in and find diffuse and reflective properties
			const tinygltf::PbrMetallicRoughness & pbr = i_Material.pbrMetallicRoughness;
			eff_data->m_ColorDiffuse.Set(pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2], pbr.baseColorFactor[3]);
			eff_data->m_ColorSpecular.Set(pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2], pbr.baseColorFactor[3]);
			eff_data->m_DiffuseRoughness = pbr.roughnessFactor;
			eff_data->m_Reflectivity = pbr.metallicFactor;

			// look for textures:
			
			// Look for Diffuse Texture
			if (pbr.baseColorTexture.index != -1) {
				std::string diffuse_texture = get_texture(i_pModel, pbr.baseColorTexture.index);
				if (!diffuse_texture.empty())
				{
					// Switch all slashes to the format fsFileUtil expects
					std::replace(diffuse_texture.begin(), diffuse_texture.end(), '/', '\\');

					fsLocator tex_loc;
					fsFileUtil::ANSIFilenameToLocator(diffuse_texture, tex_loc);

					// Resolve single filenames and relative paths here:
					const bool bAllowSingleFilenameTextures = false;
					const bool bResolveAbsolutePaths = true;
					matTexturePathUtil::ResolveFullPath(tex_loc, i_ContainingFile,
						bAllowSingleFilenameTextures, bResolveAbsolutePaths);

					// Use fullpath to texture now
					std::string nameDiffuse;
					fsFileUtil::LocatorToANSIFilename(tex_loc, nameDiffuse);
					eff_data->m_NameDiffuse = nameDiffuse;
				}
			}

			shared_ptr<effShaderParams> eff_params(new effShaderParams()); 
			eff_params->SetShaderName(itString("Blinn.fx"));
			eff_data->AddToParams(*eff_params);

			o_MatInfo.SetShaderParams(eff_params);
		}
	}	// end of local namespace

	//--------------------------------------------------------------------
	// Get materials from node containing a mesh
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
		for (int p = 0; p < i_pMesh->primitives.size(); ++p)
		{
			const tinygltf::Primitive& prim = i_pMesh->primitives[p];
			int matIndex = prim.material;
			if (matIndex > -1) {
				const tinygltf::Material& mat = i_pModel->materials[matIndex];
				std::string mat_name = get_material_name(mat);
				if (io_MaterialTable.find(mat_name) == io_MaterialTable.end())
				{
					// This material has not been added to the material table yet,
					// create material information for it now.

					shared_ptr<mdlMatInfo> material_info(new mdlMatInfo());

					//gltfMaterialImport::CreateSimpleMaterial(material_info->m_Info);
					//material_info->m_Info.SetMaterialName( mat_name );
					convert_material(i_pModel, mat, material_info->m_Info, i_ContainingFile);

					io_MaterialTable[mat_name] = material_info;

					if (i_bCreateMaterials)
					{
						// Create materials and load textures
						//material_info->LoadTextures(i_TextureFinder, o_Textures);
						material_info->CreateMaterial();
						o_Materials.push_back(material_info->m_pMaterial);
					}

				}

			}
		}

	}


}	// end of namespace

