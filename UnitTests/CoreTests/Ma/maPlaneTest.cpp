/*****************************************************************************
**  maPlaneTest.cpp
**
**      maPlane construction and signed point distances.
**
**      Ported from TestTerawatt/Math/ma/maTestMain.cpp (TestmaPlane),
**      which printed the results.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctMath.hpp"

#ifndef MA_PLANE_HPP
#include "Core/ma/maPlane.hpp"
#endif

#include <gtest/gtest.h>

#include <cmath>

TEST(maPlane, FromEquation)
{
	maPoint3d normal(4, 9, 2);
	normal.Normalize();
	maPlane plane(normal.GetX(), normal.GetY(), normal.GetZ(), 10.0f);

	EXPECT_TRUE(ctNear(normal, plane.GetNormal()));
	EXPECT_EQ(10.0f, plane.GetValue());
	EXPECT_NEAR(0.0f, plane.TestPoint(normal * 10.0f), c_ctTolerance);
	EXPECT_NEAR(-10.0f, plane.TestPoint(maPoint3d(0, 0, 0)), c_ctTolerance);
}

TEST(maPlane, FromThreePointsContainsThem)
{
	maPoint3d p1(3, 9, 2), p2(4, 4, 9), p3(-4, 9, 2);
	maPlane plane(p1, p2, p3);

	EXPECT_NEAR(1.0f, plane.GetNormal().Length(), c_ctTolerance);
	EXPECT_NEAR(0.0f, plane.TestPoint(p1), c_ctTolerance);
	EXPECT_NEAR(0.0f, plane.TestPoint(p2), c_ctTolerance);
	EXPECT_NEAR(0.0f, plane.TestPoint(p3), c_ctTolerance);
}

//	The plane through the three unit axis points has normal (1,1,1)/sqrt(3)
//	(the winding (p2 - p1) x (p3 - p2)), so distances are signed along it.
//
TEST(maPlane, SignedDistances)
{
	maPlane plane;
	plane.Set(maPoint3d(1, 0, 0), maPoint3d(0, 1, 0), maPoint3d(0, 0, 1));

	const float root3 = std::sqrt(3.0f);
	EXPECT_TRUE(ctNear(maVector3d(1, 1, 1) / root3, plane.GetNormal()));
	EXPECT_NEAR(1.0f / root3, plane.GetValue(), c_ctTolerance);

	EXPECT_NEAR(6.0f / root3, plane.TestPoint(maPoint3d(3, 4, 0)), c_ctTolerance);
	EXPECT_NEAR(-1.0f / root3, plane.TestPoint(maPoint3d(0, 0, 0)), c_ctTolerance);
	EXPECT_NEAR(2.0f / root3, plane.TestPoint(maPoint3d(1, 1, 1)), c_ctTolerance);
}

TEST(maPlane, FromDirectionAndPoint)
{
	maPlane plane(maPoint3d(0, 1, 0), maPoint3d(5, 2, -7));

	EXPECT_NEAR(0.0f, plane.TestPoint(maPoint3d(-100, 2, 100)), c_ctTolerance);
	EXPECT_NEAR(3.0f, plane.TestPoint(maPoint3d(0, 5, 0)), c_ctTolerance);
}
