/*****************************************************************************
**  g2dFontUtilVk.hpp
**
**      GraphicsVulkan port of GraphicsDX11's g2dFontUtilDX11: the bitmap
**	font (BITMAP_FONT_SPRITE, the path DX11 builds). A font is an scrText
**	sprite drawn as a tmeshFrag with the font map texture and the font
**	shaders (G2d/private/Shaders/FontVS.hlsl, FontPS.hlsl, compiled at
**	build time instead of from strings at run time).
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G2DFONTUTILVK_HPP
#error g2dFontUtilVk.hpp multiply included
#endif
#define GVK_G2DFONTUTILVK_HPP

#ifndef G2D_FONTUTIL_HPP
#include "Graphics/G2d/g2dFontUtil.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#include <string>
#include <map>

#undef DrawText

class matTexture;
class scrText;

namespace gvk
{
	class g2dFontObjectVk
	{
	public:
		g2dFontObjectVk(matTexture* i_FontMap);
		~g2dFontObjectVk();

		//	Draws into the current render target, the text's top left at
		//	(i_OffsetX, i_OffsetY)
		void DrawText(const std::string& i_Text, const maFloatRGBA& i_Color, int i_OffsetX = 0, int i_OffsetY = 0);
		void SetFontMap(matTexture* i_fontMap);
		void SetTextSize(float i_Size) {m_TextSize = i_Size;}

		//	The shaders and states (g3dSystemVk calls these, as g3dSystemDX11 does)
		static void Init();
		static void Cleanup();

	private:
		static void InitDefaultShader();
		static void CleanupDefaultShader();

		scrText* m_TextSprite;
		matTexture* m_FontMap;
		float m_TextSize;
	};

	class g2dFontUtilVk : public g2dFontUtilImpl
	{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		g2dFontUtilVk();
		~g2dFontUtilVk();

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		static g2dFontUtilVk* Implementation();

		//------------------------------------------------------------------------
		//	LoadFont make the given font name available for use and returns
		//	a g2dFontHandle which can be used later.  When you are done with the
		//	font, call ReleaseFont.
		//	(The bitmap font: i_Name isn't used; NULL if the global font
		//	bitmap doesn't exist.)
		//------------------------------------------------------------------------
		g2dFontHandle LoadFont(const std::string& i_Name, int i_Size);

		//------------------------------------------------------------------------
		//	ReleaseFont informs the system that you are done with the given
		//	g2dFontHandle.
		//------------------------------------------------------------------------
		void ReleaseFont(g2dFontHandle i_Handle);

		//------------------------------------------------------------------------
		// GetLoadedFontName returns the name of the font being used for the given
		// g2dFontHandle.  It looks this up in the system to learn exactly what
		// the system loaded.
		// (Not implemented for the bitmap font, as in DX11: o_Name is unchanged.)
		//------------------------------------------------------------------------
		void GetLoadedFontName( g2dFontHandle i_Font, std::string& o_Name );

		//------------------------------------------------------------------------
		//	GetTextSizeInPixels returns the width and height in pixels of the given text and font
		//	(1 x 1 for the bitmap font, as in DX11)
		//------------------------------------------------------------------------
		void GetTextSizeInPixels(g2dFontHandle i_Font, const std::string& i_Text, int& o_Width, int& o_Height);

		//------------------------------------------------------------------------
		//	Get and set the global font path
		//------------------------------------------------------------------------
		void SetGlobalFontBitmap(const fsLocator& i_FontPath);

		void GetGlobalFontBitmap(fsLocator& o_FontPath);

		struct FontInfo
		{
			FontInfo() : m_Font(NULL), m_Reference(0) {}

			g2dFontObjectVk* m_Font;

			int m_Reference;
		};

		typedef std::map<g2dFontHandle, FontInfo> FontMap;
		typedef FontMap::iterator FontMapIt;

	private:

		FontMap m_Fonts;
	};
}
