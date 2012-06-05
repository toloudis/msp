/********************************************************************************************\
**  cmraDriverDirectorParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverDirectorParser.hpp"

#include "Systems/Cameras/Drivers/cmraDriverCaptureInfo.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ch/chChunkParserUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	const chDefs::Name c_CDIR = chDefs::MakeName('C', 'D', 'I', 'R');	//cmra director driver

}	// local namespace


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
cmraDriverDirectorParser::cmraDriverDirectorParser()
{
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
cmraDriverDirectorParser::~cmraDriverDirectorParser()
{
}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name cmraDriverDirectorParser::GetChunkName()
{
	return c_CDIR;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void cmraDriverDirectorParser::Read(chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									tmlnDriverInfo& o_Driver ) const
{
	cmraDriverCaptureInfo &driver = dynamic_cast<cmraDriverCaptureInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void cmraDriverDirectorParser::Write(chWriter& o_Writer,
									 const tmlnDriverInfo& i_Driver ) const
{
	const cmraDriverCaptureInfo &driver = dynamic_cast<const cmraDriverCaptureInfo &>(i_Driver);

	const int c_CDIR_VERSION = 0;
	o_Writer.WriteChunkHeader( c_CDIR, c_CDIR_VERSION, true );

	tmlnDriverInfoParser::WriteDriverInfo( o_Writer, driver );

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverDirectorParser::Create() const
{
	return new cmraDriverCaptureInfo(c_CDIR);
}


