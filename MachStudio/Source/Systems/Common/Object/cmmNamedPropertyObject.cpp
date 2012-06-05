/****************************************************************************\
**	cmmNamedPropertyObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/Object/cmmNamedPropertyObject.hpp"
#include "Core/prty/prtyProperty.hpp"

#include "Core/name/nameMgr.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
	//--------------------------------------------------------------------
	// Named object reference, uses nameMgr to locate the object 
	//	by name.
	//--------------------------------------------------------------------
	class NamedObjectReference : public relObjectReference
	{
		public:
			NamedObjectReference(const nameString& i_Name)
				: m_Name(i_Name) {}

			virtual relObject* GetObject() const
			{
				nameObject *pNameObj = nameMgr::GetObjectByName(m_Name);
				if (pNameObj)
				{
					relObject *pRelObj = dynamic_cast<relObject*>(pNameObj);
					if (pRelObj)
					{
						return pRelObj;
					}
				}
				DBG_WARNING("Could not resolve named object " << m_Name.GetString());
				return NULL;
			}
		private:
			nameString m_Name;
	};
}
//--------------------------------------------------------------------
//	CreateReferenceToSelf - create a reference that refers to the
//	given object. This reference should be able to refer to 
//  future instances of this object persistent through deletion
//	and recreation through undo.
//--------------------------------------------------------------------
shared_ptr<relObjectReference> cmmNamedPropertyObject::CreateReferenceToSelf()
{
	shared_ptr<relObjectReference> named_reference(new NamedObjectReference(this->GetName()));
	return named_reference;
}
