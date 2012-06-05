/****************************************************************************\
**  lwTextDisplayMgr.cpp
**
**      lwTextDisplayMgr.hpp holds lines of text for debug display
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "Features/Capture/lwTextDisplayMgr.hpp"

#include "Graphics/G2d/g2dFontUtil.hpp"
#include "Graphics/G2d/g2dWindow.hpp"
#include "Core/Env/envSTLHelpers.hpp"

#include <algorithm>


//============================================================================
//============================================================================
namespace
{
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
lwTextDisplayBlock::lwTextDisplayBlock(g2dWindow& i_Window, const char* i_FontName, int i_Size)
:	m_Window(i_Window), 
	m_bEnabled(true),
	m_FontColor(256,256,256)
{
	m_pFont = g2dFontUtil::LoadFont( itString(i_FontName), i_Size );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lwTextDisplayBlock::~lwTextDisplayBlock()
{
	g2dFontUtil::ReleaseFont(m_pFont);
}

//--------------------------------------------------------------------
//	Render renders the debug display, if necessary
//--------------------------------------------------------------------
void lwTextDisplayBlock::Render()
{
	if ( m_bEnabled )
	{
		std::vector<lwTextDisplayInfo*>::iterator it = m_DisplayText.begin();
		std::vector<lwTextDisplayInfo*>::iterator end = m_DisplayText.end();

		m_Window.BeginScene();

		while ( it != end )
		{
			int y = (*it)->m_Y;
			int x = (*it)->m_X;

			m_Window.DrawText( x, y, m_pFont, (*it)->m_Text, m_FontColor );
			++it;
		}

		m_Window.EndScene();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void lwTextDisplayBlock::SetColor( g2dRGBColor& i_Color )
{
	m_FontColor = i_Color;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void lwTextDisplayBlock::AddText(int i_X, int i_Y, const char* i_Text)
{
	if ( i_Text )
	{
		lwTextDisplayInfo* tdi = new lwTextDisplayInfo();
		tdi->m_Text = itString(i_Text);
		tdi->m_X = i_X;
		tdi->m_Y = i_Y;

		m_DisplayText.push_back( tdi );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void lwTextDisplayBlock::ClearText()
{
	envSTLHelpers::DeleteContainer( m_DisplayText );
}

/*
//----------------------------------------------------------------------------
//	RenderDebugText causes the given (single-byte) text to be drawn on the
//	screen using the debug font.
//----------------------------------------------------------------------------
void RenderDebugText(const char* i_Text, int i_X, int i_Y)
{
	LPD3DXFONT font = g2dFontUtilPAC::GetD3DFont(m_pFont);

	int width, height;
	g2dFontUtilPAC::GetTextSizeInPixels(m_pFont, itString(i_Text), width, height);

	RECT rect;
	rect.left = i_X;
	rect.top = i_Y;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	g2dDX9Global::g_pDevice->BeginScene();

	INT ret_val = font->DrawTextA(	i_Text,
									-1,
									&rect,
									DT_LEFT | DT_NOCLIP | DT_TOP,
									D3DCOLOR_COLORVALUE(0.0f, 1.0f,0.0f, 1.0f));

	g2dDX9Global::g_pDevice->EndScene();
}
*/


//============================================================================
//============================================================================
namespace lwTextDisplayMgr
{
	std::vector<lwTextDisplayBlock*> m_TextBlocks;

//--------------------------------------------------------------------
//	does not own it
//--------------------------------------------------------------------
void AddBlock(lwTextDisplayBlock* i_pBlock)
{
	DBG_ASSERT0( i_pBlock != NULL, "Cannot add a NULL block");

	m_TextBlocks.push_back(i_pBlock);
}

//--------------------------------------------------------------------
//	does not delete the block
//--------------------------------------------------------------------
void RemoveBlock(lwTextDisplayBlock* i_pBlock)
{
	DBG_ASSERT0( i_pBlock != NULL, "Cannot remove a NULL block");

	envSTLHelpers::RemoveOneValue( m_TextBlocks, i_pBlock );
}

//--------------------------------------------------------------------
//	delete all the blocks
//--------------------------------------------------------------------
void DeleteBlocks()
{
	envSTLHelpers::DeleteContainer( m_TextBlocks );
}

//--------------------------------------------------------------------
//	Render the blocks
//--------------------------------------------------------------------
void Render()
{
	for (int i=0; i < m_TextBlocks.size(); ++i)
	{
		m_TextBlocks[i]->Render();
	}
}

}
