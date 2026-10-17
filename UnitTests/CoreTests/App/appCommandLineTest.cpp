/*****************************************************************************
**  appCommandLineTest.cpp
**
**      appCommandLine tokenizing (whitespace, double quotes) and flag
**      matching.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef APP_COMMANDLINE_HPP
#include "Core/App/appCommandLine.hpp"
#endif

#include <gtest/gtest.h>

#include <string>
#include <vector>

typedef std::vector<std::string> Tokens;

TEST(appCommandLine, EmptyGivesNoTokens)
{
	EXPECT_TRUE(appCommandLine::Tokenize("").empty());
	EXPECT_TRUE(appCommandLine::Tokenize(" \t\n ").empty());
}

TEST(appCommandLine, SplitsOnWhitespace)
{
	EXPECT_EQ((Tokens{"-a", "b", "c"}), appCommandLine::Tokenize("  -a\tb \n c "));
}

TEST(appCommandLine, QuotedKeepsSpaces)
{
	EXPECT_EQ((Tokens{"-file", "C:\\my dir\\a.mab", "-new"}),
			  appCommandLine::Tokenize("-file \"C:\\my dir\\a.mab\" -new"));
}

TEST(appCommandLine, UnterminatedQuoteRunsToEnd)
{
	EXPECT_EQ((Tokens{"-file", "a b"}), appCommandLine::Tokenize("-file \"a b"));
}

TEST(appCommandLine, EmptyQuotesGiveEmptyToken)
{
	EXPECT_EQ((Tokens{"-x", ""}), appCommandLine::Tokenize("-x \"\""));
}

TEST(appCommandLine, IsFlagAcceptsDashAndSlash)
{
	EXPECT_TRUE(appCommandLine::IsFlag("-render", "render"));
	EXPECT_TRUE(appCommandLine::IsFlag("/render", "render"));
}

TEST(appCommandLine, IsFlagIsExact)
{
	EXPECT_FALSE(appCommandLine::IsFlag("-renderX", "render"));
	EXPECT_FALSE(appCommandLine::IsFlag("render", "render"));
	EXPECT_FALSE(appCommandLine::IsFlag("-Render", "render"));
	EXPECT_FALSE(appCommandLine::IsFlag("-", "render"));
}
