/********************************************************************************************\
**  giGIDataParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#include "Systems/GlobalIllumination/Data/giGIDataParser.hpp"

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
const chDefs::Name c_GICD = chDefs::MakeName('G', 'I', 'C', 'D');

// gi base data
const chDefs::Name c_GICB = chDefs::MakeName('G', 'I', 'C', 'B');
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
chDefs::Name  giGIDataParser::GetChunkName()
{
	return c_GICD;
}

//========================================================================
//   ReadGIData
//========================================================================
void giGIDataParser::ReadGIData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					giGIData& o_Data )
{
	// GICB chunk
	o_Data.m_AngleBias.Read(i_Reader);
	o_Data.m_GIRadius.Read(i_Reader);

	// init the far radius to the near radius value prior to trying to read it.
	o_Data.m_GIRadiusFar.SetValue(o_Data.m_GIRadius.GetValue());

	o_Data.m_Attenuation.Read(i_Reader);
	o_Data.m_BlurSharpness.Read(i_Reader);
	o_Data.m_BlurWidth.Read(i_Reader);
	o_Data.m_Contrast.Read(i_Reader);
	o_Data.m_Color.Read(i_Reader);
	o_Data.m_GIRadiusFar.Read(i_Reader);
	o_Data.m_OverscanPixels.Read(i_Reader);

	if (i_Version >= 1)
	{	
		o_Data.m_LPVScale.Read(i_Reader);
		o_Data.m_LPVIteration.Read(i_Reader);
		o_Data.m_LPVVolumeSize.Read(i_Reader);
		o_Data.m_LPVGIFalloff.Read(i_Reader);
		o_Data.m_LPVRSMSize.Read(i_Reader);
	}
}

//========================================================================
//   WriteGIData
//========================================================================
void giGIDataParser::WriteGIData(	chWriter& o_Writer,
					const giGIData& i_Data )
{
	static int s_Version = 1;
	o_Writer.WriteChunkHeader( c_GICB, s_Version, false );

	// v0
	i_Data.m_AngleBias.Write(o_Writer);
	i_Data.m_GIRadius.Write(o_Writer);
	i_Data.m_Attenuation.Write(o_Writer);
	i_Data.m_BlurSharpness.Write(o_Writer);
	i_Data.m_BlurWidth.Write(o_Writer);
	i_Data.m_Contrast.Write(o_Writer);
	i_Data.m_Color.Write(o_Writer);
	i_Data.m_GIRadiusFar.Write(o_Writer);
	i_Data.m_OverscanPixels.Write(o_Writer);

	// version 1
	i_Data.m_LPVScale.Write(o_Writer);
	i_Data.m_LPVIteration.Write(o_Writer);
	i_Data.m_LPVVolumeSize.Write(o_Writer);
	i_Data.m_LPVGIFalloff.Write(o_Writer);
	i_Data.m_LPVRSMSize.Write(o_Writer);

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void giGIDataParser::ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				giScriptData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_GICB )
		{
			ReadGIData(i_Reader, version, size, o_Data.m_BaseData);
		}
		else if ( name == c_XTRP )
		{
			// Read custom property info
			xtraPropertyDataParser::ReadCustomPropertyData(i_Reader, version, size, o_Data.m_CustomProperties);
		}
		else if ( name == c_DRVS )
		{
			tmlnParser::ReadDrivers(i_Reader, 
									giGIDataParser::GetChunkName(),  
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
void giGIDataParser::WriteData( chWriter& o_Writer,
					   const giScriptData& i_Data )
{
	static int s_Version = 0;
	o_Writer.WriteChunkHeader( c_GICD, s_Version, true );

	WriteGIData(o_Writer, i_Data.m_BaseData);

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
							giGIDataParser::GetChunkName(), 
							i_Data.m_Drivers);
	o_Writer.FinishChunk();

	// Write channels info
	const int l_CHNI_VERSION = 0;
	o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), l_CHNI_VERSION, false );
	tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_ChannelInfo);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();
}

