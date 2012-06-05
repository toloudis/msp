/*****************************************************************************
**	mnmTitleCardDocumentChunk.hpp
**
**		TitleCard document chunk
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCDC_TIMELINEITEMDOCUMENTCHUNK_HPP
#error fcdcTimelineItemDocumentChunk.hpp multiply included
#endif
#define FCDC_TIMELINEITEMDOCUMENTCHUNK_HPP

#ifndef DOC_DOCUMENTCHUNK_HPP
#include "Tool/doc/docDocumentChunk.hpp"
#endif
#ifndef FCDC_TIMELINEITEMDATA_HPP
#include "FCSupport/fcdc/data/fcdcTimelineItemData.hpp"
#endif


//============================================================================
//============================================================================
class fcdcTimelineItemDocumentChunk : public docDocumentChunk
{
public:
	//--------------------------------------------------------------------
	// Return pointer to currently active chunk, may return NULL
	//--------------------------------------------------------------------
	static fcdcTimelineItemDocumentChunk* GetActiveChunk();

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	fcdcTimelineItemDocumentChunk();

	//--------------------------------------------------------------------
	//  Clear document data back to initial state
	//--------------------------------------------------------------------
	virtual void  Clear();

	//--------------------------------------------------------------------
	//  returns chunk name for this data type
	//--------------------------------------------------------------------
	virtual chDefs::Name  GetChunkName() const;

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;

	//--------------------------------------------------------------------
	//  build a list of "items" in this chunk
	//
	//	this is used for itemizing of objects in this chunk.  It can
	//	also be used to select parts of this chunk as "active" or not.
	//--------------------------------------------------------------------
	virtual void BuildDataList( std::vector<std::string>& o_List );

	//--------------------------------------------------------------------
	//	remove the items in this chunk by name.  each chunk built the
	//	list using BuildDataList() so each chunk will know what to do
	//	with this data.
	//--------------------------------------------------------------------
	virtual void RemoveItems(const std::vector<std::string>& i_List );

	//--------------------------------------------------------------------
	//  Read chunk data
	//--------------------------------------------------------------------
	virtual void Read(chReader &io_Reader,
					  chDefs::Version i_Version,
					  chDefs::Size i_Size);

	//--------------------------------------------------------------------
	//  Write chunk data
	//--------------------------------------------------------------------
	virtual void Write(chWriter &io_Writer, bool i_bResetDirtyFlag = true);

	//--------------------------------------------------------------------
	//  Return true if the chunk has been modified since the last
	// call to Write()
	//--------------------------------------------------------------------
	virtual bool IsDirty() const;

	//--------------------------------------------------------------------
	//  This chunk is currently made active, add data to the scene
	// or world
	//--------------------------------------------------------------------
	virtual void SetActive();

	//--------------------------------------------------------------------
	//  this chunk is currently being made in active, remove data
	// from scene or world
	//--------------------------------------------------------------------
	virtual void SetInactive( bool i_bUpdateData );

	//--------------------------------------------------------------------
	//  This virtual function is called on all chunks after all 
	//	chunks have finished loading successfully
	//--------------------------------------------------------------------
	virtual void NotifyLoadFinished();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetupPaths();

	//--------------------------------------------------------------------
	// Callback when dialog changes data, sets dirty bit
	//--------------------------------------------------------------------
	void DataChanged();

private:
	static fcdcTimelineItemDocumentChunk* sm_pActiveChunk;
		mutable bool m_bDirty;
	fcdcTimelineItemData m_Data;
	bool	m_bChunkRead;
};
