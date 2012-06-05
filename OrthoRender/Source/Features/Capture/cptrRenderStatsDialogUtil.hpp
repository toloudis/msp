/*****************************************************************************
**	cptrRenderStatsDialogUtil.hpp
**
**		API for opening the capture options dialog
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERSTATSDIALOGUTIL_HPP
#error cptrRenderStatsDialogUtil.hpp multiply included
#endif
#define CPTR_RENDERSTATSDIALOGUTIL_HPP


//============================================================================
//============================================================================
namespace cptrRenderStatsDialogUtil
{
	//------------------------------------------------------------------------
	//  Show
	//------------------------------------------------------------------------
	void  Show();

	//------------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//------------------------------------------------------------------------
	void  Hide();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateStatsDialog();

}	// end of namespace
