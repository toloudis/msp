/********************************************************************************************\
**  ptltDataParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Systems/PtLt/Data/ptltDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Support/mnm/mnmBaseDataParser.hpp"
#include "Support/tmln/tmlnBaseDataParser.hpp"
#include "Support/tmln/tmlnChannelInfoParser.hpp"
#include "Support/tmln/tmlnParser.hpp"
#include "Support/xtra/xtraPropertyDataParser.hpp"


//============================================================================
//============================================================================
namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_PTLD = chDefs::MakeName('P', 'T', 'L', 'D');
const chDefs::Name c_PTLT = chDefs::MakeName('P', 'T', 'L', 'T');
const chDefs::Name c_PTLC = chDefs::MakeName('P', 'T', 'L', 'C');
const chDefs::Name c_PTLB = chDefs::MakeName('P', 'T', 'L', 'B');
const chDefs::Name c_DRVS = chDefs::MakeName('D', 'R', 'V', 'S');
const chDefs::Name c_XTRP = chDefs::MakeName('X', 'T', 'R', 'P');

//------------------------------------------------------------------------
// Used to support old file formats of channel info. This list
// does not need to be updates as channels are added, it just
// represents the order of the channels when the file format
// for channel info was changed from indexing to name mapping.
//------------------------------------------------------------------------
const char* c_ChannelNameStrings[] = 
{ "Position", "Enabled", "Color", "Range", "Shadow Source", "Enable Diffuse", "Enable Specular",
 "Affects Fur", "Affects Glow", "Falloff", "Intensity" };
const int c_NumChannelStrings = 11;


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void check_falloff(chReader& i_Reader,
				   chDefs::Version i_Version)
{
	//bga - in version 8, there was a mistake in the file format that expanded
	// the falloff property to a vector4. This was never released in a version
	// to users, but handle it here to help out with programmers for the few days 
	// that the code was at version 8
	if (i_Version == 8)
	{
		envType::Float32 cubic_falloff;
		i_Reader.Read(cubic_falloff);
	}
}

//------------------------------------------------------------------------
//   read_pointlight_data
//------------------------------------------------------------------------
void read_pointlight_data(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							ptltData& o_Data )
{
	if ( i_Version >= 3 )
	{
		o_Data.m_Color.Read(i_Reader);
		o_Data.m_Enabled.Read(i_Reader);
		o_Data.m_ShadowSource.Read(i_Reader);
		check_falloff(i_Reader, i_Version); // check for version 8 problems
		o_Data.m_Falloff.Read(i_Reader);
		o_Data.m_Range.Read(i_Reader);

		if ( i_Version >= 4 )
		{
			o_Data.m_bDiffuseEnabled.Read(i_Reader);
			o_Data.m_bSpecularEnabled.Read(i_Reader);

			if (i_Version >= 5)
			{
				o_Data.m_bAffectsFur.Read(i_Reader);
				if (i_Version >= 6)
				{
					o_Data.m_bAffectsGlow.Read(i_Reader);
					if (i_Version >= 7)
					{
						o_Data.m_Intensity.Read(i_Reader);
					}
				}
			}
		}
	}
	else
	{
		o_Data.m_Color.Read(i_Reader);
		o_Data.m_Enabled.Read(i_Reader);
		o_Data.m_ShadowSource.Read(i_Reader);
		o_Data.m_Position.Read(i_Reader);
		o_Data.m_Falloff.Read(i_Reader);
		o_Data.m_Range.Read(i_Reader);

		if ( i_Version >= 1 )
		{
			std::string name;
			chChunkParserUtil::Read(i_Reader, name);
			o_Data.m_Name.SetValue( name );

			if (i_Version >= 2)
			{
				nameUID uid;
				chChunkParserUtil::Read(i_Reader, uid);
				o_Data.m_Name.SetUID( uid );
			}
		}
	}

	//maPoint3d pos = o_Data.m_Position;
	//DBG_LOG3( "read ptlt pos (%6.3f, %6.3f, %6.3f)", pos.GetX(), pos.GetY(), pos.GetZ() );
}


//------------------------------------------------------------------------
//   read_base_data
//------------------------------------------------------------------------
void read_base_data(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						ptltData& o_Data )
{
	o_Data.m_Name.Read(i_Reader);
	o_Data.m_bEditorVisible.Read(i_Reader);
	o_Data.m_Position.Read(i_Reader);

	//o_Data.m_Orientation.Read(i_Reader);
	//o_Data.m_Filename.Read(i_Reader);

	// HACK: [rjk] oh this hurts.  because some files wrote out invalid IDs
	//	this hack will find an ID "up higher" so they don't get assigned an
	//	ID of something that hasn't been loaded yet (but has that ID).
	//	This is temporary until the "old" scenes are re-saved. remove nameMgr.hpp also
	if ( i_Version < 1 )
	{
		if ( o_Data.m_Name.GetUID() == nameString::e_InvalidUID )
		{
			o_Data.m_Name.RegisterName();
			if (o_Data.m_Name.GetUID() < 150)		// if it generated a low UID, force it higher.  all others will be above this one
			{
				o_Data.m_Name.SetUID(150);
				o_Data.m_Name.RegisterName();
			}
		}
	}
	//DBG_LOG2( "Reading ptlt %d %s", uid, name.c_str() );
}

}


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  ptltDataParser::GetChunkName()
{
	return c_PTLD;
}

//------------------------------------------------------------------------
//   ReadPointLightData
//------------------------------------------------------------------------
void  ptltDataParser::ReadPointLightData(	chReader& i_Reader,
											chDefs::Version i_Version,
											chDefs::Size i_Size,
											ptltScriptData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	//int nData;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		// Info divided into base chunk and driver list
		if ( name == c_PTLT )
		{
			read_pointlight_data(i_Reader, version, size, o_Data.m_BaseData);
		}
		else if ( name == c_PTLB )
		{
			read_base_data(i_Reader, version, size, o_Data.m_BaseData);
		}
		else if ( name == c_XTRP )
		{
			// Read custom property info
			xtraPropertyDataParser::ReadCustomPropertyData(i_Reader, version, size, o_Data.m_CustomProperties);
		}
		else if ( name == c_DRVS )
		{
			tmlnParser::ReadDrivers(i_Reader, 
									ptltDataParser::GetChunkName(), 
									o_Data.m_Drivers);
		}
		else if ( name == mnmBaseDataParser::GetChunkName() )		// chunk PTLC version 0
		{
			mnmBaseData bdata;
			mnmBaseDataParser::ReadData(i_Reader, 
							ptltDataParser::GetChunkName(), 
							version, size, bdata);
			o_Data.m_BaseData.m_bEditorVisible.SetValue(bdata.m_bEditorVisible);
			o_Data.m_BaseData.m_Name.SetValue(bdata.m_Name);
			o_Data.m_BaseData.m_Position.SetValue(bdata.m_Position);
		}
		else if ( name == tmlnBaseDataParser::GetChunkName() )		// chunk PTLC version 0
		{
			tmlnBaseData tmln_data;
			tmlnBaseDataParser::ReadData(i_Reader, 
							ptltDataParser::GetChunkName(), 
							version, size, tmln_data);

			envSTLHelpers::DeleteContainer(o_Data.m_Drivers);

			int num_drivers = tmln_data.m_Drivers.size();
			for (int i=0; i<num_drivers; i++)
			{
				o_Data.m_Drivers.push_back(tmln_data.m_Drivers[i]->Clone());
			}
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
//   WritePointLightData
//------------------------------------------------------------------------
void  ptltDataParser::WritePointLightData(	chWriter& o_Writer,
											const ptltScriptData& i_Data )
{
	const int l_PTLTCHNK_VERSION = 3;
	o_Writer.WriteChunkHeader( c_PTLC, l_PTLTCHNK_VERSION, true );

	// Write light info
	//
	//	version 1 = all data written from light data
	//	version 2 = use of mnmBaseData and tmlnBaseData
	//	version 3 = all data written from ptltScriptData structure
	//  version 4 = adds DiffuseEnabled and SpecularEnabled flags
	//
	const int l_PTLTITEM_VERSION = 7; //bga - Note, the next version should be 9, not 8!
	o_Writer.WriteChunkHeader( c_PTLT, l_PTLTITEM_VERSION, false );
	i_Data.m_BaseData.m_Color.Write(o_Writer);
	i_Data.m_BaseData.m_Enabled.Write(o_Writer);
	i_Data.m_BaseData.m_ShadowSource.Write(o_Writer);
	i_Data.m_BaseData.m_Falloff.Write(o_Writer);
	i_Data.m_BaseData.m_Range.Write(o_Writer);
	i_Data.m_BaseData.m_bDiffuseEnabled.Write(o_Writer);
	i_Data.m_BaseData.m_bSpecularEnabled.Write(o_Writer);
	// version 5:
	i_Data.m_BaseData.m_bAffectsFur.Write(o_Writer);
	// version 6:
	i_Data.m_BaseData.m_bAffectsGlow.Write(o_Writer);
	// version 7:
	i_Data.m_BaseData.m_Intensity.Write(o_Writer);

	o_Writer.FinishChunk();

	// Write base info
	const int l_PTLB_VERSION = 1;
	o_Writer.WriteChunkHeader( c_PTLB, l_PTLB_VERSION, false );
	i_Data.m_BaseData.m_Name.Write(o_Writer);
	i_Data.m_BaseData.m_bEditorVisible.Write(o_Writer);
	i_Data.m_BaseData.m_Position.Write(o_Writer);
	//i_Data.m_BaseData.m_Orientation.Write(o_Writer);
	//i_Data.m_BaseData.m_Filename.Write(o_Writer);
	o_Writer.FinishChunk();

	// Write custom property info
	const int l_XTRP_VERSION = 0;
	const bool c_bXTRP_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_XTRP, l_XTRP_VERSION, c_bXTRP_CONTAINER_CHUNK );
	xtraPropertyDataParser::WriteCustomPropertyData(o_Writer, i_Data.m_CustomProperties);
	o_Writer.FinishChunk();

	// Write driver info
	const int l_DRVS_VERSION = 0;
	const bool c_bDRVS_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_DRVS, l_DRVS_VERSION, c_bDRVS_CONTAINER_CHUNK );
	tmlnParser::WriteDrivers(o_Writer, 
							ptltDataParser::GetChunkName(), 
							i_Data.m_Drivers);
	o_Writer.FinishChunk();

	// Write channels info
	const int l_CHNI_VERSION = 0;
	o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), l_CHNI_VERSION, false );
	tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_ChannelInfo);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void  ptltDataParser::ReadData(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								ptltPointLightsData& o_Data )
{

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_PTLC )
		{
			// New chunk format, contains base info chunk and drivers
			ptltScriptData data;
			ReadPointLightData(i_Reader, version, size, data);
			o_Data.m_Items.push_back(data);
		}
		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void  ptltDataParser::WriteData(chWriter& o_Writer,
								const ptltPointLightsData& i_Data )
{
	const int l_PTLTLIST_VERSION = 0;
	o_Writer.WriteChunkHeader( c_PTLD, l_PTLTLIST_VERSION, true );
	for (int i=0; i<i_Data.m_Items.size(); i++)
	{
		WritePointLightData(o_Writer, i_Data.m_Items[i]);
	}
	o_Writer.FinishChunk();
}

