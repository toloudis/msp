/*****************************************************************************
**	mspDocument.hpp
**
**	 The mspDocument class holds the chunks of information
**	that represents a file
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef MSP_DOCUMENT_HPP
#error mspDocument.hpp multiply included
#endif
#define MSP_DOCUMENT_HPP

#ifndef DOC_DOCUMENT_HPP
#include "Tool/doc/docDocument.hpp"
#endif


//============================================================================
//============================================================================
class api3dObject;


//============================================================================
//============================================================================
class mspDocument : public docDocument
{
public:
	//--------------------------------------------------------------------
	// Constructor
	//--------------------------------------------------------------------
	mspDocument();

	//--------------------------------------------------------------------
	// Destructor
	//--------------------------------------------------------------------
	virtual ~mspDocument();

	//--------------------------------------------------------------------
	// Clear
	//--------------------------------------------------------------------
	virtual void  Clear();

	//--------------------------------------------------------------------
	// Load
	//--------------------------------------------------------------------
	virtual bool  Load(const fsLocator &i_Locator, bool i_bAppend = false );

	//--------------------------------------------------------------------
	// Save
	//--------------------------------------------------------------------
	virtual void  Save( const fsLocator &i_Locator, bool i_bResetDirtyFlags = true );

	//--------------------------------------------------------------------
	// IsLoading - return true if this document is currently loading
	//--------------------------------------------------------------------
	virtual bool IsLoading();

	//--------------------------------------------------------------------
	// IsDirty
	//--------------------------------------------------------------------
	virtual bool  IsDirty();

	//--------------------------------------------------------------------
	// SetActive
	//--------------------------------------------------------------------
	virtual void  SetActive();

	//--------------------------------------------------------------------
	// SetInactive - set the chunks for this document inactive
	//
	//	if i_bUpdateData is true then the app will ask the appropriate
	//	"manager" to give the chunk the current data.  This shouldn't be
	//	done only if you need to load another document while the current
	//	one is active.
	//--------------------------------------------------------------------
	virtual void  SetInactive( bool i_bUpdateData = true );

	//--------------------------------------------------------------------
	// Filename
	//--------------------------------------------------------------------
	virtual const fsLocator& GetFilename() const;
	virtual void  SetFilename(const fsLocator &i_Filename);

	//--------------------------------------------------------------------
	//  Get a list of resources as unique full paths with no hierarchy.
	//--------------------------------------------------------------------
	virtual void GetResourceList( fsResourceTrackerData& o_ResourceTrackerData ) {}

private:
	fsLocator m_Filename;
	bool m_bIsLoading;

};
