/*****************************************************************************
**  maFunctions.hpp
**
**      This file contains the non-component related functions for the
**	for the Math package.  
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MA_FUNCTIONS_HPP
#error maFunctions.hpp multiply included
#endif
#define MA_FUNCTIONS_HPP

#ifndef DBG_ASSERT_HPP
#include "Core/dbg/dbgAssert.hpp"	// required for checking max/min in rand methods
#endif

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif

#include <stdlib.h>  // rand, RAND_MAX
#include <vector>

//============================================================================
//	forward references
//============================================================================
class maRotation;
class maVector3d;

//============================================================================
//============================================================================
namespace maFunctions
{
	//========================================================================
	//	Highest Value
	//========================================================================
	template <class T>
	inline T Highest(const T& i_Value1, const T& i_Value2);
	template <class T>
	inline T Highest(const T& i_Value1, const T& i_Value2, const T& i_Value3);

	//========================================================================
	//	Lowest Value
	//========================================================================
	template <class T>
	inline T Lowest(const T& i_Value1, const T& i_Value2);
	template <class T>
	inline T Lowest(const T& i_Value1, const T& i_Value2, const T& i_Value3);

	//========================================================================
	//	Swap Value
	//========================================================================
	template <class T>
	inline void Swap(T& io_Value1, T& io_Value2);

	//========================================================================
	//	Clamp
	//========================================================================
	template <class T>
	inline void Clamp(T& io_Value, const T& i_Low, const T& i_High);

	//========================================================================
	//	FloatRand returns a random floating point number in the range
	//	[i_Lo, i_Hi)
	//========================================================================
	float FloatRand(float i_Lo, float i_Hi);

	//========================================================================
	//	IntRand returns a random integer in the range [i_Lo, i_Hi)
	//========================================================================
	int IntRand(int i_Lo, int i_Hi);

	//========================================================================
	//	float-specific absolute value
	//========================================================================
	float FAbs(float i_Val);

	//========================================================================
	//	Returns i_Val rounded up to the nearest multiple of i_Spacing, WHICH
	//	MUST BE A POWER OF 2.
	//========================================================================
	int RoundUp(int i_Val, int i_Spacing);

	//========================================================================
	//	Highest Value
	//========================================================================
	template <class T>
	inline T Highest(const T& i_Value1, const T& i_Value2)
	{
		return (i_Value1 >= i_Value2) ? i_Value1 : i_Value2;
	}

	template <class T>
	inline T Highest(const T& i_Value1, const T& i_Value2, const T& i_Value3)
	{
		if( i_Value1 > i_Value2 )
		{
			if( i_Value1 > i_Value3 )
				return i_Value1;
			else
				return i_Value3;
		}
		else
		{
			if( i_Value2 > i_Value3 )
				return i_Value2;
			else
				return i_Value3;
		}
	}

	//========================================================================
	//	Lowest Value
	//========================================================================
	template <class T>
	inline T Lowest(const T& i_Value1, const T& i_Value2)
	{
		return (i_Value1 <= i_Value2) ? i_Value1 : i_Value2;
	}

	template <class T>
	inline T Lowest(const T& i_Value1, const T& i_Value2, const T& i_Value3)
	{
		if( i_Value1 < i_Value2 )
		{
			if( i_Value1 < i_Value3 )
				return i_Value1;
			else
				return i_Value3;
		}
		else
		{
			if( i_Value2 < i_Value3 )
				return i_Value2;
			else
				return i_Value3;
		}
	}

	//========================================================================
	//	Swap Value
	//========================================================================
	template <class T>
	inline void Swap(T& io_Value1, T& io_Value2)
	{
		const T temp = io_Value1;
		io_Value1 = io_Value2;
		io_Value2 = temp;
	}

	//========================================================================
	//	Clamp
	//========================================================================
	template <class T>
	inline void Clamp(T& io_Value, const T& i_Low, const T& i_High)
	{
		if( io_Value < i_Low )
			io_Value = i_Low;
		else if( io_Value > i_High )
			io_Value = i_High;
	}

	//========================================================================
	//	float-specific absolute value
	//========================================================================
	inline float FAbs(float i_Val)
	{
		if( i_Val >= 0 )
			return i_Val;
		else
			return -i_Val;
	}

	//========================================================================
	//	Returns i_Val rounded up to the nearest multiple of i_Spacing, WHICH
	//	MUST BE A POWER OF 2.
	//========================================================================
	inline int RoundUp(int i_Val, int i_Spacing)
	{
		DBG_ASSERT0((i_Spacing & (i_Spacing - 1)) == 0, "i_Spacing must be a power of 2");
		return (i_Val + (i_Spacing - 1)) & (~(i_Spacing - 1));
	}

	////----------------------------------------------------------------------------
	////  GetRelativeYawPitch returns the yaw and pitch of the direction vectors 
	////	world orientation
	////----------------------------------------------------------------------------
	//void GetYawPitch(const maVector3d& i_DirectionVec, float& o_Yaw, float& o_Pitch);

	////----------------------------------------------------------------------------
	////  GetRelativeYawPitch returns the yaw and pitch of the direction vector 
	////	relative to the orientation that is passed in.
	////----------------------------------------------------------------------------
	//void GetRelativeYawPitch(const maRotation& i_Rotation, const maVector3d& i_DirectionVec, float& o_Yaw, float& o_Pitch);

	////----------------------------------------------------------------------------
	////  GetRelativeDirection returns a new direction vector 
	////	relative to the orientation that is passed in.
	////----------------------------------------------------------------------------
	//void GetRelativeDirection(const maRotation& i_Rotation, const maVector3d& i_DirectionVec, maVector3d& o_DirectionVec);

	////----------------------------------------------------------------------------
	////	LinesIntersect() returns:
	////	0 - no intersection
	////	1 - intersection
	////	2 - Collinear
	////
	////	Note: adapted from Graphics Gems 2
	////----------------------------------------------------------------------------
	//int LinesIntersect( maPoint2d& i_Pt1, maPoint2d& i_Pt2, maPoint2d& i_Pt3, maPoint2d& i_Pt4, maPoint2d& o_Pt );

	////------------------------------------------------------------------------
	////	GetSamples2D() - calculate random jittered sample positions 
	////	these are 2d sample positions, with a z coordinate being a weight factor
	////	sample positions range from 0,0 to 1,1
	////------------------------------------------------------------------------
	//void GetSamples2D(int i_nSamples, std::vector<maVector3d>& o_Samples, float i_JitterAmount = 1 );
	//void GetSamples2D_NRooks(int i_nSamples, std::vector<maVector3d>& o_Samples);

	////-----------------------------------------------------------------------------
	//// GaussianDistribution() - compute the 2 parameter Gaussian distrubution 
	////	using the given standard deviation rho
	////-----------------------------------------------------------------------------
	//float GaussianDistribution( float i_X, float i_Y, float i_Rho );

	////-----------------------------------------------------------------------------
	//// Log() - compute the logarithm of a number in any base
	////-----------------------------------------------------------------------------
	//float Log(float i_Val, float i_Base);

}

