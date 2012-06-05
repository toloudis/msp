/*****************************************************************************
**  fogScriptObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Fog/Object/fogScriptObject.hpp"

#include "Systems/Fog/Object/fogFogObject.hpp"
#include "Systems/Fog/Data/fogDocumentChunk.hpp"
#include "Systems/Fog/Undo/fogOperations.hpp"

#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fogScriptObject::fogScriptObject( )
: m_pIcon(NULL)
{
	m_pIcon = new fogFogObject;
	this->SetPropertyObject(*m_pIcon);

	// Timeline stuff
	tmlnTimelineMgr::AddObject(this,"Fog");

	m_pbEnableChannel				= new tmlnChannelBooleanProperty(m_pIcon->PropertybEnable());
	this->AddChannel(m_pbEnableChannel);
	m_pbWorldOrientationChannel		= new tmlnChannelBooleanProperty(m_pIcon->PropertybWorldOrientation());
	this->AddChannel(m_pbWorldOrientationChannel);
	//m_pModeChannel				= new tmlnChannelEnumProperty(m_pIcon->PropertyMode());
	//this->AddChannel(m_pModeChannel);
	m_pColorChannel				= new tmlnChannelColorProperty(m_pIcon->PropertyColor());
	this->AddChannel(m_pColorChannel);
	m_pDensityChannel			= new tmlnChannelFloatProperty(m_pIcon->PropertyDensity());
	this->AddChannel(m_pDensityChannel);
	m_pStartChannel				= new tmlnChannelFloatProperty(m_pIcon->PropertyStart());
	this->AddChannel(m_pStartChannel);
	m_pEndChannel				= new tmlnChannelFloatProperty(m_pIcon->PropertyEnd());
	this->AddChannel(m_pEndChannel);
	m_pAltitudeStartChannel		= new tmlnChannelFloatProperty(m_pIcon->PropertyAltitudeStart());
	this->AddChannel(m_pAltitudeStartChannel);
	m_pAltitudeEndChannel		= new tmlnChannelFloatProperty(m_pIcon->PropertyAltitudeEnd());
	this->AddChannel(m_pAltitudeEndChannel);
	m_pAltitudeDensityChannel	= new tmlnChannelFloatProperty(m_pIcon->PropertyAltitudeDensity());
	this->AddChannel(m_pAltitudeDensityChannel);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fogScriptObject::~fogScriptObject()
{
	// Unregister from Timeline
	tmlnTimelineMgr::RemoveObject(this);

	delete m_pIcon;
}

//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
void fogScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
std::string fogScriptObject::GetDisplayName() const
{
	return "Fog";//m_pIcon->GetName().GetString();
}

//--------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//--------------------------------------------------------------------
//sel3dObject* fogFogObject::GetParentObject() const
//{
//	return m_pParent;
//}
//void fogFogObject::SetParentObject(sel3dObject* i_pPO)
//{
//	m_pParent = i_pPO;
//}


//============================================================================
//	Data
//============================================================================


//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
fogScriptData fogScriptObject::GetScriptData() const
{
	fogScriptData data(this->GetBaseData());

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
void fogScriptObject::SetScriptData(const fogScriptData &i_Data)
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
void fogScriptObject::SetBaseData(const fogFogData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);

	m_pbEnableChannel->SetOriginalState(i_Data.m_bEnable.GetValue());
	m_pbWorldOrientationChannel->SetOriginalState(i_Data.m_bWorldOrientation.GetValue());
	//m_pModeChannel->SetOriginalValue( i_Data.m_FogMode.GetValue() );
	m_pColorChannel->SetOriginalColor(i_Data.m_Color.GetValue());
	m_pDensityChannel->SetOriginalValue( i_Data.m_Density.GetValue() );
	m_pStartChannel->SetOriginalValue( i_Data.m_Start.GetValue() );
	m_pEndChannel->SetOriginalValue( i_Data.m_End.GetValue() );
	m_pAltitudeStartChannel->SetOriginalValue( i_Data.m_AltitudeStart.GetValue() );
	m_pAltitudeEndChannel->SetOriginalValue( i_Data.m_AltitudeEnd.GetValue() );
	m_pAltitudeDensityChannel->SetOriginalValue( i_Data.m_AltitudeDensity.GetValue() );
}

//--------------------------------------------------------------------
// Get values as a light base data structure
//--------------------------------------------------------------------
fogFogData fogScriptObject::GetBaseData() const
{
	fogFogData data( m_pIcon->GetData() );

	data.m_bEnable	= m_pbEnableChannel->GetOriginalState();
	data.m_bWorldOrientation	= m_pbWorldOrientationChannel->GetOriginalState();
	//data.m_FogMode	= m_pModeChannel->GetOriginalValue();
	data.m_Color	= m_pColorChannel->GetOriginalColor();
	data.m_Density	= m_pDensityChannel->GetOriginalValue();
	data.m_Start	= m_pStartChannel->GetOriginalValue();
	data.m_End		= m_pEndChannel->GetOriginalValue();
	data.m_AltitudeStart	= m_pAltitudeStartChannel->GetOriginalValue();
	data.m_AltitudeEnd		= m_pAltitudeEndChannel->GetOriginalValue();
	data.m_AltitudeDensity	= m_pAltitudeDensityChannel->GetOriginalValue();

	return data;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
fogFogObject* fogScriptObject::GetPickObject() const
{
	return m_pIcon;
}

//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelBoolean&	fogScriptObject::bEnableChannel()
{
	return (*m_pbEnableChannel);
}

//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelBoolean&	fogScriptObject::bWorldOrientationChannel()
{
	return (*m_pbWorldOrientationChannel);
}

//tmlnChannelFloat&	aoScriptObject::ModeChannel()
//{
//	return (*m_pModeChannel);
//}
tmlnChannelColor&	fogScriptObject::ColorChannel()
{
	return (*m_pColorChannel);
}
tmlnChannelFloat&	fogScriptObject::DensityChannel()
{
	return (*m_pDensityChannel);
}
tmlnChannelFloat&	fogScriptObject::StartChannel()
{
	return (*m_pStartChannel);
}
tmlnChannelFloat&	fogScriptObject::EndChannel()
{
	return (*m_pEndChannel);
}
tmlnChannelFloat&	fogScriptObject::AltitudeStartChannel()
{
	return (*m_pAltitudeStartChannel);
}
tmlnChannelFloat&	fogScriptObject::AltitudeEndChannel()
{
	return (*m_pAltitudeEndChannel);
}
tmlnChannelFloat&	fogScriptObject::AltitudeDensityChannel()
{
	return (*m_pAltitudeDensityChannel);
}

//--------------------------------------------------------------------
//	Name - passes functions on to pick object
//--------------------------------------------------------------------
//void fogScriptObject::SetName(const nameString& i_Name)
//{
//    m_pIcon->SetName( i_Name );
//}
//const nameString& fogScriptObject::GetName() const
//{
//	return m_pIcon->GetName();
//}

//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void fogScriptObject::NotifyDriverChanged()
{
	fogOperations::ChangeDriverData(this->GetScriptData());
}
