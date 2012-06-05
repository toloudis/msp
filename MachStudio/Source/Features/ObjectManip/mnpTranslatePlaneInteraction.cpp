/*****************************************************************************
**  mnpTranslatePlaneInteraction.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Features/ObjectManip/mnpTranslatePlaneInteraction.hpp"

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
// Begin interaction for the given selected object along the axis
//	defined by the mouse down point and the projection axis given.
//------------------------------------------------------------------------
mnpTranslatePlaneInteraction::mnpTranslatePlaneInteraction(
									mnmObject* i_pSelectedObject,
									const maPoint3d &i_MouseDownPoint,
									const maVector3d &i_PlaneNormal)
:	mnpTranslateInteraction(i_pSelectedObject),
	m_bNewOperation(true),
	m_MouseDownPoint(i_MouseDownPoint),
	m_PlaneNormal(i_PlaneNormal)
{
	// Get original position of object in world space
	m_OriginalPosition	= m_pSelectedObject->GetPosition();
	m_ParentXform.Transform(m_OriginalPosition);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
mnpTranslatePlaneInteraction::~mnpTranslatePlaneInteraction()
{
}


//------------------------------------------------------------------------
// Abort the mouse interaction, return the objects back to their
//	original positions.
//------------------------------------------------------------------------
void mnpTranslatePlaneInteraction::AbortInteraction()
{
	mnpInteraction::AbortInteraction();
	
	const bool new_operation = false;
	this->TranslateToWorldPosition(m_OriginalPosition, new_operation);
}

//------------------------------------------------------------------------
// Handle mouse motion. Mouse position is given in terms of a 
// position on the camera near plane and the direction of the
// ray that passes through that point into the camera view frustrum.
//------------------------------------------------------------------------
void mnpTranslatePlaneInteraction::RayMotion(const maPoint3d& i_RayPos, 
											  const maVector3d& i_RayDir)
{
	// Calculate the change in position
	maPoint3d intersect_point = m_MouseDownPoint;
	float ray_t = 0;
	if (plane_intersection(i_RayPos, i_RayDir,
						   m_MouseDownPoint, m_PlaneNormal,
						   intersect_point, ray_t))
	{
		if (ray_t > 0 && ray_t < 1) // Keep projection within camera frustrum
		{
			maVector3d diff = intersect_point - m_MouseDownPoint;
			maVector3d new_world_position = m_OriginalPosition + diff;
		
			// Need undo only on first motion of the objects
			bool bNewOperation = m_bNewOperation;
			m_bNewOperation = false;

			// Translate all selected objects so that main selected object is at
			// the given new world position.
			this->TranslateToWorldPosition(new_world_position, bNewOperation);
		}
	}
}

