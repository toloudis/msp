/*****************************************************************************
**  maRotationTest.cpp
**
**      maRotation (quaternion): from/to rotations, inverses, matrix and
**      Euler conversions, axis/angle, slerp.
**
**      Ported from TestTerawatt/Math/ma/maTestMain.cpp (TestmaRotation
**      and TestEuler), which printed the results.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctMath.hpp"

#ifndef MA_CONSTANTS_HPP
#include "Core/ma/maConstants.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

#include <gtest/gtest.h>

namespace
{
	const float c_DegToRad = maConstants::c_fAngleToRad;

	maVector3d Rotated(const maRotation& i_Rot, maVector3d i_Vec)
	{
		i_Rot.RotateVector(i_Vec);
		return i_Vec;
	}

	//	Two rotations are the same if they move test vectors the same way
	//	(q and -q are the same rotation).
	testing::AssertionResult SameRotation(const maRotation& i_Expected, const maRotation& i_Actual)
	{
		const maVector3d probes[] = { maVector3d(1, 0, 0), maVector3d(0, 1, 0), maVector3d(0.3f, -0.5f, 0.8f) };
		for( const maVector3d& probe : probes )
		{
			testing::AssertionResult result = ctNear(Rotated(i_Expected, probe), Rotated(i_Actual, probe));
			if( !result )
				return result;
		}
		return testing::AssertionSuccess();
	}
}

TEST(maRotation, FromToRotatesFromOntoTo)
{
	maVector3d from(-3, 4, 9);
	from.Normalize();
	maVector3d to(3, -1, 33);
	to.Normalize();

	maRotation rot;
	rot.SetValue(from, to);
	EXPECT_TRUE(ctNear(to, Rotated(rot, from)));

	rot.Invert();
	EXPECT_TRUE(ctNear(from, Rotated(rot, to)));
	EXPECT_TRUE(SameRotation(rot, rot.GetInverse().GetInverse()));
}

TEST(maRotation, MatrixRoundTrip)
{
	maVector3d from(-3, 4, 9);
	from.Normalize();
	maVector3d to(3, -1, 33);
	to.Normalize();

	maRotation rot;
	rot.SetValue(from, to);

	maMatrix3x3 matrix = rot.GetMatrix3x3();

	maRotation from_matrix;
	from_matrix.Identity();	// make sure nothing carries over
	from_matrix.SetValue(matrix);
	EXPECT_TRUE(SameRotation(rot, from_matrix));

	// and the 4x4 matrix moves vectors the same way
	const maVector3d v(0.3f, -0.5f, 0.8f);
	EXPECT_TRUE(ctNear(Rotated(rot, v), rot.GetMatrix() * v));
}

TEST(maRotation, AxisAngle)
{
	maRotation rot(maVector3d(0, 0, 1), 90 * c_DegToRad);

	// counter-clockwise about +Z, like maMatrix4x4::MakeRotateZ
	EXPECT_TRUE(ctNear(maVector3d(0, 1, 0), Rotated(rot, maVector3d(1, 0, 0))));

	maVector3d axis;
	float angle = 0;
	rot.GetValue(axis, angle);
	EXPECT_TRUE(ctNear(maVector3d(0, 0, 1), axis));
	EXPECT_NEAR(90 * c_DegToRad, angle, c_ctTolerance);
}

TEST(maRotation, EulerRoundTrip)
{
	const float x = 32 * c_DegToRad, y = 46 * c_DegToRad, z = 68 * c_DegToRad;
	maRotation euler_rot(x, y, z);

	float got_x = 0, got_y = 0, got_z = 0;
	euler_rot.GetEuler(got_x, got_y, got_z);
	EXPECT_NEAR(x, got_x, 1e-3f);
	EXPECT_NEAR(y, got_y, 1e-3f);
	EXPECT_NEAR(z, got_z, 1e-3f);

	maRotation from_euler;
	from_euler.SetEuler(got_x, got_y, got_z);
	EXPECT_TRUE(SameRotation(euler_rot, from_euler));
}

TEST(maRotation, SlerpEndsAndMiddle)
{
	maRotation start(maVector3d(0, 0, 1), 0);
	maRotation end(maVector3d(0, 0, 1), 90 * c_DegToRad);

	maRotation r;
	r.Slerp(start, end, 0);
	EXPECT_TRUE(SameRotation(start, r));
	r.Slerp(start, end, 1);
	EXPECT_TRUE(SameRotation(end, r));

	r.Slerp(start, end, 0.5f);
	EXPECT_TRUE(SameRotation(maRotation(maVector3d(0, 0, 1), 45 * c_DegToRad), r));
}

//	RotateVector computes q v q^-1, so a * b rotates by b first, then a:
//	the opposite order from maMatrix4x4, where (A * B) * v applies A first.
//
TEST(maRotation, ProductAppliesRightOperandFirst)
{
	maRotation a(maVector3d(1, 0, 0), 30 * c_DegToRad);
	maRotation b(maVector3d(0, 1, 0), 50 * c_DegToRad);

	const maVector3d v(0.3f, -0.5f, 0.8f);
	EXPECT_TRUE(ctNear(Rotated(a, Rotated(b, v)), Rotated(a * b, v)));

	maRotation product = a;
	product *= b;
	EXPECT_TRUE(SameRotation(a * b, product));
}
