/*****************************************************************************
**  itStringTest.cpp
**
**      itString building, extension handling, searching, and the
**      itStringUtil filename helpers.
**
**      Ported from TestLibXLT/Core/It/itTestMain.cpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/it/itStringUtil.hpp"
#endif

#include <gtest/gtest.h>

#include <string>

namespace
{
	std::string Str(const itString& i_String)
	{
		return itStringUtil::GetStdString(i_String);
	}
}

TEST(itString, Concatenate)
{
	itString base("Base");
	EXPECT_EQ("BaseSuffix", Str(base + itString("Suffix")));

	itString comp = base;
	comp += itString("Suffix");
	comp += itString::CharType('!');
	EXPECT_EQ("BaseSuffix!", Str(comp));
	EXPECT_EQ(11, comp.GetLength());
}

TEST(itString, ReplaceExtension)
{
	itString name("BaseSuffix");
	name.ReplaceExtension(itString("png"));
	EXPECT_EQ("BaseSuffix.png", Str(name));

	name.ReplaceExtension(itString("jpg"));
	EXPECT_EQ("BaseSuffix.jpg", Str(name));

	name.StripExtension();
	EXPECT_EQ("BaseSuffix", Str(name));
}

TEST(itString, BaseAndExtensionUseTheLastPeriod)
{
	itString name("Scene.File.bmp");

	itString base, ext;
	name.GetBase(base);
	name.GetExtension(ext);
	EXPECT_EQ("Scene.File", Str(base));
	EXPECT_EQ("bmp", Str(ext));
}

TEST(itString, Searching)
{
	itString alphabet("abcdefghijklmnopqrstuvwxyz");

	EXPECT_TRUE(alphabet.HasSubString(itString("def")));
	EXPECT_FALSE(alphabet.StartsWith(itString("def")));
	EXPECT_FALSE(alphabet.EndsWith(itString("def")));

	EXPECT_TRUE(alphabet.HasSubString(itString("abc")));
	EXPECT_TRUE(alphabet.StartsWith(itString("abc")));
	EXPECT_FALSE(alphabet.EndsWith(itString("abc")));

	EXPECT_TRUE(alphabet.HasSubString(itString("wxyz")));
	EXPECT_FALSE(alphabet.StartsWith(itString("wxyz")));
	EXPECT_TRUE(alphabet.EndsWith(itString("wxyz")));

	// longer than the string itself
	itString longer("abcdefghijklmnopqrstuvwxyzasdf");
	EXPECT_FALSE(alphabet.HasSubString(longer));
	EXPECT_FALSE(alphabet.StartsWith(longer));
	EXPECT_FALSE(alphabet.EndsWith(longer));

	// an empty string is neither a prefix nor a suffix
	EXPECT_FALSE(alphabet.StartsWith(itString("")));
	EXPECT_FALSE(alphabet.EndsWith(itString("")));
}

TEST(itStringUtil, BreakupNumberedFilename)
{
	itString base, ext;
	int number = -1;
	int digits = itStringUtil::Breakup_Filename(itString("SceneFile015.mab"), base, ext, number);

	EXPECT_EQ("SceneFile", Str(base));
	EXPECT_EQ("mab", Str(ext));
	EXPECT_EQ(15, number);
	EXPECT_EQ(3, digits);
}

TEST(itStringUtil, BreakupUnnumberedFilename)
{
	itString base, ext;
	int number = -1;
	int digits = itStringUtil::Breakup_Filename(itString("Scene.File.bmp"), base, ext, number);

	EXPECT_EQ("Scene.File", Str(base));
	EXPECT_EQ("bmp", Str(ext));
	EXPECT_EQ(0, number);
	EXPECT_EQ(0, digits);
}

TEST(itStringUtil, BuildFilenameReversesBreakup)
{
	itString name;
	itStringUtil::Build_Filename(itString("SceneFile"), itString("mab"), 16, 3, name);
	EXPECT_EQ("SceneFile016.mab", Str(name));

	itString base, ext;
	int number = -1;
	int digits = itStringUtil::Breakup_Filename(name, base, ext, number);
	EXPECT_EQ("SceneFile", Str(base));
	EXPECT_EQ("mab", Str(ext));
	EXPECT_EQ(16, number);
	EXPECT_EQ(3, digits);
}
