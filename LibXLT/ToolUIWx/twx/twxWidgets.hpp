/*****************************************************************************
**  twxWidgets.hpp
**
**     Wrapper for the main header of wxWidgets. Also provides the
**	preprocessor define for turning wxWidgets on and off.
**
**	StudioGPU
**	Copyright(C) 2008-9 - All Rights Reserved
\****************************************************************************/

#ifdef TWX_WIDGETS_HPP
#error twxWidgets.hpp multiply included
#endif
#define TWX_WIDGETS_HPP


// MSP_GUI_QT is set when a project builds the Qt GUI instead (see
// ToolUIQt/qt6.props); wxWidgets stays off there so the wx GUI code
// compiles out.
#if !defined(BATCH_MODE) && !defined(MSP_GUI_QT)
// This define activates Library usage of wxWidgets
#define USE_WXWIDGETS 1
#endif

//#define WXUSINGDLL
//#define wxMSVC_VERSION_AUTO
#ifdef USE_WXWIDGETS

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include <wx/wx.h>
#include <wx/progdlg.h>

#undef GetOpenFileName
#undef GetSaveFileName
#undef SetCurrentDirectory

// wxWidgets needs these libraries also:
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"rpcrt4.lib")
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"odbc32.lib")
#endif // USE_WXWIDGETS


