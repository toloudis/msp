/*****************************************************************************
**	tmlnAdapterReference.cpp
**
**	 Adapter for getting target position from attachment reference to object
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "Drivers/Attach/tmlnAdapterReference.hpp"

#include "Tool/api3d/api3dObject.hpp"
#include "Tool/api3d/api3dReference.hpp"

#include "Core/dbg/dbgLog.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnAdapterReference::tmlnAdapterReference()
: m_pReference(NULL)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnAdapterReference::~tmlnAdapterReference()
{
	// if we are deleted first, remove auditor from the reference
	if (m_pReference)
	{
		m_pReference->RemoveAuditor(this);
	}
}

//--------------------------------------------------------------------
// Attach to given object at given named reference point
//--------------------------------------------------------------------
void tmlnAdapterReference::AttachTo(api3dReference *i_pReference)
{
	if (m_pReference != i_pReference)
	{
		// Remove old auditor
		if (m_pReference)
		{
			m_pReference->RemoveAuditor(this);
		}

		m_pReference = i_pReference;

		if (m_pReference)
		{
			// Add as auditor, so we know if the reference 
			// (meaning the parent object) is deleted.
			m_pReference->AddAuditor(this);
		}
	}
}

//--------------------------------------------------------------------
// Set offset to position within attachment matrix
//--------------------------------------------------------------------
void tmlnAdapterReference::SetAttachOffset(const maPoint3d &i_AttachOffset)
{
	m_AttachOffset = i_AttachOffset;
}

//--------------------------------------------------------------------
// Set offset to rotation within attachment matrix
//--------------------------------------------------------------------
void tmlnAdapterReference::SetAttachOrientation(const maRotation &i_AttachOrient)
{
	m_AttachOrient = i_AttachOrient;

}

//--------------------------------------------------------------------
//  Get position of object, for target of camera
//--------------------------------------------------------------------
maPoint3d  tmlnAdapterReference::GetPosition() const
{
	if (m_pReference)
	{
		maMatrix4x4 matx = m_pReference->GetMatrix();
		return matx * m_AttachOffset; // target offset
	}

	return m_AttachOffset;
}

//--------------------------------------------------------------------
//  Get rotation of object based on attachment matrix
//--------------------------------------------------------------------
maRotation  tmlnAdapterReference::GetOrientation() const
{
	if (m_pReference)
	{
		maMatrix4x4 matx = m_pReference->GetMatrix();
		maRotation rot;
		rot.SetValue(matx);
		return rot * m_AttachOrient; // combine rotations
	}

	return m_AttachOrient;

}

//--------------------------------------------------------------------
// From envAuditor, this callback is called when the reference 
//	is deleted, so that we can break the connection.
//--------------------------------------------------------------------
//virtual 
void tmlnAdapterReference::AuditorNotify(envAuditable* i_pReference)
{
	if (m_pReference == i_pReference)
		m_pReference = NULL;
}


