/*****************************************************************************
**	ImportDialogUtil.hpp
**
**		API for Import dialog
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef IMPORTDIALOGUTIL_HPP
#error ImportDialogUtil.hpp multiply included
#endif
#define IMPORTDIALOGUTIL_HPP

#ifndef IMPORTDATA_HPP
#include "Features/Import/ImportData.hpp"
#endif


namespace ImportDialogUtil
{
	//------------------------------------------------------------------------
	//  Show
	//------------------------------------------------------------------------
	void  Show();

	//------------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//------------------------------------------------------------------------
	void  Hide();

}	// end of namespace
