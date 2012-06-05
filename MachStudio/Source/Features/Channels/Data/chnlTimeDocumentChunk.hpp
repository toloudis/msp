/*****************************************************************************
**	chnlTimeDocumentChunk.hpp
**
**	 Derived chunk for the Time.
**
**	StudioGPU
**	Copyright(C) 2005-7 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TIME_DOCUMENTCHUNK_HPP
#error chnlTimeDocumentChunk.hpp multiply included
#endif
#define CHNL_TIME_DOCUMENTCHUNK_HPP

#ifndef CHNL_TIME_DATA_HPP
#include "Features/Channels/Data/chnlTimeData.hpp"
#endif

#ifndef DOC_DOCUMENTCHUNK_HPP
#include "Tool/doc/docDocumentChunk.hpp"
#endif


//============================================================================
//============================================================================
class chnlTimeDocumentChunk : public docDocumentChunk
{
public:
	//--------------------------------------------------------------------
	// Call DataChanged function on the active document chunk
	//--------------------------------------------------------------------
	static void ActiveDataChanged();

public:

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	chnlTimeDocumentChunk();

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
	virtual void RemoveItems( const std::vector<std::string>& i_List );

	//--------------------------------------------------------------------
	//  Import chunk data and add it to existing data
	//--------------------------------------------------------------------
	virtual void  Import(chReader &i_Reader,
						 chDefs::Version i_Version,
						 chDefs::Size i_Size);

	//--------------------------------------------------------------------
	//  Read chunk data
	//--------------------------------------------------------------------
	virtual void  Read(chReader &i_Reader,
					   chDefs::Version i_Version,
					   chDefs::Size i_Size);

	//--------------------------------------------------------------------
	//  Write chunk data
	//--------------------------------------------------------------------
	virtual void  Write(chWriter &i_Writer, bool i_bResetDirtyFlag = true);

	//--------------------------------------------------------------------
	//  Return true if the chunk has been modified since the last
	// call to Write()
	//--------------------------------------------------------------------
	virtual bool  IsDirty() const;

	//--------------------------------------------------------------------
	//  This chunk is currently made active, add data to the scene
	// or world
	//--------------------------------------------------------------------
	virtual void  SetActive();

	//--------------------------------------------------------------------
	//  this chunk is currently being made in active, remove data
	// from scene or world
	//--------------------------------------------------------------------
	virtual void  SetInactive( bool i_bUpdateData );

	//--------------------------------------------------------------------
	// Callback when dialog changes data, sets dirty bit
	//--------------------------------------------------------------------
	void DataChanged();

private:
	mutable bool m_bDirty;
	bool m_bActive;						// could be in base class?
	chnlTimeData	m_Data;

	static chnlTimeDocumentChunk* sm_pActiveChunk;
};
