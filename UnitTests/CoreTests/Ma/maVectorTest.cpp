/*****************************************************************************
**  maVectorTest.cpp
**
**      maVector2d/3d/4d (and the maPoint typedefs) arithmetic.
**
**      Ported from TestTerawatt/Math/ma/maTestMain.cpp, which printed the
**      results; the expected values are worked out here.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctMath.hpp"

#ifndef MA_VECTOR2D_HPP
#include "Core/ma/maVector2d.hpp"
#endif

#include <gtest/gtest.h>

#include <cmath>

TEST(maVector2d, Arithmetic)
{
	maVector2d a;
	a.Set(0, 0);
	a.SetX(1);
	a.SetY(1);
	EXPECT_EQ(1.0f, a.GetX());
	EXPECT_EQ(1.0f, a.GetY());

	a = maVector2d(5, 5) + maVector2d(3, 3);
	EXPECT_EQ(maVector2d(8, 8), a);

	a *= 3;
	EXPECT_EQ(maVector2d(24, 24), a);
	EXPECT_FLOAT_EQ(24.0f * std::sqrt(2.0f), a.Length());
	EXPECT_EQ(24.0f, a[0]);

	a.Normalize();
	EXPECT_NEAR(1.0f, a.Length(), c_ctTolerance);
	EXPECT_NEAR(std::sqrt(0.5f), a.GetX(), c_ctTolerance);

	EXPECT_EQ(11.0f, maVector2d(1, 2) * maVector2d(3, 4));	// dot product
}

TEST(maVector3d, Arithmetic)
{
	maVector3d a(5, 5, 5);
	a = a + maVector3d(3, 3, 3);
	EXPECT_EQ(maVector3d(8, 8, 8), a);

	a *= 2;
	EXPECT_EQ(maVector3d(16, 16, 16), a);
	a /= 2;
	EXPECT_EQ(maVector3d(8, 8, 8), a);

	EXPECT_FLOAT_EQ(8.0f * std::sqrt(3.0f), a.Length());
	EXPECT_FLOAT_EQ(192.0f, a.LengthSqr());
	EXPECT_EQ(8.0f, a[0]);

	EXPECT_TRUE(a.Normalize());
	EXPECT_TRUE(ctNear(maVector3d(1, 1, 1) / std::sqrt(3.0f), a));

	EXPECT_EQ(maVector3d(-1, -2, -3), -maVector3d(1, 2, 3));
}

TEST(maVector3d, DotAndCross)
{
	maVector3d x(1, 0, 0), y(0, 1, 0), z(0, 0, 1);

	EXPECT_EQ(0.0f, x.Dot(y));
	EXPECT_EQ(32.0f, maVector3d(1, 2, 3) * maVector3d(4, 5, 6));

	EXPECT_EQ(z, x.Cross(y));
	EXPECT_EQ(x, y.Cross(z));
	EXPECT_EQ(y, z.Cross(x));
	EXPECT_EQ(-z, y.Cross(x));
}

TEST(maVector3d, EqualWithTolerance)
{
	maVector3d a(1, 2, 3);
	EXPECT_TRUE(a.Equal(maVector3d(1.001f, 2, 3), 0.01f));
	EXPECT_FALSE(a.Equal(maVector3d(1.1f, 2, 3), 0.01f));
}

TEST(maVector4d, Arithmetic)
{
	maVector4d a;
	a.Set(0, 0, 0, 1);
	EXPECT_EQ(1.0f, a.GetW());

	maVector4d b;
	b.Set(3, 3, 3);	// W defaults to 1
	EXPECT_EQ(1.0f, b.GetW());

	a = maVector4d(5, 5, 5, 1) + b;
	EXPECT_EQ(maVector4d(8, 8, 8, 2), a);

	a *= 2;
	EXPECT_EQ(maVector4d(16, 16, 16, 4), a);
	a /= 2;
	EXPECT_EQ(maVector4d(8, 8, 8, 2), a);

	EXPECT_FLOAT_EQ(14.0f, a.Length());		// sqrt(64 * 3 + 4)
	EXPECT_EQ(8.0f, a[0]);

	EXPECT_TRUE(a.Normalize());
	EXPECT_TRUE(ctNear(maVector4d(8, 8, 8, 2) / 14.0f, a));
}
