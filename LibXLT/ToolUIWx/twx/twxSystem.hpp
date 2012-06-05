/*****************************************************************************
**  twxSystem.hpp
**
**      Globals for current main form.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWX_SYSTEM_HPP
#error twxSystem.hpp multiply included
#endif
#define TWX_SYSTEM_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class twxSystem
{
public:
	static wxFrame *g_pMainForm;
	static wxMenuBar *g_pMainMenu;
	static wxStatusBar *g_pStatusBar;

	// number of menu items to leave on the right when adding 
	// new menus from subsystems
	static int g_nNumMenuOffset;
};

#endif // USE_WXWIDGETS
