#include <stdio.h>
#include <windows.h>
#include <ddraw.h>
#include <math.h>
#include <time.h>

#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

#include "dbgAssert.hpp"
#include "dbgLog.hpp"
#include "dbgPackage.hpp"
#include "envError.hpp"
#include "envPackage.hpp"
#include "maAxisBox.hpp"
#include "maConstants.hpp"
#include "maAngle.hpp"
#include "maEigen.hpp"
#include "maFunctions.hpp"
#include "maMatrix4x4.hpp"
#include "maPackage.hpp"
#include "maPlane.hpp"
#include "maRand.hpp"
#include "maRect.hpp"
#include "maRotation.hpp"
#include "maRunningAverage.hpp"
#include "maTypes.hpp"


//========================================================================
//========================================================================
namespace
{
void Display( maMatrix3x3& i_Mat )
{
	DBG_LOG3( "%6.3f %6.3f %6.3f", i_Mat(0,0),  i_Mat(0,1),  i_Mat(0,2) );
	DBG_LOG3( "%6.3f %6.3f %6.3f", i_Mat(1,0),  i_Mat(1,1),  i_Mat(1,2) );
	DBG_LOG3( "%6.3f %6.3f %6.3f", i_Mat(2,0),  i_Mat(2,1),  i_Mat(2,2) );
}
void Display( maMatrix4x4& i_Mat )
{
	DBG_LOG4( "%6.3f %6.3f %6.3f %6.3f", i_Mat(0,0),  i_Mat(0,1),  i_Mat(0,2),  i_Mat(0,3) );
	DBG_LOG4( "%6.3f %6.3f %6.3f %6.3f", i_Mat(1,0),  i_Mat(1,1),  i_Mat(1,2),  i_Mat(1,3) );
	DBG_LOG4( "%6.3f %6.3f %6.3f %6.3f", i_Mat(2,0),  i_Mat(2,1),  i_Mat(2,2),  i_Mat(2,3) );
	DBG_LOG4( "%6.3f %6.3f %6.3f %6.3f", i_Mat(3,0),  i_Mat(3,1),  i_Mat(3,2),  i_Mat(3,3) );
}
void Display(const maVector3d& i_Vec)
{
	DBG_LOG3( "(%6.3f %6.3f %6.3f)", i_Vec.GetX(), i_Vec.GetY(), i_Vec.GetZ());
}

void Display(const maVector4d& i_Vec)
{
	DBG_LOG4( "(%6.3f %6.3f %6.3f %6.3f)", i_Vec.GetX(), i_Vec.GetY(), i_Vec.GetZ(), i_Vec.GetW());
}

void Display(const maPlane& i_Plane)
{
	DBG_LOG4( "(%6.3f %6.3f %6.3f %6.3f)", i_Plane.GetNormal().GetX(), i_Plane.GetNormal().GetY(), i_Plane.GetNormal().GetZ(), i_Plane.GetValue());
}

void Display(const maAxisBox& i_Box)
{
	DBG_LOG6( "(%6.3f - %6.3f), (%6.3f - %6.3f), (%6.3f - %6.3f)",	i_Box.GetMinX(), i_Box.GetMaxX(),
																	i_Box.GetMinY(), i_Box.GetMaxY(),
																	i_Box.GetMinZ(), i_Box.GetMaxZ());	
}

void Display(const maRotation& i_Rot)
{
	maVector3d axis;
	float angle;
	i_Rot.GetValue(axis, angle);
	DBG_LOG4("(<%6.3f %6.3f %6.3f>, %6.3f)", axis.GetX(), axis.GetY(), axis.GetZ(), angle);
}

void TestmaAxisBox()
{
	DBG_LOG0( "======================================================" );
	DBG_LOG0( "maAxisBox" );
	DBG_LOG0( "" );

	maAxisBox box1;
	DBG_LOG0( "Box made with default constructor:" );
	Display(box1);

	DBG_LOG0( "Box set to contain two points:" );
	maPoint3d p1(4, 9, -3);
	maPoint3d p2(-3, 8, -8);
	Display(p1);
	Display(p2);
	box1.Set(p1, p2);
	Display(box1);

	DBG_LOG0( "Existing box unioned with a point:" );
	maPoint3d p3( 23, 9, 3 );
	Display(p3);
	box1.Union(p3);
	Display(box1);
	
	maAxisBox box2( maPoint3d(4, 5, 9), maPoint3d(3, -33, 9));
	DBG_LOG0( "Existing box unioned with another box:" );
	Display(box2);
	box1.Union(box2);
	Display(box1);

	DBG_LOG0( "Box:" );
	Display(box1);
	DBG_LOG0( "contains point:" );
	maPoint3d p4(3, 4, 1);
	Display(p4);
	if( box1.ContainsPoint(p4) )
		DBG_LOG0("True");
	else
		DBG_LOG0("False");	

	DBG_LOG0( "Box:" );
	Display(box1);
	DBG_LOG0( "contains point:" );
	maPoint3d p5(9, -40, 13);
	Display(p5);
	if( box1.ContainsPoint(p5) )
		DBG_LOG0("True");
	else
		DBG_LOG0("False");	
}

void TestmaPlane()
{
	DBG_LOG0( "======================================================" );
	DBG_LOG0( "maPlane" );
	DBG_LOG0( "" );

	maPlane plane1;
	DBG_LOG0( "Plane made with default constructor:" );
	Display(plane1);

	maPoint3d p2normal(4, 9, 2);
	p2normal.Normalize();
	maPlane plane2(p2normal.m_X, p2normal.m_Y, p2normal.m_Z, 10.0f);
	DBG_LOG0( "Plane made with equation constructor:" );
	Display(plane2);

	maPoint3d p31(3, 9, 2);
	maPoint3d p32(4, 4, 9);
	maPoint3d p33(-4, 9, 2);
	maPlane plane3(p31, p32, p33);
	DBG_LOG0( "Plane made with 3 point constructor:" );
	Display(plane3);

	DBG_LOG0( "Plane containing 3 axis points:" );
	maPoint3d p41(1, 0, 0);
	maPoint3d p42(0, 1, 0);
	maPoint3d p43(0, 0, 1);
	Display(p41);
	Display(p42);
	Display(p43);
	plane3.Set(p41, p42, p43);
	Display(plane3);

	DBG_LOG0( "Distance from point:" );
	maPoint3d p51(3, 4, 0);
	Display(p51);
	DBG_LOG0( "to plane:" );
	Display(plane3);
	DBG_LOG1("%.4f", plane3.TestPoint(p51));

	DBG_LOG0( "Distance from point:" );
	maPoint3d p61(0, 0, 0);
	Display(p61);
	DBG_LOG0( "to plane:" );
	Display(plane3);
	DBG_LOG1("%.4f", plane3.TestPoint(p61));

	DBG_LOG0( "Distance from point:" );
	maPoint3d p71(1, 1, 1);
	Display(p71);
	DBG_LOG0( "to plane:" );
	Display(plane3);
	DBG_LOG1("%.4f", plane3.TestPoint(p71));
}

void TestmaRotation()
{
	//	Rotation
	//
	DBG_LOG0( "maRotation" );
	DBG_LOG0( "--------------------------------" );
	DBG_LOG0( "(rotations displayed in axis, angle format)" );
	
	DBG_LOG0( "Rotate from point" );
	maVector3d p1(-3, 4, 9);
	p1.Normalize();
	Display(p1);
	DBG_LOG0( "to point" );
	maVector3d p2(3, -1, 33);
	p2.Normalize();
	Display(p2);
	maRotation rot1;
	rot1.SetValue(p1, p2);
	Display(rot1);
	DBG_LOG0( "transforming first point should equal second point:" );
	rot1.RotateVector(p1);
	Display(p1);
	DBG_LOG0( "Inverse of rotation:" );
	rot1.Invert();
	Display(rot1);
	DBG_LOG0( "transforming the second point with the inverse rotation should produce the first point" );
	rot1.RotateVector(p1);
	Display(p1);

	DBG_LOG0( "Make a matrix from the rotation:" );
	maMatrix3x3 matrix = rot1.GetMatrix3x3();
	Display(matrix);
	DBG_LOG0( "Deducing the rotation from the matrix should produce the same rotation" );
	DBG_LOG0("before:");
	Display(rot1);
	rot1.Identity(); // make sure we aren't just saving the old values
	rot1.SetValue(matrix);
	DBG_LOG0("after:");
	Display(rot1);
}

void TestEuler()
{
	//	Rotation
	//
	DBG_LOG0( "euler angle conversion" );
	DBG_LOG0( "--------------------------------" );
	DBG_LOG0( "(rotations displayed in axis, angle format)" );
	

	DBG_LOG0( "Create euler from degree angles: X=32, Y=46, Z=68" );
	maRotation euler_rot( 32*maConstants::c_fAngleToRad, 
						  46*maConstants::c_fAngleToRad, 
						  68*maConstants::c_fAngleToRad);
	Display(euler_rot);

	float xAngle = 0, yAngle = 0, zAngle = 0;
	euler_rot.GetEuler(xAngle, yAngle, zAngle); 
	DBG_LOG3( "GetEuler(): X %f Y %f Z %f", 
		xAngle * maConstants::c_fRadToAngle, 
		yAngle * maConstants::c_fRadToAngle, 
		zAngle * maConstants::c_fRadToAngle );

	maRotation test_rot;
	test_rot.SetEuler(xAngle, yAngle, zAngle);
	DBG_LOG0("re-set euler from values from GetEuler:");
	Display(test_rot);

}

void TestmaFunctions()
{
	DBG_LOG0( "maFunctions" );
	DBG_LOG0( "--------------------------------" );

	float acc = 0.0f;
	const int num_samples = 20;
	int i;
	DBG_LOG0("Some random numbers in the rage [0, 1]");
	for( i = 0 ; i < num_samples ; i++ )
	{
		float val = maFunctions::FloatRand(0, 1);
		acc += val;
		DBG_LOG1(": %.4f", val);
	}

	DBG_LOG1("Their mean value: %.4f\n", acc / float(num_samples));
}

void TestmaRunningAverage()
{
	DBG_LOG0( "maRunningAverage" );
	DBG_LOG0( "--------------------------------" );

	maRunningAverage average(3);

	int i;
	for( i = 0 ; i < 100 ; ++i )
	{
		average.Push( (float) (i % 4) );
		DBG_LOG1("Average: %.2f", average.GetAverage());
	}
	DBG_LOG0( "\n" );
}

void TestRandomStandard()
{
	DBG_LOG0( "Random - standard" );
	DBG_LOG0( "--------------------------------" );

	srand(0);
	float acc = 0.0f;
	const int num_samples = 20;
	int i;
	DBG_LOG0("Some random numbers in the rage [0, 1]");
	for( i = 0 ; i < num_samples ; i++ )
	{
		float val = rand() / (float)RAND_MAX;
		acc += val;
		DBG_LOG1(": %.4f", val);
	}

	DBG_LOG1("Their mean value: %.4f\n", acc / float(num_samples));

}


//========================================================================
//========================================================================
void DoTests()
{
	DBG_LOG0( "======================================================" );
	DBG_LOG0( "MATH START" );
	DBG_LOG0( "" );

	//	maConstants
	//
	DBG_LOG0( "CONSTANTS" );
	DBG_LOG0( "---------" );
	DBG_LOG1( "Constant: PI         (float) = %7.4f", maConstants::c_fPI );
	DBG_LOG1( "Constant: RadToAngle (float) = %7.4f", maConstants::c_fRadToAngle );
	DBG_LOG1( "Constant: AngleToRad (float) = %7.4f", maConstants::c_fAngleToRad );
	DBG_LOG0( "" );

	//	maAngle
	//
	DBG_LOG0( "ANGLE" );
	DBG_LOG0( "-----" );
	maAngle	TheAngle;
	TheAngle.SetDegrees( 90.0f );
	DBG_LOG1( "Angle degrees %7.4f", TheAngle.GetDegrees() );
	DBG_LOG1( "Angle radians %7.4f", TheAngle.GetRadians() );
	TheAngle.Constrain( 0.0f, 45.0f );
	DBG_LOG1( "Angle constrained 0 to 45 -> %7.4f", TheAngle.GetDegrees() );
	TheAngle.SetRadians( 1.5708f );
	DBG_LOG1( "Angle set radians 1.5708 -> degrees %7.4f", TheAngle.GetDegrees() );
	TheAngle	+= 45.0f;
	DBG_LOG1( "Angle += 45 -> %7.4f", TheAngle.GetDegrees() );
	TheAngle	-= 45.0f;
	DBG_LOG1( "Angle -= 45 -> %7.4f", TheAngle.GetDegrees() );
	TheAngle	= 180.0f;
	DBG_LOG1( "Angle assigned 180 -> %7.4f", TheAngle.GetDegrees() );
	DBG_LOG1( "Angle sine -> %7.4f", TheAngle.Sine() );
	DBG_LOG1( "Angle cosine -> %7.4f", TheAngle.Cosine() );
	DBG_LOG0( "" );

	//	maFunctions
	//
	DBG_LOG0( "FUNCTIONS" );
	DBG_LOG0( "---------" );
	DBG_LOG1( "Highest 1 and 10 -> %7.4f", maFunctions::Highest( 1.0f, 10.0f ) );
	DBG_LOG1( "Lowest  1 and 10 -> %7.4f", maFunctions::Lowest( 1.0f, 10.0f ) );;

	float fVal1, fVal2;
	fVal1 = 1.0f;
	fVal2 = 10.0f;
	DBG_LOG2( "PreSwap    -> %7.4f %7.4f", fVal1, fVal2 );
	maFunctions::Swap( fVal1, fVal2 );
	DBG_LOG2( "After Swap -> %7.4f %7.4f", fVal1, fVal2 );
	DBG_LOG0( "" );

	// maRand
	//
	int iRand;
	envType::UInt32	randNum=0, mytime=0;

	DBG_LOG0( "RAND seed test" );
	mytime = time(NULL);
	for (iRand=0; iRand<100; iRand++ )
	{
		maRand32 SeedTest( mytime++ );
		randNum = SeedTest.Rand();
		DBG_LOG2("Seed = 0x%08x      Rand = 0x%08x", mytime, randNum );
	}

	DBG_WARNING0( "RAND number generation and performance" );
	DBG_LOG0( "---------" );
	// we create a giant array to print the random numbers at the end so performance is
	// based solely on calculations and not logging to the file.
	//
	// randCount is divisble by 256, the number of entries created in each maRand results
	// set generated.
	const envType::UInt32 randCount = 0x989700;  // 10 million 128 entries
	DBG_LOG1(" Performance based on %u generated numbers.", randCount);
	mytime = time(NULL);
	DBG_LOG1(" Seeding with 0x%08x", mytime);

	envType::UInt32	randResults[1000];
	memset(randResults, 0, sizeof(envType::UInt32)*1000 );

	FILETIME systimestart, systimeend;
	memset(&systimestart, 0, sizeof(FILETIME));
	memset(&systimeend, 0, sizeof(FILETIME));
	::GetSystemTimeAsFileTime(&systimestart);
	maRand32* randPerfTest = new maRand32( mytime );
	for (iRand=0; iRand<randCount; iRand++)
	{
		if (iRand < 1000 )
			randResults[iRand] = randPerfTest->Rand();
		else
			randNum = randPerfTest->Rand();
	}
	::GetSystemTimeAsFileTime(&systimeend);
	
	DBG_LOG2(" TimeStart: High(0x%08x) Low(0x%08x) (100-nanosecond intervals)", systimestart.dwHighDateTime, systimestart.dwLowDateTime );
	DBG_LOG2("   TimeEnd: High(0x%08x) Low(0x%08x) (100-nanosecond intervals)", systimeend.dwHighDateTime, systimeend.dwLowDateTime );

	float fTimePerNumber = (float)(systimeend.dwLowDateTime - systimestart.dwLowDateTime) / (float)randCount;
	fTimePerNumber /= 10; // convert from FILETIME (100 nanosecond intervals) to microseconds per number
	DBG_WARNING1(" Performance:  Average time to generate one random number:  %f microseconds",
				fTimePerNumber);
	float test = 1/fTimePerNumber;  // numbers per microsecond
	test *= 1000000;  // numbers per second
	DBG_WARNING2(" Performance:  Random numbers per microsecond (%f) or %f numbers per second",
					(1/fTimePerNumber), ((1/fTimePerNumber)*1000000)  );

	// Compare to Microsoft RAND
	memset(&systimestart, 0, sizeof(FILETIME));
	memset(&systimeend, 0, sizeof(FILETIME));
	::GetSystemTimeAsFileTime(&systimestart);
	for (iRand=0; iRand<randCount; iRand++)
	{
		randNum = rand();
	}
	::GetSystemTimeAsFileTime(&systimeend);
	
	DBG_LOG2(" TimeStart: High(0x%08x) Low(0x%08x) (100-nanosecond intervals)", systimestart.dwHighDateTime, systimestart.dwLowDateTime );
	DBG_LOG2("   TimeEnd: High(0x%08x) Low(0x%08x) (100-nanosecond intervals)", systimeend.dwHighDateTime, systimeend.dwLowDateTime );

	fTimePerNumber = (float)(systimeend.dwLowDateTime - systimestart.dwLowDateTime) / (float)randCount;
	fTimePerNumber /= 10; // convert from FILETIME (100 nanosecond intervals) to microseconds per number
	DBG_LOG1(" Performance:  Average time to generate one random number:  %f microseconds",
				fTimePerNumber);
	test = 1/fTimePerNumber;  // numbers per microsecond
	test *= 1000000;  // numbers per second
	DBG_WARNING1(" As compared to Microsoft's Rand:  %f numbers per second.", ((1/fTimePerNumber)*1000000) );

	// first 1000 entries logged
	DBG_LOG0(" First 1000 entries: in sets of ten");
	for (iRand=0; iRand<1000; iRand+=10)
	{
		DBG_LOG10("Rand:  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x",
			randResults[iRand], randResults[iRand+1], randResults[iRand+2], randResults[iRand+3],
			randResults[iRand+4], randResults[iRand+5], randResults[iRand+6], randResults[iRand+7],
			randResults[iRand+8], randResults[iRand+9] );
	}
	delete randPerfTest;

	DBG_LOG0( "RAND 0 to 1 Test" );
	// generate values between zero and one, assert if anything exceeds or falls below.
	float fTest;
	memset(&systimestart, 0, sizeof(FILETIME));
	memset(&systimeend, 0, sizeof(FILETIME));
	::GetSystemTimeAsFileTime(&systimestart);
	for (iRand=0; iRand<10000000; ++iRand )
	{
		fTest = maFunctions::FloatRand(0.0f, 1.0f );
		DBG_ASSERT1(fTest >= 0.0f && fTest <= 1.0f, "Unexpected rand float out of range (%f).", fTest );
	}
	::GetSystemTimeAsFileTime(&systimeend);

	DBG_LOG0( "RAND 1 to 100 Test" );
	memset(&systimestart, 0, sizeof(FILETIME));
	memset(&systimeend, 0, sizeof(FILETIME));
	::GetSystemTimeAsFileTime(&systimestart);
	for (iRand=0; iRand<10000000; ++iRand )
	{
		fTest = maFunctions::FloatRand(1.0f, 100.0f );
		DBG_ASSERT1(fTest >= 1.0f && fTest <= 100.0f, "Unexpected rand float out of range(1-100) (%f).", fTest );
	}
	::GetSystemTimeAsFileTime(&systimeend);

	DBG_LOG0( "RAND -100 to 100 Test" );
	memset(&systimestart, 0, sizeof(FILETIME));
	memset(&systimeend, 0, sizeof(FILETIME));
	::GetSystemTimeAsFileTime(&systimestart);
	for (iRand=0; iRand<10000000; ++iRand )
	{
		fTest = maFunctions::FloatRand(-100.0f, 100.0f );
		DBG_ASSERT1(fTest >= -100.0f && fTest <= 100.0f, "Unexpected rand float out of range(-100 - +100) (%f).", fTest );
	}
	::GetSystemTimeAsFileTime(&systimeend);

	DBG_LOG0(" ");  // END RAND

	//	Points
	//
	DBG_LOG0( "POINT 2-D" );
	DBG_LOG0( "---------" );
	maPoint2d Pt2A;
	Pt2A.Set( 0.0f, 0.0f );
	DBG_LOG2( "PointA( %7.3f,%7.3f ) set(x,y)", Pt2A.GetX(), Pt2A.GetY() );
	Pt2A.SetX( 1.0f );
	Pt2A.SetY( 1.0f );
	DBG_LOG2( "PointA( %7.3f,%7.3f ) setx,sety", Pt2A.GetX(), Pt2A.GetY() );
	DBG_LOG0( "" );
	maPoint2d Pt2B;
	Pt2B.Set( 3.0f, 3.0f );
	DBG_LOG2( "PointB( %7.3f,%7.3f ) set(x,y)", Pt2B.GetX(), Pt2B.GetY() );
	Pt2A = maPoint2d( 5.0f, 5.0f );
	DBG_LOG2( "PointA( %7.3f,%7.3f ) copy constructor", Pt2A.GetX(), Pt2A.GetY() );
	Pt2A = Pt2A + Pt2B;
	DBG_LOG2( "PointA( %7.3f,%7.3f ) addition and assignment", Pt2A.GetX(), Pt2A.GetY() );
	Pt2A *= 3;
	DBG_LOG2( "PointA( %7.3f,%7.3f ) scaled three-times", Pt2A.GetX(), Pt2A.GetY() );
	DBG_LOG3( "PointA( %7.3f,%7.3f ) Length = %7.3f", Pt2A.GetX(), Pt2A.GetY(), Pt2A.Length() );
	DBG_LOG3( "PointA( %7.3f,%7.3f ) index 0 = %7.3f", Pt2A.GetX(), Pt2A.GetY(), Pt2A[0] );
	Pt2A.Normalize();
	DBG_LOG2( "PointA( %7.3f,%7.3f ) normalized", Pt2A.GetX(), Pt2A.GetY() );
	DBG_LOG0( "" );
	
	DBG_LOG0( "POINT 3-D" );
	DBG_LOG0( "---------" );
	maPoint3d Pt3A;
	Pt3A.Set( 0.0f, 0.0f, 0.0f );
	DBG_LOG3( "PointA( %7.3f,%7.3f,%7.3f )", Pt3A.GetX(), Pt3A.GetY(), Pt3A.GetZ() );
	Pt3A.SetX( 1.0f );
	Pt3A.SetY( 1.0f );
	DBG_LOG3( "PointA( %7.3f,%7.3f,%7.3f )", Pt3A.GetX(), Pt3A.GetY(), Pt3A.GetZ() );
	DBG_LOG0( "" );
	maPoint3d Pt3B;
	Pt3B.Set( 3.0f, 3.0f, 3.0f );
	DBG_LOG3( "PointB( %7.3f,%7.3f,%7.3f )", Pt3B.GetX(), Pt3B.GetY(), Pt3B.GetZ() );
	Pt3A = maPoint3d( 5.0f, 5.0f, 5.0f );
	DBG_LOG3( "PointA( %7.3f,%7.3f,%7.3f ) operator = from maPoint3D", Pt3A.GetX(), Pt3A.GetY(), Pt3A.GetZ() );
	Pt3A = Pt3A + Pt3B;
	DBG_LOG3( "PointA( %7.3f,%7.3f,%7.3f ) addition and assignment A+B", Pt3A.GetX(), Pt3A.GetY(), Pt3A.GetZ() );
	Pt3A *= 2;
	DBG_LOG3( "PointA( %7.3f,%7.3f,%7.3f ) scaled two-times", Pt3A.GetX(), Pt3A.GetY(), Pt3A.GetZ() );
	Pt3A /= 2;
	DBG_LOG3( "PointA( %7.3f,%7.3f,%7.3f ) scaled down two-times", Pt3A.GetX(), Pt3A.GetY(), Pt3A.GetZ() );
	DBG_LOG4( "PointA( %7.3f,%7.3f,%7.3f ) Length = %7.3f", Pt3A.GetX(), Pt3A.GetY(), Pt3A.GetZ(), Pt3A.Length() );
	DBG_LOG4( "PointA( %7.3f,%7.3f,%7.3f ) index 0 = %7.3f", Pt3A.GetX(), Pt3A.GetY(), Pt3A.GetZ(), Pt3A[0] );
	Pt3A.Normalize();
	DBG_LOG3( "PointA( %7.3f,%7.3f,%7.3f ) normalized", Pt3A.GetX(), Pt3A.GetY(), Pt3A.GetZ() );
	DBG_LOG0( "" );

	DBG_LOG0( "POINT 4-D" );
	DBG_LOG0( "---------" );
	maPoint4d Pt4A;
	Pt4A.Set( 0.0f, 0.0f, 0.0f, 1.0f );
	DBG_LOG4( "PointA( %7.3f,%7.3f,%7.3f,%7.3f )", Pt4A.GetX(), Pt4A.GetY(), Pt4A.GetZ(), Pt4A.GetW() );
	Pt4A.SetX( 1.0f );
	Pt4A.SetY( 1.0f );
	DBG_LOG4( "PointA( %7.3f,%7.3f,%7.3f,%7.3f )", Pt4A.GetX(), Pt4A.GetY(), Pt4A.GetZ(), Pt4A.GetW() );
	DBG_LOG0( "" );
	maPoint4d Pt4B;
	Pt4B.Set( 3.0f, 3.0f, 3.0f );
	DBG_LOG4( "PointB( %7.3f,%7.3f,%7.3f,%7.3f )", Pt4B.GetX(), Pt4B.GetY(), Pt4B.GetZ(), Pt4A.GetW() );
	Pt4A = maPoint4d( 5.0f, 5.0f, 5.0f, 1.0f );
	DBG_LOG4( "PointA( %7.3f,%7.3f,%7.3f,%7.3f ) operator = from B", Pt4A.GetX(), Pt4A.GetY(), Pt4A.GetZ(), Pt4A.GetW() );
	Pt4A = Pt4A + Pt4B;
	DBG_LOG4( "PointA( %7.3f,%7.3f,%7.3f,%7.3f ) addition and assignment", Pt4A.GetX(), Pt4A.GetY(), Pt4A.GetZ(), Pt4A.GetW() );
	Pt4A *= 2;
	DBG_LOG4( "PointA( %7.3f,%7.3f,%7.3f,%7.3f ) scaled two-times", Pt4A.GetX(), Pt4A.GetY(), Pt4A.GetZ(), Pt4A.GetW() );
	Pt4A /= 2;
	DBG_LOG4( "PointA( %7.3f,%7.3f,%7.3f,%7.3f ) scaled down two-times", Pt4A.GetX(), Pt4A.GetY(), Pt4A.GetZ(), Pt4A.GetW() );
	DBG_LOG5( "PointA( %7.3f,%7.3f,%7.3f,%7.3f ) Length = %7.3f", Pt4A.GetX(), Pt4A.GetY(), Pt4A.GetZ(), Pt4A.GetW(), Pt4A.Length() );
	DBG_LOG5( "PointA( %7.3f,%7.3f,%7.3f,%7.3f ) index 0 = %7.3f", Pt4A.GetX(), Pt4A.GetY(), Pt4A.GetZ(), Pt4A.GetW(), Pt4A[0] );
	Pt4A.Normalize();
	DBG_LOG4( "PointA( %7.3f,%7.3f,%7.3f,%7.3f ) normalized", Pt4A.GetX(), Pt4A.GetY(), Pt4A.GetZ(), Pt4A.GetW() );
	DBG_LOG0( "" );

	//	Vectors
	//
	DBG_LOG0( "VECTOR 2-D" );
	DBG_LOG0( "----------" );
	maVector2d Vec2A;
	Vec2A.Set( 0.0f, 0.0f );
	DBG_LOG2( "VectorA( %7.3f,%7.3f ) set(x,y)", Vec2A.GetX(), Vec2A.GetY() );
	Vec2A.SetX( 1.0f );
	Vec2A.SetY( 1.0f );
	DBG_LOG2( "VectorA( %7.3f,%7.3f ) setx,sety", Vec2A.GetX(), Vec2A.GetY() );
	DBG_LOG0( "" );
	maVector2d Vec2B;
	Vec2B.Set( 3.0f, 3.0f );
	DBG_LOG2( "VectorB( %7.3f,%7.3f ) set(x,y)", Vec2B.GetX(), Vec2B.GetY() );
	Vec2A = maVector2d( 5.0f, 5.0f );
	DBG_LOG2( "VectorA( %7.3f,%7.3f ) copy constructor", Vec2A.GetX(), Vec2A.GetY() );
	Vec2A = Vec2A + Vec2B;
	DBG_LOG2( "VectorA( %7.3f,%7.3f ) addition and assignment", Vec2A.GetX(), Vec2A.GetY() );
	Vec2A *= 3;
	DBG_LOG2( "VectorA( %7.3f,%7.3f ) scaled three-times", Vec2A.GetX(), Vec2A.GetY() );
	DBG_LOG3( "VectorA( %7.3f,%7.3f ) Length = %7.3f", Vec2A.GetX(), Vec2A.GetY(), Vec2A.Length() );
	DBG_LOG3( "VectorA( %7.3f,%7.3f ) index 0 = %7.3f", Vec2A.GetX(), Vec2A.GetY(), Vec2A[0] );
	Vec2A.Normalize();
	DBG_LOG2( "VectorA( %7.3f,%7.3f ) normalized", Vec2A.GetX(), Vec2A.GetY() );
	DBG_LOG0( "" );
	
	DBG_LOG0( "VECTOR 3-D" );
	DBG_LOG0( "---------" );
	maVector3d Vec3A;
	Vec3A.Set( 0.0f, 0.0f, 0.0f );
	DBG_LOG3( "VectorA( %7.3f,%7.3f,%7.3f )", Vec3A.GetX(), Vec3A.GetY(), Vec3A.GetZ() );
	Vec3A.SetX( 1.0f );
	Vec3A.SetY( 1.0f );
	DBG_LOG3( "VectorA( %7.3f,%7.3f,%7.3f )", Vec3A.GetX(), Vec3A.GetY(), Vec3A.GetZ() );
	DBG_LOG0( "" );
	maVector3d Vec3B;
	Vec3B.Set( 3.0f, 3.0f, 3.0f );
	DBG_LOG3( "VectorB( %7.3f,%7.3f,%7.3f )", Vec3B.GetX(), Vec3B.GetY(), Vec3B.GetZ() );
	Vec3A = maVector3d( 5.0f, 5.0f, 5.0f );
	DBG_LOG3( "VectorA( %7.3f,%7.3f,%7.3f ) operator = from maVector3D", Vec3A.GetX(), Vec3A.GetY(), Vec3A.GetZ() );
	Vec3A = Vec3A + Vec3B;
	DBG_LOG3( "VectorA( %7.3f,%7.3f,%7.3f ) addition and assignment A+B", Vec3A.GetX(), Vec3A.GetY(), Vec3A.GetZ() );
	Vec3A *= 2;
	DBG_LOG3( "VectorA( %7.3f,%7.3f,%7.3f ) scaled two-times", Vec3A.GetX(), Vec3A.GetY(), Vec3A.GetZ() );
	Vec3A /= 2;
	DBG_LOG3( "VectorA( %7.3f,%7.3f,%7.3f ) scaled down two-times", Vec3A.GetX(), Vec3A.GetY(), Vec3A.GetZ() );
	DBG_LOG4( "VectorA( %7.3f,%7.3f,%7.3f ) Length = %7.3f", Vec3A.GetX(), Vec3A.GetY(), Vec3A.GetZ(), Vec3A.Length() );
	DBG_LOG4( "VectorA( %7.3f,%7.3f,%7.3f ) index 0 = %7.3f", Vec3A.GetX(), Vec3A.GetY(), Vec3A.GetZ(), Vec3A[0] );
	Vec3A.Normalize();
	DBG_LOG3( "VectorA( %7.3f,%7.3f,%7.3f ) normalized", Vec3A.GetX(), Vec3A.GetY(), Vec3A.GetZ() );
	DBG_LOG0( "" );

	DBG_LOG0( "VECTOR 4-D" );
	DBG_LOG0( "---------" );
	maVector4d Vec4A;
	Vec4A.Set( 0.0f, 0.0f, 0.0f, 1.0f );
	DBG_LOG4( "VectorA( %7.3f,%7.3f,%7.3f,%7.3f )", Vec4A.GetX(), Vec4A.GetY(), Vec4A.GetZ(), Vec4A.GetW() );
	Vec4A.SetX( 1.0f );
	Vec4A.SetY( 1.0f );
	DBG_LOG4( "VectorA( %7.3f,%7.3f,%7.3f,%7.3f )", Vec4A.GetX(), Vec4A.GetY(), Vec4A.GetZ(), Vec4A.GetW() );
	DBG_LOG0( "" );
	maVector4d Vec4B;
	Vec4B.Set( 3.0f, 3.0f, 3.0f );
	DBG_LOG4( "VectorB( %7.3f,%7.3f,%7.3f,%7.3f )", Vec4B.GetX(), Vec4B.GetY(), Vec4B.GetZ(), Vec4A.GetW() );
	Vec4A = maVector4d( 5.0f, 5.0f, 5.0f, 1.0f );
	DBG_LOG4( "VectorA( %7.3f,%7.3f,%7.3f,%7.3f ) operator = from B", Vec4A.GetX(), Vec4A.GetY(), Vec4A.GetZ(), Vec4A.GetW() );
	Vec4A = Vec4A + Vec4B;
	DBG_LOG4( "VectorA( %7.3f,%7.3f,%7.3f,%7.3f ) addition and assignment", Vec4A.GetX(), Vec4A.GetY(), Vec4A.GetZ(), Vec4A.GetW() );
	Vec4A *= 2;
	DBG_LOG4( "VectorA( %7.3f,%7.3f,%7.3f,%7.3f ) scaled two-times", Vec4A.GetX(), Vec4A.GetY(), Vec4A.GetZ(), Vec4A.GetW() );
	Vec4A /= 2;
	DBG_LOG4( "VectorA( %7.3f,%7.3f,%7.3f,%7.3f ) scaled down two-times", Vec4A.GetX(), Vec4A.GetY(), Vec4A.GetZ(), Vec4A.GetW() );
	DBG_LOG5( "VectorA( %7.3f,%7.3f,%7.3f,%7.3f ) Length = %7.3f", Vec4A.GetX(), Vec4A.GetY(), Vec4A.GetZ(), Vec4A.GetW(), Vec4A.Length() );
	DBG_LOG5( "VectorA( %7.3f,%7.3f,%7.3f,%7.3f ) index 0 = %7.3f", Vec4A.GetX(), Vec4A.GetY(), Vec4A.GetZ(), Vec4A.GetW(), Vec4A[0] );
	Vec4A.Normalize();
	DBG_LOG4( "VectorA( %7.3f,%7.3f,%7.3f,%7.3f ) normalized", Vec4A.GetX(), Vec4A.GetY(), Vec4A.GetZ(), Vec4A.GetW() );
	DBG_LOG0( "" );

	//	Rect
	//
	DBG_LOG0( "RECT" );
	DBG_LOG0( "----" );
	maRect	Rect;
	Rect.SetLeft( 10.0f );
	Rect.SetUpper( 30.0f );
	Rect.SetRight( 20.0f );
	Rect.SetLower( 40.0f );
	DBG_LOG4( "RectangleA LU( %7.3f, %7.3f ) RL( %7.3f, %7.3f )", Rect.GetLeft(), Rect.GetUpper(), Rect.GetRight(), Rect.GetLower() );
	maPoint2d	aPoint;
	aPoint.Set( 15.0f, 35.0f );
	DBG_LOG2( "PointA( %7.3f,%7.3f ) set(x,y)", aPoint.GetX(), aPoint.GetY() );
	DBG_LOG1( "Testing Point inside Rect...verdict = %s", ( Rect.IsInside( aPoint ) ? "TRUE":"FALSE" ) );
	maRect	RectB;
	RectB.SetUpperLeft( maVector2d( 15.0f, 35.0f ) );
	RectB.SetLowerRight( maVector2d( 55.0f, 55.0f ) );
	DBG_LOG4( "RectangleB LU( %7.3f, %7.3f ) RL( %7.3f, %7.3f )", RectB.GetLeft(), RectB.GetUpper(), Rect.GetRight(), Rect.GetLower() );
	DBG_LOG1( "Testing RectB inside RectA...verdict = %s", ( Rect.DoesIntersect( RectB ) ? "TRUE":"FALSE" ) );
	RectB.SetUpperLeft( maVector2d( 45.0f, 45.0f ) );
	RectB.SetLowerRight( maVector2d( 55.0f, 55.0f ) );
	DBG_LOG4( "RectangleB LU( %7.3f, %7.3f ) RL( %7.3f, %7.3f )", RectB.GetLeft(), RectB.GetUpper(), RectB.GetRight(), RectB.GetLower() );
	DBG_LOG1( "Testing RectB inside RectA...verdict = %s", ( Rect.DoesIntersect( RectB ) ? "TRUE":"FALSE" ) );
	DBG_LOG0( "" );

	//	Matrix 3x3
	//
	DBG_LOG0( "MATRIX 3x3" );
	DBG_LOG0( "----------" );
	maMatrix3x3	Mat33A;
	maMatrix3x3	Mat33B;
	Mat33A.Clear();
	DBG_LOG0( "MatrixA cleared" );
	Display( Mat33A );
	Mat33B.Identity();
	DBG_LOG0( "MatrixB indentity" );
	Display( Mat33B );
	Mat33A(1,0) = 10.0f;
	Mat33A(2,1) = 5.0f;
	DBG_LOG0( "MatrixA change 1,0 to 10.0 and 2,1 to 5" );
	Display( Mat33A );
	Mat33B.MakeRotateX( maAngle(45.0f).GetRadians() );
	DBG_LOG0( "MatrixB set RotateX" );
	Display( Mat33B );
	Mat33B.MakeRotateY( maAngle(45.0f).GetRadians() );
	DBG_LOG0( "MatrixB set RotateY" );
	Display( Mat33B );
	Mat33B.MakeRotateZ( maAngle(45.0f).GetRadians() );
	DBG_LOG0( "MatrixB set RotateZ" );
	Display( Mat33B );
	Mat33A(0,0) = 1.0f;
	Mat33A(0,1) = 2.0f;
	Mat33A(0,2) = 3.0f;
	Mat33A(1,0) = 4.0f;
	Mat33A(1,1) = 5.0f;
	Mat33A(1,2) = 6.0f;
	Mat33A(2,0) = 7.0f;
	Mat33A(2,1) = 8.0f;
	Mat33A(2,2) = 9.0f;
	DBG_LOG0( "MatrixA" );
	Display( Mat33A );
	Mat33A.Transpose();
	DBG_LOG0( "MatrixA Transpose" );
	Display( Mat33A );
	Mat33B(0,0) = 2.0f;
	Mat33B(0,1) = 2.0f;
	Mat33B(0,2) = 2.0f;
	Mat33B(1,0) = 3.0f;
	Mat33B(1,1) = 3.0f;
	Mat33B(1,2) = 3.0f;
	Mat33B(2,0) = 4.0f;
	Mat33B(2,1) = 4.0f;
	Mat33B(2,2) = 4.0f;
	DBG_LOG0( "MatrixB" );
	Display( Mat33B );
	DBG_LOG0( "MatrixA * MatrixB" );
	Display( Mat33A * Mat33B );

	DBG_LOG0("Some matrix that has an inverse");
	Mat33A.MakeRotateX(3.2f);
	Mat33A.ScaleBy(2.3f, 4.5f, -1.4f);
	Mat33A.RotateBy(3.4f, maVector3d(0, 1, 0));
	Display(Mat33A);
	DBG_LOG0("The inverse: ");
	Mat33B = Mat33A;
	Mat33B.Invert();
	Display(Mat33B);
	DBG_LOG0("The product should be the identity");

	maMatrix3x3 product3x3 = Mat33A * Mat33B;
	Display(product3x3);


	DBG_LOG0( "" );

	//	Matrix 4x4
	//
	DBG_LOG0( "MATRIX 4x4" );
	DBG_LOG0( "----------" );
	maMatrix4x4	Mat44A;
	maMatrix4x4	Mat44B;
	Mat44A.Clear();
	DBG_LOG0( "MatrixA cleared" );
	Display( Mat44A );
	Mat44B.Identity();
	DBG_LOG0( "MatrixB indentity" );
	Display( Mat44B );
	Mat44A(1,0) = 10.0f;
	Mat44A(2,1) = 5.0f;
	DBG_LOG0( "MatrixA change 1,0 to 10.0 and 2,1 to 5" );
	Display( Mat44A );
	Mat44B.MakeRotateX( 45.0f );
	DBG_LOG0( "MatrixB set RotateX" );
	Display( Mat44B );
	Mat44B.MakeRotateY( 45.0f );
	DBG_LOG0( "MatrixB set RotateY" );
	Display( Mat44B );
	Mat44B.MakeRotateZ( 45.0f );
	DBG_LOG0( "MatrixB set RotateZ" );
	Display( Mat44B );
	Mat44A(0,0) = 1.0f;
	Mat44A(0,1) = 2.0f;
	Mat44A(0,2) = 3.0f;
	Mat44A(0,3) = 4.0f;
	Mat44A(1,0) = 4.0f;
	Mat44A(1,1) = 5.0f;
	Mat44A(1,2) = 6.0f;
	Mat44A(1,3) = 7.0f;
	Mat44A(2,0) = 7.0f;
	Mat44A(2,1) = 8.0f;
	Mat44A(2,2) = 9.0f;
	Mat44A(3,3) = 9.0f;
	Mat44A(3,0) = 7.0f;
	Mat44A(3,1) = 8.0f;
	Mat44A(3,2) = 9.0f;
	Mat44A(3,3) = 9.0f;
	DBG_LOG0( "MatrixA" );
	Display( Mat44A );
	Mat44A.Transpose();
	DBG_LOG0( "MatrixA Transpose" );
	Display( Mat44A );
	Mat44B(0,0) = 2.0f;
	Mat44B(0,1) = 2.0f;
	Mat44B(0,2) = 2.0f;
	Mat44B(0,3) = 2.0f;
	Mat44B(1,0) = 3.0f;
	Mat44B(1,1) = 3.0f;
	Mat44B(1,2) = 3.0f;
	Mat44B(1,3) = 3.0f;
	Mat44B(2,0) = 4.0f;
	Mat44B(2,1) = 4.0f;
	Mat44B(2,2) = 4.0f;
	Mat44B(2,3) = 4.0f;
	Mat44B(3,0) = 4.0f;
	Mat44B(3,1) = 4.0f;
	Mat44B(3,2) = 4.0f;
	Mat44B(3,3) = 4.0f;
	DBG_LOG0( "MatrixB" );
	Display( Mat44B );
	DBG_LOG0( "MatrixA * MatrixB" );
	Display( Mat44A * Mat44B );
	DBG_LOG0( "" );

	DBG_LOG0("Multiplication of translation matrices");
	maMatrix4x4 t1, t2;
	t1.MakeTranslate(2, 3, 4);
	t2.MakeTranslate(6, 7, 8);
	DBG_LOG0("Translation 1");
	Display(t1);
	DBG_LOG0("Translation 2");
	Display(t2);
	DBG_LOG0("Product");
	t1 = t1 * t2;
	Display(t1);

	maVector3d test_rotate;
	maMatrix4x4 rx, ry, rz;

	DBG_LOG0("Rotating a vector 90 deg about X axis");
	rx.MakeRotateX( 90.0f * maConstants::c_fAngleToRad );
	test_rotate.Set(1, 1, 1);
	DBG_LOG0("Before rotate");
	Display(test_rotate);
	test_rotate = rx * test_rotate;
	DBG_LOG0("After rotate");
	Display(test_rotate);

	DBG_LOG0("Rotating a vector 90 deg about Y axis");
	ry.MakeRotateY( 90.0f * maConstants::c_fAngleToRad );
	test_rotate.Set(1, 1, 1);
	DBG_LOG0("Before rotate");
	Display(test_rotate);
	test_rotate = ry * test_rotate;
	DBG_LOG0("After rotate");
	Display(test_rotate);

	DBG_LOG0("Rotating a vector 90 deg about Z axis");
	rz.MakeRotateZ( 90.0f * maConstants::c_fAngleToRad );
	test_rotate.Set(1, 1, 1);
	DBG_LOG0("Before rotate");
	Display(test_rotate);
	test_rotate = rz * test_rotate;
	DBG_LOG0("After rotate");
	Display(test_rotate);

	DBG_LOG0("");
	DBG_LOG0("Concatenate all three rotations (order X, Y, Z)");
	maMatrix4x4 total = rx * ry * rz;
	test_rotate.Set(1, 1, 1);
	DBG_LOG0("Before rotate");
	Display(test_rotate);
	test_rotate = total * test_rotate;
	DBG_LOG0("After rotate");
	Display(test_rotate);

	DBG_LOG0("");

	maVector3d test_rotate2;

	DBG_LOG0("rotations one by one should be equal (order X, Y, Z)");
	test_rotate2.Set(1, 1, 1);
	DBG_LOG0("Before rotate");
	Display(test_rotate2);
	DBG_LOG0("Rotate X");
	test_rotate2 = rx * test_rotate2;
	Display(test_rotate2);
	DBG_LOG0("Rotate Y");
	test_rotate2 = ry * test_rotate2;
	Display(test_rotate2);
	DBG_LOG0("Rotate Z");
	test_rotate2 = rz * test_rotate2;
	Display(test_rotate2);

	DBG_LOG0("Some matrix that has an inverse");
	DBG_LOG0( "--------" );

	Mat44A.Identity();
	Mat44A.MakeRotateX(3.2f);
	Mat44A.ScaleBy(2.3f, 4.5f, -1.4f);
	Mat44A.TranslateBy(1, 2, 3);
	Mat44A.RotateBy(3.4f, maVector3d(0, 1, 0));
	Mat44A.ScaleBy(1.0f, 3.3f, -.2f);
	Display(Mat44A);
	DBG_LOG0("The inverse: ");
	DBG_LOG0( "--------" );
	Mat44B = Mat44A;
	Mat44B.Invert();
	Display(Mat44B);
	DBG_LOG0("The product should be the identity");
	DBG_LOG0( "--------" );

	maMatrix4x4 product4x4 = Mat44A * Mat44B;
	Display(product4x4);

	DBG_LOG0("");

	TestmaAxisBox();

	TestmaPlane();

	TestmaRotation(); 
	
	TestEuler();

	TestmaFunctions();

	TestmaRunningAverage();

	TestRandomStandard();

	DBG_LOG0( "" );
	DBG_LOG0( "MATH END" );
	DBG_LOG0( "======================================================" );
}

}


//====================================================================
//====================================================================
int WINAPI 
WinMain(
		HINSTANCE hInstance,      // handle to current instance
		HINSTANCE hPrevInstance,  // handle to previous instance
		LPSTR lpCmdLine,          // command line
		int nCmdShow)             // show state
{
	envPackage::Init();
	dbgPackage::Init();
	maPackage::Init();

	DoTests();
	//TestEuler();

	maPackage::CleanUp();
	dbgPackage::CleanUp();
	envPackage::CleanUp();
	return 0;
}
