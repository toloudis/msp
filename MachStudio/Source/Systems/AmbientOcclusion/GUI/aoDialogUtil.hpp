/*****************************************************************************
**	aoDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef AO_DIALOGUTIL_HPP
#error aoDialogUtil.hpp multiply included
#endif
#define AO_DIALOGUTIL_HPP




class aoDialogUtil
{
public:
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	static void  Init();

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	static void  CleanUp();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void UpdateListDialog();

};	// end of namespace
