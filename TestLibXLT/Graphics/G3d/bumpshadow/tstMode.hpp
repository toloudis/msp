/*****************************************************************************
**  tstMode.hpp
**
**      This is the base class mode for this test project.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef TST_MODE_HPP
#error tstMode.hpp multiply included
#endif
#define TST_MODE_HPP

#ifndef APP_MODE_HPP
#include "appMode.hpp"
#endif
#ifndef G2D_FONTHANDLE_HPP
#include "g2dFontHandle.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "g2dRGBColor.hpp"
#endif
#ifndef IT_STRING_HPP
#include "itString.hpp"
#endif

#include <vector>

class g3dDirectionalLight;

class tstMode : public appMode
{
	public:
		//====================================================================
		// Construct with the desired number of debug strings, text color and
		// background color (used on scene clear)
		//====================================================================
		tstMode( int i_nNumDebugStrings, 
				bool i_bCreateLights = true,
				const g2dRGBColor& i_TextColor=g2dRGBColor(0,0,0),
				const g2dRGBColor& i_BackColor=g2dRGBColor(0x30, 0x80, 0xa0) );

		//====================================================================
		// pure-virtual, children must be created
		//====================================================================
		virtual ~tstMode() = 0;

		//====================================================================
		//	The mode should do it's per frame "work" in the Think function.
		// This simply calls the base class and handles screen captures.
		//====================================================================
		virtual void Think() = 0;

		//====================================================================
		//	Initialize will be called before the first call of Think after
		//	the object is first created or DeInitialized.  During the
		//	lifetime of a mode, Initialize and DeInitialize may be called
		//	several times.  Children of extModeWidthTest should remember to call
		//	extModeWidthTest::Initialize() at the beginning of their Initialize
		//	function.
		//====================================================================
		virtual void Initialize() = 0;

		//====================================================================
		//	The object should clean up things that are not needed while the
		//	mode is not running in the DeInitialize function.
		//====================================================================
		virtual void DeInitialize() = 0;

	protected:
		//====================================================================
		// Between thinks, debug strings can be added, they are set to
		// the display vector until Render is called, at which time they 
		// are displayed.
		//====================================================================
		void SetDebugString(int i_nIndex, const itString& i_String );

		//====================================================================
		//	Render is a convenience for child classes which does the usual
		//	rendering stuff like the scScene cull, g3dRenderer::RenderModels,
		//	and so on.  i_Camera can be NULL, in which case no culling is
		//	done (for menus, for instance).
		//	i_ClearColor can also be NULL, for no background clear
		//====================================================================
		void Render();

	private:

		//====================================================================
		// display_debug_text will draw all of the debug strings on each render
		//====================================================================
		void display_debug_text();

		// the debug strings to display
		std::vector<itString> m_DebugStrings;
		bool m_bCreateLights;

		// the font
		g2dFontHandle m_Font;

		// text and background colors
		g2dRGBColor m_TextColor;
		g2dRGBColor m_BackColor;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;

};
