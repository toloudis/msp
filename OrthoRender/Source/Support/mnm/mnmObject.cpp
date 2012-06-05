/*****************************************************************************
**  mnmObject.cpp
**
**      A mnmObject is base class for objects that can be positioned
**  by manipulation modes.
**
**	Extra Large Technology
**	Copyright(C) 2001 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/mnmObject.hpp"

#include "Support/mnm/mnmReference.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mnmObject::mnmObject()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mnmObject::~mnmObject()
{
	envSTLHelpers::DeleteContainer(m_References);
}

//--------------------------------------------------------------------
//	GetWorldPivot returns the point in world space that this
//		object will rotate around. Used to center rotation and
//		scale compasses.
//--------------------------------------------------------------------
//virtual 
maPoint3d mnmObject::GetWorldPivot() const
{
	// without a specific implementation for a pivot point,
	//	the object will rotate around its world space position
	return this->GetPosition();
}

//--------------------------------------------------------------------
//	Get reference for given name.  The returned pointer is owned
//	by this object.  The object should be retained by the caller
//	to avoid repeated string searches.
//--------------------------------------------------------------------
//virtual
api3dReference* mnmObject::GetReference(const char* i_Name)
{
	api3dReference* pRef = new mnmReference(*this);
	m_References.push_back(pRef);
	return pRef;
//	return NULL;
}

//--------------------------------------------------------------------
// Get list of references for possible attachment within this object.
//--------------------------------------------------------------------
//virtual
void mnmObject::GetReferenceList(std::vector<std::string> &o_List)
{
	// default does nothing
}

//--------------------------------------------------------------------
// Add reference to list to be managed.
//--------------------------------------------------------------------
void mnmObject::AddReference(api3dReference* i_pReference)
{
	m_References.push_back(i_pReference);
}
