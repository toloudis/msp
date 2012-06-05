/*****************************************************************************
**	quiMessageBox.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiMessageBox.hpp"
#include "ToolUIQt/tqt/tqtMessageBox.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/it/itString.hpp"


//===========================================================================
//	quiMessageBox functions
//===========================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int quiMessageBox::Show( const char* i_Message, const char* i_Title, int i_Type )
{
#ifdef USE_QT
	return tqtMessageBox::Show( i_Message, i_Title, i_Type );
#else // USE_QT
	DBG_ERROR("MessageBox: "<< i_Message);
	return guiMessageBox::e_Cancel;
#endif // USE_QT
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int quiMessageBox::Show( const itString& i_Message, const itString& i_Title, int i_Type )
{
#ifdef USE_QT
	return tqtMessageBox::Show( i_Message, i_Title, i_Type );
#else // USE_QT
	DBG_ERROR("MessageBox: "<< i_Message.GetString());
	return guiMessageBox::e_Cancel;
#endif // USE_QT
}
