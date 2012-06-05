/*****************************************************************************
**	rtIntersections.hpp
**
**		Ray tracing utility functions
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef RT_INTERSECTIONS_HPP
#error rtIntersections.hpp multiply included
#endif
#define RT_INTERSECTIONS_HPP

#ifndef RT_STRUCTURES_HPP
#include "Graphics/rt/rtStructures.hpp"
#endif


//============================================================================
//============================================================================
namespace rtIntersections
{
	//--------------------------------------------------------------------
	// IntersectTriangle()
	//--------------------------------------------------------------------
	int IntersectTriangle(IntersectionData &id, Ray ray, Triangle triangle, float &tmin);

	//--------------------------------------------------------------------
	// IntersectSphere()
	//--------------------------------------------------------------------
	int IntersectSphere(IntersectionData &id , Ray ray , Sphere sphere , float &tmin );

	//--------------------------------------------------------------------
	// IntersectRectangle()
	//--------------------------------------------------------------------
	int IntersectRectangle(IntersectionData &id , Ray ray , Rectangle rectangle , float &tmin );
};

