/*****************************************************************************
**  aoScriptObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/AmbientOcclusion/Object/aoScriptObject.hpp"

//#include "Systems/AmbientOcclusion/Object/aoAOObject.hpp"
#include "Systems/AmbientOcclusion/Data/aoDocumentChunk.hpp"
#include "Systems/AmbientOcclusion/Undo/aoOperations.hpp"

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
aoScriptObject::aoScriptObject( )
: m_pIcon(NULL)
{
	m_pIcon = new aoAOObject;
	this->SetPropertyObject(*m_pIcon);

	// Timeline stuff
	tmlnTimelineMgr::AddObject(this,"Ambient Occlusion");

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
	m_pClipPlaneEpsilonChannel	= new tmlnChannelFloatProperty(m_pIcon->PropertyClipPlaneEpsilon());
	this->AddChannel(m_pClipPlaneEpsilonChannel);
	m_pNoClipPlaneEpsilonChannel= new tmlnChannelFloatProperty(m_pIcon->PropertyNoClipPlaneEpsilon());
	this->AddChannel(m_pNoClipPlaneEpsilonChannel);
	m_pAreaRatioChannel		= new tmlnChannelFloatProperty(m_pIcon->PropertyAreaRatio());
	this->AddChannel(m_pAreaRatioChannel);
	m_pBehindPlaneEpsilonChannel	= new tmlnChannelFloatProperty(m_pIcon->PropertyBehindPlaneEpsilon());
	this->AddChannel(m_pBehindPlaneEpsilonChannel);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
aoScriptObject::~aoScriptObject()
{
	// Unregister from Timeline
	tmlnTimelineMgr::RemoveObject(this);

	delete m_pIcon;
}

//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
void aoScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
std::string aoScriptObject::GetDisplayName() const
{
	return "Ambient Occlusion";//m_pIcon->GetName().GetString();
}

//============================================================================
//	Data
//============================================================================


//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
aoScriptData aoScriptObject::GetScriptData() const
{
	aoScriptData data(this->GetBaseData());

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
void aoScriptObject::SetScriptData(const aoScriptData &i_Data)
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
void aoScriptObject::SetBaseData(const aoAOData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);

	m_pRadiusChannel->SetOriginalValue( i_Data.m_AORadius.GetValue() );
	m_pRadiusFarChannel->SetOriginalValue( i_Data.m_AORadiusFar.GetValue() );
	m_pAngleBiasChannel->SetOriginalValue( i_Data.m_AngleBias.GetValue() );
	m_pAttenuationChannel->SetOriginalValue( i_Data.m_Attenuation.GetValue() );
	m_pContrastChannel->SetOriginalValue( i_Data.m_Contrast.GetValue() );
	m_pBlurWidthChannel->SetOriginalValue( i_Data.m_BlurWidth.GetValue() );
	m_pBlurSharpnessChannel->SetOriginalValue( i_Data.m_BlurSharpness.GetValue() );
	m_pColorChannel->SetOriginalColor(i_Data.m_Color.GetValue());
	m_pClipPlaneEpsilonChannel->SetOriginalValue(i_Data.m_ClipPlaneEpsilon.GetValue());
	m_pNoClipPlaneEpsilonChannel->SetOriginalValue(i_Data.m_NoClipPlaneEpsilon.GetValue());
	m_pAreaRatioChannel->SetOriginalValue(i_Data.m_AreaRatio.GetValue());
	m_pBehindPlaneEpsilonChannel->SetOriginalValue(i_Data.m_BehindPlaneEpsilon.GetValue());
}

//--------------------------------------------------------------------
// Get values as a light base data structure
//--------------------------------------------------------------------
aoAOData aoScriptObject::GetBaseData() const
{
	aoAOData data( m_pIcon->GetData() );
	data.m_AORadius	= m_pRadiusChannel->GetOriginalValue();
	data.m_AORadiusFar	= m_pRadiusFarChannel->GetOriginalValue();
	data.m_AngleBias	= m_pAngleBiasChannel->GetOriginalValue();
	data.m_Attenuation	= m_pAttenuationChannel->GetOriginalValue();
	data.m_Contrast	= m_pContrastChannel->GetOriginalValue();
	data.m_BlurWidth	= m_pBlurWidthChannel->GetOriginalValue();
	data.m_BlurSharpness	= m_pBlurSharpnessChannel->GetOriginalValue();
	data.m_Color	= m_pColorChannel->GetOriginalColor();
	data.m_ClipPlaneEpsilon = m_pClipPlaneEpsilonChannel->GetOriginalValue(); 
	data.m_NoClipPlaneEpsilon = m_pNoClipPlaneEpsilonChannel->GetOriginalValue();
	data.m_AreaRatio = m_pAreaRatioChannel->GetOriginalValue();
	data.m_BehindPlaneEpsilon = m_pBehindPlaneEpsilonChannel->GetOriginalValue();

	return data;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
aoAOObject* aoScriptObject::GetPickObject() const
{
	return m_pIcon;
}


//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelFloat&	aoScriptObject::RadiusChannel()
{
	return (*m_pRadiusChannel);
}
tmlnChannelFloat&	aoScriptObject::RadiusFarChannel()
{
	return (*m_pRadiusFarChannel);
}
tmlnChannelFloat&	aoScriptObject::AngleBiasChannel()
{
	return (*m_pAngleBiasChannel);
}
tmlnChannelFloat&	aoScriptObject::AttenuationChannel()
{
	return (*m_pAttenuationChannel);
}
tmlnChannelFloat&	aoScriptObject::ContrastChannel()
{
	return (*m_pContrastChannel);
}
tmlnChannelFloat&	aoScriptObject::BlurWidthChannel()
{
	return (*m_pBlurWidthChannel);
}
tmlnChannelFloat&	aoScriptObject::BlurSharpnessChannel()
{
	return (*m_pBlurSharpnessChannel);
}
tmlnChannelColor&	aoScriptObject::ColorChannel()
{
	return (*m_pColorChannel);
}
tmlnChannelFloat&	aoScriptObject::ClipPlaneEpsilonChannel()
{
	return (*m_pClipPlaneEpsilonChannel);
}
tmlnChannelFloat&	aoScriptObject::NoClipPlaneEpsilonChannel()
{
	return (*m_pNoClipPlaneEpsilonChannel);
}
tmlnChannelFloat&	aoScriptObject::AreaRatioChannel()
{
	return (*m_pAreaRatioChannel);
}
tmlnChannelFloat&	aoScriptObject::BehindPlaneEpsilonChannel()
{
	return (*m_pBehindPlaneEpsilonChannel);
}

//--------------------------------------------------------------------
//	Name - passes functions on to pick object
//--------------------------------------------------------------------
//void aoScriptObject::SetName(const nameString& i_Name)
//{
//    m_pIcon->SetName( i_Name );
//}
//const nameString& aoScriptObject::GetName() const
//{
//	return m_pIcon->GetName();
//}

//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void aoScriptObject::NotifyDriverChanged()
{
	aoOperations::ChangeDriverData(this->GetScriptData());
}
