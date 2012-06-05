/*****************************************************************************
**  mnmTimeCodeUtil.hpp
**
**      utility for time code display
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef MNM_TIMECODEUTIL_HPP
#error mnmTimeCodeUtil.hpp multiply included
#endif
#define MNM_TIMECODEUTIL_HPP

//============================================================================
//============================================================================
class g2dWindow;


//============================================================================
//============================================================================
namespace mnmTimeCodeUtil
{
	//------------------------------------------------------------------------
	// Set the MAIN window for the time code output
	//------------------------------------------------------------------------
	void SetMainWindow(g2dWindow* i_pWindow);
	g2dWindow* GetMainWindow();

	//------------------------------------------------------------------------
	// Set window for the time code output
	//
	//	if the SetWindow() is sent NULL then it reverts back to the main
	//	window
	//------------------------------------------------------------------------
	void SetWindow(g2dWindow* i_pWindow);
	g2dWindow* GetWindow();
}