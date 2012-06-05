/*****************************************************************************
**  wuiMainWindow.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/wui/wuiMainWindow.hpp"

#include "Core/fs/fsLocator.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#include <string>


//===========================================================================
//===========================================================================
namespace
{
	itString l_AppTitle("Application");
}


//===========================================================================
//	wuiMainWindow functions
//===========================================================================

//---------------------------------------------------------------------------
//	SetAppTitle()
//---------------------------------------------------------------------------
void wuiMainWindow::SetAppTitle( const itString& i_Text )
{
	l_AppTitle = i_Text;
#ifdef USE_WXWIDGETS
	if (twxSystem::g_pMainForm)
	{
		//twxSystem::g_pMainForm->SetTitle(wxString(i_Text, wxConvUTF8));
		twxSystem::g_pMainForm->SetTitle(i_Text.GetString());
	}
#endif
}

//---------------------------------------------------------------------------
//	GetAppTitle()
//---------------------------------------------------------------------------
itString wuiMainWindow::GetAppTitle()
{
	return l_AppTitle;
}

//---------------------------------------------------------------------------
//	give focus to the main window
//---------------------------------------------------------------------------
void wuiMainWindow::Focus()
{
}
