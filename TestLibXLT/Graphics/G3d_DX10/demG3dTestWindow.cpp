/*****************************************************************************
**  demG3dTestWindow.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestWindow.hpp"

#include "Core/app/appApplication.hpp"
#include "Core/app/appCharEvent.hpp"
#include "Core/app/appTime.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"
#include "Graphics/g2d/g2dImageCreate.hpp"
#undef DrawText

namespace
{
	const int l_nColors = 5;
	g2dRGBColor l_Colors[l_nColors] = {
		g2dRGBColor(0,0,0),
		g2dRGBColor(255,255,255),
		g2dRGBColor(255,0,0),
		g2dRGBColor(0,255,0),
		g2dRGBColor(0,0,255)};

	const int l_nSizes = 3;
	int l_Sizes[l_nSizes*2] = {
		640, 480, 
		1000, 100,
		100, 1000
		};
};

//====================================================================
//====================================================================
demG3dTestWindow::demG3dTestWindow(g2dWindow &i_Window)
:	m_Window(i_Window), m_Image(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestWindow::~demG3dTestWindow()
{
}

//====================================================================
//====================================================================
void demG3dTestWindow::Initialize()
{
	demG3dTestMode::Initialize();

	m_BGColor = g2dRGBColor(0xff,0,0xff);
	m_Font = g2dFontUtil::LoadFont(itString("Arial"), 16);

	fsLocator loc;
	loc.Push("data");
	loc.Push("green003.png");
	m_Image = g2dImageCreate::Load(loc);

	// set up display info
//	m_Viewer.SetTextMessage(0, itString("Keys A, S, D."));
//	m_Viewer.SetTextMessage(1, itString("The moving spheres are coincident with the positions of two point lights.  The scene is"));
//	m_Viewer.SetTextMessage(2, itString("also illuminated by a directional light.  The ground material and the sphere material"));
//	m_Viewer.SetTextMessage(3, itString("both show specular reflections, whereas the moving spheres only have diffuse reflection."));
//	m_Viewer.SetTextMessage(4,  itString("Also, the moving spheres show a non-textured translucent material."));
//	m_Viewer.SetTextMessage(5, itString("Also, the texture on the rectangle is mip-mapped.  (Try moving the camera forward and back.)"));
//	m_Viewer.SetTextMessage(6,  itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
//	m_Viewer.SetTextMessage(7,  itString("Press space to go to the next section"));
}

//====================================================================
//	Think
//====================================================================
void demG3dTestWindow::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	clear color
	m_Window.Clear(m_BGColor);
	m_Window.BeginScene();

	m_Window.DrawImage(0,0, *m_Image);
	m_Window.DrawImage(128,128, 0,0,20,20, *m_Image);
	// draw text
	int top = 0;//30;
	int left = 0;//10;
	int lineNum = 0; 
	int y = (lineNum * 15) + top;
	int x = left;
	m_Window.DrawText( x, y, m_Font, itString("I am DirectX 11!"), m_TextColor );

	m_Window.EndScene();
	m_Window.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestWindow::DeInitialize()
{
	delete m_Image;
	g2dFontUtil::ReleaseFont(m_Font);

	demG3dTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestWindow::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		// change bg color
		case itString::CharType('a'):
		case itString::CharType('A'):
			{
				static int iColor = 0;
				iColor = (iColor+1)%l_nColors;
				m_BGColor = (l_Colors[iColor]);
			}
		break;
		// change text color
		case itString::CharType('s'):
		case itString::CharType('S'):
			{
				static int iTextColor = 0;
				iTextColor = (iTextColor+1)%l_nColors;
				m_TextColor = (l_Colors[iTextColor]);
			}
		break;
		// change window size
		case itString::CharType('d'):
		case itString::CharType('D'):
			static int iSize = 0;
			iSize = (iSize+1)%l_nSizes;
			m_SizeX = (l_Sizes[iSize*2]);
			m_SizeY = (l_Sizes[iSize*2+1]);
			appApplication::SetMainWindowSize(0,0,m_SizeX, m_SizeY);
			m_Window.ResizeWindow(m_SizeX,m_SizeY);
		break;
	}

	demG3dTestMode::ReceiveCharEvent(i_Event);
}