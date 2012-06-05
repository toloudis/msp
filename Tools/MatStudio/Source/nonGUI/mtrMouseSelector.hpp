/*****************************************************************************
**  mtrMouseSelector.hpp
**
**		Picks with mouse input
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MTR_MOUSESELECTOR_HPP
#error mtrMouseSelector.hpp multiply included
#endif
#define MTR_MOUSESELECTOR_HPP

#ifndef MA_POINT3D_HPP
#include "maPoint3d.hpp"
#endif

class scCamera;

namespace mtrMouseSelector
{

	//====================================================================
	//	GetPickDir() - returns position and direction for
	//		picking ray.  Ray length is near plane of camera,
	//		so it needs to be resized to pick in world enviroment.
	//====================================================================
	void	GetPickDir(maPoint3d &o_Pos, maVector3d &o_Dir);

	//====================================================================
	//	GetPickRay() - returns start and end of pick ray
	//====================================================================
	void	GetPickRay(maPoint3d &o_RayStart, maPoint3d &o_RayEnd);

	//====================================================================
	//	Think
	//====================================================================
	void		Think();
}
