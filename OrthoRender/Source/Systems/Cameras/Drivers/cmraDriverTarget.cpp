/*****************************************************************************
**	cmraDriverTarget.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverTarget.hpp"

#include "Systems/Cameras/Drivers/cmraDriverTargetInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverTargetParser.hpp"

//	App
#include "Support/mnm/mnmObject.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"

//	Library
#include "Core/Name/NameMgr.hpp"
#include "Core/Name/NameObject.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverTarget::cmraDriverTarget(tmlnChannelPosition &i_Channel)
:	m_Channel(i_Channel), 
	m_TargetOffset("Target Offset", maPoint3d(0,0,0)),
	m_ObjectName("Object Name"),
	m_AttachName("Attach Name")
{
	// TODO UIINFO
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyPropertyUIInfo(&(m_TargetOffset), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_ObjectName), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_AttachName), "category", "description");
	AddProperty( pPUII );

	//	property callbacks
	m_TargetOffset.AddCallback(new prtyCallbackWrapper<cmraDriverTarget>(this, &cmraDriverTarget::PropertyChanged));
	m_ObjectName.AddCallback(new prtyCallbackWrapper<cmraDriverTarget>(this, &cmraDriverTarget::PropertyChanged));
	m_AttachName.AddCallback(new prtyCallbackWrapper<cmraDriverTarget>(this, &cmraDriverTarget::PropertyChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverTarget::~cmraDriverTarget()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string cmraDriverTarget::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	std::string str = "Target : ";
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
tmlnDriverInfo*  cmraDriverTarget::GetDriverInfo() const
{
	cmraDriverTargetInfo *pInfo = new cmraDriverTargetInfo(cmraDriverTargetParser::GetChunkName());

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_ObjectName = this->m_ObjectName.GetValue();
	pInfo->m_AttachName = this->m_AttachName.GetValue();
	pInfo->m_TargetOffset = this->m_TargetOffset.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void cmraDriverTarget::SetDriverInfo(	const cmraDriverTargetInfo& i_Info, 
										prtyProperty::UndoFlags i_Undoable)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	// Store local info
	this->m_ObjectName.SetValue( i_Info.m_ObjectName, i_Undoable );
	this->m_AttachName.SetValue( i_Info.m_AttachName, i_Undoable );
	this->m_TargetOffset.SetValue( i_Info.m_TargetOffset, i_Undoable );

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

	//	attach the object
	//
	nameObject* obj = nameMgr::GetObjectByName(i_Info.m_ObjectName);	// name of prop to target
	if (obj && dynamic_cast<mnmObject*>(obj))
	{
		mnmObject *pObj = dynamic_cast<mnmObject*>(obj);
		api3dReference *pRef = pObj->GetReference(i_Info.m_AttachName.c_str()); // reference node name
//		DBG_LOG1("Got mnmObject, and reference?: %s", (pRef ? "True" : "NULL"));
		m_Reference.AttachTo(pRef, i_Info.m_TargetOffset);
	}
	else m_Reference.AttachTo(NULL, i_Info.m_TargetOffset);
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  cmraDriverTarget::Operate(float i_Time)
{
	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		maPoint3d goal = m_Reference.GetPosition();
		maPoint3d cur = m_Channel.GetPosition();
		float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
		maPoint3d pos = goal*percent + cur*(1.0f - percent);	// linear blend
		m_Channel.SetPosition(pos);
	}
	else
	{
		// within driver range
		float percent = (i_Time - this->GetBeginTime()) / this->GetDuration();
		maPoint3d pos = m_Reference.GetPosition();;
		m_Channel.SetPosition(pos);
	}
}


//--------------------------------------------------------------------
//  Get list of possible reference attachment points.
//--------------------------------------------------------------------
void  cmraDriverTarget::GetReferenceList(std::vector<std::string> &o_List)
{
	nameObject* pNameObj = nameMgr::GetObjectByName( this->m_ObjectName.GetValue() );
	if (pNameObj)
	{
		mnmObject *pObj = dynamic_cast<mnmObject*>(pNameObj);
		if (pObj)
			pObj->GetReferenceList(o_List);
	}
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA cmraDriverTarget::GetClipFillColor() const
{
	return maFloatRGBA( 0.6941f, 0.5412f, 1.0f, 1.0f );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverTarget::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}
