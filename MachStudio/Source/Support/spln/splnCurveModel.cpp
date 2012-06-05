/*****************************************************************************
**  splnCurveModel.cpp
**
**      The splnCurveModel handles highlight of curves and
**	adds control points for altering curve shape.
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#include "Support/spln/splnCurveModel.hpp"
#include "Support/spln/splnCurvePointObject.hpp"
#include "Support/spln/splnSpline.hpp"

#include "MainApp/mnmApp.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoPickRay.hpp"
#include "Core/Ma/maFunctions.hpp"
#include "Core/rel/relRelationshipMultiple.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/gpx/gpxFragment.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gpx/gpxSceneObject.hpp"
#include "Tool/icn/icnIconLayer.hpp"
#include "Tool/icn/icnIconSet.hpp"
#include "Tool/pick3d/pick3dPickList.hpp"

namespace
{

void get_spline_eval_pts(const splnSpline &i_Curve, std::vector<maPoint3d> &o_Verts)
{
	bool linear = (i_Curve.GetSplineType() == splnSpline::e_Linear);

	const int c_PointsPerChord = 8;
	const float c_MinSegmentLength = 2.5f;

	//int num_verts = (i_Curve.GetNumPoints()-1) * c_PointsPerChord + 1;
	int num_verts = 1;
	if (linear)
	{
		num_verts = i_Curve.GetNumPoints();
	}
	else
	{
		for (int p=1; p<i_Curve.GetNumPoints(); p++)
		{
			float len  = (i_Curve.GetPointPos(p) - i_Curve.GetPointPos(p-1)).Length();
			int num_segments = (int)(len / c_MinSegmentLength);
			if (num_segments < c_PointsPerChord)
				num_segments = c_PointsPerChord;
			num_verts += num_segments;
		}
	}
	if (num_verts <= 1)
	{
		// Return empty list
		o_Verts.clear();
	}
	else
	{
		if (!linear)
		{		
			// Clamp the number of vertices to a power of two with a minimum value so that
			// it is more likely that the number of vertices will not change.
			if (num_verts <= 16) 
				num_verts = 16;
			else 			
				num_verts = 1 << (int)floor(maFunctions::Log((float)num_verts,2.0f)+0.5f);

		}

		// Return positions in given array.
		o_Verts.resize(num_verts);

		//int num_inds = (num_verts-1) * 2;
		//std::vector<unsigned short> indices(num_inds);

		for (int v=0; v<num_verts; v++)
		{
			if (linear)
				o_Verts[v] = i_Curve.GetPointPos(v);
			else
				o_Verts[v] = i_Curve.Evaluate( v / float(num_verts-1) );
		}
	}
}

api3dObjectSimple* make_line_frag(std::vector<maPoint3d> &i_Verts)
{
	if (i_Verts.empty()) return NULL;
	const bool bClosed = false;
	const bool bMorphable = true;
	return api3dShape::CreateLineList(	maFloatRGBA(0.0f, 0.3f, 0.0f, 1.0f),
										&(i_Verts[0]), 
										i_Verts.size(),
										bClosed, bMorphable);
}

api3dObjectSimple* make_line_frag(const splnSpline &i_Curve)
{
	std::vector<maPoint3d> verts;
	get_spline_eval_pts(i_Curve, verts);
	return make_line_frag(verts);
}
	

}	// end of namespace


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
splnCurveModel::splnCurveModel(splnSpline &i_Curve)
:	m_pLineObj(NULL),
	m_pLineObjProxy(NULL),
	m_pLineFragmentProxy(NULL),
	m_bVisible(true)
{
	api3dObjectSimple* pLineObjBase = make_line_frag(i_Curve);
	if (pLineObjBase)
	{
		m_pLineObj = icnIconLayer::CreateIconSet(pLineObjBase);
		m_pLineObjProxy = new gpxSceneObject(*m_pLineObj);
		m_pLineFragmentProxy = new gpxFragment(*(pLineObjBase->Fragment()));
	}

	// Create new multiple relationship connecting the control points to 
	// this model object
	m_CtrlPtsRelationship.reset(
		new relRelationshipMultiple<splnCurvePointObject>("ControlPoints", *this, m_Objects));
	this->AddRelationship(m_CtrlPtsRelationship);

	// Put spheres at control points
	for (int i=0; i<i_Curve.GetNumPoints(); i++)
	{
		splnCurvePointObject *obj = new splnCurvePointObject(&i_Curve, i);
		obj->SetParentRelationship(m_CtrlPtsRelationship);
		m_Objects.push_back(obj);
		obj->SetRenderable( m_bVisible );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
splnCurveModel::~splnCurveModel()
{
	// By making the spline invisible first, we eliminate a problem
	// where the selection change makes the spline invisible because the
	// selection does not contain the spline anymore.
	this->SetRenderable(false);

	envSTLHelpers::DeleteContainer(m_Objects);

	delete m_pLineFragmentProxy;
	delete m_pLineObjProxy;
	delete m_pLineObj;
}

//----------------------------------------------------------------------------
// UpdateCurve - attempt to use existing objects to represent
//	the given curve.  
//----------------------------------------------------------------------------
void splnCurveModel::UpdateCurve(splnSpline &i_Curve)
{
	// Generate evaluation points along spline.
	std::vector<maPoint3d> verts;
	get_spline_eval_pts(i_Curve, verts);

	// If same number of vertices in our fragment already, we can 
	// just change their positions
	if (verts.size() == (m_pLineFragmentProxy ? m_pLineFragmentProxy->GetNumVertices() : 0))
	{
		// Update vertices within existing spline
		if (m_pLineFragmentProxy)
			m_pLineFragmentProxy->UpdateVertices(verts.size(), &(verts[0]));

		// If m_pLineFragmentProxy is NULL, then no current line and no new verts, so nothing to do.
	}
	else
	{
		// Otherwise, remake line set completely

		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		delete m_pLineFragmentProxy;
		m_pLineFragmentProxy = NULL;
		delete m_pLineObjProxy;
		m_pLineObjProxy = NULL;
		delete m_pLineObj;
		m_pLineObj = NULL;
		api3dObjectSimple* pLineObjBase = make_line_frag(verts);
		if (pLineObjBase)
		{
			m_pLineObj = icnIconLayer::CreateIconSet(pLineObjBase);
			m_pLineObjProxy = new gpxSceneObject(*m_pLineObj);
			m_pLineFragmentProxy = new gpxFragment(*(pLineObjBase->Fragment()));
		}
	}

	// Update control points
	int num_existing_points = maFunctions::Lowest(i_Curve.GetNumPoints(), (int)m_Objects.size());
	for (int i=0; i<num_existing_points; i++)
	{
		m_Objects[i]->SetPosition(i_Curve.GetPointPos(i));
	}
	// Delete any extra points that we had left over
	for (int i=num_existing_points; i<m_Objects.size(); i++)
	{
		delete m_Objects[i];
	}
	m_Objects.resize(num_existing_points);

	// Create new control points if needed
	for (int i=num_existing_points; i<i_Curve.GetNumPoints(); i++)
	{
		splnCurvePointObject *obj = new splnCurvePointObject(&i_Curve, i);
		obj->SetParentRelationship(m_CtrlPtsRelationship);
		m_Objects.push_back(obj);
		obj->SetRenderable( m_bVisible );
	}
}

//----------------------------------------------------------------------------
//	GetControlPointObject()
//----------------------------------------------------------------------------
splnCurvePointObject* splnCurveModel::GetControlPointObject(int i_Index)
{
	return m_Objects[i_Index];
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
pick3dPickObject* splnCurveModel::MatchPickCode(envType::UInt32 i_PickCode)
{
	for (int i=0; i<m_Objects.size(); i++)
	{
		if (m_Objects[i]->MatchPickCode(i_PickCode))
		{
			 return m_Objects[i];
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
//	Render the curve or not
//----------------------------------------------------------------------------
void splnCurveModel::SetRenderable( bool i_bVisible )
{
	if (m_pLineObjProxy)
		m_pLineObjProxy->SetRenderable( i_bVisible );

	// Update control points
	for (int i=0; i<m_Objects.size(); i++)
	{
		m_Objects[i]->SetRenderable( i_bVisible );
	}

	m_bVisible = i_bVisible;
}

//----------------------------------------------------------------------------
//	return whether the curve is visible or not
//----------------------------------------------------------------------------
bool splnCurveModel::GetRenderable()
{
	return m_bVisible;
}

//----------------------------------------------------------------------------
// Get parent object of the spline in order for selection to trace from
//	this spline back to the controlled object.
//----------------------------------------------------------------------------
//sel3dObject* splnCurveModel::GetParentObject() const
//{
//	return m_pParent;
//}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void splnCurveModel::SetColor( maFloatRGBA& i_Color )
{
	if (m_pLineObjProxy)
		m_pLineObjProxy->SetColor(i_Color);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//void splnCurveModel::add_to_scene()
//{
//	api3dScene::AddObject(m_pLineObj, mnmApp::GetIconsLayerIndex());
//}
//void splnCurveModel::remove_from_scene()
//{
//	api3dScene::RemoveObject(m_pLineObj, mnmApp::GetIconsLayerIndex());
//}
