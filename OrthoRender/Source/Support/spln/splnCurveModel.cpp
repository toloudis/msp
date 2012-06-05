/*****************************************************************************
**  splnCurveModel.cpp
**
**      The splnCurveModel handles highlight of curves and
**	adds control points for altering curve shape.
**
**	Extra Large Technology
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#include "Support/spln/splnCurveModel.hpp"
#include "Support/spln/splnCurvePointObject.hpp"
#include "Support/spln/splnSpline.hpp"

#include "MainApp/mnmApp.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoPickRay.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/pick3d/pick3dPickList.hpp"

namespace
{

api3dObject* make_line_frag(const splnSpline &i_Curve)
{
	bool linear = (i_Curve.GetSplineType() == splnSpline::e_Linear);

	const int c_PointsPerChord = 4;
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
	if (num_verts <= 1) return NULL;

	std::vector<maPoint3d> verts(num_verts);

	int num_inds = (num_verts-1) * 2;
	std::vector<unsigned short> indices(num_inds);

	for (int v=0; v<num_verts; v++)
	{
		if (linear)
			verts[v] = i_Curve.GetPointPos(v);
		else
			verts[v] = i_Curve.Evaluate( v / float(num_verts-1) );
	}

	return api3dShape::CreateLineList(	maFloatRGBA(0.0f, 0.3f, 0.0f, 1.0f),
										&(verts[0]), 
										verts.size());
}

}	// end of namespace


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
splnCurveModel::splnCurveModel(splnSpline &i_Curve, 
							   pick3dPickObject* i_pParent)
:	m_pLineObj(NULL),
	m_bVisible(true),
	m_pParent(i_pParent)
{
	m_pLineObj = make_line_frag(i_Curve);
	if ( m_bVisible && m_pLineObj ) api3dScene::AddObject(m_pLineObj, mnmApp::GetIconsLayerIndex());

	// Put spheres at control points
	for (int i=0; i<i_Curve.GetNumPoints(); i++)
	{
		splnCurvePointObject *obj = new splnCurvePointObject(&i_Curve, i, i_pParent);
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

	if ( m_bVisible )
	{
		if (m_pLineObj)	api3dScene::RemoveObject(m_pLineObj, mnmApp::GetIconsLayerIndex());
	}
	delete m_pLineObj;
}

//----------------------------------------------------------------------------
// UpdateCurve - attempt to use existing objects to represent
//	the given curve.  If the number of control points are
//	different, then false is returned.
//----------------------------------------------------------------------------
bool splnCurveModel::UpdateCurve(splnSpline &i_Curve)
{
	if (i_Curve.GetNumPoints() != m_Objects.size())
		return false;

	// Remake line set
	if ( m_bVisible )
	{
		if (m_pLineObj)	api3dScene::RemoveObject(m_pLineObj, mnmApp::GetIconsLayerIndex());
	}

	delete m_pLineObj;
	m_pLineObj = make_line_frag(i_Curve);

	if ( m_bVisible )
	{
		if (m_pLineObj)	api3dScene::AddObject(m_pLineObj, mnmApp::GetIconsLayerIndex());
	}

	// Update control points
	for (int i=0; i<i_Curve.GetNumPoints(); i++)
	{
		m_Objects[i]->SetPosition(i_Curve.GetPointPos(i));
	}

	return true;
}

//----------------------------------------------------------------------------
//	GetControlPointObject()
//----------------------------------------------------------------------------
splnCurvePointObject* splnCurveModel::GetControlPointObject(int i_Index)
{
	return m_Objects[i_Index];
}

//----------------------------------------------------------------------------
// Do ray pick on control point objects
//----------------------------------------------------------------------------
bool splnCurveModel::ObjectPick(geoPickRay& i_Ray, pick3dPickList& io_PickList)
{
	bool found = false;
	for (int i=0; i<m_Objects.size(); i++)
	{
		float tval = 0.0f;
		if (m_Objects[i]->RayPick(i_Ray.GetRayStart(), i_Ray.GetRayEnd(), tval))
		{
			io_PickList.AddItem(m_Objects[i], tval);
			found = true;
		}
	}

	return found;
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
	if ( i_bVisible )
	{
		if ( !m_bVisible )
		{
			if (m_pLineObj)
			{
				api3dScene::AddObject(m_pLineObj, mnmApp::GetIconsLayerIndex());
			}

			// Update control points
			for (int i=0; i<m_Objects.size(); i++)
			{
				m_Objects[i]->SetRenderable( i_bVisible );
			}
		}
	}
	else
	{
		if ( m_bVisible )
		{
			if (m_pLineObj)
			{
				api3dScene::RemoveObject(m_pLineObj, mnmApp::GetIconsLayerIndex());
			}

			// Update control points
			for (int i=0; i< m_Objects.size(); i++)
			{
				m_Objects[i]->SetRenderable( i_bVisible );
			}
		}
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
pick3dPickObject* splnCurveModel::GetParentObject() const
{
	return m_pParent;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void splnCurveModel::SetColor( maFloatRGBA& i_Color )
{
	if (m_pLineObj)
		m_pLineObj->SetColor(i_Color);
}
