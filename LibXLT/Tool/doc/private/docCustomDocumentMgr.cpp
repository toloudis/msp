/*****************************************************************************
**	docCustomDocumentMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/doc/docCustomDocumentMgr.hpp"

#include "Core/app/appTime.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"


//============================================================================
//============================================================================
namespace docCustomDocumentMgr
{
	namespace
	{
		docDocument*	l_Document = 0;
		fsLocator		l_Empty;
	}

	//--------------------------------------------------------------------
	//  This passes in the custom document to be managed.  The ownership 
	//	of this document passes to this manager and the document is 
	//	deleted when "CloseDocument()" is called.
	//--------------------------------------------------------------------
	void ManageDocument(docDocument *i_pDocument)
	{
		DBG_ASSERT(l_Document == NULL, "Already have an existing document. Call CloseDocument() first.");
		l_Document = i_pDocument;
	}

	//--------------------------------------------------------------------
	//  Call this on File->New
	//--------------------------------------------------------------------
	void NewDocument()
	{
		DBG_ASSERT(l_Document != NULL, "Need an existing document. Call ManageDocument() first.");
		undoUndoMgr::Commit();
		
		//l_Document->SetInactive();
		l_Document->Clear();
		
		l_Document->SetActive();	//?
	}

	//--------------------------------------------------------------------
	//  Call this on File->Open after getting the filename
	//--------------------------------------------------------------------
	void LoadDocument( const fsLocator &i_Locator, docDocument::LoadFlags i_LoadMethod )
	{
		DBG_ASSERT(l_Document != NULL, "Need an existing document. Call ManageDocument() first.");

		if (i_LoadMethod == docDocument::e_New_Doc)
		{
			NewDocument();
		}

		try
		{
			float begin_time = appTime::GetTime();

			l_Document->Load( i_Locator, i_LoadMethod );
			docSingleTypeMgr::AddToMRU(i_Locator);

			float duration = appTime::GetTime() - begin_time;
			DBG_LOG("Load time " << duration);
		}
		catch(...)
		{
			// If exception ocurred while loading, clear out the document
			// data that was partially loaded and then pass the exception up to 
			// the caller in order to let them display the error.
			//CloseDocument();
			l_Document->Clear();
			throw;
		}
	}

	//--------------------------------------------------------------------
	//  Call this on File->Save
	//--------------------------------------------------------------------
	void SaveDocument()
	{
		DBG_ASSERT(l_Document != NULL, "Need an existing document. Call ManageDocument() first.");
		
		// Confirm undo on save also?
		//undoUndoMgr::Commit();

		DBG_ASSERT(l_Document, "Document is NULL");
		l_Document->Save(l_Document->GetFilename());
		docSingleTypeMgr::AddToMRU(l_Document->GetFilename());

	}

	//--------------------------------------------------------------------
	//  Call this on File->Save As.. after getting the filename
	//--------------------------------------------------------------------
	void SaveDocument( const fsLocator &i_Locator, bool i_bResetDirtyFlags )
	{
		DBG_ASSERT(l_Document != NULL, "Need an existing document. Call ManageDocument() first.");
		
		// Confirm undo on save also?
		//undoUndoMgr::Commit();

		DBG_ASSERT(l_Document, "Document is NULL");
		l_Document->Save( i_Locator, i_bResetDirtyFlags );
		docSingleTypeMgr::AddToMRU(i_Locator);
	}

	//--------------------------------------------------------------------
	//  Call this on File->Exit
	//--------------------------------------------------------------------
	void CloseDocument()
	{
		undoUndoMgr::Commit();
		if (l_Document)
		{
			l_Document->Clear();
			l_Document->SetInactive();
		}

		delete l_Document;
		l_Document = NULL;

	}

	//--------------------------------------------------------------------
	//  Call this to see if current document needs saving.
	//--------------------------------------------------------------------
	bool  NeedsSave()
	{
		if (l_Document && l_Document->IsDirty())
		{
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------------
	//  Call this to see if current document needs saving. If true,
	//	the current filename will be returned in o_Locator
	//--------------------------------------------------------------------
	bool  NeedsSave(fsLocator &o_Locator)
	{
		if (l_Document && l_Document->IsDirty())
		{
			o_Locator = l_Document->GetFilename();
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------------
	// Returns filename from current document
	//--------------------------------------------------------------------
	const fsLocator& GetFilename()
	{
		if ( l_Document )
		{
			return l_Document->GetFilename();
		}
		else
		{
			return l_Empty;
		}
	}

	//--------------------------------------------------------------------
	// Returns the filename ONLY
	//--------------------------------------------------------------------
	void GetFilenameOnly( std::string& o_Filename )
	{
		if ( l_Document )
		{
			o_Filename = itStringUtil::GetStdString( l_Document->GetFilename().GetLastName() );
		}
	}

	//--------------------------------------------------------------------
	//	Change the filename of the document
	//--------------------------------------------------------------------
	void SetFilename( const fsLocator& i_Filename )
	{
		if ( l_Document )
		{
			l_Document->SetFilename( i_Filename );
		}
	}

	//--------------------------------------------------------------------
	//	return a pointer to the single document
	//--------------------------------------------------------------------
	docDocument* GetDocument()
	{
		return l_Document;
	}

}	// end of namespace
