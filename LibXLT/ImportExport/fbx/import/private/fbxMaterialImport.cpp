/****************************************************************************\
**  fbxMaterialImport.cpp
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/fbx/import/private/fbxMaterialImport.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Graphics/Eff/effPhongData.hpp"
#include "Graphics/Mat/matTexturePathUtil.hpp"
#include "Graphics/mdl/mdlMatInfo.hpp"

#include <set>

#undef FindResource


#ifdef USE_FBX_IMPORTEXPORT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace fbxMaterialImport
{

	namespace
	{
		//--------------------------------------------------------------------
		// Alpha here is bizarre. Different file formats use opacity or transparency.
		// I am going to force things that are alpha==0 to be 1 to handle
		// both usual cases, because who really wants a fully transparent object?
		//--------------------------------------------------------------------
		void fix_alpha(fbxDouble1 &io_Alpha)
		{
			if (io_Alpha == 0) io_Alpha = 1;
			//DBG_LOG("  Material Opacity: " << io_Alpha);
		}

		//--------------------------------------------------------------------
		// get_texture_from_property - returns "" if no texture found
		//--------------------------------------------------------------------
		std::string get_texture_from_property(KFbxProperty& i_TextureProperty)
		{
			if( i_TextureProperty.IsValid() )
			{
				//Here we have to check if it's layeredtextures, or just textures:
				int lLayeredTextureCount = i_TextureProperty.GetSrcObjectCount(KFbxLayeredTexture::ClassId);
				if (lLayeredTextureCount > 0)
				{
					for (int j=0; j<lLayeredTextureCount; ++j)
					{
						KFbxLayeredTexture *lLayeredTexture = KFbxCast <KFbxLayeredTexture>(i_TextureProperty.GetSrcObject(KFbxLayeredTexture::ClassId, j));
						int lNbTextures = lLayeredTexture->GetSrcObjectCount(KFbxTexture::ClassId);
						for(int k =0; k<lNbTextures; ++k)
						{
							KFbxTexture* pTexture = KFbxCast <KFbxTexture> (lLayeredTexture->GetSrcObject(KFbxTexture::ClassId,k));
							if (pTexture)
							{
								// Skipping all blend information
								return pTexture->GetFileName();
							}
						}
					}
				}
				else
				{
					//no layered texture simply get on the property
					int lNbTextures = i_TextureProperty.GetSrcObjectCount(KFbxTexture::ClassId);
					for(int j =0; j<lNbTextures; ++j)
					{
						KFbxTexture* pTexture = KFbxCast <KFbxTexture> (i_TextureProperty.GetSrcObject(KFbxTexture::ClassId,j));
						if (pTexture)
						{
							return pTexture->GetFileName();
						}
					}
				}
			}
			return "";
		}

		//--------------------------------------------------------------------
		// get material name or assign simple one
		//--------------------------------------------------------------------
		std::string get_material_name(KFbxSurfaceMaterial &i_Material)
		{
			// Give material some sort of name, no attempt to 
			// make it unique though.
			if (*i_Material.GetName())
				return i_Material.GetName();
			else
				return "Unnamed";
		}

		//--------------------------------------------------------------------
		// convert_material - convert FBX material to our material format
		//--------------------------------------------------------------------
		void convert_material(KFbxSurfaceMaterial &i_Material,
							  mdlMaterialInfo& o_MatInfo,
							  const fsLocator& i_ContainingFile)
		{
			o_MatInfo.SetMaterialName(get_material_name(i_Material));

			shared_ptr<effPhongData> phong_data(new effPhongData());

			// Always forcing ambient and emissive, could be controlled by a setting.
			phong_data->m_ColorAmbient.Set(1,1,1,1);
			phong_data->m_ColorEmissive.Set(0,0,0,1);

			// Default the specular values, could be altered if we find a phong material
			phong_data->m_ColorSpecular.Set(0,0,0,1);
			phong_data->m_SpecularPower = 1.0f;

			if (i_Material.GetClassId().Is(KFbxSurfaceLambert::ClassId) )
            {
                // We found a Lambert material. 
				KFbxSurfaceLambert& lambert_mat = static_cast<KFbxSurfaceLambert&>(i_Material);
                fbxDouble3 diffuse = lambert_mat.GetDiffuseColor().Get();

				// TransparencyFactor according to the FBX SDK docs should be :
				// 0=opaque, 1=transparent, so we invert it
				fbxDouble1 alpha = (1 -lambert_mat.GetTransparencyFactor().Get());
				fix_alpha(alpha);

				phong_data->m_ColorDiffuse.Set(diffuse[0],diffuse[1],diffuse[2],alpha);
				phong_data->m_Transparency = alpha;
            }
            else if (i_Material.GetClassId().Is(KFbxSurfacePhong::ClassId))
            {
                // We found a Phong material.  
				KFbxSurfacePhong& phong_mat = static_cast<KFbxSurfacePhong&>(i_Material);

                fbxDouble3 diffuse = phong_mat.GetDiffuseColor().Get();

				// TransparencyFactor according to the FBX SDK docs should be :
				// 0=opaque, 1=transparent, so we invert it
				fbxDouble1 alpha = (1 -phong_mat.GetTransparencyFactor().Get());
				fix_alpha(alpha);

				phong_data->m_ColorDiffuse.Set(diffuse[0],diffuse[1],diffuse[2],alpha);
				phong_data->m_Transparency = alpha;

                fbxDouble3 specular = phong_mat.GetSpecularColor().Get();
				phong_data->m_ColorSpecular.Set(specular[0],specular[1],specular[2],1);

				phong_data->m_SpecularPower = phong_mat.GetShininess().Get();
            }
			else
			{
				phong_data->m_ColorDiffuse.Set(1,1,1,1);
				phong_data->m_Transparency = 1;
			}

			// Loof for Diffuse Texture
            KFbxProperty diffuse_texture_property = i_Material.FindProperty(KFbxSurfaceMaterial::sDiffuse);
			std::string diffuse_texture = get_texture_from_property(diffuse_texture_property); 
			if (!diffuse_texture.empty())
			{
				// Switch all slashes to the format fsFileUtil expects
				std::replace(diffuse_texture.begin(), diffuse_texture.end(), '/','\\');

				fsLocator tex_loc;
				fsFileUtil::ANSIFilenameToLocator(diffuse_texture, tex_loc);

				// Get just the filename from the full path
				//itString tex_filename = tex_loc.GetLastName();
				//phong_data->m_NameDiffuse = itStringUtil::GetStdString(tex_filename);
				//DBG_LOG("  Material Texture: " << phong_data->m_NameDiffuse);

				// Resolve single filenames and relative paths here:
				const bool bAllowSingleFilenameTextures = false;
				const bool bResolveAbsolutePaths = true;
				matTexturePathUtil::ResolveFullPath(tex_loc, i_ContainingFile,
							  bAllowSingleFilenameTextures, bResolveAbsolutePaths);

				// Use fullpath to texture now
				phong_data->m_FullpathDiffuse = tex_loc;

				// If we have a texture on the diffuse channel, then
				// force the color to be white
				phong_data->m_ColorDiffuse.Set(1,1,1,1);
				phong_data->m_Transparency = 1;
			}

			shared_ptr<effShaderParams> phong_params(new effShaderParams()); 
			if (diffuse_texture.empty())
				phong_params->SetShaderName(itString("Simple.fx"));
			else
				phong_params->SetShaderName(itString("Phong.fx"));
			phong_data->AddToParams(*phong_params);

			o_MatInfo.SetShaderParams(phong_params);
		}
	}	// end of local namespace

	//--------------------------------------------------------------------
	// CreateSimpleMaterial - create simple grey phong material
	//--------------------------------------------------------------------
	void CreateSimpleMaterial(mdlMaterialInfo& o_MatInfo)
	{
		o_MatInfo.SetMaterialName( "SimpleMat" );

		shared_ptr<effPhongData> phong_data(new effPhongData());
		phong_data->m_ColorDiffuse.Set(1,1,1,1);
		phong_data->m_ColorAmbient.Set(1,1,1,1);
		phong_data->m_ColorEmissive.Set(0,0,0,1);
		phong_data->m_Transparency = 1;
		
		shared_ptr<effShaderParams> phong_params(new effShaderParams());
		phong_params->SetShaderName(itString("Simple.fx"));
		phong_data->AddToParams(*phong_params);

		o_MatInfo.SetShaderParams(phong_params);
	}

	//--------------------------------------------------------------------
	// Get materials from node containing a mesh
	//--------------------------------------------------------------------
	void GetNodeMaterials(KFbxNode& i_Node,
						  mdlMatInfoTable& io_MaterialTable,
						  //const fsResourceFinder& i_TextureFinder,
						  std::vector<matMaterial*>& o_Materials,
						  //std::vector<matTexture*>& o_Textures,
						  const bool i_bCreateMaterials,
						  const fsLocator& i_ContainingFile)
	{
		// materials are in the containing node
		const int num_materials = i_Node.GetMaterialCount();
		//DBG_LOG("Number of materials: " << num_materials);
		for (int m=0; m<num_materials; m++)
		{
			KFbxSurfaceMaterial* pMaterial = i_Node.GetMaterial(m);
			if (pMaterial)
			{
				std::string mat_name = get_material_name(*pMaterial);

				//DBG_LOG(" Material " << m << " named: " << mat_name);
				if (io_MaterialTable.find(mat_name) == io_MaterialTable.end())
				{
					// This material has not been added to the material table yet,
					// create material information for it now.

					shared_ptr<mdlMatInfo> material_info(new mdlMatInfo());
					
					//fbxMaterialImport::CreateSimpleMaterial(material_info->m_Info);
					//material_info->m_Info.SetMaterialName( mat_name );
					convert_material(*pMaterial, material_info->m_Info, i_ContainingFile);
					
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

#endif // USE_FBX_IMPORTEXPORT
