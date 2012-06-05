/*****************************************************************************
**	cmraDriverFocusDistanceAttach.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverFocusDistanceAttach.hpp"

#include "Drivers/Attach/tmlnDriverAttachUtil.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/mnm/mnmObject.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFocusDistanceAttachInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFocusDistanceAttachParser.hpp"

#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Graphics/cam/camCamera.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFocusDistanceAttach::cmraDriverFocusDistanceAttach(nameUID i_UID, 
											 tmlnChannelFloat &i_FocusDistanceChannel, 
											 chDefs::Name i_ChunkName)
:	m_ChannelFocusDistance(i_FocusDistanceChannel),
	m_ChunkName(i_ChunkName),
	m_bNeedsAttach(false),
	m_cameraUID(i_UID),
	m_ObjectName("Object Name"),
	m_AttachName("Attach Name")
{
	// Register the properties so they can be displayed to the user
	//
	const int lc_ATTACHCONTROL_WIDTH = 300;
	m_pObjectNameUIInfo = new prtyComboBoxUIInfo(&(m_ObjectName), "Properties", "Name of the object to which the driver should attach");
	m_pObjectNameUIInfo->SetWidth( lc_ATTACHCONTROL_WIDTH );
	AddProperty( m_pObjectNameUIInfo );
	m_pAttachNameUIInfo = new prtyComboBoxUIInfo(&(m_AttachName), "Properties", "Name of transform node or joint for matrix");
	m_pAttachNameUIInfo->SetWidth( lc_ATTACHCONTROL_WIDTH );
	AddProperty( m_pAttachNameUIInfo );	


	//	property callbacks
	m_ObjectName.AddCallback(new prtyCallbackWrapper<cmraDriverFocusDistanceAttach>(this, &cmraDriverFocusDistanceAttach::ObjectNameChanged));
	m_AttachName.AddCallback(new prtyCallbackWrapper<cmraDriverFocusDistanceAttach>(this, &cmraDriverFocusDistanceAttach::AttachNameChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFocusDistanceAttach::~cmraDriverFocusDistanceAttach()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string cmraDriverFocusDistanceAttach::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	std::string str = "Attach Focus : ";
	str += m_ObjectName.GetValue().GetString();
	str += " ";
	str += m_AttachName.GetValue();

	desc += str;
	return desc;
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  cmraDriverFocusDistanceAttach::GetDriverInfo() const
{
	cmraDriverFocusDistanceAttachInfo *pInfo = new cmraDriverFocusDistanceAttachInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_ObjectName = this->m_ObjectName.GetValue();
	pInfo->m_AttachName = this->m_AttachName.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void cmraDriverFocusDistanceAttach::SetDriverInfo(	const cmraDriverFocusDistanceAttachInfo& i_Info)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// Store local info
	this->m_ObjectName.SetValue( i_Info.m_ObjectName);
	this->m_AttachName.SetValue(i_Info.m_AttachName);

	//DBG_LOG2( "driverAttach INFO    %02d (%s)", i_Info.m_ObjectName.GetUID(), i_Info.m_ObjectName.GetString().c_str() );

	// Delay actual attachment, helps to make sure all named objects
	// are loaded and created.
	m_bNeedsAttach = true;

	//DBG_LOG2( "driverAttach objname %02d (%s)", this->m_ObjectName.GetUID(), this->m_ObjectName.GetString().c_str() );

}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  cmraDriverFocusDistanceAttach::Operate(const maTime& i_Time)
{
	if (m_bNeedsAttach)
	{
		// Find object by name
		nameObject* obj = tmlnDriverAttachUtil::GetObjectByName( this->m_ObjectName.GetValue() );	// name of prop to target
		if (obj && dynamic_cast<mnmObject*>(obj))
		{
			//	attach object
			//
			this->m_ObjectName.SetValue(obj->GetName()); // sets the UID correctly if we found it by string
			
			mnmObject *pObj = dynamic_cast<mnmObject*>(obj);

			// Get list of references
			std::vector<std::string> references;
			pObj->GetReferenceList(references);

			// Set the reference list UI Info now that we have the object,
			// but only if necessary
			if (m_pAttachNameUIInfo->m_List != references)
			{
				m_pAttachNameUIInfo->ClearItems();
				pObj->GetReferenceList(m_pAttachNameUIInfo->m_List);
				m_pAttachNameUIInfo->UpdateControl();
			}

			// If our attach name is empty, pick the first name in the reference list
			if ( this->m_AttachName.GetValue().empty() 
				&& !references.empty() )
			{	
				this->m_AttachName.SetValue( references[0] );
			}

			api3dReference *pRef = pObj->GetReference(this->m_AttachName.GetValue().c_str()); // reference node name
			//DBG_LOG("Got mnmObject, and reference?: " << (pRef ? "True" : "NULL"));
			m_Reference.AttachTo(pRef);
		}
		else m_Reference.AttachTo(NULL);
		//m_Reference.SetAttachOffset( this->m_AttachOffset.GetValue() );
		m_Reference.SetUseBoundingBox(true); // always true for this type of attachment

		m_bNeedsAttach = false;
	}

	maPoint3d pw = m_Reference.GetPosition();
	maPoint3d cam_pos = get_camera_position();

	float goal_dist = (pw - cam_pos).Length();

	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		// Near focus blend
		float cur_dist = m_ChannelFocusDistance.GetValue();
		float dist_pct = this->GetBlendAlpha(m_ChannelFocusDistance.GetPreviousTime(i_Time), i_Time);
		float dist = goal_dist*dist_pct + cur_dist*(1.0f - dist_pct);	// linear blend
		m_ChannelFocusDistance.SetValue(dist);
	}
	else
	{
		m_ChannelFocusDistance.SetValue( goal_dist );
	}

	// Because the attachment object might move at any time without telling us,
	// we need to always operate
	this->MarkDirty();
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA cmraDriverFocusDistanceAttach::GetClipFillColor() const
{
	return maFloatRGBA( 0.7843f, 1.0f, 0.3569f, 1.0f );
}

//--------------------------------------------------------------------
//	Override base class to have this driver's Operate called at end.
//--------------------------------------------------------------------
//virtual 
bool cmraDriverFocusDistanceAttach::NeedsDelayedOperate()
{
	return true;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverFocusDistanceAttach::ObjectNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	//	if the UID is invalid, then see if the object that has a name match
	//	has a valid UID.  This should only be the case with object's that 
	//	haven't been converted to the current format.
	//
	if ( this->m_ObjectName.GetValue().GetUID() == nameString::e_InvalidUID )
	{
		// Find object by name
		nameObject* obj = nameMgr::GetObjectByName( this->m_ObjectName.GetValue() );	// name of prop to target
		if (obj && dynamic_cast<mnmObject*>(obj))
		{
			if ( obj->GetName().GetUID() != nameString::e_InvalidUID )
			{
				this->m_ObjectName.SetUID( obj->GetName().GetUID() );
			}
		}
	}

	// Delay actual attachment, helps to make sure all named objects
	// are loaded and created.
	m_bNeedsAttach = true;

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverFocusDistanceAttach::AttachNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Delay actual attachment, helps to make sure all named objects
	// are loaded and created.
	m_bNeedsAttach = true;

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maPoint3d cmraDriverFocusDistanceAttach::get_camera_position()
{
	nameString name;
	name.SetUID( this->m_cameraUID );

	int camindex = camsCameraMgr::GetIndexForName( name );
	DBG_ASSERT( camindex != -1, "Couldn't find the camera - " << name.GetString().c_str() );
	const camCamera* pCamera = camsCameraMgr::GetCamera(camindex);
	return pCamera ? pCamera->GetPosition() : maPoint3d(0,0,0);
}

//--------------------------------------------------------------------
//  Show dialog that allows user to edit this driver's properties
//--------------------------------------------------------------------
void  cmraDriverFocusDistanceAttach::DoEditProperties()
{
	// Before showing the properties dialog,
	// update the name list to the current list of named objects.
	m_pObjectNameUIInfo->ClearItems();
	nameList aNameList;
	nameMgr::GetNameList( aNameList );
	for ( int i = 0 ; i < aNameList.size() ; i++ )
	{
		//DBG_LOG2( "%02d) %s", i, aNameList[i]->GetString().c_str() );
		m_pObjectNameUIInfo->AddItem( aNameList[i]->GetString() );
	}
	m_pObjectNameUIInfo->UpdateControl();

	// Let base class set up the properties-based dialog
	tmlnDriver::DoEditProperties();
}
