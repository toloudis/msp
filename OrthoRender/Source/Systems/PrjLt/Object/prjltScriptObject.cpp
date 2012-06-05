/*****************************************************************************
**  prjltScriptObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Object/prjltScriptObject.hpp"

#include "Systems/PrjLt/Object/prjltIconObject.hpp"
#include "Systems/PrjLt/Undo/prjltOperations.hpp"
#include "Systems/PrjLt/Object/prjltProjectedLightObject.hpp"
#include "Systems/PrjLt/GUI/prjltTextureList.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Support/grps/grpsGroupMgr.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmReferencePosChannel.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"

namespace
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prjltScriptObject::prjltScriptObject(const prjltScriptData &i_Data)
:	m_bShowDriverIcons(true),
	m_bSelected(false)
{
	m_pIcon = new prjltProjectedLightObject();
	m_pIcon->SetParentObject(this);

	// Timline stuff
	tmlnTimelineMgr::AddObject(this,"Projected Light");

	// Layer manager registration
	lyerLayerMgr::AddObject(m_pIcon, this);
	grpsGroupMgr::AddObject(m_pIcon, m_pIcon);

	//
	m_pPosChannel		= new tmlnChannelPositionProperty("Position", m_pIcon->PropertyPosition());
	this->AddChannel(m_pPosChannel);
	m_pTargetChannel	= new tmlnChannelPositionProperty("Target", m_pIcon->PropertyTarget());
	this->AddChannel(m_pTargetChannel);
	m_pEnableChannel	= new tmlnChannelBooleanProperty("Enabled", m_pIcon->PropertyEnabled());
	this->AddChannel(m_pEnableChannel);
	m_pColorChannel		= new tmlnChannelColorProperty("Color", m_pIcon->PropertyColor());
	this->AddChannel(m_pColorChannel);
	m_pTiltChannel		= new tmlnChannelFloatProperty("Tilt", m_pIcon->PropertyTilt());
	this->AddChannel(m_pTiltChannel);
	m_pRangeChannel		= new tmlnChannelFloatProperty("Range", m_pIcon->PropertyRange());
	this->AddChannel(m_pRangeChannel);
	m_pIntensityChannel	= new tmlnChannelFloatProperty("Intensity", m_pIcon->PropertyIntensity());
	this->AddChannel(m_pIntensityChannel);
	m_pAngleChannel		= new tmlnChannelFloatProperty("Angle", m_pIcon->PropertyAngle());
	this->AddChannel(m_pAngleChannel);
	m_pScaleChannel		= new tmlnChannelFloatProperty("Scale", m_pIcon->PropertyScale());
	this->AddChannel(m_pScaleChannel);
	m_pShadowSourceChannel	= new tmlnChannelBooleanProperty("Shadow Source", m_pIcon->PropertyShadowSource());
	this->AddChannel(m_pShadowSourceChannel);
	m_pAspectChannel	= new tmlnChannelFloatProperty("Aspect", m_pIcon->PropertyAspect());
	this->AddChannel(m_pAspectChannel);
	m_pShadowIntensityChannel	= new tmlnChannelFloatProperty("Shadow Intensity", m_pIcon->PropertyShadowIntensity());
	this->AddChannel(m_pShadowIntensityChannel);
	m_pLightSizeChannel		= new tmlnChannelFloatProperty("Shadow Softness", m_pIcon->PropertyLightSize());
	this->AddChannel(m_pLightSizeChannel);
	m_pDepthMapSizeChannel	= new tmlnChannelInt32Property("Depth Map Size", m_pIcon->PropertyDepthMapSize());
	this->AddChannel(m_pDepthMapSizeChannel);
	m_pDepthBiasChannel	= new tmlnChannelFloatProperty("Depth Bias", m_pIcon->PropertyDepthBias());
	this->AddChannel(m_pDepthBiasChannel);
	m_pShaftVisibleChannel	= new tmlnChannelBooleanProperty("Shaft Visible", m_pIcon->PropertyShaftVisible());
	this->AddChannel(m_pShaftVisibleChannel);
	m_pShaftAlphaChannel	= new tmlnChannelFloatProperty("Shaft Alpha", m_pIcon->PropertyShaftAlpha());
	this->AddChannel(m_pShaftAlphaChannel);
	m_pShaftDensityChannel	= new tmlnChannelFloatProperty("Edge Softness", m_pIcon->PropertyShaftDensity());
	this->AddChannel(m_pShaftDensityChannel);
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
	m_pShaftFalloffStartChannel		= new tmlnChannelFloatProperty("Shaft Falloff Start", m_pIcon->PropertyShaftFalloffStart());
	this->AddChannel(m_pShaftFalloffStartChannel);
	m_pShaftFalloffEndChannel		= new tmlnChannelFloatProperty("Shaft Falloff End", m_pIcon->PropertyShaftFalloffEnd());
	this->AddChannel(m_pShaftFalloffEndChannel);

	// Set starting values from passed in data
	this->SetScriptData(i_Data);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prjltScriptObject::~prjltScriptObject()
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
std::string prjltScriptObject::GetTmlnName() const
{
	return m_pIcon->GetName().GetString();
}


//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void prjltScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
	if (m_pIcon->GetData().m_TextureFilename.GetValue().GetLength() > 0)
		io_List.Add( m_pIcon->GetData().m_TextureFilename.GetValue() );

	//	add all the driver resources as well
	tmlnScriptObject::GetResourceList( io_List );
}

//--------------------------------------------------------------------
// Get values as a light data structure
//--------------------------------------------------------------------
prjltData prjltScriptObject::GetBaseData() const
{
	prjltData data( m_pIcon->GetData() );

	data.m_Color			= m_pColorChannel->GetOriginalColor();
	data.m_Enabled			= m_pEnableChannel->GetOriginalState();
	data.m_Position			= m_pPosChannel->GetOriginalPosition();
	data.m_Target			= m_pTargetChannel->GetOriginalPosition();
	data.m_Tilt				= m_pTiltChannel->GetOriginalValue();
	data.m_Range			= m_pRangeChannel->GetOriginalValue();
	data.m_Intensity		= m_pIntensityChannel->GetOriginalValue();
	data.m_Angle			= m_pAngleChannel->GetOriginalValue();
	data.m_Scale			= m_pScaleChannel->GetOriginalValue();
	data.m_ShadowSource		= m_pShadowSourceChannel->GetOriginalState();
	data.m_Aspect			= m_pAspectChannel->GetOriginalValue();
	data.m_ShadowIntensity	= m_pShadowIntensityChannel->GetOriginalValue();
	data.m_LightSize		= m_pLightSizeChannel->GetOriginalValue();
	data.m_DepthMapSize		= (envType::Int32)m_pDepthMapSizeChannel->GetOriginalValue();
	data.m_DepthBias		= m_pDepthBiasChannel->GetOriginalValue();
	data.m_bShaftVisible	= m_pShaftVisibleChannel->GetOriginalState();
	data.m_ShaftAlpha		= m_pShaftAlphaChannel->GetOriginalValue();
	data.m_ShaftDensity		= m_pShaftDensityChannel->GetOriginalValue();
	data.m_bDiffuseEnabled	= m_pEnableDiffuseChannel->GetOriginalState();
	data.m_bSpecularEnabled	= m_pEnableSpecularChannel->GetOriginalState();
	data.m_bAffectsFur		= m_pEnableFurChannel->GetOriginalState();
	data.m_bAffectsGlow		= m_pAffectsGlowChannel->GetOriginalState();
	data.m_Falloff			= m_pFalloffChannel->GetOriginalPosition();
	data.m_ShaftDistFalloffStart= m_pShaftFalloffStartChannel->GetOriginalValue();
	data.m_ShaftDistFalloffEnd	= m_pShaftFalloffEndChannel->GetOriginalValue();

	return data;
}

//--------------------------------------------------------------------
// Set from light data structure
//--------------------------------------------------------------------
void prjltScriptObject::SetBaseData( const prjltData &i_Data )
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);

	//	call functions for all data that sets the object + data
	//
	m_pPosChannel->SetOriginalPosition(i_Data.m_Position.GetValue());
	m_pTargetChannel->SetOriginalPosition( i_Data.m_Target.GetValue() );
	m_pColorChannel->SetOriginalColor( i_Data.m_Color.GetValue() );
	m_pEnableChannel->SetOriginalState( i_Data.m_Enabled.GetValue() );
	m_pTiltChannel->SetOriginalValue( i_Data.m_Tilt.GetValue() );
	m_pRangeChannel->SetOriginalValue( i_Data.m_Range.GetValue() );
	m_pIntensityChannel->SetOriginalValue( i_Data.m_Intensity.GetValue() );
	m_pAngleChannel->SetOriginalValue( i_Data.m_Angle.GetValue() );
	m_pScaleChannel->SetOriginalValue( i_Data.m_Scale.GetValue() );
	m_pShadowSourceChannel->SetOriginalState( i_Data.m_ShadowSource.GetValue() );
	m_pAspectChannel->SetOriginalValue( i_Data.m_Aspect.GetValue() );
	m_pShadowIntensityChannel->SetOriginalValue( i_Data.m_ShadowIntensity.GetValue() );
	m_pLightSizeChannel->SetOriginalValue( i_Data.m_LightSize.GetValue() );
	m_pDepthMapSizeChannel->SetOriginalValue( (float)i_Data.m_DepthMapSize.GetValue() );
	m_pDepthBiasChannel->SetOriginalValue( i_Data.m_DepthBias.GetValue() );
	m_pShaftVisibleChannel->SetOriginalState( i_Data.m_bShaftVisible.GetValue() );
	m_pShaftAlphaChannel->SetOriginalValue( i_Data.m_ShaftAlpha.GetValue() );
	m_pShaftDensityChannel->SetOriginalValue( i_Data.m_ShaftDensity.GetValue() );
	m_pEnableDiffuseChannel->SetOriginalState( i_Data.m_bDiffuseEnabled.GetValue() );
	m_pEnableSpecularChannel->SetOriginalState( i_Data.m_bSpecularEnabled.GetValue() );
	m_pEnableFurChannel->SetOriginalState( i_Data.m_bAffectsFur.GetValue() );
	m_pAffectsGlowChannel->SetOriginalState( i_Data.m_bAffectsGlow.GetValue() );
	m_pFalloffChannel->SetOriginalPosition( i_Data.m_Falloff.GetValue() );
	m_pShaftFalloffStartChannel->SetOriginalValue( i_Data.m_ShaftDistFalloffStart.GetValue() );
	m_pShaftFalloffEndChannel->SetOriginalValue( i_Data.m_ShaftDistFalloffEnd.GetValue() );
}

//--------------------------------------------------------------------
// Get values as a light data structure
//--------------------------------------------------------------------
prjltScriptData prjltScriptObject::GetScriptData() const
{
	prjltScriptData data( this->GetBaseData() );

	// get driver info
	tmlnCreator::GetDriverInfo(this, (data.m_Drivers));

	// get the channel info
	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	return data;
}

//--------------------------------------------------------------------
// Set from light data structure
//--------------------------------------------------------------------
void prjltScriptObject::SetScriptData( const prjltScriptData &i_Data )
{
	this->SetBaseData(i_Data.m_BaseData);

	// create drivers from info
	tmlnCreator::SetDriverInfo( this, (i_Data.m_Drivers) );

	// set the channels 
	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );
}

//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelPosition& prjltScriptObject::PositionChannel()
{
	return (*m_pPosChannel);
}
tmlnChannelPosition& prjltScriptObject::TargetChannel()
{
	return (*m_pTargetChannel);
}
tmlnChannelBoolean& prjltScriptObject::EnabledChannel()
{
	return (*m_pEnableChannel);
}
tmlnChannelColor& prjltScriptObject::ColorChannel()
{
	return (*m_pColorChannel);
}
tmlnChannelFloat& prjltScriptObject::TiltChannel()
{
	return (*m_pTiltChannel);
}
tmlnChannelFloat& prjltScriptObject::RangeChannel()
{
	return (*m_pRangeChannel);
}
tmlnChannelFloat& prjltScriptObject::IntensityChannel()
{
	return (*m_pIntensityChannel);
}
tmlnChannelFloat& prjltScriptObject::AngleChannel()
{
	return (*m_pAngleChannel);
}
tmlnChannelFloat& prjltScriptObject::ScaleChannel()
{
	return (*m_pScaleChannel);
}
tmlnChannelBoolean& prjltScriptObject::ShadowSourceChannel()
{
	return (*m_pShadowSourceChannel);
}
tmlnChannelFloat& prjltScriptObject::AspectChannel()
{
	return (*m_pAspectChannel);
}
tmlnChannelFloat& prjltScriptObject::ShadowIntensityChannel()
{
	return (*m_pShadowIntensityChannel);
}
tmlnChannelFloat& prjltScriptObject::LightSizeChannel()
{
	return (*m_pLightSizeChannel);
}
tmlnChannelFloat& prjltScriptObject::DepthMapSizeChannel()
{
	return (*m_pDepthMapSizeChannel);
}
tmlnChannelFloat& prjltScriptObject::DepthBiasChannel()
{
	return (*m_pDepthBiasChannel);
}
tmlnChannelBoolean& prjltScriptObject::ShaftVisibleChannel()
{
	return (*m_pShaftVisibleChannel);
}
tmlnChannelFloat& prjltScriptObject::ShaftAlphaChannel()
{
	return (*m_pShaftAlphaChannel);
}
tmlnChannelFloat& prjltScriptObject::ShaftDensityChannel()
{
	return (*m_pShaftDensityChannel);
}
tmlnChannelBoolean& prjltScriptObject::EnableDiffuseChannel()
{
	return (*m_pEnableDiffuseChannel);
}
tmlnChannelBoolean& prjltScriptObject::EnableSpecularChannel()
{
	return (*m_pEnableSpecularChannel);
}
tmlnChannelBoolean& prjltScriptObject::EnableFurChannel()
{
	return (*m_pEnableFurChannel);
}
tmlnChannelBoolean& prjltScriptObject::AffectsGlowChannel()
{
	return (*m_pAffectsGlowChannel);
}
tmlnChannelPosition& prjltScriptObject::FalloffChannel()
{
	return (*m_pFalloffChannel);
}
tmlnChannelFloat& prjltScriptObject::ShaftFalloffStartChannel()
{
	return (*m_pShaftFalloffStartChannel);
}
tmlnChannelFloat& prjltScriptObject::ShaftFalloffEndChannel()
{
	return (*m_pShaftFalloffEndChannel);
}

//--------------------------------------------------------------------
//	Name - passes functions on to pick object
//--------------------------------------------------------------------
void prjltScriptObject::SetName(const nameString& i_Name)
{
    m_pIcon->SetName( i_Name );
}
const nameString& prjltScriptObject::GetName() const
{
	return m_pIcon->GetName();
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the prjltScriptObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool prjltScriptObject::RayPick(	const maPoint3d& i_RayStart,
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
void prjltScriptObject::ShowIcons(bool i_bVisible)
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
void prjltScriptObject::SetSelected(bool i_bSelected)
{
	m_bSelected = i_bSelected;
	bool editor_visible = m_pIcon->GetEditorVisible();
	this->ShowDriverIcons(m_bShowDriverIcons && editor_visible 
		&& this->GetLayerPickable() && m_bSelected);
}

//--------------------------------------------------------------------
//  Changes visible state of light 
//--------------------------------------------------------------------
void  prjltScriptObject::SetEditorVisible(bool i_bVisible)
{
	m_pIcon->SetEditorVisible(i_bVisible);
	this->ShowDriverIcons(i_bVisible && m_bShowDriverIcons 
		&& this->GetLayerPickable() && m_bSelected);
}

//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
//virtual 
void prjltScriptObject::SetLayerVisible(bool i_bVisible)
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
void prjltScriptObject::SetLayerPickable(bool i_bPickable)
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
void prjltScriptObject::SetLayerWireframe(bool i_bWireframe)
{
	// nothing to do for lights
	//m_pIcon->SetWireframe(i_bWireframe);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prjltProjectedLightObject* prjltScriptObject::GetPickObject() const
{
	return m_pIcon;
}

//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void prjltScriptObject::NotifyDriverChanged()
{
	prjltOperations::ChangeDriverData(this->GetScriptData());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prjltScriptObject::ReportMemory(gfFileTxt& i_File)
{
	m_pIcon->ReportMemory(i_File);
}

