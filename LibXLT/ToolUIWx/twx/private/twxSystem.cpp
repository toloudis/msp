/*****************************************************************************
**  twxSystem.cpp
**
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twx/twxSystem.hpp"

#ifdef USE_WXWIDGETS

wxFrame *twxSystem::g_pMainForm = NULL;
wxMenuBar *twxSystem::g_pMainMenu = NULL;
wxStatusBar *twxSystem::g_pStatusBar = NULL;

// number of menu items to leave on the right when adding 
// new menus from subsystems
int twxSystem::g_nNumMenuOffset = 0;

#endif // USE_WXWIDGETS
