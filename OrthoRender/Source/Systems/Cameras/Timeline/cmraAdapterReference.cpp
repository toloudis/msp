/*****************************************************************************
**	cmraAdapterReference.cpp
**
**	 Adapter for getting target position from attachment reference to object
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "Systems/Cameras/Timeline/cmraAdapterReference.hpp"

#include "Tool/api3d/api3dObject.hpp"
#include "Tool/api3d/api3dReference.hpp"

#include "Core/dbg/dbgLog.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraAdapterReference::cmraAdapterReference()
: m_pReference(NULL)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraAdapterReference::~cmraAdapterReference()
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
void cmraAdapterReference::AttachTo(api3dReference *i_pReference,
									const maPoint3d &i_TargetOffset)
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

	m_Offset = i_TargetOffset;
}

//--------------------------------------------------------------------
//  Get position of object, for target of camera
//--------------------------------------------------------------------
maPoint3d  cmraAdapterReference::GetPosition() const
{
	if (m_pReference)
	{
		maMatrix4x4 matx = m_pReference->GetMatrix();
		return matx * m_Offset; // target offset
	}

	return m_Offset;
}

//--------------------------------------------------------------------
// From envAuditor, this callback is called when the reference 
//	is deleted, so that we can break the connection.
//--------------------------------------------------------------------
//virtual 
void cmraAdapterReference::AuditorNotify(envAuditable* i_pReference)
{
	if (m_pReference == i_pReference)
		m_pReference = NULL;
}