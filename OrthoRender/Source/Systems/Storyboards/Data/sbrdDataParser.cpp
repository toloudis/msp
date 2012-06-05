/********************************************************************************************\
**  sbrdDataParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Storyboards/Data/sbrdDataParser.hpp"

#include "Systems/Storyboards/Data/sbrdListData.hpp"

#include "Support/mnm/mnmBaseDataParser.hpp"
#include "Support/tmln/tmlnBaseDataParser.hpp"
#include "Support/tmln/tmlnChannelInfoParser.hpp"
#include "Support/tmln/tmlnParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/name/nameMgr.hpp"


//============================================================================
//============================================================================
namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_SBRD = chDefs::MakeName('S', 'B', 'R', 'D');	// sbrd data
const chDefs::Name c_SBRL = chDefs::MakeName('S', 'B', 'R', 'L');	// +-sbrd list chunk
const chDefs::Name c_SBRO = chDefs::MakeName('S', 'B', 'R', 'C');	// +-sbrd object chunk
const chDefs::Name c_SBDA = chDefs::MakeName('S', 'B', 'D', 'A');	//	 +-sbrd data (part of chunk)
const chDefs::Name c_DRVS = chDefs::MakeName('D', 'R', 'V', 'S');	//	 +-sbrd drivers (part of chunk)


//------------------------------------------------------------------------
// Used to support old file formats of channel info. This list
// does not need to be updates as channels are added, it just
// represents the order of the channels when the file format
// for channel info was changed from indexing to name mapping.
//------------------------------------------------------------------------
const char* c_ChannelNameStrings[] = 
{ "Position", "Texture", "Orientation", "Scale", "Visible", "Sound" };
const int c_NumChannelStrings = 6;
}


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  sbrdDataParser::GetChunkName()
{
	return c_SBRD;
}

//------------------------------------------------------------------------
//   ReadObjectChunk
//------------------------------------------------------------------------
void sbrdDataParser::ReadObjectChunk(chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										sbrdScriptData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		// Info divided into base chunk and driver list
		if ( name == mnmBaseDataParser::GetChunkName() )
		{
			mnmBaseData data;
			mnmBaseDataParser::ReadData(i_Reader, 
							sbrdDataParser::GetChunkName(), 
							version, size, data);

			o_Data.m_BaseData.m_bEditorVisible	= data.m_bEditorVisible;
			o_Data.m_BaseData.m_Filename		= data.m_Filename;
			o_Data.m_BaseData.m_Name			= data.m_Name;
//			o_Data.m_BaseData.m_Orientation		= data.m_Orientation;
			o_Data.m_BaseData.m_Position		= data.m_Position;
		}
		else if ( name == tmlnBaseDataParser::GetChunkName() )
		{
			tmlnBaseData tmln_data;
			tmlnBaseDataParser::ReadData(i_Reader, 
							sbrdDataParser::GetChunkName(), 
							version, size, tmln_data);

			envSTLHelpers::DeleteContainer(o_Data.m_Drivers);

			int num_drivers = tmln_data.m_Drivers.size();
			for (int i=0; i<num_drivers; i++)
			{
				o_Data.m_Drivers.push_back(tmln_data.m_Drivers[i]->Clone());
			}
		}
		else if ( name == c_SBDA )
		{
			o_Data.m_BaseData.m_Name.Read(i_Reader);
			o_Data.m_BaseData.m_bEditorVisible.Read(i_Reader);
			o_Data.m_BaseData.m_Position.Read(i_Reader);
			o_Data.m_BaseData.m_Orientation.Read(i_Reader);
			o_Data.m_BaseData.m_Scale.Read(i_Reader);
			o_Data.m_BaseData.m_Filename.Read(i_Reader);
			o_Data.m_BaseData.m_bOrientToCamera.Read(i_Reader);
			o_Data.m_BaseData.m_bAdditiveMaterial.Read(i_Reader);
			o_Data.m_BaseData.m_Color.Read(i_Reader);
		}
		else if ( name == c_DRVS )
		{
			tmlnParser::ReadDrivers(i_Reader, 
									sbrdDataParser::GetChunkName(),  
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

//------------------------------------------------------------------------
//   WriteObjectChunk
//------------------------------------------------------------------------
void sbrdDataParser::WriteObjectChunk(	chWriter& o_Writer,
										const sbrdScriptData& i_Data )
{
	const int l_SBRDCHUNK_VERSION = 0;
	o_Writer.WriteChunkHeader( c_SBRO, l_SBRDCHUNK_VERSION, false );

	//	data part
	const int l_SBDA_VERSION = 0;
	o_Writer.WriteChunkHeader( c_SBDA, l_SBDA_VERSION, false );

	i_Data.m_BaseData.m_Name.Write(o_Writer);
	i_Data.m_BaseData.m_bEditorVisible.Write(o_Writer);
	i_Data.m_BaseData.m_Position.Write(o_Writer);
	i_Data.m_BaseData.m_Orientation.Write(o_Writer);
	i_Data.m_BaseData.m_Scale.Write(o_Writer);
	i_Data.m_BaseData.m_Filename.Write(o_Writer);
	i_Data.m_BaseData.m_bOrientToCamera.Write(o_Writer);
	i_Data.m_BaseData.m_bAdditiveMaterial.Write(o_Writer);
	i_Data.m_BaseData.m_Color.Write(o_Writer);

	o_Writer.FinishChunk();

	DBG_LOG1("Storyboard object writing out %d drivers", i_Data.m_Drivers.size());

	// Write drivers info
	const int l_DRVS_VERSION = 0;
	const bool c_bDRVS_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_DRVS, l_DRVS_VERSION, c_bDRVS_CONTAINER_CHUNK );
	tmlnParser::WriteDrivers(o_Writer, 
							sbrdDataParser::GetChunkName(), 
							i_Data.m_Drivers);
	o_Writer.FinishChunk();

	DBG_LOG1("Storyboard object writing out %d channels", i_Data.m_ChannelInfo.size());

	// Write channels info
	const int l_CHNI_VERSION = 0;
	o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), l_CHNI_VERSION, false );
	tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_ChannelInfo);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadListChunk
//------------------------------------------------------------------------
//static 
void sbrdDataParser::ReadListChunk(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									sbrdListData& o_Data )
{
	int num_sbrds;

	i_Reader.Read( num_sbrds );
	o_Data.m_Filenames.resize( num_sbrds );

	for (int i=0; i < num_sbrds ; i++)
	{
		o_Data.m_Filenames[i].Read(i_Reader);
	}
}

//------------------------------------------------------------------------
//   WriteListChunk
//------------------------------------------------------------------------
//static 
void sbrdDataParser::WriteListChunk(	chWriter& o_Writer,
										const sbrdListData& i_Data )
{
	const int l_SBRDLIST_VERSION = 1;
	const bool c_bSBRL_CONTAINER_CHUNK = false;
	o_Writer.WriteChunkHeader( c_SBRL, l_SBRDLIST_VERSION, c_bSBRL_CONTAINER_CHUNK );

	int num_sbrds = i_Data.m_Filenames.size();

	o_Writer.Write( num_sbrds );

	for (int i=0; i < num_sbrds ; i++)
	{
		i_Data.m_Filenames[i].Write(o_Writer);
	}

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void sbrdDataParser::ReadData(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								sbrdListData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_SBRL )
		{
			ReadListChunk( i_Reader, version, size, o_Data );
		}
		else if ( name == c_SBRO )
		{
			o_Data.m_Items.resize(1);
			ReadObjectChunk(i_Reader, version, size, o_Data.m_Items[0]);

			//DBG_LOG1("Storyboard Object has %d drivers", o_Data.m_Items[0].m_Drivers.size());
			//DBG_LOG1("Storyboard Object has %d channels", o_Data.m_Items[0].m_Channels.size());
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void sbrdDataParser::WriteData(	chWriter& o_Writer,
								const sbrdListData& i_Data )
{
	const int l_SBRDDATA_VERSION = 0;
	o_Writer.WriteChunkHeader( c_SBRD, l_SBRDDATA_VERSION, true );

	WriteListChunk(o_Writer, i_Data);

	//	only write the object is there is one
	if (i_Data.m_Items.size() > 0)
	{
		WriteObjectChunk(o_Writer, i_Data.m_Items[0]);
	}

	o_Writer.FinishChunk();
}
