/*****************************************************************************
**  mnpScaleInteraction.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Features/ObjectManip/mnpScaleInteraction.hpp"

#include "Support/mnm/mnmObject.hpp"

#include "Core/Geo/geoRayIntersection.hpp"
#include "Core/undo/undoMultipleOperationBlock.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/sel3d/sel3dObject.hpp"

namespace
{
	//--------------------------------------------------------------------
	// plane_intersection checks to see if the cursor ray intersects with
	// the plane defined by a point and a normal.
	//--------------------------------------------------------------------
	bool plane_intersection(const maPoint3d& i_CameraPoint,
							 const maPoint3d& i_CameraRay,
							 const maPoint3d& i_PlanePoint,
							 const maPoint3d& i_PlaneNormal,
							 maPoint3d& o_PickPos,
							 float& o_fT)
	{
		float attempt_t = 2.0f;

		bool intersect = geoRayIntersection::ProjectLineToPlane(i_CameraPoint,
																i_CameraRay,
																i_PlanePoint,
																i_PlaneNormal,
																attempt_t);

		if( intersect )
		{
			o_PickPos = i_CameraPoint + i_CameraRay * attempt_t;
			o_fT = attempt_t;
		}

		return intersect;
	}
}

//------------------------------------------------------------------------
// Begin interaction for the given selected object based on 
//	the mouse down point.
//------------------------------------------------------------------------
mnpScaleInteraction::mnpScaleInteraction(mnmObject* i_pSelectedObject,
										   const maPoint3d &i_MouseDownPoint,
										   const maVector3d &i_ProjectionAxis,
										   bool i_bFlipDirection)
:	m_bNewOperation(true),
	m_pSelectedObject(i_pSelectedObject),
	m_MouseDownPoint(i_MouseDownPoint),
	m_ProjectionAxis(i_ProjectionAxis),
	m_bFlipDirection(i_bFlipDirection)
{
	m_OriginalScale = m_pSelectedObject->GetScale();

	// Gather up the additional objects we are also manipulating from the
	// selection list. But do not include the m_pSelectedObject.
	m_AdditionalObjects.clear();
	const std::list<sel3dObject*> &selected_list = sel3dMgr::GetSelectedList();
	std::list<sel3dObject*>::const_iterator it, end = selected_list.end();
	for (it = selected_list.begin(); it != end; ++it)
	{
		if (mnmObject* pObject = dynamic_cast<mnmObject*>(*it))
		{
			// skip first one, it is receiving the translation from the compass
			if (pObject != m_pSelectedObject)
				m_AdditionalObjects.push_back(pObject);
		}
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
mnpScaleInteraction::~mnpScaleInteraction()
{
}

//------------------------------------------------------------------------
// Abort the mouse interaction, return the objects back to their
//	original positions.
//------------------------------------------------------------------------
void mnpScaleInteraction::AbortInteraction()
{
	mnpInteraction::AbortInteraction();

	// Create multiple operation block if multiple selection is true
	undoMultipleOperationBlock multiple_op_block("Scale", !m_AdditionalObjects.empty());

	// Extract out the frame delta movement for this object
	// in order to apply it to the other selected objects
	float delta_scale = 1.0f;
	if (m_pSelectedObject->GetScale().m_X > 0)
		delta_scale = m_OriginalScale.m_X / m_pSelectedObject->GetScale().m_X;
	const bool new_operation = false;
	apply_delta_scale(delta_scale, new_operation);

	m_pSelectedObject->UpdateScale( m_OriginalScale, new_operation );
	//cmpsCompassMgr::SetScale( cmpsCompassMgr::e_Scale, m_OriginalScale  );
}

//------------------------------------------------------------------------
// Handle mouse motion. Mouse position is given in terms of a 
// position on the camera near plane and the direction of the
// ray that passes through that point into the camera view frustrum.
//------------------------------------------------------------------------
void mnpScaleInteraction::RayMotion(const maPoint3d& i_RayPos, 
									  const maVector3d& i_RayDir)
{
	// Get the normal of a plane passing through axis and roughly
	// perpindicular to camera plane
	maVector3d proj_normal = -i_RayDir;

	//bga - should the scale interaction also have this code from 
	// the mnpTranslationInteraction class?
	//maVector3d perp_vec = proj_normal.Cross(m_ProjectionAxis);
	//if (perp_vec.Normalize())
	//	proj_normal = m_ProjectionAxis.Cross(perp_vec);

	// Calculate the change in position
	maPoint3d intersect_point = m_MouseDownPoint;
	float ray_t = 0;
	if (plane_intersection(i_RayPos, i_RayDir,
						   m_MouseDownPoint, proj_normal,
						   intersect_point, ray_t))
	{
		// Scale is measured by distance to the origin of the object
		float orig_dist = m_ProjectionAxis * (m_MouseDownPoint - m_pSelectedObject->GetWorldPivot());
		float cur_dist = m_ProjectionAxis * (intersect_point - m_pSelectedObject->GetWorldPivot());
		float scale_delta;
		
		//scale delta value depends on which direction the compass is facing
		if (m_bFlipDirection)
			scale_delta = (orig_dist < 0) ? cur_dist / orig_dist : 1.0f;
		else
			scale_delta = (orig_dist > 0) ? cur_dist / orig_dist : 1.0f;

		const float c_fMinScale = 0.05f;
		if (scale_delta < c_fMinScale)
			scale_delta = c_fMinScale;

		// This is uniform scale, should handle non-uniform also?
		maPoint3d scaleaxis = m_OriginalScale * scale_delta;

		// Need undo only on first motion of the objects
		bool bNewOperation = m_bNewOperation;
		m_bNewOperation = false;

		// Create multiple operation block if multiple selection is true
		undoMultipleOperationBlock multiple_op_block("Scale", !m_AdditionalObjects.empty());

		// Extract out the frame delta movement for this object
		// in order to apply it to the other selected objects
		float delta_scale = 1.0f;
		if (m_pSelectedObject->GetScale().m_X > 0)
			delta_scale = scaleaxis.m_X / m_pSelectedObject->GetScale().m_X;
		apply_delta_scale(delta_scale, bNewOperation);

		// now update the scale on the main object
		m_pSelectedObject->UpdateScale( scaleaxis, bNewOperation );
	}
}

//--------------------------------------------------------------------
// Scale objects in the selected list that aren't the focus
// of the scale compass.
//--------------------------------------------------------------------
void mnpScaleInteraction::apply_delta_scale(float i_Delta, bool i_bNewOperation)
{
	std::list<mnmObject*>::const_iterator it, end = m_AdditionalObjects.end();
	for (it = m_AdditionalObjects.begin(); it != end; ++it)
	{
		mnmObject* pObject = (*it);
		pObject->UpdateScale(pObject->GetScale() * i_Delta, i_bNewOperation);
	}
}
