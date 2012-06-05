/*****************************************************************************
**	docSingleDocumentMgr.hpp
**
**	 SingleDocumentMgr is the interface for the File menu
**	when the app is using a single document metaphor.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef DOC_SINGLEDOCUMENTMGR_HPP
#error docSingleDocumentMgr.hpp multiply included
#endif
#define DOC_SINGLEDOCUMENTMGR_HPP

#ifndef DOC_DOCUMENT_HPP
#include "Tool/doc/docDocument.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif

#include <set>


//============================================================================
//============================================================================
class fsResourceTrackerData;


//============================================================================
//============================================================================
namespace docSingleDocumentMgr
{
	//--------------------------------------------------------------------
	//	set the format for writing: binary or XML
	//--------------------------------------------------------------------
	void SetWriteFormat(int i_WriteFormat = docDocument::eDocBinary);

	//--------------------------------------------------------------------
	//  Call this on File->New after checking NeedsSave()
	//--------------------------------------------------------------------
	void  NewDocument();

	//--------------------------------------------------------------------
	//  Call this on File->Open after getting the filename
	//	and after checking NeedSave()
	//
	//	returns true if the file loaded is a newer version than the
	//	user's executable supports.  The file is still loaded as best
	//	it can be.
	//--------------------------------------------------------------------
	bool LoadDocument( const fsLocator &i_Locator, docDocument::LoadFlags i_LoadMethod = docDocument::e_New_Doc );

	//--------------------------------------------------------------------
	//  Call this on File->Save
	//--------------------------------------------------------------------
	void  SaveDocument();

	//--------------------------------------------------------------------
	//  Call this on File->Save As.. after getting the filename
	//--------------------------------------------------------------------
	void  SaveDocument( const fsLocator &i_Locator, bool i_bResetDirtyFlags = true );

	//--------------------------------------------------------------------
	//  Call this on File->Exit after checking NeedsSave()
	//--------------------------------------------------------------------
	void  CloseDocument();

	//--------------------------------------------------------------------
	//  Call this to see if current document needs saving.
	//--------------------------------------------------------------------
	bool  NeedsSave();

	//--------------------------------------------------------------------
	//  Call this to see if current document needs saving. If true,
	//	the current filename will be returned in o_Locator
	//--------------------------------------------------------------------
	bool  NeedsSave(fsLocator &o_Locator);

	//--------------------------------------------------------------------
	// Returns filename from current document
	//--------------------------------------------------------------------
	const fsLocator& GetFilename();

	//--------------------------------------------------------------------
	// Returns the filename ONLY
	//--------------------------------------------------------------------
	void GetFilenameOnly( std::string& o_Filename );

	//--------------------------------------------------------------------
	//	Change the filename of the document
	//--------------------------------------------------------------------
	void SetFilename( const fsLocator& i_Filename );

	//--------------------------------------------------------------------
	//	return a pointer to the single document
	//--------------------------------------------------------------------
	docDocument* GetDocument();

	//--------------------------------------------------------------------
	// Flags for current state of document manager
	//--------------------------------------------------------------------
	bool IsLoading();
	bool IsLoaded();
	bool IsSaving();

	//--------------------------------------------------------------------
	//  Get a list of resources used by the document.  
	//--------------------------------------------------------------------
	void GetResourceList( fsResourceTrackerData& o_ResourceTrackerData );
	void GetResourceList( std::set<fsLocator>& o_ResourceList );
}	// end of namespace
