/*****************************************************************************
**  pntPointMgr.hpp
**
**      The pntPointMgr handles points
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef PNT_POINTMGR_HPP
#error pntPointMgr.hpp multiply included
#endif
#define PNT_POINTMGR_HPP

#ifndef PNT_POINT_HPP
#include "Support/pnt/pntPoint.hpp"
#endif

class pick3dPickObject;

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
	void AddPoint(PointGroup i_Group, 
				  pntPoint* i_Point, 
				  pick3dPickObject* m_pParent,
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