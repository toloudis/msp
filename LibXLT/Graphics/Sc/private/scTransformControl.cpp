/*****************************************************************************
**scTransformControl.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Graphics/sc/scTransformControl.hpp"

#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Core/ma/maConstants.hpp"


//--------------------------------------------------------------------
// Constructor - pass in node to control
//--------------------------------------------------------------------
scTransformControl::scTransformControl(g3dSceneNode *i_pControlNode)
:	m_pControlNode(i_pControlNode), 
	m_EulerAngles(0,0,0),
	m_Scale(1,1,1),
	m_bDirty(false)
{
}


//--------------------------------------------------------------------
//  Destructor
//--------------------------------------------------------------------
scTransformControl::~scTransformControl()
{
	// ControlNode is inserted into hierarchy, so it doesn't need to be
	// deleted, the scene graph will handle that.
}


//--------------------------------------------------------------------
// Rotation, expressed as quaternions
//--------------------------------------------------------------------
void scTransformControl::SetRotation(const maRotation& i_Rotation)
{
	if (!(m_Rotation == i_Rotation))
	{
		m_bDirty = true;

		// Update euler angles, converting to degrees
		i_Rotation.GetEuler(m_EulerAngles.m_X, m_EulerAngles.m_Y, m_EulerAngles.m_Z);
		m_EulerAngles *= maConstants::c_fRadToAngle;
	}

	m_Rotation = i_Rotation;
}
const maRotation& scTransformControl::GetRotation() const
{
	return m_Rotation;
}

//--------------------------------------------------------------------
//  Rotation value, stored as Euler angles (X,Y,Z) in degrees
//--------------------------------------------------------------------
void  scTransformControl::SetEulerAngles(const maVector3d &i_Angles)
{
	if (m_EulerAngles != i_Angles)
	{
		m_bDirty = true;

		// Update quaternion
		m_Rotation.SetEuler(i_Angles.m_X * maConstants::c_fAngleToRad, 
			i_Angles.m_Y * maConstants::c_fAngleToRad, 
			i_Angles.m_Z * maConstants::c_fAngleToRad);
	}

	m_EulerAngles = i_Angles;
}
const maVector3d &  scTransformControl::GetEulerAngles() const
{
	return m_EulerAngles;
}

//--------------------------------------------------------------------
// Translation
//--------------------------------------------------------------------
void scTransformControl::SetTranslation(const maVector3d& i_Translation)
{
	if (m_Translation != i_Translation)
		m_bDirty = true;

	m_Translation = i_Translation;
}
const maVector3d& scTransformControl::GetTranslation() const
{
	return m_Translation;
}


//--------------------------------------------------------------------
// Scale
//--------------------------------------------------------------------
void scTransformControl::SetScale(const maVector3d& i_Scale)
{
	if (m_Scale != i_Scale)
		m_bDirty = true;

	m_Scale = i_Scale;
}
const maVector3d& scTransformControl::GetScale() const
{
	return m_Scale;
}

//--------------------------------------------------------------------
//	Animate is called by the scObject after it has done its base
//		animation
//--------------------------------------------------------------------
void scTransformControl::Animate(float i_SimulationTime)
{
	maMatrix4x4 transform;
	transform.MakeScale( m_Scale.m_X, m_Scale.m_Y, m_Scale.m_Z );

	transform *= m_Rotation.GetMatrix();
	//transform = m_Rotation.GetMatrix();	// old way without scale

	if (m_pControlNode->GetOrientation())
	{
		transform *= *m_pControlNode->GetOrientation();
	}
	transform.TranslateBy(m_Translation.m_X, m_Translation.m_Y, m_Translation.m_Z);
	m_pControlNode->SetTransform( transform );
}

//--------------------------------------------------------------------
// CheckDirty - return true if the object needs to animate the model.
//		Clears the dirty bit so that if the time is different
//		it will be dirty next call.
//--------------------------------------------------------------------
//virtual 
bool scTransformControl::CheckDirty(float i_SimTime)
{
	bool dirty = m_bDirty;
	m_bDirty = false;
	return dirty;
}
