/*****************************************************************************
**  demG3dTestWindow.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTWINDOW_HPP
#error demG3dTestWindow.hpp multiply included
#endif
#define DEM_G3DTESTWINDOW_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
#endif

#ifndef G2D_FONTHANDLE_HPP
#include "Graphics/g2d/g2dFontHandle.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/g2d/g2dRGBColor.hpp"
#endif

class g2dWindow;
class g2dImage;

class demG3dTestWindow 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestWindow(g2dWindow &i_Window);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestWindow();

		//====================================================================
		//	Think
		//====================================================================
		virtual void Think();

		//====================================================================
		//====================================================================
		virtual void Initialize();

		//====================================================================
		//====================================================================
		virtual void DeInitialize();

		//====================================================================
		//	Override this function to get appCharEvents.
		//====================================================================
		virtual void ReceiveCharEvent(appCharEvent& i_Event);

	private:
		g2dWindow &m_Window;

		g2dRGBColor m_BGColor;
		g2dRGBColor m_TextColor;
		g2dFontHandle m_Font;
		g2dImage* m_Image;
		int m_SizeX, m_SizeY;
};
