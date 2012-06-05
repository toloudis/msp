/*****************************************************************************
**	docDocumentChunk.hpp
**
**	 Base class for chunks of a document, each chunk is
**	handled by its own system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef DOC_DOCUMENTCHUNK_HPP
#error docDocumentChunk.hpp multiply included
#endif
#define DOC_DOCUMENTCHUNK_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef FS_RESOURCETRACKERDATA_HPP
#include "Core/fs/fsResourceTrackerData.hpp"
#endif

#include <vector>
#include <string>


//============================================================================
// forward references
//============================================================================
class chReader;
class chWriter;


//============================================================================
//============================================================================
class docDocumentChunk
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~docDocumentChunk() = 0;

	//--------------------------------------------------------------------
	//  Clear document data back to initial state
	//--------------------------------------------------------------------
	virtual void  Clear() = 0;

	//--------------------------------------------------------------------
	//  returns chunk name for this data type
	//--------------------------------------------------------------------
	virtual chDefs::Name  GetChunkName() const = 0;

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const = 0;

	//--------------------------------------------------------------------
	//  build a list of "items" in this chunk
	//
	//	this is used for itemizing of objects in this chunk.  It can
	//	also be used to select parts of this chunk as "active" or not.
	//--------------------------------------------------------------------
	virtual void BuildDataList( std::vector<std::string>& o_List ) = 0;

	//--------------------------------------------------------------------
	//  get a list of resources.  the resources will be appended to the
	//	passed in list.
	//--------------------------------------------------------------------
	virtual void GetResourceList( fsResourceTrackerData& io_List ) {};

	//--------------------------------------------------------------------
	//	remove the items in this chunk by name.  each chunk built the 
	//	list using BuildDataList() so each chunk will know what to do
	//	with this data.
	//--------------------------------------------------------------------
	virtual void RemoveItems( const std::vector<std::string>& i_List ) = 0;

	//--------------------------------------------------------------------
	//  Read chunk data
	//--------------------------------------------------------------------
	virtual void  Read(chReader &i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size) = 0;

	//--------------------------------------------------------------------
	//  Import chunk data and add it to existing data.
	//	Default implementation simply calls Read().
	//--------------------------------------------------------------------
	virtual void  Import(chReader &i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						bool i_bRenameDupes = false);

	//--------------------------------------------------------------------
	//  Write chunk data
	//--------------------------------------------------------------------
	virtual void  Write( chWriter &o_Writer, bool i_bResetDirtyFlag = true ) = 0;

	//--------------------------------------------------------------------
	//  Return true if the chunk has been modified since the last
	// call to Write()
	//--------------------------------------------------------------------
	virtual bool  IsDirty() const = 0;

	//--------------------------------------------------------------------
	// Callback when dialog changes data, sets dirty bit
	//--------------------------------------------------------------------
	virtual void DataChanged() = 0;

	//--------------------------------------------------------------------
	//  This chunk is currently made active, add data to the scene
	// or world
	//--------------------------------------------------------------------
	virtual void  SetActive() = 0;

	//--------------------------------------------------------------------
	//  this chunk is currenlty being made in active, remove data
	// from scene or world
	//--------------------------------------------------------------------
	virtual void  SetInactive( bool i_bUpdateData ) = 0;

	//--------------------------------------------------------------------
	//  This virtual function is called on all chunks after all 
	//	chunks have finished loading successfully
	//--------------------------------------------------------------------
	virtual void  NotifyLoadFinished() {}
};
