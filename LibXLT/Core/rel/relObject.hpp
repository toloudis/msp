/****************************************************************************\
**	relObject.hpp
**
**		Base class for application objects with relationships to 
**	other application objects.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef REL_OBJECT_HPP
#error relObject.hpp multiply included
#endif
#define REL_OBJECT_HPP

#ifndef REL_OBJECTREFERENCE_HPP
#include "Core/rel/relObjectReference.hpp"
#endif 
#ifndef REL_HANDLE_HPP
#include "Core/rel/relHandle.hpp"
#endif 

#include <vector>


//============================================================================
//============================================================================
class relRelationship;


//============================================================================
//============================================================================
class relObject 
{
	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		relObject();

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~relObject();

		//--------------------------------------------------------------------
		//	CreateReferenceToSelf - create a reference that refers to the
		//	given object. This reference should be able to refer to 
		//  future instances of this object persistent through deletion
		//	and recreation through undo.
		//--------------------------------------------------------------------
		virtual shared_ptr<relObjectReference> CreateReferenceToSelf();

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetDisplayName() const;

		//--------------------------------------------------------------------
		// Get parent object of this object by following the 
		// parent relationship, if it exists. Otherwise, retuns NULL.
		//--------------------------------------------------------------------
		relObject* GetParentObject() const;

		//--------------------------------------------------------------------
		// Return relationship with given handle, if found
		//--------------------------------------------------------------------
		relRelationship* GetRelationshipByHandle(relHandle i_Handle) const;

		//--------------------------------------------------------------------
		// Add relationship to this object
		//--------------------------------------------------------------------
		void AddRelationship(const shared_ptr<relRelationship>& i_Relationship);

		//--------------------------------------------------------------------
		// Remove relationship from this object
		//--------------------------------------------------------------------
		void RemoveRelationship(const shared_ptr<relRelationship>& i_Relationship);

		//--------------------------------------------------------------------
		// Define the parent relationship for this object. This class
		//	will take a weak_ptr to this relationship.
		//
		// Note that you should not add parent relationships to the 
		// relationship list through "AddRelationship", just use
		// this function.
		//--------------------------------------------------------------------
		void SetParentRelationship(const shared_ptr<relRelationship>& i_Relationship);

	private:
		weak_ptr<relRelationship> m_pParent;
		std::vector< shared_ptr<relRelationship> > m_Relationships;
};
