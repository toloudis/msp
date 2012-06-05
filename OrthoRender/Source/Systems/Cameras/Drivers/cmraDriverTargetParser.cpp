/********************************************************************************************\
**  cmraDriverTargetParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverTargetParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverTargetInfo.hpp"

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
	const chDefs::Name c_CTGT = chDefs::MakeName('C', 'T', 'G', 'T');
	const chDefs::Name c_TGTI = chDefs::MakeName('T', 'G', 'T', 'I');

	const int l_TGTI_VERSION = 2;

	//========================================================================
	//   ReadTargetInfo
	//========================================================================
	void ReadTargetInfo(chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						cmraDriverTargetInfo &o_Driver )
	{
		std::string name;
		chChunkParserUtil::Read(i_Reader, name);
		o_Driver.m_ObjectName.SetString( name );

		if (i_Version >= 2)
		{
			nameUID uid;
			chChunkParserUtil::Read(i_Reader, uid);
			o_Driver.m_ObjectName.SetUID( uid );
		}

		if (i_Version >= 1)
		{
			chChunkParserUtil::Read(i_Reader, o_Driver.m_AttachName);
			chChunkParserUtil::Read(i_Reader, o_Driver.m_TargetOffset);
		}
	}

	//========================================================================
	//   WriteTargetInfo
	//========================================================================
	void WriteTargetInfo(	chWriter& o_Writer,
							const cmraDriverTargetInfo &i_Driver )
	{
		o_Writer.WriteChunkHeader( c_TGTI, l_TGTI_VERSION, false );

		chChunkParserUtil::Write(o_Writer, i_Driver.m_ObjectName.GetString());
		chChunkParserUtil::Write(o_Writer, i_Driver.m_ObjectName.GetUID());
		chChunkParserUtil::Write(o_Writer, i_Driver.m_AttachName);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_TargetOffset);

		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
cmraDriverTargetParser::cmraDriverTargetParser()
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
cmraDriverTargetParser::~cmraDriverTargetParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name cmraDriverTargetParser::GetChunkName()
{
	return c_CTGT;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void cmraDriverTargetParser::Read(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									tmlnDriverInfo& o_Driver ) const
{
	cmraDriverTargetInfo &driver = dynamic_cast<cmraDriverTargetInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_TGTI )
		{
			ReadTargetInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void cmraDriverTargetParser::Write( chWriter& o_Writer,
									const tmlnDriverInfo& i_Driver ) const
{
	const cmraDriverTargetInfo &driver = dynamic_cast<const cmraDriverTargetInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( c_CTGT, 0, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteTargetInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverTargetParser::Create() const
{
	return new cmraDriverTargetInfo(c_CTGT);
}


