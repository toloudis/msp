/*****************************************************************************
**	dcutCueDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/


#ifdef DCUT_CUEDIALOGUTIL_HPP
#error dcutCueDialogUtil.hpp multiply included
#endif
#define DCUT_CUEDIALOGUTIL_HPP

class g2dSystem;

namespace dcutCueDialogUtil
{

	//--------------------------------------------------------------------
	// Init - needs the system for creating rendering views
	//--------------------------------------------------------------------
	void Init(g2dSystem *i_pSystem);

	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp();

	//--------------------------------------------------------------------
	// Show
	//--------------------------------------------------------------------
	void  Show();

	//--------------------------------------------------------------------
	// Update dialog for current timeline time
	//--------------------------------------------------------------------
	void UpdateDialog(float i_Time);

	//--------------------------------------------------------------------
	// UpdateCameraNames - call this when cameras are added/deleted
	//	or if a name has been changed
	//--------------------------------------------------------------------
	void  UpdateCameraNames();

}	// end of namespace
