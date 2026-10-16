/*****************************************************************************
**  maMatrixTest.cpp
**
**      maMatrix3x3 and maMatrix4x4: element access, transpose, products,
**      rotations, translations and inverses.
**
**      The matrices use the row-vector convention: M * v computes v M, so
**      (A * B) * v applies A first, and translations sit in row 3.
**
**      Ported from TestTerawatt/Math/ma/maTestMain.cpp, which printed the
**      results; the expected values are worked out here.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctMath.hpp"

#ifndef MA_CONSTANTS_HPP
#include "Core/ma/maConstants.hpp"
#endif

#include <gtest/gtest.h>

namespace
{
	const float c_90Degrees = 90.0f * maConstants::c_fAngleToRad;

	maMatrix3x3 OneToNine()
	{
		return maMatrix3x3(1, 2, 3,
						   4, 5, 6,
						   7, 8, 9);
	}
}

TEST(maMatrix3x3, ClearIdentityAndElements)
{
	maMatrix3x3 m;
	m.Clear();
	EXPECT_TRUE(ctNear(maMatrix3x3(0, 0, 0, 0, 0, 0, 0, 0, 0), m));

	m(1, 0) = 10;
	m(2, 1) = 5;
	EXPECT_TRUE(ctNear(maMatrix3x3(0, 0, 0, 10, 0, 0, 0, 5, 0), m));

	m.Identity();
	EXPECT_TRUE(ctNear(maMatrix3x3(1, 0, 0, 0, 1, 0, 0, 0, 1), m));
}

TEST(maMatrix3x3, TransposeAndProduct)
{
	maMatrix3x3 a = OneToNine();
	a.Transpose();
	EXPECT_TRUE(ctNear(maMatrix3x3(1, 4, 7,
								   2, 5, 8,
								   3, 6, 9), a));

	maMatrix3x3 b(2, 2, 2,
				  3, 3, 3,
				  4, 4, 4);
	EXPECT_TRUE(ctNear(maMatrix3x3(42, 42, 42,
								   51, 51, 51,
								   60, 60, 60), a * b));
}

TEST(maMatrix3x3, Determinant)
{
	EXPECT_FLOAT_EQ(0.0f, OneToNine().GetDeterminant());
	EXPECT_FLOAT_EQ(1.0f, ctIdentity3x3().GetDeterminant());

	maMatrix3x3 scale;
	scale.MakeScale(2, 3, 4);
	EXPECT_FLOAT_EQ(24.0f, scale.GetDeterminant());
}

TEST(maMatrix3x3, InverseTimesMatrixIsIdentity)
{
	maMatrix3x3 a;
	a.MakeRotateX(3.2f);
	a.ScaleBy(2.3f, 4.5f, -1.4f);
	a.RotateBy(3.4f, maVector3d(0, 1, 0));

	maMatrix3x3 inverse = a;
	inverse.Invert();

	EXPECT_TRUE(ctNear(ctIdentity3x3(), a * inverse));
	EXPECT_TRUE(ctNear(ctIdentity3x3(), inverse * a));
}

TEST(maMatrix3x3, RotationsAgreeWith4x4)
{
	const maVector3d v(1, 2, 3);
	const float angle = 0.7f;

	maMatrix3x3 r3;
	maMatrix4x4 r4;

	r3.MakeRotateX(angle);
	r4.MakeRotateX(angle);
	EXPECT_TRUE(ctNear(r4 * v, r3 * v));

	r3.MakeRotateY(angle);
	r4.MakeRotateY(angle);
	EXPECT_TRUE(ctNear(r4 * v, r3 * v));

	r3.MakeRotateZ(angle);
	r4.MakeRotateZ(angle);
	EXPECT_TRUE(ctNear(r4 * v, r3 * v));
}

TEST(maMatrix4x4, ClearIdentityAndElements)
{
	maMatrix4x4 m;
	m.Clear();
	EXPECT_FALSE(m.IsIdentity());
	m(1, 0) = 10;
	m(2, 1) = 5;
	EXPECT_EQ(10.0f, m[4]);		// row major
	EXPECT_EQ(5.0f, m[9]);

	m.Identity();
	EXPECT_TRUE(m.IsIdentity());
}

TEST(maMatrix4x4, TransposeAndProduct)
{
	maMatrix4x4 a(1, 2, 3, 4,
				  4, 5, 6, 7,
				  7, 8, 9, 0,
				  7, 8, 9, 9);
	a.Transpose();
	EXPECT_TRUE(ctNear(maMatrix4x4(1, 4, 7, 7,
								   2, 5, 8, 8,
								   3, 6, 9, 9,
								   4, 7, 0, 9), a));

	maMatrix4x4 b(2, 2, 2, 2,
				  3, 3, 3, 3,
				  4, 4, 4, 4,
				  4, 4, 4, 4);
	EXPECT_TRUE(ctNear(maMatrix4x4(70, 70, 70, 70,
								   83, 83, 83, 83,
								   96, 96, 96, 96,
								   65, 65, 65, 65), a * b));
}

TEST(maMatrix4x4, TranslationsAdd)
{
	maMatrix4x4 t1, t2;
	t1.MakeTranslate(2, 3, 4);
	t2.MakeTranslate(6, 7, 8);

	maMatrix4x4 product = t1 * t2;
	EXPECT_TRUE(ctNear(maVector3d(8, 10, 12), product.GetTranslation()));
	EXPECT_TRUE(ctNear(maVector3d(9, 11, 13), product * maVector3d(1, 1, 1)));
}

//	Each rotation is counter-clockwise looking down its positive axis
//	in a right-handed system.
//
TEST(maMatrix4x4, RotateNinetyDegrees)
{
	maMatrix4x4 rx, ry, rz;
	rx.MakeRotateX(c_90Degrees);
	ry.MakeRotateY(c_90Degrees);
	rz.MakeRotateZ(c_90Degrees);

	EXPECT_TRUE(ctNear(maVector3d(0, 0, 1), rx * maVector3d(0, 1, 0)));
	EXPECT_TRUE(ctNear(maVector3d(1, 0, 0), ry * maVector3d(0, 0, 1)));
	EXPECT_TRUE(ctNear(maVector3d(0, 1, 0), rz * maVector3d(1, 0, 0)));

	EXPECT_TRUE(ctNear(maVector3d(1, -1, 1), rx * maVector3d(1, 1, 1)));
	EXPECT_TRUE(ctNear(maVector3d(1, 1, -1), ry * maVector3d(1, 1, 1)));
	EXPECT_TRUE(ctNear(maVector3d(-1, 1, 1), rz * maVector3d(1, 1, 1)));
}

TEST(maMatrix4x4, ConcatenatedRotationsApplyLeftToRight)
{
	maMatrix4x4 rx, ry, rz;
	rx.MakeRotateX(c_90Degrees);
	ry.MakeRotateY(c_90Degrees);
	rz.MakeRotateZ(c_90Degrees);

	const maVector3d v(1, 1, 1);
	maVector3d one_by_one = rz * (ry * (rx * v));
	EXPECT_TRUE(ctNear(one_by_one, (rx * ry * rz) * v));
}

TEST(maMatrix4x4, InverseTimesMatrixIsIdentity)
{
	maMatrix4x4 a;
	a.Identity();
	a.MakeRotateX(3.2f);
	a.ScaleBy(2.3f, 4.5f, -1.4f);
	a.TranslateBy(1, 2, 3);
	a.RotateBy(3.4f, maVector3d(0, 1, 0));
	a.ScaleBy(1.0f, 3.3f, -0.2f);

	maMatrix4x4 inverse = a;
	inverse.Invert();

	EXPECT_TRUE(ctNear(ctIdentity4x4(), a * inverse));
	EXPECT_TRUE(ctNear(ctIdentity4x4(), inverse * a));

	const maVector3d v(4, -5, 6);
	EXPECT_TRUE(ctNear(v, inverse * (a * v), 1e-3f));
}
