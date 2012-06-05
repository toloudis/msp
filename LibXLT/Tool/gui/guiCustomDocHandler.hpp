/*****************************************************************************
**	guiCustomDocHandler.hpp
**
**		Handles calls from the File menu, presenting message and
**	file dialogs as needed.
**
**		This API is used for custom document formats that don't 
**	use DocumentChunks.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_CUSTOMDOCHANDLER_HPP
#error guiCustomDocHandler.hpp multiply included
#endif
#define GUI_CUSTOMDOCHANDLER_HPP

#ifndef FS_ABSOLUTEPATHMGR_HPP
#include "Core/fs/fsAbsolutePathMgr.hpp"
#endif


//============================================================================
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
namespace guiCustomDocHandler
{
	//--------------------------------------------------------------------
	// SaveIfDirty - ask user to save document if it is dirty.
	//		Returns false if User answered "Cancel"
	//--------------------------------------------------------------------
	bool SaveIfDirty();

	//--------------------------------------------------------------------
	// New
	//--------------------------------------------------------------------
	void  New();

	//--------------------------------------------------------------------
	// Open
	//--------------------------------------------------------------------
	void  Open(bool i_bSkipErrorDialog = false);

	//--------------------------------------------------------------------
	// Open - called from MRU
	//--------------------------------------------------------------------
	void  Open(const fsLocator &i_Locator, bool i_bSkipErrorDialog = false);

	//--------------------------------------------------------------------
	// Save
	//--------------------------------------------------------------------
	void  Save();

	//--------------------------------------------------------------------
	// SaveAs
	//--------------------------------------------------------------------
	void  SaveAs();

	//--------------------------------------------------------------------
	// Exit
	//--------------------------------------------------------------------
	void  Exit();

	//--------------------------------------------------------------------
	//	the initial directory Open() starts in
	//--------------------------------------------------------------------
	void SetInitialDirectory( const fsLocator& i_InitialDir );

	//--------------------------------------------------------------------
	//	set a flag so the initial directory for Open() starts in
	//	the last directory the user picked.
	//--------------------------------------------------------------------
	void SetInitialDirectoryToBeLast( bool i_bRememberLastDir );

}	// end of namespace
