/*****************************************************************************
**  g2dModeBaseTest.hpp
**
**  g2dModeBaseTest is a sample mode that displays some text on a black background
**  just to demonstrate how you might build one.  Cut & Paste me.  :)
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_MODEBASETEST_HPP
#error g2dModeBaseTest.hpp multiply included
#endif
#define G2D_MODEBASETEST_HPP

#ifndef G2D_MODE_HPP
#include "g2dMode.hpp"
#endif

class g2dModeBaseTest : public g2dMode
{
	public:

		//====================================================================
		// Construction
		//====================================================================
		g2dModeBaseTest(g2dWindow& i_Window);

		//====================================================================
		// Destruction
		//====================================================================
		virtual ~g2dModeBaseTest();

		//====================================================================
		//	The mode should do it's per frame "work" in the Think function.
		//====================================================================
		virtual void Think();

		//====================================================================
		//	Initialize will be called before the first call of Think after
		//	the object is first created or DeInitialized.  During the
		//	lifetime of a mode, Initialize and DeInitialize may be called
		//	several times.  Children of g2dModeBaseTest should remember to call
		//	g2dModeBaseTest::Initialize() at the beginning of their Initialize
		//	function.
		//====================================================================
		virtual void Initialize();

		//====================================================================
		//	The object should clean up things that are not needed while the
		//	mode is not running in the DeInitialize function.
		//====================================================================
		virtual void DeInitialize();
		
};
