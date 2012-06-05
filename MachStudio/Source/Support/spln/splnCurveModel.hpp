/*****************************************************************************
**  splnCurveModel.hpp
**
**      The splnCurveModel handles highlight of curves and
**	adds control points for altering curve shape.
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#ifdef SPLN_CURVEMODEL_HPP
#error splnCurveModel.hpp multiply included
#endif
#define SPLN_CURVEMODEL_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef REL_OBJECT_HPP
#include "Core/rel/relObject.hpp"
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
class sel3dObject;
class gpxFragment;
class gpxSceneObject;


//============================================================================
//============================================================================
class splnCurveModel : public relObject
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	splnCurveModel(splnSpline &i_Curve);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	~splnCurveModel();

	//----------------------------------------------------------------------------
	// UpdateCurve - attempt to use existing objects to represent
	//	the given curve.  
	//----------------------------------------------------------------------------
	void UpdateCurve(splnSpline &i_Curve);

	//----------------------------------------------------------------------------
	//	GetControlPointObject()
	//----------------------------------------------------------------------------
	splnCurvePointObject* GetControlPointObject(int i_Index);

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
	//sel3dObject* GetParentObject() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetColor( maFloatRGBA& i_Color );

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//void add_to_scene();
	//void remove_from_scene();

	api3dObject *m_pLineObj;
	gpxSceneObject *m_pLineObjProxy;
	gpxFragment *m_pLineFragmentProxy;
	std::vector<splnCurvePointObject*> m_Objects;
	//sel3dObject* m_pParent;
	shared_ptr<relRelationship> m_CtrlPtsRelationship;

	bool m_bVisible;
};
