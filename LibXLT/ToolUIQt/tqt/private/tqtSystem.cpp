/*****************************************************************************
**  tqtSystem.cpp
**
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtSystem.hpp"

#ifdef USE_QT

QMainWindow *tqtSystem::g_pMainForm = NULL;
QMenuBar *tqtSystem::g_pMainMenu = NULL;
tqcStatusBar *tqtSystem::g_pStatusBar = NULL;

// number of menu items to leave on the right when adding
// new menus from subsystems
int tqtSystem::g_nNumMenuOffset = 0;

#endif // USE_QT
