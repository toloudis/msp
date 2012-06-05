/*****************************************************************************
**  prjltScriptObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Object/prjltScriptObject.hpp"

#include "Systems/PrjLt/Object/prjltIconObject.hpp"
#include "Systems/PrjLt/Undo/prjltOperations.hpp"
#include "Systems/PrjLt/Object/prjltProjectedLightObject.hpp"
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"
#include "Systems/PrjLt/Object/prjltRangeIcon.hpp"
#include "Systems/Common/Object/cmmNamedScriptObjectReference.hpp"

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
//#include "Support/tmln/tmlnChannelFilePath.hpp"
#include "Support/tmln/tmlnChannelTextureFileName.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

namespace
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prjltScriptObject::prjltScriptObject(const prjltScriptData &i_Data)
:	m_bShowDriverIcons(true),
	m_bSelected(false)
{
	m_pIcon = new prjltProjectedLightObject(i_Data.m_BaseData.m_LightType);
	this->SetPropertyObject(*m_pIcon);
	m_bPreRangeState = m_pIcon->ShouldShowFalloffIcon();
	// Timline stuff
	tmlnTimelineMgr::AddObject(this,"Projected Light");

	// Layer manager registration
	lyerLayerMgr::AddObject(m_pIcon, this);
	grpsGroupMgr::AddObject(m_pIcon, m_pIcon);

	//
	/*m_pTextureChannel = new tmlnChannelFilePathProperty( m_pIcon->PropertyTextureFileName() );
	this->AddChannel(m_pTextureChannel);*/
	m_pTextureChannel = new tmlnChannelTextureFileNameProperty( m_pIcon->PropertyTextureFileName() );
	this->AddChannel(m_pTextureChannel);
	m_pEnableChannel	= new tmlnChannelBooleanProperty( m_pIcon->PropertyEnabled());
	this->AddChannel(m_pEnableChannel);
	m_pFalloffChannel		= new tmlnChannelPositionProperty(m_pIcon->PropertyFalloff());
	this->AddChannel(m_pFalloffChannel);
	m_pColorChannel		= new tmlnChannelColorProperty( m_pIcon->PropertyColor());
	this->AddChannel(m_pColorChannel);
	m_pIntensityChannel	= new tmlnChannelFloatProperty( m_pIcon->PropertyIntensity());
	this->AddChannel(m_pIntensityChannel);
	m_pAngleChannel		= new tmlnChannelFloatProperty( m_pIcon->PropertyAngle());
	this->AddChannel(m_pAngleChannel);
	m_pPenumbraChannel		= new tmlnChannelFloatProperty( m_pIcon->PropertyPenumbra());
	this->AddChannel(m_pPenumbraChannel);
	m_pScaleChannel		= new tmlnChannelFloatProperty( m_pIcon->PropertyScale());
	this->AddChannel(m_pScaleChannel);
	m_pRangeChannel		= new tmlnChannelFloatProperty( m_pIcon->PropertyRange());
	this->AddChannel(m_pRangeChannel);
	m_pAspectChannel	= new tmlnChannelFloatProperty( m_pIcon->PropertyAspect());
	this->AddChannel(m_pAspectChannel);
	m_pEnableDiffuseChannel	= new tmlnChannelBooleanProperty( m_pIcon->PropertyEnableDiffuse());
	this->AddChannel(m_pEnableDiffuseChannel);
	m_pEnableSpecularChannel= new tmlnChannelBooleanProperty(m_pIcon->PropertyEnableSpecular());
	this->AddChannel(m_pEnableSpecularChannel);
	m_pAffectsGlowChannel	= new tmlnChannelBooleanProperty(m_pIcon->PropertyAffectsGlow());
	this->AddChannel(m_pAffectsGlowChannel);
	m_pDepthMapSizeChannel	= new tmlnChannelInt32Property(m_pIcon->PropertyDepthMapSize());
	this->AddChannel(m_pDepthMapSizeChannel);
	m_pShadowSourceChannel	= new tmlnChannelBooleanProperty( m_pIcon->PropertyShadowSource());
	this->AddChannel(m_pShadowSourceChannel);
	m_pDepthBiasChannel	= new tmlnChannelFloatProperty(m_pIcon->PropertyDepthBias());
	this->AddChannel(m_pDepthBiasChannel);
	m_pLightSizeChannel		= new tmlnChannelFloatProperty(m_pIcon->PropertyLightSize());
	this->AddChannel(m_pLightSizeChannel);
	m_pPCSSAdjustChannel		= new tmlnChannelFloatProperty(m_pIcon->PropertyPCSSAdjust());
	this->AddChannel(m_pPCSSAdjustChannel);
	m_pShadowIntensityChannel	= new tmlnChannelFloatProperty( m_pIcon->PropertyShadowIntensity());
	this->AddChannel(m_pShadowIntensityChannel);
	m_pTiltChannel		= new tmlnChannelFloatProperty( m_pIcon->PropertyTilt());
	this->AddChannel(m_pTiltChannel);
	m_pPosChannel		= new tmlnChannelPositionProperty( m_pIcon->PropertyPosition());
	this->AddChannel(m_pPosChannel);
	m_pTargetChannel	= new tmlnChannelPositionProperty( m_pIcon->PropertyTarget());
	this->AddChannel(m_pTargetChannel);
	m_pOrientationChannel	= new tmlnChannelOrientationProperty( m_pIcon->PropertyOrientation());
	this->AddChannel(m_pOrientationChannel);
	m_pShaftVisibleChannel	= new tmlnChannelBooleanProperty(m_pIcon->PropertyShaftVisible());
	this->AddChannel(m_pShaftVisibleChannel);
	m_pShaftAlphaChannel	= new tmlnChannelFloatProperty(m_pIcon->PropertyShaftAlpha());
	this->AddChannel(m_pShaftAlphaChannel);
	m_pShaftDensityChannel	= new tmlnChannelFloatProperty(m_pIcon->PropertyShaftDensity());
	this->AddChannel(m_pShaftDensityChannel);
	m_pShaftFalloffStartChannel		= new tmlnChannelFloatProperty( m_pIcon->PropertyShaftFalloffStart());
	this->AddChannel(m_pShaftFalloffStartChannel);
	m_pShaftFalloffEndChannel		= new tmlnChannelFloatProperty( m_pIcon->PropertyShaftFalloffEnd());
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
std::string prjltScriptObject::GetDisplayName() const
{
	return m_pIcon->GetName().GetString();
}

