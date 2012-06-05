/*****************************************************************************
**	tmlnDriverAttachOrient.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Drivers/Attach/tmlnDriverAttachOrient.hpp"

#include "Drivers/Attach/tmlnDriverAttachOrientInfo.hpp"

#include "Support/mnm/mnmObject.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"

#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAttachOrient::tmlnDriverAttachOrient(tmlnChannelPosition &i_PositionChannel, 
											   tmlnChannelOrientation &i_OrientationChannel, 
											   chDefs::Name i_ChunkName)
:	tmlnDriverAttachBase(i_PositionChannel.GetPosition()),
	m_ChannelPosition(i_PositionChannel), 
	m_ChannelOrientation(i_OrientationChannel), 
	m_ChunkName(i_ChunkName),
	m_AttachOrientation("Attach Orientation",maRotation(0.0f,0.0f,0.0f)), 
	m_LastPosition(i_PositionChannel.GetPosition())
{

	prtyVector3dEditUpDownUIInfo* pPVEUDUII  = new prtyVector3dEditUpDownUIInfo(&(m_AttachOrientation), "Attach", "Offset Orientation");
	pPVEUDUII->SetIncrement(0.5f, 0.5f, 0.5f);
	pPVEUDUII->SetDecimalPlaces(1);
	AddProperty( pPVEUDUII );	

	//	property callbacks
	m_AttachOrientation.AddCallback(new prtyCallbackWrapper<tmlnDriverAttachOrient>(this, &tmlnDriverAttachOrient::PropertyChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAttachOrient::~tmlnDriverAttachOrient()
{
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverAttachOrient::GetDriverInfo() const
{
	tmlnDriverAttachOrientInfo *pInfo = new tmlnDriverAttachOrientInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_ObjectName = this->m_ObjectName.GetValue();
	pInfo->m_AttachName = this->m_AttachName.GetValue();
	pInfo->m_AttachOffset = this->m_AttachOffset.GetValue();
	pInfo->m_AttachOrientation = this->m_AttachOrientation.GetQuaternion();
	pInfo->m_WorldSpaceOffset = this->m_WorldSpaceOffset.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void tmlnDriverAttachOrient::SetDriverInfo(	const tmlnDriverAttachOrientInfo& i_Info )
{
	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// Store local info
	this->m_ObjectName.SetValue( i_Info.m_ObjectName );
	this->m_AttachName.SetValue( i_Info.m_AttachName );
	this->m_AttachOffset.SetValue( i_Info.m_AttachOffset );
	this->m_AttachOrientation.SetQuaternion( i_Info.m_AttachOrientation );
	this->m_WorldSpaceOffset.SetValue( i_Info.m_WorldSpaceOffset );

	// Delay actual attachment, helps to make sure all named objects
	// are loaded and created.
	m_bNeedsAttach = true;

	// If this is a set from the GUI related to attaching then 
	// try to preserve the position of the object by 
	// setting the world offset accordingly.
	m_bPreservePosition = false;
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverAttachOrient::Operate(const maTime& i_Time)
{
	// Confirm that our attachment is ready to use.
	this->ConfirmAttachment();

	// Set our offset orientation into the reference - base class only sets position
	m_Reference.SetAttachOrientation( this->m_AttachOrientation.GetQuaternion() );

	// If preserving position adjust the WorldSpaceOffset to 
	// keep the goal position the same.
	if (m_bPreservePosition)
	{
		maVector3d diff = m_LastPosition - m_Reference.GetPosition();
		m_WorldSpaceOffset = diff;
		m_bPreservePosition = false;
	}

	maPoint3d goal_pos = m_Reference.GetPosition() + m_WorldSpaceOffset.GetValue();
	maRotation goal_rot = m_Reference.GetOrientation();

	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		// Position blend
		maPoint3d cur_pos = m_ChannelPosition.GetPosition();
		float pos_pct = this->GetBlendAlpha(m_ChannelPosition.GetPreviousTime(i_Time), i_Time);
		maPoint3d pos = goal_pos*pos_pct + cur_pos*(1.0f - pos_pct);	// linear blend
		m_ChannelPosition.SetPosition(pos);

		// Rotation blend
		maRotation cur_rot = m_ChannelOrientation.GetQuaternion();
		float rot_pct = this->GetBlendAlpha(m_ChannelOrientation.GetPreviousTime(i_Time), i_Time);
		maRotation rot;
		rot.Slerp(cur_rot, goal_rot, rot_pct);	// sphereical linear blend
		m_ChannelOrientation.SetQuaternion(rot);
	}
	//	This else is commented out after trying to get the attachment to 
	//	detach after the driver is done.
	//
	//	The problem is that if the attachment driver is from frames 10 to 20 and 
	//	the current time is at 30, then the attachment driver would have to 
	//	position the channel at the position of the attachment at a time that 
	//	is not the current time and it is not possible for the driver to know 
	//	where the attachment would be at time 20.
	//
	//	You might think that you could just check the current time against the 
	//	end time and not set the value if it is after the end time, but this 
	//	would not be correct. It would be necessary to set the time that it 
	//	would have been if it was at time 20. The user can scrub around the 
	//	timeline in any order - not just sequentially and we can't have drivers
	//	that rely on the sequential execution of the timeline.
	//
	//else if (this->IsAfter(i_Time))
	//{
	//	//	do nothing
	//}
	else
	{
		// within driver range
		//float percent = (i_Time - this->GetBeginTime()) / this->GetDuration();
		m_ChannelPosition.SetPosition(goal_pos);
		m_ChannelOrientation.SetQuaternion(goal_rot);
	}

	m_LastPosition = m_ChannelPosition.GetPosition();

	// Because the attachment object might move at any time without telling us,
	// we need to always operate
	this->MarkDirty();
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverAttachOrient::GetClipFillColor() const
{
	return maFloatRGBA( 0.7843f, 1.0f, 0.3569f, 1.0f );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAttachOrient::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}
