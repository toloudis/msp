/*****************************************************************************
**	tmlnDriverAttach.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003-7 - All Rights Reserved
\****************************************************************************/
#include "Drivers/Attach/tmlnDriverAttach.hpp"

#include "Drivers/Attach/tmlnDriverAttachInfo.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"

//============================================================================
//============================================================================
namespace
{
}

//--------------------------------------------------------------------
// Take the position in the channel and set that as the world 
// space offset so that the attached object's position remains
// in place.
//--------------------------------------------------------------------
tmlnDriverAttach::tmlnDriverAttach(tmlnChannelPosition &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverAttachBase(i_Channel.GetPosition()),
	m_Channel(i_Channel), 
	m_ChunkName(i_ChunkName), 
	m_LastPosition(i_Channel.GetPosition())
{
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

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void tmlnDriverAttach::SetDriverInfo(	const tmlnDriverAttachInfo& i_Info, 
										prtyProperty::UndoFlags i_Undoable)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	// Store local info
	this->m_ObjectName.SetValue(i_Info.m_ObjectName, i_Undoable);
	this->m_AttachName.SetValue(i_Info.m_AttachName, i_Undoable);
	this->m_AttachOffset.SetValue(i_Info.m_AttachOffset, i_Undoable);
	this->m_WorldSpaceOffset.SetValue(i_Info.m_WorldSpaceOffset, i_Undoable);

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
void  tmlnDriverAttach::Operate(float i_Time)
{
	// Confirm that our attachment is ready to use.
	this->ConfirmAttachment();

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
		maPoint3d cur = m_Channel.GetPosition();
		float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
		maPoint3d pos = goal*percent + cur*(1.0f - percent);	// linear blend
		m_Channel.SetPosition(pos);
	}
	else
	{
		// within driver range
		//float percent = (i_Time - this->GetBeginTime()) / this->GetDuration();
		maPoint3d pos = m_Reference.GetPosition() + m_WorldSpaceOffset.GetValue();
		m_Channel.SetPosition(pos);
	}

	m_LastPosition = m_Channel.GetPosition();

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
