/*****************************************************************************
**	todDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef TOD_DIALOGUTIL_HPP
#error todDialogUtil.hpp multiply included
#endif
#define TOD_DIALOGUTIL_HPP




namespace todDialogUtil
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
	//  Create tab page for tod properties
	//--------------------------------------------------------------------
	void  CreateTimeOfDayTabPage();

	//--------------------------------------------------------------------
	//  Show dialog to edit tod
	//--------------------------------------------------------------------
	void  ShowTimeOfDayDialog();


}	// end of namespace
