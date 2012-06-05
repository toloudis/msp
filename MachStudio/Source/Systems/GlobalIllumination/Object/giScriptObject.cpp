/*****************************************************************************
**  giScriptObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/GlobalIllumination/Object/giScriptObject.hpp"

//#include "Systems/GlobalIllumination/Object/giGIObject.hpp"
#include "Systems/GlobalIllumination/Data/giDocumentChunk.hpp"
#include "Systems/GlobalIllumination/Undo/giOperations.hpp"

#include "Support/grps/grpsGroupMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Support/mnm/mnmReferencePosChannel.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
giScriptObject::giScriptObject( )
: m_pIcon(NULL)
{
	m_pIcon = new giGIObject;
	this->SetPropertyObject(*m_pIcon);

	// Timeline stuff
	tmlnTimelineMgr::AddObject(this,"Global Illumination");

	m_pColorChannel		= new tmlnChannelColorProperty(m_pIcon->PropertyColor());
	this->AddChannel(m_pColorChannel);
	m_pRadiusChannel		= new tmlnChannelFloatProperty(m_pIcon->PropertyRadius());
	this->AddChannel(m_pRadiusChannel);
	m_pRadiusFarChannel		= new tmlnChannelFloatProperty(m_pIcon->PropertyRadiusFar());
	this->AddChannel(m_pRadiusFarChannel);
	m_pAngleBiasChannel		= new tmlnChannelFloatProperty(m_pIcon->PropertyAngleBias());
	this->AddChannel(m_pAngleBiasChannel);
	m_pContrastChannel		= new tmlnChannelFloatProperty(m_pIcon->PropertyContrast());
	this->AddChannel(m_pContrastChannel);
	m_pAttenuationChannel		= new tmlnChannelFloatProperty(m_pIcon->PropertyAttenuation());
	this->AddChannel(m_pAttenuationChannel);
	m_pBlurWidthChannel		= new tmlnChannelFloatProperty(m_pIcon->PropertyBlurWidth());
	this->AddChannel(m_pBlurWidthChannel);
	m_pBlurSharpnessChannel		= new tmlnChannelFloatProperty(m_pIcon->PropertyBlurSharpness());
	this->AddChannel(m_pBlurSharpnessChannel);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
giScriptObject::~giScriptObject()
{
	// Unregister from Timeline
	tmlnTimelineMgr::RemoveObject(this);

	delete m_pIcon;
}

//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
void giScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
std::string giScriptObject::GetDisplayName() const
{
	return "Global Illumination";//m_pIcon->GetName().GetString();
}

//============================================================================
//	Data
//============================================================================


//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
giScriptData giScriptObject::GetScriptData() const
{
	giScriptData data(this->GetBaseData());

	// get custom property data
	this->GetCustomPropertyData(data.m_CustomProperties);

	// get driver info
	tmlnCreator::GetDriverInfo(this, data.m_Drivers);

	// get the channel info
	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	return data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void giScriptObject::SetScriptData(const giScriptData &i_Data)
{
	this->SetBaseData( i_Data.m_BaseData );

	// set custom property data
	this->SetCustomPropertyData(i_Data.m_CustomProperties);

	// create drivers from info
	tmlnCreator::SetDriverInfo(this, i_Data.m_Drivers);

	// set the channels 
	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );
}

//--------------------------------------------------------------------
// Set from light base data structure
//--------------------------------------------------------------------
void giScriptObject::SetBaseData(const giGIData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);

	m_pRadiusChannel->SetOriginalValue( i_Data.m_GIRadius.GetValue() );
	m_pRadiusFarChannel->SetOriginalValue( i_Data.m_GIRadiusFar.GetValue() );
	m_pAngleBiasChannel->SetOriginalValue( i_Data.m_AngleBias.GetValue() );
	m_pAttenuationChannel->SetOriginalValue( i_Data.m_Attenuation.GetValue() );
	m_pContrastChannel->SetOriginalValue( i_Data.m_Contrast.GetValue() );
	m_pBlurWidthChannel->SetOriginalValue( i_Data.m_BlurWidth.GetValue() );
	m_pBlurSharpnessChannel->SetOriginalValue( i_Data.m_BlurSharpness.GetValue() );
	m_pColorChannel->SetOriginalColor(i_Data.m_Color.GetValue());
}

//--------------------------------------------------------------------
// Get values as a light base data structure
//--------------------------------------------------------------------
giGIData giScriptObject::GetBaseData() const
{
	giGIData data( m_pIcon->GetData() );
	data.m_GIRadius	= m_pRadiusChannel->GetOriginalValue();
	data.m_GIRadiusFar	= m_pRadiusFarChannel->GetOriginalValue();
	data.m_AngleBias	= m_pAngleBiasChannel->GetOriginalValue();
	data.m_Attenuation	= m_pAttenuationChannel->GetOriginalValue();
	data.m_Contrast	= m_pContrastChannel->GetOriginalValue();
	data.m_BlurWidth	= m_pBlurWidthChannel->GetOriginalValue();
	data.m_BlurSharpness	= m_pBlurSharpnessChannel->GetOriginalValue();
	data.m_Color	= m_pColorChannel->GetOriginalColor();

	return data;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
giGIObject* giScriptObject::GetPickObject() const
{
	return m_pIcon;
}


//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelFloat&	giScriptObject::RadiusChannel()
{
	return (*m_pRadiusChannel);
}
tmlnChannelFloat&	giScriptObject::RadiusFarChannel()
{
	return (*m_pRadiusFarChannel);
}
tmlnChannelFloat&	giScriptObject::AngleBiasChannel()
{
	return (*m_pAngleBiasChannel);
}
tmlnChannelFloat&	giScriptObject::AttenuationChannel()
{
	return (*m_pAttenuationChannel);
}
tmlnChannelFloat&	giScriptObject::ContrastChannel()
{
	return (*m_pContrastChannel);
}
tmlnChannelFloat&	giScriptObject::BlurWidthChannel()
{
	return (*m_pBlurWidthChannel);
}
tmlnChannelFloat&	giScriptObject::BlurSharpnessChannel()
{
	return (*m_pBlurSharpnessChannel);
}
tmlnChannelColor&	giScriptObject::ColorChannel()
{
	return (*m_pColorChannel);
}

//--------------------------------------------------------------------
//	Name - passes functions on to pick object
//--------------------------------------------------------------------
//void giScriptObject::SetName(const nameString& i_Name)
//{
//    m_pIcon->SetName( i_Name );
//}
//const nameString& giScriptObject::GetName() const
//{
//	return m_pIcon->GetName();
//}

//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void giScriptObject::NotifyDriverChanged()
{
	giOperations::ChangeDriverData(this->GetScriptData());
}
