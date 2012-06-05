/****************************************************************************\
**	relRelationshipSingle.hpp
**
**		Relationship between a parent object and a single child object.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef REL_RELATIONSHIPSINGLE_HPP
#error relRelationshipSingle.hpp multiply included
#endif
#define REL_RELATIONSHIPSINGLE_HPP

#ifndef REL_RELATIONSHIP_HPP
#include "Core/rel/relRelationship.hpp"
#endif 


//============================================================================
// relRelationshipSingle - relationship between parent and child object
//============================================================================
class relRelationshipSingle : public relRelationship 
{
	public:
		//--------------------------------------------------------------------
		// Static function as autility for creating a parent child 
		// relationship between two objects and adding the relationship
		// to the two objects.
		//--------------------------------------------------------------------
		static void CreateParentChildRelationship(relHandle i_Handle,
										   relObject& io_ParentObject,
										   relObject& io_ChildObject);

		//--------------------------------------------------------------------
		// constructor takes references to parent and child in the relationship
		//--------------------------------------------------------------------
		relRelationshipSingle(relHandle i_Handle,
							  relObject& i_ParentObject,
							  relObject& i_ChildObject);

		//--------------------------------------------------------------------
		//	CreateReferenceToObject - create a reference that refers to the
		//	given object. This reference should be able to refer to 
		//  future instances of this object persistent through deletion
		//	and recreation through undo. This object is assumed to be
		//	part of the relationship but not the container object.
		//--------------------------------------------------------------------
		virtual shared_ptr<relObjectReference> CreateReferenceToObject(relObject* i_pObject);

		//--------------------------------------------------------------------
		// Return pointer to child object
		//--------------------------------------------------------------------
		const relObject& GetChildObject() const { return m_ChildObject; }
		relObject& GetChildObject() { return m_ChildObject; }

	private:
		relObject& m_ChildObject;
};
