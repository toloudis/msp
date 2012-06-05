/*****************************************************************************
**	ptclDialogUtil.hpp
**
**		API for opening a dialog
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef PTCL_DIALOGUTIL_HPP
#error ptclDialogUtil.hpp multiply included
#endif
#define PTCL_DIALOGUTIL_HPP


namespace ptclDialogUtil
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
	void ShowParticleDialog(bool i_bShow = true);

}	// end of namespace
