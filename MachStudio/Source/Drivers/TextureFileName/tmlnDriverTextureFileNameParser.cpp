/********************************************************************************************\
**  tmlnDriverTextureFileNameParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#include "Drivers/TextureFileName/tmlnDriverTextureFileNameParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Drivers/TextureFileName/tmlnDriverTextureFileNameInfo.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ch/chChunkParserUtil.hpp"

namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_TFNM = chDefs::MakeName('T', 'F', 'N', 'M');


	//========================================================================
	//   ReadTextureFileNameInfo
	//========================================================================
	void ReadTextureFileNameInfo(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverTextureFileNameInfo &o_Driver )
	{
		//chChunkParserUtil::Read(i_Reader, o_Driver.m_Value);
			
		// Read in a single itString with path separators and then 
		// split into locator
		itString savefilename;
		fsLocator driver_loc;
		chChunkParserUtil::Read( i_Reader, savefilename );
		fsFileUtil::UnicodeStringToLocator( savefilename, driver_loc );
		prtyTextureFileData val = o_Driver.m_Value.GetFullValue();
		val.m_TextureLocator = driver_loc;
		o_Driver.m_Value.SetValue(val);
	}

	//========================================================================
	//   WriteTextureFileNameInfo
	//========================================================================
	void WriteTextureFileNameInfo(	chWriter& o_Writer,
					const tmlnDriverTextureFileNameInfo &i_Driver )
	{
		const int l_cTFNM_VERSION = 1;
		o_Writer.WriteChunkHeader( c_TFNM, l_cTFNM_VERSION, false );

		// Convert array of itStrings to a single itString with
		// path separators in order to write to file.
		itString savefilename;
		fsFileUtil::LocatorToUnicodeString( i_Driver.m_Value.GetValue(), savefilename );
		chChunkParserUtil::Write( o_Writer, savefilename );

		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverTextureFileNameParser::tmlnDriverTextureFileNameParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tmlnDriverTextureFileNameParser::~tmlnDriverTextureFileNameParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
chDefs::Name tmlnDriverTextureFileNameParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void tmlnDriverTextureFileNameParser::Read(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverTextureFileNameInfo &driver = dynamic_cast<tmlnDriverTextureFileNameInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_TFNM )
		{
			ReadTextureFileNameInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void tmlnDriverTextureFileNameParser::Write( chWriter& o_Writer,
					const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverTextureFileNameInfo &driver = dynamic_cast<const tmlnDriverTextureFileNameInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( m_ChunkName, 0, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteTextureFileNameInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverTextureFileNameParser::Create() const
{
	return new tmlnDriverTextureFileNameInfo(m_ChunkName);
}


