/****************************************************************************\
**	g2dDebugDisplay.hpp
**
**		g2dDebugDisplay.hpp holds lines of text for debug display
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_DEBUGDISPLAY_HPP
#error g2dDebugDisplay.hpp multiply included
#endif
#define G2D_DEBUGDISPLAY_HPP

#ifndef G2D_FONTHANDLE_HPP
#include "Graphics/g2d/g2dFontHandle.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

#include <map>


//============================================================================
//============================================================================
class g2dWindow;


//============================================================================
//============================================================================
class g2dDebugDisplay
{
	public:
		//--------------------------------------------------------------------
		// takes window to which to draw text
		//--------------------------------------------------------------------
		g2dDebugDisplay(g2dWindow& i_Window);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~g2dDebugDisplay();

		//--------------------------------------------------------------------
		//	Render renders the debug display, if necessary
		//--------------------------------------------------------------------
		void Render();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetDebugInfo(int i_Line, const char* i_Text);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		bool IsEnabled() const { return m_bEnabled; }

		//--------------------------------------------------------------------
		// for clients without inDeviceMgr::GetKeyboard() or needing to 
		// enable this by some other means
		//--------------------------------------------------------------------
		void ToggleEnabled() {m_bEnabled = !m_bEnabled;}
		void EnableDebugOverlay(bool i_bEnabled) {m_bEnabled = i_bEnabled;}

	private:
		g2dWindow& m_Window;
		g2dFontHandle m_DebugFont;
		bool m_bEnabled;
		std::map<int, itString> m_DebugInfo;
};

