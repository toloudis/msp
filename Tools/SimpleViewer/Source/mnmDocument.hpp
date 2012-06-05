/*****************************************************************************
**	mnmDocument.hpp
**
**	 The mnmDocument class holds the chunks of information
**	that represents a file
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef MNM_DOCUMENT_HPP
#error mnmDocument.hpp multiply included
#endif
#define MNM_DOCUMENT_HPP

#ifndef DOC_DOCUMENT_HPP
#include "Tool/doc/docDocument.hpp"
#endif


//============================================================================
//============================================================================
class api3dObject;


//============================================================================
//============================================================================
class mnmDocument : public docDocument
{
public:
	//--------------------------------------------------------------------
	// Constructor
	//--------------------------------------------------------------------
	mnmDocument();

	//--------------------------------------------------------------------
	// Destructor
	//--------------------------------------------------------------------
	virtual ~mnmDocument();

	//--------------------------------------------------------------------
	// Clear
	//--------------------------------------------------------------------
	virtual void  Clear();

	//--------------------------------------------------------------------
	// Load
	//--------------------------------------------------------------------
	virtual void  Load(const fsLocator &i_Locator, bool i_bAppend = false );

	//--------------------------------------------------------------------
	// Save
	//--------------------------------------------------------------------
	virtual void  Save( const fsLocator &i_Locator, bool i_bResetDirtyFlags = true );

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

private:
	fsLocator m_Filename;
	
	api3dObject *m_pObject;

};
