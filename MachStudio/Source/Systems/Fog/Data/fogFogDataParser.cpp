/********************************************************************************************\
**  fogFogDataParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Fog/Data/fogFogDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Support/tmln/tmlnChannelInfoParser.hpp"
#include "Support/tmln/tmlnParser.hpp"
#include "Support/xtra/xtraPropertyDataParser.hpp"


namespace
{
//========================================================================
//========================================================================
const chDefs::Name c_FOGD = chDefs::MakeName('F', 'O', 'G', 'D');

// fog base data
const chDefs::Name c_FOGCB = chDefs::MakeName('F', 'O', 'G', 'B');
// drivers
const chDefs::Name c_DRVS = chDefs::MakeName('D', 'R', 'V', 'S');
const chDefs::Name c_XTRP = chDefs::MakeName('X', 'T', 'R', 'P');

//------------------------------------------------------------------------
// Used to support old file formats of channel info. This list
// does not need to be updates as channels are added, it just
// represents the order of the channels when the file format
// for channel info was changed from indexing to name mapping.
//------------------------------------------------------------------------
const char* c_ChannelNameStrings[] = {""};
const int c_NumChannelStrings = 0;

}	// local namespace

//========================================================================
//  returns chunk name for this data type
//========================================================================
chDefs::Name  fogFogDataParser::GetChunkName()
{
	return c_FOGD;
}

//========================================================================
//   ReadLegacyFogData
//========================================================================
void fogFogDataParser::ReadLegacyFogData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					fogFogData& o_Data )
{
	if (i_Version == 0)
	{
		envType::Int32 mode;
		i_Reader.Read(mode);
		o_Data.m_Mode.SetValue(fogFogData::FogMode(mode));
	}
	else
	{
		envType::Int8 mode;
		i_Reader.Read(mode);
		o_Data.m_Mode.SetValue(fogFogData::FogMode(mode));
	}

	o_Data.m_bEnable.Read(i_Reader);	
	//o_Data.m_bWorldOrientation.Read(i_Reader);
	o_Data.m_Color.Read(i_Reader);
	o_Data.m_Density.Read(i_Reader);
	o_Data.m_Start.Read(i_Reader);
	o_Data.m_End.Read(i_Reader);
}

//========================================================================
//   ReadFogData
//========================================================================
void fogFogDataParser::ReadFogData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					fogFogData& o_Data )
{
	// FOGB chunk
	o_Data.m_Mode.Read(i_Reader);
	o_Data.m_Color.Read(i_Reader);
	o_Data.m_Density.Read(i_Reader);
	o_Data.m_Start.Read(i_Reader);
	o_Data.m_End.Read(i_Reader);
	if (i_Version >= 1)
	{
		o_Data.m_AltitudeStart.Read(i_Reader);
		o_Data.m_AltitudeEnd.Read(i_Reader);
		o_Data.m_AltitudeDensity.Read(i_Reader);
	}
	if (i_Version >= 2) 
	{
		o_Data.m_bEnable.Read(i_Reader);
	}
	if (i_Version >= 3)
	{
		o_Data.m_bWorldOrientation.Read(i_Reader);
		o_Data.m_Orientation.Read(i_Reader);
	}
}

//========================================================================
//   WriteFogData
//========================================================================
void fogFogDataParser::WriteFogData(	chWriter& o_Writer,
					const fogFogData& i_Data ) 
{
	static int s_Version = 3;
	o_Writer.WriteChunkHeader( c_FOGCB, s_Version, false );

	i_Data.m_Mode.Write(o_Writer);
	i_Data.m_Color.Write(o_Writer);
	i_Data.m_Density.Write(o_Writer);
	i_Data.m_Start.Write(o_Writer);
	i_Data.m_End.Write(o_Writer);
	i_Data.m_AltitudeStart.Write(o_Writer);
	i_Data.m_AltitudeEnd.Write(o_Writer);
	i_Data.m_AltitudeDensity.Write(o_Writer);

	i_Data.m_bEnable.Write(o_Writer);

	//verision 3
	i_Data.m_bWorldOrientation.Write(o_Writer);
	i_Data.m_Orientation.Write(o_Writer);

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void fogFogDataParser::ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				fogScriptData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_FOGCB )
		{
			ReadFogData(i_Reader, version, size, o_Data.m_BaseData);
		}
		else if ( name == c_XTRP )
		{
			// Read custom property info
			xtraPropertyDataParser::ReadCustomPropertyData(i_Reader, version, size, o_Data.m_CustomProperties);
		}
		else if ( name == c_DRVS )
		{
			tmlnParser::ReadDrivers(i_Reader, 
									fogFogDataParser::GetChunkName(),  
									(o_Data.m_Drivers) );
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
//   WriteData
//------------------------------------------------------------------------
void fogFogDataParser::WriteData( chWriter& o_Writer,
					   const fogScriptData& i_Data )
{
	static int s_Version = 1;
	o_Writer.WriteChunkHeader( c_FOGD, s_Version, true );

	WriteFogData(o_Writer, i_Data.m_BaseData);

	// Write custom property info
	const int l_XTRP_VERSION = 0;
	const bool c_bXTRP_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_XTRP, l_XTRP_VERSION, c_bXTRP_CONTAINER_CHUNK );
	xtraPropertyDataParser::WriteCustomPropertyData(o_Writer, i_Data.m_CustomProperties);
	o_Writer.FinishChunk();
	
	// Write base info
	const int l_DRVS_VERSION = 0;
	const bool c_bDRVS_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_DRVS, l_DRVS_VERSION, c_bDRVS_CONTAINER_CHUNK );
	tmlnParser::WriteDrivers(o_Writer, 
							fogFogDataParser::GetChunkName(), 
							i_Data.m_Drivers);
	o_Writer.FinishChunk();

	// Write channels info
	const int l_CHNI_VERSION = 0;
	o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), l_CHNI_VERSION, false );
	tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_ChannelInfo);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();
}

