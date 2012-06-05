/*****************************************************************************
**  mnpRotateInteraction.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Features/ObjectManip/mnpRotateInteraction.hpp"

#include "Support/mnm/mnmObject.hpp"

#include "Core/Geo/geoRayIntersection.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Core/undo/undoMultipleOperationBlock.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/sel3d/sel3dObject.hpp"

namespace
{
	const float lc_fRotateIncrement = maConstants::c_fPI / 200.0f;

	//----------------------------------------------------------------------------
	//	Determine the point to rotate around considering the selected list
	//----------------------------------------------------------------------------
	maPoint3d compute_pivot_point()
	{	
		const std::list<sel3dObject*> &selected_list = sel3dMgr::GetSelectedList();

		int count = 0;
		maAxisBox combined_box;
		mnmObject *pFirstObject = NULL;
		std::list<sel3dObject*>::const_iterator it, end = selected_list.end();
		for (it = selected_list.begin(); it != end; ++it)
		{
			if (mnmObject* pObject = dynamic_cast<mnmObject*>(*it))
			{
				if (!pFirstObject) pFirstObject = pObject; // record the top selection object

				combined_box.Union( pObject->GetWorldBox() );
				count++;
			}
		}

		if (count > 1)
		{
			// Pivot point is center of combined box
			return combined_box.GetCenter();
		}
		else if ((count == 1) && (pFirstObject != NULL))
		{
			// If only one object, use its pivot point
			return pFirstObject->GetWorldPivot();
		}
		return maPoint3d(0,0,0);
	}
	
	//------------------------------------------------------------------------
	//	get_rotation_from_direction returns the rotation from the direction
	//------------------------------------------------------------------------
	maRotation get_rotation_from_direction( const maVector3d& i_RotationAxis,
											float i_fDirection )
	{
		// keep the direction between 0 an 2*PI
		while ( i_fDirection > maConstants::c_fPI_Times_2 )
		{
			i_fDirection -= maConstants::c_fPI_Times_2;
		}

		while ( i_fDirection < 0 )
		{
			i_fDirection += maConstants::c_fPI_Times_2;
		}

		return maRotation( i_RotationAxis, i_fDirection );
	}

	//--------------------------------------------------------------------
	// apply a delta rotation around the given pivot point to the 
	//	object passed in
	//--------------------------------------------------------------------
	void apply_delta_rotation_to_object(mnmObject* i_pObject,
									  const maRotation& i_Delta, 
									  const maPoint3d &i_Pivot, 
									  bool i_bNewOperation)
	{
		// Apply the delta rotation
		i_pObject->UpdateOrientation( i_Delta * i_pObject->GetOrientation(), i_bNewOperation );
		
		maVector3d pivot_delta = i_pObject->GetWorldPivot()- i_Pivot;
		if (pivot_delta != maVector3d(0,0,0))
		{
			maVector3d xformed_delta(pivot_delta); 
			i_Delta.RotateVector(xformed_delta);

			maVector3d compensation = xformed_delta - pivot_delta;
			i_pObject->UpdatePosition( compensation + i_pObject->GetPosition(), i_bNewOperation );
		}
	}
}

//------------------------------------------------------------------------
// Begin interaction for the given selected object along the axis
//	defined by the mouse down point and the projection axis given.
//------------------------------------------------------------------------
mnpRotateInteraction::mnpRotateInteraction(mnmObject* i_pSelectedObject,
											int i_X, int i_Y,
											const maVector3d &i_RotationAxis)
:	m_bNewOperation(true),
	m_pSelectedObject(i_pSelectedObject),
	m_StartRotationValue(i_X),
	m_RotationAxis(i_RotationAxis)
{
	m_OriginalOrientation = m_pSelectedObject->GetOrientation();

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
	
	// set the pivot point based on the selection list
	m_PivotPoint = compute_pivot_point();

	//cmpsCompassMgr::SetOrientation( cmpsCompassMgr::e_Rotate, m_OriginalOrientation );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
mnpRotateInteraction::~mnpRotateInteraction()
{
}

//------------------------------------------------------------------------
// Handle mouse motion. Rotation only wants the cursor position,
//	could be changed to project to a 3D circle.
//------------------------------------------------------------------------
void mnpRotateInteraction::MouseMotion(int i_X, int i_Y, g3dViewer *i_pViewer)
{
	if (this->IsInteracting())
	{
		// Get the difference between the two points
		maRotation rot = get_rotation_from_direction( m_RotationAxis,
				float(i_X - m_StartRotationValue) * lc_fRotateIncrement );

		//sprintf(text, "start rot(%6.3f), curpos( %6.3f)", m_StartRotationValue, cur_pos );
		//mnmDebugInfo::SetDebugInfo(20, text);

		m_StartRotationValue = i_X;

		// Create multiple operation block if multiple selection is true
		undoMultipleOperationBlock multiple_op_block("Rotate", !m_AdditionalObjects.empty());

		// Need undo only on first motion of the objects
		bool bNewOperation = m_bNewOperation;
		m_bNewOperation = false;

		// Apply delta rotation to all other selected objects
		apply_delta_rotation(rot, m_PivotPoint, bNewOperation);

		// Apply delta rotation to main selected object
		apply_delta_rotation_to_object(m_pSelectedObject, rot, m_PivotPoint, bNewOperation);

		maRotation new_rot = rot * m_pSelectedObject->GetOrientation();
		//cmpsCompassMgr::SetOrientation( cmpsCompassMgr::e_Rotate, new_rot  );
	}
}

//------------------------------------------------------------------------
// Abort the mouse interaction, return the objects back to their
//	original positions.
//------------------------------------------------------------------------
void mnpRotateInteraction::AbortInteraction()
{ 
	mnpInteraction::AbortInteraction();

	// Create multiple operation block if multiple selection is true
	undoMultipleOperationBlock multiple_op_block("Rotate", !m_AdditionalObjects.empty());

	//FIX - If multiple selection, then need to reset orientation of
	// other objects in selection.

	// User aborted moving
	m_pSelectedObject->UpdateOrientation( m_OriginalOrientation );
	//cmpsCompassMgr::SetOrientation( cmpsCompassMgr::e_Rotate, m_OriginalOrientation  );

}

//--------------------------------------------------------------------
// Rotate objects in the selected list that aren't the focus
// of the orientation compass.
//--------------------------------------------------------------------
void mnpRotateInteraction::apply_delta_rotation(const maRotation& i_Delta, 
						  const maPoint3d &i_Pivot, 
						  bool i_bNewOperation)
{

	std::list<mnmObject*>::const_iterator it, end = m_AdditionalObjects.end();
	for (it = m_AdditionalObjects.begin(); it != end; ++it)
	{
		mnmObject* pObject = (*it);

		// this line rotates without pivot
		//pObject->UpdateOrientation( i_Delta * pObject->GetOrientation(), i_bNewOperation );

		// this line applies rotation and translation based on pivot position
		apply_delta_rotation_to_object(pObject, i_Delta, i_Pivot, i_bNewOperation);
	}
}