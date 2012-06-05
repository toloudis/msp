/*****************************************************************************
**  pntPointSelect.hpp
**
**      The pntPointSelect handles highlight of points
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef PNT_POINTSELECT_HPP
#error pntPointSelect.hpp multiply included
#endif
#define PNT_POINTSELECT_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

#include <string>

//============================================================================
//	forward references
//============================================================================
class pntPoint;
class pntPointObject;
class geoPickRay;
class pick3dPickList;
class pick3dPickObject;

//============================================================================
//============================================================================
namespace pntPointSelect
{
	//----------------------------------------------------------------------------
	//	Clear
	//----------------------------------------------------------------------------
	void Clear();

	//----------------------------------------------------------------------------
	//	CreatePoint() - create graphical representation for this Point,
	//		if it doesn't already exist
	//----------------------------------------------------------------------------
	void CreatePoint(pntPoint *i_pPoint, 
					 pick3dPickObject* i_pParent,
					 const std::string& i_Name);

	//----------------------------------------------------------------------------
	//	RemovePoint()
	//----------------------------------------------------------------------------
	void RemovePoint(pntPoint *i_pPoint);

	//----------------------------------------------------------------------------
	//	UpdatePoint()
	//----------------------------------------------------------------------------
	void UpdatePoint(pntPoint *i_pPoint);

	//----------------------------------------------------------------------------
	//	SetRenderable()
	//----------------------------------------------------------------------------
	void SetRenderable(pntPoint *i_pPoint, bool i_Visible);

	//----------------------------------------------------------------------------
	//	GetPointObject()
	//----------------------------------------------------------------------------
	pntPointObject* GetPointObject(pntPoint *i_pPoint);

	//----------------------------------------------------------------------------
	// Do ray pick on control point objects
	//----------------------------------------------------------------------------
	bool ObjectPick(geoPickRay& i_Ray, pick3dPickList& io_PickList);

	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode);
}