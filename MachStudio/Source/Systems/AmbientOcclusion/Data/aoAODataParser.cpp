/********************************************************************************************\
**  aoAODataParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Systems/AmbientOcclusion/Data/aoAODataParser.hpp"

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
const chDefs::Name c_AOCD = chDefs::MakeName('A', 'O', 'C', 'D');

// ao base data
const chDefs::Name c_AOCB = chDefs::MakeName('A', 'O', 'C', 'B');
// ao volumes params
const chDefs::Name c_AOVD = chDefs::MakeName('A', 'O', 'V', 'D');
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
chDefs::Name  aoAODataParser::GetChunkName()
{
	return c_AOCD;
}

//========================================================================
//   ReadAOData
//========================================================================
void aoAODataParser::ReadAOData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					aoAOData& o_Data )
{
	// AOCB chunk
	o_Data.m_AngleBias.Read(i_Reader);
	o_Data.m_AORadius.Read(i_Reader);

	// init the far radius to the near radius value prior to trying to read it.
	o_Data.m_AORadiusFar.SetValue(o_Data.m_AORadius.GetValue());

	o_Data.m_Attenuation.Read(i_Reader);
	o_Data.m_BlurSharpness.Read(i_Reader);
	o_Data.m_BlurWidth.Read(i_Reader);

	// contrast was removed in v1
	if (i_Version < 1)
	{
		prtyFloat contrast;
		contrast.Read(i_Reader);
	}
	else if (i_Version > 1)
	{
		// reinserted for v2.
		o_Data.m_Contrast.Read(i_Reader);

		if (i_Version >= 3)
		{
			o_Data.m_Color.Read(i_Reader);

			if (i_Version >= 4)
			{
				o_Data.m_AORadiusFar.Read(i_Reader);
				if( i_Version >= 5 )
				{
					o_Data.m_OverscanPixels.Read(i_Reader);
				}
			}
		}
	}
}
//========================================================================
//   ReadAOVolumesData
//========================================================================
void aoAODataParser::ReadAOVolumesData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					aoAOData& o_Data )
{
	// AOVD chunk
	o_Data.m_ClipPlaneEpsilon.Read(i_Reader);
	o_Data.m_NoClipPlaneEpsilon.Read(i_Reader);
	o_Data.m_AreaRatio.Read(i_Reader);
	o_Data.m_BehindPlaneEpsilon.Read(i_Reader);
}

//========================================================================
//   WriteAOData
//========================================================================
void aoAODataParser::WriteAOData(	chWriter& o_Writer,
					const aoAOData& i_Data )
{
	static int s_Version = 5;
	o_Writer.WriteChunkHeader( c_AOCB, s_Version, false );

	i_Data.m_AngleBias.Write(o_Writer);
	i_Data.m_AORadius.Write(o_Writer);
	i_Data.m_Attenuation.Write(o_Writer);
	i_Data.m_BlurSharpness.Write(o_Writer);
	i_Data.m_BlurWidth.Write(o_Writer);
//	i_Data.m_Contrast.Write(o_Writer); // removed in v1
	i_Data.m_Contrast.Write(o_Writer); // added in v2
	i_Data.m_Color.Write(o_Writer); // added in v3
	i_Data.m_AORadiusFar.Write(o_Writer); // added in v4
	i_Data.m_OverscanPixels.Write(o_Writer);//added in v5

	o_Writer.FinishChunk();
}

//========================================================================
//   WriteAOVolumesData
//========================================================================
void aoAODataParser::WriteAOVolumesData(	chWriter& o_Writer,
					const aoAOData& i_Data )
{
	static int s_Version = 0;
	o_Writer.WriteChunkHeader( c_AOVD, s_Version, false );

	i_Data.m_ClipPlaneEpsilon.Write(o_Writer);
	i_Data.m_NoClipPlaneEpsilon.Write(o_Writer);
	i_Data.m_AreaRatio.Write(o_Writer);
	i_Data.m_BehindPlaneEpsilon.Write(o_Writer);

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void aoAODataParser::ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				aoScriptData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_AOCB )
		{
			ReadAOData(i_Reader, version, size, o_Data.m_BaseData);
		}
		else if ( name == c_AOVD )
		{
			ReadAOVolumesData(i_Reader, version, size, o_Data.m_BaseData);
		}
		else if ( name == c_XTRP )
		{
			// Read custom property info
			xtraPropertyDataParser::ReadCustomPropertyData(i_Reader, version, size, o_Data.m_CustomProperties);
		}
		else if ( name == c_DRVS )
		{
			tmlnParser::ReadDrivers(i_Reader, 
									aoAODataParser::GetChunkName(),  
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
void aoAODataParser::WriteData( chWriter& o_Writer,
					   const aoScriptData& i_Data )
{
	static int s_Version = 0;
	o_Writer.WriteChunkHeader( c_AOCD, s_Version, true );

	WriteAOData(o_Writer, i_Data.m_BaseData);
	WriteAOVolumesData(o_Writer, i_Data.m_BaseData);

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
							aoAODataParser::GetChunkName(), 
							i_Data.m_Drivers);
	o_Writer.FinishChunk();

	// Write channels info
	const int l_CHNI_VERSION = 0;
	o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), l_CHNI_VERSION, false );
	tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_ChannelInfo);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();
}

