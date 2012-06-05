/********************************************************************************************\
**  propDataParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Props/Data/propDataParser.hpp"

#include "Support/dyn/dynControlDataParser.hpp"
#include "Support/fgmt/fgmtFragmentParser.hpp"
#include "Support/mtrl/mtrlMaterialParser.hpp"
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
#include "Support/mnm/mnmBaseDataParser.hpp"
#include "Core/name/nameMgr.hpp"
#include "Support/tmln/tmlnBaseDataParser.hpp"
#include "Tool/gui/guiMessageBox.hpp"


namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_PROPS = chDefs::MakeName('P', 'R', 'P', 'S');	// prop list
const chDefs::Name c_PROPC = chDefs::MakeName('P', 'R', 'P', 'C');	// prop chunk
const chDefs::Name c_PROPD = chDefs::MakeName('P', 'R', 'P', 'D');	//	prop data (part of chunk)
const chDefs::Name c_DRVS = chDefs::MakeName('D', 'R', 'V', 'S');	//	prop drivers (part of chunk)
const chDefs::Name c_CTRS = chDefs::MakeName('C', 'T', 'R', 'S');	//	prop controls (part of chunk)
const chDefs::Name c_MTLS = chDefs::MakeName('M', 'T', 'L', 'S');	//	prop materials (part of chunk)
const chDefs::Name c_FRGS = chDefs::MakeName('F', 'R', 'G', 'S');	//	prop fragment flags (part of chunk)


//------------------------------------------------------------------------
// Used to support old file formats of channel info. This list
// does not need to be updates as channels are added, it just
// represents the order of the channels when the file format
// for channel info was changed from indexing to name mapping.
//------------------------------------------------------------------------
const char* c_ChannelNameStrings[] = 
{ "Anim-Full", "Position", "Orientation", "Visible", "Sound", "SubAnim", "Scale" };
const int c_NumChannelStrings = 7;
}


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  propDataParser::GetChunkName()
{
	return c_PROPS;
}

//------------------------------------------------------------------------
//   ReadPropChunk
//------------------------------------------------------------------------
void propDataParser::ReadPropChunk(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									propScriptData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		// Info divided into base chunk and driver list
		if ( name == c_CTRS )
		{
			dynControlDataParser::ReadControlData(i_Reader, version, size, o_Data.m_Controls);
		}
		else if ( name == c_MTLS )
		{
			mtrlMaterialParser::ReadMaterialData(i_Reader, version, size, o_Data.m_Materials);
		}
		else if ( name == c_FRGS )
		{
			fgmtFragmentParser::ReadFragmentData(i_Reader, version, size, o_Data.m_Fragments, o_Data.m_AOData);
		}
		else if ( name == mnmBaseDataParser::GetChunkName())
		{
			mnmBaseData bdata;
			mnmBaseDataParser::ReadData(i_Reader, 
							 propDataParser::GetChunkName(), 
							 version, size, bdata);
			o_Data.m_BaseData.m_Name			= bdata.m_Name;
			o_Data.m_BaseData.m_Filename		= bdata.m_Filename;
			o_Data.m_BaseData.m_bEditorVisible	= bdata.m_bEditorVisible;
			o_Data.m_BaseData.m_Position		= bdata.m_Position;
			o_Data.m_BaseData.m_Orientation		= bdata.m_Orientation;
			//o_Data.m_BaseData.m_Scale			= bdata.m_Position;
		}
		else if ( name == tmlnBaseDataParser::GetChunkName())
		{
			tmlnBaseData tmln_data;
			tmlnBaseDataParser::ReadData(i_Reader, 
							 propDataParser::GetChunkName(), 
							 version, size, tmln_data);

			envSTLHelpers::DeleteContainer(o_Data.m_Drivers);

			int num_drivers = tmln_data.m_Drivers.size();
			for (int i=0; i<num_drivers; i++)
			{
				o_Data.m_Drivers.push_back(tmln_data.m_Drivers[i]->Clone());
			}
		}
		else if ( name == c_PROPD )
		{
			if ( version >= 4 )
			{
				o_Data.m_BaseData.m_Name.Read(i_Reader);
				o_Data.m_BaseData.m_Filename.Read(i_Reader);
				o_Data.m_BaseData.m_bEditorVisible.Read(i_Reader);
				o_Data.m_BaseData.m_Position.Read(i_Reader);
				o_Data.m_BaseData.m_Orientation.Read(i_Reader);
				// scale removed in version 6
				//o_Data.m_BaseData.m_Scale.Read(i_Reader);
				if (version >= 6)
				{
					// visible added in version 6
					o_Data.m_BaseData.m_bVisible.Read(i_Reader);
				}
				if (version >= 7)
				{
					// uniform scale (single value) added in version 7
					o_Data.m_BaseData.m_Scale.Read(i_Reader);
				}
				if (version >= 8)
				{
					// pivot info added in version 8
					o_Data.m_BaseData.m_PivotPoint.Read(i_Reader);
					o_Data.m_BaseData.m_PivotCompensation.Read(i_Reader);
				}
				if (version >= 9)
				{
					o_Data.m_BaseData.m_bLockedMaterials.Read(i_Reader);
				}

				// HACK: [rjk] oh this hurts.  because some files wrote out invalid IDs
				//	this hack will find an ID "up higher" so they don't get assigned an
				//	ID of something that hasn't been loaded yet (but has that ID).
				//	This is temporary until the "old" scenes are re-saved. remove nameMgr.hpp also
				//DBG_LOG2( "Reading prop %d %s", uid, name.c_str() );
				if ( version < 5 )
				{
					if ( o_Data.m_BaseData.m_Name.GetUID() == nameString::e_InvalidUID )
					{
						o_Data.m_BaseData.m_Name.RegisterName();
						if (o_Data.m_BaseData.m_Name.GetUID() < 150)		// if it generated a low UID, force it higher.  all others will be above this one
						{
							o_Data.m_BaseData.m_Name.SetUID(150);
							o_Data.m_BaseData.m_Name.RegisterName();
						}
					}
				}
			}
			else
			{
				if ( version < 3 )
				{
					if ( version >= 1 )
					{
						std::string name;
						chChunkParserUtil::Read(i_Reader, name);
						o_Data.m_BaseData.m_Name.SetValue( name );

						if (version >= 2)
						{
							nameUID uid;
							chChunkParserUtil::Read(i_Reader, uid);
							o_Data.m_BaseData.m_Name.SetUID( uid );

							//DBG_LOG2( "Reading %02d (%s)", uid, name.c_str() );
						}
					}

					o_Data.m_BaseData.m_Filename.Read(i_Reader);
					o_Data.m_BaseData.m_Position.Read(i_Reader);
					o_Data.m_BaseData.m_Orientation.Read(i_Reader);
				}
			}
		}
		else if ( name == c_DRVS )
		{
			tmlnParser::ReadDrivers(i_Reader, 
									propDataParser::GetChunkName(), 
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
//   WritePropChunk
//------------------------------------------------------------------------
void propDataParser::WritePropChunk(chWriter& o_Writer,
									const propScriptData& i_Data )
{
	const int l_PROPCHUNK_VERSION = 3;
	o_Writer.WriteChunkHeader( c_PROPC, l_PROPCHUNK_VERSION, false );

	const int l_PROPD_VERSION = 9;
	o_Writer.WriteChunkHeader( c_PROPD, l_PROPD_VERSION, false );
	i_Data.m_BaseData.m_Name.Write(o_Writer);
	i_Data.m_BaseData.m_Filename.Write(o_Writer);
	i_Data.m_BaseData.m_bEditorVisible.Write(o_Writer);
	i_Data.m_BaseData.m_Position.Write(o_Writer);
	i_Data.m_BaseData.m_Orientation.Write(o_Writer);
	// version 6 removes scale, adds visible
	//i_Data.m_BaseData.m_Scale.Write(o_Writer);
	i_Data.m_BaseData.m_bVisible.Write(o_Writer);
	// version 7 adds uniform scale (single value)
	i_Data.m_BaseData.m_Scale.Write(o_Writer);
	// version 8 adds pivot point info
	i_Data.m_BaseData.m_PivotPoint.Write(o_Writer);
	i_Data.m_BaseData.m_PivotCompensation.Write(o_Writer);
	//	version 9 adds locked materials flag
	i_Data.m_BaseData.m_bLockedMaterials.Write(o_Writer);

	o_Writer.FinishChunk();

	const int l_DRVS_VERSION = 4;
	const bool c_bDRVS_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_DRVS, l_DRVS_VERSION, c_bDRVS_CONTAINER_CHUNK );
	tmlnParser::WriteDrivers(o_Writer, 
							 propDataParser::GetChunkName(), 
							 i_Data.m_Drivers);
	o_Writer.FinishChunk();

	// Write controls' info
	const int l_PROPCONTROLS_VERSION = 0;
	o_Writer.WriteChunkHeader( c_CTRS, l_PROPCONTROLS_VERSION, true );
	dynControlDataParser::WriteControlData(o_Writer, i_Data.m_Controls);
	o_Writer.FinishChunk();

	// Write materials' info
	const int l_PROPMATERIALS_VERSION = 0;
	o_Writer.WriteChunkHeader( c_MTLS, l_PROPMATERIALS_VERSION, true );
	mtrlMaterialParser::WriteMaterialData(o_Writer, i_Data.m_Materials);
	o_Writer.FinishChunk();

	// Write channels info
	const int l_CHNI_VERSION = 0;
	o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), l_CHNI_VERSION, false );
	tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_ChannelInfo);
	o_Writer.FinishChunk();

	// Write fragments' info
	const int l_PROPFRAGMENTS_VERSION = 0;
	o_Writer.WriteChunkHeader( c_FRGS, l_PROPFRAGMENTS_VERSION, true );
	fgmtFragmentParser::WriteFragmentData(o_Writer, i_Data.m_Fragments, i_Data.m_AOData);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void propDataParser::ReadData(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								propPropsData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_PROPC )
		{
			propScriptData data;
			ReadPropChunk(i_Reader, version, size, data);
			o_Data.m_Items.push_back(data);
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void propDataParser::WriteData(	chWriter& o_Writer,
								const propPropsData& i_Data )
{
	//bga - Props no longer saved, show warning dialog.
	if (!i_Data.m_Items.empty())
	{
		guiMessageBox::Show("Props are no longer supported in MAB scene files, please switch them to \"Objects\".", "Props not saved.");
	}

	//const int l_PROPLIST_VERSION = 1;
	//o_Writer.WriteChunkHeader( c_PROPS, l_PROPLIST_VERSION, true );

	//int num_props = i_Data.m_Items.size();
	//for (int i=0; i < num_props ; i++)
	//{
	//	WritePropChunk(o_Writer, i_Data.m_Items[i]);
	//}

	//o_Writer.FinishChunk();
}
