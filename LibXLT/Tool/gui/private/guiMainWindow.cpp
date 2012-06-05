/*****************************************************************************
**	guiMainWindow.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiMainWindow.hpp"

#include <string>


//===========================================================================
//===========================================================================
namespace
{
	itString l_AppTitle(L"Application");
}


//===========================================================================
//	guiMainWindow functions
//===========================================================================

//---------------------------------------------------------------------------
//	SetAppTitle()
//---------------------------------------------------------------------------
void guiMainWindow::SetAppTitle( const itString& i_Text )
{
	DBG_ASSERT(sm_pImplementation, "guiMainWindow: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->SetAppTitle(i_Text);
	}

	l_AppTitle = i_Text;
}

//---------------------------------------------------------------------------
//	GetAppTitle()
//---------------------------------------------------------------------------
itString guiMainWindow::GetAppTitle()
{
	DBG_ASSERT(sm_pImplementation, "guiMainWindow: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->GetAppTitle();
	}

	return l_AppTitle;
}

//---------------------------------------------------------------------------
//	give focus to the main window
//---------------------------------------------------------------------------
void guiMainWindow::Focus()
{
	DBG_ASSERT(sm_pImplementation, "guiMainWindow: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->Focus();
	}
}
