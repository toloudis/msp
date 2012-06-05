/*****************************************************************************
**  g2dModeWindowTest.hpp
**
**  g2dModeWindowTest is a sample mode that displays some text on a black background
**  just to demonstrate how you might build one.  Cut & Paste me.  :)
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_MODEWINDOWTEST_HPP
#error g2dModeWindowTest.hpp multiply included
#endif
#define G2D_MODEWINDOWTEST_HPP

#ifndef G2D_MODE_HPP
#include "g2dMode.hpp"
#endif

class g2dSystem;

class g2dModeWindowTest : public g2dMode
{
	public:

		//====================================================================
		// Construction
		//====================================================================
		g2dModeWindowTest(g2dSystem &i_System, g2dWindow& i_Window);

		//====================================================================
		// Destruction
		//====================================================================
		virtual ~g2dModeWindowTest();

		//====================================================================
		//	The mode should do it's per frame "work" in the Think function.
		//====================================================================
		virtual void Think();

		//====================================================================
		//	Initialize will be called before the first call of Think after
		//	the object is first created or DeInitialized.  During the
		//	lifetime of a mode, Initialize and DeInitialize may be called
		//	several times.  Children of g2dModeWindowTest should remember to call
		//	g2dModeWindowTest::Initialize() at the beginning of their Initialize
		//	function.
		//====================================================================
		virtual void Initialize();

		//====================================================================
		//	The object should clean up things that are not needed while the
		//	mode is not running in the DeInitialize function.
		//====================================================================
		virtual void DeInitialize();
		
private:
	g2dSystem &m_System;
	g2dWindow* m_pSubWindow;
};
