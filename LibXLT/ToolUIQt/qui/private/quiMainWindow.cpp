/*****************************************************************************
**  quiMainWindow.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiMainWindow.hpp"

#include "Core/fs/fsLocator.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

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
	if (tqtSystem::g_pMainForm)
	{
		tqtSystem::g_pMainForm->setWindowTitle(QString::fromWCharArray(i_Text.GetString()));
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
