/********************************************************************************************\
**  cmraDriverDataAttachNodeParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverDataAttachNodeParser.hpp"

#include "Core/Ch/chReader.hpp"
#include "Core/Ch/chWriter.hpp"
#include "Core/Ch/chExceptionX.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsFileX.hpp"
#include "Core/Gf/gfFileBin.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"


namespace
{
	//------------------------------------------------------------------------
	//	Constants
	//------------------------------------------------------------------------
	const chDefs::Name c_CDAN = chDefs::MakeName('C', 'D', 'A', 'N');
	const chDefs::Name c_ATND = chDefs::MakeName('A', 'T', 'N', 'D');

	const int l_CDAN_VERSION = 0;
	const int l_ATND_VERSION = 0;

	//------------------------------------------------------------------------
	//   ReadAttachNodeInfo
	//------------------------------------------------------------------------
	void ReadAttachNodeInfo(chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							cmraDriverDataAttachNodeInfo &o_Driver )
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

	//------------------------------------------------------------------------
	//   WriteAttachNodeInfo
	//------------------------------------------------------------------------
	void WriteAttachNodeInfo(	chWriter& o_Writer,
								const cmraDriverDataAttachNodeInfo &i_Driver )
	{
		o_Writer.WriteChunkHeader( c_ATND, l_ATND_VERSION, false );

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
cmraDriverDataAttachNodeParser::cmraDriverDataAttachNodeParser()
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
cmraDriverDataAttachNodeParser::~cmraDriverDataAttachNodeParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name cmraDriverDataAttachNodeParser::GetChunkName()
{
	return c_CDAN;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void cmraDriverDataAttachNodeParser::Read(chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										cmraDriverDataAttachNodeInfo& o_Driver ) const
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_ATND )
		{
			ReadAttachNodeInfo(i_Reader, version, size, o_Driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void cmraDriverDataAttachNodeParser::Write(	chWriter& o_Writer,
											const cmraDriverDataAttachNodeInfo& i_Driver ) const
{
	o_Writer.WriteChunkHeader( c_CDAN, l_CDAN_VERSION, true );

	WriteAttachNodeInfo(o_Writer, i_Driver);

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
cmraDriverDataAttachNodeInfo* cmraDriverDataAttachNodeParser::Create() const
{
	return new cmraDriverDataAttachNodeInfo(c_CDAN);
}


