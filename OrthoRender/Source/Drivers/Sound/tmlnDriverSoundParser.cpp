/********************************************************************************************\
**  tmlnDriverSoundParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Drivers/Sound/tmlnDriverSoundParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Drivers/Sound/tmlnDriverSoundInfo.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ch/chChunkParserUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_PDSD = chDefs::MakeName('P', 'D', 'S', 'D');
	const chDefs::Name c_SNDI = chDefs::MakeName('S', 'N', 'D', 'I');


	//------------------------------------------------------------------------
	//   ReadSoundInfo
	//------------------------------------------------------------------------
	void ReadSoundInfo(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						tmlnDriverSoundInfo &o_Driver )
	{
		itString null_terminated_string;
		i_Reader.Read(null_terminated_string);
		o_Driver.m_SoundName = itString(0, null_terminated_string.GetLength() - 1, null_terminated_string); // un-NULL terminate

		if ( i_Version > 0 )
		{
			i_Reader.Read(o_Driver.m_fSoundStartTime);
		}
	}

	//------------------------------------------------------------------------
	//   WriteSoundInfo
	//------------------------------------------------------------------------
	void WriteSoundInfo(	chWriter& o_Writer,
							const tmlnDriverSoundInfo &i_Driver )
	{
		o_Writer.WriteChunkHeader( c_SNDI, 1, false );

		itString null_terminated_string = i_Driver.m_SoundName;
		null_terminated_string += 0;
		o_Writer.Write(null_terminated_string.GetString());

		o_Writer.Write(i_Driver.m_fSoundStartTime);

		o_Writer.FinishChunk();
	}

}	// local namespace


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverSoundParser::tmlnDriverSoundParser()
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tmlnDriverSoundParser::~tmlnDriverSoundParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name tmlnDriverSoundParser::GetChunkName()
{
	return c_PDSD;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void tmlnDriverSoundParser::Read(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverSoundInfo &driver = dynamic_cast<tmlnDriverSoundInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_SNDI )
		{
			ReadSoundInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void tmlnDriverSoundParser::Write(	chWriter& o_Writer,
									const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverSoundInfo &driver = dynamic_cast<const tmlnDriverSoundInfo &>(i_Driver);

	const int l_cPDSD_VERSION = 0;
	o_Writer.WriteChunkHeader( c_PDSD, l_cPDSD_VERSION, true );

	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);
	WriteSoundInfo(o_Writer, driver);

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverSoundParser::Create() const
{
	return new tmlnDriverSoundInfo(c_PDSD);
}


