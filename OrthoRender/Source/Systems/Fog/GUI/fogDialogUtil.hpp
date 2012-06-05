/*****************************************************************************
**	fogDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef FOG_DIALOGUTIL_HPP
#error fogDialogUtil.hpp multiply included
#endif
#define FOG_DIALOGUTIL_HPP




class fogDialogUtil
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
