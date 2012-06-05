/*****************************************************************************
**	tmlnDriverAttach.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003-7 - All Rights Reserved
\****************************************************************************/
#include "Drivers/Attach/tmlnDriverAttach.hpp"

#include "Drivers/Attach/tmlnDriverAttachInfo.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"

#include "Core/prty/prtyCheckBoxUIInfo.hpp"


//--------------------------------------------------------------------
// Take the position in the channel and set that as the world 
// space offset so that the attached object's position remains
// in place.
//--------------------------------------------------------------------
tmlnDriverAttach::tmlnDriverAttach(tmlnChannelPosition &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverAttachBase(i_Channel.GetPosition()),
	m_Channel(i_Channel), 
	m_ChunkName(i_ChunkName), 
	m_LastPosition(i_Channel.GetPosition()),
	m_bUseBoundingBox("Use Bounding Box", true)
{
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyCheckBoxUIInfo(&(m_bUseBoundingBox), "Attach", "Attach to center of bounding box");
	AddProperty( pPUII );

	m_bUseBoundingBox.AddCallback(new prtyCallbackWrapper<tmlnDriverAttach>(this, &tmlnDriverAttach::UseBBoxChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAttach::~tmlnDriverAttach()
{
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverAttach::GetDriverInfo() const
{
	tmlnDriverAttachInfo *pInfo = new tmlnDriverAttachInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_ObjectName = this->m_ObjectName.GetValue();
	pInfo->m_AttachName = this->m_AttachName.GetValue();
	pInfo->m_AttachOffset = this->m_AttachOffset.GetValue();
	pInfo->m_WorldSpaceOffset = this->m_WorldSpaceOffset.GetValue();
	pInfo->m_bUseBbox = this->m_bUseBoundingBox.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void tmlnDriverAttach::SetDriverInfo(	const tmlnDriverAttachInfo& i_Info )
{
	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// Store local info
	this->m_ObjectName.SetValue(i_Info.m_ObjectName);
	this->m_AttachName.SetValue(i_Info.m_AttachName);
	this->m_AttachOffset.SetValue(i_Info.m_AttachOffset);
	this->m_WorldSpaceOffset.SetValue(i_Info.m_WorldSpaceOffset);
	this->m_bUseBoundingBox.SetValue(i_Info.m_bUseBbox);

	// Delay actual attachment, helps to make sure all named objects
	// are loaded and created.
	m_bNeedsAttach = true;

	// If we are coming through SetDriverInfo, then we know that this
	// is a set from the file format, so we should not try to preserve the position
	// (the callbacks may have changed it when we set the object and attachment node
	// properties above).
	m_bPreservePosition = false;
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverAttach::Operate(const maTime& i_Time)
{
	// All attachment animation is done in world space
	const tmlnChannelPosition::Space c_Space = tmlnChannelPosition::e_WorldSpace;

	// Confirm that our attachment is ready to use.
	this->ConfirmAttachment();
	m_Reference.SetUseBoundingBox(m_bUseBoundingBox.GetValue());

	// If preserving position adjust the WorldSpaceOffset to 
	// keep the goal position the same.
	if (m_bPreservePosition)
	{
		maVector3d diff = m_LastPosition - m_Reference.GetPosition();
		m_WorldSpaceOffset = diff;
		m_bPreservePosition = false;
	}

	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		maPoint3d goal = m_Reference.GetPosition() + m_WorldSpaceOffset.GetValue();
		maPoint3d cur = m_Channel.GetPosition(c_Space);
		float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
		maPoint3d pos = goal*percent + cur*(1.0f - percent);	// linear blend
		m_Channel.SetPosition(pos, c_Space);
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
		maPoint3d pos = m_Reference.GetPosition() + m_WorldSpaceOffset.GetValue();
		m_Channel.SetPosition(pos, c_Space);
	}

	m_LastPosition = m_Channel.GetPosition(c_Space);

	// Because the attachment object might move at any time without telling us,
	// we need to always operate
	this->MarkDirty();
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverAttach::GetClipFillColor() const
{
	return maFloatRGBA( 0.9843f, 1.0f, 0.5569f, 1.0f );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAttach::UseBBoxChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	m_Reference.SetUseBoundingBox( this->m_bUseBoundingBox.GetValue() );

	if (i_bDirty)
	{
		// If this is a set from the GUI related to attaching then 
		// try to preserve the position of the object by 
		// setting the world offset accordingly.
		m_bPreservePosition = sm_bPreservePosition.GetValue();
	}

	this->MarkDirty();
}
