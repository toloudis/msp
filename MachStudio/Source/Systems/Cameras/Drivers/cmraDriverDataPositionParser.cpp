/********************************************************************************************\
**  cmraDriverDataPositionParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverDataPositionParser.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//	Constants
	//------------------------------------------------------------------------
	const chDefs::Name c_CDPO = chDefs::MakeName('C', 'D', 'P', 'O');
	const chDefs::Name c_POSI = chDefs::MakeName('P', 'O', 'S', 'I');

	const int l_CDPO_VERSION = 0;
	const int l_POSI_VERSION = 1;

	//------------------------------------------------------------------------
	//   ReadPositionInfo
	//------------------------------------------------------------------------
	void ReadPositionInfo(chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						cmraDriverDataPositionInfo &o_Driver )
	{
		chChunkParserUtil::Read(i_Reader, o_Driver.m_fYawAngle);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_fPitchAngle);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_fDistance);
		if ( i_Version >= 1 )
		{
			chChunkParserUtil::Read(i_Reader, o_Driver.m_fDeltaFromStartTime);
			chChunkParserUtil::Read(i_Reader, o_Driver.m_CamPosition);
			chChunkParserUtil::Read(i_Reader, o_Driver.m_CamTarget);
		}
	}

	//------------------------------------------------------------------------
	//   WritePositionInfo
	//------------------------------------------------------------------------
	void WritePositionInfo(	chWriter& o_Writer,
							const cmraDriverDataPositionInfo &i_Driver )
	{
		o_Writer.WriteChunkHeader( c_POSI, l_POSI_VERSION, false );

		chChunkParserUtil::Write(o_Writer, i_Driver.m_fYawAngle);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_fPitchAngle);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_fDistance);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_fDeltaFromStartTime);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_CamPosition);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_CamTarget);

		o_Writer.FinishChunk();
	}

}	// local namespace


namespace cmraDriverDataPositionParser
{
	//--------------------------------------------------------------------
	// Returns chunk name used by this parser
	//--------------------------------------------------------------------
	chDefs::Name cmraDriverDataPositionParser::GetChunkName()
	{
		return c_CDPO;
	}

	//--------------------------------------------------------------------
	//	Read is called by a parser utility when the chunk name has been read
	//  from the data file.  It will parse from the chunk into the given
	//  parsable object.
	//--------------------------------------------------------------------
	void Read(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				cmraDriverDataPositionInfo& o_Driver )
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;

		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_POSI )
			{
				ReadPositionInfo(i_Reader, version, size, o_Driver);
			}
			i_Reader.FinishChunk();
		}
	}

	//--------------------------------------------------------------------
	//	Write is called to parse data from the given object to the given
	//	chWriter.  i_Driver is the object being written.
	//--------------------------------------------------------------------
	void Write(	chWriter& o_Writer,
				const cmraDriverDataPositionInfo& i_Driver )
	{
		o_Writer.WriteChunkHeader( c_CDPO, l_CDPO_VERSION, true );

		WritePositionInfo(o_Writer, i_Driver);

		o_Writer.FinishChunk();
	}

	//--------------------------------------------------------------------
	// Create should be overridden to create the user specific parsable object.
	// Users of parsers should call create to create the object that is then
	// passed to Read.
	//--------------------------------------------------------------------
	cmraDriverDataPositionInfo* Create()
	{
		return new cmraDriverDataPositionInfo();
	}
}

