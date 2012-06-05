/********************************************************************************************\
**  prtclDriverEmitParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Particles/Timeline/prtclDriverEmitParser.hpp"

#include "Systems/Particles/Timeline/prtclDriverEmitInfo.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ch/chChunkParserUtil.hpp"


namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_PDEM = chDefs::MakeName('P', 'D', 'E', 'M');	//prtcl drive emit
	const chDefs::Name c_PDED = chDefs::MakeName('P', 'D', 'E', 'D');	//prtcl driver emit data
	const int c_PDEM_version = 0;
	const int c_PDED_version = 0;

	//========================================================================
	//   ReadEmitInfo
	//========================================================================
	void ReadEmitInfo(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size
							 )
	{
		int junk = 0;
		chChunkParserUtil::Read(i_Reader, junk);
	}

	//========================================================================
	//   WriteEmitInfo
	//========================================================================
	void WriteEmitInfo(	chWriter& o_Writer )
	{
		o_Writer.WriteChunkHeader( c_PDED, c_PDED_version, false );

		chChunkParserUtil::Write(o_Writer, 0 );

		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
prtclDriverEmitParser::prtclDriverEmitParser()
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
prtclDriverEmitParser::~prtclDriverEmitParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name prtclDriverEmitParser::GetChunkName()
{
	return c_PDEM;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void prtclDriverEmitParser::Read(	chReader& i_Reader,
											chDefs::Version i_Version,
											chDefs::Size i_Size,
											tmlnDriverInfo& o_Driver ) const
{
	prtclDriverEmitInfo &driver = dynamic_cast<prtclDriverEmitInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_PDED )
		{
			ReadEmitInfo(i_Reader, version, size );
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void prtclDriverEmitParser::Write( chWriter& o_Writer,
					const tmlnDriverInfo& i_Driver ) const
{
	const prtclDriverEmitInfo &driver = dynamic_cast<const prtclDriverEmitInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( c_PDEM, c_PDEM_version, true );

	tmlnDriverInfoParser::WriteDriverInfo( o_Writer, driver );

	//WriteEmitInfo( o_Writer );

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* prtclDriverEmitParser::Create() const
{
	return new prtclDriverEmitInfo(c_PDEM);
}


