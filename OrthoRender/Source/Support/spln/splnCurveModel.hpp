/*****************************************************************************
**  splnCurveModel.hpp
**
**      The splnCurveModel handles highlight of curves and
**	adds control points for altering curve shape.
**
**	Extra Large Technology
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#ifdef SPLN_CURVEMODEL_HPP
#error splnCurveModel.hpp multiply included
#endif
#define SPLN_CURVEMODEL_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

#include <vector>

//============================================================================
//============================================================================
class splnSpline;
class splnCurvePointObject;
class api3dObject;
class geoPickRay;
class maFloatRGBA;
class pick3dPickList;
class pick3dPickObject;


//============================================================================
//============================================================================
class splnCurveModel
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	splnCurveModel(splnSpline &i_Curve, pick3dPickObject* i_pParent);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	~splnCurveModel();

	//----------------------------------------------------------------------------
	// UpdateCurve - attempt to use existing objects to represent
	//	the given curve.  If the number of control points are
	//	different, then false is returned.
	//----------------------------------------------------------------------------
	bool UpdateCurve(splnSpline &i_Curve);

	//----------------------------------------------------------------------------
	//	GetControlPointObject()
	//----------------------------------------------------------------------------
	splnCurvePointObject* GetControlPointObject(int i_Index);

	//----------------------------------------------------------------------------
	// Do ray pick on control point objects
	//----------------------------------------------------------------------------
	bool ObjectPick(geoPickRay& i_Ray, pick3dPickList& io_PickList);

	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode);

	//----------------------------------------------------------------------------
	//	Render the curve or not
	//----------------------------------------------------------------------------
	void SetRenderable( bool i_bVisible );

	//----------------------------------------------------------------------------
	//	return whether the curve is visible or not
	//----------------------------------------------------------------------------
	bool GetRenderable();

	//----------------------------------------------------------------------------
	// Get parent object of the spline in order for selection to trace from
	//	this spline back to the controlled object.
	//----------------------------------------------------------------------------
	pick3dPickObject* GetParentObject() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetColor( maFloatRGBA& i_Color );

private:

	api3dObject *m_pLineObj;
	std::vector<splnCurvePointObject*> m_Objects;
	pick3dPickObject* m_pParent;

	bool m_bVisible;
};
