/********************************************************************************************\
**  dirltDataParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "dirltDataParser.hpp"

#include "mnmBaseDataParser.hpp"
#include "tmlnBaseDataParser.hpp"
#include "tmlnChannelInfoParser.hpp"
#include "tmlnParser.hpp"

#include "chBinReader.hpp"
#include "chBinWriter.hpp"
#include "chChunkParserUtil.hpp"
#include "chExceptionX.hpp"
#include "dbgLog.hpp"
#include "envSTLHelpers.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "gfFileBin.hpp"



namespace
{
//========================================================================
//========================================================================
const chDefs::Name c_DILD = chDefs::MakeName('D', 'I', 'L', 'D');	// dir light data (all)
const chDefs::Name c_DILC = chDefs::MakeName('D', 'I', 'L', 'C');	//	+-- light data chunk
const chDefs::Name c_DILT = chDefs::MakeName('D', 'I', 'L', 'T');	//	  +- dir light data
const chDefs::Name c_DRVS = chDefs::MakeName('D', 'R', 'V', 'S');	//	  +- driver chunk


//========================================================================
//   read_base_data
//========================================================================
void read_base_data(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						dirltData& o_Data )
{
	o_Data.m_Color.Read(i_Reader);
	o_Data.m_Enabled.Read(i_Reader);
	o_Data.m_ShadowSource.Read(i_Reader);
	o_Data.m_Direction.Read(i_Reader);

	if ( i_Version >= 3 )
	{
		o_Data.m_Name.Read(i_Reader);

		o_Data.m_Filename.Read(i_Reader);
		o_Data.m_Position.Read(i_Reader);
		o_Data.m_bEditorVisible.Read(i_Reader);

		if ( i_Version >= 4 )
		{
			o_Data.m_bDiffuseEnabled.Read(i_Reader);
			o_Data.m_bSpecularEnabled.Read(i_Reader);
		}
	}
	else if ( i_Version < 2 )
	{
		if ( i_Version >= 0 )
		{
			std::string name;
			chChunkParserUtil::Read(i_Reader, name);
			o_Data.m_Name.SetName( name );

			if (i_Version >= 1)
			{
				nameUID uid;
				chChunkParserUtil::Read(i_Reader, uid);
				o_Data.m_Name.SetUID( uid );
			}
		}
	}

	//DBG_LOG3( "read dirlt pos (%6.3f, %6.3f, %6.3f)", o_Data.m_Direction.GetX(), o_Data.m_Direction.GetY(), o_Data.m_Direction.GetZ() );
}

}


//========================================================================
//  returns chunk name for this data type
//========================================================================
chDefs::Name  dirltDataParser::GetChunkName()
{
	return c_DILD;
}

//========================================================================
//   ReadDirLightData
//========================================================================
void dirltDataParser::ReadDirLightData(	chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										dirltScriptData& o_Data )
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
			mnmBaseDataParser::ReadData( i_Reader, version, size, data );
			o_Data.m_BaseData.m_bEditorVisible	= data.m_bEditorVisible;
			o_Data.m_BaseData.m_Filename		= data.m_Filename;
			o_Data.m_BaseData.m_Name			= data.m_Name;
			o_Data.m_BaseData.m_Position		= data.m_Position;
		}
		else if ( name == tmlnBaseDataParser::GetChunkName() )
		{
			tmlnBaseData tmln_data;
			tmlnBaseDataParser::ReadData(i_Reader, version, size, tmln_data);

			envSTLHelpers::DeleteContainer(o_Data.m_Drivers);

			int num_drivers = tmln_data.m_Drivers.size();
			for (int i=0; i<num_drivers; i++)
			{
				o_Data.m_Drivers.push_back(tmln_data.m_Drivers[i]->Clone());
			}
		}
		else if ( name == c_DILT )
		{
			read_base_data(i_Reader, version, size, o_Data.m_BaseData);
		}
		else if ( name == c_DRVS )	// old - version 1
		{
			tmlnParser::ReadDrivers(i_Reader, o_Data.m_Drivers);
		}
		else if ( name == tmlnChannelInfoParser::GetChunkName() )
		{
			tmlnChannelInfoParser::ReadChannels(i_Reader, (o_Data.m_Channels));
		}

		i_Reader.FinishChunk();
	}
}

//========================================================================
//   WriteDirLightData
//========================================================================
void dirltDataParser::WriteDirLightData(chWriter& o_Writer,
										const dirltScriptData& i_Data )
{
	const int l_DILC_VERSION = 4;
	o_Writer.WriteChunkHeader( c_DILC, l_DILC_VERSION, true );

	// Write base info
	const int l_DILT_VERSION = 4;
	o_Writer.WriteChunkHeader( c_DILT, l_DILT_VERSION, false );

	i_Data.m_BaseData.m_Color.Write(o_Writer);
	i_Data.m_BaseData.m_Enabled.Write(o_Writer);
	i_Data.m_BaseData.m_ShadowSource.Write(o_Writer);
	i_Data.m_BaseData.m_Direction.Write(o_Writer);
	i_Data.m_BaseData.m_Name.Write(o_Writer);
	i_Data.m_BaseData.m_Filename.Write(o_Writer);
	i_Data.m_BaseData.m_Position.Write(o_Writer);
	i_Data.m_BaseData.m_bEditorVisible.Write(o_Writer);

	//DBG_LOG3( "write dirlt pos (%6.3f, %6.3f, %6.3f)", i_Data.m_Direction.GetX(), i_Data.m_Direction.GetY(), i_Data.m_Direction.GetZ() );

	// version 4
	i_Data.m_BaseData.m_bDiffuseEnabled.Write(o_Writer);
	i_Data.m_BaseData.m_bSpecularEnabled.Write(o_Writer);

	o_Writer.FinishChunk();

	// Write driver info
	const int l_DRVS_VERSION = 1;
	o_Writer.WriteChunkHeader( c_DRVS, l_DRVS_VERSION, false );
	tmlnParser::WriteDrivers(o_Writer, i_Data.m_Drivers);
	o_Writer.FinishChunk();

	// Write channels info
	const int l_CHNI_VERSION = 0;
	o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), l_CHNI_VERSION, false );
	tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_Channels);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();
}

//========================================================================
//   ReadData
//========================================================================
void dirltDataParser::ReadData(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								dirltDirLightsData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_DILC )
		{
			// New chunk format, contains base info chunk and drivers
			dirltScriptData data;
			ReadDirLightData(i_Reader, version, size, data);
			o_Data.m_Items.push_back(data);
		}
		i_Reader.FinishChunk();
	}
}

//========================================================================
//   WriteData
//========================================================================
void dirltDataParser::WriteData(chWriter& o_Writer,
								const dirltDirLightsData& i_Data )
{
	const int l_DILD_VERSION = 1;
	o_Writer.WriteChunkHeader( c_DILD, l_DILD_VERSION, true );

	//DBG_LOG1( "Writing out %d diretional lights", i_Data.m_Items.size() );

	for (int i=0; i<i_Data.m_Items.size(); i++)
	{
		WriteDirLightData(o_Writer, i_Data.m_Items[i]);
	}
	o_Writer.FinishChunk();
}

