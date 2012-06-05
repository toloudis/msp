/****************************************************************************\
**	relRelationshipMultiple.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Core/rel/relRelationshipMultiple.hpp"
#include "Core/rel/relObject.hpp"

#include "Core/Dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
namespace
{
	//============================================================================
	// MultipleReference - reference that can follow the relationship between 
	//	parent and child within an indexed vector
	//============================================================================
	class MultipleReference : public relObjectReference
	{
		public:
			//--------------------------------------------------------------------
			//--------------------------------------------------------------------
			MultipleReference(const shared_ptr<relObjectReference> &i_ParentRef,
							relHandle i_RelationshipHandle,
							int i_ChildIndex)
				: m_ParentReference(i_ParentRef), 
				  m_Handle(i_RelationshipHandle),
				  m_ChildIndex(i_ChildIndex) {}

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
						relRelationshipMultipleBase *pMultipleRel = dynamic_cast<relRelationshipMultipleBase*>(pChildRel);
						DBG_ASSERT(pMultipleRel, "Expected multiple relationship: " << m_Handle);
						if (pMultipleRel)
						{
							int num_objects = pMultipleRel->GetNumObjects();
							DBG_ASSERT(num_objects > m_ChildIndex, "Not enough children in multiple relationship " << m_Handle);
							if (num_objects > m_ChildIndex)
								return (pMultipleRel->GetObjectByIndex(m_ChildIndex));
						}
					}
				}
				return NULL;
			}

		private:
			shared_ptr<relObjectReference> m_ParentReference;
			relHandle m_Handle;
			int m_ChildIndex;

	};
}


//============================================================================
// relRelationshipMultipleBase
//============================================================================

//--------------------------------------------------------------------
// constructor takes reference to object owning the relationship
//--------------------------------------------------------------------
relRelationshipMultipleBase::relRelationshipMultipleBase(relHandle i_Handle,
														 relObject& i_ContainerObject)
: relRelationship(i_Handle, i_ContainerObject)
{

}

//--------------------------------------------------------------------
//	CreateReferenceToObject - create a reference that refers to the
//	given object. This reference should be able to refer to 
//  future instances of this object persistent through deletion
//	and recreation through undo. This object is assumed to be
//	part of the relationship but not the container object.
//--------------------------------------------------------------------
shared_ptr<relObjectReference> relRelationshipMultipleBase::CreateReferenceToObject(relObject* i_pObject)
{
	shared_ptr<relObjectReference> parent_ref = this->GetContainerObject().CreateReferenceToSelf();

	int num_objects = this->GetNumObjects();
	for (int i=0; i<num_objects; ++i)
	{
		if (this->GetObjectByIndex(i) == i_pObject)
		{
			shared_ptr<relObjectReference> multiple_reference(new MultipleReference(parent_ref, this->GetHandle(), i));
			return multiple_reference;
		}
	}
	DBG_ASSERT(false, "Multiple relationships need to reference one of the child objects.");
	return shared_ptr<relObjectReference>();
}
