/*****************************************************************************
**  splnCurveSelect.hpp
**
**      The splnCurveSelect handles highlight of curves
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#ifdef SPLN_CURVESELECT_HPP
#error splnCurveSelect.hpp multiply included
#endif
#define SPLN_CURVESELECT_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

//============================================================================
//============================================================================
class splnSpline;
class splnCurvePointObject;
class splnCurveModel;
class geoPickRay;
class maFloatRGBA;
class pick3dPickList;
class pick3dPickObject;
class sel3dObject;


//============================================================================
//============================================================================
namespace splnCurveSelect
{
	//----------------------------------------------------------------------------
	//	Clear
	//----------------------------------------------------------------------------
	void Clear();

	//----------------------------------------------------------------------------
	//	CreateCurve() - create a graphical representation for the given curve,
	//		if it doesn't already exist
	//----------------------------------------------------------------------------
	splnCurveModel* CreateCurve(splnSpline *i_pCurve);

	//----------------------------------------------------------------------------
	//	RemoveCurve()
	//----------------------------------------------------------------------------
	void RemoveCurve(splnSpline *i_pCurve);

	//----------------------------------------------------------------------------
	//	UpdateCurve()
	//----------------------------------------------------------------------------
	void UpdateCurve(splnSpline *i_pCurve);

	//----------------------------------------------------------------------------
	//	SetRenderable()
	//----------------------------------------------------------------------------
	void SetRenderable(splnSpline *i_pCurve, bool i_Visible);

	//----------------------------------------------------------------------------
	//	GetControlPointObject()
	//----------------------------------------------------------------------------
	splnCurvePointObject* GetControlPointObject(splnSpline *i_pCurve, int i_Index);

	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetColor( splnSpline *i_pCurve, maFloatRGBA& i_Color );
}
