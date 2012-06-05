/*****************************************************************************
**	dcutDriverCamera.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/Drivers/dcutDriverCamera.hpp"

#include "Systems/DirectorsCut/Timeline/dcutChannelCamera.hpp"
#include "Systems/DirectorsCut/Drivers/dcutDriverCameraInfo.hpp"
#include "Systems/DirectorsCut/Drivers/dcutDriverCameraParser.hpp"

#include "Core/name/nameMgr.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Support/cams/camsCameraMgr.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dcutDriverCamera::dcutDriverCamera(dcutChannelCamera& i_Channel)
:	m_Channel(i_Channel)
{
	this->SetRestoreOriginalValue(false);
	
	m_pCameraNameUIInfo = new prtyComboBoxUIInfo(&(m_CameraName), "Properties", "Name of the camera to view");
	AddProperty( m_pCameraNameUIInfo );
		
	m_CameraName.AddCallback(new prtyCallbackWrapper<dcutDriverCamera>(this, &dcutDriverCamera::CameraNameChanged));
}

//--------------------------------------------------------------------
//  Update the object that is being driven
//--------------------------------------------------------------------
void  dcutDriverCamera::Operate(float i_Time)
{
	// This driver ignores the blending and only sets its
	// value if the time is after its Begin Time
	if (!IsBefore(i_Time))
	{
		int camera_index = camsCameraMgr::GetIndexForName(this->m_CameraName.GetValue());
		m_Channel.SetCameraIndex(camera_index);
	}
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string dcutDriverCamera::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();
	return desc;
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  dcutDriverCamera::GetDriverInfo() const
{
	dcutDriverCameraInfo *pInfo = new dcutDriverCameraInfo(dcutDriverCameraParser::GetChunkName());
	this->GetBaseDriverInfo(*pInfo);

	// TODO: re-get the camera name from the camsCameraMgr in order to confirm
	// any UID changes.
	pInfo->m_CameraName = this->m_CameraName.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void dcutDriverCamera::SetDriverInfo(const dcutDriverCameraInfo& i_Info, 
										prtyProperty::UndoFlags i_Undoable)
{
	this->SetBaseDriverInfo(i_Info);

	// Store local info
	this->m_CameraName.SetValue(i_Info.m_CameraName, i_Undoable);
}

//--------------------------------------------------------------------
//  Show dialog that allows user to edit this driver's properties
//--------------------------------------------------------------------
void  dcutDriverCamera::DoEditProperties()
{
	// Before showing the properties dialog,
	// update the name list to the current list of named objects.
	m_pCameraNameUIInfo->ClearItems();

	nameString name;
	const int num_cameras = camsCameraMgr::GetNumCameras();
	for ( int i = 0 ; i < num_cameras; i++ )
	{
		//DBG_LOG2( "%02d) %s", i, name.GetString().c_str() );

		camsCameraMgr::GetCameraName(i, name);
		m_pCameraNameUIInfo->AddItem( name.GetString() );
	}
	m_pCameraNameUIInfo->UpdateControl();

	// Let base class set up the properties-based dialog
	tmlnDriver::DoEditProperties();
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA dcutDriverCamera::GetClipFillColor() const
{
	// Let the color change based on which camera is chosen
	int camera_index = camsCameraMgr::GetIndexForName(this->m_CameraName.GetValue());
	if (camera_index < 0)
		return maFloatRGBA( 0.3f, 0.3f, 0.3f, 1.0f );
	else
	{
		switch (camera_index % 6)
		{
		default:
		case 0:
			return maFloatRGBA( 0.2f, 0.3f, 0.8f, 1.0f );
		case 1:
			return maFloatRGBA( 0.2f, 0.8f, 0.3f, 1.0f );
		case 2:
			return maFloatRGBA( 0.8f, 0.3f, 0.2f, 1.0f );
		case 3:
			return maFloatRGBA( 0.2f, 0.8f, 0.8f, 1.0f );
		case 4:
			return maFloatRGBA( 0.8f, 0.3f, 0.8f, 1.0f );
		case 5:
			return maFloatRGBA( 0.8f, 0.8f, 0.2f, 1.0f );
		}
	}
}

//--------------------------------------------------------------------
// Return name of camera being indexed
//--------------------------------------------------------------------
const nameString& dcutDriverCamera::GetCameraName()
{
	//	if there is a valid name UID then get the name text associated
	//	with it in case it changed since the item grabbed it last.
	//
	//DBG_LOG( "object UID " << pInfo->m_ObjectName.GetUID() );

	if ( m_CameraName.GetUID() != nameString::e_InvalidUID )
	{
		std::string name;
		nameMgr::GetNameString( m_CameraName.GetUID(), name );
		m_CameraName.SetString( name );

		//DBG_LOG( "attach object (" << name.c_str() << ")" );
	}

	return m_CameraName.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void dcutDriverCamera::CameraNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	if (i_bDirty)
	{
		// Set name of driver to name of the camera
		if (!m_CameraName.GetString().empty())
			this->SetName(m_CameraName.GetString().c_str());
	}

	this->MarkDirty();
}
