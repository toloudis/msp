/*****************************************************************************
**	rcdFloatDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef RCD_FLOATDIALOGUTIL_HPP
#error rcdFloatDialogUtil.hpp multiply included
#endif
#define RCD_FLOATDIALOGUTIL_HPP

class rcdDriverFloat;

namespace rcdFloatDialogUtil
{

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init();

	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp();

	//--------------------------------------------------------------------
	// Show
	//--------------------------------------------------------------------
	void  Show(rcdDriverFloat &i_Driver);

	//--------------------------------------------------------------------
	// Update dialog for current timeline time
	//--------------------------------------------------------------------
	void UpdateDialog(float i_Time);

}	// end of namespace
