/*****************************************************************************
**	docDocument.hpp
**
**	 The docDocument class is a base class for the information
**	that represents a file.  An app can derive a custom document from 
**	this type, or it can use the derived class for using document chunks.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef DOC_DOCUMENT_HPP
#error docDocument.hpp multiply included
#endif
#define DOC_DOCUMENT_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif

#include <set>


//============================================================================
//============================================================================
class fsResourceTrackerData;


//============================================================================
//============================================================================
class docDocument
{
public:
	//	these are obsolete, use gfFileConstants instead
	enum docWriteFormats
	{
		eDocBinary = 0,
		eDocXML = 1,
		eDocASCII = 2,
		eDocAll = 3			// special flag to write one of each format
	};

	enum LoadFlags
	{
		e_New_Doc = 0,
		e_Append_NoDupes = 1,
		e_Append_RenameDupes = 2
	};

public:
	//--------------------------------------------------------------------
	// Constructor
	//--------------------------------------------------------------------
	docDocument();

	//--------------------------------------------------------------------
	// Destructor
	//--------------------------------------------------------------------
	virtual ~docDocument();

	//--------------------------------------------------------------------
	// Clear
	//--------------------------------------------------------------------
	virtual void Clear() = 0;

	//--------------------------------------------------------------------
	// Load
	//
	//	returns true if the file loaded is a newer version than the
	//	user's executable supports.  The file is still loaded as best
	//	it can be.
	//--------------------------------------------------------------------
	virtual bool Load(const fsLocator &i_Locator, LoadFlags i_LoadMethod = e_New_Doc ) = 0;

	//--------------------------------------------------------------------
	// Save
	//--------------------------------------------------------------------
	virtual void Save( const fsLocator &i_Locator, bool i_bResetDirtyFlags = true ) = 0;

	//--------------------------------------------------------------------
	// IsLoading - return true if this document is currently loading
	//--------------------------------------------------------------------
	virtual bool IsLoading() = 0;

	//--------------------------------------------------------------------
	// IsDirty
	//--------------------------------------------------------------------
	virtual bool IsDirty() = 0;

	//--------------------------------------------------------------------
	// SetActive
	//--------------------------------------------------------------------
	virtual void SetActive() = 0;

	//--------------------------------------------------------------------
	// SetInactive - set the chunks for this document inactive
	//
	//	if i_bUpdateData is true then the app will ask the appropriate
	//	"manager" to give the chunk the current data.  This shouldn't be
	//	done only if you need to load another document while the current
	//	one is active.
	//--------------------------------------------------------------------
	virtual void SetInactive( bool i_bUpdateData = true ) = 0;

	//--------------------------------------------------------------------
	// SetWriteFormat - set the format for output
	//--------------------------------------------------------------------
	virtual void SetWriteFormat( int i_WriteFormat = docDocument::eDocBinary ) {};

	//--------------------------------------------------------------------
	// Filename
	//--------------------------------------------------------------------
	virtual const fsLocator& GetFilename() const = 0;
	virtual void SetFilename(const fsLocator &i_Filename) = 0;

	//--------------------------------------------------------------------
	//  Get a list of resources as unique full paths with no hierarchy.
	//--------------------------------------------------------------------
	virtual void GetResourceList( fsResourceTrackerData& o_ResourceTrackerData ) = 0;

};
