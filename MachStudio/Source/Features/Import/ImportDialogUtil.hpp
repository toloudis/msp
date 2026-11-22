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

#include <string>


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

	//------------------------------------------------------------------------
	//  The dialog's file filter: .mab and every scene file format
	//	("Scene files (*.mab;<patterns>)|*.mab;<patterns>")
	//------------------------------------------------------------------------
	std::string FileFilter();

}	// end of namespace
