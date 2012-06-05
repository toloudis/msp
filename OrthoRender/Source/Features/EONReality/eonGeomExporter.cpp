/*****************************************************************************
**	eonGeomExporter.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/EONReality/eonGeomExporter.hpp"

#include "Core/Ch/chBinWriter.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsLocator.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Graphics/Eff/effPhongData.hpp"
#include "Graphics/G3d/g3dFragment.hpp"
#include "Graphics/Mat/matMaterial.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlReader.hpp"
#include "Tool/gui/guiMessageBox.hpp"

#ifdef EON_REALITY



//============================================================================
//============================================================================
namespace eonGeomExporter
{
	namespace
	{
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		const chDefs::Name c_ECVF = chDefs::MakeName('E', 'C', 'V', 'F');	// Convert a file
		const chDefs::Name c_FNAM = chDefs::MakeName('F', 'N', 'A', 'M');	// filename
		const chDefs::Name c_FRAG = chDefs::MakeName('F', 'R', 'A', 'G');	// fragment chunk
		//const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');	// fragment name
		const chDefs::Name c_MATD = chDefs::MakeName('M', 'A', 'T', 'D');	// material chunk
		const chDefs::Name c_UVST = chDefs::MakeName('U', 'V', 'S', 'T');	// uv transform

		//--------------------------------------------------------------------
		// Look for the material information in the override list by name
		//--------------------------------------------------------------------
		void find_material_by_name(const std::vector< shared_ptr<mdlMaterialInfo> >& i_MaterialOverrides,
									mdlMaterialInfo& io_MatInfo)
		{
			std::string mat_name = io_MatInfo.GetMaterialName();

			const int num_mats = i_MaterialOverrides.size();
			for (int i=0; i<num_mats; ++i)
			{
				if (i_MaterialOverrides[i]->GetMaterialName() == mat_name)
				{
					io_MatInfo = (*i_MaterialOverrides[i]);
					break;
				}
			}
		}

		//--------------------------------------------------------------------
		// Export instructions for how to convert the given file to EON.
		// These will be executed by a separate process later.
		//--------------------------------------------------------------------
		void convert_mx_file(const fsLocator& i_MachFile,
							 chWriter& o_Writer,
							 const std::vector< shared_ptr<mdlMaterialInfo> >& i_MaterialOverrides,
							 const std::vector<g3dFragment*>& i_Fragments,
							 std::map<const g3dFragment*, std::string> &io_TextureNameMap,
							 bool i_bAddRelativePath)
		{
			o_Writer.WriteChunkHeader(c_ECVF, 0, true);

			o_Writer.WriteChunkHeader(c_FNAM, 0, false);
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_MachFile, filename);
			chChunkParserUtil::Write( o_Writer, filename );
			o_Writer.FinishChunk();

			for (int fi=0; fi<i_Fragments.size(); fi++)
			{
				o_Writer.WriteChunkHeader(c_FRAG, 0, true);

				//o_Writer.WriteChunkHeader(c_NNAM, 0, false);
				//chChunkParserUtil::Write( writer, geo_data[i]->m_Name);
				//o_Writer.FinishChunk();

				//const int num_mats = geo_data[i]->m_Materials.size();
				//for (int mi=0; mi<num_mats; mi++)
				//{
				//	// Figure out which material to write
				//	mdlMaterialInfo mat_info = geo_data[i]->m_Materials[mi]->m_Info;
				//	if (!i_MaterialOverrides.empty())
				//	{
				//		find_material_by_name(i_MaterialOverrides, mat_info);
				//	}

				matMaterial *pMaterial = i_Fragments[fi]->GetMaterial();
				std::vector<std::string> texture_names;
				pMaterial->GetEffectData()->GetTextureNames(texture_names);

				std::string baseName("");
				//std::string lightmapName("Texture_baked.dds");
				if (!texture_names.empty())
				{
					baseName = texture_names[0];
				}

				// Get light map name from map
				std::string lightmapName = io_TextureNameMap[i_Fragments[fi]];

				// Write out basic material information for converter.
				// We could expand this chunk with more info, if needed,
				// and increment the version
				const int c_MATD_VERSION = 1;
				o_Writer.WriteChunkHeader(c_MATD, c_MATD_VERSION, false);
				chChunkParserUtil::Write( o_Writer, pMaterial->GetName());

				if (i_bAddRelativePath)
					chChunkParserUtil::Write( o_Writer, std::string("..\\Textures\\") + baseName);
				else
					chChunkParserUtil::Write( o_Writer, baseName);

				chChunkParserUtil::Write( o_Writer, lightmapName);
				// Version 1 adds OpenGL material colors,
				// needed especially when there is no base texture
				chChunkParserUtil::Write( o_Writer, pMaterial->GetEffectData()->GetDiffuse()); 
				maFloatRGBA full_ambient(1,1,1,1); // EON dark maps use ambient value
				chChunkParserUtil::Write( o_Writer, full_ambient); 
				//chChunkParserUtil::Write( o_Writer, pMaterial->GetEffectData()->GetAmbient()); 
				chChunkParserUtil::Write( o_Writer, pMaterial->GetEffectData()->GetSpecular()); 
				chChunkParserUtil::Write( o_Writer, pMaterial->GetEffectData()->GetEmissive()); 
				o_Writer.FinishChunk();

				// Write out the UV scaling attributes
				maPoint2d uv_scale, uv_translate;
				i_Fragments[fi]->GetUVBakeFactors(uv_scale, uv_translate);
				o_Writer.WriteChunkHeader(c_UVST, 0, false);
				chChunkParserUtil::Write( o_Writer, uv_scale);
				chChunkParserUtil::Write( o_Writer, uv_translate);
				o_Writer.FinishChunk();
			
				o_Writer.FinishChunk();	// c_FRAG
			}

			o_Writer.FinishChunk();	// c_ECVF
		}
	}

	//------------------------------------------------------------------------
	//  ConvertToEON() - read the given MachStudio Maya exported file
	//		and export to EON, setting up materials to match the
	//		names generated by the baked textures.
	//------------------------------------------------------------------------
	void  ConvertToEON(const fsLocator& i_MachFile,
						chWriter& o_Writer,
						const std::vector< shared_ptr<mdlMaterialInfo> >& i_MaterialOverrides,
						const std::vector<g3dFragment*>& i_Fragments,
						std::map<const g3dFragment*, std::string> &i_TextureNameMap,
						bool i_bRelativeTextureNames)
	{
		DBG_ASSERT0(i_MachFile.GetNumNames()>0, "Filename is empty");
		itString filename = i_MachFile.GetLastName();
		itString ext;
		filename.GetExtension(ext);

		if (ext == itString("mx") || ext == itString("MX"))
		{
			// Static list of fragments
			//const bool c_bAddRelativePath = true;
			convert_mx_file(i_MachFile, o_Writer, i_MaterialOverrides, 
				i_Fragments, i_TextureNameMap, i_bRelativeTextureNames);
		}
		else 
		{
			std::string message("Cannot export file type to EON: ");
			message += itStringUtil::GetStdString( filename );
			message += "Only static geometry (.mx) can be exported.";
			guiMessageBox::Show(message.c_str(), "Cannot export file type");
		}
	}

}

#endif	// EON_REALITY