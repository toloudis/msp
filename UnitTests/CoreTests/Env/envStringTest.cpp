/*****************************************************************************
**  envStringTest.cpp
**
**      envString's UTF-8 / UTF-16 conversions: round trips, the code units
**      they give, invalid input, and agreement with the platform's wide
**      string conversions. Also checks the build compiles narrow string
**      literals as UTF-8 (/utf-8 in Directory.Build.props).
**
**	Extra Large Technology
\****************************************************************************/
#ifndef ENV_STRING_HPP
#include "Core/env/envString.hpp"
#endif

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace
{

//	Strings the conversions must carry unchanged: ASCII, accented Latin,
//	CJK, characters outside the Basic Multilingual Plane (surrogate pairs
//	in UTF-16), an embedded NUL, and nothing.
const std::vector<std::string> c_Samples =
{
	"plain ASCII",
	u8"Crème brûlée à la façon",
	u8"東京の夜景",
	u8"🎬 clap 🎥",
	std::string("before\0after", 12),
	"",
};

const char16_t c_Replacement = 0xFFFD;

}	// namespace

TEST(envString, NarrowLiteralsAreUTF8)
{
	// without /utf-8, "é" would be the one byte E9 (Windows-1252)
	EXPECT_EQ(std::string("\xC3\xA9"), std::string("é"));
	EXPECT_EQ(std::string("\xE6\x9D\xB1"), std::string("東"));
}

TEST(envString, RoundTrip)
{
	for( const std::string& sample : c_Samples )
	{
		SCOPED_TRACE(sample);
		EXPECT_EQ(sample, envString::UTF16ToUTF8(envString::UTF8ToUTF16(sample)));
	}
}

TEST(envString, CodeUnits)
{
	EXPECT_EQ(u"plain", envString::UTF8ToUTF16("plain"));
	EXPECT_EQ(std::u16string(1, char16_t(0x00E9)), envString::UTF8ToUTF16(u8"é"));
	EXPECT_EQ(std::u16string(1, char16_t(0x6771)), envString::UTF8ToUTF16(u8"東"));
	EXPECT_EQ((std::u16string{ char16_t(0xD83C), char16_t(0xDFAC) }), envString::UTF8ToUTF16(u8"🎬"));
	EXPECT_EQ((std::u16string{ char16_t(0xDBFF), char16_t(0xDFFF) }), envString::UTF8ToUTF16("\xF4\x8F\xBF\xBF"));	// U+10FFFF

	EXPECT_EQ(std::string(u8"é"), envString::UTF16ToUTF8(std::u16string(1, char16_t(0x00E9))));
	EXPECT_EQ(std::string(u8"🎬"), envString::UTF16ToUTF8(std::u16string{ char16_t(0xD83C), char16_t(0xDFAC) }));
}

TEST(envString, EmbeddedNulKept)
{
	const std::u16string utf16 = envString::UTF8ToUTF16(std::string("a\0b", 3));
	ASSERT_EQ(3u, utf16.size());
	EXPECT_EQ(char16_t(0), utf16[1]);
	EXPECT_EQ(3u, envString::UTF16ToUTF8(utf16).size());
}

TEST(envString, InvalidUTF8BecomesReplacement)
{
	struct Case
	{
		std::string m_Input;
		std::u16string m_Expected;
	};
	const char16_t R = c_Replacement;
	const std::vector<Case> cases =
	{
		{ "\x80", { R } },									// stray continuation byte
		{ "a\xC3", { u'a', R } },							// lead byte at the end
		{ "\xC3" "A", { R, u'A' } },						// lead byte, then not a continuation
		{ "\xE2\x82" "A", { R, u'A' } },					// 3-byte sequence cut short: one replacement
		{ "\xF0\x9F\x8E" "x", { R, u'x' } },				// 4-byte sequence cut short
		{ "\xC0\xAF", { R, R } },							// overlong '/'
		{ "\xE0\x80\xAF", { R, R, R } },					// overlong '/'
		{ "\xED\xA0\x80", { R, R, R } },					// encoded surrogate D800
		{ "\xF4\x90\x80\x80", { R, R, R, R } },				// above U+10FFFF
		{ "\xF5\x80", { R, R } },							// F5..FF never occur
		{ "\xFF" "ok", { R, u'o', u'k' } },
	};
	for( const Case& c : cases )
	{
		SCOPED_TRACE(testing::PrintToString(c.m_Input));
		EXPECT_EQ(c.m_Expected, envString::UTF8ToUTF16(c.m_Input));
	}
}

TEST(envString, UnpairedSurrogateBecomesReplacement)
{
	const std::string r = u8"�";
	EXPECT_EQ(r, envString::UTF16ToUTF8(std::u16string{ char16_t(0xD83C) }));						// high, at the end
	EXPECT_EQ(r + "a", envString::UTF16ToUTF8(std::u16string{ char16_t(0xDFAC), u'a' }));		// low, alone
	EXPECT_EQ(r + u8"🎬", envString::UTF16ToUTF8(std::u16string{ char16_t(0xD83C), char16_t(0xD83C), char16_t(0xDFAC) }));
}

//	Where wchar_t is UTF-16 (Windows), the platform conversions
//	(UTF8ToWideChar, WideCharToUTF8) and the portable ones agree on valid
//	text.
TEST(envString, AgreesWithWideConversions)
{
	if( sizeof(wchar_t) != sizeof(char16_t) )
		GTEST_SKIP() << "wchar_t isn't UTF-16 on this platform";

	for( const std::string& sample : c_Samples )
	{
		if( sample.find('\0') != std::string::npos )
			continue;	// the wide conversions stop at a NUL
		SCOPED_TRACE(sample);

		const std::u16string utf16 = envString::UTF8ToUTF16(sample);
		const std::wstring wide = envString::UTF8ToWideChar(sample);
		EXPECT_EQ(std::wstring(utf16.begin(), utf16.end()), wide);
		EXPECT_EQ(envString::WideCharToUTF8(wide), envString::UTF16ToUTF8(utf16));
	}
}
