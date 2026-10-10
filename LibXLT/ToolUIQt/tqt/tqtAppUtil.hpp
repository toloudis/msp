/*****************************************************************************
**  tqtAppUtil.hpp
**
**     Help for integrating our WinMain functions with Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef TQT_APPUTIL_HPP
#error tqtAppUtil.hpp multiply included
#endif
#define TQT_APPUTIL_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

//============================================================================
//============================================================================
namespace tqtAppUtil
{
	//--------------------------------------------------------------------
	// Initializes Qt (creates the QApplication), returns true if
	//	successful. i_pAppName is the executable name given to Qt as
	//	argv[0]; our own command line is parsed separately.
	//--------------------------------------------------------------------
	bool Init(const char* i_pAppName);

	//--------------------------------------------------------------------
	// Runs the application loop
	//--------------------------------------------------------------------
	void Run();

	//--------------------------------------------------------------------
	// DeInitializes Qt
	//--------------------------------------------------------------------
	void CleanUp();
}

#endif // USE_QT
