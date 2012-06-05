/*****************************************************************************
**	guiMessageBox.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiMessageBox.hpp"

#include "Core/dbg/dbgMsg.hpp"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int guiMessageBox::Show( const char* i_Message, const char* i_Title, int i_Type )
{
	if (sm_pImplementation)
	{
		return sm_pImplementation->Show(i_Message, i_Title, i_Type);
	}
	else return 0;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int guiMessageBox::Show( const itString& i_Message, const itString& i_Title, int i_Type )
{
	if (sm_pImplementation)
	{
		return sm_pImplementation->Show(i_Message, i_Title, i_Type);
	}
	else return 0;
}
