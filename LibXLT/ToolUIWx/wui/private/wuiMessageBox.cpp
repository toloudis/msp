/*****************************************************************************
**	wuiMessageBox.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/wui/wuiMessageBox.hpp"
#include "ToolUIWx/twx/twxMessageBox.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/it/itString.hpp"


//===========================================================================
//	wuiMessageBox functions
//===========================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int wuiMessageBox::Show( const char* i_Message, const char* i_Title, int i_Type )
{
#ifdef USE_WXWIDGETS
	return twxMessageBox::Show( i_Message, i_Title, i_Type );
#else // USE_WXWIDGETS
	DBG_ERROR("MessageBox: "<< i_Message);
	return guiMessageBox::e_Cancel;
#endif // USE_WXWIDGETS
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int wuiMessageBox::Show( const itString& i_Message, const itString& i_Title, int i_Type )
{
#ifdef USE_WXWIDGETS
	return twxMessageBox::Show( i_Message, i_Title, i_Type );
#else // USE_WXWIDGETS
	DBG_ERROR("MessageBox: "<< i_Message.GetString());
	return guiMessageBox::e_Cancel;
#endif // USE_WXWIDGETS
}
