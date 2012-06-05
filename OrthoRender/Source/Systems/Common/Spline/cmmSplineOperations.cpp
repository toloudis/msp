/*****************************************************************************
**	cmmSplineOperations.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/Spline/cmmSplineOperations.hpp"

#include "Support/spln/splnSpline.hpp"
#include "Support/spln/splnCurveMgr.hpp"
#include "Support/spln/splnCurvePointObject.hpp"
#include "Support/spln/splnCurveSelect.hpp"

#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"

//============================================================================
//============================================================================
namespace cmmSplineOperations
{
	namespace
	{
	}	// end of namespace

	//--------------------------------------------------------------------
	//  Select previous and next control points in selected spline
	//--------------------------------------------------------------------
	void  SelectPreviousControlPoint()
	{
		if ( splnCurvePointObject *pPoint = sel3dCastUtil::CastSelectedObject<splnCurvePointObject>() )
		{
			splnSpline* pSpline = pPoint->GetCurve();
			int num_points = pSpline->GetNumPoints();
			int sel_ind = (pPoint->GetPointIndex() + num_points - 1) % num_points;

			sel3dMgr::CreateUndoOperation();
			sel3dMgr::Select(splnCurveSelect::GetControlPointObject(pSpline, sel_ind));
		}
	}
	void  SelectNextControlPoint()
	{
		if ( splnCurvePointObject *pPoint = sel3dCastUtil::CastSelectedObject<splnCurvePointObject>() )
		{
			splnSpline* pSpline = pPoint->GetCurve();
			int num_points = pSpline->GetNumPoints();
			int sel_ind = (pPoint->GetPointIndex() + 1) % num_points;

			sel3dMgr::CreateUndoOperation();
			sel3dMgr::Select(splnCurveSelect::GetControlPointObject(pSpline, sel_ind));
		}
	}

	//--------------------------------------------------------------------
	//  Select all control points in selected spline
	//--------------------------------------------------------------------
	void  SelectAllControlPoints()
	{
		if ( splnCurvePointObject *pPoint = sel3dCastUtil::CastSelectedObject<splnCurvePointObject>() )
		{
			splnSpline* pSpline = pPoint->GetCurve();
			int num_points = pSpline->GetNumPoints();
			if (num_points > 0)
			{
				sel3dMgr::CreateUndoOperation();
				sel3dMgr::Select(splnCurveSelect::GetControlPointObject(pSpline, 0));
				for (int i=1; i<num_points; ++i)
					sel3dMgr::AddToSelection(splnCurveSelect::GetControlPointObject(pSpline, i));
			}
		}
	}

	//--------------------------------------------------------------------
	//  Insert control point after selection
	//--------------------------------------------------------------------
	void  InsertControlPoint()
	{
		if ( splnCurvePointObject *pPoint = sel3dCastUtil::CastSelectedObject<splnCurvePointObject>() )
		{
			splnSpline* pSpline = pPoint->GetCurve();
			splnCurveMgr::InsertCtrlPoint(pSpline, pPoint->GetPointIndex());
		}
	}

	//--------------------------------------------------------------------
	// Delete selected control points
	//--------------------------------------------------------------------
	void  DeleteControlPoint()
	{
		if ( splnCurvePointObject *pPoint = sel3dCastUtil::CastSelectedObject<splnCurvePointObject>() )
		{
			 //	delete the spline point
			splnSpline* pSpline = pPoint->GetCurve();
			int num_points = pSpline->GetNumPoints();
			int sel_ind = pPoint->GetPointIndex();
			splnCurveMgr::DeleteCtrlPoint(pSpline, sel_ind);

			// select the next point (if there is one)
			num_points = pSpline->GetNumPoints();
			if (num_points > 0)
			{
				sel_ind = (sel_ind % num_points);
				sel3dMgr::Select(splnCurveSelect::GetControlPointObject(pSpline, sel_ind));
			}
		}
	}

	//--------------------------------------------------------------------
	// Association between editor camera and selected control point
	//--------------------------------------------------------------------
	void  SetEditCamToPoint()
	{
		if ( splnCurvePointObject *pPoint = sel3dCastUtil::CastSelectedObject<splnCurvePointObject>() )
		{
			camCamera& cam = cam3dMgr::GetEditorCamera();
			cam3dMgr::SetManipPosition( pPoint->GetPosition() );
		}
	}
	void  SetPointToEditCam()
	{
		if ( splnCurvePointObject *pPoint = sel3dCastUtil::CastSelectedObject<splnCurvePointObject>() )
		{
			camCamera& cam = cam3dMgr::GetEditorCamera();
			pPoint->UpdatePosition( cam.GetPosition() );
			//splnCurveMgr::AlterCtrlPoint(pSpline, sel_ind, cam.GetPosition());
		}
	}

}	// end of namespace
