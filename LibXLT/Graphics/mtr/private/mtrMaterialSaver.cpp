/*****************************************************************************
**	mtrMaterialSaver.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mtr/mtrMaterialSaver.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chBinWriter.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/Ch/chExceptionX.hpp"
#include "Core/ch/chXMLReader.hpp"
#include "Core/ch/chXMLWriter.hpp"
#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFile.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/gf/gfFileEnum.hpp"
#include "Core/gf/gfFileUtil.hpp"
#include "Core/gf/gfFileX.hpp"
#include "Core/gf/gfFileXML.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/eff/effDisplacementData.hpp"
#include "Graphics/eff/effDisplacementDataParser.hpp"
#include "Graphics/eff/effGlowData.hpp"
#include "Graphics/eff/effGlowDataParser.hpp"
#include "Graphics/eff/effNormalsData.hpp"
#include "Graphics/eff/effNormalsDataParser.hpp"
#include "Graphics/eff/effRendermanOverrideData.hpp"
#include "Graphics/eff/effRendermanOverrideDataParser.hpp"
#include "Graphics/eff/effOutlineData.hpp"
#include "Graphics/eff/effOutlineDataParser.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/eff/effPhongDataParser.hpp"
#include "Graphics/eff/effReflData.hpp"
#include "Graphics/eff/effWaterData.hpp"
#include "Graphics/g2d/g2dPFD.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/Mat/matThumbnailParser.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Graphics/mdl/private/mdlMaterialParser.hpp"
#include "Graphics/mtr/mtrThumbnailData.hpp"


//============================================================================
//============================================================================
namespace
{
const chDefs::Name c_MATR = chDefs::MakeName('M', 'A', 'T', 'R');
const chDefs::Name c_MNAM = chDefs::MakeName('M', 'N', 'A', 'M');
const chDefs::Name c_MBAS = chDefs::MakeName('M', 'B', 'A', 'S');
const chDefs::Name c_TXLY = chDefs::MakeName('T', 'X', 'L', 'Y');
const chDefs::Name c_VSHD = chDefs::MakeName('V', 'S', 'H', 'D');
const chDefs::Name c_MANI = chDefs::MakeName('M', 'A', 'N', 'I');
const chDefs::Name c_MBSC = chDefs::MakeName('M', 'B', 'S', 'C');
const chDefs::Name c_MRFL = chDefs::MakeName('M', 'R', 'F', 'L');
const chDefs::Name c_MTBL = chDefs::MakeName('M', 'T', 'B', 'L');
const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
const chDefs::Name c_MTID = chDefs::MakeName('M', 'T', 'I', 'D');
const chDefs::Name c_EFCT = chDefs::MakeName('E', 'F', 'C', 'T');
const chDefs::Name c_MGLO = chDefs::MakeName('M', 'G', 'L', 'O');
const chDefs::Name c_NMAP = chDefs::MakeName('N', 'M', 'A', 'P');
const chDefs::Name c_RMOV = chDefs::MakeName('R', 'M', 'O', 'V');
const chDefs::Name c_UVSC = chDefs::MakeName('U', 'V', 'S', 'C');
const chDefs::Name c_SHDR = chDefs::MakeName('S', 'H', 'D', 'R');
const chDefs::Name c_SHNM = chDefs::MakeName('S', 'H', 'N', 'M');
const chDefs::Name c_MLIB = chDefs::MakeName('M', 'L', 'I', 'B');
const chDefs::Name c_WNAM = chDefs::MakeName('W', 'N', 'A', 'M');
const chDefs::Name c_MOLN = chDefs::MakeName('M', 'O', 'L', 'N');
const chDefs::Name c_THMB = chDefs::MakeName('T', 'H', 'M', 'B');
const chDefs::Name c_UVXF = chDefs::MakeName('U', 'V', 'X', 'F');
const chDefs::Name c_REFL = chDefs::MakeName('R', 'E', 'F', 'L');
const chDefs::Name c_SVFN = chDefs::MakeName('S', 'V', 'F', 'N');
const chDefs::Name c_MDSP = chDefs::MakeName('M', 'D', 'S', 'P');

bool l_ReadMTBL = false;


//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline bool is_letter(char i_Char)
{
	return ((i_Char >= 'A') && (i_Char <= 'Z'));
}

//------------------------------------------------------------------------
// Name chunks don't ever contain sub chunks, but they can
// trick the parser because they might have 4 characters
// in the place where a chunk name might be if it was a 
// container chunk.
// Return true if the chunk should be skipped.
//------------------------------------------------------------------------
inline bool skip_name(chDefs::Name i_Name)
{
	return ((i_Name == c_NNAM) || (i_Name == c_WNAM) || (i_Name == c_MTID));
}

//------------------------------------------------------------------------
// Peek at next few characters, try to guess if
// this is a container or not.
//------------------------------------------------------------------------
bool peek_container(gfFile& i_File)
{
	chDefs::Name name;
	fsFileStream::FilePosType cur_pos = i_File.GetFilePos();
	gfFileUtil::Read(i_File, name);
	i_File.SetFilePos(cur_pos);

	// See if name is 4 letters
	char *name_str = (char*)(&name);
	if (!is_letter(name_str[0]) || !is_letter(name_str[1]) || 
		!is_letter(name_str[2]) || !is_letter(name_str[3]))
		return false;

//	DBG_LOG4("Got name: %c%c%c%c", name_str[0], name_str[1], name_str[2], name_str[3]);
	return true;
}


//------------------------------------------------------------------------
// skip through material chunk, looking only for material name.
//------------------------------------------------------------------------
void read_material_name_only(	chReader& i_Reader,
								std::string& o_MaterialName )
{
	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	try
	{
		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			if( name == c_MNAM )
			{
				// material name
				i_Reader.Read(o_MaterialName);
				
				// Abort reading of data for this chunk, we got what we needed.
				i_Reader.FinishChunk();
				break;
			}

			i_Reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX& i_Ex )
	{
		i_Ex;
		DBG_LOG("Invalid chunk in read_material_name_only");
		throw mdlInvalidModelFileX(i_Reader.GetLocator());
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void count_nodes(chReader& i_Reader, 
				  gfFile& i_File,
				  int &o_NodeCount)
{
	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	// Peek at next few characters, try to guess if
	// this is a container or not.
	if (!peek_container(i_File))
		return;

	// try to get child chunks
	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if (name == c_MATR)
		{
			// Make sure we have a material name before we count the node...
			std::string material_name;
			read_material_name_only(i_Reader, material_name);
			if (!material_name.empty())
				o_NodeCount++;
		}
		else if (skip_name(name))
		{
			// skip, don't recurse
		}
		else
			count_nodes(i_Reader, i_File, o_NodeCount);

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int count_materials_binary(const fsLocator &i_Locator)
{
	gfFileBin file(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);

	//	If this isn't a real Terawatt/XLT binary file this will throw
	file.ReadHeader();
	chBinReader reader(file);
	int mat_count = 0;
	count_nodes(reader, file, mat_count);
	return mat_count;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int count_materials_XML(const fsLocator &i_Locator)
{
	// TODO - this function won't work in XML
	//

	gfFileXML file(i_Locator, fsFileStream::e_ReadOnly);

	//	If this isn't a real Terawatt/XLT XML file this will throw
	file.ReadHeader();
	chXMLReader reader(file);

	int mat_count = 0;
	count_nodes(reader, file, mat_count);
	return mat_count;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int count_materials(const fsLocator &i_Locator)
{
	//	handle reading from a binary file and writing to ASCII or the other way around.
	//
	itString extension;
	i_Locator.GetLastName().GetExtension( extension );
	if ((extension == itString("xml"))
	        || (extension == itString("XML")) )
	{
		return count_materials_XML( i_Locator );
	}
	else
	{
		// This binary reader has to handle MTL files 
		// and all of the geometry file formats (when saving overriden materials).
		return count_materials_binary( i_Locator );
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void write_color(chWriter &o_Writer, const maFloatRGBA& i_Color)
{
	o_Writer.Write(i_Color.GetRed());
	o_Writer.Write(i_Color.GetGreen());
	o_Writer.Write(i_Color.GetBlue());
	o_Writer.Write(i_Color.GetAlpha());
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void write_SVFN(chWriter &o_Writer)
{
	// Write saved filename in order to resolve absolute paths when read from
	// a new path.
	o_Writer.WriteChunkHeader(c_SVFN, 0, false);
	itString savefilename;
	fsFileUtil::LocatorToUnicodeString( o_Writer.GetLocator(), savefilename );
	chChunkParserUtil::Write( o_Writer, savefilename );
	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void write_SHDR(chWriter &o_Writer, const mdlMaterialInfo &i_Data)
{
	int version = 2;
	const bool c_bSHDR_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader(c_SHDR, version, c_bSHDR_CONTAINER_CHUNK);

	o_Writer.WriteChunkHeader(c_SHNM, 1, false);
	std::string savename = itStringUtil::GetStdString( i_Data.GetShader().GetLastName() );
	savename += '\0';

	fsFileUtil::LocatorToANSIFilename( i_Data.GetShader(), savename );

	o_Writer.Write(savename);
	o_Writer.FinishChunk();

//	matShaderParser::WriteShader(o_Writer, *i_Data.GetShaderData());
	matShaderParser::WriteShader(o_Writer, i_Data.GetShaderParams());

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void write_MGLO(chWriter &o_Writer, const mdlMaterialInfo &i_Data)
{
	int version = 1;
	const bool c_bMGLO_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader(c_MGLO, version, c_bMGLO_CONTAINER_CHUNK);

	matShaderParser::WriteShader(o_Writer, i_Data.GetGlowParams());

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void write_REFL(chWriter &o_Writer, const mdlMaterialInfo &i_Data)
{
	int version = 0;
	const bool c_bREFL_CONTAINER_CHUNK = false;
	o_Writer.WriteChunkHeader(c_REFL, version, c_bREFL_CONTAINER_CHUNK);

	o_Writer.Write(i_Data.GetReflectionParams().m_bAutoGenEnvMap);
	o_Writer.Write(i_Data.GetReflectionParams().m_ReflMapResolution);
	o_Writer.Write(i_Data.GetReflectionParams().m_bIsPlanar);
	o_Writer.Write(i_Data.GetReflectionParams().m_NearPlane);

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void write_UVXF(chWriter &o_Writer, const mdlMaterialInfo &i_Data)
{
	int version = 0;
	const bool c_bUVXF_CONTAINER_CHUNK = false;
	o_Writer.WriteChunkHeader(c_UVXF, version, c_bUVXF_CONTAINER_CHUNK);

	o_Writer.Write(i_Data.GetUVTransform().m_UScale);
	o_Writer.Write(i_Data.GetUVTransform().m_VScale);
	o_Writer.Write(i_Data.GetUVTransform().m_UTrans);
	o_Writer.Write(i_Data.GetUVTransform().m_VTrans);
	o_Writer.Write(i_Data.GetUVTransform().m_UVAngle);

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void write_MLIB(chWriter &o_Writer, const mdlMaterialInfo &i_Data)
{
	int version = 1;
	o_Writer.WriteChunkHeader(c_MLIB, version, false);

	// path to material library filename
	std::string lib_filename;
	fsFileUtil::LocatorToANSIFilename( i_Data.GetLibraryFilename(), lib_filename );
	chChunkParserUtil::Write( o_Writer, lib_filename );

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void write_MOLN(chWriter &o_Writer, const mdlMaterialInfo &i_Data)
{
	int version = 0;
	const bool c_bMOLN_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader(c_MOLN, version, c_bMOLN_CONTAINER_CHUNK);

	matShaderParser::WriteShader(o_Writer, i_Data.GetOutlineParams());

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void write_MDSP(chWriter &o_Writer, const mdlMaterialInfo &i_Data)
{
	int version = 0;
	const bool c_bMDSP_CONTAINER_CHUNK = false;
	o_Writer.WriteChunkHeader(c_MDSP, version, c_bMDSP_CONTAINER_CHUNK);

	matShaderParser::WriteShader(o_Writer, i_Data.GetDisplacementParams());

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void write_NMAP(chWriter &o_Writer, const mdlMaterialInfo &i_Data)
{
	int version = 0;
	const bool c_bNMAP_CONTAINER_CHUNK = false;
	o_Writer.WriteChunkHeader(c_NMAP, version, c_bNMAP_CONTAINER_CHUNK);

	matShaderParser::WriteShader(o_Writer, i_Data.GetNormalsParams());

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void write_RMOV(chWriter &o_Writer, const mdlMaterialInfo &i_Data)
{
	int version = 0;
	const bool c_bRMOV_CONTAINER_CHUNK = false;
	o_Writer.WriteChunkHeader(c_RMOV, version, c_bRMOV_CONTAINER_CHUNK);

	matShaderParser::WriteShader(o_Writer, i_Data.GetRendermanOverrideParams());

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int find_material(const std::string &i_MaterialName,
				  const std::vector< shared_ptr<mdlMaterialInfo> > &i_Materials)
{
	for (int i=0; i<i_Materials.size(); i++)
	{
		if (i_Materials[i]->GetMaterialName() == i_MaterialName)
			return i;
	}
	return -1;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void write_THMB(chWriter &o_Writer, const mtrThumbnailData &i_Thumbnail)
{
	const int version = 1; // version 1 uses shader ball asset
	const bool c_bTHMB_CONTAINER_CHUNK = false;
	o_Writer.WriteChunkHeader(c_THMB, version, c_bTHMB_CONTAINER_CHUNK);

	matThumbnailParser::Write(o_Writer, i_Thumbnail.GetThumbParams(),i_Thumbnail.GetBufferSize() );
	//matThumbnailParser::Write(o_Writer, i_Thumbnail );

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void copy_chunks( chReader& i_Reader, 
				 gfFile& i_File,
				 chWriter& o_Writer, 
				 gfFile& o_File, 
				 const std::vector< shared_ptr<mdlMaterialInfo> > &i_Materials,
				 int &io_MatCount)
{
	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	// try to get child chunks
	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if (name == c_MATR)
		{
			// Write our material instead ...
			// Old style wrote materials in order and the list of materials had to
			// be in the same order.
			//mtrMaterialSaver::WriteMaterialData(o_Writer, *i_Materials[io_MatCount]);
			//io_MatCount++;

			// New style reads enough of this chunk to know the name of the material,
			// then looks for the data with the same name
			std::string material_name;
			read_material_name_only(i_Reader, material_name);
			if (!material_name.empty())
			{
				int mat_index = find_material(material_name, i_Materials);
				if (mat_index >= 0)
					mtrMaterialSaver::WriteMaterialData(o_Writer, *i_Materials[mat_index]);
				else
					DBG_WARNING("Could not find material with name " << material_name << " in order to write new material data");
			}
			else
			{
				// This case would have been detected in count_materials already...
				//DBG_WARNING("Could not parse material name in order to write new material data");
			}
		}
		else if (name == c_SVFN)
		{
			// skip the save file name chunk, we will add a new one to the
			// material table chunk below with the save filename we are writing now.
		}
		else if ((!skip_name(name)) && peek_container(i_File))
		{
			//char *name_str = (char*)(&name);
			//DBG_LOG4("Recursing: %c%c%c%c", name_str[0], name_str[1], name_str[2], name_str[3]);

			o_Writer.WriteChunkHeader(name, version, true);

			// If we are copying over the material table, then
			// add the save filename chunk here, this will replace the old one.
			if (name == c_MTBL)
			{
				write_SVFN(o_Writer);
			}

			copy_chunks(i_Reader, i_File, o_Writer, o_File, i_Materials, io_MatCount);
			o_Writer.FinishChunk();
		}
		else
		{
			//char *name_str = (char*)(&name);
			//DBG_LOG4("Copying: %c%c%c%c", name_str[0], name_str[1], name_str[2], name_str[3]);

			// copy whole size of chunk from input file to
			// 
			std::unique_ptr<char> buffer(new char [size]);

			o_Writer.WriteChunkHeader(name, version, false);
			i_File.Read(size, buffer.get());
			o_File.Write(size, buffer.get());
			o_Writer.FinishChunk();
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void parse_chunks(chReader& i_Reader, 
				  gfFile& i_File,
				  std::vector< shared_ptr<mdlMaterialInfo> > &o_Materials)
{
	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;
	
	//DBG_LOG("start parse_chunks");

	// try to get child chunks
	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		//DBG_LOG("----------------");
		//chChunkParserUtil::DebugDisplayChunkName(name);
		//DBG_LOG("----------------");

		if (name == c_MATR)
		{
			// Parse material
			shared_ptr<mdlMaterialInfo> mat_data(new mdlMaterialInfo);
			mtrMaterialSaver::ReadMaterialData(i_Reader, version, size, *mat_data);
			o_Materials.push_back(mat_data);
		}
		else if (name == c_SVFN)
		{
			// Read in name of fullpath of filename that was saved,
			// we can compare this to the filename we are currently loading
			// in order to resolve absolute paths
			fsLocator org_locator;
			itString savefilename;
			chChunkParserUtil::Read( i_Reader, savefilename );
			fsFileUtil::UnicodeStringToLocator( savefilename, org_locator );
			fsAbsolutePathMgr::AddDirMappingFromFiles(org_locator, i_Reader.GetLocator());
		}
		else if (skip_name(name))
		{
			// Name chunks don't ever contain sub chunks, but they can
			// trick the parser because they might have 4 characters
			// in the place where a chunk name might be if it was a 
			// container chunk.
			// Just skip over it.
		}
		else if (peek_container(i_File))
		{
			if (name == c_MTBL)
			{
				l_ReadMTBL = true;
			}

			parse_chunks(i_Reader, i_File, o_Materials);
		}

		i_Reader.FinishChunk();
	}

	//DBG_LOG("end   parse_chunks");
}


}	// end of namespace


//------------------------------------------------------------------------
// Return chunk name used for a single material chunk
//------------------------------------------------------------------------
const chDefs::Name	mtrMaterialSaver::GetChunkName()
{
	return c_MATR;
}

//------------------------------------------------------------------------
// Writes the header for a material table chunk and writes 
// full filename the writer is using in order to locate
// absolute paths to textures
//------------------------------------------------------------------------
void mtrMaterialSaver::BeginMaterialTable(chWriter &o_Writer)
{
	o_Writer.WriteChunkHeader(c_MTBL, 0, true);
	write_SVFN(o_Writer);
}

//------------------------------------------------------------------------
//	Write single material data structure as chunk
//------------------------------------------------------------------------
void mtrMaterialSaver::WriteMaterialData(chWriter &o_Writer, const mdlMaterialInfo &i_Data)
{
	int version = 5;

	o_Writer.WriteChunkHeader(c_MATR, version, true);

	if (!i_Data.GetMaterialName().empty())
	{
		o_Writer.WriteChunkHeader(c_MNAM, 0, false);
		o_Writer.Write(i_Data.GetMaterialName().c_str());
		o_Writer.FinishChunk();
	}

	// Add material animation
	//int num_anims = i_Data.GetNumMatAnims();
	//int i;
	//for (i=0; i<num_anims; i++)
	//{
	//	write_MANI(o_Writer, i_Data.GetMatAnim(i));
	//}

	if (i_Data.GetShaderParams())
	{
		write_SHDR(o_Writer, i_Data);
	}

	if (i_Data.GetHasGlow())
	{
		write_MGLO(o_Writer, i_Data);
	}

	if (i_Data.UsesMaterialLibrary())
	{
		write_MLIB(o_Writer, i_Data);
	}

	if (i_Data.GetHasOutline())
	{
		write_MOLN(o_Writer, i_Data);
	}

	// added this chunk for v2
	write_UVXF(o_Writer, i_Data);

	if (i_Data.GetHasReflection())
	{
		write_REFL(o_Writer, i_Data);
	}

	//version 4
	if (i_Data.GetHasDisplacement())
	{
		write_MDSP(o_Writer, i_Data);
	}

	//version 5
	write_NMAP(o_Writer, i_Data);

	//version 6
	if (i_Data.GetHasRendermanOverride())
	{
		write_RMOV(o_Writer, i_Data);
	}

	o_Writer.FinishChunk();	// end of MATR
}

//------------------------------------------------------------------------
// Read single material data from chunk
//------------------------------------------------------------------------
void mtrMaterialSaver::ReadMaterialData(chReader &i_Reader, 				
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										mdlMaterialInfo &o_MatInfo)
{
	mdlMaterialParser::ReadMATR(i_Reader, i_Version, i_Size, o_MatInfo);
}

//------------------------------------------------------------------------
// Write_Binary - copies chunks from one file to another, but changes
//					materials based on passed list.
//------------------------------------------------------------------------
bool Write_Binary( const fsLocator &i_NewLocator,
					const fsLocator &i_OldLocator, 
					const std::vector< shared_ptr<mdlMaterialInfo> > &i_Materials)
{
	// Set up input and output files
	gfFileBin::Header header;
	gfFileBin ifile(i_OldLocator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
	ifile.ReadHeader(&header);
	chBinReader reader(ifile);

	gfFileBin ofile(i_NewLocator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	ofile.WriteHeader(header);
	chBinWriter writer(ofile);

	int mat_counter = 0;
	copy_chunks(reader, ifile, writer, ofile, i_Materials, mat_counter);
	return true;
}

//------------------------------------------------------------------------
// Write_XML - copies chunks from one file to another, but changes
//					materials based on passed list.
//------------------------------------------------------------------------
bool Write_XML( const fsLocator &i_NewLocator,
				const fsLocator &i_OldLocator, 
				const std::vector< shared_ptr<mdlMaterialInfo> > &i_Materials)
{
	// Set up input and output files
	gfFileXML::Header header;
	gfFileXML ifile(i_OldLocator, fsFileStream::e_ReadOnly);
	ifile.ReadHeader(&header);
	chXMLReader reader(ifile);

	gfFileXML ofile(i_NewLocator, fsFileStream::e_WriteOnly);
	ofile.WriteHeader(header);
	chXMLWriter writer(ofile);

	int mat_counter = 0;
	copy_chunks(reader, ifile, writer, ofile, i_Materials, mat_counter);
	return true;
}

//------------------------------------------------------------------------
// WriteMaterialSet - writes GMB file of named materials to be
// used as set of overriden materials for a geometry file.
//------------------------------------------------------------------------
void mtrMaterialSaver::WriteMaterialSet( const fsLocator &i_Locator,
								const std::vector< shared_ptr<mdlMaterialInfo> > &i_Materials )
{
	// Create new locator
	if( fsFileUtil::FileExists(i_Locator) )
		fsFileUtil::DeleteFile(i_Locator);
	fsFileUtil::CreateFile(i_Locator);

	gfFileBin ofile(i_Locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	ofile.WriteHeader();
	chBinWriter writer(ofile);

	write_SVFN(writer); // store save filename in order to resolve paths later.

	const int num_materials = i_Materials.size();
	for (int i=0; i<num_materials; i++)
	{
		mtrMaterialSaver::WriteMaterialData(writer, *i_Materials[i]);
	}
}

//------------------------------------------------------------------------
// Write - copies chunks from one file to another, but changes
//		materials based on passed list.
//------------------------------------------------------------------------
bool mtrMaterialSaver::Write( const fsLocator &i_NewLocator,
								const fsLocator &i_OldLocator, 
								const std::vector< shared_ptr<mdlMaterialInfo> > &i_Materials, 
								const gfFileConstants::gfWriteFormats i_WriteFormat )
{
	// First of all, count the number of material chunks
	// within the old file and make sure it matches the
	// number in the list
	int mat_count = count_materials(i_OldLocator);
	//DBG_LOG2("Material count %d ?= %d", mat_count, i_Materials.size());
	if (mat_count != i_Materials.size())
		return false;

	// Determine if old and new files are the same.
	// Convert to ascii to do case-independent comparison
	std::string old_fname, new_fname;
	fsFileUtil::LocatorToANSIFilename(i_OldLocator, old_fname);
	fsFileUtil::LocatorToANSIFilename(i_NewLocator, new_fname);
	bool bSame = !::_stricmp(old_fname.c_str(), new_fname.c_str());

	fsLocator input_locator = i_OldLocator;
	if (bSame)
	{
		// Need to check for read-only first, otherwise
		// our temporary file will become read-only
		if (fsFileUtil::IsReadOnly(i_OldLocator))
			throw fsReadOnlyX(i_OldLocator);

		// Copy old locator to temporary location in order to
		// have backup in case we mess up file, or to make
		// sure we don't try to overwrite file we are reading
		fsLocator temp_file = gfPaths::GetPath(gfPaths::e_ExePath);
		temp_file.Push(itString("model.bak"));
		if (i_NewLocator == temp_file)
			return false;	// can't save to our temp file name
		
		if( fsFileUtil::FileExists(temp_file) )
			fsFileUtil::DeleteFile(temp_file);
		fsFileUtil::CopyFile(i_OldLocator, temp_file);

		input_locator = temp_file;
	}

	// Create new locator
	if( fsFileUtil::FileExists(i_NewLocator) )
		fsFileUtil::DeleteFile(i_NewLocator);
	fsFileUtil::CreateFile(i_NewLocator);

	//
	switch (i_WriteFormat)
	{
		case gfFileConstants::eFileBinary:
		{
			Write_Binary( i_NewLocator, input_locator, i_Materials );
			return true;
		}
		case gfFileConstants::eFileXML:
		{
			Write_XML( i_NewLocator, input_locator, i_Materials );
			return true;
		}
	}

	return true;
}

//------------------------------------------------------------------------
// Read_Binary - get material info directly from file.
//------------------------------------------------------------------------
void Read_Binary( const fsLocator &i_Locator,
					std::vector< shared_ptr<mdlMaterialInfo> > &o_Materials)
{
	gfFileBin ifile(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
	ifile.ReadHeader();
	chBinReader reader(ifile);

	l_ReadMTBL = false;
	parse_chunks(reader, ifile, o_Materials);
	//DBG_LOG("Finished material read");
}

//------------------------------------------------------------------------
// Read_XML - get material info directly from file.
//------------------------------------------------------------------------
void Read_XML( const fsLocator &i_Locator,
				std::vector< shared_ptr<mdlMaterialInfo> > &o_Materials )
{
	gfFileXML ifile(i_Locator, fsFileStream::e_ReadOnly);
	ifile.ReadHeader();
	chXMLReader reader(ifile);

	l_ReadMTBL = false;
	parse_chunks(reader, ifile, o_Materials);
	//DBG_LOG("Finished material read");
}

//------------------------------------------------------------------------
// Read - get material info directly from file.
//------------------------------------------------------------------------
void mtrMaterialSaver::Read( const fsLocator &i_Locator,
							std::vector< shared_ptr<mdlMaterialInfo> > &o_Materials)
{
	//	handle reading from a binary file and writing to ASCII or the other way around.
	//
	itString extension;
	i_Locator.GetLastName().GetExtension( extension );
	if (   (extension == itString("mtl"))
	    || (extension == itString("MTL")) )
	{
		Read_Binary( i_Locator, o_Materials );
	}
	else if (   (extension == itString("xml"))
	         || (extension == itString("XML")) )
	{
		Read_XML( i_Locator, o_Materials );
	}
	else
	{
		//	gmb, gxb ...
		//	chx, mhx, mx, ...
		//
		try
		{
			Read_Binary( i_Locator, o_Materials );
		}
		catch ( gfInvalidFileBinX &i_Ex )
		{
			DBG_WARNING("Could not read materials from file: " << i_Ex.GetLocator());
		}
	}
}

//------------------------------------------------------------------------
// returns true if the last file read had a material table
//------------------------------------------------------------------------
bool mtrMaterialSaver::DidReadMaterialTable()
{
	return l_ReadMTBL;
}

//------------------------------------------------------------------------
// ReadSingleMaterial - read a single material from a file in
//	the material library. Returns true if material info was read.
//------------------------------------------------------------------------
bool mtrMaterialSaver::ReadSingleMaterial( const fsLocator &i_Locator,
							mdlMaterialInfo &o_Material)
{

	gfFileBin ifile(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
	ifile.ReadHeader();
	chBinReader reader(ifile);

	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	while( reader.ReadChunkHeader(name, version, size) )
	{
		if (name == c_SVFN)
		{
			// Read in name of fullpath of filename that was saved,
			// we can compare this to the filename we are currently loading
			// in order to resolve absolute paths
			fsLocator org_locator;
			itString savefilename;
			chChunkParserUtil::Read( reader, savefilename );
			fsFileUtil::UnicodeStringToLocator( savefilename, org_locator );
			fsAbsolutePathMgr::AddDirMappingFromFiles(org_locator, i_Locator);
		}
		else if (name == c_MATR)
		{
			mtrMaterialSaver::ReadMaterialData( reader, version, size, o_Material);
			return true;
		}
		reader.FinishChunk();
	}

	return false;
}

//------------------------------------------------------------------------
// ReadThumbnail - reads thumbnail data from material file. 
// Returns true if bitmap data was read
//------------------------------------------------------------------------
bool mtrMaterialSaver::ReadThumbnail( const fsLocator &i_Locator, 
									 mtrThumbnailData &o_Thumbnail)
									 //envType::UInt8* &o_PixelBuffer, 
									 //int &o_BufferSize,
									 //int &o_BitmapSize)
{
	gfFileBin ifile(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
	ifile.ReadHeader();
	chBinReader reader(ifile);

	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	while( reader.ReadChunkHeader(name, version, size) )
	{
		if (name == c_THMB)
		{
			// Make sure the thumbnail is the correct format by checking
			// the version number. Old style thumbnails should fail.
			if (version >= 1)
			{
				envType::UInt8* pixelBuffer = NULL; 
				int bufferSize = 0;
				int bitmapSize = 0;
				matThumbnailParser::Read(reader, version, size,
					pixelBuffer, bufferSize, bitmapSize);
					//o_Thumbnail);
				// Ownership of the pixel buffer transfers to the thumbnail data:
				o_Thumbnail.SetThumbData(pixelBuffer, bufferSize, bitmapSize);
				return (pixelBuffer != NULL);
			}
			// thumbnail was old format, treat it like there was no thumbnail
			break;
		}
		reader.FinishChunk();
	}

	return false;
}

//------------------------------------------------------------------------
// WriteSingleMaterial_Binary - write a single material to a file in
//	the material library. 
//------------------------------------------------------------------------
void WriteSingleMaterial_Binary(const fsLocator &i_Locator,
								const mdlMaterialInfo &i_Material,
								const mtrThumbnailData &i_Thumbnail)
{
	gfFileBin ofile(i_Locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	ofile.WriteHeader();
	chBinWriter writer(ofile);

	write_SVFN(writer); // store save filename in order to resolve paths later.
	mtrMaterialSaver::WriteMaterialData(writer, i_Material);

	// Thumb is written at end of the single material file (.mtl) 
	// only in binary format. This is written outside of the material
	// information above.
	if (i_Thumbnail.GetHasThumb())
	{
		write_THMB(writer, i_Thumbnail);
	}
}

//------------------------------------------------------------------------
// WriteSingleMaterial_XML - write a single material to a file in
//	the material library. 
//------------------------------------------------------------------------
void WriteSingleMaterial_XML(	const fsLocator &i_Locator,
								const mdlMaterialInfo &i_Material)
{
	gfFileXML ofile(i_Locator, fsFileStream::e_WriteOnly);
	ofile.WriteHeader();
	chXMLWriter writer(ofile);

	write_SVFN(writer); // store save filename in order to resolve paths later.
	mtrMaterialSaver::WriteMaterialData(writer, i_Material);
}

//------------------------------------------------------------------------
// WriteSingleMaterial - write a single material to a file in
//	the material library. 
//------------------------------------------------------------------------
void mtrMaterialSaver::WriteSingleMaterial( const fsLocator &i_Locator,
											const mdlMaterialInfo &i_Material,
											const mtrThumbnailData &i_Thumbnail,
											const gfFileConstants::gfWriteFormats i_WriteFormat)
{
	// Create new locator
	if( fsFileUtil::FileExists(i_Locator) )
		fsFileUtil::DeleteFile(i_Locator);
	fsFileUtil::CreateFile(i_Locator);

	//
	switch (i_WriteFormat)
	{
		case gfFileConstants::eFileBinary:
		{
			WriteSingleMaterial_Binary( i_Locator, i_Material, i_Thumbnail );
			break;
		}
		case gfFileConstants::eFileXML:
		{
			WriteSingleMaterial_XML( i_Locator, i_Material );
			break;
		}
	}
}

