/*****************************************************************************
**	mtrMaterialSaver.hpp
**
**		The mtrMaterialSaver writes chunks from one file to another
**	but changes the material chunks.
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#ifdef MTR_MATERIALSAVER_HPP
#error mtrMaterialSaver.hpp multiply included
#endif
#define MTR_MATERIALSAVER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef GF_FILECONSTANTS_HPP
#include "Core/gf/gfFileConstants.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class chReader;
class chWriter;
class fsLocator;
class mdlMaterialInfo;
class mtrThumbnailData;


//============================================================================
//============================================================================
namespace mtrMaterialSaver
{
	//------------------------------------------------------------------------
	// Return chunk name used for a single material chunk
	//------------------------------------------------------------------------
	const chDefs::Name	GetChunkName();

	//------------------------------------------------------------------------
	// Writes the header for a material table chunk and writes 
	// full filename the writer is using in order to locate
	// absolute paths to textures
	//------------------------------------------------------------------------
	void BeginMaterialTable(chWriter &o_Writer);

	//------------------------------------------------------------------------
	//	Write single material data structure as chunk
	//------------------------------------------------------------------------
	void	WriteMaterialData(chWriter &o_Writer, 
							  const mdlMaterialInfo &i_Data);

	//------------------------------------------------------------------------
	// Read single material data from chunk
	//------------------------------------------------------------------------
	void	ReadMaterialData(chReader &i_Reader,
							 chDefs::Version i_Version,
							 chDefs::Size i_Size,
							 mdlMaterialInfo &o_MatInfo);

	//------------------------------------------------------------------------
	// WriteMaterialSet - writes GMB file of named materials to be
	// used as set of overriden materials for a geometry file.
	//------------------------------------------------------------------------
	void	WriteMaterialSet( const fsLocator &i_NewLocator,
							  const std::vector< shared_ptr<mdlMaterialInfo> > &i_Materials );

	//------------------------------------------------------------------------
	// Write - copies chunks from one file to another, but changes
	//		materials based on passed list.
	//------------------------------------------------------------------------
	bool	Write(  const fsLocator &i_NewLocator,
					const fsLocator &i_OldLocator, 
					const std::vector< shared_ptr<mdlMaterialInfo> > &i_Materials, 
					const gfFileConstants::gfWriteFormats i_WriteFormat = gfFileConstants::eFileBinary);

	//------------------------------------------------------------------------
	// Read - get material info directly from file.
	//------------------------------------------------------------------------
	void	Read(const fsLocator &i_Locator,
				 std::vector< shared_ptr<mdlMaterialInfo> > &o_Materials);

	//------------------------------------------------------------------------
	// returns true if the last file read had a material table
	//------------------------------------------------------------------------
	bool DidReadMaterialTable();

	//------------------------------------------------------------------------
	// ReadSingleMaterial - read a single material from a file in
	//	the material library. Returns true if material info was read.
	//------------------------------------------------------------------------
	bool	ReadSingleMaterial( const fsLocator &i_Locator,
								mdlMaterialInfo &o_Material);

	//------------------------------------------------------------------------
	// ReadThumbnail - reads thumbnail data from material file. 
	// Returns true if bitmap data was read
	//------------------------------------------------------------------------
	bool ReadThumbnail( const fsLocator &i_Locator, 
						mtrThumbnailData &o_Thumbnail);
						 //envType::UInt8* &o_PixelBuffer, 
						 //int &o_BufferSize,
						 //int &o_BitmapSize);

	//------------------------------------------------------------------------
	// WriteSingleMaterial - write a single material to a file in
	//	the material library. 
	//------------------------------------------------------------------------
	void	WriteSingleMaterial( const fsLocator &i_Locator,
								 const mdlMaterialInfo &i_Material, 
								 const mtrThumbnailData &i_Thumbnail,
								 const gfFileConstants::gfWriteFormats i_WriteFormat = gfFileConstants::eFileBinary);
}

