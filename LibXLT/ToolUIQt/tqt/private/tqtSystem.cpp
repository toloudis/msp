/*****************************************************************************
**	tqtSystem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtSystem.hpp"


#ifdef USE_QT
QMainWindow	*tqtSystem::g_pMainForm = NULL;
QMenuBar	*tqtSystem::g_pMainMenu = NULL;
QStatusBar	*tqtSystem::g_pStatusBar = NULL;

// number of menu items to leave on the right when adding 
// new menus from subsystems
//int tqtSystem::g_nNumMenuOffset = 0;
#endif

