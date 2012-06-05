/********************************************************************************************\
**  chtrDataParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Character/Data/chtrDataParser.hpp"

#include "Support/dyn/dynControlDataParser.hpp"
#include "Support/fgmt/fgmtFragmentParser.hpp"
#include "Support/mnm/mnmBaseDataParser.hpp"
#include "Support/mtrl/mtrlMaterialParser.hpp"
#include "Support/tmln/tmlnBaseDataParser.hpp"
#include "Support/tmln/tmlnChannelInfoParser.hpp"
#include "Support/tmln/tmlnParser.hpp"
#include "Support/xtra/xtraPropertyDataParser.hpp"
#include "Systems/Character/Expressions/chtrExpressionDataParser.hpp"
#include "Systems/Character/GUI/chtrGeomList.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/it/itStringUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_CHRS = chDefs::MakeName('C', 'H', 'R', 'S');	// character list
	const chDefs::Name c_CHRC = chDefs::MakeName('C', 'H', 'R', 'C');	// character chunk
	const chDefs::Name c_CHRD = chDefs::MakeName('C', 'H', 'R', 'D');	//	character data (part of chunk)
	const chDefs::Name c_CDRVS = chDefs::MakeName('D', 'R', 'V', 'S');	//	character drivers (part of chunk)
	const chDefs::Name c_CTLS = chDefs::MakeName('C', 'T', 'L', 'S');	//	character controls (part of chunk)
	const chDefs::Name c_MTLS = chDefs::MakeName('M', 'T', 'L', 'S');	//	character materials (part of chunk)
	const chDefs::Name c_FRGS = chDefs::MakeName('F', 'R', 'G', 'S');	//	character fragment flags (part of chunk)
	const chDefs::Name c_XTRP = chDefs::MakeName('X', 'T', 'R', 'P');	//	character extra properties (part of chunk)


	//------------------------------------------------------------------------
	// Used to support old file formats of channel info. This list
	// does not need to be updates as channels are added, it just
	// represents the order of the channels when the file format
	// for channel info was changed from indexing to name mapping.
	//------------------------------------------------------------------------
	const char* c_ChannelNameStrings[] = 
	{ "Anim-Full", "Position", "Orientation", "Visible", "Sound", "SubAnim", "Scale" };
	const int c_NumChannelStrings = 7;

	//--------------------------------------------------------------------
	// Check absolute path and resolve single filenames into fullpaths
	//--------------------------------------------------------------------
	void resolve_fullpath(prtyFilePath &io_FilePath)
	{
		fsLocator geom_loc = io_FilePath.GetValue();
		if (geom_loc.GetNumNames() > 0)
		{
			// This comparison is only needed to support old file formats
			// and could be removed in product
			if (geom_loc.GetNumNames() == 1)
			{
				//	get the file path
				//
				fsysFileList file_list;
				chtrGeomList::BuildFileList(file_list);
				itString tex_fname = geom_loc.GetLastName();
				if (file_list.FindFilePath(tex_fname, geom_loc))
					geom_loc.Push(tex_fname);
			}

			if (fsAbsolutePathMgr::ResolvePath(geom_loc, "Geometry"))
				io_FilePath.SetValue(geom_loc);
		}
	}

}	// end of anonymous namespace


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  chtrDataParser::GetChunkName()
{
	return c_CHRS;
}

//------------------------------------------------------------------------
//   ReadCharacterChunk
//------------------------------------------------------------------------
void chtrDataParser::ReadCharacterChunk(chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										chtrScriptData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		// Info divided into base chunk and driver list
		if ( name == mnmBaseDataParser::GetChunkName() )
		{
			mnmBaseData bdata;
			mnmBaseDataParser::ReadData(i_Reader, 
										chtrDataParser::GetChunkName(),
										version, size, bdata);
			o_Data.m_BaseData.m_Name			= bdata.m_Name;
			o_Data.m_BaseData.m_Filename		= bdata.m_Filename;
			resolve_fullpath(o_Data.m_BaseData.m_Filename);
			o_Data.m_BaseData.m_bEditorVisible	= bdata.m_bEditorVisible;
			o_Data.m_BaseData.m_Position		= bdata.m_Position;
			o_Data.m_BaseData.m_Orientation		= bdata.m_Orientation;
			//o_Data.m_BaseData.m_Scale			= bdata.m_Position;

			//DBG_TRACE("Char Name = " << (bdata.m_Name.GetString()));
			//DBG_TRACE(" filename = " << (itStringUtil::GetStdString( bdata.m_Filename )));
		}
		else if ( name == tmlnBaseDataParser::GetChunkName() )
		{
			//DBG_TRACE("chtr Base Data");
			tmlnBaseData tmln_data;
			tmlnBaseDataParser::ReadData(i_Reader, 
				chtrDataParser::GetChunkName(),
				version, size, tmln_data);

			envSTLHelpers::DeleteContainer(o_Data.m_Drivers);

			int num_drivers = tmln_data.m_Drivers.size();
			for (int i=0; i<num_drivers; i++)
			{
				o_Data.m_Drivers.push_back(tmln_data.m_Drivers[i]->Clone());
			}
		}
		else if ( name == c_CTLS )
		{
			//DBG_TRACE("chtr dynControls");
			dynControlDataParser::ReadControlData(i_Reader, version, size, o_Data.m_Controls);
		}
		else if ( name == c_MTLS )
		{
			//DBG_TRACE("chtr Materials");
			mtrlMaterialParser::ReadMaterialData(i_Reader, version, size, o_Data.m_Materials);
		}
		else if ( name == c_FRGS )
		{
			//DBG_TRACE("chtr Frags");
			fgmtFragmentParser::ReadFragmentData(i_Reader, version, size, o_Data.m_Fragments, o_Data.m_AOData);
		}
		else if ( name == c_CHRD )
		{
			//DBG_TRACE("chtr CHR Data");
			if ( version < 4 )
			{
				std::string name;
				chChunkParserUtil::Read(i_Reader, name);
				o_Data.m_BaseData.m_Name.SetValue( name );
				
				nameUID uid;
				chChunkParserUtil::Read(i_Reader, uid);
				o_Data.m_BaseData.m_Name.SetUID( uid );
				
				o_Data.m_BaseData.m_Filename.Read(i_Reader);
				resolve_fullpath(o_Data.m_BaseData.m_Filename);
				o_Data.m_BaseData.m_Position.Read(i_Reader);
				o_Data.m_BaseData.m_Orientation.Read(i_Reader);
			}
			else
			{
				o_Data.m_BaseData.m_Name.Read(i_Reader);
				o_Data.m_BaseData.m_Filename.Read(i_Reader);
				resolve_fullpath(o_Data.m_BaseData.m_Filename);
				o_Data.m_BaseData.m_bEditorVisible.Read(i_Reader);
				o_Data.m_BaseData.m_Position.Read(i_Reader);
				o_Data.m_BaseData.m_Orientation.Read(i_Reader);
				// removed in version 6:
				//o_Data.m_BaseData.m_Scale.Read(i_Reader);
				if (version >= 6)
				{
					o_Data.m_BaseData.m_bVisible.Read(i_Reader);
				}
				if (version >= 7)
				{
					// uniform scale (single value) and pivot info added in version 7
					o_Data.m_BaseData.m_Scale.Read(i_Reader);
					o_Data.m_BaseData.m_PivotPoint.Read(i_Reader);
					o_Data.m_BaseData.m_PivotCompensation.Read(i_Reader);
				}
				if (version >= 8)
				{
					o_Data.m_BaseData.m_bLockedMaterials.Read(i_Reader);
				}

				// HACK: [rjk] oh this hurts.  because some files wrote out invalid IDs
				//	this hack will find an ID "up higher" so they don't get assigned an
				//	ID of something that hasn't been loaded yet (but has that ID).
				//	This is temporary until the "old" scenes are re-saved. remove nameMgr.hpp also
				//DBG_LOG2( "Reading chtr %d %s", uid, name.c_str() );
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
		}
		else if ( name == c_XTRP )
		{
			// Read custom property info
			xtraPropertyDataParser::ReadCustomPropertyData(i_Reader, version, size, o_Data.m_CustomProperties);
		}
		else if ( name == c_CDRVS )
		{
			//DBG_TRACE("chtr Drivers");
			tmlnParser::ReadDrivers(i_Reader, 
									chtrDataParser::GetChunkName(),  
									(o_Data.m_Drivers) );
		}
		else if ( name == tmlnChannelInfoParser::GetChunkName() )
		{
			//DBG_TRACE("chtr Channel Info");
			tmlnChannelInfoParser::ReadChannels(i_Reader, 
												o_Data.m_ChannelInfo, 
												c_ChannelNameStrings, 
												c_NumChannelStrings);
		}
		else if ( name == chtrExpressionDataParser::GetChunkName() )
		{
			//DBG_TRACE("chtr Expressions");
			chtrExpressionDataParser::ReadData( i_Reader, version, size, o_Data.m_Expressions );
		}

		i_Reader.FinishChunk();
		//DBG_TRACE("  - finished");
	}
}

//------------------------------------------------------------------------
//   WriteCharacterChunk
//------------------------------------------------------------------------
void chtrDataParser::WriteCharacterChunk(chWriter& o_Writer,
										 const chtrScriptData& i_Data )
{
	const int c_CHRCHUNK_VERSION = 5;
	const bool c_bCHRC_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_CHRC, c_CHRCHUNK_VERSION, c_bCHRC_CONTAINER_CHUNK );

	const int l_CHRD_VERSION = 8;
	o_Writer.WriteChunkHeader( c_CHRD, l_CHRD_VERSION, false );
	i_Data.m_BaseData.m_Name.Write(o_Writer);
	i_Data.m_BaseData.m_Filename.Write(o_Writer);
	i_Data.m_BaseData.m_bEditorVisible.Write(o_Writer);
	i_Data.m_BaseData.m_Position.Write(o_Writer);
	i_Data.m_BaseData.m_Orientation.Write(o_Writer);
	//i_Data.m_BaseData.m_Scale.Write(o_Writer);
	i_Data.m_BaseData.m_bVisible.Write(o_Writer);
	// version 7 adds uniform scale (single value) and pivot info
	i_Data.m_BaseData.m_Scale.Write(o_Writer);
	i_Data.m_BaseData.m_PivotPoint.Write(o_Writer);
	i_Data.m_BaseData.m_PivotCompensation.Write(o_Writer);

	//	version 8 adds locked materials flag
	i_Data.m_BaseData.m_bLockedMaterials.Write(o_Writer);

	o_Writer.FinishChunk();

	// Write custom property info
	const int l_XTRP_VERSION = 0;
	const bool c_bXTRP_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_XTRP, l_XTRP_VERSION, c_bXTRP_CONTAINER_CHUNK );
	xtraPropertyDataParser::WriteCustomPropertyData(o_Writer, i_Data.m_CustomProperties);
	o_Writer.FinishChunk();

	const int l_CDRVS_VERSION = 4;
	const bool c_bDRVS_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_CDRVS, l_CDRVS_VERSION, c_bDRVS_CONTAINER_CHUNK );
	tmlnParser::WriteDrivers(o_Writer, 
							chtrDataParser::GetChunkName(),  
							i_Data.m_Drivers);
	o_Writer.FinishChunk();

	// Write controls' info
	const int c_CHRCONTROLS_VERSION	= 0;
	o_Writer.WriteChunkHeader( c_CTLS, c_CHRCONTROLS_VERSION, true );
	dynControlDataParser::WriteControlData(o_Writer, i_Data.m_Controls);
	o_Writer.FinishChunk();

	// Write materials' info
	const int l_CHRMATERIALS_VERSION = 0;
	o_Writer.WriteChunkHeader( c_MTLS, l_CHRMATERIALS_VERSION, true );
	mtrlMaterialParser::WriteMaterialData(o_Writer, i_Data.m_Materials);
	o_Writer.FinishChunk();

	// Write channels info
	const int l_CHNI_VERSION = 0;
	o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), l_CHNI_VERSION, true );
	tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_ChannelInfo);
	o_Writer.FinishChunk();

	// Write expressions info
	chtrExpressionDataParser::WriteData( o_Writer, i_Data.m_Expressions );

	// Write fragments' info
	const int l_CHRFRAGMENTS_VERSION = 0;
	o_Writer.WriteChunkHeader( c_FRGS, l_CHRFRAGMENTS_VERSION, true );
	fgmtFragmentParser::WriteFragmentData(o_Writer, i_Data.m_Fragments, i_Data.m_AOData);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void chtrDataParser::ReadData(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								chtrCharactersData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_CHRC )
		{
			chtrScriptData data;
			ReadCharacterChunk(i_Reader, version, size, data);
			o_Data.m_Items.push_back(data);
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void chtrDataParser::WriteData(	chWriter& o_Writer,
								const chtrCharactersData& i_Data )
{
	const int c_CHRLIST_VERSION	= 1;
	o_Writer.WriteChunkHeader( c_CHRS, c_CHRLIST_VERSION, true );

	int num_characters = i_Data.m_Items.size();
	for (int i=0; i < num_characters ; i++)
	{
		WriteCharacterChunk(o_Writer, i_Data.m_Items[i]);
	}

	o_Writer.FinishChunk();
}

