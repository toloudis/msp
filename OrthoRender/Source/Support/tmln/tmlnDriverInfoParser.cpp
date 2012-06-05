/********************************************************************************************\
**  tmlnDriverInfoParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#include "Support/tmln/tmlnDriverInfoParser.hpp"

#include "Support/tmln/tmlnDriverInfo.hpp"

#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/ch/chChunkParserUtil.hpp"

namespace tmlnDriverInfoParser
{

namespace
{
//========================================================================
//========================================================================
const chDefs::Name c_DRVI = chDefs::MakeName('D', 'R', 'V', 'I');
}	// local namespace

//========================================================================
//========================================================================
chDefs::Name GetChunkName()
{
	return c_DRVI;
}

//========================================================================
//   ReadDriverInfo
//========================================================================
void ReadDriverInfo(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						tmlnDriverInfo& o_Driver )
{
	chChunkParserUtil::Read(i_Reader, o_Driver.m_Name);
	chChunkParserUtil::Read(i_Reader, o_Driver.m_BeginTime);
	chChunkParserUtil::Read(i_Reader, o_Driver.m_EndTime);

	if (i_Version >= 1)
	{
		chChunkParserUtil::Read(i_Reader, o_Driver.m_BlendType);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_BlendTime);

		if (i_Version >= 2)
		{
			chChunkParserUtil::Read(i_Reader, o_Driver.m_bRestoreOriginal);

			if (i_Version >= 3)
			{
				chChunkParserUtil::Read(i_Reader, o_Driver.m_EaseInWeight);
				chChunkParserUtil::Read(i_Reader, o_Driver.m_EaseOutWeight);
			}
		}
	}
}

//========================================================================
//   WriteDriverInfo
//========================================================================
void WriteDriverInfo(	chWriter& o_Writer,
						const tmlnDriverInfo& i_Driver )
{
	o_Writer.WriteChunkHeader( c_DRVI, 3, false );
	chChunkParserUtil::Write(o_Writer, i_Driver.m_Name);
	chChunkParserUtil::Write(o_Writer, i_Driver.m_BeginTime);
	chChunkParserUtil::Write(o_Writer, i_Driver.m_EndTime);
	// version 1
	chChunkParserUtil::Write(o_Writer, i_Driver.m_BlendType);
	chChunkParserUtil::Write(o_Writer, i_Driver.m_BlendTime);
	// version 2
	chChunkParserUtil::Write(o_Writer, i_Driver.m_bRestoreOriginal);
	// version 3
	chChunkParserUtil::Write(o_Writer, i_Driver.m_EaseInWeight);
	chChunkParserUtil::Write(o_Writer, i_Driver.m_EaseOutWeight);
	o_Writer.FinishChunk();
}


}	// end of namespace

