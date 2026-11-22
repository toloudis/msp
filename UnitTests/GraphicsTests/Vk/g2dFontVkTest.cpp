/*****************************************************************************
**  g2dFontVkTest.cpp
**
**      g2dFontUtilVk: the bitmap font (font-lucd00.png, an scrText sprite
**      drawn with the font shaders) against GraphicsDX11's, with the whole
**      layer up on each backend (gtBackend): text drawn by a window
**      (g2dWindow::DrawText) and into a render target (g2dWindowDrawUtil's
**      DrawText, which g3dRenderPass uses), at several sizes, colours and
**      offsets; GetTextSizeInPixels; and no font without the bitmap.
**      Skipped without a Vulkan device. Also g2dFontUtil's cache (no
**      device): the last ReleaseFont unloads the font.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtPlatform.hpp"
#include "gtSceneWindow.hpp"
#include "gtTarget.hpp"
#include "gtTempDir.hpp"

#include <gtest/gtest.h>

#ifndef G2D_FONTUTIL_HPP
#include "Graphics/G2d/g2dFontUtil.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/G2d/g2dRGBColor.hpp"
#endif
#ifndef G2D_WINDOW_HPP
#include "Graphics/G2d/g2dWindow.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef G2D_WINDOWDRAWUTILD3D11_HPP
#include "GraphicsDX11/g2d/private/g2dWindowDrawUtilDX11.hpp"
#endif
#ifndef GVK_G2DWINDOWDRAWUTILVK_HPP
#include "GraphicsVulkan/G2d/private/g2dWindowDrawUtilVk.hpp"
#endif

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#undef DrawText

namespace
{
	fsLocator font_bitmap()
	{
		const std::filesystem::path path = std::filesystem::u8path(gtPlatform::RepoRoot()) / "MachStudio" / "Runtime" / "Data" /
			"Icons" / "font-lucd00.png";
		fsLocator locator;
		fsFileUtil::UnicodeStringToLocator(path.u8string(), locator);
		return locator;
	}

	struct gtText
	{
		const char*	m_pText;
		int			m_Size;		// LoadFont's
		g2dRGBColor	m_Color;
		int			m_X, m_Y;
	};

	//	Sizes, colours, offsets; the last ones run off the right and bottom
	//	edges (DrawText's viewport is 200x150 at the offset)
	const gtText k_Texts[] =
	{
		{ "Hello, MachStudio",		10, g2dRGBColor(255, 255, 255),	0,		0 },
		{ "0123456789 !?%&()",		16, g2dRGBColor(255, 128, 0),	37,		61 },
		{ "frame 42 / 120",			24, g2dRGBColor(0, 200, 255),	12,		120 },
		{ "small",					6,	g2dRGBColor(40, 255, 90),	250,	30 },
		{ "edge text",				12, g2dRGBColor(200, 60, 220),	270,	200 },
	};

	//	Loads one font per size in k_Texts (NULL entries if the bitmap is missing)
	std::vector<g2dFontHandle> load_fonts()
	{
		g2dFontUtil::SetGlobalFontBitmap(font_bitmap());
		std::vector<g2dFontHandle> fonts;
		for (const gtText& text : k_Texts)
			fonts.push_back(g2dFontUtil::LoadFont(std::string("Lucida"), text.m_Size));
		return fonts;
	}

	void release_fonts(const std::vector<g2dFontHandle>& i_Fonts)
	{
		for (g2dFontHandle font : i_Fonts)
			g2dFontUtil::ReleaseFont(font);
	}

	void draw_into_target(gtBackend::Kind i_Kind, int i_X, int i_Y, g2dFontHandle i_Font, const std::string& i_Text,
						  const g2dRGBColor& i_Color)
	{
		if (i_Kind == gtBackend::e_DX11)
			g2dWindowDrawUtilDX11::DrawText(i_X, i_Y, i_Font, i_Text, i_Color);
		else
			gvk::g2dWindowDrawUtilVk::DrawText(i_X, i_Y, i_Font, i_Text, i_Color);
	}

	//	How many pixels differ from the first one (the clear colour)
	int drawn_pixels(const gtPicture& i_rPicture)
	{
		int drawn = 0;
		for (size_t i = 4; i + 3 < i_rPicture.m_Rgba.size(); i += 4)
			drawn += memcmp(&i_rPicture.m_Rgba[i], &i_rPicture.m_Rgba[0], 3) != 0;
		return drawn;
	}

	void expect_match(const char* i_pName, const gtPicture& i_rDX11, const gtPicture& i_rVulkan)
	{
		ASSERT_TRUE(i_rDX11.Valid()) << i_pName;
		ASSERT_TRUE(i_rVulkan.Valid()) << i_pName;
		ASSERT_EQ(i_rDX11.m_Width, i_rVulkan.m_Width);
		ASSERT_EQ(i_rDX11.m_Height, i_rVulkan.m_Height);
		const int pixels = i_rDX11.m_Width * i_rDX11.m_Height;
		EXPECT_GT(drawn_pixels(i_rDX11), pixels / 100) << i_pName << ": DX11 drew almost no text";
		EXPECT_GT(drawn_pixels(i_rVulkan), pixels / 100) << i_pName << ": Vulkan drew almost no text";
		const gtCompareResult result = gtCompareImages(i_rDX11.m_Rgba.data(), i_rVulkan.m_Rgba.data(), i_rDX11.m_Width, i_rDX11.m_Height);
		std::cout << "  " << i_pName << ": " << result.Describe() << std::endl;
		if (!result.m_bPass)
		{
			ADD_FAILURE() << i_pName << ": " << result.Describe() << " (images in "
				<< gtSaveComparison(i_pName, i_rDX11.m_Rgba.data(), i_rVulkan.m_Rgba.data(), i_rDX11.m_Width, i_rDX11.m_Height) << ")";
		}
	}
}

namespace
{
	const int c_Width = 320, c_Height = 240;

	//	On i_Kind: the texts drawn by a window over dark grey, and into a
	//	render target over dark blue
	void draw_texts(gtBackend::Kind i_Kind, gtPicture& o_Window, gtPicture& o_Target)
	{
		const std::vector<g2dFontHandle> fonts = load_fonts();
		for (g2dFontHandle font : fonts)
			ASSERT_TRUE(font != NULL) << "no font (" << gtBackend::Name(i_Kind) << ")";
		{
			gtSceneWindow window(i_Kind, c_Width, c_Height);
			g2dWindow& w = window.Window();
			w.BeginScene();
			w.MakeCurrent();
			w.Clear(g2dRGBColor(30, 30, 30));
			for (size_t i = 0; i < fonts.size(); i++)
			{
				const gtText& text = k_Texts[i];
				w.DrawText(text.m_X, text.m_Y, fonts[i], std::string(text.m_pText), text.m_Color);
			}
			w.EndScene();
			o_Window = window.Present();
		}
		{
			const float blue[4] = { 0.05f, 0.1f, 0.3f, 1.0f };
			gtTarget target(i_Kind, c_Width, c_Height, blue);
			for (size_t i = 0; i < fonts.size(); i++)
			{
				const gtText& text = k_Texts[i];
				draw_into_target(i_Kind, text.m_X, text.m_Y, fonts[i], std::string(text.m_pText), text.m_Color);
			}
			o_Target = target.Read();
		}
		release_fonts(fonts);
	}

	//	GetTextSizeInPixels of a few strings in each font, flattened
	std::vector<int> text_sizes()
	{
		const char* strings[] = { "", "x", "Hello, MachStudio", "frame 42 / 120" };
		std::vector<int> result;
		const std::vector<g2dFontHandle> fonts = load_fonts();
		for (g2dFontHandle font : fonts)
		{
			for (const char* s : strings)
			{
				int w = -1, h = -1;
				g2dFontUtil::GetTextSizeInPixels(font, std::string(s), w, h);
				result.push_back(w);
				result.push_back(h);
			}
		}
		release_fonts(fonts);
		return result;
	}

	//	Whether LoadFont gives no font when the global bitmap doesn't exist
	//	(and the bitmap path reads back as set)
	bool no_bitmap_no_font()
	{
		const fsLocator missing(std::string("no-such-font-bitmap.png"));
		g2dFontUtil::SetGlobalFontBitmap(missing);
		g2dFontHandle font = g2dFontUtil::LoadFont(std::string("Lucida"), 10);
		fsLocator path;
		g2dFontUtil::GetGlobalFontBitmap(path);
		EXPECT_TRUE(path == missing);
		g2dFontUtil::ReleaseFont(font);
		return font == NULL;
	}
}

TEST(g2dFontVk, TextMatchesDX11)
{
	gtPicture dx11_window, dx11_target, vulkan_window, vulkan_target;
	gtBackend::Run(gtBackend::e_DX11, [&]() { draw_texts(gtBackend::e_DX11, dx11_window, dx11_target); });
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { draw_texts(gtBackend::e_Vulkan, vulkan_window, vulkan_target); }))
		return;
	expect_match("TextMatchesDX11_Window", dx11_window, vulkan_window);
	expect_match("TextMatchesDX11_Target", dx11_target, vulkan_target);
}

TEST(g2dFontVk, TextSizeMatchesDX11)
{
	std::vector<int> dx11, vulkan;
	gtBackend::Run(gtBackend::e_DX11, [&]() { dx11 = text_sizes(); });
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = text_sizes(); }))
		return;
	ASSERT_FALSE(dx11.empty());
	EXPECT_EQ(dx11, vulkan);
}

TEST(g2dFontVk, NoBitmapNoFontAsDX11)
{
	bool dx11 = false, vulkan = false;
	gtBackend::Run(gtBackend::e_DX11, [&]() { dx11 = no_bitmap_no_font(); });
	EXPECT_TRUE(dx11) << "DX11 gave a font";
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = no_bitmap_no_font(); }))
		return;
	EXPECT_TRUE(vulkan) << "Vulkan gave a font";
}

namespace
{
	//	Counts the fonts g2dFontUtil loads and releases through it
	class gtCountingFonts : public g2dFontUtilImpl
	{
		public:
			int m_Loads = 0;
			int m_Releases = 0;

			g2dFontHandle LoadFont(const std::string&, int) override
			{
				++m_Loads;
				return reinterpret_cast<g2dFontHandle>((intptr_t)m_Loads);
			}
			void ReleaseFont(g2dFontHandle) override									{ ++m_Releases; }
			void GetLoadedFontName(g2dFontHandle, std::string& o_Name) override		{ o_Name.clear(); }
			void GetTextSizeInPixels(g2dFontHandle, const std::string&, int& o_Width, int& o_Height) override
			{
				o_Width = o_Height = 0;
			}
			void SetGlobalFontBitmap(const fsLocator&) override						{}
			void GetGlobalFontBitmap(fsLocator& o_FontPath) override					{ o_FontPath = fsLocator(); }
	};
}

//	A font is loaded once while it is in use, and unloaded (and forgotten)
//	when the last user releases it, so a later LoadFont loads it again from
//	the implementation that is installed then (a backend's font util; the
//	tests run one backend after the other in one process)
TEST(g2dFontUtil, LastReleaseUnloadsTheFont)
{
	g2dFontUtilImpl* previous = g2dFontUtil::Implementation();
	gtCountingFonts fonts;
	g2dFontUtil::SetImplementation(&fonts);
	const std::string name("gtCountingFonts");

	g2dFontHandle a = g2dFontUtil::LoadFont(name, 11);
	g2dFontHandle b = g2dFontUtil::LoadFont(name, 11);
	EXPECT_EQ(a, b);
	EXPECT_EQ(1, fonts.m_Loads);
	g2dFontUtil::ReleaseFont(a);
	EXPECT_EQ(0, fonts.m_Releases);
	g2dFontUtil::ReleaseFont(b);
	EXPECT_EQ(1, fonts.m_Releases);

	g2dFontHandle c = g2dFontUtil::LoadFont(name, 11);
	EXPECT_EQ(2, fonts.m_Loads);
	g2dFontUtil::ReleaseFont(c);
	EXPECT_EQ(2, fonts.m_Releases);

	g2dFontUtil::SetImplementation(previous);
}
