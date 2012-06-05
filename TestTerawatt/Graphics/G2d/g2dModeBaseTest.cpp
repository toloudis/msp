/*****************************************************************************
**  g2dModeBaseTest.cpp
**
**  See g2dModeBaseTest.hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "g2dModeBaseTest.hpp"

#include "appSimTime.hpp"
#include "fsLocator.hpp"
#include "g2dFontUtil.hpp"
#include "g2dImage.hpp"
#include "g2dImageCreate.hpp"
#include "g2dWindow.hpp"


namespace
{
	int m_ThinkCount;
	g2dFontHandle m_FontSmall;
	g2dFontHandle m_FontLarge;
	g2dImage* m_Image1;
	g2dImage* m_Image2;
	int m_Frame;
	int m_X,m_Y;
	bool m_MoreText;
}

//====================================================================
// Construction -- Inititialize the LandingSequenceScriptMgr
//====================================================================
g2dModeBaseTest::g2dModeBaseTest(g2dWindow& i_Window)
:	g2dMode(i_Window, 1) // we use exactly one debug string
{
	m_X = 0;
	m_Y = 0;
	m_Frame = 0;
}

//====================================================================
// Destruction -- cleanup LandingSequenceScriptMgr
//====================================================================
g2dModeBaseTest::~g2dModeBaseTest()
{
}

//====================================================================
//	The mode should do it's per frame "work" in the Think function.
//====================================================================
void g2dModeBaseTest::Think()
{
	// base class
	g2dMode::Think();

	if (e_Continue != GetTerminateCondition() )
	{
		return; // we're terminated
	}

	// Do work here
	m_Frame++;

	GetWindow().Clear(g2dRGBColor(0x00, 0x70, 0x50));

	const int num_messages = 7;
	itString message[num_messages];

	message[0] = itString("This is a application demonstrating");
	message[1] = itString("some basic Terawatt 2D functionality.");
	message[2] = itString("It also shows how to do some event handling.");
	message[3] = itString("Press q to quit,");
	message[4] = itString("w to toggle display of more text");
	message[5] = itString("(which will slow it down a lot)");
	message[6] = itString("s to test resolution switching (deinitializes and goes back to same res)");

	int i;
	for( i = 0 ; i < num_messages ; i++ )
		this->SetDebugString(0, message[i]);

	itString text("Some unicode text: ");
	text += itString::CharType(0x306E);
	text += itString::CharType(0x308A);
	text += itString::CharType(0x3053);

	if( m_MoreText )
		this->SetDebugString(0, text);

	GetWindow().DrawText(20, 250, m_FontLarge, text, g2dRGBColor(0xf0, 0x90, 0x20));

	GetWindow().DrawImage(	m_X, m_Y, *m_Image1);

	char positionxy[32];
	sprintf(positionxy, " X:%d, Y:%d", m_X, m_Y );
	itString position(positionxy);

	if( m_MoreText )
		GetWindow().DrawText( 20, 600, m_FontLarge, position, g2dRGBColor(0xf0, 0x90, 0x20));

	GetWindow().DrawImage(300, 300, *m_Image2);

	this->Render();
}

//====================================================================
//	Initialize will be called before the first call of Think after
//	the object is first created or DeInitialized.  During the
//	lifetime of a mode, Initialize and DeInitialize may be called
//	several times.  Children of g2dModeAttract should remember to call
//	g2dModeAttract::Initialize() at the beginning of their Initialize
//	function.
//====================================================================
void g2dModeBaseTest::Initialize()
{
	g2dMode::Initialize();

	// Do Initializing here
	m_FontSmall = g2dFontUtil::LoadFont(itString("Arial Unicode MS"), 16);
	m_FontLarge = g2dFontUtil::LoadFont(itString("Arial Unicode MS"), 64);

	//m_Image1 = g2dImageCreate::Make(200, 200, GetWindow().GetPixelFormat(), g2dImage::e_SystemMemory );
	
	fsLocator locator;
	locator.Push(itString("data"));
	locator.Push(itString("image.bmp"));

	m_Image1 = g2dImageCreate::Load( locator, g2dImage::e_SystemMemory );
	//g2dImage *tree = g2dImageCreate::Load( locator, g2dImage::e_SystemMemory );
	//m_Image1->DrawImage(100, 100, 0, 0, 100, 100, *tree);
	//delete tree;

	locator.Pop();
	locator.Push("image2.png");
	m_Image2 = g2dImageCreate::Load(locator);

	//g2dScreen::SetGamma(0.5f);
}

//====================================================================
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//====================================================================
void g2dModeBaseTest::DeInitialize()
{
	// Do DeInitializing here
	delete m_Image1;
	delete m_Image2;

	//	let the base deinitialize
	g2dMode::DeInitialize();
}
