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

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif 

#include <windows.h>


//============================================================================
//	Forward Declarations
//============================================================================
class wxMainForm;


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
#endif
};
