/*****************************************************************************
**	tqtWidgets.hpp
**
**	Wrapper for the main header of Qt. Also provides the
**	preprocessor define for turning Qt on and off.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_WIDGETS_HPP
#error tqtWidgets.hpp multiply included
#endif
#define TQT_WIDGETS_HPP

#ifndef BATCH_MODE
// This define activates Library usage of Qt
#define USE_QT 1
#endif


#ifdef USE_QT

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
//#include <wx/wx.h>
//#include <wx/progdlg.h>
//
//#undef GetOpenFileName
//#undef GetSaveFileName
//#undef SetCurrentDirectory

// Qt needs these libraries:
#ifdef _DEBUG
#pragma comment(lib,"QtCored4.lib")
#pragma comment(lib,"QtGuid4.lib")
#pragma comment(lib,"QtMainD.lib")
#else
#pragma comment(lib,"QtCore4.lib")
#pragma comment(lib,"QtGui4.lib")
#pragma comment(lib,"QtMain.lib")
#endif
//#pragma comment(lib,"comctl32.lib")
//#pragma comment(lib,"rpcrt4.lib")
//#pragma comment(lib,"wsock32.lib")
//#pragma comment(lib,"odbc32.lib")
//opengl32.lib glu32.lib gdi32.lib user32.lib QtOpenGLd4.lib QtGuid4.lib QtCored4.lib qtmaind.lib
#endif // USE_QT


