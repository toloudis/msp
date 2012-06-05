/*****************************************************************************
**	rstkDocumentChunk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Tool/rstk/rstkDocumentChunk.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Tool/doc/docDocumentWithChunks.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/rstk/rstkDataMgr.hpp"
#include "Tool/rstk/rstkDataParser.hpp"

#include <assert.h>
#include <string>
#include <set>


//============================================================================
// static variable
//============================================================================
rstkDocumentChunk* rstkDocumentChunk::sm_pActiveChunk = NULL;


//--------------------------------------------------------------------
// Return pointer to currently active chunk, may return NULL
//--------------------------------------------------------------------
rstkDocumentChunk* rstkDocumentChunk::GetActiveChunk()
{
	return sm_pActiveChunk;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rstkDocumentChunk::rstkDocumentChunk() 
:	m_bDirty(false), 
	m_bActive(false)
{
}

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void  rstkDocumentChunk::Clear()
{
	m_bDirty = false;

	if (this->m_bActive)
	{
		rstkDataMgr::Data().m_Resources.clear();
	}
}

//--------------------------------------------------------------------
//  returns chunk name for this data type
//--------------------------------------------------------------------
chDefs::Name  rstkDocumentChunk::GetChunkName() const
{
	return rstkDataParser::GetChunkName();
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual
const char* rstkDocumentChunk::GetChunkDesc() const
{
	return "rstk";
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
//virtual
void rstkDocumentChunk::BuildDataList( std::vector<std::string>& o_List )
{
}

//--------------------------------------------------------------------
//	remove the items in this chunk by name.  each chunk built the
//	list using BuildDataList() so each chunk will know what to do
//	with this data.
//--------------------------------------------------------------------
//virtual
void rstkDocumentChunk::RemoveItems( const std::vector<std::string>& i_List )
{
}

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void  rstkDocumentChunk::Read( chReader &i_Reader,
								 chDefs::Version i_Version,
								 chDefs::Size i_Size )
{
	rstkDataParser::ReadData( i_Reader, i_Version, i_Size, rstkDataMgr::Data() );

	m_bDirty = false;
}

//--------------------------------------------------------------------
//  Write chunk data
//--------------------------------------------------------------------
void  rstkDocumentChunk::Write(chWriter &i_Writer, bool i_bResetDirtyFlag )
{
	// create the list
	fsResourceTrackerData TopData;
	fsResourceTrackerData RTData;
	docDocumentWithChunks* pDoc = dynamic_cast<docDocumentWithChunks*>(docSingleDocumentMgr::GetDocument());

	//	search through the chunks and get a "high-level" list of assets.
	//
	int numchunks = pDoc->GetNumDocumentChunks();
	//DBG_LOG( "Write rstk chunk searching through " << numchunks << " chunks" );
	for ( int i = 0; i < numchunks ; ++i )
	{
		docDocumentChunk* pChunk = pDoc->GetDocumentChunk( i );
		pChunk->GetResourceList( TopData );
	}

	// Use a set to make sure the top level filenames are unique
	std::set<fsLocator> unique_filepaths;
	for (int j = 0; j < TopData.m_Resources.size(); ++j)
	{
		fsLocator filepath;
		filepath = TopData.m_Resources[j].GetFilePath();
		unique_filepaths.insert(filepath);
	}

	std::set<fsLocator>::iterator it;
	for (it = unique_filepaths.begin(); it != unique_filepaths.end(); ++it)
	{
		//std::string strpath;
		//fsFileUtil::LocatorToANSIFilename( *it, strpath );
		//DBG_LOG2("%03d. %s", j, strpath.c_str());

		if (it->GetNumNames() > 0)
		{
			fsResourceTracker::GetResourceList( it->GetLastName(), RTData.m_Resources );
		}
	}

	//	set the data before writing out.
	//
	//DBG_LOG("Before new Resource Data");
	//fsResourceTracker::Debug_OutputList();

	rstkDataMgr::SetData(RTData);

	//DBG_LOG("After new Resource Data");
	//fsResourceTracker::Debug_OutputList();

	rstkDataParser::WriteData( i_Writer, rstkDataMgr::Data() );

	if ( i_bResetDirtyFlag )
		m_bDirty = false;
}

//--------------------------------------------------------------------
//  Return true if the chunk has been modified since the last
// call to Write()
//--------------------------------------------------------------------
bool  rstkDocumentChunk::IsDirty() const
{
	return m_bDirty;
}

//--------------------------------------------------------------------
//  This chunk is currently made active, add data to the scene
// or world
//--------------------------------------------------------------------
void  rstkDocumentChunk::SetActive()
{
	m_bActive = true;
	sm_pActiveChunk = this;
}

//--------------------------------------------------------------------
//  this chunk is currently being made in active, remove data
// from scene or world
//--------------------------------------------------------------------
void  rstkDocumentChunk::SetInactive( bool i_bUpdateData )
{
	m_bActive = false;
	if (sm_pActiveChunk == this) sm_pActiveChunk = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rstkDocumentChunk::SetupPaths()
{
//	ProjectSetupMgr::SetData( m_PData );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rstkDocumentChunk::GetData(fsResourceTrackerData& o_Data)
{
	o_Data = rstkDataMgr::Data();
}

//--------------------------------------------------------------------
// Callback when dialog changes data, sets dirty bit
//--------------------------------------------------------------------
void rstkDocumentChunk::DataChanged()
{
	m_bDirty = true;
}

