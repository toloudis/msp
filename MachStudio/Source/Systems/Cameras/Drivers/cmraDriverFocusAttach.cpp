/*****************************************************************************
**	cmraDriverFocusAttach.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverFocusAttach.hpp"

#include "Drivers/Attach/tmlnDriverAttachUtil.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/mnm/mnmObject.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFocusAttachInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFocusAttachParser.hpp"

#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Graphics/cam/camCamera.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFocusAttach::cmraDriverFocusAttach(nameUID i_UID, 
											 tmlnChannelFloatProperty &i_NearFocusChannel, 
											 tmlnChannelFloatProperty &i_FarFocusChannel, 
											 tmlnChannelFloatProperty &i_NearBlurChannel, 
											 tmlnChannelFloatProperty &i_FarBlurChannel, 
											 chDefs::Name i_ChunkName)
:	m_ChannelNearFocus(i_NearFocusChannel), 
	m_ChannelFarFocus(i_FarFocusChannel), 
	m_ChannelFarBlur(i_FarBlurChannel), 
	m_ChannelNearBlur(i_NearBlurChannel), 
	m_ChunkName(i_ChunkName),
	m_bNeedsAttach(false),
	m_cameraUID(i_UID),
	m_NearFocusOffset("Extend near focus", 30),
	m_FarFocusOffset("Extend far focus", 30),
	m_NearFalloffDist("Near blur falloff distance", 30),
	m_FarFalloffDist("Far blur falloff distance", 30),
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

	prtyRangedFloatUIInfo* pPUII;
	pPUII = new prtyRangedFloatUIInfo(&(m_NearFocusOffset), "Properties", "description");
	pPUII->SetMinimum(0);
	pPUII->SetMaximum(2500);
	AddProperty( pPUII );
	pPUII = new prtyRangedFloatUIInfo(&(m_FarFocusOffset), "Properties", "description");
	pPUII->SetMinimum(0);
	pPUII->SetMaximum(2500);
	AddProperty( pPUII );
	pPUII = new prtyRangedFloatUIInfo(&(m_NearFalloffDist), "Properties", "description");
	pPUII->SetMinimum(0);
	pPUII->SetMaximum(2500);
	AddProperty( pPUII );
	pPUII = new prtyRangedFloatUIInfo(&(m_FarFalloffDist), "Properties", "description");
	pPUII->SetMinimum(0);
	pPUII->SetMaximum(2500);
	AddProperty( pPUII );


	//	property callbacks
	m_NearFocusOffset.AddCallback(new prtyCallbackWrapper<cmraDriverFocusAttach>(this, &cmraDriverFocusAttach::PropertyChanged));
	m_FarFocusOffset.AddCallback(new prtyCallbackWrapper<cmraDriverFocusAttach>(this, &cmraDriverFocusAttach::PropertyChanged));
	m_NearFalloffDist.AddCallback(new prtyCallbackWrapper<cmraDriverFocusAttach>(this, &cmraDriverFocusAttach::PropertyChanged));
	m_FarFalloffDist.AddCallback(new prtyCallbackWrapper<cmraDriverFocusAttach>(this, &cmraDriverFocusAttach::PropertyChanged));
	m_ObjectName.AddCallback(new prtyCallbackWrapper<cmraDriverFocusAttach>(this, &cmraDriverFocusAttach::ObjectNameChanged));
	m_AttachName.AddCallback(new prtyCallbackWrapper<cmraDriverFocusAttach>(this, &cmraDriverFocusAttach::AttachNameChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFocusAttach::~cmraDriverFocusAttach()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string cmraDriverFocusAttach::GetHoverDescription()
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
tmlnDriverInfo*  cmraDriverFocusAttach::GetDriverInfo() const
{
	cmraDriverFocusAttachInfo *pInfo = new cmraDriverFocusAttachInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_ObjectName = this->m_ObjectName.GetValue();
	pInfo->m_AttachName = this->m_AttachName.GetValue();
	pInfo->m_NearFocusOffset = this->m_NearFocusOffset.GetValue();
	pInfo->m_FarFocusOffset = this->m_FarFocusOffset.GetValue();
	pInfo->m_NearFalloffDist = this->m_NearFalloffDist.GetValue();
	pInfo->m_FarFalloffDist = this->m_FarFalloffDist.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void cmraDriverFocusAttach::SetDriverInfo(	const cmraDriverFocusAttachInfo& i_Info)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// Store local info
	this->m_ObjectName.SetValue( i_Info.m_ObjectName);
	this->m_AttachName.SetValue(i_Info.m_AttachName);
	this->m_NearFocusOffset.SetValue( i_Info.m_NearFocusOffset);
	this->m_FarFocusOffset.SetValue( i_Info.m_FarFocusOffset);
	this->m_NearFalloffDist.SetValue( i_Info.m_NearFalloffDist);
	this->m_FarFalloffDist.SetValue( i_Info.m_FarFalloffDist);

	//DBG_LOG2( "driverAttach INFO    %02d (%s)", i_Info.m_ObjectName.GetUID(), i_Info.m_ObjectName.GetString().c_str() );

	// Delay actual attachment, helps to make sure all named objects
	// are loaded and created.
	m_bNeedsAttach = true;

	//DBG_LOG2( "driverAttach objname %02d (%s)", this->m_ObjectName.GetUID(), this->m_ObjectName.GetString().c_str() );

}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  cmraDriverFocusAttach::Operate(const maTime& i_Time)
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

		m_bNeedsAttach = false;
	}

	maPoint3d pw = m_Reference.GetPosition();
	maMatrix4x4 worldToCamera = GetCameraMatrix();
	maPoint3d pc = pw;
	worldToCamera.Transform(pc);

	// put bounds in camera space 
//		maAxisBox bounds = pObj->GetWorldBox();
//		maAxisBox camSpaceBounds = XForm(bounds, worldToCamera);

	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		// Near focus blend
		float goal_pos = pc.GetZ() - m_NearFocusOffset.GetValue();
		float cur_pos = m_ChannelNearFocus.GetValue();
		float pos_pct = this->GetBlendAlpha(m_ChannelNearFocus.GetPreviousTime(i_Time), i_Time);
		float pos = goal_pos*pos_pct + cur_pos*(1.0f - pos_pct);	// linear blend
		m_ChannelNearFocus.SetValue(pos);

		float goal_blur = goal_pos - m_NearFalloffDist.GetValue();
		float cur_blur = m_ChannelNearBlur.GetValue();
		float blur_pct = this->GetBlendAlpha(m_ChannelNearBlur.GetPreviousTime(i_Time), i_Time);
		float blur = goal_blur*blur_pct + cur_blur*(1.0f - blur_pct);
		m_ChannelNearBlur.SetValue(blur);

		// Far focus blend
		goal_pos = pc.GetZ() + m_FarFocusOffset.GetValue();
		cur_pos = m_ChannelFarFocus.GetValue();
		pos_pct = this->GetBlendAlpha(m_ChannelFarFocus.GetPreviousTime(i_Time), i_Time);
		pos = goal_pos*pos_pct + cur_pos*(1.0f - pos_pct);	// linear blend
		m_ChannelFarFocus.SetValue(pos);

		goal_blur = goal_pos + m_FarFalloffDist.GetValue();
		cur_blur = m_ChannelFarBlur.GetValue();
		blur_pct = this->GetBlendAlpha(m_ChannelFarBlur.GetPreviousTime(i_Time), i_Time);
		blur = goal_blur*blur_pct + cur_blur*(1.0f - blur_pct);
		m_ChannelFarBlur.SetValue(blur);
	}
	else
	{
		// within driver range
		//float percent = (i_Time - this->GetBeginTime()) / this->GetDuration();
		float pos = pc.GetZ() - m_NearFocusOffset.GetValue();
		m_ChannelNearFocus.SetValue(pos);
		m_ChannelNearBlur.SetValue(pos - m_NearFalloffDist.GetValue());

		pos = pc.GetZ() + m_FarFocusOffset.GetValue();
		m_ChannelFarFocus.SetValue(pos);
		m_ChannelFarBlur.SetValue(pos + m_FarFalloffDist.GetValue());
	}

	// Because the attachment object might move at any time without telling us,
	// we need to always operate
	this->MarkDirty();
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA cmraDriverFocusAttach::GetClipFillColor() const
{
	return maFloatRGBA( 0.7843f, 1.0f, 0.3569f, 1.0f );
}

//--------------------------------------------------------------------
//	Override base class to have this driver's Operate called at end.
//--------------------------------------------------------------------
//virtual 
bool cmraDriverFocusAttach::NeedsDelayedOperate()
{
	return true;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverFocusAttach::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverFocusAttach::ObjectNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
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
void cmraDriverFocusAttach::AttachNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Delay actual attachment, helps to make sure all named objects
	// are loaded and created.
	m_bNeedsAttach = true;

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maMatrix4x4 cmraDriverFocusAttach::GetCameraMatrix()
{
	nameString name;
	name.SetUID( this->m_cameraUID );

	int camindex = camsCameraMgr::GetIndexForName( name );
	DBG_ASSERT( camindex != -1, "Couldn't find the camera - " << name.GetString().c_str() );
	const camCamera* pCamera = camsCameraMgr::GetCamera(camindex);

	maMatrix4x4 cammat;
	pCamera->GetCameraMatrix(cammat);
	return cammat;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maAxisBox cmraDriverFocusAttach::XForm(const maAxisBox& i_bounds, const maMatrix4x4& i_xform)
{
	// temp storage to be reused on subsequent calls
	static maPoint3d l_box_pts[8];

	i_bounds.GetBoxPoints( l_box_pts );
	// transform bounds into camera space
	maAxisBox newBounds;
	for( int i = 0; i < 8; ++i )
	{
		i_xform.Transform( l_box_pts[i] );
		newBounds.Union( l_box_pts[i] );
	}
	return newBounds;
}

//--------------------------------------------------------------------
//  Show dialog that allows user to edit this driver's properties
//--------------------------------------------------------------------
void  cmraDriverFocusAttach::DoEditProperties()
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
