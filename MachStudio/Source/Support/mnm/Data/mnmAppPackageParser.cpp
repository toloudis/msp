/********************************************************************************************\
**	mnmAppPackageParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#include "Support/mnm/data/mnmAppPackageParser.hpp"

#include "Support/mnm/data/mnmAppPackageData.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
//#include "Core/ch/chWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_APPV = chDefs::MakeName('A', 'P', 'P', 'V');

}	// local namespace


//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name mnmAppPackageParser::GetChunkName()
{
	return c_APPV;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void mnmAppPackageParser::Read(	chReader& io_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								mnmAppPackageData& o_Data )
{
	io_Reader.Read( o_Data.m_SGPUAppName );
	io_Reader.Read( o_Data.m_SGPUAppVersion );
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void mnmAppPackageParser::Write(chWriter& io_Writer,
								const mnmAppPackageData& i_Data )
{
	const int c_APPV_VERSION = 0;
	io_Writer.WriteChunkHeader( c_APPV, c_APPV_VERSION, true );

	itString app_name( i_Data.m_SGPUAppName );
	app_name += itString::CharType(0);
	io_Writer.Write( app_name );
	io_Writer.Write( i_Data.m_SGPUAppVersion );

	io_Writer.FinishChunk();
}

