/*****************************************************************************
**  g2dModeLoadTexture.cpp
**
**  See g2dModeLoadTexture.hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "g2dModeLoadTexture.hpp"

#include "appSimTime.hpp"
#include "fsLocator.hpp"
#include "g2dImage.hpp"
#include "g2dImageCreate.hpp"
#include "g2dWindow.hpp"

const int MAX_IMAGES = 4;

g2dImage* l_CurPageImage[ MAX_IMAGES ];
fsLocator l_CurFilename;


//====================================================================
// Construction -- Inititialize the LandingSequenceScriptMgr
//====================================================================
g2dModeLoadTexture::g2dModeLoadTexture(g2dWindow& i_Window)
:	g2dMode(i_Window, 1) // we use exactly one debug string
{
}

//====================================================================
// Destruction -- cleanup LandingSequenceScriptMgr
//====================================================================
g2dModeLoadTexture::~g2dModeLoadTexture()
{
}

//====================================================================
//	The mode should do it's per frame "work" in the Think function.
//====================================================================
void g2dModeLoadTexture::Think()
{
	// base class
	g2dMode::Think();

	if (e_Continue != GetTerminateCondition() )
	{
		return; // we're terminated
	}

	// Do work here
	for ( int i = 0; i < MAX_IMAGES ; i++ )
	{
		int x,y;
		x = 20 + 144 * (i % (MAX_IMAGES/2));
		y = 200 + 144 * ( (int) (i/(MAX_IMAGES/2)));
		if( l_CurPageImage[i] )
			GetWindow().DrawImage(x,y, *l_CurPageImage[i]);
	}

	char temp[32];
	sprintf(temp, "Time(%.2f)", appSimTime::GetTime() );
	itString my_string( "This is a sample debug string, ");
	my_string += itString(temp);
	this->SetDebugString(0, my_string);

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
void g2dModeLoadTexture::Initialize()
{
	g2dMode::Initialize();

	// Do Initializing here
	//for ( int i = 0; i < MAX_IMAGES ; i++ )
	//{
	//	l_CurPageImage[i] = new g2dImage(16, 16, GetWindow().GetPixelFormat());
	//}

	l_CurFilename.Push( "." );
	l_CurFilename.Push( "data" );
	l_CurFilename.Push( "128x128x24-2.png" );
	l_CurPageImage[0] = g2dImageCreate::Load(l_CurFilename);

	l_CurFilename.Pop();
	l_CurFilename.Push( "128x128x24-1.bmp" );
	l_CurPageImage[1] = g2dImageCreate::Load(l_CurFilename);

	l_CurFilename.Pop();
	l_CurFilename.Push( "128x128x24-3.png" );   // fyi, has alpha
	l_CurPageImage[2] = g2dImageCreate::Load(l_CurFilename);

	l_CurFilename.Pop();
	l_CurFilename.Push( "128x128x24-4.png" );  // fyi, has alpha
	l_CurPageImage[3] = g2dImageCreate::Load(l_CurFilename);
}

//====================================================================
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//====================================================================
void g2dModeLoadTexture::DeInitialize()
{
	// Do DeInitializing here

	for ( int i = 0; i < MAX_IMAGES ; i++ )
	{
		if( l_CurPageImage[i] )
		{
			delete l_CurPageImage[i];
			l_CurPageImage[i] = NULL;
		}
	}

	//	let the base deinitialize
	g2dMode::DeInitialize();
}
