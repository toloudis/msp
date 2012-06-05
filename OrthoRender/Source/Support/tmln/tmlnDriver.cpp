/*****************************************************************************
**	tmlnDriver.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"
#include "Support/tmln/tmlnTimeEditUIInfo.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#include "ToolUIManaged/prtym/prtyFormControlBuilder.hpp"

#include <assert.h>


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriver::tmlnDriver()
:	m_Name("Name"), 
	m_BeginTime("Begin Time", 0.0f), 
	m_EndTime("End Time", 0.0f),
	m_Duration("Duration", 0.0f),
	m_Blend("Blend Type", tmlnDriver::e_NoBlending), 
	m_BlendTime("Blend Time", 0.0f), 
	m_EaseInWeight("Ease In Weight", 1.0f),
	m_EaseOutWeight("Ease Out Weight", 1.0f),
	m_bRestoreOriginal("Restore Original Value", false),
	m_bNeedsOperate(true),
	m_bAutoPopUpEditProperties(true),
	m_DriverId(0)
{
	m_bNotifyNotActive	= false;
	m_bNotifyActive		= true;

	// Fill enum for Blend Type
	m_Blend.SetEnumTag(0,"No Blending");
	m_Blend.SetEnumTag(1,"Previous");
	m_Blend.SetEnumTag(2,"Blend Time");
	m_Blend.SetEnumTag(3,"Overwrite");
	m_Blend.SetEnumTag(4,"Smooth Blend");

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new tmlnTimeEditUIInfo(&(m_BeginTime), "Basic", "Begin Time");
	AddProperty( pPUII );
	pPUII = new tmlnTimeEditUIInfo(&(m_EndTime), "Basic", "End Time");
	AddProperty( pPUII );
	pPUII = new tmlnTimeEditUIInfo(&(m_Duration), "Basic", "Duration");
	AddProperty( pPUII );
	prtyComboBoxUIInfo* pCBUII;
	pCBUII  = new prtyComboBoxUIInfo(&(m_Blend), "Basic", "Blend Type");
	AddProperty( pCBUII );
	pPUII = new tmlnTimeEditUIInfo(&(m_BlendTime), "Basic", "Blend Time");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_bRestoreOriginal), "Basic", "Restore Original Value");
	AddProperty( pPUII );
	prtyRangedFloatUIInfo* pRFUI = new prtyRangedFloatUIInfo(&(m_EaseInWeight), "Basic", "Ease In Weight");
	pRFUI->SetMinimum( 0.0f );
	pRFUI->SetMaximum( 40.0f );
	pRFUI->SetExponent( 3 );
	AddProperty( pRFUI );
	pRFUI = new prtyRangedFloatUIInfo(&(m_EaseOutWeight), "Basic", "Ease Out Weight");
	pRFUI->SetMinimum( 0.0f );
	pRFUI->SetMaximum( 40.0f );
	pRFUI->SetExponent( 3 );
	AddProperty( pRFUI );

	// Register callbacks to sync end time and duration redundancy
	m_BeginTime.AddCallback(new prtyCallbackWrapper<tmlnDriver>(this, &tmlnDriver::BeginTimeChanged));
	m_EndTime.AddCallback(new prtyCallbackWrapper<tmlnDriver>(this, &tmlnDriver::EndTimeChanged));
	m_Duration.AddCallback(new prtyCallbackWrapper<tmlnDriver>(this, &tmlnDriver::DurationChanged));

	// Register callbacks to mark dirty bit
	m_BeginTime.AddCallback(new prtyCallbackWrapper<tmlnDriver>(this, &tmlnDriver::PropertyChanged));
	m_Blend.AddCallback(new prtyCallbackWrapper<tmlnDriver>(this, &tmlnDriver::PropertyChanged));
	m_BlendTime.AddCallback(new prtyCallbackWrapper<tmlnDriver>(this, &tmlnDriver::PropertyChanged));
	m_bRestoreOriginal.AddCallback(new prtyCallbackWrapper<tmlnDriver>(this, &tmlnDriver::PropertyChanged));
	m_EaseInWeight.AddCallback(new prtyCallbackWrapper<tmlnDriver>(this, &tmlnDriver::PropertyChanged));
	m_EaseOutWeight.AddCallback(new prtyCallbackWrapper<tmlnDriver>(this, &tmlnDriver::PropertyChanged));

	// Get a driver id
	tmlnDriverIdMgr::SubmitDriver(this, this->m_DriverId);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriver::~tmlnDriver()
{
	// Unregister the driver id
	tmlnDriverIdMgr::RemoveDriver(this, this->m_DriverId);
}

//--------------------------------------------------------------------
// Return unique id for this driver.
//--------------------------------------------------------------------
tmlnDriverId tmlnDriver::GetDriverId() const
{
	return m_DriverId;
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse interacts with the clip.
//--------------------------------------------------------------------
//virtual 
std::string tmlnDriver::GetInteractionDescription(double i_InteractStart, double i_InteractDuration)
{
	char buffer[128];
	std::string time_start;
	std::string time_end;
	std::string time_duration;
	tmlnTimeUtil::GetTimeString((float)i_InteractStart, time_start);
	tmlnTimeUtil::GetTimeString((float)(i_InteractStart+i_InteractDuration), time_end);
	tmlnTimeUtil::GetTimeString((float)i_InteractDuration, time_duration);
	::sprintf(buffer, "%s\nstart %s\nend   %s\ndur   %s\n", m_Name.GetValue().c_str(), time_start.c_str(), time_end.c_str(), time_duration.c_str() );
	return std::string(buffer);
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string tmlnDriver::GetHoverDescription()
{
	char buffer[128];
	std::string time_start;
	std::string time_end;
	std::string time_duration;
	tmlnTimeUtil::GetTimeString(GetBeginTime(), time_start);
	if (GetDuration() > 0.0f)
	{
		tmlnTimeUtil::GetTimeString(GetEndTime(), time_end);
		tmlnTimeUtil::GetTimeString(GetDuration(), time_duration);
		::sprintf(buffer, "%s\nstart %s\nend   %s\r\ndur   %s\n", m_Name.GetValue().c_str(), time_start.c_str(), time_end.c_str(), time_duration.c_str() );
	}
	else
	{
		::sprintf(buffer, "%s\ntime %s\n", m_Name.GetValue().c_str(), time_start.c_str() );
	}
	return std::string(buffer);
}

//--------------------------------------------------------------------
//	ShowIcons - show or hide icons that are not part of real scene.
//--------------------------------------------------------------------
void tmlnDriver::ShowIcons( bool i_bVisible )
{
	// default does nothing
}


//--------------------------------------------------------------------
//	AlterKey - look at the values in the channels to which this 
//	driver is connected and alter the driver in order to match 
//	these values.
//	Returns true if this driver was able to alter its value.
//--------------------------------------------------------------------
bool tmlnDriver::AlterKey()
{
	//	Default behavior is to return false.
	return false;
}

//--------------------------------------------------------------------
//  Show dialog that allows user to edit this driver's properties
//--------------------------------------------------------------------
//virtual 
void  tmlnDriver::DoEditProperties()
{
	//	remove the tabs if any exist
	guiDialogTabbedMgr::RemoveTabPages("Driver");

	//	add a new tab based on our driver name
	std::string tab_name = this->GetName();
	if (tab_name.empty())
		tab_name = "Value";
	guiDialogTabbedMgr::AddTabPage("Driver", tab_name.c_str());

	//	sort the list and build the form
	SortListByCategory();
	guiDialogTabbedMgr::BuildForm("Driver", tab_name.c_str(), GetList(), true );

	if (this->IsAutoPopUpEditProperties())
		guiDialogTabbedMgr::Show("Driver");
}

//--------------------------------------------------------------------
//  This is called when a driver is selected. It lets the driver
//		select one of its icons or whatever else it wants to do.
//--------------------------------------------------------------------
//virtual 
void  tmlnDriver::DoSelect()
{
	// default behavior does nothing
}

//--------------------------------------------------------------------
//  Driver should select its 3D icon
//--------------------------------------------------------------------
//virtual 
void  tmlnDriver::DoSelectIcon()
{
	// default behavior does nothing
}

//--------------------------------------------------------------------
//	NotActive() - called ONCE when a driver goes from active to not
//	active.  This function needs to reset m_bNotifyNotActive so it
//	doesn't get anymore calls.
//--------------------------------------------------------------------
//virtual
void tmlnDriver::NotActive()
{
	m_bNotifyNotActive	= false;
	m_bNotifyActive		= true;
}

//--------------------------------------------------------------------
//	Active() - called ONCE when a driver first goes active.  This
//	function needs to reset m_bNotifyNotActive so it doesn't get
//	anymore calls.
//--------------------------------------------------------------------
//virtual
void tmlnDriver::Active()
{
	m_bNotifyActive		= false;
	m_bNotifyNotActive	= true;
}

//--------------------------------------------------------------------
//	Check if the time is before, within, or after the driver's time
//	including a small epsilon to handle round-off errors.
//--------------------------------------------------------------------
bool tmlnDriver::IsBefore(const float i_Time)
{
	const float c_EPSILON_BEGIN = (0.5f / g3dConstants::c_fDefaultFrameRate);
	return (i_Time < (m_BeginTime.GetValue() - c_EPSILON_BEGIN));
}
bool tmlnDriver::IsWithin(const float i_Time)
{
	const float c_EPSILON_BEGIN = (0.5f / g3dConstants::c_fDefaultFrameRate);
	const float c_EPSILON_END = 0.0f; //(0.5f / g3dConstants::c_fDefaultFrameRate);
	return (   (i_Time >= (m_BeginTime.GetValue() - c_EPSILON_BEGIN))
			&& (i_Time < (m_EndTime.GetValue() + c_EPSILON_END)));
}
bool tmlnDriver::IsAfter(const float i_Time)
{
	const float c_EPSILON_END = 0.0f; //(0.5f / g3dConstants::c_fDefaultFrameRate);
	return (i_Time >= (m_EndTime.GetValue() + c_EPSILON_END));
}

//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void tmlnDriver::GetResourceList( fsResourceTrackerData& io_List )
{
	//	the base class does nothing, so only drivers that have resources
	//	to add need do anything.
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriver::GetClipFillColor() const
{
	return maFloatRGBA( 1.0f, 0.9215f, 0.8431f, 1.0f );
}

//--------------------------------------------------------------------
// BeginTime
//--------------------------------------------------------------------
float  tmlnDriver::GetBeginTime() const
{
	return m_BeginTime.GetValue();
}
//virtual
void  tmlnDriver::SetBeginTime(float i_BeginTime, prtyProperty::UndoFlags i_Undoable)
{
	m_BeginTime.SetValue( i_BeginTime, i_Undoable );
	this->MarkDirty();
}

//--------------------------------------------------------------------
// EndTime
//--------------------------------------------------------------------
float  tmlnDriver::GetEndTime() const
{
	return m_EndTime.GetValue();
}
//virtual
void  tmlnDriver::SetEndTime(float i_EndTime, prtyProperty::UndoFlags i_Undoable)
{
	m_EndTime.SetValue( i_EndTime, i_Undoable );
	if (m_EndTime.GetValue() == 0.0f)
		m_EndTime.SetValue(0.0f);
	this->MarkDirty();
}


//--------------------------------------------------------------------
// A convenience function for setting the begin/end times of a newly
//	created driver.  It will create the driver so that it begins at
//	the current timeline time and lasts for the given duration.
//	If the duration is negative, then the duration will be set so
//	that the end time of driver equals the end time in the timeline.
//--------------------------------------------------------------------
void tmlnDriver::SetInitialTime(float i_Duration)
{
	float begin = tmlnTimeLine::GetValue();
	float end = tmlnTimeLine::GetMaximum();
	if (i_Duration >= 0)
		end = begin + i_Duration;

	this->SetBeginTime(begin);
	this->SetEndTime(end);
}

//--------------------------------------------------------------------
// BlendType - type of blending from old value to this driver
//--------------------------------------------------------------------
tmlnDriver::BlendType  tmlnDriver::GetBlendType() const
{
	return (tmlnDriver::BlendType)m_Blend.GetValue();
}
void  tmlnDriver::SetBlendType(BlendType i_BlendType, prtyProperty::UndoFlags i_Undoable)
{
	m_Blend.SetValue((int)i_BlendType, i_Undoable);
	this->MarkDirty();
}

//--------------------------------------------------------------------
// BlendTime - time to blend from last value to this driver.
//	This is only relevant for BlendType::e_BlendTime
//--------------------------------------------------------------------
float  tmlnDriver::GetBlendTime() const
{
	return m_BlendTime.GetValue();
}
void  tmlnDriver::SetBlendTime(float i_BlendTime, prtyProperty::UndoFlags i_Undoable)
{
	m_BlendTime.SetValue(i_BlendTime, i_Undoable);
	this->MarkDirty();
}

//--------------------------------------------------------------------
// EaseInWeight - modifier to tangent into this driver
//	This is only relevant for BlendType::e_SmoothBlend
//--------------------------------------------------------------------
float  tmlnDriver::GetEaseInWeight() const
{
	return m_EaseInWeight.GetValue();
}
void  tmlnDriver::SetEaseInWeight(float i_Weight, prtyProperty::UndoFlags i_Undoable)
{
	m_EaseInWeight.SetValue(i_Weight, i_Undoable);
	this->MarkDirty();
}

//--------------------------------------------------------------------
// EaseOutWeight - modifier to tangent out of this driver
//	This is only relevant for BlendType::e_SmoothBlend
//--------------------------------------------------------------------
float  tmlnDriver::GetEaseOutWeight() const
{
	return m_EaseOutWeight.GetValue();
}
void  tmlnDriver::SetEaseOutWeight(float i_Weight, prtyProperty::UndoFlags i_Undoable)
{
	m_EaseOutWeight.SetValue(i_Weight, i_Undoable);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//	RestoreOriginalValue - when driver is finished, should the
//	 value of the channels be restored to their "original" value,
//	 or should the final state of the driver remain?
//--------------------------------------------------------------------
bool tmlnDriver::IsRestoreOriginalValue() const
{
	return m_bRestoreOriginal.GetValue();
}
void tmlnDriver::SetRestoreOriginalValue(bool i_bRestore, prtyProperty::UndoFlags i_Undoable)
{
	m_bRestoreOriginal.SetValue(i_bRestore, i_Undoable);
	this->MarkDirty();
}

//--------------------------------------------------------------------
// GetBlendAlpha - get percentage from 0-1 for blending from
//	previous time to the given time, using this driver's values
//	for BeginTime, BlendType and BlendTime
//--------------------------------------------------------------------
float  tmlnDriver::GetBlendAlpha(float i_PrevTime, float i_Time) const
{
	tmlnDriver::BlendType blend_type = (tmlnDriver::BlendType)this->m_Blend.GetValue();
	switch (blend_type)
	{
	default:
	case tmlnDriver::e_NoBlending:
		return 0.0f;
	case tmlnDriver::e_Overwrite:
		return 1.0f;
	case tmlnDriver::e_BlendTime:
		if (this->m_BlendTime.GetValue() <= 0.0f)
			return 1.0f;
		else if (this->m_BeginTime.GetValue() < i_Time)
			return 0.0f;
		else
		{
			float diff = this->m_BeginTime.GetValue() - i_Time;
			if (diff > this->m_BlendTime.GetValue())
				return 0.0f;
			else
				return 1.0f - diff / this->m_BlendTime.GetValue();
		}
	case tmlnDriver::e_Previous:
	case tmlnDriver::e_SmoothBlend:
		{
			// When used with tmlnChannel::GetPreviousTime(), a small negative
			// number as the PreviousTime signifies that there is no previous
			// driver. In version 1.0, this would have meant a blend between 
			// the original value of the channel at time=0 and the driver. 
			// In version 2.0, this situation should now be handled as
			// "do not blend", and the start value of the driver should 
			// be set for all time before the begin time.
			if (i_PrevTime < 0)
				return 1.0f;

			// Otherwise, do usual blend between previous time and begin time
			float diff = i_Time - i_PrevTime;
			float blend_time = this->m_BeginTime.GetValue() - i_PrevTime;
			if (blend_time <= 0.0f)
				return 1.0f;

			float alpha = diff / blend_time;
			maFunctions::Clamp(alpha, 0.0f, 1.0f);

			if (blend_type == tmlnDriver::e_Previous)
				return alpha;

			// For smooth blending, apply cubic blend to the alpha
			// just calculated.  Note that this will create a situation
			// with flat tangents on both ends. A better blending is
			// possible in specific drivers, but this is the default
			// behavior in the base class.
			float cubic_alpha = 3*alpha*alpha - 2*alpha*alpha*alpha;
			maFunctions::Clamp(cubic_alpha, 0.0f, 1.0f);
			return cubic_alpha;
		}
	}
}

//--------------------------------------------------------------------
//	Some drivers, like attachment need to be delayed to make sure
//	that other drivers such as position and animation are updated
//	first. The default value of this function is false, override
//	this function in the derived class if you need to be delayed.
//--------------------------------------------------------------------
//virtual 
bool tmlnDriver::NeedsDelayedOperate()
{
	return false;
}

//--------------------------------------------------------------------
//	Split - split this driver at time i_fTime.
//	This function will clone the driver, change the appropriate
//	values in each driver instance and return the new driver that is 
//	the other half of the split.
//--------------------------------------------------------------------
void tmlnDriver::Split(tmlnDriver* io_pDriverAtEnd, float i_fTime)
{
	DBG_ASSERT0( io_pDriverAtEnd != 0, "cannot split with a NULL driver" );

	if (   (i_fTime > m_BeginTime.GetValue())
		&& (i_fTime < m_EndTime.GetValue()))
	{
		PerformSplit( io_pDriverAtEnd, i_fTime );
	}
}

//--------------------------------------------------------------------
//	PerformSplit - this is the heart of the split.  It is called by
//	Split() and it make the decisions on how to split up the
//	drivers.  It then sets the appropriate values for each driver.
//
//	Note: Special drivers will implement a version of this.
//--------------------------------------------------------------------
//virtual 
void tmlnDriver::PerformSplit( tmlnDriver* io_pDriverAtEnd, float i_fTime )
{
	float endtime = this->GetEndTime();

	//	change the end time of the "first" driver
	this->SetEndTime( i_fTime );

	//	change the begin time + name of the "second" driver
	io_pDriverAtEnd->SetBeginTime( i_fTime );
	io_pDriverAtEnd->SetEndTime( endtime );
	std::string name = io_pDriverAtEnd->GetName();
	std::string newname = name + "+";
	io_pDriverAtEnd->SetName( newname.c_str() );
}

//--------------------------------------------------------------------
//	calculate the percent this time is into the driver.
//--------------------------------------------------------------------
float tmlnDriver::CalculatePercent( float i_fTime )
{
	float duration = (m_EndTime.GetValue() - m_BeginTime.GetValue());
	if (duration > 0)
		return ((i_fTime - m_BeginTime.GetValue()) / duration);
	else
		return 0.0f;
}

//--------------------------------------------------------------------
// Get/Set base driver information for derived classes Get/Set
//	from tmlnDriverInfo
//--------------------------------------------------------------------
void tmlnDriver::GetBaseDriverInfo(tmlnDriverInfo& o_Info) const
{
	o_Info.m_Name = this->m_Name.GetValue();
	o_Info.m_BeginTime = this->m_BeginTime.GetValue();
	o_Info.m_EndTime = this->m_EndTime.GetValue();
	//if (m_EndTime == 0.0f)
	//	float x = 0.0f;		// for debug only
	o_Info.m_BlendType = this->m_Blend.GetValue();
	o_Info.m_BlendTime = this->m_BlendTime.GetValue();
	o_Info.m_bRestoreOriginal = this->m_bRestoreOriginal.GetValue();
	o_Info.m_EaseInWeight = this->m_EaseInWeight.GetValue();
	o_Info.m_EaseOutWeight = this->m_EaseOutWeight.GetValue();

	o_Info.m_DriverId = this->m_DriverId;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriver::SetBaseDriverInfo(const tmlnDriverInfo& i_Info, prtyProperty::UndoFlags i_Undoable)
{
	this->m_Name.SetValue( i_Info.m_Name, i_Undoable );
	this->m_BeginTime.SetValue( i_Info.m_BeginTime, i_Undoable );
	this->m_EndTime.SetValue( i_Info.m_EndTime, i_Undoable );
	//if (m_EndTime == 0.0f)
	//	m_EndTime = 0.0f;	// for debug only
	this->m_Blend.SetValue( (tmlnDriver::BlendType)i_Info.m_BlendType, i_Undoable );
	this->m_BlendTime.SetValue( i_Info.m_BlendTime, i_Undoable );
	this->m_bRestoreOriginal.SetValue( i_Info.m_bRestoreOriginal, i_Undoable );
	this->m_EaseInWeight.SetValue( i_Info.m_EaseInWeight, i_Undoable );
	this->m_EaseOutWeight.SetValue( i_Info.m_EaseOutWeight, i_Undoable );

	if ((i_Info.m_DriverId != 0) &&
		(this->m_DriverId != i_Info.m_DriverId))
	{
		// Unregister the driver id
		tmlnDriverIdMgr::RemoveDriver(this, this->m_DriverId);
		
		this->m_DriverId = i_Info.m_DriverId;

		// Register the new driver id
		tmlnDriverIdMgr::SubmitDriver(this, this->m_DriverId);
	}

	this->MarkDirty();
}

//--------------------------------------------------------------------
//	Add the base driver properties as a seperate tab to the driver
//	dialog.  This is only to be used for drivers that have a custom
//	dialog.
//--------------------------------------------------------------------
void tmlnDriver::AddBasePropertiesTab()
{
	//	add a new tab and get a pointer to it
	guiDialogTabbedMgr::AddTabPage("Driver", "Base");

	//	sort the list and build the form
	SortListByCategory();
	guiDialogTabbedMgr::BuildForm( "Driver", "Base", GetList(), true );
}


//--------------------------------------------------------------------
//	Value equal to the default type of blend for any driver
//
//	Note: Should the get return a prty instead of enum value?
//--------------------------------------------------------------------
//static 
void tmlnDriver::SetDefaultBlendType(const tmlnDriver::BlendType i_BlendType)
{
	//tmlnDriver::m_BlendDefault.SetValue( (envType::Int8)(i_BlendType) );
	//tmlnDriver::m_BlendDefault.SetValue( (tmlnDriver::BlendType)i_BlendType );	
}
//static 
const tmlnDriver::BlendType tmlnDriver::GetDefaultBlendType()
{
	//return (tmlnDriver::BlendType)tmlnDriver::m_BlendDefault.GetValue();
	return tmlnDriver::e_NoBlending;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriver::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriver::BeginTimeChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// If the begin time changes, move the end time also in order to
	// preserve the duration of the clip. In situations where the
	// begin and end time are being edited, the begin time call is first
	// so that we can then later set the end time explicitly.

	float end_time = this->m_BeginTime.GetValue() + this->m_Duration.GetValue();
	if (fabsf(end_time - this->m_EndTime.GetValue()) > maConstants::c_fEpsilon)
	{
		// undo was set when Begin was changed before, now we can do
		// the end time set quietly without undo
		this->m_EndTime.SetValue(end_time);
	}
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriver::EndTimeChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	float duration = this->m_EndTime.GetValue() - this->m_BeginTime.GetValue();
	if (fabsf(duration - this->m_Duration.GetValue()) > maConstants::c_fEpsilon)
	{
		// undo was set when EndTime was changed before, now we can do
		// the duration set quietly without undo
		this->m_Duration.SetValue(duration);
	}

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriver::DurationChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	float end_time = this->m_BeginTime.GetValue() + this->m_Duration.GetValue();
	if (fabsf(end_time - this->m_EndTime.GetValue()) > maConstants::c_fEpsilon)
	{
		// undo was set when Duration was changed before, now we can do
		// the end time set quietly without undo
		this->m_EndTime.SetValue(end_time);
	}

	// only need to mark dirty if end time was changed, which is already
	// handled in the EndTimeChanged callback
}

