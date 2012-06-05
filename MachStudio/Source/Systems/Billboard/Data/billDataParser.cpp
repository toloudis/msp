/********************************************************************************************\
**	billDataParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Billboard/Data/billDataParser.hpp"
#include "Systems/Billboard/GUI/billGeomList.hpp"

#include "Support/mnm/mnmBaseDataParser.hpp"
#include "Support/tmln/tmlnBaseDataParser.hpp"
#include "Support/tmln/tmlnChannelInfoParser.hpp"
#include "Support/tmln/tmlnParser.hpp"
#include "Support/xtra/xtraPropertyDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
//#include "Core/name/nameMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_BILL = chDefs::MakeName('B', 'I', 'L', 'L');	// bill list
	const chDefs::Name c_BILC = chDefs::MakeName('B', 'I', 'L', 'C');	// +-bill chunk
	const chDefs::Name c_BILD = chDefs::MakeName('B', 'I', 'L', 'D');	//	 +-bill data (part of chunk)
	const chDefs::Name c_DRVS = chDefs::MakeName('D', 'R', 'V', 'S');	//	 +-bill drivers (part of chunk)
	const chDefs::Name c_XTRP = chDefs::MakeName('X', 'T', 'R', 'P');


	//------------------------------------------------------------------------
	// Used to support old file formats of channel info. This list
	// does not need to be updates as channels are added, it just
	// represents the order of the channels when the file format
	// for channel info was changed from indexing to name mapping.
	//------------------------------------------------------------------------
	const char* c_ChannelNameStrings[] = 
		{ "Position", "Texture", "Orientation", "Scale" };
	const int c_NumChannelStrings = 4;

	//--------------------------------------------------------------------
	// Check absolute path and resolve single filenames into fullpaths
	//--------------------------------------------------------------------
	void resolve_fullpath(prtyFilePath &io_FilePath)
	{
		fsLocator tex_loc = io_FilePath.GetValue();
		if (tex_loc.GetNumNames() > 0)
		{
			// This comparison is only needed to support old file formats
			// and could be removed in product
			if (tex_loc.GetNumNames() == 1)
			{
				itString filename = tex_loc.GetLastName();
				billGeomList::FindFile(filename, tex_loc);
			}

			if (fsAbsolutePathMgr::ResolvePath(tex_loc, "Textures"))
				io_FilePath.SetValue(tex_loc);
		}
	}

}


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  billDataParser::GetChunkName()
{
	return c_BILL;
}

//------------------------------------------------------------------------
//   ReadBillboardChunk
//------------------------------------------------------------------------
void billDataParser::ReadBillboardChunk(chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										billScriptData& o_Data )
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
							billDataParser::GetChunkName(), 
							version, size, data);

			o_Data.m_BaseData.m_bEditorVisible	= data.m_bEditorVisible;
			o_Data.m_BaseData.m_Filename		= data.m_Filename;
			resolve_fullpath(o_Data.m_BaseData.m_Filename);
			o_Data.m_BaseData.m_Name			= data.m_Name;
//			o_Data.m_BaseData.m_Orientation		= data.m_Orientation;
			o_Data.m_BaseData.m_Position		= data.m_Position;
		}
		else if ( name == tmlnBaseDataParser::GetChunkName() )
		{
			tmlnBaseData tmln_data;
			tmlnBaseDataParser::ReadData(i_Reader, 
							billDataParser::GetChunkName(), 
							version, size, tmln_data);

			envSTLHelpers::DeleteContainer(o_Data.m_Drivers);

			int num_drivers = tmln_data.m_Drivers.size();
			for (int i=0; i<num_drivers; i++)
			{
				o_Data.m_Drivers.push_back(tmln_data.m_Drivers[i]->Clone());
			}
		}
		else if ( name == c_BILD )
		{
			if ( version < 3 )
			{
				if ( version < 2 )
				{
					if ( version >= 0 )
					{
						std::string name;
						chChunkParserUtil::Read(i_Reader, name);
						o_Data.m_BaseData.m_Name.SetValue( name );

						if (version >= 1)
						{
							nameUID uid;
							chChunkParserUtil::Read(i_Reader, uid);
							o_Data.m_BaseData.m_Name.SetUID( uid );
						}
					}

					o_Data.m_BaseData.m_Filename.Read(i_Reader);
					resolve_fullpath(o_Data.m_BaseData.m_Filename);
					o_Data.m_BaseData.m_Position.Read(i_Reader);
				}
				o_Data.m_BaseData.m_Scale.Read(i_Reader);
			}
			else
			{
				o_Data.m_BaseData.m_Name.Read(i_Reader);
				o_Data.m_BaseData.m_bEditorVisible.Read(i_Reader);
				o_Data.m_BaseData.m_Position.Read(i_Reader);
				o_Data.m_BaseData.m_Orientation.Read(i_Reader);
				o_Data.m_BaseData.m_Scale.Read(i_Reader);
				o_Data.m_BaseData.m_Filename.Read(i_Reader);
				resolve_fullpath(o_Data.m_BaseData.m_Filename);

				if (version >= 4)
				{
					o_Data.m_BaseData.m_bOrientToCamera.Read(i_Reader);
					o_Data.m_BaseData.m_bAdditiveMaterial.Read(i_Reader);

					if (version >= 6)
					{
						o_Data.m_BaseData.m_Color.Read(i_Reader);
					}

					//	misplaced property written
					if (version == 11)
					{
						o_Data.m_BaseData.m_Brightness.Read(i_Reader);
					}

					if (version >= 7)
					{
						o_Data.m_BaseData.m_bVisible.Read(i_Reader);
					}
					if (version >= 8)
					{
						o_Data.m_BaseData.m_bSnapToCamera.Read(i_Reader);
						o_Data.m_BaseData.m_DistToCamera.Read(i_Reader);
						o_Data.m_BaseData.m_bCKActive.Read(i_Reader);
						o_Data.m_BaseData.m_CKColor.Read(i_Reader);
						o_Data.m_BaseData.m_CKTolerance.Read(i_Reader);
						o_Data.m_BaseData.m_bCKRemoveSpill.Read(i_Reader);
						o_Data.m_BaseData.m_CKSpillType.Read(i_Reader);
						o_Data.m_BaseData.m_CKSpillBias.Read(i_Reader);
						o_Data.m_BaseData.m_CKEdgeBlur.Read(i_Reader);
					}
					if (version >= 9)
					{
						o_Data.m_BaseData.m_CameraIndex.Read(i_Reader);
					}
					if (version >= 10)
					{
						o_Data.m_BaseData.m_NonUniformScale.Read(i_Reader);
						o_Data.m_BaseData.m_Video.Read(i_Reader);
					}
					if ((version >= 12) && (version != 11))
					{
						o_Data.m_BaseData.m_Brightness.Read(i_Reader);
					}
				}

				// HACK: [rjk] oh this hurts.  because some files wrote out invalid IDs
				//	this hack will find an ID "up higher" so they don't get assigned an
				//	ID of something that hasn't been loaded yet (but has that ID).
				//	This is temporary until the "old" scenes are re-saved. remove nameMgr.hpp also
				//DBG_LOG2( "Reading billboard %d %s", uid, name.c_str() );
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
		else if ( name == c_DRVS )
		{
			tmlnParser::ReadDrivers(i_Reader, 
									billDataParser::GetChunkName(), 
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
//   WriteBillboardChunk
//------------------------------------------------------------------------
void billDataParser::WriteBillboardChunk(	chWriter& o_Writer,
											const billScriptData& i_Data )
{
	const int l_BILLCHUNK_VERSION = 4;
	o_Writer.WriteChunkHeader( c_BILC, l_BILLCHUNK_VERSION, true );

	//	data part
	const int l_BILLDATA_VERSION = 12;
	o_Writer.WriteChunkHeader( c_BILD, l_BILLDATA_VERSION, false );

	i_Data.m_BaseData.m_Name.Write(o_Writer);
	i_Data.m_BaseData.m_bEditorVisible.Write(o_Writer);
	i_Data.m_BaseData.m_Position.Write(o_Writer);
	i_Data.m_BaseData.m_Orientation.Write(o_Writer);
	i_Data.m_BaseData.m_Scale.Write(o_Writer);
	i_Data.m_BaseData.m_Filename.Write(o_Writer);
	i_Data.m_BaseData.m_bOrientToCamera.Write(o_Writer);
	i_Data.m_BaseData.m_bAdditiveMaterial.Write(o_Writer);
	i_Data.m_BaseData.m_Color.Write(o_Writer);
	i_Data.m_BaseData.m_bVisible.Write(o_Writer);

	// added in version 8
	i_Data.m_BaseData.m_bSnapToCamera.Write(o_Writer);
	i_Data.m_BaseData.m_DistToCamera.Write(o_Writer);
	i_Data.m_BaseData.m_bCKActive.Write(o_Writer);
	i_Data.m_BaseData.m_CKColor.Write(o_Writer);
	i_Data.m_BaseData.m_CKTolerance.Write(o_Writer);
	i_Data.m_BaseData.m_bCKRemoveSpill.Write(o_Writer);
	i_Data.m_BaseData.m_CKSpillType.Write(o_Writer);
	i_Data.m_BaseData.m_CKSpillBias.Write(o_Writer);
	i_Data.m_BaseData.m_CKEdgeBlur.Write(o_Writer);

	// added in version 9
	i_Data.m_BaseData.m_CameraIndex.Write(o_Writer);

	// added in version 10
	i_Data.m_BaseData.m_NonUniformScale.Write(o_Writer);
	i_Data.m_BaseData.m_Video.Write(o_Writer);

	// added in version 11
	i_Data.m_BaseData.m_Brightness.Write(o_Writer);

	o_Writer.FinishChunk();

	// Write custom property info
	const int l_XTRP_VERSION = 0;
	const bool c_bXTRP_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_XTRP, l_XTRP_VERSION, c_bXTRP_CONTAINER_CHUNK );
	xtraPropertyDataParser::WriteCustomPropertyData(o_Writer, i_Data.m_CustomProperties);
	o_Writer.FinishChunk();

	// Write drivers info
	const int l_DRVS_VERSION = 1;
	const bool c_bDRVS_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_DRVS, l_DRVS_VERSION, c_bDRVS_CONTAINER_CHUNK );
	tmlnParser::WriteDrivers(o_Writer, 
							billDataParser::GetChunkName(), 
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
void billDataParser::ReadData(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								billListData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_BILC )
		{
			billScriptData data;
			ReadBillboardChunk(i_Reader, version, size, data);
			o_Data.m_Items.push_back(data);
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void billDataParser::WriteData(	chWriter& o_Writer,
								const billListData& i_Data )
{
	const int l_BILLLIST_VERSION = 0;
	o_Writer.WriteChunkHeader( c_BILL, l_BILLLIST_VERSION, true );

	int num_bills = i_Data.m_Items.size();
	for (int i=0; i < num_bills ; i++)
	{
		WriteBillboardChunk(o_Writer, i_Data.m_Items[i]);
	}

	o_Writer.FinishChunk();
}
