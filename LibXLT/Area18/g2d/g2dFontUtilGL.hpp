/*****************************************************************************
**  g2dFontUtilGL.hpp
**
**      g2dFontUtilGL contains the windows implementation of the
**	g2dFontUtil.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_FONTUTILGL_HPP
#error g2dFontUtilGL.hpp multiply included
#endif
#define G2D_FONTUTILGL_HPP

#ifndef G2D_FONTUTIL_HPP
#include "Graphics/g2d/g2dFontUtil.hpp"
#endif

//#ifndef G2D_DX11TYPES_HPP
//#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
//#endif

#include <string>
#include <map>

#define BITMAP_FONT_SPRITE
#undef DrawText

class itString;
class matTexture;
class scrText;
#if 0
class g2dFontObjectDX11
{
public:
	g2dFontObjectDX11(matTexture* i_FontMap);
	~g2dFontObjectDX11();

	void DrawText(const itString& i_Text, const maFloatRGBA& i_Color, int i_OffsetX = 0, int i_OffsetY = 0);
	void SetFontMap(matTexture* i_fontMap);
	void SetTextSize(float i_Size) {m_TextSize = i_Size;}

	static void Init();
	static void Cleanup();

private:
	static void InitDefaultShader();
	static void CleanupDefaultShader();

	scrText* m_TextSprite;
	matTexture* m_FontMap;
	float m_TextSize;

	static ID3D11InputLayout* m_pQuadLayout;
	static ID3D11VertexShader* m_pQuadVS;
	static ID3D11PixelShader* m_pQuadPS;
	static ID3D11Buffer* m_pcbPSColor;

};
#endif
class g2dFontUtilGL : public g2dFontUtilImpl
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	g2dFontUtilGL();
	~g2dFontUtilGL();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	static g2dFontUtilGL* Implementation();

	//------------------------------------------------------------------------
	//	LoadFont make the given font name available for use and returns
	//	a g2dFontHandle which can be used later.  When you are done with the
	//	font, call ReleaseFont.
	//------------------------------------------------------------------------
	g2dFontHandle LoadFont(const itString& i_Name, int i_Size);

	//------------------------------------------------------------------------
	//	ReleaseFont informs the system that you are done with the given
	//	g2dFontHandle.
	//------------------------------------------------------------------------
	void ReleaseFont(g2dFontHandle i_Handle);

	//------------------------------------------------------------------------
	// GetLoadedFontName returns the name of the font being used for the given
	// g2dFontHandle.  It looks this up in the system to learn exactly what
	// the system loaded.
	//------------------------------------------------------------------------
	void GetLoadedFontName( g2dFontHandle i_Font, itString& o_Name );
	void GetLoadedFontName( g2dFontHandle i_Font, std::string& o_Name );

	//------------------------------------------------------------------------
	//	GetTextSizeInPixels returns the width and height in pixels of the given text and font
	//------------------------------------------------------------------------
	void GetTextSizeInPixels(g2dFontHandle i_Font, const itString& i_Text, int& o_Width, int& o_Height);

	//------------------------------------------------------------------------
	//	Get and set the global font path
	//------------------------------------------------------------------------
	void SetGlobalFontBitmap(const fsLocator& i_FontPath);
	
	void GetGlobalFontBitmap(fsLocator& o_FontPath);

	//------------------------------------------------------------------------
	//	GetD3DFont returns a ID3DX11Font for the given font handle.
	//------------------------------------------------------------------------
//	ID3DX11Font* GetD3DFont(g2dFontHandle i_Handle);

	struct FontInfo
	{
		FontInfo() : /*m_Font(NULL),*/ m_Reference(0) {}

#ifdef BITMAP_FONT_SPRITE
//		g2dFontObjectDX11* m_Font;
#else
//		IDWriteTextFormat* m_Font;
#endif

		int m_Reference;
	};

	typedef std::map<g2dFontHandle, FontInfo> FontMap;
	typedef FontMap::iterator FontMapIt;

private:

	FontMap m_Fonts;
};

