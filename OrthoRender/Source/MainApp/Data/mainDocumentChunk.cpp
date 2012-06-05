/*****************************************************************************
**	mainDocumentChunk.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/Data/mainDocumentChunk.hpp"

#include "Features/Prefs/wxGUI/prefsLayoutMgr.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmAutoSaveMgr.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/mat/matTextureTracking.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"

#include <assert.h>
#include <string>
#include <time.h>


//============================================================================
//============================================================================
namespace
{
	const chDefs::Name c_EXPV = chDefs::MakeName('E', 'X', 'P', 'V');

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
//WXGUI
/*
		guiDialogTabbedMgr::RemoveTabPages("Driver");
	//tmaDialogTabbedMgr::RemoveTabPages("Object");
*/

	// Clear tracked missing textures, restart tracking for new file
	matTextureTracking::Clear();

	// update mennu items
//WXGUI
/*
	SetMenuItemsBasedOnFileLoaded();
*/
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

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void  mainDocumentChunk::Read( chReader &i_Reader,
								 chDefs::Version i_Version,
								 chDefs::Size i_Size )
{
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

	// Write as strings so readable from bin viewer
	//
	i_Writer.WriteChunkHeader(c_EXPV, 0, false);
	i_Writer.Write(m_ExecutableVersion);
	i_Writer.Write(date_string);
	i_Writer.Write(time_string);
	i_Writer.FinishChunk();

	// We can use this as an opportunity to save preferences also
#ifdef USE_WXWIDGETS
//	prefsLayoutMgr::SaveLastLayout();
#endif
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
