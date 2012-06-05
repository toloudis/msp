/*****************************************************************************
**  dcutScriptObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/Object/dcutScriptObject.hpp"

#include "Systems/DirectorsCut/Object/dcutDirectorsCutObject.hpp"
#include "Systems/DirectorsCut/Timeline/dcutChannelCamera.hpp"
#include "Systems/DirectorsCut/Timeline/dcutChannelCapture.hpp"
#include "Systems/DirectorsCut/Undo/dcutOperations.hpp"
#include "Systems/DirectorsCut/Data/dcutScriptData.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"


//============================================================================
//============================================================================
namespace
{
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dcutScriptObject::dcutScriptObject(const dcutScriptData &i_Data)
{
	m_pIcon = new dcutDirectorsCutObject();
	m_pIcon->SetParentObject(this);

	// Timeline stuff
	tmlnTimelineMgr::AddObject(this, "Director's Cut" );

	m_pCaptureChannel = new dcutChannelCapture("Capture");
	this->AddChannel(m_pCaptureChannel);
	m_pCameraChannel = new dcutChannelCamera("Camera", m_pIcon);
	this->AddChannel(m_pCameraChannel);

	// Set starting values from passed in data
	this->SetScriptData(i_Data);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dcutScriptObject::~dcutScriptObject()
{
	tmlnTimelineMgr::RemoveObject(this);

	delete m_pIcon;
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string dcutScriptObject::GetTmlnName() const
{
	return m_pIcon->GetName().GetString();
}

//--------------------------------------------------------------------
// Get values as a camera data structure
//--------------------------------------------------------------------
dcutScriptData dcutScriptObject::GetScriptData() const
{
	dcutScriptData data(this->GetBaseData());

	// get driver info
	tmlnCreator::GetDriverInfo(this, (data.m_Drivers));

	// get the channel info
	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	return data;
}

//--------------------------------------------------------------------
// Set from camera data structure
//--------------------------------------------------------------------
void dcutScriptObject::SetScriptData(const dcutScriptData &i_Data)
{
	this->SetBaseData(i_Data.m_BaseData);

	// create drivers from info
	DBG_LOG(this->GetName().GetString().c_str() << " about to create " << i_Data.m_Drivers.size() << " drivers" );
	tmlnCreator::SetDriverInfo( this, (i_Data.m_Drivers) );

	// set the channels 
	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );
}


//--------------------------------------------------------------------
// Get values as a base data structure
//--------------------------------------------------------------------
dcutCueData dcutScriptObject::GetBaseData() const
{
	dcutCueData data( m_pIcon->GetData());

//	data.m_bCapture		= m_pCaptureChannel->GetOriginalValue();

	return data;
}

//--------------------------------------------------------------------
// Set from base data structure
//--------------------------------------------------------------------
void dcutScriptObject::SetBaseData(const dcutCueData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);

	//	call functions for all data that sets the object + data
	//
	//m_pCaptureChannel->SetOriginalValue(i_Data.m_bCapture.GetValue());
}

//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
dcutChannelCapture& dcutScriptObject::CaptureChannel()
{
	return (*m_pCaptureChannel);
}
dcutChannelCamera& dcutScriptObject::CameraChannel()
{
	return (*m_pCameraChannel);
}

//--------------------------------------------------------------------
//	Name - passes functions on to pick object
//--------------------------------------------------------------------
void dcutScriptObject::SetName(const nameString& i_Name)
{
    m_pIcon->SetName( i_Name );
}
const nameString& dcutScriptObject::GetName() const
{
	return m_pIcon->GetName();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dcutDirectorsCutObject* dcutScriptObject::GetPickObject() const
{
	return m_pIcon;
}

//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void dcutScriptObject::NotifyDriverChanged()
{
	dcutOperations::ChangeDriverData(this->GetScriptData());
}
