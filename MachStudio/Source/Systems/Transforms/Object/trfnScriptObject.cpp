/*****************************************************************************
**  trfnScriptObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/Transforms/Object/trfnScriptObject.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/grps/grpsGroupMgr.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "Support/xfrm/xfrmTransformMgr.hpp"
#include "Systems/Transforms/Data/trfnDocumentChunk.hpp"
#include "Systems/Transforms/Undo/trfnOperations.hpp"
#include "Systems/Common/Object/cmmNamedScriptObjectReference.hpp"
#include "Systems/Common/GUI/cmmSceneOperations.hpp"

// library
#include "Core/fs/fsFileUtil.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"


//----------------------------------------------------------------------------
// ownership for the api3dObject passes to this object
//----------------------------------------------------------------------------
trfnScriptObject::trfnScriptObject( )
:	m_bShowDriverIcons(true),
	m_bSelected(false)
{
	//	create the base object
	m_pIcon = new trfnTransformObject();
	this->SetPropertyObject(*m_pIcon);

	// Timeline stuff
	tmlnTimelineMgr::AddObject(this, "Parents");

	// Layer manager registration
	//lyerLayerMgr::AddObject(m_pIcon, this);
	//grpsGroupMgr::AddObject(m_pIcon, m_pIcon);
	
	//	Add the channels
	//
	m_pChannelVisible = new tmlnChannelBooleanProperty( m_pIcon->PropertyVisible() );
	this->AddChannel(m_pChannelVisible);
	m_pChannelPosition = new tmlnChannelPositionProperty(  m_pIcon->PropertyPosition() );
	this->AddChannel(m_pChannelPosition);
	m_pChannelOrientation = new tmlnChannelOrientationProperty( m_pIcon->PropertyOrientation() );
	this->AddChannel(m_pChannelOrientation);
	m_pChannelScale = new tmlnChannelFloatProperty( m_pIcon->PropertyScale() );
	this->AddChannel(m_pChannelScale);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
trfnScriptObject::~trfnScriptObject()
{
	//DBG_LOG("In trfnScriptObject destructor");

	// Timeline stuff
	tmlnTimelineMgr::RemoveObject(this);

	// Unregister from layers
	//lyerLayerMgr::RemoveObject(m_pIcon, this);
	//grpsGroupMgr::RemoveObject(m_pIcon, m_pIcon);

	delete m_pIcon;
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string trfnScriptObject::GetDisplayName() const
{
	return m_pIcon->GetName().GetString();
}

//--------------------------------------------------------------------
//	CreateReferenceToSelf - create a reference that refers to the
//	given object. This reference should be able to refer to 
//  future instances of this object persistent through deletion
//	and recreation through undo.
//--------------------------------------------------------------------
shared_ptr<relObjectReference> trfnScriptObject::CreateReferenceToSelf()
{
	shared_ptr<relObjectReference> named_reference(new cmmNamedScriptObjectReference<trfnObjectMgr>(this->GetName()));
	return named_reference;
}

//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void trfnScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
	//	add all the driver resources as well
	tmlnScriptObject::GetResourceList( io_List );
}

//--------------------------------------------------------------------
//	Remap internal name attachments using the given map.
//  This is part of the duplication process and makes sures 
//	internal attachments are passed onto the duplicated objects.
//--------------------------------------------------------------------
//virtual 
void trfnScriptObject::RemapNames(const std::map<nameString, nameString> &i_DuplicateNameMap)
{
	cmmScriptObject::RemapNames(i_DuplicateNameMap);

	std::vector<nameString> child_objects;
	xfrmTransformMgr::GetNodesInTransform(this->GetName(), child_objects);

	for (int i=0; i<child_objects.size(); ++i)
		cmmSceneOperations::RemapNames(child_objects[i], i_DuplicateNameMap);
}

//--------------------------------------------------------------------
//	Name - simply pass functions to icon object
//--------------------------------------------------------------------
void trfnScriptObject::SetName(const nameString& i_Name)
{
    m_pIcon->SetName( i_Name );
}
const nameString& trfnScriptObject::GetName() const
{
	return m_pIcon->GetName();
}

//--------------------------------------------------------------------
//	ShowIcons sets whether the driver icons are visible
//--------------------------------------------------------------------
void trfnScriptObject::ShowIcons(bool i_bVisible)
{
	m_bShowDriverIcons = i_bVisible;
	//bool editor_visible = m_pIcon->GetEditorVisible();
	//this->ShowDriverIcons(i_bVisible && editor_visible 
	//	&& this->GetLayerPickable() && m_bSelected);
	this->ShowDriverIcons(i_bVisible && m_bSelected);
}

//--------------------------------------------------------------------
//	Set whether this object is selected in order to control
//	display of icons or render style, etc.
//--------------------------------------------------------------------
void trfnScriptObject::SetSelected(bool i_bSelected)
{
	//TODO: Need to have selection interest call this function
	m_bSelected = i_bSelected;
	//bool editor_visible = m_pIcon->GetEditorVisible();
	//this->ShowDriverIcons(m_bShowDriverIcons && editor_visible 
	//	&& this->GetLayerPickable() && m_bSelected);	
	this->ShowDriverIcons(m_bShowDriverIcons && m_bSelected);
}

//--------------------------------------------------------------------
// Set object active state in the render layer
//--------------------------------------------------------------------
void trfnScriptObject::SetActiveRenderLayer(bool i_bActive)
{
	//TODO: Need to add transform hierarchy into render layer GUI
	//m_pIcon->SetActiveRenderLayer(i_bActive);
	//this->ShowDriverIcons(i_bActive && m_bShowDriverIcons 
	//	&& this->GetLayerPickable() && m_bSelected);
}

//--------------------------------------------------------------------
//  Changes visible state of character 
//--------------------------------------------------------------------
void  trfnScriptObject::SetEditorVisible(bool i_bVisible)
{
	// Transforms don't have a "editor visible" state themselves,
	// but they do pass the visible state to all child nodes.
	m_pIcon->SetEditorVisible(i_bVisible);

	//this->ShowDriverIcons(i_bVisible && m_bShowDriverIcons 
	//	&& this->GetLayerPickable() && m_bSelected);	
	//this->ShowDriverIcons(i_bVisible && m_bShowDriverIcons && m_bSelected);
}

//--------------------------------------------------------------------
//  Returns the visible state of character 
//--------------------------------------------------------------------
bool  trfnScriptObject::GetEditorVisible()
{
	// Transforms don't have a "editor visible" state themselves,
	// so return "true" here
	return true;
	//return m_pIcon->GetEditorVisible();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
trfnTransformObject* trfnScriptObject::GetPickObject() const
{
	return m_pIcon;
}


//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void trfnScriptObject::NotifyDriverChanged()
{
//	trfnOperations::ChangeDriverData(this->GetScriptData());
	trfnDocumentChunk::ActiveDataChanged();
}


//============================================================================
//	Data
//============================================================================


//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
trfnScriptData trfnScriptObject::GetScriptData() const
{
	trfnScriptData data( m_pIcon->GetData() );

	data.m_BaseData.m_Position	= m_pChannelPosition->GetOriginalPosition();
	float x=0,y=0,z=0;
	m_pChannelOrientation->GetOriginalValue(x,y,z);
	data.m_BaseData.m_Orientation.SetEuler(x,y,z);
	data.m_BaseData.m_Scale		= m_pChannelScale->GetOriginalValue();

	// get custom property data
	this->GetCustomPropertyData(data.m_CustomProperties);

	// get driver info
	tmlnCreator::GetDriverInfo( this, (data.m_Drivers) );

	// get the channel info
	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );
		
	// get the connection info
	ltstLightSetMgr::GetLightSetsForObject(data.m_BaseData.m_Name.GetValue(), data.m_ConnectionData.m_LightSets);
	////evmtEnvironmentMgr::GetEnvironmentNameFromObject(data.m_BaseData.m_Name.GetValue(), data.m_ConnectionData.m_EnvironmentName);
	////lyerLayerMgr::GetLayerNameFromObject(data.m_BaseData.m_Name.GetValue(), data.m_ConnectionData.m_LayerName);
	////grpsGroupMgr::GetGroupsForObject(data.m_BaseData.m_Name.GetValue(), data.m_ConnectionData.m_GroupNames);
	//rlyrRenderLayerMgr::GetVisibilityForObject(data.m_BaseData.m_Name.GetValue(), data.m_ConnectionData.m_RLayerNames);

	return data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void trfnScriptObject::SetScriptData(const trfnScriptData &i_Data)
{
	this->SetBaseData(i_Data.m_BaseData);

	//DBG_LOG2( "Set Prop Data %02d (%s)", i_Data.m_Name.GetUID(), i_Data.m_Name.GetString().c_str() );
	//DBG_LOG2( "              %02d (%s)", this->GetName().GetUID(), this->GetName().GetString().c_str() );

	// set custom property data
	this->SetCustomPropertyData(i_Data.m_CustomProperties);
	//
	//this->SetEditorVisible( i_Data.m_BaseData.m_bEditorVisible.GetValue() );

	// set the channels 
	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );

	// create drivers from info
	tmlnCreator::SetDriverInfo( this, (i_Data.m_Drivers) );

	//DBG_LOG2( "Set Transforms Data %02d (%s)", i_Data.m_Name.GetUID(), i_Data.m_Name.GetString().c_str() );
	//DBG_LOG2( "              %02d (%s)", this->GetName().GetUID(), this->GetName().GetString().c_str() );

	// set the connection info, if set
	const int num_lightsets = i_Data.m_ConnectionData.m_LightSets.size();
	for (int l=0; l<num_lightsets; ++l)
	{
		const ltstLightSetObjectData &lset_data = i_Data.m_ConnectionData.m_LightSets[l];
		ltstLightSetMgr::AddObjectToLightSet(lset_data.m_Name, i_Data.m_BaseData.m_Name.GetValue());
	}

	//if(!i_Data.m_ConnectionData.m_RLayerNames.empty())
	//	rlyrRenderLayerMgr::SetVisibilityForObject(i_Data.m_BaseData.m_Name.GetValue(), i_Data.m_ConnectionData.m_RLayerNames);
		
}

//--------------------------------------------------------------------
// Get values as a base data structure
//--------------------------------------------------------------------
trfnData trfnScriptObject::GetBaseData() const
{
	trfnData data		= m_pIcon->GetData();

	data.m_Position		= m_pChannelPosition->GetOriginalPosition();
	float x=0,y=0,z=0;
	m_pChannelOrientation->GetOriginalValue(x,y,z);
	data.m_Orientation.SetEuler(x,y,z);
	data.m_Scale		= m_pChannelScale->GetOriginalValue();

	return data;
}

//--------------------------------------------------------------------
// Set from base data structure
//--------------------------------------------------------------------
void trfnScriptObject::SetBaseData(const trfnData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);

	m_pChannelPosition->SetOriginalPosition(i_Data.m_Position.GetValue());
	float x=0,y=0,z=0;
	i_Data.m_Orientation.GetEuler(x,y,z);
	m_pChannelOrientation->SetOriginalValue(x,y,z);
	m_pChannelScale->SetOriginalValue(i_Data.m_Scale.GetValue());
}


//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelOrientation& trfnScriptObject::ChannelOrientation()
{
	return (*m_pChannelOrientation);
}
tmlnChannelPosition& trfnScriptObject::ChannelPosition()
{
	return (*m_pChannelPosition);
}
tmlnChannelFloat& trfnScriptObject::ChannelScale()
{
	return (*m_pChannelScale);
}
tmlnChannelBoolean& trfnScriptObject::ChannelVisible()
{
	return (*m_pChannelVisible);
}


//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
//void trfnScriptObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
//{	
	//TODO - figure out which relationships a transform node is involved with
	// Update LayerMgr
	//lyerLayerMgr::ObjectRenamed(m_pIcon);
	//evmtEnvironmentMgr::ObjectRenamed(m_pIcon);
	//grpsGroupMgr::ObjectRenamed(m_pIcon);
//}

