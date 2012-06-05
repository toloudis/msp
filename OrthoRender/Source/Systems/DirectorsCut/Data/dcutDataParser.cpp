/********************************************************************************************\
**  dcutCamerasDataParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "Systems/DirectorsCut/Data/dcutDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Support/mnm/mnmBaseDataParser.hpp"
#include "Support/tmln/tmlnBaseDataParser.hpp"
#include "Support/tmln/tmlnChannelInfoParser.hpp"
#include "Support/tmln/tmlnParser.hpp"


//============================================================================
//============================================================================
namespace
{
//
//	constants
//
const chDefs::Name c_CUED = chDefs::MakeName('C', 'U', 'E', 'D');
const chDefs::Name c_DCUT = chDefs::MakeName('D', 'C', 'U', 'T');
const chDefs::Name c_DCBD = chDefs::MakeName('D', 'C', 'B', 'D');		// cut's base data
const chDefs::Name c_DRVS = chDefs::MakeName('D', 'R', 'V', 'S');
const chDefs::Name c_CCFD = chDefs::MakeName('C', 'C', 'F', 'D');


//------------------------------------------------------------------------
// Used to support old file formats of channel info. This list
// does not need to be updates as channels are added, it just
// represents the order of the channels when the file format
// for channel info was changed from indexing to name mapping.
//------------------------------------------------------------------------
const char* c_ChannelNameStrings[] = 
{ "Capture", "Camera" };
const int c_NumChannelStrings = 2;


//========================================================================
//   ReadCameraData
//========================================================================
void ReadCameraData(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						dcutScriptData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		// Info divided into base chunk and driver list
		if ( name == c_DCBD )
		{
			o_Data.m_BaseData.m_Name.Read(i_Reader);
			o_Data.m_BaseData.m_Description.Read(i_Reader);
		}
		else if ( name == c_DRVS )
		{
			tmlnParser::ReadDrivers(i_Reader, 
				dcutCamerasDataParser::GetChunkName(), 
				(o_Data.m_Drivers));
		}
		else if ( name == tmlnChannelInfoParser::GetChunkName() )
		{
			tmlnChannelInfoParser::ReadChannels(i_Reader, o_Data.m_ChannelInfo,
					c_ChannelNameStrings, c_NumChannelStrings);
		}

		i_Reader.FinishChunk();
	}
}

//========================================================================
//   WriteCameraData
//========================================================================
void WriteCameraData(	chWriter& o_Writer,
						const dcutScriptData& i_Data )
{
	const int l_DCUT_VERSION = 0;
	o_Writer.WriteChunkHeader( c_DCUT, l_DCUT_VERSION, true );

	//	write base info
	const int l_DCBD_VERSION = 0;
	o_Writer.WriteChunkHeader( c_DCBD, l_DCBD_VERSION, false );
	i_Data.m_BaseData.m_Name.Write(o_Writer);
	i_Data.m_BaseData.m_Description.Write(o_Writer);
	o_Writer.FinishChunk();

	const int l_DRVS_VERSION = 0;
	const bool c_bDRVS_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_DRVS, l_DRVS_VERSION, c_bDRVS_CONTAINER_CHUNK );
	tmlnParser::WriteDrivers(o_Writer,  
				dcutCamerasDataParser::GetChunkName(),  
				i_Data.m_Drivers);
	o_Writer.FinishChunk();

	// Write channels info
	const int l_CHNI_VERSION = 0;
	o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), l_CHNI_VERSION, false );
	tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_ChannelInfo);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();
}

//========================================================================
//   ReadCameraCueFormData
//========================================================================
//void ReadCameraCueFormData(	chReader& i_Reader,
//							chDefs::Version i_Version,
//							chDefs::Size i_Size,
//							dcutCueFormData& o_Data )
//{
//	// Not reading the camera names in this version
//	o_Data.m_CameraNames.clear();
//
//	chChunkParserUtil::Read(i_Reader, o_Data.m_Index[0]);
//	chChunkParserUtil::Read(i_Reader, o_Data.m_Index[1]);
//	chChunkParserUtil::Read(i_Reader, o_Data.m_Index[2]);
//	chChunkParserUtil::Read(i_Reader, o_Data.m_Index[3]);
//}


//========================================================================
//   WriteCameraCueFormData
//========================================================================
//void WriteCameraCueFormData(	chWriter& o_Writer,
//								const dcutCueFormData& i_Data )
//{
//	const int l_CCFD_VERSION = 1;
//	o_Writer.WriteChunkHeader( c_CCFD, l_CCFD_VERSION, false );
//
//	// I'm not going to read the camera names for now.
//
//	chChunkParserUtil::Write(o_Writer, i_Data.m_Index[0]);
//	chChunkParserUtil::Write(o_Writer, i_Data.m_Index[1]);
//	chChunkParserUtil::Write(o_Writer, i_Data.m_Index[2]);
//	chChunkParserUtil::Write(o_Writer, i_Data.m_Index[3]);
//
//	o_Writer.FinishChunk();
//}

}	// local namespace

//========================================================================
//  returns chunk name for this data type
//========================================================================
chDefs::Name  dcutCamerasDataParser::GetChunkName()
{
	return c_CUED;
}

//========================================================================
//   ReadData
//========================================================================
void dcutCamerasDataParser::ReadData(	chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										dcutCuesData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_DCUT )
		{
			// Scripted Camera chunk format, contains base info chunk and drivers
			dcutScriptData data;
			ReadCameraData(i_Reader, version, size, data);

			//	add it to the list
			o_Data.m_Items.push_back(data);

			//DBG_LOG1("Read camera %s", data.m_Name.c_str());
		}
		//else if (name == c_CCFD)
		//{
		//	ReadCameraCueFormData(i_Reader, version, size, o_Data.m_CueForm);
		//}
		i_Reader.FinishChunk();
	}
}

//========================================================================
//   WriteData
//========================================================================
void dcutCamerasDataParser::WriteData(	chWriter& o_Writer,
										const dcutCuesData& i_Data )
{
	const int c_CUED_VERSION = 1;
	o_Writer.WriteChunkHeader( c_CUED, c_CUED_VERSION, true );

	for (int i=0; i<i_Data.m_Items.size(); i++)
	{
		WriteCameraData(o_Writer, i_Data.m_Items[i]);
	}

	// Cue Form Data
//	WriteCameraCueFormData(o_Writer, i_Data.m_CueForm);

	o_Writer.FinishChunk();
}


