/****************************************************************************\
**	relRelationshipMultiple.hpp
**
**		Relationship between a parent object and a multiple child objects
**	organized through a std::vector of pointers to the child objects.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef REL_RELATIONSHIPMULTIPLE_HPP
#error relRelationshipMultiple.hpp multiply included
#endif
#define REL_RELATIONSHIPMULTIPLE_HPP

#ifndef REL_RELATIONSHIP_HPP
#include "Core/rel/relRelationship.hpp"
#endif 

#include <vector>


//============================================================================
// relRelationshipMultipleBase 
//============================================================================
class relRelationshipMultipleBase : public relRelationship 
{
	public:		
		//--------------------------------------------------------------------
		// constructor takes reference to object owning the relationship
		//--------------------------------------------------------------------
		relRelationshipMultipleBase(relHandle i_Handle,
									relObject& i_ContainerObject);

		//--------------------------------------------------------------------
		//	CreateReferenceToObject - create a reference that refers to the
		//	given object. This reference should be able to refer to 
		//  future instances of this object persistent through deletion
		//	and recreation through undo. This object is assumed to be
		//	part of the relationship but not the container object.
		//--------------------------------------------------------------------
		virtual shared_ptr<relObjectReference> CreateReferenceToObject(relObject* i_pObject);

		//--------------------------------------------------------------------
		// Return number of objects in child vector
		//--------------------------------------------------------------------
		virtual int GetNumObjects() const = 0;

		//--------------------------------------------------------------------
		// Return pointer to child object by index
		//--------------------------------------------------------------------
		virtual const relObject* GetObjectByIndex(int i_Index) const = 0;
		virtual relObject* GetObjectByIndex(int i_Index) = 0;

};


//============================================================================
// relRelationshipMultiple 
//============================================================================
template<class T>
class relRelationshipMultiple : public relRelationshipMultipleBase 
{
	public:
		//--------------------------------------------------------------------
		// constructor takes reference to parent and to the vector
		//	that will hold the children in the relationship
		//--------------------------------------------------------------------
		relRelationshipMultiple(relHandle i_Handle,
							  relObject& i_ParentObject,
							  std::vector<T*>& i_ChildVector)
		: relRelationshipMultipleBase(i_Handle, i_ParentObject),
			m_Children(i_ChildVector) {}

		//--------------------------------------------------------------------
		// Return number of objects in child vector
		//--------------------------------------------------------------------
		virtual int GetNumObjects() const
		{
			return m_Children.size();
		}

		//--------------------------------------------------------------------
		// Return pointer to child object by index
		//--------------------------------------------------------------------
		virtual const relObject* GetObjectByIndex(int i_Index) const
		{
			return m_Children[i_Index];
		}
		virtual relObject* GetObjectByIndex(int i_Index)
		{
			return m_Children[i_Index];
		}

	private:
		std::vector<T*>& m_Children;
};
