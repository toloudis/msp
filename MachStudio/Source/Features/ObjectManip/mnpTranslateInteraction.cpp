/*****************************************************************************
**  mnpTranslateInteraction.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Features/ObjectManip/mnpTranslateInteraction.hpp"

#include "Support/mnm/mnmObject.hpp"

#include "Core/Geo/geoRayIntersection.hpp"
#include "Core/undo/undoMultipleOperationBlock.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/sel3d/sel3dObject.hpp"

namespace
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
mnpTranslateInteraction::mnpTranslateInteraction(mnmObject* i_pSelectedObject)
: m_pSelectedObject(i_pSelectedObject)
{
	// Get transformation matrices between world and object space
	m_pSelectedObject->GetParentMatrix(m_ParentXform);
	m_InverseParentXform = m_ParentXform;
	m_InverseParentXform.Invert();

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
			{
				sObjectMatrix obj_matrix;
				obj_matrix.m_pObject = pObject;
				pObject->GetParentMatrix(obj_matrix.m_InverseParentXform);
				obj_matrix.m_InverseParentXform.Invert();
				m_AdditionalObjects.push_back(obj_matrix);
			}
		}
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
mnpTranslateInteraction::~mnpTranslateInteraction()
{
}

//------------------------------------------------------------------------
// Translate the main selected object to the given world position,
// computing and applying the delta translation to 
//------------------------------------------------------------------------
void mnpTranslateInteraction::TranslateToWorldPosition(const maPoint3d& i_NewWorldPos,
															 bool i_bNewOperation)
{
	bool bMultipleSelection = (!m_AdditionalObjects.empty());
	// Create multiple operation block if bMultipleSelection is true
	undoMultipleOperationBlock multiple_op_block("Translate", bMultipleSelection);

	if (bMultipleSelection)
	{
		// Extract out the frame delta movement for this object
		// in order to apply it to the other selected objects
		maPoint3d old_world_pos = m_ParentXform * m_pSelectedObject->GetPosition();
		maVector3d delta_pos = i_NewWorldPos - old_world_pos;
		apply_delta_position(delta_pos, i_bNewOperation);
	}

	// Set the new position
	maPoint3d new_object_pos = m_InverseParentXform * i_NewWorldPos;
	m_pSelectedObject->UpdatePosition( new_object_pos, i_bNewOperation );
}

//------------------------------------------------------------------------
// Translate objects in the selected list that aren't the focus
// of the translation compass.
//------------------------------------------------------------------------
void mnpTranslateInteraction::apply_delta_position(const maVector3d& i_Delta, bool i_bNewOperation)
{
	std::list<sObjectMatrix>::const_iterator it, end = m_AdditionalObjects.end();
	for (it = m_AdditionalObjects.begin(); it != end; ++it)
	{
		// Convert delta motion into object space and then apply it to
		// the additionally selected object
		mnmObject* pObject = it->m_pObject;
		maPoint3d object_delta(i_Delta);
		m_InverseParentXform.TransformDir(object_delta);
		pObject->UpdatePosition(pObject->GetPosition() + object_delta, i_bNewOperation);
	}
}
