/*****************************************************************************
**	chnlTimeDocumentChunk.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005-7 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/Data/chnlTimeDocumentChunk.hpp"

#include "Features/Channels/Data/chnlTimeDataParser.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#include "Features/Channels/Notes/chnlNotesMgr.hpp"

#include "Support/mnm/mnmPaths.hpp"
#include "Features/ProjectSetup/Data/ProjectSetupData.hpp"
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include <assert.h>
#include <string>


//============================================================================
// static variable
//============================================================================
chnlTimeDocumentChunk* chnlTimeDocumentChunk::sm_pActiveChunk = NULL;


//--------------------------------------------------------------------
// Call DataChanged function on the active document chunk
//--------------------------------------------------------------------
//static 
void chnlTimeDocumentChunk::ActiveDataChanged()
{
	if (sm_pActiveChunk != NULL)
		sm_pActiveChunk->DataChanged();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlTimeDocumentChunk::chnlTimeDocumentChunk() 
:	m_bDirty(false), 
	m_bActive(false)
{
}

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void  chnlTimeDocumentChunk::Clear()
{
	m_bDirty = false;

	if (this->m_bActive)
	{
		// Clear out markers and notes
		m_Data = chnlTimeData();
		chnlMarkerMgr::SetData(m_Data.m_Markers);
		chnlNotesMgr::SetData(m_Data.m_Notes);
		chnlDialogUtil::UpdateMarkersAndNotes();
	}
}

//--------------------------------------------------------------------
//  returns chunk name for this data type
//--------------------------------------------------------------------
chDefs::Name  chnlTimeDocumentChunk::GetChunkName() const
{
	return chnlTimeDataParser::GetChunkName();
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual
const char* chnlTimeDocumentChunk::GetChunkDesc() const
{
	return "Time";
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
//virtual
void chnlTimeDocumentChunk::BuildDataList( std::vector<std::string>& o_List )
{
	o_List.clear();
	std::string item;

	item = "Markers";
	o_List.push_back( item );

	item = "Notes";
	o_List.push_back( item );
}

//--------------------------------------------------------------------
//	remove the items in this chunk by name.  each chunk built the
//	list using BuildDataList() so each chunk will know what to do
//	with this data.
//--------------------------------------------------------------------
//virtual
void chnlTimeDocumentChunk::RemoveItems( const std::vector<std::string>& i_List )
{
	// if active, remove items from the manager, otherwise from the data list.
	//
	if ( this->m_bActive )
	{
		for (int i = 0; i < i_List.size(); ++i)
		{
			//grpsGroupMgr::DeleteGroup(i_List[i]);
		}
	}
	else
	{
		for (int j = 0; j < i_List.size(); ++j)
		{
			//for (int w = 0 ; w < m_Data.m_Groups.size(); ++w )
			//{
			//	//  find the match
			//	//
			//	if ( m_Data.m_Groups[w].m_Name.GetString() == i_List[j] )
			//	{
			//		//	erase the item in the list
			//		//
			//		m_Data.m_Groups.erase( m_Data.m_Groups.begin() + w);
			//		break;
			//	}
			//}
		}
	}
}

//--------------------------------------------------------------------
//  Import chunk data and add it to existing data
//--------------------------------------------------------------------
void  chnlTimeDocumentChunk::Import(chReader &i_Reader,
							  chDefs::Version i_Version,
							  chDefs::Size i_Size)
{
	//	If this is the active chunk and there is already data,
	//	then do a merge of the new data into the manager
	// 
	if (this->m_bActive)
	{
	//	grpsGroupsData new_data;
	//	grupGroupsDataParser::ReadData(i_Reader, i_Version, i_Size, new_data);
	//	grpsGroupMgr::MergeData(new_data);

	//	m_Data = grpsGroupMgr::GetData();
		this->Read(i_Reader, i_Version, i_Size);
	}
	else
	{
	//	// append data to our m_Data field
	//	grupGroupsDataParser::ReadData(i_Reader, i_Version, i_Size, m_Data);
	
		this->Read(i_Reader, i_Version, i_Size);
	}

	//	if importing data, then this chunk is dirty
	//this->DataChanged();
}

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void  chnlTimeDocumentChunk::Read(	chReader &i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size )
{
	chnlTimeDataParser::ReadData( i_Reader, i_Version, i_Size, m_Data );

	if (this->m_bActive)
	{
		chnlMarkerMgr::SetData(m_Data.m_Markers);
		chnlNotesMgr::SetData(m_Data.m_Notes);
		chnlDialogUtil::UpdateMarkersAndNotes();
	}

	m_bDirty = false;
}

//--------------------------------------------------------------------
//  Write chunk data
//--------------------------------------------------------------------
void  chnlTimeDocumentChunk::Write( chWriter &i_Writer, bool i_bResetDirtyFlag )
{
	if (this->m_bActive)
	{
		chnlMarkerMgr::GetData(m_Data.m_Markers);
		chnlNotesMgr::GetData(m_Data.m_Notes);
		chnlTimeDataParser::WriteData( i_Writer, m_Data );
	}
	else
	{
		chnlTimeDataParser::WriteData( i_Writer, m_Data );
	}

	if ( i_bResetDirtyFlag )
		m_bDirty = false;
}

//--------------------------------------------------------------------
//  Return true if the chunk has been modified since the last
// call to Write()
//--------------------------------------------------------------------
bool  chnlTimeDocumentChunk::IsDirty() const
{
	return m_bDirty;
}

//--------------------------------------------------------------------
//  This chunk is currently made active, add data to the scene
// or world
//--------------------------------------------------------------------
void  chnlTimeDocumentChunk::SetActive()
{
	m_bActive = true;
	sm_pActiveChunk = this;
}

//--------------------------------------------------------------------
//  this chunk is currently being made in active, remove data
// from scene or world
//--------------------------------------------------------------------
void  chnlTimeDocumentChunk::SetInactive( bool i_bUpdateData )
{
	m_bActive = false;
	if (sm_pActiveChunk == this) sm_pActiveChunk = NULL;
}

//--------------------------------------------------------------------
// Callback when dialog changes data, sets dirty bit
//--------------------------------------------------------------------
void chnlTimeDocumentChunk::DataChanged()
{
	m_bDirty = true;
}


