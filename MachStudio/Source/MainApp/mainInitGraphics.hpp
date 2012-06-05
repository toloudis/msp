/****************************************************************************\
**	mainInitGraphics.hpp
**
**		Object that initializes the graphics for our application on
**	constructor and then closes them down on destructor.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MAIN_INITGRAPHICS_HPP
#error mainInitGraphics.hpp multiply included
#endif
#define MAIN_INITGRAPHICS_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif 


//============================================================================
//	Forward References
//============================================================================
class g2dSystem;
class mnmApp;
class wxMainForm;


//============================================================================
//============================================================================
class mainInitGraphics
{
public:
	//--------------------------------------------------------------------
	// Initializes library packages
	//--------------------------------------------------------------------
#ifdef USE_WXWIDGETS
	mainInitGraphics(wxMainForm *i_pMainFrame);
#else // USE_WXWIDGETS
	mainInitGraphics(int i_AppWidth = 800,
					 int i_AppHeight = 600,
					 int i_ViewOffsetX = 0,
					 int i_ViewOffsetY = 0);
#endif // USE_WXWIDGETS

	//--------------------------------------------------------------------
	// DeInitializes library packages
	//--------------------------------------------------------------------
	~mainInitGraphics();

	//--------------------------------------------------------------------
	// Returns the Graphics system that was created
	//--------------------------------------------------------------------
	g2dSystem* GetSystem();

	//--------------------------------------------------------------------
	// Return true if the graphics system was initilaized succesfully.
	// This will return false when the system cannot run because of
	// insufficient graphics power.
	//--------------------------------------------------------------------
	bool InitSuccessful() { return m_bSuccessInit; }

private:
	mnmApp *m_pApp;
	bool m_bSuccessInit;
};
