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
	std::string l_AppTitle("Application");
}


//===========================================================================
//	wuiMainWindow functions
//===========================================================================

//---------------------------------------------------------------------------
//	SetAppTitle()
//---------------------------------------------------------------------------
void wuiMainWindow::SetAppTitle( const std::string& i_Text )
{
	l_AppTitle = i_Text;
#ifdef USE_WXWIDGETS
	if (twxSystem::g_pMainForm)
	{
		//twxSystem::g_pMainForm->SetTitle(wxString(i_Text, wxConvUTF8));
		twxSystem::g_pMainForm->SetTitle(wxString::FromUTF8(i_Text));
	}
#endif
}

//---------------------------------------------------------------------------
//	GetAppTitle()
//---------------------------------------------------------------------------
std::string wuiMainWindow::GetAppTitle()
{
	return l_AppTitle;
}

//---------------------------------------------------------------------------
//	give focus to the main window
//---------------------------------------------------------------------------
void wuiMainWindow::Focus()
{
}
