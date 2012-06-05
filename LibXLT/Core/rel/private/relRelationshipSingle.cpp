/****************************************************************************\
**	relRelationshipSingle.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Core/rel/relRelationshipSingle.hpp"
#include "Core/rel/relObject.hpp"

#include "Core/Dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
namespace
{
	//============================================================================
	// SingleReference - reference that can follow the relationship between 
	//	parent and child
	//============================================================================
	class SingleReference : public relObjectReference
	{
		public:
			//--------------------------------------------------------------------
			//--------------------------------------------------------------------
			SingleReference(const shared_ptr<relObjectReference> &i_ParentRef,
							relHandle i_RelationshipHandle)
				: m_ParentReference(i_ParentRef), m_Handle(i_RelationshipHandle)
			{

			}

			//--------------------------------------------------------------------
			//	GetObject - return a pointer to an object that is usable
			//	for a short period of time. 
			//--------------------------------------------------------------------
			virtual relObject* GetObject() const
			{
				relObject *pParent = m_ParentReference->GetObject();
				if (pParent)
				{
					relRelationship *pChildRel = pParent->GetRelationshipByHandle(m_Handle);
					DBG_ASSERT(pChildRel, "Could not find relationship: " << m_Handle);
					if (pChildRel)
					{
						relRelationshipSingle *pSingleRel = dynamic_cast<relRelationshipSingle*>(pChildRel);
						DBG_ASSERT(pSingleRel, "Expected single relationship: " << m_Handle);
						if (pSingleRel)
						{
							return (&pSingleRel->GetChildObject());
						}
					}
				}
				return NULL;
			}

		private:
			shared_ptr<relObjectReference> m_ParentReference;
			relHandle m_Handle;

	};
}

//--------------------------------------------------------------------
// Static function as autility for creating a parent child 
// relationship between two objects and adding the relationship
// to the two objects.
//--------------------------------------------------------------------
void relRelationshipSingle::CreateParentChildRelationship(relHandle i_Handle,
								   relObject& io_ParentObject,
								   relObject& io_ChildObject)
{
	// Create relationship between two objects
	shared_ptr<relRelationshipSingle> parent_child(
		new relRelationshipSingle(i_Handle, io_ParentObject, io_ChildObject));

	// Only the parent object owns the relationship
	io_ParentObject.AddRelationship( parent_child );

	// The child relationship is through a weak_ptr
	io_ChildObject.SetParentRelationship( parent_child );
}

//--------------------------------------------------------------------
// constructor takes references to parent and child in the relationship
//--------------------------------------------------------------------
relRelationshipSingle::relRelationshipSingle(relHandle i_Handle,
											 relObject& i_ParentObject,
											 relObject& i_ChildObject)
: relRelationship(i_Handle, i_ParentObject),
  m_ChildObject(i_ChildObject)
{

}

//--------------------------------------------------------------------
//	CreateReferenceToObject - create a reference that refers to the
//	given object. This reference should be able to refer to 
//  future instances of this object persistent through deletion
//	and recreation through undo. This object is assumed to be
//	part of the relationship but not the container object.
//--------------------------------------------------------------------
shared_ptr<relObjectReference> relRelationshipSingle::CreateReferenceToObject(relObject* i_pObject)
{
	DBG_ASSERT(&m_ChildObject == i_pObject, "Single relationships can only reference child object");
	shared_ptr<relObjectReference> parent_ref = this->GetContainerObject().CreateReferenceToSelf();

	shared_ptr<relObjectReference> single_reference(new SingleReference(parent_ref, this->GetHandle()));
	return single_reference;
}

