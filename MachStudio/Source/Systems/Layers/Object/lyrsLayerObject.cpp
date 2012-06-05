/*****************************************************************************
**  lyrsLayerObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Layers/Object/lyrsLayerObject.hpp"

#include "Systems/Layers/GUI/lyrsDialogDataUtil.hpp"
#include "Systems/Layers/Data/lyrsDocumentChunk.hpp"
#include "Systems/Layers/Undo/lyrsOperations.hpp"

#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lyrsLayerObject::lyrsLayerObject(const lyrsData& i_Data,
								 const nameString& i_Name)
:	m_pParent(NULL)
{
	m_Data.m_Name.SetValue(i_Name);
	m_pLayer = lyerLayerMgr::CreateLayer(i_Name);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Layer", "Name of the object");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bVisible), "Layer", "Visibility of layer objects");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bPickable), "Layer", "Pickability of layer objects");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bWireframe), "Layer", "Wireframe display of layer objects");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bLowRes), "Layer", "Low resolution display for layer objects");
	AddProperty( pPUII );


	// Register callbacks to update particle generator and icons
	// when properties change
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<lyrsLayerObject>(this, &lyrsLayerObject::NameChanged));
	m_Data.m_bVisible.AddCallback(new prtyCallbackWrapper<lyrsLayerObject>(this, &lyrsLayerObject::ValueChanged));
	m_Data.m_bPickable.AddCallback(new prtyCallbackWrapper<lyrsLayerObject>(this, &lyrsLayerObject::ValueChanged));
	m_Data.m_bWireframe.AddCallback(new prtyCallbackWrapper<lyrsLayerObject>(this, &lyrsLayerObject::ValueChanged));
	m_Data.m_bLowRes.AddCallback(new prtyCallbackWrapper<lyrsLayerObject>(this, &lyrsLayerObject::ValueChanged));

	// Now, set the data into our properties
	this->SetData(i_Data);
	this->SetName(i_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lyrsLayerObject::~lyrsLayerObject()
{
	lyerLayerMgr::DeleteLayer(lyerLayerMgr::GetLayerName(m_pLayer));
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string lyrsLayerObject::GetDisplayName() const
{
	return GetName().GetString();
}

//--------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//--------------------------------------------------------------------
//virtual 
//sel3dObject* lyrsLayerObject::GetParentObject() const
//{
//	return m_pParent;
//}
//void lyrsLayerObject::SetParentObject(sel3dObject* i_pPO)
//{
//	m_pParent = i_pPO;
//}


//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void lyrsLayerObject::GetResourceList( fsResourceTrackerData& io_List )
{
	// no resources in layer
}

//--------------------------------------------------------------------
// Get values as a layer data structure
//--------------------------------------------------------------------
const lyrsData& lyrsLayerObject::GetData() const
{
	// Since we don't have a property for checked lists, the layer manager
	// still maintains the "true" object connectíons.
	// We have to refresh that info from the manager whenever
	// someone asks for the data.
	m_Data.m_Objects.clear();
	lyerLayerMgr::GetObjectsInLayer(lyerLayerMgr::GetLayerName(m_pLayer), m_Data.m_Objects);

	return m_Data;
}

//--------------------------------------------------------------------
// Set from layer data structure
//--------------------------------------------------------------------
void lyrsLayerObject::SetData(const lyrsData &i_Data)
{
	lyerLayerMgr::ClearLayer(m_Data.m_Name.GetValue());

	// This may change the name of the layer
	m_Data = i_Data;

	nameString set_name = m_Data.m_Name.GetValue();
	const int num_objects = m_Data.m_Objects.size();
	for (int i=0; i<num_objects; ++i)
	{
		lyerLayerMgr::AddObjectToLayer(set_name, m_Data.m_Objects[i]);
	}

}

//--------------------------------------------------------------------
// Name property access
//--------------------------------------------------------------------
prtyName&	lyrsLayerObject::PropertyName()
{
	return m_Data.m_Name;
}
const prtyName&	lyrsLayerObject::GetPropertyName() const
{
	return m_Data.m_Name;
}

//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
void lyrsLayerObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name.SetValue(this->GetName());

	// set name into underlying layer
	lyerLayerMgr::SetLayerName(m_pLayer, this->GetName());
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void lyrsLayerObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// The property's check will prevent an infinite
	// loop here.
	this->SetName( m_Data.m_Name.GetValue() );

	if (i_bDirty)
	{
		lyrsDialogDataUtil::UpdateListDialog();
		lyrsDocumentChunk::ActiveDataChanged();
	}
}

void lyrsLayerObject::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	nameString layer_name = lyerLayerMgr::GetLayerName(m_pLayer);
	lyerLayerMgr::SetLayerVisible(layer_name, m_Data.m_bVisible.GetValue());
	lyerLayerMgr::SetLayerPickable(layer_name, m_Data.m_bPickable.GetValue());
	lyerLayerMgr::SetLayerWireframe(layer_name, m_Data.m_bWireframe.GetValue());
	lyerLayerMgr::SetLayerLowRes(layer_name, m_Data.m_bLowRes.GetValue());

	if (i_bDirty)
	{
		lyrsDocumentChunk::ActiveDataChanged();
	}
}