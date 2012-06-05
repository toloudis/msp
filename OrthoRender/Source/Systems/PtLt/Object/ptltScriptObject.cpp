/*****************************************************************************
**  ptltScriptObject.cpp
**
**      A ptltScriptObject is a derived class for displaying a point
**	light's position.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/Object/ptltScriptObject.hpp"

#include "Systems/PtLt/Undo/ptltOperations.hpp"

#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Support/grps/grpsGroupMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Support/mnm/mnmReferencePosChannel.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ptltScriptObject::ptltScriptObject(const ptltScriptData &i_Data)
:	m_pIcon(NULL),
	m_pPosChannel(NULL),
	m_pEnableChannel(NULL),
	m_pColorChannel(NULL),
	m_bShowDriverIcons(true),
	m_bSelected(false)
{
	m_pIcon	= new ptltPointLightObject();
	m_pIcon->SetParentObject(this);

	// Timeline stuff
	tmlnTimelineMgr::AddObject(this,"Point Light");

	// Layer manager registration
	lyerLayerMgr::AddObject(m_pIcon, this);
	grpsGroupMgr::AddObject(m_pIcon, m_pIcon);

	//
	m_pPosChannel		= new tmlnChannelPositionProperty("Position", m_pIcon->PropertyPosition());
	this->AddChannel(m_pPosChannel);
	m_pEnableChannel	= new tmlnChannelBooleanProperty("Enabled", m_pIcon->PropertyEnabled());
	this->AddChannel(m_pEnableChannel);
	m_pColorChannel		= new tmlnChannelColorProperty("Color", m_pIcon->PropertyColor());
	this->AddChannel(m_pColorChannel);
	m_pRangeChannel		= new tmlnChannelFloatProperty("Range", m_pIcon->PropertyRange());
	this->AddChannel(m_pRangeChannel);
	m_pIntensityChannel	= new tmlnChannelFloatProperty("Intensity", m_pIcon->PropertyIntensity());
	this->AddChannel(m_pIntensityChannel);
	m_pShadowSourceChannel	= new tmlnChannelBooleanProperty("Shadow Source", m_pIcon->PropertyShadowSource());
	this->AddChannel(m_pShadowSourceChannel);
	m_pEnableDiffuseChannel	= new tmlnChannelBooleanProperty("Enable Diffuse", m_pIcon->PropertyEnableDiffuse());
	this->AddChannel(m_pEnableDiffuseChannel);
	m_pEnableSpecularChannel= new tmlnChannelBooleanProperty("Enable Specular", m_pIcon->PropertyEnableSpecular());
	this->AddChannel(m_pEnableSpecularChannel);
	m_pEnableFurChannel		= new tmlnChannelBooleanProperty("Affects Fur", m_pIcon->PropertyEnableFur());
	this->AddChannel(m_pEnableFurChannel);
	m_pAffectsGlowChannel	= new tmlnChannelBooleanProperty("Affects Glow", m_pIcon->PropertyAffectsGlow());
	this->AddChannel(m_pAffectsGlowChannel);
	m_pFalloffChannel		= new tmlnChannelPositionProperty("Falloff", m_pIcon->PropertyFalloff());
	this->AddChannel(m_pFalloffChannel);

	// Set starting values from passed in data
	this->SetScriptData(i_Data);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ptltScriptObject::~ptltScriptObject()
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
std::string ptltScriptObject::GetTmlnName() const
{
	return m_pIcon->GetName().GetString();
}

//--------------------------------------------------------------------
// Get values as a light data structure
//--------------------------------------------------------------------
ptltScriptData ptltScriptObject::GetScriptData() const
{
	ptltScriptData data(this->GetBaseData());

	// get driver info
	tmlnCreator::GetDriverInfo(this, data.m_Drivers);

	// get the channel info
	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	return data;
}

//--------------------------------------------------------------------
// Set from light data structure
//--------------------------------------------------------------------
void ptltScriptObject::SetScriptData( const ptltScriptData &i_Data )
{
	this->SetBaseData( i_Data.m_BaseData );

	// create drivers from info
	tmlnCreator::SetDriverInfo(this, i_Data.m_Drivers);

	// set the channels 
	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );
}

//--------------------------------------------------------------------
// Get values as a light base data structure
//--------------------------------------------------------------------
ptltData ptltScriptObject::GetBaseData() const
{
	ptltData data( m_pIcon->GetData() );

	data.m_Color	= m_pColorChannel->GetOriginalColor();
	data.m_Enabled	= m_pEnableChannel->GetOriginalState();
	data.m_Position	= m_pPosChannel->GetOriginalPosition();
	data.m_Range	= m_pRangeChannel->GetOriginalValue();
	data.m_Intensity= m_pIntensityChannel->GetOriginalValue();
	data.m_ShadowSource		= m_pShadowSourceChannel->GetOriginalState();
	data.m_bDiffuseEnabled	= m_pEnableDiffuseChannel->GetOriginalState();
	data.m_bSpecularEnabled	= m_pEnableSpecularChannel->GetOriginalState();
	data.m_bAffectsFur		= m_pEnableFurChannel->GetOriginalState();
	data.m_bAffectsGlow		= m_pAffectsGlowChannel->GetOriginalState();
	data.m_Falloff			= m_pFalloffChannel->GetOriginalPosition();

	return data;
}

//--------------------------------------------------------------------
// Set from light base data structure
//--------------------------------------------------------------------
void ptltScriptObject::SetBaseData(const ptltData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);

	m_pColorChannel->SetOriginalColor(i_Data.m_Color.GetValue());
	m_pEnableChannel->SetOriginalState(i_Data.m_Enabled.GetValue());
	m_pPosChannel->SetOriginalPosition(i_Data.m_Position.GetValue());
	m_pRangeChannel->SetOriginalValue( i_Data.m_Range.GetValue() );
	m_pIntensityChannel->SetOriginalValue( i_Data.m_Intensity.GetValue() );
	m_pShadowSourceChannel->SetOriginalState( i_Data.m_ShadowSource.GetValue() );
	m_pEnableDiffuseChannel->SetOriginalState( i_Data.m_bDiffuseEnabled.GetValue() );
	m_pEnableSpecularChannel->SetOriginalState( i_Data.m_bSpecularEnabled.GetValue() );
	m_pEnableFurChannel->SetOriginalState( i_Data.m_bAffectsFur.GetValue() );
	m_pAffectsGlowChannel->SetOriginalState( i_Data.m_bAffectsGlow.GetValue() );
	m_pFalloffChannel->SetOriginalPosition( i_Data.m_Falloff.GetValue() );
}

//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelPosition& ptltScriptObject::PositionChannel()
{
	return (*m_pPosChannel);
}
tmlnChannelBoolean& ptltScriptObject::EnabledChannel()
{
	return (*m_pEnableChannel);
}
tmlnChannelColor& ptltScriptObject::ColorChannel()
{
	return (*m_pColorChannel);
}
tmlnChannelFloat& ptltScriptObject::RangeChannel()
{
	return (*m_pRangeChannel);
}
tmlnChannelFloat& ptltScriptObject::IntensityChannel()
{
	return (*m_pIntensityChannel);
}
tmlnChannelBoolean& ptltScriptObject::ShadowSourceChannel()
{
	return (*m_pShadowSourceChannel);
}
tmlnChannelBoolean& ptltScriptObject::EnableDiffuseChannel()
{
	return (*m_pEnableDiffuseChannel);
}
tmlnChannelBoolean& ptltScriptObject::EnableSpecularChannel()
{
	return (*m_pEnableSpecularChannel);
}
tmlnChannelBoolean& ptltScriptObject::EnableFurChannel()
{
	return (*m_pEnableFurChannel);
}
tmlnChannelBoolean& ptltScriptObject::AffectsGlowChannel()
{
	return (*m_pAffectsGlowChannel);
}
tmlnChannelPosition& ptltScriptObject::FalloffChannel()
{
	return (*m_pFalloffChannel);
}

//--------------------------------------------------------------------
//	Name - passes functions on to pick object
//--------------------------------------------------------------------
void ptltScriptObject::SetName(const nameString& i_Name)
{
    m_pIcon->SetName( i_Name );
}
const nameString& ptltScriptObject::GetName() const
{
	return m_pIcon->GetName();
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the ptltScriptObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool ptltScriptObject::RayPick(	const maPoint3d& i_RayStart,
									const maPoint3d& i_RayEnd,
									float& o_T)
{
	if (!this->GetLayerPickable())
		return false;

	return m_pIcon->RayPick(i_RayStart, i_RayEnd, o_T);
}

//--------------------------------------------------------------------
//	ShowIcons - show icons for the light and drivers
//--------------------------------------------------------------------
void ptltScriptObject::ShowIcons(bool i_bVisible)
{
	m_pIcon->SetRenderable(i_bVisible);

	m_bShowDriverIcons = i_bVisible;
	this->ShowDriverIcons(m_bShowDriverIcons 
		&& m_pIcon->GetEditorVisible() 
		&& this->GetLayerPickable()
		&& m_bSelected);
}

//--------------------------------------------------------------------
//	Set whether this object is selected in order to control
//	display of icons or render style, etc.
//--------------------------------------------------------------------
void ptltScriptObject::SetSelected(bool i_bSelected)
{
	m_bSelected = i_bSelected;
	bool editor_visible = m_pIcon->GetEditorVisible();
	this->ShowDriverIcons(m_bShowDriverIcons && editor_visible 
		&& this->GetLayerPickable() && m_bSelected);
}

//--------------------------------------------------------------------
//  Changes visible state of light 
//--------------------------------------------------------------------
void  ptltScriptObject::SetEditorVisible(bool i_bVisible)
{
	m_pIcon->SetEditorVisible(i_bVisible);
	this->ShowDriverIcons(i_bVisible && m_bShowDriverIcons 
		&& this->GetLayerPickable() && m_bSelected);
}

//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
//virtual 
void ptltScriptObject::SetLayerVisible(bool i_bVisible)
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
void ptltScriptObject::SetLayerPickable(bool i_bPickable)
{
	// base function sets private flag
	lyerObject::SetLayerPickable(i_bPickable);

	// Set the pickable state for the icons
	m_pIcon->SetGPUPickable(i_bPickable);

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
void ptltScriptObject::SetLayerWireframe(bool i_bWireframe)
{
	// nothing to do for lights
	//m_pIcon->SetWireframe(i_bWireframe);
}

//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void ptltScriptObject::NotifyDriverChanged()
{
	ptltOperations::ChangeDriverData(this->GetScriptData());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
ptltPointLightObject*	ptltScriptObject::GetPickObject() const
{
	return m_pIcon;
}


