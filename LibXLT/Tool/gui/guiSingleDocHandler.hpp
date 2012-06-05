/*****************************************************************************
**	guiSingleDocHandler.hpp
**
**		Handles calls from the File menu, presenting message and
**	file dialogs as needed 
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_SINGLEDOCHANDLER_HPP
#error guiSingleDocHandler.hpp multiply included
#endif
#define GUI_SINGLEDOCHANDLER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef FS_ABSOLUTEPATHMGR_HPP
#include "Core/fs/fsAbsolutePathMgr.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class fsLocator;
class chReader;


//============================================================================
//============================================================================
namespace guiSingleDocHandler
{
	//--------------------------------------------------------------------
	// SaveIfDirty - ask user to save document if it is dirty.
	//		Returns false if User answered "Cancel"
	//--------------------------------------------------------------------
	bool SaveIfDirty();

	//--------------------------------------------------------------------
	// New
	//--------------------------------------------------------------------
	void New();

	//--------------------------------------------------------------------
	// Open
	//--------------------------------------------------------------------
	void Open(bool i_bSkipErrorDialog = false);

	//--------------------------------------------------------------------
	// Open - called from MRU
	//--------------------------------------------------------------------
	void Open(const fsLocator &i_Filename, 
				bool i_bSkipSaveCheck = false,
				bool i_bSkipErrorDialog = false);

	//--------------------------------------------------------------------
	// Save
	//--------------------------------------------------------------------
	void Save();

	//--------------------------------------------------------------------
	// SaveAs
	//--------------------------------------------------------------------
	void SaveAs();

	//--------------------------------------------------------------------
	// Exit
	//--------------------------------------------------------------------
	void Exit();

	//--------------------------------------------------------------------
	//	the initial directory Open() starts in
	//--------------------------------------------------------------------
	void SetInitialDirectory( const fsLocator& i_InitialDir );

	//--------------------------------------------------------------------
	//	set a flag so the initial directory for Open() starts in
	//	the last directory the user picked.
	//--------------------------------------------------------------------
	void SetInitialDirectoryToBeLast( bool i_bRememberLastDir );

	//--------------------------------------------------------------------
	//	is there a document loaded?
	//--------------------------------------------------------------------
	bool IsLoaded();

	//--------------------------------------------------------------------
	//	This function will check the versions passed in and if they
	//	are different it will ask the user to abort to conitnue. If the
	//	user continues, it won't ask for other newer chunks found. If
	//	the user aborts, it will throw an exception.
	//
	//	throws: fsNewerVersionAbortX
	//--------------------------------------------------------------------
	void CheckForNewerChunkVersion( chReader& i_Reader,
									const chDefs::Version i_ReadVersion,
									const chDefs::Version i_ExpectedVersion );

}	// end of namespace
