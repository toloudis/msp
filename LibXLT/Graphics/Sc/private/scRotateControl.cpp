/*****************************************************************************
**	scRotateControl.hpp
**
**		scRotateControl defines a base class for animations that give
**	programmatic control over nodes in an object.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Graphics/sc/scRotateControl.hpp"

#include "Graphics/g3d/g3dSceneNode.hpp"


//--------------------------------------------------------------------
// Constructor - pass in node to control
//--------------------------------------------------------------------
scRotateControl::scRotateControl(g3dSceneNode *i_pControlNode)
:	m_pControlNode(i_pControlNode), m_bDirty(false)
{
}

//--------------------------------------------------------------------
//  Destructor
//--------------------------------------------------------------------
scRotateControl::~scRotateControl()
{
	// ControlNode is inserted into hierarchy, so it doesn't need to be
	// deleted, the scene graph will handle that.
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void scRotateControl::SetRotation(const maRotation& i_Rotation)
{
	if (!(m_Rotation == i_Rotation))
		m_bDirty = true;

	m_Rotation = i_Rotation;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const maRotation& scRotateControl::GetRotation() const
{
	return m_Rotation;
}

//--------------------------------------------------------------------
//	Animate is called by the scObject after it has done its base
//		animation
//--------------------------------------------------------------------
void scRotateControl::Animate(float i_SimulationTime)
{
	m_pControlNode->SetTransform( m_Rotation.GetMatrix() );
}

//--------------------------------------------------------------------
// CheckDirty - return true if the object needs to animate the model.
//		Clears the dirty bit so that if the time is different
//		it will be dirty next call.
//--------------------------------------------------------------------
//virtual 
bool scRotateControl::CheckDirty(float i_SimTime)
{
	bool dirty = m_bDirty;
	m_bDirty = false;
	return dirty;
}
