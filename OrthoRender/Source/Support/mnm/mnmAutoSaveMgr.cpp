/*****************************************************************************
**  mnmAutoSaveMgr.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/mnm/mnmAutoSaveMgr.hpp"

#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmSecurityMgr.hpp"

//	tools
#include "Tool/gui/guiMessageBox.hpp"

//	library
#include "Core/app/appTime.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"

#include <assert.h>
#include <string>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace AutoSaveData
{
	bool	l_bAutoSaveActive = true;
	int		l_AutoSaveFrequency;
	int		l_AutoSaveBackups;
	int		l_AutoSaveBackupCounter;
	float	l_fLastAutoSave;
}

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
	char extras[ 4 ];
	sprintf( extras, "~%02d.mab", AutoSaveData::l_AutoSaveBackupCounter );
	io_filename += itString( extras );

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

	if ( (AutoSaveData::l_fLastAutoSave + (float)(AutoSaveData::l_AutoSaveFrequency)) > appTime::GetTime() )
	{
		return;
	}

	//DBG_LOG3( "last %6.2f  freq %d  time=%6.2f", AutoSaveData::l_fLastAutoSave, AutoSaveData::l_AutoSaveFrequency, appTime::GetTime() );

	//	perform the save
	//
	fsLocator origfile = docSingleDocumentMgr::GetFilename();
	itString filename = (origfile.GetNumNames() > 0) ? origfile.GetLastName() : itString("Untitled");
	
	fsLocator bufile;
	bufile = gfPaths::GetPath( mnmPaths::e_AutoSave );

	create_autosave_filename( filename );

	bufile.Push( filename );

	//  debug output
	//
	//std::string StrLocF;
	//fsFileUtil::LocatorToANSIFilename(bufile, StrLocF);
	//DBG_LOG1("autosave filename: %s", StrLocF.c_str() );

	try
	{
		docSingleDocumentMgr::SaveDocument( bufile, false );
	}
	catch (const fsReadOnlyX& i_Ex)
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		std::string msg = "Auto-Save file is read only: " + filename + "app will continue";
		guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
	}
	catch (const fsDirectoryDoesntExistX&)
	{
		fsLocator newdir(bufile);
		newdir.Pop();
		fsFileUtil::CreateDirectory(newdir);

		try
		{
			docSingleDocumentMgr::SaveDocument( bufile, false );
		}
		catch( const fsDiskFullX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			DBG_LOG1("fsDiskFullX: %s", filename.c_str());

			std::string msg = "Out of disk space or the disk is corrupt.  Could not write " + filename;
			DBG_ERROR1("%s", msg.c_str() );
			guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
			assert(false);
		}

	}
	catch( const fsDiskFullX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		DBG_LOG1("fsDiskFullX: %s", filename.c_str());

		std::string msg = "Out of disk space or the disk is corrupt.  Could not write " + filename;
		DBG_ERROR1("%s", msg.c_str() );
		assert(false);
		//MessageBox::Show(msg.c_str(), "Error");
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
								int i_FrequencyInSeconds,
								int i_NumBackups)
{
	AutoSaveData::l_bAutoSaveActive		= i_bActive;
	AutoSaveData::l_AutoSaveFrequency	= i_FrequencyInSeconds;
	AutoSaveData::l_AutoSaveBackups		= i_NumBackups;

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

	//	only perform the auto-save if the dongle is present
	if (mnmSecurityMgr::CheckForDongle())
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
//	BackupSavedFile() - back up the currently saved file.
//	(see docSingleDocMgr).  This could be called right BEFORE someone
//	saves the current scene to keep a "last good saved version".
//------------------------------------------------------------------------
void mnmAutoSaveMgr::BackupSavedFile()
{
	if ( !AutoSaveData::l_bAutoSaveActive )
		return;

	//	only perform if the dongle is present
	if (!mnmSecurityMgr::CheckForDongle())
		return;

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

	//  debug output
	//
	std::string StrLocF;
	fsFileUtil::LocatorToANSIFilename(bufile, StrLocF);
	DBG_LOG1("autosave filename: %s", StrLocF.c_str() );

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
			catch (const fsFileDoesntExistX& i_Ex)
			{
				//	do nothing, just continue
				i_Ex;
			}
			catch (const fsReadOnlyX& i_Ex)
			{
				i_Ex;
				char msg[256];
				std::string filename;
				fsFileUtil::LocatorToANSIFilename( bufile, filename );
				sprintf( msg, "Cannot save back-up of (%s) because read only.", filename.c_str() );
				guiMessageBox::Show( msg, "Read Only File" , guiMessageBox::e_OKOnly);
				return;
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
