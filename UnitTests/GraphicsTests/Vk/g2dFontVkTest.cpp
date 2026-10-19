/*****************************************************************************
**  g2dFontVkTest.cpp
**
**      g2dFontUtilVk: the bitmap font (font-lucd00.png, an scrText sprite
**      drawn with the font shaders) against GraphicsDX11's, with the whole
**      layer up on each backend (gtBackend): text drawn by a window
**      (g2dWindow::DrawText) and into a render target (g2dWindowDrawUtil's
**      DrawText, which g3dRenderPass uses), at several sizes, colours and
**      offsets; GetTextSizeInPixels; and no font without the bitmap.
**      GraphicsDX11 can load the font bitmap safely in only one run per
**      process (see in_dx11), so each DX11 half runs in a gtest death-test
**      child process.
**      Skipped without a Vulkan device.
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
#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif
#ifndef G2D_WINDOWDRAWUTILD3D11_HPP
#include "GraphicsDX11/g2d/private/g2dWindowDrawUtilDX11.hpp"
#endif
#ifndef GVK_G2DWINDOWDRAWUTILVK_HPP
#include "GraphicsVulkan/G2d/private/g2dWindowDrawUtilVk.hpp"
#endif

#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#undef DrawText

namespace
{
	fsLocator font_bitmap()
	{
		const std::filesystem::path path = std::filesystem::u8path(gtPlatform::RepoRoot()) / "MachStudio" / "MachStudio-x64" / "Data" /
			"Icons" / "font-lucd00.png";
		fsLocator locator;
		fsFileUtil::UnicodeStringToLocator(itString(path.wstring().c_str()), locator);
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
			fonts.push_back(g2dFontUtil::LoadFont(itString("Lucida"), text.m_Size));
		return fonts;
	}

	void release_fonts(const std::vector<g2dFontHandle>& i_Fonts)
	{
		for (g2dFontHandle font : i_Fonts)
			g2dFontUtil::ReleaseFont(font);
	}

	void draw_into_target(gtBackend::Kind i_Kind, int i_X, int i_Y, g2dFontHandle i_Font, const itString& i_Text,
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
				w.DrawText(text.m_X, text.m_Y, fonts[i], itString(text.m_pText), text.m_Color);
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
				draw_into_target(i_Kind, text.m_X, text.m_Y, fonts[i], itString(text.m_pText), text.m_Color);
			}
			o_Target = target.Read();
		}
		release_fonts(fonts);
	}

	//	The failures the current test has had so far
	int failures()
	{
		const testing::TestResult* result = testing::UnitTest::GetInstance()->current_test_info()->result();
		int n = 0;
		for (int i = 0; i < result->total_part_count(); i++)
			n += result->GetTestPartResult(i).failed() ? 1 : 0;
		return n;
	}

	typedef std::function<std::vector<uint8_t>()> Dx11Function;

	//	The child process's half of in_dx11: runs i_Function with DX11 up,
	//	writes what it returned to i_File and exits (0 if all went well).
	//	(The child runs the test from the start, so failures before this
	//	one aren't its own.)
	void dx11_child(const Dx11Function& i_Function, const std::filesystem::path& i_File)
	{
		const int before = failures();
		std::vector<uint8_t> bytes;
		bool ok = gtBackend::Run(gtBackend::e_DX11, [&]() { bytes = i_Function(); }) && failures() == before;
		if (ok)
		{
			std::ofstream out(i_File, std::ios::binary);
			out.write((const char*)bytes.data(), bytes.size());
			ok = out.good();
		}
		if (!ok)
			std::cerr << "the DX11 half failed (its output is above)" << std::endl;
		std::cout.flush();
		std::exit(ok ? 0 : 1);
	}

	//	What i_Function returns with GraphicsDX11 up. After a GraphicsDX11
	//	run that loaded the font bitmap, a later run in the same process can
	//	crash loading it again (depending on what ran before; the cause is
	//	in GraphicsDX11, which MachStudio starts once per process), so each
	//	DX11 half runs in a process of its own (a gtest death test child)
	//	and hands its result back in a file.
	std::vector<uint8_t> in_dx11(const Dx11Function& i_Function)
	{
		gtTempDir dir;
		const std::filesystem::path file = dir.GetPath() / "dx11.bin";
		EXPECT_EXIT(dx11_child(i_Function, file), ::testing::ExitedWithCode(0), "");
		std::ifstream in(file, std::ios::binary);
		return std::vector<uint8_t>(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	}

	//	Pictures and ints to and from bytes
	void put_int(std::vector<uint8_t>& io_Bytes, int i_Value)
	{
		const uint8_t* p = (const uint8_t*)&i_Value;
		io_Bytes.insert(io_Bytes.end(), p, p + sizeof(int));
	}

	bool get_int(const std::vector<uint8_t>& i_Bytes, size_t& io_At, int& o_Value)
	{
		if (io_At + sizeof(int) > i_Bytes.size())
			return false;
		memcpy(&o_Value, &i_Bytes[io_At], sizeof(int));
		io_At += sizeof(int);
		return true;
	}

	void put_picture(std::vector<uint8_t>& io_Bytes, const gtPicture& i_rPicture)
	{
		put_int(io_Bytes, i_rPicture.m_Width);
		put_int(io_Bytes, i_rPicture.m_Height);
		io_Bytes.insert(io_Bytes.end(), i_rPicture.m_Rgba.begin(), i_rPicture.m_Rgba.end());
	}

	gtPicture get_picture(const std::vector<uint8_t>& i_Bytes, size_t& io_At)
	{
		gtPicture picture;
		int width = 0, height = 0;
		if (!get_int(i_Bytes, io_At, width) || !get_int(i_Bytes, io_At, height) || width <= 0 || height <= 0)
			return picture;
		const size_t size = (size_t)width * height * 4;
		if (io_At + size > i_Bytes.size())
			return picture;
		picture.m_Width = width;
		picture.m_Height = height;
		picture.m_Rgba.assign(i_Bytes.begin() + io_At, i_Bytes.begin() + io_At + size);
		io_At += size;
		return picture;
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
				g2dFontUtil::GetTextSizeInPixels(font, itString(s), w, h);
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
		const fsLocator missing(itString("no-such-font-bitmap.png"));
		g2dFontUtil::SetGlobalFontBitmap(missing);
		g2dFontHandle font = g2dFontUtil::LoadFont(itString("Lucida"), 10);
		fsLocator path;
		g2dFontUtil::GetGlobalFontBitmap(path);
		EXPECT_TRUE(path == missing);
		g2dFontUtil::ReleaseFont(font);
		return font == NULL;
	}
}

TEST(g2dFontVk, TextMatchesDX11)
{
	const std::vector<uint8_t> bytes = in_dx11([]()
	{
		gtPicture window, target;
		draw_texts(gtBackend::e_DX11, window, target);
		std::vector<uint8_t> result;
		put_picture(result, window);
		put_picture(result, target);
		return result;
	});
	size_t at = 0;
	const gtPicture dx11_window = get_picture(bytes, at);
	const gtPicture dx11_target = get_picture(bytes, at);
	gtPicture vulkan_window, vulkan_target;
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { draw_texts(gtBackend::e_Vulkan, vulkan_window, vulkan_target); }))
		return;
	expect_match("TextMatchesDX11_Window", dx11_window, vulkan_window);
	expect_match("TextMatchesDX11_Target", dx11_target, vulkan_target);
}

TEST(g2dFontVk, TextSizeMatchesDX11)
{
	const std::vector<uint8_t> bytes = in_dx11([]()
	{
		std::vector<uint8_t> result;
		for (int value : text_sizes())
			put_int(result, value);
		return result;
	});
	std::vector<int> dx11;
	size_t at = 0;
	int value = 0;
	while (get_int(bytes, at, value))
		dx11.push_back(value);
	std::vector<int> vulkan;
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = text_sizes(); }))
		return;
	ASSERT_FALSE(dx11.empty());
	EXPECT_EQ(dx11, vulkan);
}

TEST(g2dFontVk, NoBitmapNoFontAsDX11)
{
	const std::vector<uint8_t> bytes = in_dx11([]()
	{
		return std::vector<uint8_t>(1, no_bitmap_no_font() ? 1 : 0);
	});
	ASSERT_EQ(1u, bytes.size());
	EXPECT_EQ(1, bytes[0]) << "DX11 gave a font";
	bool vulkan = false;
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = no_bitmap_no_font(); }))
		return;
	EXPECT_TRUE(vulkan) << "Vulkan gave a font";
}
