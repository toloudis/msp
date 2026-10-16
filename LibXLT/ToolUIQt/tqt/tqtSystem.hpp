/*****************************************************************************
**  tqtSystem.hpp
**
**      Globals for current main form.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_SYSTEM_HPP
#error tqtSystem.hpp multiply included
#endif
#define TQT_SYSTEM_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

class tqcStatusBar;

//============================================================================
//============================================================================
class tqtSystem
{
public:
	static QMainWindow *g_pMainForm;
	static QMenuBar *g_pMainMenu;
	static tqcStatusBar *g_pStatusBar;

	// number of menu items to leave on the right when adding
	// new menus from subsystems
	static int g_nNumMenuOffset;
};

#endif // USE_QT
