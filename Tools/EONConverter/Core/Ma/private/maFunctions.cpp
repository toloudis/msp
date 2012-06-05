/****************************************************************************\
**  maFunctions.cpp
**
**      This file contains the package generic functions for the math package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/ma/maFunctions.hpp"

#include "Core/ma/maConstants.hpp"
//#include "Core/ma/maRand.hpp"
//#include "Core/ma/maRotation.hpp"

#include <stdlib.h>
#include <time.h>

namespace
{
	// This does not require maPackage::Init().  The truly proper way to do this
	// is instantiate in an maFunctions::Init() and clean up in maFunctions::CleanUp.
	// we did not do that here, knowing the implementation of maRand32

	//	As soon as you can notice that this is not random enough, go ahead and implement
	//	a more complicated system.  Just remember you can't use time on the PS2.
	//const int l_ThisIsRandomEnough = 1355;
	//maRand32	l_randgen( l_ThisIsRandomEnough );
	//// ensure float is non-negative by dividing max by 2, ffffffff / 2 = 7fffffff
	//float		lc_fRandPositiveMax = (float)( l_randgen.RandMax() / 2 );
}

namespace maFunctions
{

	//========================================================================
	//	FloatRand returns a random floating point number in the range
	//	[i_Lo, i_Hi)
	//========================================================================
	//float FloatRand(float i_Lo, float i_Hi)
	//{
	//	// old Rand code, worked fine for old rand code, 
	//	// Runs at an average of 1 entry every 269 nanoseconds over a ten million 
	//	// number sample. -- TMS 2/4/02
	//	//
	//	// int num = rand();
	//	// float ret_val = float(num) / float(RAND_MAX);
	//
	//	// This runs at an average of 1 number every 818 nanoseconds over a ten million
	//	// number sample.
	//	// This is about 3 times slower than Microsoft's rand(), but the random number distribution is 
	//	// significantly improved.  A trade-off, we choose accuracy.  -- TMS 2/4/02
	//	//
	//	envType::UInt32 num = ( l_randgen.Rand() / 2 );  // just the positive entries
	//	float ret_val = float(num) / float( lc_fRandPositiveMax );

	//	// They share this code to the end
	//	ret_val *= i_Hi - i_Lo;
	//	ret_val += i_Lo;

	//	return ret_val;

	//}

	////========================================================================
	////	IntRand returns a random integer in the range [i_Lo, i_Hi)
	////========================================================================
	//int IntRand(int i_Lo, int i_Hi)
	//{
	//	// This could alternatively call into FloatRand.
	//	// Question: What is slower, a FP mul + integer convert, or a integer
	//	// mod?  Some places say that the FP mul gives better uniformity.

	//	envType::UInt32 num = ( l_randgen.Rand() / 2 );  // just the positive entries
	//	num = num % (i_Hi-i_Lo+1);
	//	return i_Lo + num;
	//}

	////========================================================================
	////  GetRelativeYawPitch returns the yaw and pitch of the direction vectors 
	////	world orientation
	////----------------------------------------------------------------------------
	//void GetYawPitch(const maVector3d& i_DirectionVec, float& o_Yaw, float& o_Pitch)
	//{	
	//	// find yaw and pitch between weapon_dir and target_in_obj_space
	//	o_Yaw = atan2f(i_DirectionVec.GetX(), i_DirectionVec.GetZ());
	//	float len = i_DirectionVec.GetX() * i_DirectionVec.GetX() + i_DirectionVec.GetZ() * i_DirectionVec.GetZ();
	//	len = sqrtf(len);
	//	o_Pitch = atan2f(i_DirectionVec.GetY(), len);
	//}

	////----------------------------------------------------------------------------
	////  GetRelativeyawPitch returns the yaw and pitch of the direction vector 
	////	relative to the orientation that is passed in.
	////----------------------------------------------------------------------------
	//void GetRelativeYawPitch(const maRotation& i_Rotation, const maVector3d& i_DirectionVec, float& o_Yaw, float& o_Pitch)
	//{	
	//	maRotation inv_rot = i_Rotation;
	//	inv_rot.Invert();
	//	maVector3d target_dir = i_DirectionVec;
	//	inv_rot.RotateVector(target_dir);

	//	// find yaw and pitch between weapon_dir and target_in_obj_space
	//	o_Yaw = atan2f(target_dir.GetX(), target_dir.GetZ());
	//	float len = target_dir.GetX() * target_dir.GetX() + target_dir.GetZ() * target_dir.GetZ();
	//	len = sqrtf(len);
	//	o_Pitch = atan2f(target_dir.GetY(), len);
	//}

	////----------------------------------------------------------------------------
	////  GetRelativeDirection returns a new direction vector 
	////	relative to the orientation that is passed in.
	////----------------------------------------------------------------------------
	//void GetRelativeDirection(const maRotation& i_Rotation, const maVector3d& i_DirectionVec, maVector3d& o_DirectionVec)
	//{
	//	maRotation inv_rot = i_Rotation;
	//	inv_rot.Invert();
	//	o_DirectionVec = i_DirectionVec;
	//	inv_rot.RotateVector(o_DirectionVec);
	//}

	////----------------------------------------------------------------------------
	////	LinesIntersect() returns:
	////	0 - no intersection
	////	1 - intersection
	////	2 - Collinear
	////
	////	Note: adapted from Graphics Gems 2
	////----------------------------------------------------------------------------
	//int LinesIntersect( maPoint2d& i_Pt1, maPoint2d& i_Pt2, maPoint2d& i_Pt3, maPoint2d& i_Pt4, maPoint2d& o_Pt )
	//{
	//	///**************************************************************
	//	// *                                                            *
	//	// *    NOTE:  The following macro to determine if two numbers  *
	//	// *    have the same sign, is for 2's complement number        *
	//	// *    representation.  It will need to be modified for other  *
	//	// *    number systems.                                         *
	//	// *                                                            *
	//	// **************************************************************/
	//	#define SAME_SIGNS( a, b )	\
	//			(((long)a ^ (long)b) >= 0 )

	//	float x1, y1, x2, y2, x3, y3, x4, y4;
	//	float a1, a2, b1, b2, c1, c2; ///* Coefficients of line eqns. */
	//	float r1, r2, r3, r4;         ///* 'Sign' values */
	//	float denom, offset, num;     ///* Intermediate values */

	//	x1 = i_Pt1.GetX();
	//	y1 = i_Pt1.GetY();
	//	x2 = i_Pt2.GetX();
	//	y2 = i_Pt2.GetY();
	//	x3 = i_Pt3.GetX();
	//	y3 = i_Pt3.GetY();
	//	x4 = i_Pt4.GetX();
	//	y4 = i_Pt4.GetY();

	//	///* Compute a1, b1, c1, where line joining points 1 and 2
	//	// * is "a1 x  +  b1 y  +  c1  =  0".
	//	// */

	//	a1 = y2 - y1;
	//	b1 = x1 - x2;
	//	c1 = x2 * y1 - x1 * y2;

	//	///* Compute r3 and r4.
	//	// */


	//	r3 = a1 * x3 + b1 * y3 + c1;
	//	r4 = a1 * x4 + b1 * y4 + c1;

	//	///* Check signs of r3 and r4.  If both point 3 and point 4 lie on
	//	// * same side of line 1, the line segments do not intersect.
	//	// */

	//	if ( r3 != 0 &&
	//		 r4 != 0 &&
	//		 SAME_SIGNS( r3, r4 ))
	//		return ( 0 ); 	// DO NOT INTERSECT

	//	///* Compute a2, b2, c2 */

	//	a2 = y4 - y3;
	//	b2 = x3 - x4;
	//	c2 = x4 * y3 - x3 * y4;

	//	///* Compute r1 and r2 */

	//	r1 = a2 * x1 + b2 * y1 + c2;
	//	r2 = a2 * x2 + b2 * y2 + c2;

	//	///* Check signs of r1 and r2.  If both point 1 and point 2 lie
	//	// * on same side of second line segment, the line segments do
	//	// * not intersect.
	//	// */

	//	if ( r1 != 0 &&
	//		 r2 != 0 &&
	//		 SAME_SIGNS( r1, r2 ))
	//		return ( 0 );	// DO NOT INTERSECT

	//	///* Line segments intersect: compute intersection point. 
	//	// */

	//	denom = a1 * b2 - a2 * b1;
	//	if ( denom == 0 )
	//		return ( 2 );	// COLLINEAR
	//	offset = denom < 0 ? - denom / 2 : denom / 2;

	//	///* The denom/2 is to get rounding instead of truncating.  It
	//	// * is added or subtracted to the numerator, depending upon the
	//	// * sign of the numerator.
	//	// */

	//	num = b1 * c2 - b2 * c1;
	//	o_Pt.SetX( ( num < 0 ? num - offset : num + offset ) / denom );

	//	num = a2 * c1 - a1 * c2;
	//	o_Pt.SetY( ( num < 0 ? num - offset : num + offset ) / denom );

	//	return ( 1 );	// DO INTERSECT
	//}	

	////------------------------------------------------------------------------
	////	Jitter() - calculate random jittered sample positions 
	////	number of samples returned is i_nSamples*i_nSamples.
	////	these are 2d sample positions, with a z coordinate being a weight factor
	////	samples are in a grid with random shifts inside of grid cells.
	////	sample positions range from 0,0 to 1,1
	////	I arbitrarily restrict the max nsamples to 8, or 64 sample positions.
	////	JitterAmount is how much to wiggle the sample point within its cell
	////	0 means only at center of cell, 1 means can be anywhere in entire cell
	////------------------------------------------------------------------------
	//void GetSamples2D(int i_nSamples, std::vector<maVector3d>& o_Samples, 
	//	float i_JitterAmount /* = 1 */ )
	//{
	//	DBG_ASSERT0(i_nSamples > 0, "Bad input to GetSamples2D, number of samples < 1");
	//	DBG_ASSERT0(i_nSamples < 9, "Bad input to GetSamples2D, number of samples > 8");
	//	DBG_ASSERT0(i_JitterAmount >= 0, "Bad input to GetSamples2D, jitterAmount < 0");
	//	DBG_ASSERT0(i_JitterAmount <= 1, "Bad input to GetSamples2D, jitterAmount > 1");

	//	o_Samples.resize(i_nSamples*i_nSamples);
	//	o_Samples.clear();

	//	// never jitter the trivial case?
	//	if (i_nSamples == 1)
	//	{
	//		o_Samples.push_back(maVector3d(0.5f,0.5f,1.0f));
	//		return;
	//	}


	//	float sampleSpacing = 1.0f / (float)i_nSamples;

	//	// example:
	//	// nsamples = 4, samplespacing = 0.25
	//	// 0                       1
	//	// |--*--|--*--|--*--|--*--|
	//	//  0.125 0.375 0.625 0.875
	//	//    0     1     2     3
	//	//  

	//	int i,j;
	//	float x,y,z,jit;
	//	for (i = 0; i < i_nSamples; i++)
	//	{
	//		// center of jitter cell for sample
	//		y = ((float)i + 0.5f) * sampleSpacing;

	//		// add random jitter
	//		jit = FloatRand(-0.5, 0.5);
	//		jit *= i_JitterAmount;
	//		jit *= sampleSpacing;
	//		y += jit;

	//		for (j = 0; j < i_nSamples; j++)
	//		{
	//			x = ((float)j + 0.5f) * sampleSpacing;

	//			// add random jitter
	//			jit = FloatRand(-0.5, 0.5);
	//			jit *= i_JitterAmount;
	//			jit *= sampleSpacing;
	//			x += jit;

	//			// weight = area of cell (box filter)
	//			// this could be abstracted to pass the sample position 
	//			// into a filter function to get a weight
	//			z = sampleSpacing*sampleSpacing;

	//			o_Samples.push_back(maVector3d(x,y,z));
	//		}
	//	}
	//}
	//void GetSamples2D_NRooks(int i_nSamples, std::vector<maVector3d>& o_Samples)
	//{
	//	DBG_ASSERT0(i_nSamples > 0, "Bad input to GetSamples2D_NRooks, number of samples < 1");
	//	DBG_ASSERT0(i_nSamples < 9, "Bad input to GetSamples2D_NRooks, number of samples > 8");

	//	o_Samples.resize(i_nSamples*i_nSamples);
	//	o_Samples.clear();

	//	// never jitter the trivial case?
	//	if (i_nSamples == 1)
	//	{
	//		o_Samples.push_back(maVector3d(0.5f,0.5f,1.0f));
	//		return;
	//	}

	//	// note that i square the sample spacing already here:
	//	float sampleSpacing = 1.0f / (float)(i_nSamples*i_nSamples);

	//	// example:
	//	// nsamples = 4, samplespacing = 0.25
	//	// 0                       1
	//	// |--*--|--*--|--*--|--*--|
	//	//  0.125 0.375 0.625 0.875
	//	//    0     1     2     3
	//	//  

	//	// fill sample cells down diagonal (with jitter within cell?)
	//	int i;
	//	float x,y,z;
	//	for( i = 0; i < i_nSamples*i_nSamples; ++i )
	//	{
 //           x = (FloatRand(0,1) + i)*sampleSpacing;
 //           y = (FloatRand(0,1) + i)*sampleSpacing;
	//		// for no jitter, this would be:
	//		// x = (i+0.5)*sampleSpacing;
	//		z = sampleSpacing;
	//		o_Samples.push_back(maVector3d(x,y,z));
	//	}
	//	// shuffle the x coordinates.
	//	for( i = (i_nSamples*i_nSamples) - 1; i > 0; --i )
	//	{
	//		int target = IntRand(0,i);
	//		// swap i with target
	//		x = o_Samples[i].GetX();
	//		o_Samples[i].SetX(o_Samples[target].GetX());
	//		o_Samples[target].SetX(x);
	//	}
	//}

	////-----------------------------------------------------------------------------
	//// Name: GaussianDistribution
	//// Desc: Helper function for GetSampleOffsets function to compute the 
	////       2 parameter Gaussian distrubution using the given standard deviation
	////       rho
	////-----------------------------------------------------------------------------
	//float GaussianDistribution( float i_X, float i_Y, float i_Rho )
	//{
	//	float g = 1.0f / sqrtf( 2.0f * maConstants::c_fPI * i_Rho * i_Rho );
	//	g *= expf( -(i_X*i_X + i_Y*i_Y)/(2*i_Rho*i_Rho) );

	//	return g;
	//}

	////-----------------------------------------------------------------------------
	//// Log() - compute the logarithm of a number in any base
	////-----------------------------------------------------------------------------
	//float Log(float i_Val, float i_Base)
	//{
	//   return log(i_Val) / log(i_Base);
	//}

}
