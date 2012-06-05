/*****************************************************************************
**	rpnDocumentChunk.hpp
**
**	 Derived chunk for render panels
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef RPN_DOCUMENTCHUNK_HPP
#error rpnDocumentChunk.hpp multiply included
#endif
#define RPN_DOCUMENTCHUNK_HPP

#ifndef DOC_DOCUMENTCHUNK_HPP
#include "Tool/doc/docDocumentChunk.hpp"
#endif


//============================================================================
//============================================================================
class rpnDocumentChunk : public docDocumentChunk
{
public:
	//--------------------------------------------------------------------
	// Return pointer to currently active chunk, may return NULL
	//--------------------------------------------------------------------
	static rpnDocumentChunk* GetActiveChunk();

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rpnDocumentChunk();

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
	//--------------------------------------------------------------------
	void SetupPaths();

	//--------------------------------------------------------------------
	// Callback when dialog changes data, sets dirty bit
	//--------------------------------------------------------------------
	void DataChanged();

private:
	static rpnDocumentChunk* sm_pActiveChunk;
};
