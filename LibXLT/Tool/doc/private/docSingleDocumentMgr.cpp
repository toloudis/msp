/*****************************************************************************
**	docSingleDocumentMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/doc/docSingleDocumentMgr.hpp"

#include "Core/App/appTime.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/Fs/fsResourceTracker.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"


//============================================================================
//============================================================================
namespace docSingleDocumentMgr
{
	namespace
	{
		docDocument*	l_Document = 0;
		bool l_bIsSaving = false;
		int l_WriteFormat = docDocument::eDocBinary;

		fsLocator	l_Empty;

		// structure for exception safety to make sure the
		// boolean flag l_bIsSaving is restored to false when exception is thrown
		struct safe_flag_set
		{
			safe_flag_set(bool &i_bFlag) : m_bFlag(i_bFlag)
			{
				m_bFlag = true;
			}
			~safe_flag_set()
			{
				m_bFlag = false;
			}
			bool &m_bFlag;
		};
	}

	//--------------------------------------------------------------------
	//	set the format for writing: binary or XML
	//--------------------------------------------------------------------
	void SetWriteFormat(int i_WriteFormat)
	{
		l_WriteFormat = i_WriteFormat;

		if (l_Document)
			l_Document->SetWriteFormat( i_WriteFormat );
	}

	//--------------------------------------------------------------------
	//  Call this on File->New
	//--------------------------------------------------------------------
	void  NewDocument()
	{
		undoUndoMgr::Commit();
		if (l_Document)
		{
			//l_Document->SetInactive();
			l_Document->Clear();
		}
		else
		{
			l_Document = docSingleTypeMgr::CreateDocument(l_WriteFormat);
		}

		nameMgr::ResetUID();
		l_Document->SetActive();
	}

	//--------------------------------------------------------------------
	//  Call this on File->Open after getting the filename
	//
	//	returns true if the file loaded is a newer version than the
	//	user's executable supports.  The file is still loaded as best
	//	it can be.
	//--------------------------------------------------------------------
	bool LoadDocument( const fsLocator &i_Locator, docDocument::LoadFlags i_LoadMethod )
	{
		if ( i_LoadMethod == docDocument::e_New_Doc )
		{
			NewDocument();
		}

		bool bNewerVersion = false;
		try
		{
			float begin_time = appTime::GetTime();

			bNewerVersion = l_Document->Load( i_Locator, i_LoadMethod );
			docSingleTypeMgr::AddToMRU(i_Locator);

			float duration = appTime::GetTime() - begin_time;
			DBG_LOG("Load Time " << duration);
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

		return bNewerVersion;
	}

	//--------------------------------------------------------------------
	//  Call this on File->Save
	//--------------------------------------------------------------------
	void  SaveDocument()
	{
		// Confirm undo on save also?
		//undoUndoMgr::Commit();
		DBG_ASSERT(l_Document, "Document is NULL");
		// Exception safe structure sets flag to true and then restores it to false on destructor
		safe_flag_set save_flag(l_bIsSaving);
		//l_bIsSaving = true;
		l_Document->Save(l_Document->GetFilename());
		//l_bIsSaving = false;
		docSingleTypeMgr::AddToMRU(l_Document->GetFilename());
	}

	//--------------------------------------------------------------------
	//  Call this on File->Save As.. after getting the filename
	//--------------------------------------------------------------------
	void  SaveDocument( const fsLocator &i_Locator, bool i_bResetDirtyFlags )
	{
		// Confirm undo on save also?
		//undoUndoMgr::Commit();

		DBG_ASSERT(l_Document, "Document is NULL");
		// Exception safe structure sets flag to true and then restores it to false on destructor
		safe_flag_set save_flag(l_bIsSaving);
		//l_bIsSaving = true;
		l_Document->Save( i_Locator, i_bResetDirtyFlags );
		//l_bIsSaving = false;

		if ( i_bResetDirtyFlags )
		{
			docSingleTypeMgr::AddToMRU(i_Locator);
		}
	}

	//--------------------------------------------------------------------
	//  Call this on File->Exit
	//--------------------------------------------------------------------
	void  CloseDocument()
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
			fsLocator fullpath = l_Document->GetFilename();
			if (fullpath.GetNumNames() == 0)
				o_Filename = "";
			else
				o_Filename = itStringUtil::GetStdString( fullpath.GetLastName() );
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

	//--------------------------------------------------------------------
	// Flags for current state of document manager
	//--------------------------------------------------------------------
	bool IsLoading()
	{
		if (l_Document)
			return l_Document->IsLoading();
		return false;
	}
	bool IsLoaded()
	{
		return (l_Document != 0);
	}
	bool IsSaving()
	{
		return l_bIsSaving;
	}

	//--------------------------------------------------------------------
	//  Get a list of resources used by the document. 
	//--------------------------------------------------------------------
	void GetResourceList( fsResourceTrackerData& o_ResourceTrackerData ) 
	{
		if (l_Document)
			return l_Document->GetResourceList(o_ResourceTrackerData);
	}

	//--------------------------------------------------------------------
	//  Get a list of resources used by the document. 
	//--------------------------------------------------------------------
	void GetResourceList( std::set<fsLocator>& o_ResourceList )
	{
		if (l_Document)
		{
			fsResourceTrackerData rtdata;
			l_Document->GetResourceList(rtdata);

			// Use a set to make sure the top level filenames are unique
			o_ResourceList.clear();
			for (int j = 0; j < rtdata.m_Resources.size(); ++j)
			{
				fsLocator filepath;
				filepath = rtdata.m_Resources[j].GetFilePath();
				o_ResourceList.insert(filepath);
			}
		}
	}
}	// end of namespace
