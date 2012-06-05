/*****************************************************************************
**	quiMainWindow.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiMainWindow.hpp"

#include "Core/env/envString.hpp"
#include "Core/fs/fsLocator.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

#include <QtGui/QMainWindow>

#include <string>


//===========================================================================
//===========================================================================
namespace
{
	itString l_AppTitle("Application");
}


//===========================================================================
//	quiMainWindow functions
//===========================================================================

//---------------------------------------------------------------------------
//	SetAppTitle()
//---------------------------------------------------------------------------
void quiMainWindow::SetAppTitle( const itString& i_Text )
{
	l_AppTitle = i_Text;
#ifdef USE_QT
	if (tqtSystem::g_pMainForm != NULL)
	{
		std::string title_utf8 = envString::WideCharToUTF8( i_Text.GetString(), i_Text.GetLength() );
		tqtSystem::g_pMainForm->setWindowTitle(title_utf8.c_str());
	}
#endif
}

//---------------------------------------------------------------------------
//	GetAppTitle()
//---------------------------------------------------------------------------
itString quiMainWindow::GetAppTitle()
{
	return l_AppTitle;
}

//---------------------------------------------------------------------------
//	give focus to the main window
//---------------------------------------------------------------------------
void quiMainWindow::Focus()
{
}

