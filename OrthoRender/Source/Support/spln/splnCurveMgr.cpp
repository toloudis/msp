/*****************************************************************************
**  splnCurveMgr.cpp
**
**      The splnCurveMgr handles curves
**
**	Extra Large Technology
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#include "Support/spln/splnCurveMgr.hpp"
#include "Support/spln/splnCurvePointObject.hpp"
#include "Support/spln/splnCurveSelect.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"

#include <map>
#include <string>


//-------------------------------------------------------------------=
//-------------------------------------------------------------------=
namespace splnCurveMgr
{
namespace
{
// Use Map from string to list of splines
typedef std::vector<splnSpline*> CurveList;
std::map<std::string, CurveList> l_Curves;

//splnSpline* l_ActiveCurve = NULL;

void delete_curve(splnSpline* i_Curve)
{
	splnCurveSelect::RemoveCurve(i_Curve);
	std::map<std::string, CurveList>::iterator it, end = l_Curves.end();
	for (it = l_Curves.begin(); it != end; ++it)
		envSTLHelpers::DeleteOneValue(it->second, i_Curve);
}

void update_curve(splnSpline* i_Curve)
{
	// callbacks?
}

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Clear()
{
	std::map<std::string, CurveList>::iterator it, end = l_Curves.end();
	for (it = l_Curves.begin(); it != end; ++it)
		envSTLHelpers::DeleteContainer(it->second);
	//l_ActiveCurve = NULL;
}


//----------------------------------------------------------------------------
//	GetNumCurves
//----------------------------------------------------------------------------
int GetNumCurves(CurveGroup i_Group)
{
	return l_Curves[i_Group].size();
}

//----------------------------------------------------------------------------
//	GetCurve
//----------------------------------------------------------------------------
splnSpline* GetCurve(CurveGroup i_Group, int i_Index)
{
	return l_Curves[i_Group][i_Index];
}

//----------------------------------------------------------------------------
// Remove (delete) all curves of given type
//----------------------------------------------------------------------------
void ClearType(CurveGroup i_Group)
{
	envSTLHelpers::ForAll(l_Curves[i_Group], splnCurveSelect::RemoveCurve);
	envSTLHelpers::DeleteContainer(l_Curves[i_Group]);
}

//----------------------------------------------------------------------------
//	AlterCtrlPoint()
//----------------------------------------------------------------------------
void AlterCtrlPoint(splnSpline* i_Curve, int i_PointIndex, const maPoint3d &i_Pos)
{
	splnSpline *curve = i_Curve;
	i_Curve->SetPointPos(i_PointIndex, i_Pos);
	splnCurveSelect::UpdateCurve(i_Curve);
	update_curve(i_Curve);
}

//----------------------------------------------------------------------------
//	InsertCtrlPoint() - insert control point after given
//		index in place that won't alter current curve much.
//----------------------------------------------------------------------------
void InsertCtrlPoint(splnSpline* i_Curve, int i_PointIndex)
{
	//if (l_Curves.empty()) return;
	//splnSpline *curve = l_Curves[0];
	splnSpline *curve = i_Curve;
	if (i_PointIndex < curve->GetNumPoints()-1 && i_PointIndex >= 0)
	{
		// Insert in middle
		float pct1 = curve->GetPointPercent(i_PointIndex);
		float pct2 = curve->GetPointPercent(i_PointIndex+1);
		maPoint3d pos = curve->Evaluate((pct1 + pct2) * 0.5f);
		curve->InsertPoint(i_PointIndex, pos);
	}
	else
	{
		// Insert at end
		maVector3d dir;
		maPoint3d pos = curve->Evaluate(0.99f, &dir);
		curve->InsertPoint(curve->GetNumPoints()-1, pos + dir * 4.0f);
	}
	splnCurveSelect::UpdateCurve(curve);
	sel3dMgr::CreateUndoOperation();
	sel3dMgr::Select(splnCurveSelect::GetControlPointObject(curve, i_PointIndex+1));

	update_curve(i_Curve);

}

//----------------------------------------------------------------------------
//	AppendCtrlPoint() - add control point in given position to end of spline
//----------------------------------------------------------------------------
void AppendCtrlPoint(splnSpline* i_Curve, const maPoint3d &i_Pos)
{
	splnSpline *curve = i_Curve;
	curve->AppendPoint(i_Pos);

	splnCurveSelect::UpdateCurve(curve);
	// I'm not going to select the control point, because I think the
	// interaction involves moving the main object and then doing this
	// append action. So, the next action is likely moving the main
	// object again, not the new control point.
	//sel3dMgr::Select(splnCurveSelect::GetControlPointObject(curve, curve->GetNumPoints()-1));
	update_curve(i_Curve);
}

//----------------------------------------------------------------------------
//	DeleteCtrlPoint() - delete control point in given position
//----------------------------------------------------------------------------
void DeleteCtrlPoint(splnSpline* i_Curve, const int i_PointIndex)
{
	DBG_ASSERT0(i_Curve != 0, "NULL Spline");
	DBG_ASSERT0( (i_PointIndex < i_Curve->GetNumPoints()), "Point Index out of range");

	i_Curve->DeletePoint(i_PointIndex);

	splnCurveSelect::UpdateCurve(i_Curve);

	update_curve(i_Curve);
}

//----------------------------------------------------------------------------
//	UpdateCurve() - spline's data has changed, update curve's icon
//----------------------------------------------------------------------------
void UpdateCurve(splnSpline* i_Curve)
{
	splnCurveSelect::UpdateCurve(i_Curve);
	//?update_curve(i_Curve);
}

//----------------------------------------------------------------------------
//	AddCurve()
//----------------------------------------------------------------------------
splnSpline* AddCurve(CurveGroup i_Group, const maPoint3d *i_Positions, int i_NumPts,
						bool i_bClosed,
						splnSpline::SplineType i_SplineType, 
						pick3dPickObject* i_pParent)
{
	splnSpline::SplineType type = i_SplineType;
	splnSpline *pCurve = new splnSpline(type);
	pCurve->SetClosed(i_bClosed);
	pCurve->SetPoints(i_Positions, i_NumPts);
	l_Curves[i_Group].push_back(pCurve);

	splnCurveSelect::CreateCurve(pCurve, i_pParent);
	return pCurve;
}

//----------------------------------------------------------------------------
//	AddCurve()
//----------------------------------------------------------------------------
void AddCurve(CurveGroup i_Group, splnSpline* i_Curve, 
						pick3dPickObject* i_pParent)
{
	l_Curves[i_Group].push_back(i_Curve);
	splnCurveSelect::CreateCurve(i_Curve, i_pParent);
}

//----------------------------------------------------------------------------
//	DeleteCurve()
//----------------------------------------------------------------------------
void DeleteCurve(splnSpline* i_Curve)
{
	delete_curve(i_Curve);
}

//----------------------------------------------------------------------------
//	SetRenderable()
//----------------------------------------------------------------------------
void SetRenderable(splnSpline* i_Curve, bool i_Visible)
{
	splnCurveSelect::SetRenderable(i_Curve, i_Visible);
}

/*	Spline Creation functions, turn off for now

//----------------------------------------------------------------------------
//	IsCurveActive - returns true if currently working on a curve
//----------------------------------------------------------------------------
bool IsCurveActive()
{
	return (l_ActiveCurve != NULL);
}

//----------------------------------------------------------------------------
//	StartCurve()
//----------------------------------------------------------------------------
void StartCurve(CurveGroup i_Group, const maPoint3d &i_Pos)
{
	DBG_ASSERT0(!l_ActiveCurve, "Already an active curve");
	splnSpline::SplineType type = (i_Group == e_Fence) ? splnSpline::e_Linear : splnSpline::e_Catmull;
	l_ActiveCurve = new splnSpline(type);
	l_Curves[i_Group].push_back(l_ActiveCurve);

	maPoint3d pts[2];
	pts[0] = pts[1] = i_Pos;
	pts[1].m_Y += 0.1f;	// offset slightly to make line segment
	l_ActiveCurve->SetPoints(pts, 2);

	splnCurveSelect::CreateCurve(l_ActiveCurve);
}

//----------------------------------------------------------------------------
//	AlterEndPoint()
//----------------------------------------------------------------------------
void AlterEndPoint(const maPoint3d &i_Pos)
{
	DBG_ASSERT0(l_ActiveCurve, "No active curve");
	int num_pts = l_ActiveCurve->GetNumPoints();
	l_ActiveCurve->SetPointPos(num_pts - 1, i_Pos);

	splnCurveSelect::UpdateCurve(l_ActiveCurve);
}

//void AlterEndPointRelative(const maVector3d &i_Diff)
//{
//	DBG_ASSERT0(l_ActiveCurve, "No active curve");
//	int num_pts = l_ActiveCurve->GetNumPoints();
//	AlterEndPoint(l_ActiveCurve->GetPointPos(num_pts - 1) + i_Diff);
//}

//----------------------------------------------------------------------------
//	AddEndPoint()
//----------------------------------------------------------------------------
void AddEndPoint(const maPoint3d &i_Pos)
{
	DBG_ASSERT0(l_ActiveCurve, "No active curve");
	int num_pts = l_ActiveCurve->GetNumPoints();
	l_ActiveCurve->SetPointPos(num_pts - 1, i_Pos);
	l_ActiveCurve->AppendPoint(i_Pos);

	splnCurveSelect::UpdateCurve(l_ActiveCurve);

}

//----------------------------------------------------------------------------
//	FinishCurve()
//----------------------------------------------------------------------------
splnSpline* FinishCurve(bool i_bCloseCurve)
{
	if (!l_ActiveCurve) return NULL;
//	DBG_ASSERT0(l_ActiveCurve, "No active curve");

	splnSpline *curve = l_ActiveCurve;
	l_ActiveCurve = NULL;

	// Have been maintaining extra point for next
	// possible position, need to remove it.
	//
	if (curve->GetNumPoints() <= 2)
	{
		// Insufficient points, delete curve
		delete_curve(curve);
		return NULL;
	}
	else
	{
		curve->TruncatePoint();
	}

	if (i_bCloseCurve)
		curve->SetClosed(true);

	splnCurveSelect::UpdateCurve(curve);

	return curve;
}
*/

} // end of namespace