/*****************************************************************************
**	guiCustomDocHandler.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiCustomDocHandler.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Tool/doc/docCustomDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSplashScreen.hpp"

#include <assert.h>


//============================================================================
//============================================================================
namespace guiCustomDocHandler
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
			if( ext_pos != std::string::npos)
			{
				 scene_type = cur_scene.substr(ext_pos);
				 substr_count = ext_pos;
			}
			if(cur_scene.substr(0, substr_count).length() > 22)
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

			// load
			try
			{
				docCustomDocumentMgr::LoadDocument(i_Loc);

				if ( l_bRememberLastDirectory )
				{
					fsLocator file_loc = i_Loc;
					file_loc.Pop();						// pop off the filename
					l_InitialDirectory = file_loc;		// store the initial directory
				}
			}
			catch ( const envExceptionX& i_Ex )
			{
				std::string msg = "Problem loading file, " + i_Ex.GetErrorMessage();
				DBG_ERROR(msg);
				
				if(i_bSkipErrorDialog)
				{
					throw;
				}
				
				guiMessageBox::Show(msg.c_str(), "Load Error", guiMessageBox::e_OKOnly);
				
			}
			catch (const std::bad_alloc&)
			{
				std::string msg = "Out of system memory, could not load file.";
				DBG_ERROR(msg);

				if(i_bSkipErrorDialog)
				{
					throw;
				}
				
				guiMessageBox::Show(msg.c_str(), "Load Error", guiMessageBox::e_OKOnly);
				
			}
			catch( ... )
			{
				std::string msg = "General exception error loading file.";
				DBG_ERROR(msg);

				if(i_bSkipErrorDialog)
				{
					throw;
				}
				
				guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
				
			}

			guiSplashScreen::ShutDown();
		}
		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		void safe_save_document(const fsLocator &i_Loc)
		{
			try
			{
				//std::string filename;
				//fsFileUtil::LocatorToANSIFilename(i_Loc, filename);
				//DBG_LOG("saving document: " << filename.c_str());

				docCustomDocumentMgr::SaveDocument(i_Loc);
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
				guiMessageBox::Show(msg.c_str(),"Save Error", guiMessageBox::e_OKOnly);
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
		if (docCustomDocumentMgr::NeedsSave(file_loc))
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
	void  New()
	{
		if (SaveIfDirty())
			docCustomDocumentMgr::NewDocument();
	}

	//--------------------------------------------------------------------
	// Open
	//--------------------------------------------------------------------
	void  Open(bool i_bSkipErrorDialog)
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
				safe_load_document(file_loc, i_bSkipErrorDialog);
			}
		}
	}

	//--------------------------------------------------------------------
	// Open - called from MRU
	//--------------------------------------------------------------------
	void  Open(const fsLocator &i_Locator, bool i_bSkipErrorDialog)
	{
		if (SaveIfDirty())
		{
			safe_load_document(i_Locator, i_bSkipErrorDialog);
		}
	}
	//--------------------------------------------------------------------
	// Save
	//--------------------------------------------------------------------
	void  Save()
	{
		fsLocator file_loc = docCustomDocumentMgr::GetFilename();
		if (file_loc.GetNumNames() == 0)
			SaveAs();
		else
			safe_save_document(file_loc);
	}

	//--------------------------------------------------------------------
	// SaveAs
	//--------------------------------------------------------------------
	void  SaveAs()
	{
		fsLocator file_loc = docCustomDocumentMgr::GetFilename();

		std::string filter = docSingleTypeMgr::GetFilter();
		if (guiFileDialogUtils::GetSaveFileName(filter, file_loc))
		{
			//std::string filename;
			//fsFileUtil::LocatorToANSIFilename(file_loc, filename);
			//DBG_LOG("save as document: " << filename.c_str());

			safe_save_document(file_loc);
		}
	}

	//--------------------------------------------------------------------
	// Exit
	//--------------------------------------------------------------------
	void  Exit()
	{
		docCustomDocumentMgr::CloseDocument();
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

}	// end of namespace
