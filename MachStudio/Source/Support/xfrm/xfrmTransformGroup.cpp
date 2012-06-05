/*****************************************************************************
**	xfrmTransformGroup.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support/xfrm/xfrmTransformGroup.hpp"


namespace
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xfrmTransformGroup::xfrmTransformGroup(nameObject* i_pNameObj, 
									   g3dSceneNode* i_pSceneNode, 
									   xfrmTransformNodeCallback* i_pCallback)
:	xfrmTransformNode(i_pNameObj, i_pSceneNode, i_pCallback),
	m_bInheritsTransform(true),
	m_bTotalMatrixDirty(true),
	m_bPickable(true),
	m_bTotalPickable(true),
	m_bPickableDirty(true) 
{
}


//--------------------------------------------------------------------
// Set whether this node inherits transformation from parent node
//--------------------------------------------------------------------
void xfrmTransformGroup::SetInheritsTransform(bool i_bInherit)
{
	if (i_bInherit != m_bInheritsTransform)
	{
		m_bInheritsTransform = i_bInherit;
		m_bTotalMatrixDirty = true;

		// Tell all children that their total transformation has changed
		int num_kids = m_ChildNodes.size();
		for (int k=0; k<num_kids; ++k)
		{
			m_ChildNodes[k]->ParentTransformChanged();
		}
	}
}

//--------------------------------------------------------------------
// Current transformation for this node only
//--------------------------------------------------------------------
void xfrmTransformGroup::SetTransformation(const maMatrix4x4& i_Matrix)
{
	m_Matrix = i_Matrix;
	m_bTotalMatrixDirty = true;

	// Tell all children that their total transformation has changed
	int num_kids = m_ChildNodes.size();
	for (int k=0; k<num_kids; ++k)
	{
		m_ChildNodes[k]->ParentTransformChanged();
	}
}

//--------------------------------------------------------------------
// Current transformation for this node plus parent matrices
//--------------------------------------------------------------------
const maMatrix4x4& xfrmTransformGroup::GetTotalTransformation() const
{
	if (m_bTotalMatrixDirty)
	{
		if (m_bInheritsTransform && (m_pParentGroup != NULL))
		{
			// Compute total transformation by combining with parent's total transformation
			m_TotalMatrix = m_Matrix * m_pParentGroup->GetTotalTransformation();

		}
		else
		{
			m_TotalMatrix = m_Matrix;
		}
		m_bTotalMatrixDirty = false;
	}
	return m_TotalMatrix;
}

//--------------------------------------------------------------------
// Get pickable state for this node considering the pickable
// state of all parent nodes.
//--------------------------------------------------------------------
bool xfrmTransformGroup::GetPickable() const
{	
	if (m_bPickableDirty)
	{
		if (m_pParentGroup)
		{
			// Compute total pickable state by combining with parent's pickable state
			m_bTotalPickable = (m_bPickable && m_pParentGroup->GetPickable());
		}
		else
		{
			m_bTotalPickable = m_bPickable;
		}
		m_bPickableDirty = false;
	}
	return m_bTotalPickable;

}

//--------------------------------------------------------------------
// Set the pickable state for this parent node, this will 
//	affect all children
//--------------------------------------------------------------------
void xfrmTransformGroup::SetPickable(bool i_bPickable)
{
	m_bPickable = i_bPickable;
	m_bPickableDirty = true;

	// Tell all children that their pickable state has changed
	int num_kids = m_ChildNodes.size();
	for (int k=0; k<num_kids; ++k)
	{
		m_ChildNodes[k]->ParentPickableChanged();
	}
}

//--------------------------------------------------------------------
// Notification that a parent transform node has changed its transformation
//--------------------------------------------------------------------
void xfrmTransformGroup::ParentTransformChanged()
{
	m_bTotalMatrixDirty = true;

	// Also tell all children that their total transformation has changed
	int num_kids = m_ChildNodes.size();
	for (int k=0; k<num_kids; ++k)
	{
		m_ChildNodes[k]->ParentTransformChanged();
	}
}

//--------------------------------------------------------------------
// Notification that a parent transform node has changed its pickable state
//--------------------------------------------------------------------
void xfrmTransformGroup::ParentPickableChanged()
{
	m_bPickableDirty = true;

	// Also tell all children that their pickable state has changed
	int num_kids = m_ChildNodes.size();
	for (int k=0; k<num_kids; ++k)
	{
		m_ChildNodes[k]->ParentPickableChanged();
	}
}

