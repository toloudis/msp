/*****************************************************************************
**	docCustomDocumentMgr.hpp
**
**	 docCustomDocumentMgr is the interface for the File menu
**	when the app is using a custom document type.  You need to
**	give it a document to handle in "ManageDocument"
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef DOC_CUSTOMDOCUMENTMGR_HPP
#error docCustomDocumentMgr.hpp multiply included
#endif
#define DOC_CUSTOMDOCUMENTMGR_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#ifndef DOC_DOCUMENT_HPP
#include "Tool/doc/docDocument.hpp"
#endif


//============================================================================
//============================================================================
namespace docCustomDocumentMgr
{
	//--------------------------------------------------------------------
	//  This passes in the custom document to be managed.  The ownership 
	//	of this document passes to this manager and the document is 
	//	deleted when "CloseDocument()" is called.
	//--------------------------------------------------------------------
	void ManageDocument(docDocument *i_pDocument);

	//--------------------------------------------------------------------
	//  Call this on File->New after checking NeedsSave()
	//--------------------------------------------------------------------
	void NewDocument();

	//--------------------------------------------------------------------
	//  Call this on File->Open after getting the filename
	//	and after checking NeedSave()
	//--------------------------------------------------------------------
	void LoadDocument(const fsLocator &i_Locator, docDocument::LoadFlags i_LoadMethod = docDocument::e_New_Doc );

	//--------------------------------------------------------------------
	//  Call this on File->Save
	//--------------------------------------------------------------------
	void SaveDocument();

	//--------------------------------------------------------------------
	//  Call this on File->Save As.. after getting the filename
	//--------------------------------------------------------------------
	void SaveDocument( const fsLocator &i_Locator, bool i_bResetDirtyFlags = true );

	//--------------------------------------------------------------------
	//  Call this on File->Exit after checking NeedsSave()
	//--------------------------------------------------------------------
	void CloseDocument();

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

}	// end of namespace
