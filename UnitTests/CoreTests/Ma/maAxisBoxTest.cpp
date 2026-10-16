/*****************************************************************************
**  maAxisBoxTest.cpp
**
**      maAxisBox construction, unions, containment and overlap.
**
**      Ported from TestTerawatt/Math/ma/maTestMain.cpp (TestmaAxisBox),
**      which printed the results.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctMath.hpp"

#include "Core/ma/maAxisBox.hpp"

#include <gtest/gtest.h>

TEST(maAxisBox, DefaultIsEmpty)
{
	maAxisBox box;
	EXPECT_TRUE(box.IsEmpty());
	EXPECT_FALSE(box.HasVolume());
	EXPECT_FALSE(box.ContainsPoint(maPoint3d(0, 0, 0)));
}

TEST(maAxisBox, SetSortsTheCorners)
{
	maAxisBox box;
	box.Set(maPoint3d(4, 9, -3), maPoint3d(-3, 8, -8));

	EXPECT_FALSE(box.IsEmpty());
	EXPECT_TRUE(ctNear(maVector3d(-3, 8, -8), box.GetMin()));
	EXPECT_TRUE(ctNear(maVector3d(4, 9, -3), box.GetMax()));
	EXPECT_TRUE(ctNear(maVector3d(7, 1, 5), box.GetSize()));
	EXPECT_FLOAT_EQ(35.0f, box.GetVolume());
	EXPECT_TRUE(ctNear(maPoint3d(0.5f, 8.5f, -5.5f), box.GetCenter()));
}

TEST(maAxisBox, Unions)
{
	maAxisBox box(maPoint3d(4, 9, -3), maPoint3d(-3, 8, -8));

	box.Union(maPoint3d(23, 9, 3));
	EXPECT_TRUE(ctNear(maVector3d(-3, 8, -8), box.GetMin()));
	EXPECT_TRUE(ctNear(maVector3d(23, 9, 3), box.GetMax()));

	maAxisBox other(maPoint3d(4, 5, 9), maPoint3d(3, -33, 9));
	box.Union(other);
	EXPECT_TRUE(ctNear(maVector3d(-3, -33, -8), box.GetMin()));
	EXPECT_TRUE(ctNear(maVector3d(23, 9, 9), box.GetMax()));

	// a union with an empty box changes nothing
	maAxisBox before = box;
	box.Union(maAxisBox());
	EXPECT_TRUE(before == box);
}

TEST(maAxisBox, ContainsPoint)
{
	maAxisBox box(maPoint3d(-3, -33, -8), maPoint3d(23, 9, 9));

	EXPECT_TRUE(box.ContainsPoint(maPoint3d(3, 4, 1)));
	EXPECT_FALSE(box.ContainsPoint(maPoint3d(9, -40, 13)));
	EXPECT_TRUE(box.ContainsPoint(box.GetMin()));
	EXPECT_TRUE(box.ContainsPoint(box.GetMax()));
}

TEST(maAxisBox, Overlaps)
{
	maAxisBox a(maPoint3d(0, 0, 0), maPoint3d(10, 10, 10));
	EXPECT_TRUE(a.Overlaps(maAxisBox(maPoint3d(5, 5, 5), maPoint3d(15, 15, 15))));
	EXPECT_FALSE(a.Overlaps(maAxisBox(maPoint3d(11, 0, 0), maPoint3d(15, 10, 10))));
}
