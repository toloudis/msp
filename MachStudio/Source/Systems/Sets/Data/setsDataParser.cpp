/********************************************************************************************\
**  setsDataParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Sets/Data/setsDataParser.hpp"

#include "Support/fgmt/fgmtFragmentParser.hpp"
#include "Support/mnm/mnmBaseDataParser.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mtrl/mtrlMaterialParser.hpp"
#include "Support/tmln/tmlnBaseDataParser.hpp"
#include "Support/tmln/tmlnParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/name/nameMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"


//============================================================================
//============================================================================
namespace setsDataParser
{
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_SETS = chDefs::MakeName('S', 'E', 'T', 'S');	// list of set data
	const chDefs::Name c_SETD = chDefs::MakeName('S', 'E', 'T', 'D');	// set item data (now part of chunk)

	const chDefs::Name c_SETC = chDefs::MakeName('S', 'E', 'T', 'C');	// set item chunk
	const chDefs::Name c_DRVS = chDefs::MakeName('D', 'R', 'V', 'S');	//	set item drivers (part of chunk)
	const chDefs::Name c_MTLS = chDefs::MakeName('M', 'T', 'L', 'S');	//	set item materials (part of chunk)
	const chDefs::Name c_FRGS = chDefs::MakeName('F', 'R', 'G', 'S');	//	set item fragment flags (part of chunk)

}


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_SETS;
}


//------------------------------------------------------------------------
//   ReadOldSetItem - old version, subchunks written incorrectly.
//	Kept around to handle old file formats.
//------------------------------------------------------------------------
void ReadOldSetItem(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					setsScriptData& o_Data )
{
	if ( i_Version >= 3 )
	{
		o_Data.m_BaseData.m_Name.Read(i_Reader);
		//DBG_LOG3( "Read set %d (%s)  [chunk version %d]", uid, name.c_str(), i_Version );

		o_Data.m_BaseData.m_Filename.Read(i_Reader);
		o_Data.m_BaseData.m_bEditorVisible.Read(i_Reader);

		// HACK: [rjk] invalidate set IDs from this chunk version.  The system will generate new "higher" chunks
		//	and the lightsets will automatically find the new set items based on the name.
		if ( i_Version == 5 )
		{
			o_Data.m_BaseData.m_Name.SetUID( nameString::e_InvalidUID );
		}
		// HACK: [rjk] oh this hurts.  because some files wrote out invalid IDs
		//	this hack will find an ID "up higher" so they don't get assigned an
		//	ID of something that hasn't been loaded yet (but has that ID).
		//	This is temporary until the "old" scenes are re-saved. remove nameMgr.hpp also
		if ( i_Version < 6 )
		{
			if ( o_Data.m_BaseData.m_Name.GetUID() == nameString::e_InvalidUID )
			{
                o_Data.m_BaseData.m_Name.SetValue(o_Data.m_BaseData.m_Filename.GetString());
				o_Data.m_BaseData.m_Name.RegisterName();
				if (o_Data.m_BaseData.m_Name.GetUID() < 350)		// if it generated a low UID, force it higher.  all others will be above this one
				{
					o_Data.m_BaseData.m_Name.SetUID(350);
					o_Data.m_BaseData.m_Name.RegisterName();
				}
			}
		}
		return;
	}

	//
	//	older versions
	//
	if ( i_Version == 0 )
	{
		o_Data.m_BaseData.m_Filename.Read(i_Reader);
		return;
	}

	//
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
							    setsDataParser::GetChunkName(), 
								version, size, data);

			o_Data.m_BaseData.m_bEditorVisible.SetValue(data.m_bEditorVisible);
			o_Data.m_BaseData.m_Filename.SetValue(data.m_Filename);
			o_Data.m_BaseData.m_Name.SetValue(data.m_Name);
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   ReadSetItem
//------------------------------------------------------------------------
void ReadSetItem(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					setsScriptData& o_Data )
{
	//
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_SETD )
		{
			o_Data.m_BaseData.m_Name.Read(i_Reader);
			o_Data.m_BaseData.m_Filename.Read(i_Reader);
			o_Data.m_BaseData.m_bEditorVisible.Read(i_Reader);
		}
		else if ( name == c_MTLS )
		{
			mtrlMaterialParser::ReadMaterialData(i_Reader, version, size, o_Data.m_Materials);
		}
		else if ( name == c_FRGS )
		{
			fgmtFragmentParser::ReadFragmentData(i_Reader, version, size, o_Data.m_Fragments, o_Data.m_AOData);
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteSetItem
//------------------------------------------------------------------------
void WriteSetItem(	chWriter& o_Writer,
					const setsScriptData& i_Data )
{
	const int l_SETCHUNK_VERSION = 0;
	o_Writer.WriteChunkHeader( c_SETC, l_SETCHUNK_VERSION, true );

	// Basic data subchunk
	const int l_SETSITEM_VERSION = 7;
	o_Writer.WriteChunkHeader( c_SETD, l_SETSITEM_VERSION, false );

	if (i_Data.m_BaseData.m_Name.GetUID() == nameString::e_InvalidUID)
		DBG_ERROR0( "Writing an invalid ID");

	i_Data.m_BaseData.m_Name.Write(o_Writer);
	i_Data.m_BaseData.m_Filename.Write(o_Writer);
	i_Data.m_BaseData.m_bEditorVisible.Write(o_Writer);
	o_Writer.FinishChunk();

	// Write materials' info
	const int l_SETMATERIALS_VERSION = 0;
	o_Writer.WriteChunkHeader( c_MTLS, l_SETMATERIALS_VERSION, true );
	mtrlMaterialParser::WriteMaterialData(o_Writer, i_Data.m_Materials);
	o_Writer.FinishChunk();

	// Write fragments' info
	const int l_SETFRAGMENTS_VERSION = 0;
	o_Writer.WriteChunkHeader( c_FRGS, l_SETFRAGMENTS_VERSION, true );
	fgmtFragmentParser::WriteFragmentData(o_Writer, i_Data.m_Fragments, i_Data.m_AOData);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				setsListData& o_Data )
{

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_SETC )
		{
			setsScriptData data;
			ReadSetItem(i_Reader, version, size, data);
			o_Data.m_SetItems.push_back(data);
		}
		else if ( name == c_SETD )
		{
			// Old format read item data as single chunk without
			// subchunks. (Or it tried to do subchunks incorrectly.)
			setsScriptData data;
			ReadOldSetItem(i_Reader, version, size, data);
			o_Data.m_SetItems.push_back(data);
		}
		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void WriteData(	chWriter& o_Writer,
				const setsListData& i_Data )
{	
	//bga - Sets no longer saved, show warning dialog.
	if (!i_Data.m_SetItems.empty())
	{
		guiMessageBox::Show("Sets are no longer supported in MAB scene files, please switch them to \"Objects\".", "Sets not saved.");
	}
	//const int l_SETSLIST_VERSION = 1;
	//o_Writer.WriteChunkHeader( c_SETS, l_SETSLIST_VERSION, true );

	//for (int i=0; i<i_Data.m_SetItems.size(); i++)
	//{
	//	WriteSetItem(o_Writer, i_Data.m_SetItems[i]);
	//}

	//o_Writer.FinishChunk();
}

}	// end of namespace
