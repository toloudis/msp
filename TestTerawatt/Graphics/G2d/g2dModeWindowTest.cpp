/*****************************************************************************
**  g2dModeWindowTest.cpp
**
**  See g2dModeWindowTest.hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "g2dModeWindowTest.hpp"

#include "appSimTime.hpp"
#include "fsLocator.hpp"
#include "g2dFontUtil.hpp"
#include "g2dImage.hpp"
#include "g2dImageCreate.hpp"
#include "g2dSystem.hpp"
#include "g2dWindow.hpp"

namespace
{
	int m_ThinkCount;
	g2dFontHandle m_FontLarge;
	int m_Frame;
	int m_X,m_Y;
	bool m_MoreText;
}

//--------------------------------------------------------------------
// Construction -- Inititialize the LandingSequenceScriptMgr
//--------------------------------------------------------------------
g2dModeWindowTest::g2dModeWindowTest(g2dSystem& i_System, g2dWindow& i_Window)
:	g2dMode(i_Window, 1), // we use exactly one debug string
	m_System(i_System), m_pSubWindow(NULL)
{
	m_X = 0;
	m_Y = 0;
	m_Frame = 0;
}

//--------------------------------------------------------------------
// Destruction -- cleanup LandingSequenceScriptMgr
//--------------------------------------------------------------------
g2dModeWindowTest::~g2dModeWindowTest()
{
}

//--------------------------------------------------------------------
//	The mode should do it's per frame "work" in the Think function.
//--------------------------------------------------------------------
void g2dModeWindowTest::Think()
{
	// base class
	g2dMode::Think();

	if (e_Continue != GetTerminateCondition() )
	{
		return; // we're terminated
	}

	// Do work here
	m_Frame++;

	// Subwindow test
	m_pSubWindow->Clear(g2dRGBColor(0x50, 0x20, 0x30));
	m_pSubWindow->DrawText(50, 50, m_FontLarge, itString("2"), g2dRGBColor(0xf0, 0x90, 0x20));
	m_pSubWindow->EndScene();

	// Main window regular stuff
	GetWindow().Clear(g2dRGBColor(0x20, 0x10, 0x50));

	itString text("Multiple window test");

	if( m_MoreText )
		this->SetDebugString(0, text);

	GetWindow().DrawText(20, 150, m_FontLarge, text, g2dRGBColor(0xf0, 0x90, 0x20));

	this->Render();
}

//--------------------------------------------------------------------
//	Initialize will be called before the first call of Think after
//	the object is first created or DeInitialized.  During the
//	lifetime of a mode, Initialize and DeInitialize may be called
//	several times.  Children of g2dModeAttract should remember to call
//	g2dModeAttract::Initialize() at the beginning of their Initialize
//	function.
//--------------------------------------------------------------------
void g2dModeWindowTest::Initialize()
{
	g2dMode::Initialize();

	// Do Initializing here
	m_FontLarge = g2dFontUtil::LoadFont(itString("Arial Unicode MS"), 64);

	//g2dScreen::SetGamma(0.5f);

	int width = 200, height = 200;
	int xpos = 200, ypos = 200;
	m_pSubWindow = m_System.CreateSubWindow(width, height, xpos, ypos);
}

//--------------------------------------------------------------------
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//--------------------------------------------------------------------
void g2dModeWindowTest::DeInitialize()
{
	// Do DeInitializing here
	if (m_pSubWindow)
	{
		m_System.DestroyWindow(m_pSubWindow);
		m_pSubWindow = NULL;
	}

	//	let the base deinitialize
	g2dMode::DeInitialize();
}
