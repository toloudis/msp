/*****************************************************************************
**  ctMath.hpp
**
**      Approximate comparisons of ma vectors and matrices for the tests:
**      EXPECT_TRUE(ctNear(expected, actual)) reports both values on
**      failure.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#ifndef MA_MATRIX3X3_HPP
#include "Core/ma/maMatrix3x3.hpp"
#endif
#include "Core/ma/maMatrix4x4.hpp"
#include "Core/ma/maVector3d.hpp"
#include "Core/ma/maVector4d.hpp"

#include <gtest/gtest.h>

#include <cmath>

const float c_ctTolerance = 1e-4f;

inline testing::AssertionResult ctNear(const maVector3d& i_Expected, const maVector3d& i_Actual, float i_Tolerance = c_ctTolerance)
{
	if( std::fabs(i_Expected.GetX() - i_Actual.GetX()) <= i_Tolerance &&
		std::fabs(i_Expected.GetY() - i_Actual.GetY()) <= i_Tolerance &&
		std::fabs(i_Expected.GetZ() - i_Actual.GetZ()) <= i_Tolerance )
		return testing::AssertionSuccess();

	return testing::AssertionFailure() << "expected " << i_Expected << ", got " << i_Actual;
}

inline testing::AssertionResult ctNear(const maVector4d& i_Expected, const maVector4d& i_Actual, float i_Tolerance = c_ctTolerance)
{
	for( int i = 0 ; i < 4 ; i++ )
	{
		if( std::fabs(i_Expected[i] - i_Actual[i]) > i_Tolerance )
		{
			return testing::AssertionFailure() << "expected (" << i_Expected[0] << ", " << i_Expected[1] << ", " << i_Expected[2] << ", " << i_Expected[3]
				<< "), got (" << i_Actual[0] << ", " << i_Actual[1] << ", " << i_Actual[2] << ", " << i_Actual[3] << ")";
		}
	}
	return testing::AssertionSuccess();
}

template <class M>
inline testing::AssertionResult ctNearMatrix(const M& i_Expected, const M& i_Actual, int i_Size, float i_Tolerance = c_ctTolerance)
{
	for( int r = 0 ; r < i_Size ; r++ )
	{
		for( int c = 0 ; c < i_Size ; c++ )
		{
			if( std::fabs(i_Expected(r, c) - i_Actual(r, c)) > i_Tolerance )
			{
				return testing::AssertionFailure() << "element (" << r << ", " << c << "): expected "
					<< i_Expected(r, c) << ", got " << i_Actual(r, c);
			}
		}
	}
	return testing::AssertionSuccess();
}

inline testing::AssertionResult ctNear(const maMatrix3x3& i_Expected, const maMatrix3x3& i_Actual, float i_Tolerance = c_ctTolerance)
{
	return ctNearMatrix(i_Expected, i_Actual, 3, i_Tolerance);
}

inline testing::AssertionResult ctNear(const maMatrix4x4& i_Expected, const maMatrix4x4& i_Actual, float i_Tolerance = c_ctTolerance)
{
	return ctNearMatrix(i_Expected, i_Actual, 4, i_Tolerance);
}

inline maMatrix3x3 ctIdentity3x3()
{
	maMatrix3x3 m;
	m.Identity();
	return m;
}

inline maMatrix4x4 ctIdentity4x4()
{
	maMatrix4x4 m;
	m.Identity();
	return m;
}
