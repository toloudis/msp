/*****************************************************************************
**	scrText.hpp
**
**		scrText displays a single Text string on screen
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SCR_TEXT_HPP
#error scrText.hpp multiply included
#endif
#define SCR_TEXT_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef SCR_IMAGE_HPP
#include "Graphics/scr/scrImage.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class fsLocator;
class maFloatRGBA;


//============================================================================
//============================================================================
class scrText : public scrImage
{
	public:
		//--------------------------------------------------------------------
		// Construction
		//--------------------------------------------------------------------
		scrText();

		//--------------------------------------------------------------------
		// Destruction
		//--------------------------------------------------------------------
		virtual ~scrText();

		//--------------------------------------------------------------------
		//	SetSize sets the current height and width
		//--------------------------------------------------------------------
		void SetSize(int i_Width, int i_Height);

		//--------------------------------------------------------------------
		//	GetText returns the text string to be displayed
		//--------------------------------------------------------------------
		const itString& GetText() const;

		//--------------------------------------------------------------------
		//	SetText sets the text string to be displayed
		//--------------------------------------------------------------------
		void SetText(const itString& i_Text);

		//--------------------------------------------------------------------
		//	SetFontMapImage passes an fsLocator to the Image to be used as a
		//	font map
		//--------------------------------------------------------------------
		void SetFontMapImage(const fsLocator& i_Locator, int i_Width = 0, int i_Height = 0);

		//--------------------------------------------------------------------
		//	SetFontMapImage passes a matTexture to be used as a font map
		//--------------------------------------------------------------------
		void SetFontMapImage(matTexture* i_FontMap, int i_Width = 0, int i_Height = 0);

		//--------------------------------------------------------------------
		//	SetCentered sets whether the text should be centered around its
		//	position
		//--------------------------------------------------------------------
		void SetCentered(bool i_bCentered);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetForegroundColor( const maFloatRGBA& i_Color );
		const maFloatRGBA& GetForegroundColor() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetBackgroundColor( const maFloatRGBA& i_Color );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetScreenBased(bool i_bScreenBased);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetTextSize(float i_Size);

	private:
		//--------------------------------------------------------------------
		//	make_geometry handles initial creation of the geomtry, should be
		//	called when the new text length exceeds m_FragmentLengthInChar
		//--------------------------------------------------------------------
		void make_geometry();

		//--------------------------------------------------------------------
		//	remap_geometry_uvs is called when new text length is equal to or
		//	less than m_FragmentLengthInChar, handles remapping the UVs of
		//	the geometry, which lets us avoid completely remaking the geometry
		//--------------------------------------------------------------------
		void remap_geometry_uvs();

		//--------------------------------------------------------------------
		//	update_transform
		//--------------------------------------------------------------------
		void update_transform();

		//--------------------------------------------------------------------
		//	center_text will determine the offset needed for the current text
		//	and size combination, will return immediately if not currently
		//	centered
		//--------------------------------------------------------------------
		void center_text();

private:
		int m_CharWidth, m_CharHeight;
		float m_UVWidth, m_UVHeight;
		itString m_Text;
		int m_FragmentLengthInChar;
		bool m_bCentered;
		int m_CenterOffsetX;
		bool m_bScreenBased;
		float m_CharSize;

		maFloatRGBA m_ForegroundColor;
		maFloatRGBA m_BackgroundColor;
};
