/*****************************************************************************
**  mnmTimeCodeUtilWin.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/mnm/mnmTimeCodeUtil.hpp"

#include "Graphics/g2d/g2dWindow.hpp"


namespace TCUData
{
	g2dWindow *l_pMainWindow = NULL;
	g2dWindow *l_pWindow = NULL;
}


//------------------------------------------------------------------------
// Set the main window for debug display, called from app
//------------------------------------------------------------------------
void mnmTimeCodeUtil::SetMainWindow(g2dWindow* i_pWindow)
{
	TCUData::l_pMainWindow = i_pWindow;
}

//------------------------------------------------------------------------
// Set the main window for debug display, called from app
//------------------------------------------------------------------------
g2dWindow* mnmTimeCodeUtil::GetMainWindow()
{
	return TCUData::l_pMainWindow;
}

//------------------------------------------------------------------------
// Set window for debug display, called from app
//------------------------------------------------------------------------
void mnmTimeCodeUtil::SetWindow(g2dWindow* i_pWindow)
{
	if ( i_pWindow != 0 )
	{
		TCUData::l_pWindow = i_pWindow;
	}
	else
	{
		TCUData::l_pWindow = TCUData::l_pMainWindow;
	}
}

//------------------------------------------------------------------------
// Set window for debug display, called from app
//------------------------------------------------------------------------
g2dWindow* mnmTimeCodeUtil::GetWindow()
{
	return TCUData::l_pWindow;
}
