/*****************************************************************************
**	pythDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef PYTH_DIALOGUTIL_HPP
#error pythDialogUtil.hpp multiply included
#endif
#define PYTH_DIALOGUTIL_HPP

class dynScriptObject;

namespace pythDialogUtil
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
	// ShowPythonDialog - display the python dialog
	//--------------------------------------------------------------------
	void  ShowPythonDialog();


}	// end of namespace