//--------------------------------------------------------------------
//	CreateReferenceToSelf - create a reference that refers to the
//	given object. This reference should be able to refer to 
//  future instances of this object persistent through deletion
//	and recreation through undo.
//--------------------------------------------------------------------
shared_ptr<relObjectReference> prjltScriptObject::CreateReferenceToSelf()
{
	shared_ptr<relObjectReference> named_reference(new cmmNamedScriptObjectReference<prjltObjectMgr>(this->GetName()));
	return named_reference;
}

//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void prjltScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
	io_List.Add(nameString(this->GetDisplayName()), true);

	io_List.IncrementCurrentDepth();
	if (m_pIcon->GetData().m_TextureFilename.GetValue().GetNumNames() > 0)
		io_List.Add( m_pIcon->GetData().m_TextureFilename.GetValue() );

	if (m_pIcon->GetData().m_ShaftTextureFilename.GetValue().GetNumNames() > 0)
		io_List.Add( m_pIcon->GetData().m_ShaftTextureFilename.GetValue() );

	//	add all the driver resources as well
	tmlnScriptObject::GetResourceList( io_List );
	io_List.DecrementCurrentDepth();
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

	float x=0,y=0,z=0;
	m_pOrientationChannel->GetOriginalValue(x,y,z);
	data.m_Orientation.SetEuler(x,y,z);

	data.m_Tilt				= m_pTiltChannel->GetOriginalValue();
	data.m_Range			= m_pRangeChannel->GetOriginalValue();
	data.m_Intensity		= m_pIntensityChannel->GetOriginalValue();
	data.m_Angle			= m_pAngleChannel->GetOriginalValue();
	data.m_Penumbra			= m_pPenumbraChannel->GetOriginalValue();
	data.m_Scale			= m_pScaleChannel->GetOriginalValue();
	data.m_ShadowSource		= m_pShadowSourceChannel->GetOriginalState();
	data.m_Aspect			= m_pAspectChannel->GetOriginalValue();
	data.m_ShadowIntensity	= m_pShadowIntensityChannel->GetOriginalValue();
	data.m_LightSize		= m_pLightSizeChannel->GetOriginalValue();
	data.m_PCSSAdjust		= m_pPCSSAdjustChannel->GetOriginalValue();
	data.m_DepthMapSize		= (envType::Int32)m_pDepthMapSizeChannel->GetOriginalValue();
	data.m_DepthBias		= m_pDepthBiasChannel->GetOriginalValue();
	data.m_bShaftVisible	= m_pShaftVisibleChannel->GetOriginalState();
	data.m_ShaftAlpha		= m_pShaftAlphaChannel->GetOriginalValue();
	data.m_ShaftDensity		= m_pShaftDensityChannel->GetOriginalValue();
	data.m_bDiffuseEnabled	= m_pEnableDiffuseChannel->GetOriginalState();
	data.m_bSpecularEnabled	= m_pEnableSpecularChannel->GetOriginalState();
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
	
	float x=0,y=0,z=0;
	i_Data.m_Orientation.GetEuler(x,y,z);
	m_pOrientationChannel->SetOriginalValue(x,y,z);

	m_pColorChannel->SetOriginalColor( i_Data.m_Color.GetValue() );
	m_pEnableChannel->SetOriginalState( i_Data.m_Enabled.GetValue() );
	m_pTiltChannel->SetOriginalValue( i_Data.m_Tilt.GetValue() );
	m_pRangeChannel->SetOriginalValue( i_Data.m_Range.GetValue() );
	m_pIntensityChannel->SetOriginalValue( i_Data.m_Intensity.GetValue() );
	m_pAngleChannel->SetOriginalValue( i_Data.m_Angle.GetValue() );
	m_pPenumbraChannel->SetOriginalValue( i_Data.m_Penumbra.GetValue() );
	m_pScaleChannel->SetOriginalValue( i_Data.m_Scale.GetValue() );
	m_pShadowSourceChannel->SetOriginalState( i_Data.m_ShadowSource.GetValue() );
	m_pAspectChannel->SetOriginalValue( i_Data.m_Aspect.GetValue() );
	m_pShadowIntensityChannel->SetOriginalValue( i_Data.m_ShadowIntensity.GetValue() );
	m_pLightSizeChannel->SetOriginalValue( i_Data.m_LightSize.GetValue() );
	m_pPCSSAdjustChannel->SetOriginalValue( i_Data.m_PCSSAdjust.GetValue() );
	m_pDepthMapSizeChannel->SetOriginalValue( (float)i_Data.m_DepthMapSize.GetValue() );
	m_pDepthBiasChannel->SetOriginalValue( i_Data.m_DepthBias.GetValue() );
	m_pShaftVisibleChannel->SetOriginalState( i_Data.m_bShaftVisible.GetValue() );
	m_pShaftAlphaChannel->SetOriginalValue( i_Data.m_ShaftAlpha.GetValue() );
	m_pShaftDensityChannel->SetOriginalValue( i_Data.m_ShaftDensity.GetValue() );
	m_pEnableDiffuseChannel->SetOriginalState( i_Data.m_bDiffuseEnabled.GetValue() );
	m_pEnableSpecularChannel->SetOriginalState( i_Data.m_bSpecularEnabled.GetValue() );
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

	// get custom property data
	this->GetCustomPropertyData(data.m_CustomProperties);

	// get driver info
	tmlnCreator::GetDriverInfo(this, (data.m_Drivers));

	// get the channel info
	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	// get the connection info
	ltstLightSetMgr::GetSetNameFromLight(data.m_BaseData.m_Name.GetValue(), data.m_ConnectionData.m_LightSetName);
	lyerLayerMgr::GetLayerNameFromObject(data.m_BaseData.m_Name.GetValue(), data.m_ConnectionData.m_LayerName);
	grpsGroupMgr::GetGroupsForObject(data.m_BaseData.m_Name.GetValue(), data.m_ConnectionData.m_GroupNames);

	return data;
}

