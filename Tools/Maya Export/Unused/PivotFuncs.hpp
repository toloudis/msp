/*****************************************************************************
**  PivotFuncs.hpp
**
**   Namespace for pivot-point related functions, used to 
**	set pivot points of transforms to (0,0,0) since file
**	format doesn't handle rotational pivot points.    
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef PIVOTFUNCS_HPP
#error PivotFuncs.hpp multiply included
#endif
#define PIVOTFUNCS_HPP

class MFnTransform;

namespace PivotFuncs
{
	//========================================================================
	//	RemoveXformPivots - alter transformations in graph to remove
	//		rotational pivot points.  This will compile the transformations
	//		into the vertices.
	//========================================================================
	void RemoveXformPivots(MFnTransform &transform);
}


