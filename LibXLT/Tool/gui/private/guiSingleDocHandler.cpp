/*****************************************************************************
**	guiSingleDocHandler.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiSingleDocHandler.hpp"

#include "Core/app/appTime.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/fs/private/fsFileNotifyMgr.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/mat/matExceptionX.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiCursor.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSplashScreen.hpp"

#include <assert.h>


//============================================================================
//============================================================================
namespace guiSingleDocHandler
{
	namespace
	{
		fsLocator	l_InitialDirectory( 0, itString(".") );
		bool		l_bRememberLastDirectory = false;

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		void safe_load_document(const fsLocator &i_Loc, bool i_bSkipErrorDialog = false)
		{
		
		gfPaths::SetPath(gfPaths::e_MabPath, i_Loc);

		#ifndef _DEBUG
			//Setup loading text for the new file, truncates if filename length is too long
			std::string load_txt = "Loading : ";
			std::string cur_scene = itStringUtil::GetStdString( i_Loc.GetLastName() );

			int ext_pos = cur_scene.find(".");
			int substr_count = cur_scene.length();
			std::string scene_type = "";
			if ( ext_pos != std::string::npos)
			{
				 scene_type = cur_scene.substr(ext_pos);
				 substr_count = ext_pos;
			}
			if (cur_scene.substr(0, substr_count).length() > 22)
			{
				cur_scene = cur_scene.substr(0,20);
				cur_scene += "..." + scene_type;
			}
			
			load_txt += cur_scene;

			guiSplashScreen::SetDoSplashTimeout(false);
			fsLocator splashPath = gfPaths::GetPath(gfPaths::e_ExePath);
			splashPath.Push("Data");
			splashPath.Push("Load.png");
			if (fsFileUtil::FileExists(splashPath))
			{
				guiSplashScreen::StartUp(splashPath, load_txt.c_str());
			}
		#endif
			DBG_LOG("Opening file " << i_Loc);
			//float load_start_time = appTime::GetTime();

			fsResourceTracker::Init();
			fsFileNotifyMgr::Intialize();

			bool bNewerVersion = false;		// track if loaded file is newer than supported version

			//	Load the document
			//
			try
			{
				bNewerVersion = docSingleDocumentMgr::LoadDocument(i_Loc);

				if ( l_bRememberLastDirectory )
				{
					fsLocator file_loc = i_Loc;
					file_loc.Pop();						// pop off the filename
					l_InitialDirectory = file_loc;		// store the initial directory
				}
			}
			catch( const fsNewerVersionAbortX& )
			{
				//	don't report the error.
			}
			catch( const fsUnsupportedVersionX& i_Ex )
			{
				//	file was created with a software package that this application doesn't support
				//	(e.g. MachStudio Pro file can't be read in MSCore)
				//
				std::string msg = i_Ex.GetErrorMessage();
				DBG_ERROR(msg);

				if (i_bSkipErrorDialog)
				{
					throw;
				}

				guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
			}
			catch ( const envExceptionX& i_Ex )
			{
				std::string msg = "Problem loading file, " + i_Ex.GetErrorMessage();
				DBG_ERROR(msg);

				if (i_bSkipErrorDialog)
				{
					throw;
				}

				guiMessageBox::Show(msg.c_str(), "Load Error", guiMessageBox::e_OKOnly);
			}
			catch (const std::bad_alloc&)
			{
				std::string msg = "Out of system memory, could not load file.";
				DBG_ERROR(msg);

				if (i_bSkipErrorDialog)
				{
					throw;
				}
				
				guiMessageBox::Show(msg.c_str(), "Load Error", guiMessageBox::e_OKOnly);
			}
			catch( ... )
			{
				std::string msg = "General exception error loading file.";
				DBG_ERROR(msg);

				if (i_bSkipErrorDialog)
				{
					throw;
				}

				guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
			}

			guiSplashScreen::ShutDown();

			//DBG_LOG(":LoadTime = " << (appTime::GetTime() - load_start_time));

			//	Check if the file was newer.  If so, ask them if they want to continue.
			//
			if (bNewerVersion)
			{
				int retval = guiMessageBox::Show("This file was created with a newer version of the application.  Some of the data of the scene might be lost.  Do you want to continue to try and load this file?","Newer File Encountered", guiMessageBox::e_YesNo);
				if (retval == guiMessageBox::e_No)
				{
					docSingleDocumentMgr::NewDocument();
				}
			}
		}
		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		void safe_save_document(const fsLocator &i_Loc)
		{
			try
			{
				docSingleDocumentMgr::SaveDocument(i_Loc);
			}
			catch ( const envExceptionX& i_Ex )
			{
				std::string msg = "Problem saving file, " + i_Ex.GetErrorMessage();
				DBG_ERROR(msg);
				guiMessageBox::Show(msg.c_str(), "Save Error", guiMessageBox::e_OKOnly);
			}
			catch( ... )
			{
				std::string msg = "General exception error saving file.";
				DBG_ERROR(msg);
				guiMessageBox::Show(msg.c_str(), "Save Error", guiMessageBox::e_OKOnly);
			}
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	// SaveIfDirty - ask user to save document if it is dirty.
	//		Returns false if User answered "Cancel"
	//--------------------------------------------------------------------
	bool SaveIfDirty()
	{
		fsLocator file_loc;
		if (docSingleDocumentMgr::NeedsSave(file_loc))
		{
			int res = guiMessageBox::Show("Save changes to document?",
				"Save document",
				guiMessageBox::e_YesNoCancel);
				//guiMessageBox::Question);

			if (res == guiMessageBox::e_Cancel)
				return false;

			if (res == guiMessageBox::e_Yes)
			{
				// this might route to SaveAs if no filename is assigned yet
				Save();
			}
		}
		return true;
	}

	//--------------------------------------------------------------------
	// New
	//--------------------------------------------------------------------
	void New()
	{
		if (SaveIfDirty())
		{
			docSingleDocumentMgr::NewDocument();
		}
	}

	//--------------------------------------------------------------------
	// Open
	//--------------------------------------------------------------------
	void Open(bool i_bSkipErrorDialog)
	{
		
		std::string filter = docSingleTypeMgr::GetFilter();
		fsLocator initial_dir;
		fsLocator file_loc;
		//fsFileUtil::UnicodeStringToLocator( itString("."), initial_dir );'
		initial_dir = l_InitialDirectory;

		if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
		{
			if (SaveIfDirty())
			{
				guiCursor::SetWaitCursor();
				safe_load_document(file_loc, i_bSkipErrorDialog);
				guiCursor::EndWaitCursor();
			}
		}

	}

	//--------------------------------------------------------------------
	// Open - called from MRU
	//--------------------------------------------------------------------
	void Open(const fsLocator &i_Filename, 
				bool i_bSkipSaveCheck, 
				bool i_bSkipErrorDialog)
	{
		/*if (i_bSkipSaveCheck || SaveIfDirty())
		{
			safe_load_document(i_Filename, i_bSkipErrorDialog);
		}*/
		if (i_bSkipSaveCheck)
		{
			safe_load_document(i_Filename, i_bSkipErrorDialog);
		}
		else if (SaveIfDirty())
		{
			safe_load_document(i_Filename, i_bSkipErrorDialog);
		}
	}
	//--------------------------------------------------------------------
	// Save
	//--------------------------------------------------------------------
	void Save()
	{
		fsLocator file_loc = docSingleDocumentMgr::GetFilename();
		if (file_loc.GetNumNames() == 0)
			SaveAs();
		else
			safe_save_document(file_loc);
	}

	//--------------------------------------------------------------------
	// SaveAs
	//--------------------------------------------------------------------
	void SaveAs()
	{
		fsLocator file_loc = docSingleDocumentMgr::GetFilename();
		fsLocator orig_name = file_loc;

		std::string filter = docSingleTypeMgr::GetFilter();
		if (guiFileDialogUtils::GetSaveFileName(filter, file_loc))
		{
			//	if the names are different, replace them in the resource tracker
			//
			if (orig_name != file_loc)
			{
				//fsResourceTracker::ReplaceFile(orig_name, file_loc);

				// DEBUG ONLY
				//fsResourceTracker::Debug_OutputList();
			}

			safe_save_document(file_loc);
		}
	}

	//--------------------------------------------------------------------
	// Exit
	//--------------------------------------------------------------------
	void Exit()
	{
		docSingleDocumentMgr::CloseDocument();
	}

	//--------------------------------------------------------------------
	//	the initial directory Open() starts in
	//--------------------------------------------------------------------
	void SetInitialDirectory( const fsLocator& i_InitialDir )
	{
		l_InitialDirectory = i_InitialDir;
	}

	//--------------------------------------------------------------------
	//	set a flag so the initial directory for Open() starts in
	//	the last directory the user picked.
	//--------------------------------------------------------------------
	void SetInitialDirectoryToBeLast( bool i_bRememberLastDir )
	{
		l_bRememberLastDirectory = i_bRememberLastDir;
	}

	//--------------------------------------------------------------------
	//	is there a document loaded?
	//--------------------------------------------------------------------
	bool IsLoaded()
	{
		return docSingleDocumentMgr::IsLoaded();
	}

	//--------------------------------------------------------------------
	//	This function will check the versions passed in and if they
	//	are different it will ask the user to abort to conitnue.  If the
	//	user continues, it won't ask for other newer chunks found.  If
	//	the user aborts, it will throw an exception.
	//
	//	throws: fsNewerVersionAbortX
	//--------------------------------------------------------------------
	void CheckForNewerChunkVersion( chReader& i_Reader,
									const chDefs::Version i_ReadVersion,
									const chDefs::Version i_ExpectedVersion )
	{
		//	if the file hasn't already been flagged as a newer version
		//	(which the user would already have been notified of) and
		//	a newer version chunk has occurred
		//
		if (!(i_Reader.IsNewerVersion()) && (i_ReadVersion > i_ExpectedVersion))
		{
			i_Reader.SetNewerVersion(true);

			//int retval = guiMessageBox::Show("This file was created with a newer version of the application.  Some of the data of the scene might be lost.  Do you want to continue to try and load this file?","Newer File Encountered", guiMessageBox::e_YesNo);
			//if (retval == guiMessageBox::e_No)
			//{
			//	// abort the reading, throw an exception
			//	throw fsNewerVersionAbortX(i_Reader.GetLocator());
			//}
		}
	}
	fsLocator GetInitialDirectory()
	{
		return l_InitialDirectory;
	}

}	// end of namespace
