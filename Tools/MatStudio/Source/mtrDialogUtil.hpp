/*****************************************************************************
**	mtrDialogUtil.hpp
**
**	API for opening dialogs for channel editor
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef MTR_DIALOGUTIL_HPP
#error mtrDialogUtil.hpp multiply included
#endif
#define MTR_DIALOGUTIL_HPP


namespace mtrDialogUtil
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
	//--------------------------------------------------------------------
	void ShowMaterialDialog(bool i_bShow);

	//--------------------------------------------------------------------
	// When something outside of the material dialog changes the
	// material values, call this to update the dialog to the new data.
	//--------------------------------------------------------------------
	void UpdateMaterialDialog();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ShowLightsDialog(bool i_bShow);

}	// end of namespace
