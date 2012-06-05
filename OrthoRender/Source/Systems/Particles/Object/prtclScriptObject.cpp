/*****************************************************************************
**  prtclScriptObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/Object/prtclScriptObject.hpp"

#include "Systems/Particles/Timeline/prtclChannelAnimation.hpp"
#include "Systems/Particles/Timeline/prtclChannelEmit.hpp"
#include "Systems/Particles/Object/prtclObject.hpp"
#include "Systems/Particles/Undo/prtclOperations.hpp"

#include "Support/grps/grpsGroupMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Support/mnm/mnmReferencePosChannel.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelFileName.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

// library
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dParticleGenerator.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoRayIntersection.hpp"


//============================================================================
//============================================================================
namespace
{
	const maVector3d l_One(1,1,1);
}


//----------------------------------------------------------------------------
// Ownership for the generator and the template pass to this object.
//----------------------------------------------------------------------------
prtclScriptObject::prtclScriptObject(	prtParticleGenerator* i_pGenerator,
										prtParticleGeneratorTemplate* i_pTemplate )
:	m_bShowDriverIcons(true),
	m_bSelected(false)
{
	m_pIcon = new prtclObject(i_pGenerator, i_pTemplate);
	m_pIcon->SetParentObject(this);

	// Timeline manager registration
	tmlnTimelineMgr::AddObject(this, "Particle");

	// Layer manager registration
	lyerLayerMgr::AddObject(m_pIcon, this);
	grpsGroupMgr::AddObject(m_pIcon, m_pIcon);

	//	emitter
	m_pChannelEmit = new prtclChannelEmit( "Emit", i_pGenerator );
	this->AddChannel(m_pChannelEmit);

	//	animation
	m_pChannelAnimation = new prtclChannelAnimation( "Animation", i_pGenerator );
	this->AddChannel(m_pChannelAnimation);

	//	Base Generator
	m_pChannelPos = new tmlnChannelPositionProperty( "Position", m_pIcon->PropertyPosition() );
	this->AddChannel(m_pChannelPos);
	m_pOrientationChannel = new tmlnChannelOrientationProperty("Orientation", m_pIcon->PropertyOrientation());
	this->AddChannel(m_pOrientationChannel);
	m_pRateChannel = new tmlnChannelFloatProperty("Rate", m_pIcon->PropertyRate());
	this->AddChannel(m_pRateChannel);
	m_pMaxParticlesChannel = new tmlnChannelFloatProperty("Max Particles", m_pIcon->PropertyMaxParticles());
	this->AddChannel(m_pMaxParticlesChannel);

	//	Sprite generator
	//
	prtParticleGeneratorTemplate::Type gen_type = this->m_pIcon->GetGeneratorType();
	if (   (gen_type == prtParticleGeneratorTemplate::e_Static)
		|| (gen_type == prtParticleGeneratorTemplate::e_Cone)
		|| (gen_type == prtParticleGeneratorTemplate::e_Spiral))
	{
		m_pLifetimeMinChannel = new tmlnChannelFloatProperty("Lifetime Min", m_pIcon->PropertyLifetimeMin());
		this->AddChannel(m_pLifetimeMinChannel);
		m_pLifetimeMaxChannel = new tmlnChannelFloatProperty("Lifetime Max", m_pIcon->PropertyLifetimeMax());
		this->AddChannel(m_pLifetimeMaxChannel);
		m_pPreSimTimeChannel = new tmlnChannelFloatProperty("Pre-Sim Time", m_pIcon->PropertyPreSimTime());
		this->AddChannel(m_pPreSimTimeChannel);
		m_pScaleStartChannel = new tmlnChannelFloatProperty("Scale Start", m_pIcon->PropertyScaleStart());
		this->AddChannel(m_pScaleStartChannel);
		m_pScaleCoefficientChannel = new tmlnChannelFloatProperty("Scale Coefficient", m_pIcon->PropertyScaleCoefficient());
		this->AddChannel(m_pScaleCoefficientChannel);
		m_pStartAngleMinChannel = new tmlnChannelFloatProperty("Start Angle Min", m_pIcon->PropertyStartAngleMin());
		this->AddChannel(m_pStartAngleMinChannel);
		m_pStartAngleMaxChannel = new tmlnChannelFloatProperty("Start Angle Max", m_pIcon->PropertyStartAngleMax());
		this->AddChannel(m_pStartAngleMaxChannel);
		m_pAngularVelocityMinChannel = new tmlnChannelFloatProperty("Angular Velocity Min", m_pIcon->PropertyAngularVelocityMin());
		this->AddChannel(m_pAngularVelocityMinChannel);
		m_pAngularVelocityMaxChannel = new tmlnChannelFloatProperty("Angular Velocity Max", m_pIcon->PropertyAngularVelocityMax());
		this->AddChannel(m_pAngularVelocityMaxChannel);
		m_pAngularAccelerationMinChannel = new tmlnChannelFloatProperty("Angular Acceleration Min", m_pIcon->PropertyAngularAccelerationMin());
		this->AddChannel(m_pAngularAccelerationMinChannel);
		m_pAngularAccelerationMaxChannel = new tmlnChannelFloatProperty("Angular Acceleration Max", m_pIcon->PropertyAngularAccelerationMax());
		this->AddChannel(m_pAngularAccelerationMaxChannel);
		m_pEmitterScaleChannel = new tmlnChannelFloatProperty("Emitter Scale", m_pIcon->PropertyEmitterScale());
		this->AddChannel(m_pEmitterScaleChannel);

		//	Texture Alpha
		m_pTextureAlphaStartChannel = new tmlnChannelFloatProperty("TextureAlphaStart", m_pIcon->PropertyTextureAlphaStart());
		this->AddChannel(m_pTextureAlphaStartChannel);
		m_pTextureAlphaMiddleChannel = new tmlnChannelFloatProperty("TextureAlphaMiddle", m_pIcon->PropertyTextureAlphaMiddle());
		this->AddChannel(m_pTextureAlphaMiddleChannel);
		m_pTextureAlphaEndChannel = new tmlnChannelFloatProperty("TextureAlphaEnd", m_pIcon->PropertyTextureAlphaEnd());
		this->AddChannel(m_pTextureAlphaEndChannel);
		m_pTextureAlphaMiddlePercentStartChannel = new tmlnChannelFloatProperty("TextureAlphaMiddlePercentStart", m_pIcon->PropertyTextureAlphaMiddlePercentStart());
		this->AddChannel(m_pTextureAlphaMiddlePercentStartChannel);
		m_pTextureAlphaMiddlePercentEndChannel = new tmlnChannelFloatProperty("TextureAlphaMiddlePercentEnd", m_pIcon->PropertyTextureAlphaMiddlePercentEnd());
		this->AddChannel(m_pTextureAlphaMiddlePercentEndChannel);

		m_pTextureFilenameChannel = new tmlnChannelFileNameProperty("TextureFileName", m_pIcon->PropertyTextureFilename());
		this->AddChannel(m_pTextureFilenameChannel);

		// Streaking
		m_pRenderStreaksChannel = new tmlnChannelBooleanProperty("Render Streaks", m_pIcon->PropertyRenderStreaks());
		this->AddChannel(m_pRenderStreaksChannel);
		m_pStreakLengthChannel = new tmlnChannelFloatProperty("Streak Length", m_pIcon->PropertyStreakLength());
		this->AddChannel(m_pStreakLengthChannel);
		m_pStreakTaperChannel = new tmlnChannelFloatProperty("Streak Taper", m_pIcon->PropertyStreakTaper());
		this->AddChannel(m_pStreakTaperChannel);
		m_pStreakFadeChannel = new tmlnChannelFloatProperty("Streak Fade", m_pIcon->PropertyStreakFade());
		this->AddChannel(m_pStreakFadeChannel);

		// Rendering / Shadows
		m_pShowInCubeReflectionsChannel = new tmlnChannelBooleanProperty("", m_pIcon->PropertyShowInCubeReflections());
		this->AddChannel(m_pShowInCubeReflectionsChannel);
		m_pShowInPlanarReflectionsChannel = new tmlnChannelBooleanProperty("", m_pIcon->PropertyShowInPlanarReflections());
		this->AddChannel(m_pShowInPlanarReflectionsChannel);
		m_pCastShadowsChannel = new tmlnChannelBooleanProperty("", m_pIcon->PropertyCastShadows());
		this->AddChannel(m_pCastShadowsChannel);
		m_pUseDitheredShadowsChannel = new tmlnChannelBooleanProperty("", m_pIcon->PropertyUseDitheredShadows());
		this->AddChannel(m_pUseDitheredShadowsChannel);
		m_pShadowDitherBiasChannel = new tmlnChannelFloatProperty("", m_pIcon->PropertyShadowDitherBias());
		this->AddChannel(m_pShadowDitherBiasChannel);
		m_pAdditiveChannel = new tmlnChannelBooleanProperty("", m_pIcon->PropertyAdditive());
		this->AddChannel(m_pAdditiveChannel);
	}

	if (gen_type == prtParticleGeneratorTemplate::e_Cone)
	{
		//	Cone Generator
		//
		m_pConeAngleChannel = new tmlnChannelFloatProperty("ConeAngle", m_pIcon->PropertyConeAngle());
		this->AddChannel(m_pConeAngleChannel);
		m_pMinSpeedChannel = new tmlnChannelFloatProperty("MinSpeed", m_pIcon->PropertyMinSpeed());
		this->AddChannel(m_pMinSpeedChannel);
		m_pMaxSpeedChannel = new tmlnChannelFloatProperty("MaxSpeed", m_pIcon->PropertyMaxSpeed());
		this->AddChannel(m_pMaxSpeedChannel);
		m_pAccelerationXChannel = new tmlnChannelFloatProperty("AccelerationX", m_pIcon->PropertyAccelerationX());
		this->AddChannel(m_pAccelerationXChannel);
		m_pAccelerationYChannel = new tmlnChannelFloatProperty("AccelerationY", m_pIcon->PropertyAccelerationY());
		this->AddChannel(m_pAccelerationYChannel);
		m_pAccelerationZChannel = new tmlnChannelFloatProperty("AccelerationZ", m_pIcon->PropertyAccelerationZ());
		this->AddChannel(m_pAccelerationZChannel);
	}
	else if (gen_type == prtParticleGeneratorTemplate::e_Spiral)
	{
		//	Spiral Generator
		//
		m_pMinEmitSpeedChannel = new tmlnChannelFloatProperty("MinEmitSpeed", m_pIcon->PropertyMinEmitSpeed());
		this->AddChannel(m_pMinEmitSpeedChannel);
		m_pMaxEmitSpeedChannel = new tmlnChannelFloatProperty("MaxEmitSpeed", m_pIcon->PropertyMaxEmitSpeed());
		this->AddChannel(m_pMaxEmitSpeedChannel);
		m_pEmitDirectionXChannel = new tmlnChannelFloatProperty("EmitDirectionX", m_pIcon->PropertyEmitDirectionX());
		this->AddChannel(m_pEmitDirectionXChannel);
		m_pEmitDirectionYChannel = new tmlnChannelFloatProperty("EmitDirectionY", m_pIcon->PropertyEmitDirectionY());
		this->AddChannel(m_pEmitDirectionYChannel);
		m_pEmitDirectionZChannel = new tmlnChannelFloatProperty("EmitDirectionZ", m_pIcon->PropertyEmitDirectionZ());
		this->AddChannel(m_pEmitDirectionZChannel);
		m_pMinRotStartAngleChannel = new tmlnChannelFloatProperty("MinRotStartAngle", m_pIcon->PropertyMinRotStartAngle());
		this->AddChannel(m_pMinRotStartAngleChannel);
		m_pMaxRotStartAngleChannel = new tmlnChannelFloatProperty("MaxRotStartAngle", m_pIcon->PropertyMaxRotStartAngle());
		this->AddChannel(m_pMaxRotStartAngleChannel);
		m_pMinRotAngularVelChannel = new tmlnChannelFloatProperty("MinRotAngularVel", m_pIcon->PropertyMinRotAngularVel());
		this->AddChannel(m_pMinRotAngularVelChannel);
		m_pMaxRotAngularVelChannel = new tmlnChannelFloatProperty("MaxRotAngularVel", m_pIcon->PropertyMaxRotAngularVel());
		this->AddChannel(m_pMaxRotAngularVelChannel);
		m_pRotRadiusChannel = new tmlnChannelFloatProperty("RotRadius", m_pIcon->PropertyRotRadius());
		this->AddChannel(m_pRotRadiusChannel);
		m_pRotRadiusScaleRateChannel = new tmlnChannelFloatProperty("RotRadiusScaleRate", m_pIcon->PropertyRotRadiusScaleRate());
		this->AddChannel(m_pRotRadiusScaleRateChannel);
		m_pAccelerationXChannel = new tmlnChannelFloatProperty("AccelerationX", m_pIcon->PropertyAccelerationX());
		this->AddChannel(m_pAccelerationXChannel);
		m_pAccelerationYChannel = new tmlnChannelFloatProperty("AccelerationY", m_pIcon->PropertyAccelerationY());
		this->AddChannel(m_pAccelerationYChannel);
		m_pAccelerationZChannel = new tmlnChannelFloatProperty("AccelerationZ", m_pIcon->PropertyAccelerationZ());
		this->AddChannel(m_pAccelerationZChannel);
	}

	// Add name callback in order to notify the layer manager
	m_pIcon->PropertyName().AddCallback(new prtyCallbackWrapper<prtclScriptObject>(this, &prtclScriptObject::NameChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtclScriptObject::~prtclScriptObject()
{
	// Timeline stuff
	tmlnTimelineMgr::RemoveObject(this);

	// Unregister from layers
	lyerLayerMgr::RemoveObject(m_pIcon, this);
	grpsGroupMgr::RemoveObject(m_pIcon, m_pIcon);

	delete m_pIcon;
}

//--------------------------------------------------------------------
// Clear out already created particles
//--------------------------------------------------------------------
void prtclScriptObject::ClearParticles()
{
	m_pIcon->ClearParticles();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclScriptObject::Pause(bool i_bPause)
{
	m_pIcon->Pause( i_bPause );
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string prtclScriptObject::GetTmlnName() const
{
	return m_pIcon->GetName().GetString();
}



//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void prtclScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
	io_List.Add( this->GetBaseData().m_Filename.GetValue() );

	//	add all the driver resources as well
	tmlnScriptObject::GetResourceList( io_List );
}

//--------------------------------------------------------------------
//	Name - simply pass functions to icon object
//--------------------------------------------------------------------
void prtclScriptObject::SetName(const nameString& i_Name)
{
    m_pIcon->SetName( i_Name );
}
const nameString& prtclScriptObject::GetName() const
{
	return m_pIcon->GetName();
}


//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the prtclScriptObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool prtclScriptObject::RayPick(	const maPoint3d& i_RayStart,
									const maPoint3d& i_RayEnd,
									float& o_T)
{
	if (!this->GetLayerPickable())
		return false;

	return m_pIcon->RayPick( i_RayStart, i_RayEnd, o_T );
}

//--------------------------------------------------------------------
//	ShowIcons sets whether the driver icons are visible
//--------------------------------------------------------------------
void prtclScriptObject::ShowIcons(bool i_bVisible)
{
	m_bShowDriverIcons = i_bVisible;
	m_pIcon->ShowIcons(i_bVisible);

	this->ShowDriverIcons(	   m_bShowDriverIcons 
							&& m_pIcon->GetEditorVisible() 
							&& this->GetLayerPickable()
							&& m_bSelected);
}

//--------------------------------------------------------------------
//	Set whether this object is selected in order to control
//	display of icons or render style, etc.
//--------------------------------------------------------------------
void prtclScriptObject::SetSelected(bool i_bSelected)
{
	m_bSelected = i_bSelected;
	this->ShowDriverIcons(	   m_bShowDriverIcons 
							&& m_pIcon->GetEditorVisible() 
							&& this->GetLayerPickable()
							&& m_bSelected);
}

//--------------------------------------------------------------------
//  Changes visible state of character 
//--------------------------------------------------------------------
void  prtclScriptObject::SetEditorVisible(bool i_bVisible)
{
	m_pIcon->SetEditorVisible(i_bVisible);
	this->ShowDriverIcons(	   m_bShowDriverIcons 
							&& m_pIcon->GetEditorVisible() 
							&& this->GetLayerPickable()
							&& m_bSelected);
}

//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
//virtual 
void prtclScriptObject::SetLayerVisible(bool i_bVisible)
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
void prtclScriptObject::SetLayerPickable(bool i_bPickable)
{
	// base function sets private flag
	lyerObject::SetLayerPickable(i_bPickable);

	// Set the pickable state for the icons
	m_pIcon->SetGPUPickable(i_bPickable);

	// driver icons should only be shown when the object is pickable
	this->ShowDriverIcons(	   m_bShowDriverIcons 
							&& m_pIcon->GetEditorVisible() 
							&& this->GetLayerPickable()
							&& m_bSelected);
}

//--------------------------------------------------------------------
//	LayerWireframe represents if the objects are rendered
//	in a wireframe style.
//--------------------------------------------------------------------
//virtual 
void prtclScriptObject::SetLayerWireframe(bool i_bWireframe)
{
	m_pIcon->SetWireframe(i_bWireframe);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtclObject* prtclScriptObject::GetPickObject() const
{
	return m_pIcon;
}

//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void prtclScriptObject::NotifyDriverChanged()
{
	prtclOperations::ChangeDriverData(this->GetScriptData());
}


//============================================================================
//	Data
//============================================================================

//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
prtclScriptData prtclScriptObject::GetScriptData() const
{
	prtclScriptData data(m_pIcon->GetData());

	data.m_BaseData.m_Position		= m_pChannelPos->GetOriginalPosition();
//	data.m_BaseData.m_Orientation	= m_pChannelOrientation->GetOriginalOrientation();

	// get driver info
	tmlnCreator::GetDriverInfo( this, (data.m_Drivers) );

	// get the channel info
	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	return data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void prtclScriptObject::SetScriptData(const prtclScriptData &i_Data)
{
	this->SetBaseData(i_Data.m_BaseData);

	prtParticleGeneratorTemplate::Type gen_type = this->m_pIcon->GetGeneratorType();
	//prtParticleGeneratorTemplate::EmitterType emit_type = this->m_pIcon->GetEmitterType();

	// create drivers from info
	tmlnCreator::SetDriverInfo( this, (i_Data.m_Drivers) );

	// set the channels 
	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );
}


//--------------------------------------------------------------------
// Get values as a base data structure
//--------------------------------------------------------------------
prtclData prtclScriptObject::GetBaseData() const
{
	prtclData data		= m_pIcon->GetData();

	data.m_Position		= m_pChannelPos->GetOriginalPosition();			// base generator
	float x=0,y=0,z=0;
	m_pOrientationChannel->GetOriginalValue(x,y,z);
	data.m_Orientation.SetEuler(x,y,z);
	data.m_Rate			= m_pRateChannel->GetOriginalValue();
	prtParticleGeneratorTemplate::Type gen_type = this->m_pIcon->GetGeneratorType();
	if (   (gen_type == prtParticleGeneratorTemplate::e_Static)
		|| (gen_type == prtParticleGeneratorTemplate::e_Cone)
		|| (gen_type == prtParticleGeneratorTemplate::e_Spiral))
	{
		data.m_MaxParticles		= m_pMaxParticlesChannel->GetOriginalValue();	// sprite generator
		data.m_LifetimeMin		= m_pLifetimeMinChannel->GetOriginalValue();
		data.m_LifetimeMax		= m_pLifetimeMaxChannel->GetOriginalValue();
		data.m_PreSimTime		= m_pPreSimTimeChannel->GetOriginalValue();
		data.m_ScaleStart		= m_pScaleStartChannel->GetOriginalValue();
		data.m_ScaleCoefficient	= m_pScaleCoefficientChannel->GetOriginalValue();
		data.m_StartAngleMin	= m_pStartAngleMinChannel->GetOriginalValue();
		data.m_StartAngleMax	= m_pStartAngleMaxChannel->GetOriginalValue();
		data.m_AngularVelocityMax		= m_pAngularVelocityMaxChannel->GetOriginalValue();
		data.m_AngularVelocityMin		= m_pAngularVelocityMinChannel->GetOriginalValue();
		data.m_AngularAccelerationMax	= m_pAngularAccelerationMaxChannel->GetOriginalValue();
		data.m_AngularAccelerationMin	= m_pAngularAccelerationMinChannel->GetOriginalValue();

		data.m_TextureAlphaStart	= m_pTextureAlphaStartChannel->GetOriginalValue();
		data.m_TextureAlphaMiddle	= m_pTextureAlphaMiddleChannel->GetOriginalValue();
		data.m_TextureAlphaEnd		= m_pTextureAlphaEndChannel->GetOriginalValue();
		data.m_TextureAlphaMiddlePercentStart	= m_pTextureAlphaMiddlePercentStartChannel->GetOriginalValue();
		data.m_TextureAlphaMiddlePercentEnd	= m_pTextureAlphaMiddlePercentEndChannel->GetOriginalValue();
		
		data.m_TextureFilename.SetValue( m_pTextureFilenameChannel->GetOriginalValue() );

		data.m_bRenderStreaks.SetValue( m_pRenderStreaksChannel->GetOriginalState() );
		data.m_StreakLength.SetValue( m_pStreakLengthChannel->GetOriginalValue() );
		data.m_StreakTaper.SetValue( m_pStreakTaperChannel->GetOriginalValue() );
		data.m_StreakFade.SetValue( m_pStreakFadeChannel->GetOriginalValue() );

		data.m_bShowInCubeReflections.SetValue( m_pShowInCubeReflectionsChannel->GetOriginalState() );
		data.m_bShowInPlanarReflections.SetValue( m_pShowInPlanarReflectionsChannel->GetOriginalState() );
		data.m_bCastShadows.SetValue( m_pCastShadowsChannel->GetOriginalState() );
		data.m_bUseDitheredShadows.SetValue( m_pUseDitheredShadowsChannel->GetOriginalState() );
		data.m_ShadowDitherBias.SetValue( m_pShadowDitherBiasChannel->GetOriginalValue() );
		data.m_bAdditive.SetValue( m_pAdditiveChannel->GetOriginalState() );
	}

	data.m_EmitterScale		= m_pEmitterScaleChannel->GetOriginalValue();

	if (gen_type == prtParticleGeneratorTemplate::e_Cone)
	{
		data.m_ConeAngle		= m_pConeAngleChannel->GetOriginalValue();	// prtConeParticleGenerator
		data.m_MinSpeed			= m_pMinSpeedChannel->GetOriginalValue();
		data.m_MaxSpeed			= m_pMaxSpeedChannel->GetOriginalValue();
		data.m_AccelerationX	= m_pAccelerationXChannel->GetOriginalValue();
		data.m_AccelerationY	= m_pAccelerationYChannel->GetOriginalValue();
		data.m_AccelerationZ	= m_pAccelerationZChannel->GetOriginalValue();
	}
	else if (gen_type == prtParticleGeneratorTemplate::e_Spiral)
	{
		data.m_MinEmitSpeed		= m_pMinEmitSpeedChannel->GetOriginalValue();	// prtSpiralParticleGenerator
		data.m_MaxEmitSpeed		= m_pMaxEmitSpeedChannel->GetOriginalValue();
		data.m_EmitDirectionX	= m_pEmitDirectionXChannel->GetOriginalValue();
		data.m_EmitDirectionY	= m_pEmitDirectionYChannel->GetOriginalValue();
		data.m_EmitDirectionZ	= m_pEmitDirectionZChannel->GetOriginalValue();
		data.m_MinRotStartAngle = m_pMinRotStartAngleChannel->GetOriginalValue();
		data.m_MaxRotStartAngle = m_pMaxRotStartAngleChannel->GetOriginalValue();
		data.m_MinRotAngularVel = m_pMinRotAngularVelChannel->GetOriginalValue();
		data.m_MaxRotAngularVel = m_pMaxRotAngularVelChannel->GetOriginalValue();
		data.m_RotRadius		= m_pRotRadiusChannel->GetOriginalValue();
		data.m_RotRadiusScaleRate	= m_pRotRadiusScaleRateChannel->GetOriginalValue();
		data.m_AccelerationX	= m_pAccelerationXChannel->GetOriginalValue();
		data.m_AccelerationY	= m_pAccelerationYChannel->GetOriginalValue();
		data.m_AccelerationZ	= m_pAccelerationZChannel->GetOriginalValue();
	}

	return data;
}

//--------------------------------------------------------------------
// Set from base data structure
//--------------------------------------------------------------------
void prtclScriptObject::SetBaseData(const prtclData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);

	// Since the above line causes the Icon object to register
	// the name, the icon's name is the definitive name now,
	// not the one in the i_Data struct. By calling just the
	// nameObject::SetName function here, we avoid setting the
	// icon's name again
//	nameObject::SetName(m_pIcon->GetName());

	m_pChannelPos->SetOriginalPosition(i_Data.m_Position.GetValue());
	float x=0,y=0,z=0;
	i_Data.m_Orientation.GetEuler(x,y,z);
	m_pOrientationChannel->SetOriginalValue(x,y,z);
	m_pRateChannel->SetOriginalValue(i_Data.m_Rate.GetValue());

	prtParticleGeneratorTemplate::Type gen_type = this->m_pIcon->GetGeneratorType();
	if (   (gen_type == prtParticleGeneratorTemplate::e_Static)
		|| (gen_type == prtParticleGeneratorTemplate::e_Cone)
		|| (gen_type == prtParticleGeneratorTemplate::e_Spiral))
	{
		m_pMaxParticlesChannel->SetOriginalValue(i_Data.m_MaxParticles.GetValue());	// sprite generator
		m_pLifetimeMinChannel->SetOriginalValue(i_Data.m_LifetimeMin.GetValue());
		m_pLifetimeMaxChannel->SetOriginalValue(i_Data.m_LifetimeMax.GetValue());
		m_pPreSimTimeChannel->SetOriginalValue(i_Data.m_PreSimTime.GetValue());
		m_pScaleStartChannel->SetOriginalValue(i_Data.m_ScaleStart.GetValue());
		m_pScaleCoefficientChannel->SetOriginalValue(i_Data.m_ScaleCoefficient.GetValue());
		m_pStartAngleMinChannel->SetOriginalValue(i_Data.m_StartAngleMin.GetValue());
		m_pStartAngleMaxChannel->SetOriginalValue(i_Data.m_StartAngleMax.GetValue());
		m_pAngularVelocityMaxChannel->SetOriginalValue(i_Data.m_AngularVelocityMax.GetValue());
		m_pAngularVelocityMinChannel->SetOriginalValue(i_Data.m_AngularVelocityMin.GetValue());
		m_pAngularAccelerationMaxChannel->SetOriginalValue(i_Data.m_AngularAccelerationMax.GetValue());
		m_pAngularAccelerationMinChannel->SetOriginalValue(i_Data.m_AngularAccelerationMin.GetValue());

		m_pTextureAlphaStartChannel->SetOriginalValue( i_Data.m_TextureAlphaStart.GetValue() );
		m_pTextureAlphaMiddleChannel->SetOriginalValue( i_Data.m_TextureAlphaMiddle.GetValue() );
		m_pTextureAlphaEndChannel->SetOriginalValue( i_Data.m_TextureAlphaEnd.GetValue() );
		m_pTextureAlphaMiddlePercentStartChannel->SetOriginalValue( i_Data.m_TextureAlphaMiddlePercentStart.GetValue() );
		m_pTextureAlphaMiddlePercentEndChannel->SetOriginalValue( i_Data.m_TextureAlphaMiddlePercentEnd.GetValue() );

		m_pTextureFilenameChannel->SetOriginalValue( i_Data.m_TextureFilename.GetValue() );

		m_pRenderStreaksChannel->SetOriginalState( i_Data.m_bRenderStreaks.GetValue() );
		m_pStreakLengthChannel->SetOriginalValue( i_Data.m_StreakLength.GetValue() );
		m_pStreakTaperChannel->SetOriginalValue( i_Data.m_StreakTaper.GetValue() );
		m_pStreakFadeChannel->SetOriginalValue( i_Data.m_StreakFade.GetValue() );

		m_pShowInCubeReflectionsChannel->SetOriginalState( i_Data.m_bShowInCubeReflections.GetValue() );
		m_pShowInPlanarReflectionsChannel->SetOriginalState( i_Data.m_bShowInPlanarReflections.GetValue() );
		m_pCastShadowsChannel->SetOriginalState( i_Data.m_bCastShadows.GetValue() );
		m_pUseDitheredShadowsChannel->SetOriginalState( i_Data.m_bUseDitheredShadows.GetValue() );
		m_pShadowDitherBiasChannel->SetOriginalValue( i_Data.m_ShadowDitherBias.GetValue() );
		m_pAdditiveChannel->SetOriginalState( i_Data.m_bAdditive.GetValue() );
	}

	m_pEmitterScaleChannel->SetOriginalValue(i_Data.m_EmitterScale.GetValue());

	if (gen_type == prtParticleGeneratorTemplate::e_Cone)
	{
		m_pConeAngleChannel->SetOriginalValue(i_Data.m_ConeAngle.GetValue());	// prtConeParticleGenerator
		m_pMinSpeedChannel->SetOriginalValue(i_Data.m_MinSpeed.GetValue());
		m_pMaxSpeedChannel->SetOriginalValue(i_Data.m_MaxSpeed.GetValue());
		m_pAccelerationXChannel->SetOriginalValue(i_Data.m_AccelerationX.GetValue());
		m_pAccelerationYChannel->SetOriginalValue(i_Data.m_AccelerationY.GetValue());
		m_pAccelerationZChannel->SetOriginalValue(i_Data.m_AccelerationZ.GetValue());
	}
	else if (gen_type == prtParticleGeneratorTemplate::e_Spiral)
	{
		m_pMinEmitSpeedChannel->SetOriginalValue(i_Data.m_MinEmitSpeed.GetValue());	// prtSpiralParticleGenerator
		m_pMaxEmitSpeedChannel->SetOriginalValue(i_Data.m_MaxEmitSpeed.GetValue());
		m_pEmitDirectionXChannel->SetOriginalValue(i_Data.m_EmitDirectionX.GetValue());
		m_pEmitDirectionYChannel->SetOriginalValue(i_Data.m_EmitDirectionY.GetValue());
		m_pEmitDirectionZChannel->SetOriginalValue(i_Data.m_EmitDirectionZ.GetValue());
		m_pMinRotStartAngleChannel->SetOriginalValue(i_Data.m_MinRotStartAngle.GetValue());
		m_pMaxRotStartAngleChannel->SetOriginalValue(i_Data.m_MaxRotStartAngle.GetValue());
		m_pMinRotAngularVelChannel->SetOriginalValue(i_Data.m_MinRotAngularVel.GetValue());
		m_pMaxRotAngularVelChannel->SetOriginalValue(i_Data.m_MaxRotAngularVel.GetValue());
		m_pRotRadiusChannel->SetOriginalValue(i_Data.m_RotRadius.GetValue());
		m_pRotRadiusScaleRateChannel->SetOriginalValue(i_Data.m_RotRadiusScaleRate.GetValue());
		m_pAccelerationXChannel->SetOriginalValue(i_Data.m_AccelerationX.GetValue());
		m_pAccelerationYChannel->SetOriginalValue(i_Data.m_AccelerationY.GetValue());
		m_pAccelerationZChannel->SetOriginalValue(i_Data.m_AccelerationZ.GetValue());
	}
}

//--------------------------------------------------------------------
//	Update the data with the generator and template
//--------------------------------------------------------------------
void prtclScriptObject::UpdateData()
{
	this->m_pIcon->UpdateData();
}

//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
prtclChannelEmit& prtclScriptObject::ChannelEmit()			// emitter
{
	return (*m_pChannelEmit);
}
prtclChannelAnimation& prtclScriptObject::ChannelAnimation()	// animation
{
	return (*m_pChannelAnimation);
}
tmlnChannelPosition& prtclScriptObject::ChannelPos()		// base generator
{
	return (*m_pChannelPos);
}
tmlnChannelOrientation& prtclScriptObject::OrientationChannel()
{
	return (*m_pOrientationChannel);
}
tmlnChannelFloat& prtclScriptObject::RateChannel()
{
	return (*m_pRateChannel);
}
tmlnChannelFloat& prtclScriptObject::MaxParticlesChannel()	// sprite generator
{
	return (*m_pMaxParticlesChannel);
}
tmlnChannelFloat& prtclScriptObject::LifetimeMinChannel()
{
	return (*m_pLifetimeMinChannel);
}
tmlnChannelFloat& prtclScriptObject::LifetimeMaxChannel()
{
	return (*m_pLifetimeMaxChannel);
}
tmlnChannelFloat& prtclScriptObject::PreSimTimeChannel()
{
	return (*m_pPreSimTimeChannel);
}
tmlnChannelFloat& prtclScriptObject::ScaleStartChannel()
{
	return (*m_pScaleStartChannel);
}
tmlnChannelFloat& prtclScriptObject::ScaleCoefficientChannel()
{
	return (*m_pScaleCoefficientChannel);
}
tmlnChannelFloat& prtclScriptObject::StartAngleMinChannel()
{
	return (*m_pStartAngleMinChannel);
}
tmlnChannelFloat& prtclScriptObject::StartAngleMaxChannel()
{
	return (*m_pStartAngleMaxChannel);
}
tmlnChannelFloat& prtclScriptObject::AngularVelocityMaxChannel()
{
	return (*m_pAngularVelocityMaxChannel);
}
tmlnChannelFloat& prtclScriptObject::AngularVelocityMinChannel()
{
	return (*m_pAngularVelocityMinChannel);
}
tmlnChannelFloat& prtclScriptObject::AngularAccelerationMaxChannel()
{
	return (*m_pAngularAccelerationMaxChannel);
}
tmlnChannelFloat& prtclScriptObject::AngularAccelerationMinChannel()
{
	return (*m_pAngularAccelerationMinChannel);
}
tmlnChannelFloat& prtclScriptObject::EmitterScaleChannel()
{
	return (*m_pEmitterScaleChannel);
}
tmlnChannelFloat& prtclScriptObject::ConeAngleChannel()	// prtConeParticleGenerator
{
	return (*m_pConeAngleChannel);
}
tmlnChannelFloat& prtclScriptObject::MinSpeedChannel()
{
	return (*m_pMinSpeedChannel);
}
tmlnChannelFloat& prtclScriptObject::MaxSpeedChannel()
{
	return (*m_pMaxSpeedChannel);
}
tmlnChannelFloat& prtclScriptObject::AccelerationXChannel()
{
	return (*m_pAccelerationXChannel);
}
tmlnChannelFloat& prtclScriptObject::AccelerationYChannel()
{
	return (*m_pAccelerationYChannel);
}
tmlnChannelFloat& prtclScriptObject::AccelerationZChannel()
{
	return (*m_pAccelerationZChannel);
}
tmlnChannelFloat& prtclScriptObject::MinEmitSpeedChannel()	// prtSpiralParticleGenerator
{
	return (*m_pMinEmitSpeedChannel);
}
tmlnChannelFloat& prtclScriptObject::MaxEmitSpeedChannel()
{
	return (*m_pMaxEmitSpeedChannel);
}
tmlnChannelFloat& prtclScriptObject::EmitDirectionXChannel()
{
	return (*m_pEmitDirectionXChannel);
}
tmlnChannelFloat& prtclScriptObject::EmitDirectionYChannel()
{
	return (*m_pEmitDirectionYChannel);
}
tmlnChannelFloat& prtclScriptObject::EmitDirectionZChannel()
{
	return (*m_pEmitDirectionZChannel);
}
tmlnChannelFloat& prtclScriptObject::MinRotStartAngleChannel()
{
	return (*m_pMinRotStartAngleChannel);
}
tmlnChannelFloat& prtclScriptObject::MaxRotStartAngleChannel()
{
	return (*m_pMaxRotStartAngleChannel);
}
tmlnChannelFloat& prtclScriptObject::MinRotAngularVelChannel()
{
	return (*m_pMinRotAngularVelChannel);
}
tmlnChannelFloat& prtclScriptObject::MaxRotAngularVelChannel()
{
	return (*m_pMaxRotAngularVelChannel);
}
tmlnChannelFloat& prtclScriptObject::RotRadiusChannel()
{
	return (*m_pRotRadiusChannel);
}
tmlnChannelFloat& prtclScriptObject::RotRadiusScaleRateChannel()
{
	return (*m_pRotRadiusScaleRateChannel);
}
tmlnChannelFloat& prtclScriptObject::TextureAlphaStartChannel()
{
	return (*m_pTextureAlphaStartChannel);
}
tmlnChannelFloat& prtclScriptObject::TextureAlphaMiddleChannel()
{
	return (*m_pTextureAlphaMiddleChannel);
}
tmlnChannelFloat& prtclScriptObject::TextureAlphaEndChannel()
{
	return (*m_pTextureAlphaEndChannel);
}
tmlnChannelFloat& prtclScriptObject::TextureAlphaMiddlePercentStartChannel()
{
	return (*m_pTextureAlphaMiddlePercentStartChannel);
}
tmlnChannelFloat& prtclScriptObject::TextureAlphaMiddlePercentEndChannel()
{
	return (*m_pTextureAlphaMiddlePercentEndChannel);
}
tmlnChannelFileName& prtclScriptObject::TextureFileNameChannel()
{
	return (*m_pTextureFilenameChannel);
}
tmlnChannelBoolean& prtclScriptObject::RenderStreaksChannel()
{
	return (*m_pRenderStreaksChannel);
}
tmlnChannelFloat& prtclScriptObject::StreakLengthChannel()
{
	return (*m_pStreakLengthChannel);
}
tmlnChannelFloat& prtclScriptObject::StreakTaperChannel()
{
	return (*m_pStreakTaperChannel);
}
tmlnChannelFloat& prtclScriptObject::StreakFadeChannel()
{
	return (*m_pStreakFadeChannel);
}
tmlnChannelBoolean&	prtclScriptObject::ShowInCubeReflectionsChannel()
{
	return *m_pShowInCubeReflectionsChannel;
}
tmlnChannelBoolean&	prtclScriptObject::ShowInPlanarReflectionsChannel()
{
	return *m_pShowInPlanarReflectionsChannel;
}
tmlnChannelBoolean&	prtclScriptObject::CastShadowsChannel()
{
	return *m_pCastShadowsChannel;
}
tmlnChannelBoolean&	prtclScriptObject::UseDitheredShadowsChannel()
{
	return *m_pUseDitheredShadowsChannel;
}
tmlnChannelFloat& prtclScriptObject::ShadowDitherBiasChannel()
{
	return *m_pShadowDitherBiasChannel;
}
tmlnChannelBoolean&	prtclScriptObject::AdditiveChannel()
{
	return *m_pAdditiveChannel;
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void prtclScriptObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Update LayerMgr
	lyerLayerMgr::ObjectRenamed(m_pIcon);
	grpsGroupMgr::ObjectRenamed(m_pIcon);
}