//--------------------------------------------------------------------
// Set from light data structure
//--------------------------------------------------------------------
void prjltScriptObject::SetScriptData( const prjltScriptData &i_Data )
{
	this->SetBaseData(i_Data.m_BaseData);

	// set custom property data
	this->SetCustomPropertyData(i_Data.m_CustomProperties);

	// create drivers from info
	tmlnCreator::SetDriverInfo( this, (i_Data.m_Drivers) );

	// set the channels 
	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );

	// set the connection info, if set
	if (!i_Data.m_ConnectionData.m_LightSetName.IsEmpty())
		ltstLightSetMgr::AddLightToSet(i_Data.m_ConnectionData.m_LightSetName, i_Data.m_BaseData.m_Name.GetValue());
	if (!i_Data.m_ConnectionData.m_LayerName.IsEmpty())
		lyerLayerMgr::AddObjectToLayer(i_Data.m_ConnectionData.m_LayerName, i_Data.m_BaseData.m_Name.GetValue());

	const int num_groups = i_Data.m_ConnectionData.m_GroupNames.size();
	for (int g=0; g<num_groups; ++g)
		grpsGroupMgr::AddObjectToGroup(i_Data.m_ConnectionData.m_GroupNames[g], i_Data.m_BaseData.m_Name.GetValue());
}

