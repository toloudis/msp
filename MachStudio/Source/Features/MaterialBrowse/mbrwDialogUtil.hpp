/*****************************************************************************
**	pythDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef PYTH_DIALOGUTIL_HPP
#error pythDialogUtil.hpp multiply included
#endif
#define PYTH_DIALOGUTIL_HPP

class fsLocator;

namespace mbrwDialogUtil
{

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init();

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp();

	//--------------------------------------------------------------------
	// ShowMaterialBrowseDialog 
	//--------------------------------------------------------------------
	void  ShowMaterialBrowseDialog();

	//--------------------------------------------------------------------
	// Set home directory for material browsing
	//--------------------------------------------------------------------
	void SetInitialDirectory(const fsLocator& i_MaterialHomeDir);
	fsLocator GetInitialDirectory();

	//--------------------------------------------------------------------
	// Refresh Icons from the current directory, called when
	// new materials are exported.
	//--------------------------------------------------------------------
	void RefreshIcons();

}	// end of namespace
