/*****************************************************************************
**  envNumberTextTest.cpp
**
**      envNumberText: every number type reads back exactly from its text,
**      the text it writes, and how leniently it reads text written by
**      older versions.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctNumbers.hpp"

#ifndef ENV_NUMBERTEXT_HPP
#include "Core/env/envNumberText.hpp"
#endif

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <string>

template<class T>
class envNumberTextTyped : public testing::Test
{
};
TYPED_TEST_SUITE(envNumberTextTyped, ctNumberTypes);

TYPED_TEST(envNumberTextTyped, RoundTrip)
{
	for( TypeParam value : ctNumberValues<TypeParam>() )
	{
		std::string text = envNumberText::ToText(value);
		SCOPED_TRACE(text);

		TypeParam read = TypeParam(42);
		ASSERT_TRUE(envNumberText::FromText(text, read));
		EXPECT_TRUE(ctSameNumber(value, read));
	}
}

TYPED_TEST(envNumberTextTyped, RejectsTextThatIsNotANumber)
{
	for( const char* text : { "", "   ", "abc", "-", "+", ".", "e5", "#1" } )
	{
		SCOPED_TRACE(text);
		TypeParam read = TypeParam(42);
		EXPECT_FALSE(envNumberText::FromText(text, read));
		EXPECT_TRUE(ctSameNumber(TypeParam(42), read));
	}

	TypeParam read = TypeParam(42);
	EXPECT_FALSE(envNumberText::FromText(nullptr, read));
}

//	Int8 and UInt8 are characters to an ostream; they must be written
//	as numbers.
//
TEST(envNumberText, ToTextWritesBytesAsNumbers)
{
	EXPECT_EQ("-100", envNumberText::ToText(envType::Int8(-100)));
	EXPECT_EQ("-128", envNumberText::ToText(envType::Int8(-128)));
	EXPECT_EQ("0", envNumberText::ToText(envType::Int8(0)));
	EXPECT_EQ("65", envNumberText::ToText(envType::UInt8('A')));
	EXPECT_EQ("200", envNumberText::ToText(envType::UInt8(200)));
	EXPECT_EQ("255", envNumberText::ToText(envType::UInt8(255)));
}

TEST(envNumberText, ToTextWritesWholeRange)
{
	EXPECT_EQ("-32768", envNumberText::ToText(envType::Int16(-32768)));
	EXPECT_EQ("65535", envNumberText::ToText(envType::UInt16(65535)));
	EXPECT_EQ("-2147483648", envNumberText::ToText(std::numeric_limits<envType::Int32>::lowest()));
	EXPECT_EQ("4294967295", envNumberText::ToText(std::numeric_limits<envType::UInt32>::max()));
	EXPECT_EQ("-9223372036854775808", envNumberText::ToText(std::numeric_limits<envType::Int64>::lowest()));
	EXPECT_EQ("18446744073709551615", envNumberText::ToText(std::numeric_limits<envType::UInt64>::max()));
}

//	Floats use as few digits as read back exactly, so ordinary values look
//	the way they did with an ostream's 6 digits.
//
TEST(envNumberText, ToTextWritesShortestFloats)
{
	EXPECT_EQ("0", envNumberText::ToText(0.0f));
	EXPECT_EQ("-0", envNumberText::ToText(-0.0f));
	EXPECT_EQ("1", envNumberText::ToText(1.0f));
	EXPECT_EQ("0.1", envNumberText::ToText(0.1f));
	EXPECT_EQ("0.1", envNumberText::ToText(0.1));
	EXPECT_EQ("123.456", envNumberText::ToText(123.456f));
	EXPECT_EQ("100", envNumberText::ToText(100.0f));
	EXPECT_EQ("1e+06", envNumberText::ToText(1e6f));
	EXPECT_EQ("1e-05", envNumberText::ToText(1e-5f));

	// more than an ostream's 6 digits
	EXPECT_EQ("0.33333334", envNumberText::ToText(1.0f / 3.0f));
	EXPECT_EQ("0.3333333333333333", envNumberText::ToText(1.0 / 3.0));
	EXPECT_EQ("16777216", envNumberText::ToText(16777216.0f));
	EXPECT_EQ("3.4028235e+38", envNumberText::ToText(std::numeric_limits<float>::max()));
}

TEST(envNumberText, ToTextWritesSpecialFloats)
{
	EXPECT_EQ("inf", envNumberText::ToText(std::numeric_limits<float>::infinity()));
	EXPECT_EQ("-inf", envNumberText::ToText(-std::numeric_limits<double>::infinity()));
	EXPECT_EQ("nan", envNumberText::ToText(std::numeric_limits<float>::quiet_NaN()));
	EXPECT_EQ("-nan", envNumberText::ToText(std::copysign(std::numeric_limits<double>::quiet_NaN(), -1.0)));

	// 0/0 is a negative NaN on x64, which to_chars writes as "-nan(ind)"
	volatile double zero = 0.0;
	std::string text = envNumberText::ToText(zero / zero);
	EXPECT_TRUE(text == "nan" || text == "-nan") << text;
}

//	FromText reads like the atoi/atof and stream code it replaced: it
//	skips leading whitespace, takes a '+', and stops at the end of the
//	number.
//
TEST(envNumberText, FromTextIsLenient)
{
	envType::Int32 i = 0;
	EXPECT_TRUE(envNumberText::FromText(" \t42", i));
	EXPECT_EQ(42, i);
	EXPECT_TRUE(envNumberText::FromText("+7", i));
	EXPECT_EQ(7, i);
	EXPECT_TRUE(envNumberText::FromText("12abc", i));
	EXPECT_EQ(12, i);
	EXPECT_TRUE(envNumberText::FromText("1.9", i));
	EXPECT_EQ(1, i);

	envType::Float32 f = 0;
	EXPECT_TRUE(envNumberText::FromText("  +2.5", f));
	EXPECT_EQ(2.5f, f);
	EXPECT_TRUE(envNumberText::FromText("1E6", f));
	EXPECT_EQ(1e6f, f);
	EXPECT_TRUE(envNumberText::FromText(".5", f));
	EXPECT_EQ(0.5f, f);
	EXPECT_TRUE(envNumberText::FromText("7", f));
	EXPECT_EQ(7.0f, f);
	EXPECT_TRUE(envNumberText::FromText("0.123456", f));	// as an ostream wrote it
	EXPECT_EQ(0.123456f, f);
	EXPECT_TRUE(envNumberText::FromText("-nan(ind)", f));
	EXPECT_TRUE(std::isnan(f));
	EXPECT_TRUE(envNumberText::FromText("INF", f));
	EXPECT_EQ(std::numeric_limits<float>::infinity(), f);
}

//	Integers out of range wrap, as atoi and a cast did; floats out of
//	range become infinity or zero.
//
TEST(envNumberText, FromTextOutOfRange)
{
	envType::UInt8 u8 = 0;
	EXPECT_TRUE(envNumberText::FromText("-1", u8));
	EXPECT_EQ(255, u8);
	EXPECT_TRUE(envNumberText::FromText("300", u8));
	EXPECT_EQ(44, u8);

	envType::Int8 i8 = 0;
	EXPECT_TRUE(envNumberText::FromText("200", i8));
	EXPECT_EQ(-56, i8);

	envType::UInt64 u64 = 0;
	EXPECT_TRUE(envNumberText::FromText("-1", u64));
	EXPECT_EQ(std::numeric_limits<envType::UInt64>::max(), u64);

	envType::Float32 f = 0;
	EXPECT_TRUE(envNumberText::FromText("1e300", f));
	EXPECT_EQ(std::numeric_limits<float>::infinity(), f);
	EXPECT_TRUE(envNumberText::FromText("-1e300", f));
	EXPECT_EQ(-std::numeric_limits<float>::infinity(), f);
	EXPECT_TRUE(envNumberText::FromText("1e-300", f));
	EXPECT_EQ(0.0f, f);

	envType::Float64 d = 0;
	EXPECT_TRUE(envNumberText::FromText("1e400", d));
	EXPECT_EQ(std::numeric_limits<double>::infinity(), d);
}

//	A float reads directly as a float, not rounded twice through a
//	double.
//
TEST(envNumberText, FromTextRoundsFloatOnce)
{
	// halfway between two floats, nudged up: the nearest double is the
	// halfway point itself, which then rounds (to even) the wrong way
	envType::Float32 f = 0;
	ASSERT_TRUE(envNumberText::FromText("1.00000005960464477550", f));
	EXPECT_EQ(1.00000012f, f);
}
