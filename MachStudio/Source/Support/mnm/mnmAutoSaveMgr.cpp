/*****************************************************************************
**  mnmAutoSaveMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/mnm/mnmAutoSaveMgr.hpp"

#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmSecurityMgr.hpp"

//	tools
#include "Tool/gui/guiMessageBox.hpp"

//	library
#include "Core/app/appTime.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"

#include <assert.h>
#include <iomanip>
#include <sstream>
#include <string>


//============================================================================
//============================================================================
namespace AutoSaveData
{
	bool	l_bAutoSaveActive = true;
	float	l_AutoSaveFrequencyInSeconds;
	int		l_AutoSaveBackups;
	int		l_AutoSaveBackupCounter;
	float	l_fLastAutoSave;
}


//============================================================================
//============================================================================
namespace
{

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void set_auto_save_time()
{
	AutoSaveData::l_fLastAutoSave = appTime::GetTime();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void create_autosave_filename( itString& io_filename )
{
	io_filename.StripExtension();
	
	/*char extras[ 8 ];
	sprintf( extras, "~%02d.mab", AutoSaveData::l_AutoSaveBackupCounter );
	io_filename += itString( extras );*/
	
	std::ostringstream extras_stream(std::ostringstream::out);
	std::string extras;
	extras_stream << "~" <<  std::setw(2) << std::setfill('0') <<AutoSaveData::l_AutoSaveBackupCounter << ".mab";
	extras = extras_stream.str();
	
	io_filename += itString( extras.c_str() );

	AutoSaveData::l_AutoSaveBackupCounter++;

	if ( AutoSaveData::l_AutoSaveBackupCounter >= AutoSaveData::l_AutoSaveBackups )
	{
		AutoSaveData::l_AutoSaveBackupCounter = 0;
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void perform_auto_save()
{
	if ( !docSingleDocumentMgr::NeedsSave() )
	{
		// if the document doesn't need saving then reset the time.
		set_auto_save_time();
		return;
	}
	
	if ( (AutoSaveData::l_fLastAutoSave + (float)(AutoSaveData::l_AutoSaveFrequencyInSeconds)) > appTime::GetTime() )
	{
		return;
	}

	//	only perform the auto-save if security is present
	if (!mnmSecurityMgr::CheckSecurity1())
	{
		set_auto_save_time();	// update the save variables as if we saved.
		return;
	}

	//DBG_TRACE( "last %6.2f  freq %6.2f  time=%6.2f", AutoSaveData::l_fLastAutoSave, AutoSaveData::l_AutoSaveFrequencyInSeconds, appTime::GetTime() );

	//	perform the save
	//
	fsLocator origfile = docSingleDocumentMgr::GetFilename();
	itString filename = (origfile.GetNumNames() > 0) ? origfile.GetLastName() : itString("Untitled");
	
	fsLocator bufile;
	bufile = gfPaths::GetPath( mnmPaths::e_AutoSave );

	//	If the directory doesn't exist, create it first
	//
	if (!fsFileUtil::DirectoryExists( bufile ))
	{
		fsFileUtil::CreateDirectory( bufile );
	}

	create_autosave_filename( filename );
	bufile.Push( filename );

	//  debug output
	//
	DBG_TRACE("autosave filename: " << bufile);

	try
	{
		docSingleDocumentMgr::SaveDocument( bufile, false );
	}
	catch ( const envExceptionX& i_Ex )
	{
		std::string msg = "Problem autosaving scene, " + i_Ex.GetErrorMessage();
		DBG_ERROR(msg);
		guiMessageBox::Show(msg.c_str(), "Autosave Error", guiMessageBox::e_OKOnly);
	}	

	docSingleDocumentMgr::SetFilename( origfile );

	//	update the auto save variables
	//
	set_auto_save_time();
}

}	// eon


//------------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------------
void mnmAutoSaveMgr::Initialize(bool i_bActive,
								int i_FrequencyInMinutes,
								int i_NumBackups)
{
	Update( i_bActive, i_FrequencyInMinutes, i_NumBackups );

	set_auto_save_time();
}

//------------------------------------------------------------------------
// DeInitialize
//------------------------------------------------------------------------
void mnmAutoSaveMgr::DeInitialize()
{
}

//------------------------------------------------------------------------
//	Think() - check to see if the auto save systm should save a copy
//	of the scene.
//------------------------------------------------------------------------
void mnmAutoSaveMgr::Think()
{
	if ( !AutoSaveData::l_bAutoSaveActive )
	{
		return;
	}

	perform_auto_save();
}


//------------------------------------------------------------------------
//	ResetAutoSaveTimer() - this should get called when a new file is loaded.
//------------------------------------------------------------------------
void mnmAutoSaveMgr::ResetAutoSaveTimer()
{
	set_auto_save_time();

	AutoSaveData::l_AutoSaveBackupCounter = 0;
}

//------------------------------------------------------------------------
// Update - change the autosave values
//------------------------------------------------------------------------
void mnmAutoSaveMgr::Update(bool i_bActive,
							int i_FrequencyInMinutes,
							int i_NumBackups)
{
	AutoSaveData::l_bAutoSaveActive		= i_bActive;
	AutoSaveData::l_AutoSaveFrequencyInSeconds	= i_FrequencyInMinutes * 60.0f;
	AutoSaveData::l_AutoSaveBackups		= i_NumBackups;
}


//------------------------------------------------------------------------
//	BackupSavedFile() - back up the currently saved file.
//	(see docSingleDocMgr).  This could be called right BEFORE someone
//	saves the current scene to keep a "last good saved version".
//------------------------------------------------------------------------
void mnmAutoSaveMgr::BackupSavedFile()
{
	if ( !AutoSaveData::l_bAutoSaveActive )
	{
		return;
	}

	//	only perform if the security is present
	//
	// NOTE: uncommented here because mainCommands Save() function
	//	performs the security check before calling this function.
	//
	//if (!mnmSecurityMgr::CheckSecurity4())
	//{
	//	return;
	//}

	//	perform the copy (backup)
	//
	fsLocator origfile = docSingleDocumentMgr::GetFilename();
	if ( origfile.GetNumNames() == 0 )
	{
		return;
	}
	itString filename = origfile.GetLastName();
	
	fsLocator bufile;
	bufile = gfPaths::GetPath( mnmPaths::e_AutoSave );

	create_autosave_filename( filename );

	bufile.Push( filename );

	//DBG_LOG("autosave filename: " << bufile );

	if ( fsFileUtil::FileExists( origfile ) )
	{
		if ( fsFileUtil::FileExists( bufile ) )
		{
			//	delete the file, if it already exists
			//
			try
			{
				fsFileUtil::DeleteFile( bufile );
			}
			catch ( const envExceptionX& i_Ex )
			{
				std::string msg = "Problem backing up saved file, " + i_Ex.GetErrorMessage();
				DBG_ERROR(msg);
				guiMessageBox::Show(msg.c_str(), "Autosave Error", guiMessageBox::e_OKOnly);
			}	
		}
		else
		{
			//	check to make sure the backup directory exists
			//
			fsLocator budir( bufile );
			budir.Pop();
			if ( !fsFileUtil::DirectoryExists(budir) )
			{
				fsFileUtil::CreateDirectory(budir);
			}
		}

		//	now copy the original
		//
		fsFileUtil::CopyFile( origfile, bufile );

		//	make sure we set the read attribute to off.
		//
		if ( fsFileUtil::IsReadOnly( bufile ) )
		{
			fsFileUtil::SetReadOnly( bufile, false );
		}
	}
}
