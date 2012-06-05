/*****************************************************************************
**  grupGroupObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Groups/Object/grupGroupObject.hpp"

#include "Systems/Groups/GUI/grupDialogDataUtil.hpp"
#include "Systems/Groups/Data/grupDocumentChunk.hpp"
#include "Systems/Groups/Undo/grupOperations.hpp"

#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
grupGroupObject::grupGroupObject(const grupData& i_Data,
								 const nameString& i_Name)
:	m_pParent(NULL)
{
	m_Data.m_Name.SetValue(i_Name);
	m_pGroup = grpsGroupMgr::CreateGroup(i_Name);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Group", "Name of the object");
	AddProperty( pPUII );

	// Register callbacks to update particle generator and icons
	// when properties change
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<grupGroupObject>(this, &grupGroupObject::NameChanged));

	// Now, set the data into our properties
	this->SetData(i_Data);
	this->SetName(i_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
grupGroupObject::~grupGroupObject()
{
	grpsGroupMgr::DeleteGroup(grpsGroupMgr::GetGroupName(m_pGroup));
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string grupGroupObject::GetPick3dName() const
{
	return GetName().GetString();
}

//--------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//--------------------------------------------------------------------
//virtual 
pick3dPickObject* grupGroupObject::GetParentObject() const
{
	return m_pParent;
}
void grupGroupObject::SetParentObject(pick3dPickObject* i_pPO)
{
	m_pParent = i_pPO;
}


//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void grupGroupObject::GetResourceList( fsResourceTrackerData& io_List )
{
	// no resources in group
}

//--------------------------------------------------------------------
// Get values as a group data structure
//--------------------------------------------------------------------
const grupData& grupGroupObject::GetData() const
{
	// Since we don't have a property for checked lists, the group manager
	// still maintains the "true" object connectíons.
	// We have to refresh that info from the manager whenever
	// someone asks for the data.
	m_Data.m_Objects.clear();
	grpsGroupMgr::GetObjectsInGroup(grpsGroupMgr::GetGroupName(m_pGroup), m_Data.m_Objects);

	return m_Data;
}

//--------------------------------------------------------------------
// Set from group data structure
//--------------------------------------------------------------------
void grupGroupObject::SetData(const grupData &i_Data)
{
	grpsGroupMgr::ClearGroup(m_Data.m_Name.GetValue());

	// This may change the name of the group
	m_Data = i_Data;

	nameString set_name = m_Data.m_Name.GetValue();
	const int num_objects = m_Data.m_Objects.size();
	for (int i=0; i<num_objects; ++i)
	{
		grpsGroupMgr::AddObjectToGroup(set_name, m_Data.m_Objects[i]);
	}

}

//--------------------------------------------------------------------
// Name property access
//--------------------------------------------------------------------
prtyName&	grupGroupObject::PropertyName()
{
	return m_Data.m_Name;
}
const prtyName&	grupGroupObject::GetPropertyName() const
{
	return m_Data.m_Name;
}

//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
void grupGroupObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name.SetValue(this->GetName());

	// set name into underlying group
	grpsGroupMgr::SetGroupName(m_pGroup, this->GetName());
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void grupGroupObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// The property's check will prevent an infinite
	// loop here.
	this->SetName( m_Data.m_Name.GetValue() );

	if (i_bDirty)
	{
		grupDialogDataUtil::UpdateListDialog();
		grupDocumentChunk::ActiveDataChanged();
	}
}
