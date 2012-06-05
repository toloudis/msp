/****************************************************************************\
**  lwTextDisplayMgr.hpp
**
**      lwTextDisplayMgr.hpp holds lines of text for display
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef LW_TEXTDISPLAYMGR__HPP
#error lwTextDisplayMgr.hpp multiply included
#endif
#define LW_TEXTDISPLAYMGR__HPP

#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/G2d/g2dRGBColor.hpp"
#endif
#ifndef G2D_FONTHANDLE_HPP
#include "Graphics/G2d/g2dFontHandle.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif

#include <map>


//============================================================================
//	Forward References
//============================================================================
class g2dWindow;


//============================================================================
//============================================================================
class lwTextDisplayInfo
{
public:
	itString	m_Text;
	int			m_X;
	int			m_Y;
};


//============================================================================
//============================================================================
class lwTextDisplayBlock
{
	public:
		//--------------------------------------------------------------------
		// takes window to which to draw text
		//--------------------------------------------------------------------
		lwTextDisplayBlock(g2dWindow& i_Window, const char* i_FontName, int i_Size);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~lwTextDisplayBlock();

		//--------------------------------------------------------------------
		//	Render renders the debug display, if necessary
		//--------------------------------------------------------------------
		void Render();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetColor( g2dRGBColor& i_Color );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void AddText(int i_X, int i_Y, const char* i_Text);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void ClearText();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		bool IsEnabled() const { return m_bEnabled; }

		//--------------------------------------------------------------------
		// for clients without inDeviceMgr::GetKeyboard() or needing to 
		// enable this by some other means
		//--------------------------------------------------------------------
		void ToggleEnabled() {m_bEnabled = !m_bEnabled;}

	private:
		g2dWindow& m_Window;
		g2dFontHandle m_pFont;
		g2dRGBColor m_FontColor;
		bool m_bEnabled;
		std::vector<lwTextDisplayInfo*> m_DisplayText;
};


//============================================================================
//============================================================================
namespace lwTextDisplayMgr
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void AddBlock(lwTextDisplayBlock* i_pBlock);

	//--------------------------------------------------------------------
	//	does not delete the block
	//--------------------------------------------------------------------
	void RemoveBlock(lwTextDisplayBlock* i_pBlock);

	//--------------------------------------------------------------------
	//	delete all the blocks
	//--------------------------------------------------------------------
	void DeleteBlocks();

	//--------------------------------------------------------------------
	//	Render the blocks
	//--------------------------------------------------------------------
	void Render();
}
