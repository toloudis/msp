/*****************************************************************************
**	cmmSplineOperations.cpp
**
**		see .hpp
**
**	StudioGPU
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
		// Identifier of a spline and a point index in order to keep track
		// of multiple control point deletion
		struct sSplinePointId
		{
			sSplinePointId(splnSpline* i_pSpline, int i_PointIndex)
				:m_pSpline(i_pSpline), m_PointIndex(i_PointIndex) { }

			splnSpline* m_pSpline;
			int m_PointIndex;
		};

		bool high_index_sort(const sSplinePointId& i_A, const sSplinePointId& i_B)
		{
			// Could also sort on the spline pointers here to group by spline, but 
			// that actually isn't important. It is just important that within each
			// spline, the highest index is deleted first so that the lower indices
			// don't need to be readjusted.
			return (i_A.m_PointIndex > i_B.m_PointIndex);
		}

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
		// Gather a list of spline point ids from the selection
		std::list<sSplinePointId> point_id_list;
		const std::list<sel3dObject*> selected_list = sel3dMgr::GetSelectedList();
		std::list<sel3dObject*>::const_iterator sit;
		for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
		{
			if ( splnCurvePointObject *pPoint = sel3dCastUtil::CastPickObject<splnCurvePointObject>(*sit) )
			{
				sSplinePointId point_id(pPoint->GetCurve(), pPoint->GetPointIndex());
				point_id_list.push_back(point_id);
			}
		}

		if (!point_id_list.empty())
		{
			// Sort list so that we delete the high indices first 
			// (if we deleted the low indices first, the high indices would have to be adjusted lower)
			point_id_list.sort(high_index_sort);

			// Clear the selection while we delete the points
			sel3dMgr::ClearSelection();

			// Delete each control point in the list
			std::list<sSplinePointId>::iterator pit;
			splnSpline* pSpline = NULL;
			int sel_ind = 0;
			for (pit = point_id_list.begin(); pit != point_id_list.end(); ++pit)
			{
				 //	delete the spline point
				pSpline = pit->m_pSpline;
				int num_points = pSpline->GetNumPoints();
				// Can't delete the last point of a spline, always has to be one.
				if (num_points > 1)
				{
					sel_ind = pit->m_PointIndex;
					splnCurveMgr::DeleteCtrlPoint(pSpline, sel_ind);
				}
			}

			// select some point that still remains from one of the splines we were using
			if (pSpline != NULL)
			{
				int num_points = pSpline->GetNumPoints();
				if (num_points > 0)
				{
					int new_sel_ind = (sel_ind % num_points);
					sel3dMgr::Select(splnCurveSelect::GetControlPointObject(pSpline, new_sel_ind));
				}
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
