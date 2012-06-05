/*****************************************************************************
**  muiMessageBox.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIManaged/mui/muiMessageBox.hpp"
#include "ToolUIManaged/tma/tmaMessageBox.hpp"

#include "Core/dbg/dbgLog.hpp"

//===========================================================================
//	muiMessageBox functions
//===========================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int muiMessageBox::Show( const char* i_Message, const char* i_Title, int i_Type )
{
	DBG_ERROR1("MessageBox: %s", i_Message);
	return guiMessageBox::e_Cancel;
}
