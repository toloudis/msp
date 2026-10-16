/*****************************************************************************
**  ctNumbers.hpp
**
**      Values that are hard to store as text, for each envType number
**      type, and an exact comparison, for the read/write tests:
**
**          TYPED_TEST_SUITE(MyTest, ctNumberTypes);
**          for( TypeParam value : ctNumberValues<TypeParam>() ) ...
**              EXPECT_TRUE(ctSameNumber(value, read_back));
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <sstream>
#include <type_traits>
#include <vector>

using ctNumberTypes = testing::Types<
	envType::Int8, envType::UInt8, envType::Int16, envType::UInt16,
	envType::Int32, envType::UInt32, envType::Int64, envType::UInt64,
	envType::Float32, envType::Float64>;

//	ctNumberValues returns the limits of T, values near them and near
//	zero, and for floats, values that need every significant digit,
//	plain and scientific notation, denormals, signed zero, infinities and
//	NaNs.
//
template<class T>
std::vector<T> ctNumberValues()
{
	using limits = std::numeric_limits<T>;
	std::vector<T> values = { T(0), T(1), T(2), T(10), T(100), T(127), limits::max(), T(limits::max() - 1), limits::lowest() };

	if constexpr( std::is_integral_v<T> )
	{
		values.push_back(T(limits::max() / 3));
		if constexpr( std::is_signed_v<T> )
		{
			values.insert(values.end(), { T(-1), T(-2), T(-100), T(-128), T(limits::lowest() + 1), T(limits::lowest() / 3) });
		}
	}
	else
	{
		values.insert(values.end(), {
			T(-1), T(0.5), T(-0.5), T(0.1), T(-0.1), T(1) / T(3), T(2) / T(3), T(123.456), T(-123.456),
			T(1e-5), T(-1e-5), T(1e-30), T(1e6), T(1e30), T(-1e30), T(16777216), T(16777217),
			T(3.14159265358979323846), T(2.71828182845904523536),
			limits::epsilon(), limits::min(), limits::denorm_min(), -limits::denorm_min(), -limits::max(),
			-T(0), limits::infinity(), -limits::infinity(),
			limits::quiet_NaN(), std::copysign(limits::quiet_NaN(), T(-1)) });

		if constexpr( std::is_same_v<T, envType::Float64> )
		{
			values.insert(values.end(), { T(9007199254740991.0), T(1e-300), T(1e300), T(0.1f), T(1.0000000000000002) });
		}
	}

	return values;
}

//	ctSameNumber is true when i_Actual is exactly i_Expected: for floats,
//	also with the same sign of zero, and any NaN matches a NaN.
//
template<class T>
testing::AssertionResult ctSameNumber(T i_Expected, T i_Actual)
{
	bool same;
	if constexpr( std::is_floating_point_v<T> )
	{
		if( std::isnan(i_Expected) )
			same = std::isnan(i_Actual);
		else
			same = (i_Expected == i_Actual) && (std::signbit(i_Expected) == std::signbit(i_Actual));
	}
	else
	{
		same = (i_Expected == i_Actual);
	}

	if( same )
		return testing::AssertionSuccess();

	// all the digits, so values that differ in the last bit print
	// differently; unary + prints Int8 and UInt8 as numbers
	std::ostringstream ss;
	ss.precision(std::numeric_limits<T>::max_digits10);
	ss << "expected " << +i_Expected << ", got " << +i_Actual;
	return testing::AssertionFailure() << ss.str();
}
