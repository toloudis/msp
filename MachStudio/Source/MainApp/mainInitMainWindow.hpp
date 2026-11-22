/****************************************************************************\
**	mainInitMainWindow.hpp
**
**		Object that initializes main frame for our windowed application on
**	constructor and then closes the frame on destructor.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MAIN_INITMAINWINDOW_HPP
#error mainInitMainWindow.hpp multiply included
#endif
#define MAIN_INITMAINWINDOW_HPP

#ifdef MSP_GUI_QT
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif
#else
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif 
#endif

#include <windows.h>


//============================================================================
//	Forward Declarations
//============================================================================
class wxMainForm;
class qtMainForm;


//============================================================================
//============================================================================
class mainInitMainWindow
{
public:
	//--------------------------------------------------------------------
	// Creates main window
	//--------------------------------------------------------------------
	mainInitMainWindow();

	//--------------------------------------------------------------------
	// Clean up main window
	//--------------------------------------------------------------------
	~mainInitMainWindow();
	
#ifdef USE_WXWIDGETS
	//--------------------------------------------------------------------
	// Return pointer to main frame created
	//--------------------------------------------------------------------
	wxMainForm* GetMainFrame();

private:
	// The main application window
	wxMainForm *m_pMainFrame;
#elif defined(USE_QT)
	//--------------------------------------------------------------------
	// Return pointer to main frame created
	//--------------------------------------------------------------------
	qtMainForm* GetMainFrame();

	//--------------------------------------------------------------------
	// Delete the main frame and its panes once the application loop has
	// ended (wxWidgets destroyed them inside its loop).
	//--------------------------------------------------------------------
	void DestroyMainFrame();

private:
	// The main application window
	qtMainForm *m_pMainFrame;
#endif
};
