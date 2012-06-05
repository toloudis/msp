/****************************************************************************\
**	relRelationship.hpp
**
**		Base class for defining how application objects relate to 
**	each other. 
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef REL_RELATIONSHIP_HPP
#error relRelationship.hpp multiply included
#endif
#define REL_RELATIONSHIP_HPP

#ifndef REL_OBJECTREFERENCE_HPP
#include "Core/rel/relObjectReference.hpp"
#endif 
#ifndef REL_HANDLE_HPP
#include "Core/rel/relHandle.hpp"
#endif 


//============================================================================
//============================================================================
class relRelationship 
{
	public:
		//--------------------------------------------------------------------
		// constructor takes reference to object owning the relationship
		//--------------------------------------------------------------------
		relRelationship(relHandle i_Handle,
						relObject& i_ContainerObject);

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~relRelationship();

		//--------------------------------------------------------------------
		//	CreateReferenceToObject - create a reference that refers to the
		//	given object. This reference should be able to refer to 
		//  future instances of this object persistent through deletion
		//	and recreation through undo. This object is assumed to be
		//	part of the relationship but not the container object.
		//--------------------------------------------------------------------
		virtual shared_ptr<relObjectReference> CreateReferenceToObject(relObject* i_pObject) = 0;

		//--------------------------------------------------------------------
		// Get handle for this relationship
		//--------------------------------------------------------------------
		relHandle GetHandle() const { return m_Handle; }

		//--------------------------------------------------------------------
		// Return pointer to conatiner object
		//--------------------------------------------------------------------
		const relObject& GetContainerObject() const { return m_ContainerObject; }
		relObject& GetContainerObject() { return m_ContainerObject; }

	private:
		relHandle m_Handle;
		relObject& m_ContainerObject;
};
