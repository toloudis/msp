/*****************************************************************************
**  maFunctionsTest.cpp
**
**      maFunctions helpers, the maRand32 generator and maRunningAverage.
**
**      Ported from TestTerawatt/Math/ma/maTestMain.cpp; its timing
**      comparisons with rand() are left out.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef MA_FUNCTIONS_HPP
#include "Core/ma/maFunctions.hpp"
#endif
#ifndef MA_RAND_HPP
#include "Core/ma/maRand.hpp"
#endif
#ifndef MA_RUNNINGAVERAGE_HPP
#include "Core/ma/maRunningAverage.hpp"
#endif

#include <gtest/gtest.h>

#include <set>
#include <vector>

TEST(maFunctions, HighestLowestSwapClamp)
{
	EXPECT_EQ(10.0f, maFunctions::Highest(1.0f, 10.0f));
	EXPECT_EQ(1.0f, maFunctions::Lowest(1.0f, 10.0f));
	EXPECT_EQ(7, maFunctions::Highest(3, 7, 5));
	EXPECT_EQ(3, maFunctions::Lowest(3, 7, 5));

	float a = 1.0f, b = 10.0f;
	maFunctions::Swap(a, b);
	EXPECT_EQ(10.0f, a);
	EXPECT_EQ(1.0f, b);

	EXPECT_EQ(5, maFunctions::Clamp(12, 0, 5));
	EXPECT_EQ(0, maFunctions::Clamp(-3, 0, 5));
	EXPECT_EQ(3, maFunctions::Clamp(3, 0, 5));
}

TEST(maFunctions, Interpolate)
{
	EXPECT_FLOAT_EQ(12.5f, maFunctions::Interpolate(10, 20, 0.25f));
	EXPECT_FLOAT_EQ(15.0f, maFunctions::Interpolate(10, 20, 3, 2, 4));
}

TEST(maFunctions, HalfFloatRoundTrip)
{
	// exactly representable in 16 bits
	for( float value : { 0.0f, 1.0f, -2.5f, 0.5f, 1024.0f } )
		EXPECT_EQ(value, maFunctions::HalfToFloat(maFunctions::FloatToHalf(value))) << value;
}

TEST(maFunctions, FloatRandStaysInRange)
{
	struct Range { float m_Lo, m_Hi; };
	for( Range range : { Range{ 0, 1 }, Range{ 1, 100 }, Range{ -100, 100 } } )
	{
		for( int i = 0 ; i < 100000 ; i++ )
		{
			float value = maFunctions::FloatRand(range.m_Lo, range.m_Hi);
			ASSERT_GE(value, range.m_Lo);
			ASSERT_LE(value, range.m_Hi);
		}
	}
}

TEST(maRand32, SameSeedSameSequence)
{
	maRand32 a(0x12345678), b(0x12345678), c(0x87654321);

	std::vector<uint32_t> from_a, from_b, from_c;
	for( int i = 0 ; i < 1000 ; i++ )
	{
		from_a.push_back(a.Rand());
		from_b.push_back(b.Rand());
		from_c.push_back(c.Rand());
	}

	EXPECT_EQ(from_a, from_b);
	EXPECT_NE(from_a, from_c);

	// not stuck on a few values
	EXPECT_GT(std::set<uint32_t>(from_a.begin(), from_a.end()).size(), 990u);
}

TEST(maRand32, RangedRandStaysInRange)
{
	maRand32 gen(0x2468ace0);
	for( int i = 0 ; i < 100000 ; i++ )
	{
		uint32_t value = gen.Rand(10, 20);
		ASSERT_GE(value, 10u);
		ASSERT_LE(value, 20u);
	}
}

TEST(maRand32, FloatRandWithGeneratorIsRepeatable)
{
	maRand32 a(99), b(99);
	for( int i = 0 ; i < 100 ; i++ )
	{
		float value = maFunctions::FloatRand(-1, 1, a);
		EXPECT_EQ(value, maFunctions::FloatRand(-1, 1, b));
		EXPECT_GE(value, -1.0f);
		EXPECT_LE(value, 1.0f);
	}
}

TEST(maRunningAverage, AveragesTheLastValues)
{
	maRunningAverage average(3);

	average.Push(1);
	average.Push(2);
	average.Push(3);
	EXPECT_FLOAT_EQ(2.0f, average.GetAverage());

	average.Push(6);	// drops the 1
	EXPECT_FLOAT_EQ(11.0f / 3.0f, average.GetAverage());

	for( int i = 0 ; i < 100 ; ++i )
		average.Push(float(i % 4));
	// last three pushed: 97 % 4, 98 % 4, 99 % 4 = 1, 2, 3
	EXPECT_FLOAT_EQ(2.0f, average.GetAverage());
}
