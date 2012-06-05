/*****************************************************************************
**  pntPointMgr.hpp
**
**      The pntPointMgr handles points
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef PNT_POINTMGR_HPP
#error pntPointMgr.hpp multiply included
#endif
#define PNT_POINTMGR_HPP

#ifndef PNT_POINT_HPP
#include "Support/pnt/pntPoint.hpp"
#endif

class pntPointObject;

namespace pntPointMgr
{
	typedef const char *PointGroup;  // Use strings to group types

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Clear();

	//----------------------------------------------------------------------------
	//	GetNumPoints
	//----------------------------------------------------------------------------
	int GetNumPoints(PointGroup i_Type);

	//----------------------------------------------------------------------------
	//	GetPoint
	//----------------------------------------------------------------------------
	pntPoint* GetPoint(PointGroup i_Type);

	//----------------------------------------------------------------------------
	// Remove (delete) all points of given type
	//----------------------------------------------------------------------------
	void ClearType(PointGroup i_Type);

	//----------------------------------------------------------------------------
	//	UpdatePoint() - point's data has changed, update point's icon
	//----------------------------------------------------------------------------
	void UpdatePoint(pntPoint* i_Point);

	//----------------------------------------------------------------------------
	//	AddPoint()
	//----------------------------------------------------------------------------
	pntPointObject* AddPoint(PointGroup i_Group, 
				  pntPoint* i_Point, 
				  const std::string& i_Name);

	//----------------------------------------------------------------------------
	//	RemovePoint()
	//----------------------------------------------------------------------------
	void RemovePoint(pntPoint* i_Point);

	//----------------------------------------------------------------------------
	//	SetRenderable()
	//----------------------------------------------------------------------------
	void SetRenderable(pntPoint* i_Point, bool i_Visible);
}