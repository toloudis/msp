/****************************************************************************\
**	relObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Core/rel/relObject.hpp"
#include "Core/rel/relRelationship.hpp"

#include "Core/Env/envSTLHelpers.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	// Simplest object reference, used by default - it just keeps a
	//	pointer to the objet in the assumption that the relObject
	//	will not be deleted. Bad assumption, but it works in some cases.
	// In other cases, the virtual function 
	//	relObject::CreateReferenceToSelf()
	// will have to be overriden in order to create a true logical 
	//	reference that handles deletion and recreation.
	//--------------------------------------------------------------------
	class StaticObjectReference : public relObjectReference
	{
		public:
			StaticObjectReference(relObject* i_Object)
				: m_Object(i_Object) {}

			virtual relObject* GetObject() const
			{
				return m_Object;
			}
		private:
			relObject* m_Object;
	};
}

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
relObject::relObject()
{

}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
relObject::~relObject()
{

}

//--------------------------------------------------------------------
//	CreateReferenceToSelf - create a reference that refers to the
//	given object. This reference should be able to refer to 
//  future instances of this object persistent through deletion
//	and recreation through undo.
//--------------------------------------------------------------------
shared_ptr<relObjectReference> relObject::CreateReferenceToSelf()
{
	// If we have a parent relationship, then ask that
	// parent relationship how to create a reference to this
	// object.
	if (shared_ptr<relRelationship> pParentRel = m_pParent.lock())
	{
		return pParentRel->CreateReferenceToObject(this);
	}
	else
	{
		// If no parent relationship and this virtual function has
		// not been overriden, then fall back onto the static reference
		// that assumes this object is not going to be deleted.
		shared_ptr<relObjectReference> static_reference(new StaticObjectReference(this));
		return static_reference;
	}
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
std::string relObject::GetDisplayName() const
{
	return std::string("");
}

//--------------------------------------------------------------------
// Get parent object of this object by following the 
// parent relationship, if it exists. Otherwise, retuns NULL.
//--------------------------------------------------------------------
relObject* relObject::GetParentObject() const
{
	if (shared_ptr<relRelationship> pParentRel = m_pParent.lock())
	{
		return (&(pParentRel->GetContainerObject()));
	}
	return NULL;
}

//--------------------------------------------------------------------
// Return relationship with given handle, if found
//--------------------------------------------------------------------
relRelationship* relObject::GetRelationshipByHandle(relHandle i_Handle) const
{
	std::vector< shared_ptr<relRelationship> >::const_iterator it;
	for (it = m_Relationships.begin(); it != m_Relationships.end(); ++it)
	{
		if ((*it)->GetHandle() == i_Handle)
			return it->get();
	}
	return NULL;
}

//--------------------------------------------------------------------
// Add relationship to this object
//--------------------------------------------------------------------
void relObject::AddRelationship(const shared_ptr<relRelationship>& i_Relationship)
{
	m_Relationships.push_back(i_Relationship);
}

//--------------------------------------------------------------------
// Remove relationship from this object
//--------------------------------------------------------------------
void relObject::RemoveRelationship(const shared_ptr<relRelationship>& i_Relationship)
{
	envSTLHelpers::RemoveOneValue( m_Relationships, i_Relationship);
}

//--------------------------------------------------------------------
// Define the parent relationship for this object. This class
//	will take a weak_ptr to this relationship.
//--------------------------------------------------------------------
void relObject::SetParentRelationship(const shared_ptr<relRelationship>& i_Relationship)
{
	m_pParent = i_Relationship;
}
