/*****************************************************************************
**	mnmTitleCardDocumentChunk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcdc/data/fcdcTimelineItemDocumentChunk.hpp"

#include "FCSupport/fcdc/data/fcdcTimelineItemParser.hpp"
#include "FCSupport/fcdc/fcdcDataMgr.hpp"

#include "Core/Fs/fsFileX.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"


//============================================================================
//============================================================================
namespace
{
	fcdcTimelineItemData l_TempItems;
	const chDefs::Name c_FCDC = chDefs::MakeName('F', 'C', 'D', 'C');	
	const chDefs::Name c_TITL = chDefs::MakeName('T', 'I', 'T', 'L');
	const chDefs::Name c_CTLL = chDefs::MakeName('C', 'T', 'L', 'L');
	const chDefs::Name c_CTLD = chDefs::MakeName('C', 'T', 'L', 'D');


}


//============================================================================
// static variable
//============================================================================
fcdcTimelineItemDocumentChunk* fcdcTimelineItemDocumentChunk::sm_pActiveChunk = NULL;


//--------------------------------------------------------------------
// Return pointer to currently active chunk, may return NULL
//--------------------------------------------------------------------
fcdcTimelineItemDocumentChunk* fcdcTimelineItemDocumentChunk::GetActiveChunk()
{
	return sm_pActiveChunk;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
fcdcTimelineItemDocumentChunk::fcdcTimelineItemDocumentChunk()
:	docDocumentChunk(),
	m_bChunkRead(false)
{
	//m_CommentString = "Comment";
}

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void fcdcTimelineItemDocumentChunk::Clear()
{
	//m_CommentString = "Comment";
}

//--------------------------------------------------------------------
//  returns chunk name for this data type
//--------------------------------------------------------------------
chDefs::Name  fcdcTimelineItemDocumentChunk::GetChunkName() const
{
	return fcdcTimelineItemParser::GetChunkName();
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual
const char* fcdcTimelineItemDocumentChunk::GetChunkDesc() const
{
	return "Fusion Cinema data";
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
//virtual
void fcdcTimelineItemDocumentChunk::BuildDataList( std::vector<std::string>& o_List )
{
	/*o_List.clear();

	std::string item;
	for( int i = 0; i < m_Data.m_TimelineData.m_TimelineItems.size(); i++)
	{
		item = m_Data.m_TimelineData.m_TimelineItems[i].m_Name.GetValue();
		o_List.push_back( item );
	}
	l_TempItems = m_Data;*/
}

//--------------------------------------------------------------------
//	remove the items in this chunk by name.  each chunk built the
//	list using BuildDataList() so each chunk will know what to do
//	with this data.
//--------------------------------------------------------------------
//virtual
void fcdcTimelineItemDocumentChunk::RemoveItems( const std::vector<std::string>& i_List )
{
	// remove items from the manager
	//
	/*std::vector<fcdcTimelineItem>::iterator it;
	for (int i = 0; i < i_List.size(); ++i)
	{
		for(it = m_Data.m_TimelineData.m_TimelineItems.begin(); it != m_Data.m_TimelineData.m_TimelineItems.end(); ++it)
		{		
			if(i_List[i] == (*it).m_Name.GetValue())
			{
				m_Data.m_TimelineData.m_TimelineItems.erase(it);
				break;
			}
		}
	}
	l_TempItems = m_Data;*/
}

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void fcdcTimelineItemDocumentChunk::Read( chReader &io_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size )
{
	//clear the list and read in new data
	m_Data.m_TimelineData.m_TimelineItems.clear();
	m_Data.m_HighlightData.m_HighlightItems.clear();
	m_Data.m_ProjectData.m_Title.SetValue("My Movie");
	fcdcTimelineItemParser::ReadData( io_Reader, i_Version, i_Size, m_Data );
	fcdcDataMgr::SetData(m_Data);
}

//--------------------------------------------------------------------
//  Write chunk data
//--------------------------------------------------------------------
void fcdcTimelineItemDocumentChunk::Write(chWriter &io_Writer, bool i_bResetDirtyFlag)
{
	m_Data = fcdcDataMgr::GetData();
	fcdcTimelineItemParser::WriteData( io_Writer, m_Data );
}

//--------------------------------------------------------------------
//  Return true if the chunk has been modified since the last
// call to Write()
//--------------------------------------------------------------------
bool fcdcTimelineItemDocumentChunk::IsDirty() const
{
	return false;
}

//--------------------------------------------------------------------
//  This chunk is currently made active, add data to the scene
// or world
//--------------------------------------------------------------------
void fcdcTimelineItemDocumentChunk::SetActive()
{
	sm_pActiveChunk = this;
}

//--------------------------------------------------------------------
//  this chunk is currently being made in active, remove data
// from scene or world
//--------------------------------------------------------------------
void fcdcTimelineItemDocumentChunk::SetInactive( bool i_bUpdateData )
{
	if (sm_pActiveChunk == this)
		sm_pActiveChunk = NULL;
}

//--------------------------------------------------------------------
//  This virtual function is called on all chunks after all 
//	chunks have finished loading successfully
//--------------------------------------------------------------------
void fcdcTimelineItemDocumentChunk::NotifyLoadFinished()
{
	if (sm_pActiveChunk == this)
	{
		m_bChunkRead = false;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void fcdcTimelineItemDocumentChunk::SetupPaths()
{
}

//--------------------------------------------------------------------
// Callback when dialog changes data, sets dirty bit
//--------------------------------------------------------------------
void fcdcTimelineItemDocumentChunk::DataChanged()
{
	m_bDirty = true;
}
