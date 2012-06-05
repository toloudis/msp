/*****************************************************************************
**  muiPropertyDialog.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIManaged/mui/muiPropertyDialog.hpp"

#include "ToolUIManaged/tma/tmaPropertyDialog.hpp"

#include "Core/dbg/dbgAssert.hpp"

//--------------------------------------------------------------------
//	Show a modal dialog with the given properties. 
//	Returns a dialog result, OK or Cancel.
//--------------------------------------------------------------------
guiPropertyDialog::ReturnValue muiPropertyDialog::ShowModal(const char* i_DialogTitle, 
															const PropertyUIIList& i_List,
															const char* i_Message)
{

	return guiPropertyDialog::e_Cancel;
}

