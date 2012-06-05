/*****************************************************************************
**  pntPointModel.hpp
**
**      The pntPointModel handles highlight of points and
**	adds control points for altering point shape.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef PNT_POINTMODEL_HPP
#error pntPointModel.hpp multiply included
#endif
#define PNT_POINTMODEL_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

class pntPoint;
class pntPointObject;
class api3dObject;
class geoPickRay;
class pick3dPickList;
class pick3dPickObject;

#include <vector>

class pntPointModel
{
public:
	//----------------------------------------------------------------------------
	//---------------------------------------------------------------------------- 
	pntPointModel(pntPoint &i_Point, 
				  pick3dPickObject* i_pParent,
				  const std::string& i_Name);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	~pntPointModel();

	//----------------------------------------------------------------------------
	// UpdatePoint - attempt to use existing objects to represent
	//	the given point.  If the number of control points are
	//	different, then false is returned.
	//----------------------------------------------------------------------------
	bool UpdatePoint(pntPoint &i_Point);

	//----------------------------------------------------------------------------
	//	GetPointObject()
	//----------------------------------------------------------------------------
	pntPointObject* GetPointObject();

	//----------------------------------------------------------------------------
	// Do ray pick on control point objects
	//----------------------------------------------------------------------------
	bool ObjectPick(geoPickRay& i_Ray, pick3dPickList& io_PickList);

	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	bool MatchPickCode(envType::UInt32 i_PickCode) const;

	//----------------------------------------------------------------------------
	//	Render the point or not
	//----------------------------------------------------------------------------
	void SetRenderable( bool i_bVisible );

	//----------------------------------------------------------------------------
	//	return whether the point is visible or not
	//----------------------------------------------------------------------------
	bool GetRenderable();

	//----------------------------------------------------------------------------
	// Get parent object of the spline in order for selection to trace from
	//	this spline back to the controlled object.
	//----------------------------------------------------------------------------
	pick3dPickObject* GetParentObject() const;

private:

	pntPointObject*	m_pObject;
	pick3dPickObject* m_pParent;
	//bool			m_bVisible;
};
