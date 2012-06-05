/*****************************************************************************
**	mainDocumentChunk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/Data/mainDocumentChunk.hpp"
#include "MainApp/mainResolvePath.hpp"

#include "Features/MaterialBrowse/mbrwDialogUtil.hpp"
//#include "Features/Prefs/wxGUI/prefsLayoutMgr.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Support/rman/rmanMgr.hpp"
#include "Support/fbx/fbxMgr.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmAutoSaveMgr.hpp"
#include "Support/tmln/tmlnDriverDialogUtil.hpp"

#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/mat/matTextureTracking.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"


#include <assert.h>
#include <string>
#include <time.h>


//============================================================================
//============================================================================
namespace
{
	const chDefs::Name c_EXPV = chDefs::MakeName('E', 'X', 'P', 'V');	// executable version info

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetMenuItemsBasedOnFileLoaded()
	{
		//	if the isn't a document then disable menu item
		//
		if (guiSingleDocHandler::IsLoaded())
		{
			int mid = guiMenuMgr::GetMenuItemID("File","Revert");
			if (mid != -1)
				guiMenuMgr::MenuObjectsEnable(mid,true);
		}
		else
		{
			int mid = guiMenuMgr::GetMenuItemID("File","Revert");
			if (mid != -1)
				guiMenuMgr::MenuObjectsEnable(mid,false);
		}
	}

}

//============================================================================
// static variable
//============================================================================
mainDocumentChunk* mainDocumentChunk::sm_pActiveChunk = NULL;


//--------------------------------------------------------------------
// Return pointer to currently active chunk, may return NULL
//--------------------------------------------------------------------
mainDocumentChunk* mainDocumentChunk::GetActiveChunk()
{
	return sm_pActiveChunk;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mainDocumentChunk::mainDocumentChunk(std::string& i_ExecutableVersion)
: docDocumentChunk()
{
	m_ExecutableVersion = i_ExecutableVersion;
}

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void  mainDocumentChunk::Clear()
{
	//	if the document is clearing, remove the tabs for the dialogs
	//
	tmlnDriverDialogUtil::ClearDriverProperties();
	//tmaDialogTabbedMgr::RemoveTabPages("Object");

	// Clear tracked missing textures, restart tracking for new file
	matTextureTracking::Clear();

	// Clear tracking of absolute paths from the last file
	fsAbsolutePathMgr::Clear();

	// update mennu items
	SetMenuItemsBasedOnFileLoaded();
}

//--------------------------------------------------------------------
//  returns chunk name for this data type
//--------------------------------------------------------------------
chDefs::Name  mainDocumentChunk::GetChunkName() const
{
	return c_EXPV;
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual
const char* mainDocumentChunk::GetChunkDesc() const
{
	return "Main";
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
//virtual
void mainDocumentChunk::BuildDataList( std::vector<std::string>& o_List )
{
}

//--------------------------------------------------------------------
//	remove the items in this chunk by name.  each chunk built the
//	list using BuildDataList() so each chunk will know what to do
//	with this data.
//--------------------------------------------------------------------
//virtual
void mainDocumentChunk::RemoveItems( const std::vector<std::string>& i_List )
{
}


const int c_EXPV_VERSION = 1;
//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void  mainDocumentChunk::Read( chReader &i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size )
{
	//	check to see if the chunk is newer than the code supports
	//
	//	if the file hasn't already been flagged as a newer version
	//	(which the user would already have been notified of) and
	//	a newer version chunk has occurred
	//
	//if (!(i_Reader.IsNewerVersion()) && (i_Version > c_EXPV_VERSION))
	//{
	//	i_Reader.SetNewerVersion(true);
	//
	//	int retval = guiMessageBox::Show("This file was created with a newer version of the application.  Some of the data of the scene might be lost.  Do you want to continue to try and load this file?","Newer File Encountered", guiMessageBox::e_YesNo);
	//	if (retval == guiMessageBox::e_Yes)
	//	{
	//		// abort the reading, throw an exception?
	//	}
	//}

	// version 1 added save filename to chunk
	if (i_Version >= 1)
	{
		// Read in original saved filename. We can then compare this
		// locator to the file being loaded now and look for 
		// path differences.
		fsLocator org_locator;
		itString savefilename;
		chChunkParserUtil::Read( i_Reader, savefilename );
		fsFileUtil::UnicodeStringToLocator( savefilename, org_locator );
		fsAbsolutePathMgr::AddDirMappingFromFiles(org_locator, i_Reader.GetLocator());
	}
}

//--------------------------------------------------------------------
//  Write chunk data
//--------------------------------------------------------------------
void mainDocumentChunk::Write(chWriter &i_Writer, bool i_bResetDirtyFlag)
{
	char date_string[64];
	char time_string[64];
	_strdate(date_string);
	_strtime(time_string);

	i_Writer.WriteChunkHeader(c_EXPV, c_EXPV_VERSION, false);

	// Write saved filename in order to resolve absolute paths when read from
	// a new path.
	itString savefilename;
	fsFileUtil::LocatorToUnicodeString( i_Writer.GetLocator(), savefilename );
	chChunkParserUtil::Write( i_Writer, savefilename );

	// Write as strings so readable from bin viewer
	//
	i_Writer.Write(m_ExecutableVersion);
	i_Writer.Write(date_string);
	i_Writer.Write(time_string);

	i_Writer.FinishChunk();

	//bga - I am not confortable saving to a new filename while we
	// are in the middle of a write of the scene file. I think this
	// code should be elsewhere. I am going to move it to mainCommands
	// into the Save and Save As handlers. But, do we need to also
	// connect up to the AutoSaveMgr?
	//
	// We can use this as an opportunity to save preferences also
//#ifdef USE_WXWIDGETS
//	prefsLayoutMgr::SaveLastLayout();
//#endif
}

//--------------------------------------------------------------------
//  Return true if the chunk has been modified since the last
// call to Write()
//--------------------------------------------------------------------
bool  mainDocumentChunk::IsDirty() const
{
	return false;
}

//--------------------------------------------------------------------
//  This chunk is currently made active, add data to the scene
// or world
//--------------------------------------------------------------------
void  mainDocumentChunk::SetActive()
{
	sm_pActiveChunk = this;
}

//--------------------------------------------------------------------
//  this chunk is currently being made in active, remove data
// from scene or world
//--------------------------------------------------------------------
void  mainDocumentChunk::SetInactive( bool i_bUpdateData )
{
	if (sm_pActiveChunk == this) sm_pActiveChunk = NULL;
}

//--------------------------------------------------------------------
//  This virtual function is called on all chunks after all 
//	chunks have finished loading successfully
//--------------------------------------------------------------------
void  mainDocumentChunk::NotifyLoadFinished()
{
	if (sm_pActiveChunk == this)
	{
		// Update interface
		mnmAppUtil::UpdateTitleBar( false );
		//updateTimelineRange();
		
		mnmAutoSaveMgr::ResetAutoSaveTimer();

		cmmSystemDialogUtil::UpdateDialog();

		SetMenuItemsBasedOnFileLoaded();

		// Clears the "Skip All" flag that might have been set during load of the file.
		mainResolvePath::LoadFinished();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mainDocumentChunk::SetupPaths()
{
}

//--------------------------------------------------------------------
// Callback when dialog changes data, sets dirty bit
//--------------------------------------------------------------------
void mainDocumentChunk::DataChanged()
{
}
