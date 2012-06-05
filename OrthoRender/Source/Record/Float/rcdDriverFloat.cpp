/*****************************************************************************
**	rcdDriverFloat.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Record/Float/rcdDriverFloat.hpp"

#include "Support/tmln/tmlnChannelRangedFloat.hpp"
#include "Record/Float/rcdDriverFloatInfo.hpp"
#include "Record/Float/rcdFloatDialogUtil.hpp"

#include "Core/dbg/dbgLog.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rcdDriverFloat::rcdDriverFloat(tmlnChannelRangedFloat &i_Channel, chDefs::Name i_ChunkName)
:m_Channel(i_Channel), m_Keys(i_Channel.GetOriginalValue()), m_ChunkName(i_ChunkName)
{
	this->SetBlendType(tmlnDriver::GetDefaultBlendType());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rcdDriverFloat::~rcdDriverFloat()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string rcdDriverFloat::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();
	return desc;
}

//--------------------------------------------------------------------
//	Key Accessors
//--------------------------------------------------------------------
float rcdDriverFloat::GetValue(float i_Time) const
{
	float time = i_Time - this->GetBeginTime();
	return this->m_Keys.GetValue(time);
}
void rcdDriverFloat::SetValue(float i_Time, float i_Val)
{
	float time = i_Time - this->GetBeginTime();
	int ki = this->m_Keys.GetLowerKeyIndex(time);
	if (m_Keys.GetKeyTime(ki) == time)
	{
		// Already existing key, so alter data
		m_Keys.AlterKey(ki, i_Val);
	}
	else
	{
		// Add new key
		m_Keys.AddKey(time, i_Val);
	}
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  rcdDriverFloat::GetDriverInfo() const
{
	rcdDriverFloatInfo *pInfo = new rcdDriverFloatInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_Keys = this->m_Keys;

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void rcdDriverFloat::SetDriverInfo(const rcdDriverFloatInfo& i_Info, prtyProperty::UndoFlags i_Undoable)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	// Store local info
	this->m_Keys = i_Info.m_Keys;
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  rcdDriverFloat::Operate(float i_Time)
{
	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		float goal = this->m_Keys.GetValue(0);
		float cur = m_Channel.GetValue();
		float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
		float value = goal*percent + cur*(1.0f - percent);	// linear blend
		m_Channel.SetValue(value);
	}
	else
	{
		// within driver range
		float time = i_Time - this->GetBeginTime();
		float goal = this->m_Keys.GetValue(time);
		m_Channel.SetValue(goal);
	}
}


//--------------------------------------------------------------------
//  Show dialog that allows user to edit this driver's properties
//--------------------------------------------------------------------
void  rcdDriverFloat::DoEditProperties()
{
	rcdFloatDialogUtil::Show(*this);
}
