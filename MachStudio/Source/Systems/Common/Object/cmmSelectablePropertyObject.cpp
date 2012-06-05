/****************************************************************************\
**	cmmSelectablePropertyObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/Object/cmmSelectablePropertyObject.hpp"
#include "Core/prty/prtyProperty.hpp"

#include "Core/dbg/dbgMsg.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
	//--------------------------------------------------------------------
	// Relationship based property reference - it uses the
	//	relObject base class to create a relObjectReference to find
	//	the object first and then look for the property by name.
	//--------------------------------------------------------------------
	class RelationshipPropertyReference : public prtyPropertyReference
	{
		public:
			RelationshipPropertyReference(cmmSelectablePropertyObject& i_PropertyObject,
									 const std::string& i_PropertyName)
				: m_PropertyName(i_PropertyName) 
			{
				m_ObjectReference = i_PropertyObject.CreateReferenceToSelf();
			}

			virtual prtyProperty* GetProperty()
			{
				relObject *pObject = m_ObjectReference->GetObject();
				if (pObject)
				{
					prtyObject *pPrtyObj = dynamic_cast<prtyObject*>(pObject);
					if (pPrtyObj)
					{
						const prtyProperty* pProperty = pPrtyObj->GetProperty(m_PropertyName);
						if (pProperty)
						{
							return const_cast<prtyProperty*>(pProperty);
						}
					}
				}
				DBG_WARNING("Could not resolve property" << m_PropertyName);
				return NULL;
			}
		private:
			shared_ptr<relObjectReference> m_ObjectReference;
			std::string m_PropertyName;
	};
}

//--------------------------------------------------------------------
//	CreateReferenceForProperty - given a property, create a
//	shared_ptr to a prtyPropertyReference to this property.
//--------------------------------------------------------------------
//virtual 
shared_ptr<prtyPropertyReference> cmmSelectablePropertyObject::CreateReferenceForProperty(prtyProperty& i_Property)
{
	shared_ptr<prtyPropertyReference> relative_reference(
		new RelationshipPropertyReference(*this, i_Property.GetPropertyName()));
	return relative_reference;
}

