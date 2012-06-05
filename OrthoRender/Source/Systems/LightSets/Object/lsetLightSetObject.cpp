/*****************************************************************************
**  lsetLightSetObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Object/lsetLightSetObject.hpp"

#include "Systems/LightSets/GUI/lsetDialogDataUtil.hpp"
#include "Systems/LightSets/Data/lsetDocumentChunk.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/ltst/ltstLightSet.hpp"	
#include "Core/prty/prtyColorRGBEditUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lsetLightSetObject::lsetLightSetObject(const nameString& i_Name)
:	m_pParent(NULL)
{
	m_Data.m_Name.SetValue(i_Name);
	m_pLightSet = ltstLightSetMgr::CreateLightSet(i_Name);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Light Set", "Name of the object");
	AddProperty( pPUII );

	pPUII = new prtyColorRGBEditUIInfo(&(m_Data.m_AmbientLight), "Light Set", "Ambient contribution from light set");
	AddProperty( pPUII );


	// Register callbacks to update particle generator and icons
	// when properties change
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<lsetLightSetObject>(this, &lsetLightSetObject::NameChanged));
	m_Data.m_AmbientLight.AddCallback(new prtyCallbackWrapper<lsetLightSetObject>(this, &lsetLightSetObject::ValueChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lsetLightSetObject::~lsetLightSetObject()
{
	ltstLightSetMgr::DeleteLightSet(m_pLightSet->m_Name);
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string lsetLightSetObject::GetPick3dName() const
{
	return GetName().GetString();
}

//--------------------------------------------------------------------
// Get values as a light set data structure
//--------------------------------------------------------------------
const lsetData& lsetLightSetObject::GetData() const
{
	// Since we don't have properties that handle things like
	// checked lists and tree controls, the light set manager
	// still maintains the "true" object and light connectíons.
	// We have to refresh that info from the manager whenever
	// someone asks for the data.
	ltstLightSetData data;
	m_pLightSet->GetData(data);
	m_Data.m_Lights = data.m_Lights;
	m_Data.m_Objects = data.m_Objects;

	return m_Data;
}

//--------------------------------------------------------------------
// Set from light set data structure
//--------------------------------------------------------------------
void lsetLightSetObject::SetData(const lsetData &i_Data)
{
	ltstLightSetMgr::ClearLightSet(m_Data.m_Name.GetValue());

	// This may change the name of the layer
	m_Data = i_Data;

	nameString set_name = m_Data.m_Name.GetValue();
	const int num_lights = m_Data.m_Lights.size();
	for (int i=0; i<num_lights; ++i)
	{
		ltstLightSetMgr::AddLightToSet(set_name, m_Data.m_Lights[i]);
	}

	const int num_objects = m_Data.m_Objects.size();
	for (int i=0; i<num_objects; ++i)
	{
		const ltstLightSetObjectData &obj_data = m_Data.m_Objects[i];
		if (obj_data.m_LitFragmentIndices.empty())
		{
			// Add whole object
			ltstLightSetMgr::AddObjectToLightSet(set_name, obj_data.m_Name);
		}
		else
		{
			// Add just the lit nodes
			for (int l=0; l<obj_data.m_LitFragmentIndices.size(); ++l)
			{
				ltstLightSetMgr::AddNodeToLightSet(set_name, obj_data.m_Name, obj_data.m_LitFragmentIndices[l]);
			}
		}
	}
}

//--------------------------------------------------------------------
// Name property access
//--------------------------------------------------------------------
prtyName&	lsetLightSetObject::PropertyName()
{
	return m_Data.m_Name;
}
const prtyName&	lsetLightSetObject::GetPropertyName() const
{
	return m_Data.m_Name;
}

//--------------------------------------------------------------------
// AmbientLight property access
//--------------------------------------------------------------------
prtyColor&	lsetLightSetObject::PropertyAmbientLight()
{
	return m_Data.m_AmbientLight;
}
const prtyColor&	lsetLightSetObject::GetPropertyAmbientLight() const
{
	return m_Data.m_AmbientLight;
}

//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
void lsetLightSetObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name.SetValue(this->GetName());

	// set name into underlying ltst
	m_pLightSet->m_Name = this->GetName();
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void lsetLightSetObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// The property's check will prevent an infinite
	// loop here.
	this->SetName( m_Data.m_Name.GetValue() );
	//m_pLightSet->m_Name = m_Data.m_Name.GetValue();

	if (i_bDirty)
	{
		lsetDialogDataUtil::UpdateListDialog();
		lsetDocumentChunk::ActiveDataChanged();
	}
}

void lsetLightSetObject::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_pLightSet->SetAmbientLight(m_Data.m_AmbientLight.GetValue());

	if (i_bDirty)
	{
		lsetDocumentChunk::ActiveDataChanged();
	}
}

//--------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//--------------------------------------------------------------------
//virtual 
pick3dPickObject* lsetLightSetObject::GetParentObject() const
{
	return m_pParent;
}
void lsetLightSetObject::SetParentObject(pick3dPickObject* i_pPO)
{
	m_pParent = i_pPO;
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
//void lsetLightSetObject::Add(const nameString& i_ObjectName)
//{
//	m_Data.m_Objects.push_back(i_ObjectName);
//	ltstLightSetMgr::AddObjectToLightSet(m_pLightSet, i_ObjectName);
//}
//void lsetLightSetObject::Remove(const nameString& i_ObjectName)
//{
//	envSTLHelpers::RemoveOneValue(m_Data.m_Objects, i_ObjectName);
//	ltstLightSetMgr::RemoveObjectFromLightSet(m_pLightSet, i_ObjectName);
//}
//void lsetLightSetObject::RefreshNames()
//{
//	m_Data.m_Objects.clear();
//	for (int i = 0; i < m_pLightSet->m_Objects.size(); i++)
//	{
//		m_Data.m_Objects.push_back(m_pLightSet->m_Objects[i]->m_pNameObj->GetName());
//	}
//}
