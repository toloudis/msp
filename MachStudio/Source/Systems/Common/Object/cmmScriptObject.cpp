/****************************************************************************\
**	cmmScriptObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/Object/cmmScriptObject.hpp"

#include "Systems/Common/Object/cmmSelectablePropertyObject.hpp"

#include "Support/tmln/tmlnDriver.hpp"

#include "Core/rel/relRelationshipSingle.hpp"


//--------------------------------------------------------------------
//	SetPropertyObject - create a parent-child relationship between
//		this script object and the property object it is animating.
//--------------------------------------------------------------------
void cmmScriptObject::SetPropertyObject(cmmSelectablePropertyObject& i_ChildObject)
{
	// direct where the extra properties should be created
	this->SetExtraPropertyObject(&i_ChildObject);

	// Create parent-child relationship
	shared_ptr<relRelationshipSingle> parent_child(
		new relRelationshipSingle("Script", *this, i_ChildObject));

	// Only the parent object owns the relationship
	this->AddRelationship( parent_child );

	// The child relationship is through a weak_ptr
	i_ChildObject.SetParentRelationship( parent_child );

	// Connect all drivers to this object through the child object
	this->SetDriverRelationship( i_ChildObject );
}


//--------------------------------------------------------------------
//	Remap internal name attachments using the given map.
//  This is part of the duplication process and makes sures 
//	internal attachments are passed onto the duplicated objects.
//--------------------------------------------------------------------
void cmmScriptObject::RemapNames(const std::map<nameString, nameString> &i_DuplicateNameMap)
{
	// Pass remapping request on to the drivers - most internal attachments are there.
	int num_drivers = this->GetNumDrivers();
	for (int i=0; i<num_drivers; i++)
	{
		this->GetDriverDirect(i)->RemapNames(i_DuplicateNameMap);
	}
}
