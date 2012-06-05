/*****************************************************************************
**  splnCurveMgr.hpp
**
**      The splnCurveMgr handles curves
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#ifdef SPLN_CURVEMGR_HPP
#error splnCurveMgr.hpp multiply included
#endif
#define SPLN_CURVEMGR_HPP

#ifndef SPLN_SPLINE_HPP
#include "Support/spln/splnSpline.hpp"
#endif


//--------------------------------------------------------------------
//--------------------------------------------------------------------
class relObject;


//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace splnCurveMgr
{
	typedef const char *CurveGroup;  // Use strings to group types

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Clear();

	//----------------------------------------------------------------------------
	//	GetNumCurves
	//----------------------------------------------------------------------------
	int GetNumCurves(CurveGroup i_Type);

	//----------------------------------------------------------------------------
	//	GetCurve
	//----------------------------------------------------------------------------
	splnSpline* GetCurve(CurveGroup i_Type, int i_Index);

	//----------------------------------------------------------------------------
	// Remove (delete) all curves of given type
	//----------------------------------------------------------------------------
	void ClearType(CurveGroup i_Type);

	//----------------------------------------------------------------------------
	//	AlterCtrlPoint()
	//----------------------------------------------------------------------------
	void AlterCtrlPoint(splnSpline* i_Curve, int i_PointIndex, const maPoint3d &i_Pos);

	//----------------------------------------------------------------------------
	//	InsertCtrlPoint() - insert control point after given
	//		index in place that won't alter current curve much.
	//----------------------------------------------------------------------------
	void InsertCtrlPoint(splnSpline* i_Curve, int i_PointIndex);

	//----------------------------------------------------------------------------
	//	AppendCtrlPoint() - add control point in given position to end of spline
	//----------------------------------------------------------------------------
	void AppendCtrlPoint(splnSpline* i_Curve, const maPoint3d &i_Pos);

	//----------------------------------------------------------------------------
	//	DeleteCtrlPoint() - delete control point in given position
	//----------------------------------------------------------------------------
	void DeleteCtrlPoint(splnSpline* i_Curve, const int i_PointIndex);

	//----------------------------------------------------------------------------
	//	UpdateCurve() - spline's data has changed, update curve's icon
	//----------------------------------------------------------------------------
	void UpdateCurve(splnSpline* i_Curve);

	//----------------------------------------------------------------------------
	//	AddCurve() - ownership of the spline remains with this manager
	//----------------------------------------------------------------------------
	splnSpline* AddCurve( CurveGroup i_Type,
						const maPoint3d *i_Positions,
						int i_NumPts,
						bool i_bClosed = false,
						splnSpline::SplineType i_SplineType = splnSpline::e_Catmull, 
						relObject* i_pParent = NULL);

	//----------------------------------------------------------------------------
	//	AddCurve() - ownership of the spline transfers to this manager
	//----------------------------------------------------------------------------
	void AddCurve(CurveGroup i_Group, splnSpline* i_Curve, 
					relObject* i_pParent/* = NULL*/);

	//----------------------------------------------------------------------------
	//	DeleteCurve()
	//----------------------------------------------------------------------------
	void DeleteCurve(splnSpline* i_Curve);

	//----------------------------------------------------------------------------
	//	SetRenderable()
	//----------------------------------------------------------------------------
	void SetRenderable(splnSpline* i_Curve, bool i_Visible);

/*	Spline Creation functions, turn off for now

	//----------------------------------------------------------------------------
	//	IsCurveActive - returns true if currently working on a curve
	//----------------------------------------------------------------------------
	bool IsCurveActive();

	//----------------------------------------------------------------------------
	//	StartCurve()
	//----------------------------------------------------------------------------
	void StartCurve(CurveGroup i_Type, const maPoint3d &i_Pos);

	//----------------------------------------------------------------------------
	//	AlterEndPoint()
	//----------------------------------------------------------------------------
	void AlterEndPoint(const maPoint3d &i_Pos);
	//void AlterEndPointRelative(const maVector3d &i_Diff);

	//----------------------------------------------------------------------------
	//	AddEndPoint()
	//----------------------------------------------------------------------------
	void AddEndPoint(const maPoint3d &i_Pos);

	//----------------------------------------------------------------------------
	//	FinishCurve() - returns true if enough points have been added
	//		in order to add curve to set
	//----------------------------------------------------------------------------
	splnSpline* FinishCurve(bool i_bCloseCurve);
*/
}
