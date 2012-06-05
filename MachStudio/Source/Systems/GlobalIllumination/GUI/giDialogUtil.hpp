/*****************************************************************************
**	giDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/


#ifdef GI_DIALOGUTIL_HPP
#error giDialogUtil.hpp multiply included
#endif
#define GI_DIALOGUTIL_HPP




class giDialogUtil
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
