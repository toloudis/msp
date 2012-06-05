/*****************************************************************************
**  billScriptObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Billboard/Object/billScriptObject.hpp"

#include "Systems/Billboard/Data/billDocumentChunk.hpp"
#include "Systems/Billboard/GUI/billGeomList.hpp"
#include "Systems/Common/Object/cmmNamedScriptObjectReference.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/grps/grpsGroupMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Support/mnm/mnmReferencePosChannel.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelScale.hpp"
#include "Support/tmln/tmlnChannelFilePath.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnChannelVideo.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

// library
#include "Tool/api3d/api3dBillboard.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Core/geo/geoRayIntersection.hpp"


//============================================================================
//============================================================================
namespace
{
	const float l_SphereRadius = 0.8f;
	const float l_PickRadius = 2.0f;	// pick larger than icon
	const maAxisBox l_SphereBox(-l_SphereRadius, l_SphereRadius,
								-l_SphereRadius, l_SphereRadius,
								-l_SphereRadius, l_SphereRadius);
	const maVector3d l_Zero(0,0,0);
	const maVector3d l_One(1,1,1);
	const maRotation l_NoRot;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
billScriptObject::billScriptObject( api3dBillboard * i_pBillboard,
								std::vector<shared_ptr<camCamera>>& i_CamList)
: m_bShowDriverIcons(true),
	m_bSelected(false)
{
	m_pIcon = new billBillboardObject(i_pBillboard, i_CamList);
	this->SetPropertyObject(*m_pIcon);

	// Timeline stuff
	tmlnTimelineMgr::AddObject(this, "Billboard");

	// Layer manager registration
	lyerLayerMgr::AddObject(m_pIcon, this);
	grpsGroupMgr::AddObject(m_pIcon, m_pIcon);

	//
	m_pChannelPos = new tmlnChannelPositionProperty( m_pIcon->PropertyPosition() );
	this->AddChannel(m_pChannelPos);
	m_pChannelOrientation = new tmlnChannelOrientationProperty( m_pIcon->PropertyOrientation() );
	this->AddChannel(m_pChannelOrientation);
	m_pScaleChannel		= new tmlnChannelFloatProperty( m_pIcon->PropertyScale());
	this->AddChannel(m_pScaleChannel);
	m_pChannelTexture = new tmlnChannelFilePathProperty(  m_pIcon->PropertyFileName() );
	this->AddChannel(m_pChannelTexture);
	m_pChannelColor = new tmlnChannelColorProperty(  m_pIcon->PropertyColor() );
	this->AddChannel(m_pChannelColor);

	m_pBrightnessChannel		= new tmlnChannelFloatProperty( m_pIcon->PropertyBrightness());
	this->AddChannel(m_pBrightnessChannel);
	
	m_pChannelVideo = new tmlnChannelVideoProperty( m_pIcon->PropertyVideo() );		
	this->AddChannel(m_pChannelVideo);
	//m_pChannelTexture->SetOriginalTexture( m_pIcon->GetBaseTexture() );
	m_pNonUniformScaleChannel = new tmlnChannelScaleProperty( m_pIcon->PropertyNonUniformScale());	
	this->AddChannel(m_pNonUniformScaleChannel);	
	m_pChannelVisible = new tmlnChannelBooleanProperty( m_pIcon->PropertyVisible() );
	this->AddChannel(m_pChannelVisible);

	//m_pChannelTexture->SetOriginalTexture( m_pIcon->GetBaseTexture() );

// Add name callback in order to notify the layer manager
	m_pIcon->PropertyName().AddCallback(new prtyCallbackWrapper<billScriptObject>(this, &billScriptObject::NameChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
billScriptObject::~billScriptObject()
{
	// Unregister from Timeline
	tmlnTimelineMgr::RemoveObject(this);

	// Unregister from layers
	lyerLayerMgr::RemoveObject(m_pIcon, this);
	grpsGroupMgr::RemoveObject(m_pIcon, m_pIcon);

	delete m_pIcon;
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string billScriptObject::GetDisplayName() const
{
	return m_pIcon->GetName().GetString();
}


//--------------------------------------------------------------------
//	CreateReferenceToSelf - create a reference that refers to the
//	given object. This reference should be able to refer to 
//  future instances of this object persistent through deletion
//	and recreation through undo.
//--------------------------------------------------------------------
shared_ptr<relObjectReference> billScriptObject::CreateReferenceToSelf()
{
	shared_ptr<relObjectReference> named_reference(new cmmNamedScriptObjectReference<billObjectMgr>(this->GetName()));
	return named_reference;
}

//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void billScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
	io_List.Add( this->GetBaseData().m_Filename.GetValue() );

	//	add all the driver resources as well
	tmlnScriptObject::GetResourceList( io_List );
}


//============================================================================
//	Data
//============================================================================


//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
billScriptData billScriptObject::GetScriptData() const
{
	DBG_ASSERT(m_pIcon != 0, "Icon is NULL");

	billScriptData data(m_pIcon->GetData());

	data.m_BaseData.m_Position	= m_pChannelPos->GetOriginalPosition();

	// get custom property data
	this->GetCustomPropertyData(data.m_CustomProperties);

	// get driver info
	tmlnCreator::GetDriverInfo(this, (data.m_Drivers));

	// get the channel info
	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	// get the connection info
	evmtEnvironmentMgr::GetEnvironmentNameFromObject(data.m_BaseData.m_Name.GetValue(), data.m_ConnectionData.m_EnvironmentName);
	lyerLayerMgr::GetLayerNameFromObject(data.m_BaseData.m_Name.GetValue(), data.m_ConnectionData.m_LayerName);
	grpsGroupMgr::GetGroupsForObject(data.m_BaseData.m_Name.GetValue(), data.m_ConnectionData.m_GroupNames);

	return data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void billScriptObject::SetScriptData(const billScriptData &i_Data)
{
	SetBaseData( i_Data.m_BaseData );

	// set custom property data
	this->SetCustomPropertyData(i_Data.m_CustomProperties);

	// create drivers from info
	tmlnCreator::SetDriverInfo(this, i_Data.m_Drivers);

	// set the channels 
	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );

	// set the connection info, if set
	if (!i_Data.m_ConnectionData.m_EnvironmentName.IsEmpty())
		evmtEnvironmentMgr::AddObjectToEnvironment(i_Data.m_ConnectionData.m_EnvironmentName, i_Data.m_BaseData.m_Name.GetValue());
	if (!i_Data.m_ConnectionData.m_LayerName.IsEmpty())
		lyerLayerMgr::AddObjectToLayer(i_Data.m_ConnectionData.m_LayerName, i_Data.m_BaseData.m_Name.GetValue());

	const int num_groups = i_Data.m_ConnectionData.m_GroupNames.size();
	for (int g=0; g<num_groups; ++g)
		grpsGroupMgr::AddObjectToGroup(i_Data.m_ConnectionData.m_GroupNames[g], i_Data.m_BaseData.m_Name.GetValue());
}


//--------------------------------------------------------------------
// Get values as a light base data structure
//--------------------------------------------------------------------
billData billScriptObject::GetBaseData() const
{
	DBG_ASSERT(m_pIcon != 0, "Icon is NULL");

	billData data(m_pIcon->GetData());

	data.m_Position		= m_pChannelPos->GetOriginalPosition();
	float x=0,y=0,z=0;
	m_pChannelOrientation->GetOriginalValue(x,y,z);
	data.m_Orientation.SetEuler(x,y,z);
	data.m_Scale		= m_pScaleChannel->GetOriginalValue();
	data.m_Brightness	= m_pBrightnessChannel->GetOriginalValue();
	data.m_NonUniformScale = m_pNonUniformScaleChannel->GetOriginalScale();
	data.m_Filename		= m_pChannelTexture->GetOriginalValue();
//	data.m_Video		= m_pChannelVideo->GetOriginalValue();
	return data;
}

//--------------------------------------------------------------------
// Set from light base data structure
//--------------------------------------------------------------------
void billScriptObject::SetBaseData(const billData &i_Data)
{
	DBG_ASSERT(m_pIcon != 0, "Icon is NULL");

	m_pIcon->SetData(i_Data);

	m_pChannelPos->SetOriginalPosition( i_Data.m_Position.GetValue() );
	float x=0,y=0,z=0;
	i_Data.m_Orientation.GetEuler(x,y,z);
	m_pChannelOrientation->SetOriginalValue(x,y,z);
	m_pChannelTexture->SetOriginalValue( i_Data.m_Filename.GetValue() );
	m_pChannelColor->SetOriginalColor( i_Data.m_Color.GetValue() );
	m_pScaleChannel->SetOriginalValue( i_Data.m_Scale.GetValue() );
	m_pBrightnessChannel->SetOriginalValue( i_Data.m_Brightness.GetValue() );
	m_pNonUniformScaleChannel->SetOriginalScale(i_Data.m_NonUniformScale.GetValue());
	// Going to have to figure out way to find filenames from old
	// file formats.
	//if (i_Data.m_Filename.GetValue().GetLength() > 0)
	//{
	//	fsLocator bill_dir;
	//	if (billGeomList::FindFile(i_Data.m_Filename.GetValue(), bill_dir))
	//	{
	//		bill_dir.Pop();
	//		m_pChannelTexture->SetDirectory(bill_dir);
	//	}
	//}
}

//--------------------------------------------------------------------
//	Name - passes functions on to pick object
//--------------------------------------------------------------------
void billScriptObject::SetName(const nameString& i_Name)
{
    m_pIcon->SetName( i_Name );
}
const nameString& billScriptObject::GetName() const
{
	return m_pIcon->GetName();
}


//--------------------------------------------------------------------
//	ShowIcons - show driver icons
//--------------------------------------------------------------------
void billScriptObject::ShowIcons(bool i_bVisible)
{
	m_bShowDriverIcons = i_bVisible;
	bool editor_visible = m_pIcon->GetEditorVisible();
	this->ShowDriverIcons(m_bShowDriverIcons && editor_visible 
		&& this->GetLayerPickable() && m_bSelected);
}

//--------------------------------------------------------------------
//	Set whether this object is selected in order to control
//	display of icons or render style, etc.
//--------------------------------------------------------------------
void billScriptObject::SetSelected(bool i_bSelected)
{
	m_bSelected = i_bSelected;
	bool editor_visible = m_pIcon->GetEditorVisible();
	this->ShowDriverIcons(m_bShowDriverIcons && editor_visible 
		&& this->GetLayerPickable() && m_bSelected);
}

//--------------------------------------------------------------------
// Set object active state in the render layer
//--------------------------------------------------------------------
void billScriptObject::SetActiveRenderLayer(bool i_bActive)
{
	m_pIcon->SetActiveRenderLayer(i_bActive);
}

//--------------------------------------------------------------------
//  Changes visible state of prop 
//--------------------------------------------------------------------
void  billScriptObject::SetEditorVisible(bool i_bVisible)
{
	m_pIcon->SetEditorVisible(i_bVisible);
	this->ShowDriverIcons(i_bVisible && m_bShowDriverIcons 
		&& this->GetLayerPickable() && m_bSelected);
}

//--------------------------------------------------------------------
//  Returns the visible state of prop 
//--------------------------------------------------------------------
bool  billScriptObject::GetEditorVisible()
{
	return m_pIcon->GetEditorVisible();
}

//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
//virtual 
void billScriptObject::SetLayerVisible(bool i_bVisible)
{
	// base function sets private flag
	lyerObject::SetLayerVisible(i_bVisible);

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	m_pIcon->SetLayerVisible(i_bVisible);

}

//--------------------------------------------------------------------
//	LayerPickable represents if objects in the layer can be picked.
//--------------------------------------------------------------------
//virtual 
void billScriptObject::SetLayerPickable(bool i_bPickable)
{
	// base function sets private flag
	lyerObject::SetLayerPickable(i_bPickable);

	// Set GPU pickable flag of object
	m_pIcon->GetBillboardObject()->SetGPUPickable( i_bPickable );

	// driver icons should only be shown when the object is pickable
	this->ShowDriverIcons( m_bShowDriverIcons 
						&& m_pIcon->GetEditorVisible() 
						&& this->GetLayerPickable()
						 && m_bSelected);
}


//--------------------------------------------------------------------
//	LayerWireframe represents if the objects are rendered
//	in a wireframe style.
//--------------------------------------------------------------------
//virtual 
void billScriptObject::SetLayerWireframe(bool i_bWireframe)
{
	m_pIcon->SetWireframe(i_bWireframe);
}


//--------------------------------------------------------------------
// For polling if an manipulation operation is currently enabled
//--------------------------------------------------------------------
//bool billScriptObject::IsOperationEnabled(cmpsManipObject::Operations i_Operation, const maTime& i_Time)
//{
//	switch (i_Operation)
//	{
//	case mnmObject::e_Translate:
//		return (!m_pChannelPos->IsDriverActiveAtTime( i_Time ));
//	case mnmObject::e_Rotate:
//	case mnmObject::e_Scale:
//		return true;
//	}
//	return true;
//}

//--------------------------------------------------------------------
//	Access to billboard object
//--------------------------------------------------------------------
api3dBillboard * billScriptObject::GetBillboardObject()
{
	DBG_ASSERT(m_pIcon != 0, "Icon is NULL");

	return m_pIcon->GetBillboardObject();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
billBillboardObject* billScriptObject::GetPickObject() const
{
	return m_pIcon;
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void billScriptObject::UpdateCameraList(std::vector<shared_ptr<camCamera>>& i_CameraList,
										std::vector<std::string>& i_NameList)
{
	m_pIcon->UpdateCameraList(i_CameraList, i_NameList);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void billScriptObject::DeleteCameraIndex(int i_Index)
{
	m_pIcon->DeleteCameraIndex(i_Index);
}

//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void billScriptObject::NotifyDriverChanged()
{
	billDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelPosition& billScriptObject::ChannelPosition()
{
	return (*m_pChannelPos);
}
tmlnChannelOrientation& billScriptObject::ChannelOrientation()
{
	return (*m_pChannelOrientation);
}
tmlnChannelFilePath& billScriptObject::ChannelTexture()
{
	return (*m_pChannelTexture);
}
tmlnChannelColor& billScriptObject::ChannelColor()
{
	return (*m_pChannelColor);
}
tmlnChannelFloat& billScriptObject::ScaleChannel()
{
	return (*m_pScaleChannel);
}
tmlnChannelFloat& billScriptObject::BrightnessChannel()
{
	return (*m_pBrightnessChannel);
}
tmlnChannelVideo& billScriptObject::ChannelVideo()
{
	return (*m_pChannelVideo);
}
tmlnChannelBoolean& billScriptObject::ChannelVisible()
{
	return (*m_pChannelVisible);
}

tmlnChannelScale& billScriptObject::NonUniformScaleChannel()
{
	return (*m_pNonUniformScaleChannel);
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void billScriptObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Update LayerMgr
	lyerLayerMgr::ObjectRenamed(m_pIcon);
	grpsGroupMgr::ObjectRenamed(m_pIcon);
}

