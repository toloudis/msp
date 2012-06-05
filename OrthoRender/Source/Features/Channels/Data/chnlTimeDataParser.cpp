/********************************************************************************************\
**  chnlTimeDataParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2005-7 - All Rights Reserved
\********************************************************************************************/
#include "Features/Channels/Data/chnlTimeDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Support/tmln/tmlnParser.hpp"


namespace chnlTimeDataParser
{

namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_CHDA = chDefs::MakeName('C', 'H', 'D', 'A');	// main data
const chDefs::Name c_MRKL = chDefs::MakeName('M', 'R', 'K', 'L');	// +-- marker list data
const chDefs::Name c_MRKD = chDefs::MakeName('M', 'R', 'K', 'D');	//   +-- marker data
const chDefs::Name c_NOTL = chDefs::MakeName('N', 'O', 'T', 'L');	// +-- note list data
const chDefs::Name c_NOTD = chDefs::MakeName('N', 'O', 'T', 'D');	//   +-- note data



//------------------------------------------------------------------------
//   ReadMarkerData
//------------------------------------------------------------------------
void ReadMarkerData(chReader& io_Reader,
				    chDefs::Version i_Version,
					chnlMarkerDataItem& o_Data )
{
	o_Data.m_TimeMarkerType.Read(io_Reader);
	o_Data.m_Time.Read(io_Reader);
	o_Data.m_Note.Read(io_Reader);
}

//------------------------------------------------------------------------
//   WriteMarkerData
//------------------------------------------------------------------------
void WriteMarkerData(	chWriter& o_Writer,
						const chnlMarkerDataItem& i_Data )
{
	const int l_cMRKD_VERSION = 1;
	o_Writer.WriteChunkHeader( c_MRKD, l_cMRKD_VERSION, false );

	i_Data.m_TimeMarkerType.Write(o_Writer);
	i_Data.m_Time.Write(o_Writer);
	i_Data.m_Note.Write(o_Writer);

	// version 1 removes unused m_LastModifiedData

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadMarkerListData
//------------------------------------------------------------------------
void ReadMarkerListData(chReader& io_Reader,
						chDefs::Version i_Version,
						chnlTimeData& o_Data )
{
	if (i_Version < 1)
	{
		int nummarkers;
		chChunkParserUtil::Read( io_Reader, nummarkers );
		o_Data.m_Markers.resize( nummarkers );

		for (int i = 0; i < nummarkers; ++i)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;
			io_Reader.ReadChunkHeader(name, version, size);
			DBG_ASSERT0( name == c_MRKD, "invalid chunk in time data");

			ReadMarkerData(io_Reader, i_Version, o_Data.m_Markers[i]);
			io_Reader.FinishChunk();
		}
	}
	else
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;
		int index = 0;

		while( io_Reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_MRKD )
			{
				if (index >= o_Data.m_Markers.size())
					o_Data.m_Markers.resize( index+1 );

				ReadMarkerData(io_Reader, i_Version, o_Data.m_Markers[index++]);
				io_Reader.FinishChunk();
			}
			else
			{
				DBG_ASSERT0(false, "invalid chunk header");
			}
		}
	}
}

//------------------------------------------------------------------------
//   WriteMarkerListData
//------------------------------------------------------------------------
void WriteMarkerListData(	chWriter& o_Writer,
							const chnlTimeData& i_Data )
{
	const int l_cMRKL_VERSION = 1;
	o_Writer.WriteChunkHeader( c_MRKL, l_cMRKL_VERSION, false );

	int size  = i_Data.m_Markers.size();
	//chChunkParserUtil::Write( o_Writer, size );

	for (int i = 0; i < size; ++i)
	{
		WriteMarkerData(o_Writer, i_Data.m_Markers[i]);
	}

	o_Writer.FinishChunk();
}



//------------------------------------------------------------------------
//   ReadNoteData
//------------------------------------------------------------------------
void ReadNoteData(chReader& io_Reader,
				    chDefs::Version i_Version,
					chnlNoteDataItem& o_Data )
{
	o_Data.m_Status.Read(io_Reader);
	o_Data.m_Time.Read(io_Reader);
	o_Data.m_Note.Read(io_Reader);
}

//------------------------------------------------------------------------
//   WriteNoteData
//------------------------------------------------------------------------
void WriteNoteData(	chWriter& o_Writer,
						const chnlNoteDataItem& i_Data )
{
	const int l_cNOTD_VERSION = 1;
	o_Writer.WriteChunkHeader( c_NOTD, l_cNOTD_VERSION, false );

	i_Data.m_Status.Write(o_Writer);
	i_Data.m_Time.Write(o_Writer);
	i_Data.m_Note.Write(o_Writer);

	// version 1 removes unused m_LastModifiedData

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadNoteListData
//------------------------------------------------------------------------
void ReadNoteListData(chReader& io_Reader,
						chDefs::Version i_Version,
						chnlTimeData& o_Data )
{
	if (i_Version < 1)
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;
		int nummarkers;

		chChunkParserUtil::Read( io_Reader, nummarkers );

		o_Data.m_Notes.resize( nummarkers );

		for (int i = 0; i < nummarkers; ++i)
		{
			io_Reader.ReadChunkHeader(name, version, size);
			DBG_ASSERT0( name == c_NOTD, "invalid chunk in time data");

			ReadNoteData(io_Reader, i_Version, o_Data.m_Notes[i]);
			io_Reader.FinishChunk();
		}
	}
	else
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;
		int index = 0;

		while( io_Reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_NOTD )
			{
				if (index >= o_Data.m_Notes.size())
					o_Data.m_Notes.resize( index+1 );

				ReadNoteData(io_Reader, i_Version, o_Data.m_Notes[index++]);
				io_Reader.FinishChunk();
			}
			else
			{
				DBG_ASSERT0(false, "invalid chunk header");
			}
		}
	}
}

//------------------------------------------------------------------------
//   WriteNoteListData
//------------------------------------------------------------------------
void WriteNoteListData(	chWriter& o_Writer,
						const chnlTimeData& i_Data )
{
	const int l_cNOTL_VERSION = 1;
	const bool c_bNOTL_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_NOTL, l_cNOTL_VERSION, c_bNOTL_CONTAINER_CHUNK );

	int size  = i_Data.m_Notes.size();
	//chChunkParserUtil::Write( o_Writer, size );

	for (int i = 0; i < size; ++i)
	{
		WriteNoteData(o_Writer, i_Data.m_Notes[i]);
	}

	o_Writer.FinishChunk();
}
}	// local namespace


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_CHDA;
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				chnlTimeData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_MRKL )
		{
			ReadMarkerListData( i_Reader, version, o_Data );
		}
		else
		if ( name == c_NOTL )
		{
			ReadNoteListData( i_Reader, version, o_Data );
		}
		else
		{
			DBG_ASSERT0(false, "invalid chunk header");
		}
		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void WriteData(	chWriter& o_Writer,
				const chnlTimeData& i_Data )
{
	const int l_cCHDA_VERSION = 1;
	o_Writer.WriteChunkHeader( c_CHDA, l_cCHDA_VERSION, true );

	WriteMarkerListData( o_Writer, i_Data );
	WriteNoteListData( o_Writer, i_Data );

	o_Writer.FinishChunk();
}

}	// end of namespace