//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelTextureFileName& prjltScriptObject::TextureChannel()
{
	return (*m_pTextureChannel);
}
tmlnChannelPosition& prjltScriptObject::PositionChannel()
{
	return (*m_pPosChannel);
}
tmlnChannelPosition& prjltScriptObject::TargetChannel()
{
	return (*m_pTargetChannel);
}
tmlnChannelOrientation& prjltScriptObject::OrientationChannel()
{
	return (*m_pOrientationChannel);
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
tmlnChannelFloat& prjltScriptObject::PenumbraChannel()
{
	return (*m_pPenumbraChannel);
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
tmlnChannelFloat& prjltScriptObject::PCSSAdjustChannel()
{
	return (*m_pPCSSAdjustChannel);
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


//--------------------------------------------------------------------
//	ShowIcons - show icons for the light and drivers
//--------------------------------------------------------------------
void prjltScriptObject::ShowIcons(bool i_bVisible)
{
	m_pIcon->SetRenderable(i_bVisible);
	if(i_bVisible)
	{
		m_pIcon->GetRangeIcon()->SetRenderable(m_bPreRangeState);
	}
	else
	{
		m_bPreRangeState = m_pIcon->ShouldShowFalloffIcon();
		m_pIcon->GetRangeIcon()->SetRenderable(i_bVisible);
	}
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

	if (m_bSelected)
	{
		m_pIcon->SelectObject();
	}
	else
	{
		m_pIcon->DeSelectObject();
	}
}

//--------------------------------------------------------------------
// Set object active state in the render layer
//--------------------------------------------------------------------
void prjltScriptObject::SetActiveRenderLayer(bool i_bActive)
{
	m_pIcon->SetActiveRenderLayer(i_bActive);
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
//  Returns the visible state of light 
//--------------------------------------------------------------------
bool  prjltScriptObject::GetEditorVisible()
{
	return m_pIcon->GetEditorVisible();
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
	//m_pIcon->SelectObject();
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

