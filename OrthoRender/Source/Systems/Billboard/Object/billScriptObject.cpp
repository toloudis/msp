/*****************************************************************************
**  billScriptObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Billboard/Object/billScriptObject.hpp"

#include "Systems/Billboard/Data/billDocumentChunk.hpp"
#include "Systems/Billboard/GUI/billGeomList.hpp"

#include "Support/grps/grpsGroupMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Support/mnm/mnmReferencePosChannel.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelFileName.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

// library
#include "Tool/api3d/api3dBillboard.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Core/geo/geoRayIntersection.hpp"



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
billScriptObject::billScriptObject( api3dBillboard * i_pBillboard )
: m_bShowDriverIcons(true),
	m_bSelected(false)
{
	m_pIcon = new billBillboardObject(i_pBillboard, this);

	// Timeline stuff
	tmlnTimelineMgr::AddObject(this, "Billboard");

	// Layer manager registration
	lyerLayerMgr::AddObject(m_pIcon, this);
	grpsGroupMgr::AddObject(m_pIcon, m_pIcon);

	//
	m_pChannelPos = new tmlnChannelPositionProperty( "Position", m_pIcon->PropertyPosition() );
	this->AddChannel(m_pChannelPos);
	m_pChannelOrientation = new tmlnChannelOrientationProperty( "Orientation", m_pIcon->PropertyOrientation() );
	this->AddChannel(m_pChannelOrientation);
	m_pChannelTexture = new tmlnChannelFileNameProperty( "Texture", m_pIcon->PropertyFileName() );
	this->AddChannel(m_pChannelTexture);
	m_pChannelColor = new tmlnChannelColorProperty( "Color", m_pIcon->PropertyColor() );
	this->AddChannel(m_pChannelColor);
	m_pScaleChannel		= new tmlnChannelFloatProperty("Scale", m_pIcon->PropertyScale());
	this->AddChannel(m_pScaleChannel);

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
std::string billScriptObject::GetTmlnName() const
{
	return m_pIcon->GetName().GetString();
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
	DBG_ASSERT0(m_pIcon != 0, "Icon is NULL");

	billScriptData data(m_pIcon->GetData());

	data.m_BaseData.m_Position	= m_pChannelPos->GetOriginalPosition();

	// get driver info
	tmlnCreator::GetDriverInfo(this, (data.m_Drivers));

	// get the channel info
	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	return data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void billScriptObject::SetScriptData(const billScriptData &i_Data)
{
	SetBaseData( i_Data.m_BaseData );

	// create drivers from info
	tmlnCreator::SetDriverInfo(this, i_Data.m_Drivers);

	// set the channels 
	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );
}


//--------------------------------------------------------------------
// Get values as a light base data structure
//--------------------------------------------------------------------
billData billScriptObject::GetBaseData() const
{
	DBG_ASSERT0(m_pIcon != 0, "Icon is NULL");

	billData data(m_pIcon->GetData());

	data.m_Position		= m_pChannelPos->GetOriginalPosition();
	float x=0,y=0,z=0;
	m_pChannelOrientation->GetOriginalValue(x,y,z);
	data.m_Orientation.SetEuler(x,y,z);
	data.m_Scale		= m_pScaleChannel->GetOriginalValue();
	data.m_Filename		= m_pChannelTexture->GetOriginalValue();
	return data;
}

//--------------------------------------------------------------------
// Set from light base data structure
//--------------------------------------------------------------------
void billScriptObject::SetBaseData(const billData &i_Data)
{
	DBG_ASSERT0(m_pIcon != 0, "Icon is NULL");

	m_pIcon->SetData(i_Data);

	m_pChannelPos->SetOriginalPosition( i_Data.m_Position.GetValue() );
	float x=0,y=0,z=0;
	i_Data.m_Orientation.GetEuler(x,y,z);
	m_pChannelOrientation->SetOriginalValue(x,y,z);
	m_pChannelTexture->SetOriginalValue( i_Data.m_Filename.GetValue() );
	m_pChannelColor->SetOriginalColor( i_Data.m_Color.GetValue() );
	m_pScaleChannel->SetOriginalValue( i_Data.m_Scale.GetValue() );
	
	if (i_Data.m_Filename.GetValue().GetLength() > 0)
	{
		fsLocator bill_dir;
		if (billGeomList::FindFile(i_Data.m_Filename.GetValue(), bill_dir))
		{
			bill_dir.Pop();
			m_pChannelTexture->SetDirectory(bill_dir);
		}
	}
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


//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the propScriptObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool billScriptObject::RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T)
{
	if (!this->GetLayerPickable())
		return false;

	return m_pIcon->RayPick(i_RayStart, i_RayEnd, o_T);
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
//  Changes visible state of prop 
//--------------------------------------------------------------------
void  billScriptObject::SetEditorVisible(bool i_bVisible)
{
	m_pIcon->SetEditorVisible(i_bVisible);
	this->ShowDriverIcons(i_bVisible && m_bShowDriverIcons 
		&& this->GetLayerPickable() && m_bSelected);
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
//bool billScriptObject::IsOperationEnabled(cmpsManipObject::Operations i_Operation, float i_Time)
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
	DBG_ASSERT0(m_pIcon != 0, "Icon is NULL");

	return m_pIcon->GetBillboardObject();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
billBillboardObject* billScriptObject::GetPickObject() const
{
	return m_pIcon;
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
tmlnChannelFileName& billScriptObject::ChannelTexture()
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

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void billScriptObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Update LayerMgr
	lyerLayerMgr::ObjectRenamed(m_pIcon);
	grpsGroupMgr::ObjectRenamed(m_pIcon);
}


