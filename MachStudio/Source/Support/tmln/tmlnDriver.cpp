/*****************************************************************************
**	tmlnDriver.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnDriver.hpp"

#include "Support/tmln/tmlnDriverDialogUtil.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"
#include "Support/tmln/tmlnTimeEditUIInfo.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"

#include <assert.h>


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	// Relationship based property reference - it uses the
	//	relObject base class to create a relObjectReference to find
	//	the object first and then look for the property by name.
	//--------------------------------------------------------------------
	class DriverPropertyReference : public prtyPropertyReference
	{
		public:
			DriverPropertyReference(tmlnDriver& i_Driver,
									 const std::string& i_PropertyName)
				: m_PropertyName(i_PropertyName) 
			{
				m_ObjectReference = i_Driver.CreateReferenceToSelf();
			}

			virtual prtyProperty* GetProperty()
			{
				relObject *pObject = m_ObjectReference->GetObject();
				if (pObject)
				{
					prtyObject *pPrtyObj = dynamic_cast<prtyObject*>(pObject);
					if (pPrtyObj)
					{
						const prtyProperty* pProperty = pPrtyObj->GetProperty(m_PropertyName);
						if (pProperty)
						{
							return const_cast<prtyProperty*>(pProperty);
						}
					}
				}
				DBG_WARNING("Could not resolve property" << m_PropertyName);
				return NULL;
			}
		private:
			shared_ptr<relObjectReference> m_ObjectReference;
			std::string m_PropertyName;
	};
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriver::tmlnDriver()
:	m_Name("Name"), 
	m_BeginTime("Begin Time"), 
	m_EndTime("End Time"),
	m_Duration("Duration"),
	m_Blend("Blend Type", tmlnDriver::e_NoBlending), 
	m_BlendTime("Blend Time"), 
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

	// Add this control as a time interest
	tmlnTimeLine::AddTimeInterest(this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriver::~tmlnDriver()
{
	// Unregister time interest
	tmlnTimeLine::RemoveTimeInterest(this);

	// Unregister the driver id
	tmlnDriverIdMgr::RemoveDriver(this, this->m_DriverId);
}

//--------------------------------------------------------------------
// Name of the driver, displayed in the trax editor
//--------------------------------------------------------------------
void tmlnDriver::SetName(const char* i_Name)
{
	if (_stricmp(i_Name, m_Name.GetValue().c_str()) != 0)
		m_Name.SetValue(i_Name);
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
std::string tmlnDriver::GetInteractionDescription(const maTime& i_InteractStart, const maTime& i_InteractDuration)
{
	//char buffer[128];
	std::string time_start;
	std::string time_end;
	std::string time_duration;
	tmlnTimeUtil::GetTimeString(i_InteractStart, time_start);
	tmlnTimeUtil::GetTimeString((i_InteractStart+i_InteractDuration), time_end);
	tmlnTimeUtil::GetTimeString(i_InteractDuration, time_duration);
	//::sprintf(buffer, "%s\nstart %s\nend   %s\ndur   %s\n", m_Name.GetValue().c_str(), time_start.c_str(), time_end.c_str(), time_duration.c_str() );
	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss << m_Name.GetValue()<<"\nstart   "<< time_start<<"\nend   "<<time_end<<"\ndur    "<<time_duration<<"\n";
	std::string buffer(oss.str());
	return buffer;
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string tmlnDriver::GetHoverDescription()
{
	std::string buffer;
	
	std::string time_start;
	std::string time_end;
	std::string time_duration;
	tmlnTimeUtil::GetTimeString(GetBeginTime(), time_start);
	maTime duration = this->GetDuration();
	if (duration.GetValue() > 0)
	{
		tmlnTimeUtil::GetTimeString(GetEndTime(), time_end);
		tmlnTimeUtil::GetTimeString(GetDuration(), time_duration);
		//::sprintf(buffer, "%s\nstart %s\nend   %s\r\ndur   %s\n", m_Name.GetValue().c_str(), time_start.c_str(), time_end.c_str(), time_duration.c_str() );
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss << m_Name.GetValue()<<"\nstart   "<< time_start<<"\nend   "<<time_end<<"\ndur    "<<time_duration<<"\n";
		//std::string buffer(oss.str());
		buffer = oss.str();
	
	}
	else
	{
		//::sprintf(buffer, "%s\ntime %s\n", m_Name.GetValue().c_str(), time_start.c_str() );
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss << m_Name.GetValue()<<"\ntime   "<< time_start<<"\n";
		//std::string buffer(oss.str());
		buffer = oss.str();
	}
	return buffer;
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
	tmlnDriverDialogUtil::EditDriverProperties(this);
}

//--------------------------------------------------------------------
//	CreateReferenceForProperty - given a property, create a
//	shared_ptr to a prtyPropertyReference to this property.
//--------------------------------------------------------------------
//virtual 
shared_ptr<prtyPropertyReference> tmlnDriver::CreateReferenceForProperty(prtyProperty& i_Property)
{
	shared_ptr<prtyPropertyReference> relative_reference(
		new DriverPropertyReference(*this, i_Property.GetPropertyName()));
	return relative_reference;
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
bool tmlnDriver::IsBefore(const maTime& i_Time) const
{
	// float comparison with epsilon
	//const float c_EPSILON_BEGIN = (0.5f / g3dConstants::c_fDefaultFrameRate);
	//return (i_Time < (m_BeginTime.GetValue() - c_EPSILON_BEGIN));
	
	//TIME - can maTime comparison be exact?
	return (i_Time < m_BeginTime.GetValue());
}
bool tmlnDriver::IsWithin(const maTime& i_Time) const
{
	// float comparison with epsilon
	//const float c_EPSILON_BEGIN = (0.5f / g3dConstants::c_fDefaultFrameRate);
	//const float c_EPSILON_END = 0.0f; //(0.5f / g3dConstants::c_fDefaultFrameRate);
	//return (   (i_Time >= (m_BeginTime.GetValue() - c_EPSILON_BEGIN))
	//		&& (i_Time < (m_EndTime.GetValue() + c_EPSILON_END)));
	
	//TIME - can maTime comparison be exact?
	return ((i_Time >= m_BeginTime.GetValue()) && (i_Time <= m_EndTime.GetValue()));
}
bool tmlnDriver::IsAfter(const maTime& i_Time) const
{
	// float comparison with epsilon
	//const float c_EPSILON_END = 0.0f; //(0.5f / g3dConstants::c_fDefaultFrameRate);
	//return (i_Time >= (m_EndTime.GetValue() + c_EPSILON_END));

	//TIME - can maTime comparison be exact?
	return (i_Time > m_EndTime.GetValue());
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
//	Remap internal name attachments using the given map.
//  This is part of the duplication process and makes sures 
//	internal attachments are passed onto the duplicated objects.
//--------------------------------------------------------------------
//virtual 
void tmlnDriver::RemapNames(const std::map<nameString, nameString> &i_DuplicateNameMap)
{
	//	the base class does nothing, so only drivers that have named attachments
	//	need do anything.
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
const maTime&  tmlnDriver::GetBeginTime() const
{
	return m_BeginTime.GetValue();
}
//virtual
void  tmlnDriver::SetBeginTime(const maTime& i_BeginTime)
{
	m_BeginTime.SetValue( i_BeginTime );
	this->MarkDirty();
}

//--------------------------------------------------------------------
// EndTime
//--------------------------------------------------------------------
const maTime&  tmlnDriver::GetEndTime() const
{
	return m_EndTime.GetValue();
}
//virtual
void  tmlnDriver::SetEndTime(const maTime& i_EndTime)
{
	m_EndTime.SetValue( i_EndTime );
	//bga - this was the float comparison before, but it doesn't make any sense to me...
	//if (m_EndTime.GetValue() == 0.0f)
	//	m_EndTime.SetValue(0.0f);
	if (m_EndTime.GetValue() == maTime::c_ZeroTime)
		m_EndTime.SetValue(maTime::c_ZeroTime);
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
	maTime begin = tmlnTimeLine::GetValue();
	maTime end = tmlnTimeLine::GetMaximum();
	if (i_Duration >= 0)
		end = begin + maTime::FromSeconds(i_Duration); //TIME - should this function take a maTime argument?

	this->SetBeginTime(begin);
	this->SetEndTime(end);
}

//--------------------------------------------------------------------
// Duration is end time - begin time
//--------------------------------------------------------------------
maTime tmlnDriver::GetDuration() const
{
	return (m_EndTime.GetValue() - m_BeginTime.GetValue()) + tmlnTimeLine::GetFrameIncrement();
}

//--------------------------------------------------------------------
// BlendType - type of blending from old value to this driver
//--------------------------------------------------------------------
tmlnDriver::BlendType  tmlnDriver::GetBlendType() const
{
	return (tmlnDriver::BlendType)m_Blend.GetValue();
}
void  tmlnDriver::SetBlendType(BlendType i_BlendType)
{
	m_Blend.SetValue((int)i_BlendType);
	this->MarkDirty();
}

//--------------------------------------------------------------------
// BlendTime - time to blend from last value to this driver.
//	This is only relevant for BlendType::e_BlendTime
//--------------------------------------------------------------------
const maTime&  tmlnDriver::GetBlendTime() const
{
	return m_BlendTime.GetValue();
}
void  tmlnDriver::SetBlendTime(const maTime& i_BlendTime)
{
	m_BlendTime.SetValue(i_BlendTime);
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
void  tmlnDriver::SetEaseInWeight(float i_Weight)
{
	m_EaseInWeight.SetValue(i_Weight);
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
void  tmlnDriver::SetEaseOutWeight(float i_Weight)
{
	m_EaseOutWeight.SetValue(i_Weight);
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
void tmlnDriver::SetRestoreOriginalValue(bool i_bRestore)
{
	m_bRestoreOriginal.SetValue(i_bRestore);
	this->MarkDirty();
}

//--------------------------------------------------------------------
// GetBlendAlpha - get percentage from 0-1 for blending from
//	previous time to the given time, using this driver's values
//	for BeginTime, BlendType and BlendTime
//--------------------------------------------------------------------
float  tmlnDriver::GetBlendAlpha(const maTime& i_PrevTime, const maTime& i_Time) const
{
	tmlnDriver::BlendType blend_type = (tmlnDriver::BlendType)this->m_Blend.GetValue();
	maTime begin_time = this->m_BeginTime.GetValue();
	switch (blend_type)
	{
	default:
	case tmlnDriver::e_NoBlending:
		return 0.0f;
	case tmlnDriver::e_Overwrite:
		return 1.0f;
	case tmlnDriver::e_BlendTime:
		{
			maTime blend_time = this->m_BlendTime.GetValue();
			if (blend_time <= maTime::c_ZeroTime)
				return 1.0f;
			else if (begin_time < i_Time)
				return 0.0f;
			else
			{
				maTime diff = begin_time - i_Time;
				if (diff > blend_time)
					return 0.0f;
				else
				{
					return 1.0f - (diff / blend_time);
				}
			}
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
			if (i_PrevTime < maTime::c_ZeroTime)
				return 1.0f;

			// Otherwise, do usual blend between previous time and begin time
			maTime diff = i_Time - i_PrevTime;
			maTime blend_time = begin_time - i_PrevTime;
			if (blend_time <= maTime::c_ZeroTime)
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
void tmlnDriver::Split(tmlnDriver* io_pDriverAtEnd, const maTime& i_Time)
{
	DBG_ASSERT( io_pDriverAtEnd != 0, "cannot split with a NULL driver" );

	if (   (i_Time > m_BeginTime.GetValue())
		&& (i_Time < m_EndTime.GetValue()))
	{
		PerformSplit( io_pDriverAtEnd, i_Time );
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
void tmlnDriver::PerformSplit( tmlnDriver* io_pDriverAtEnd, const maTime& i_Time )
{
	maTime endtime = this->GetEndTime();

	//	change the end time of the "first" driver
	this->SetEndTime( i_Time );

	//	change the begin time + name of the "second" driver
	io_pDriverAtEnd->SetBeginTime( i_Time );
	io_pDriverAtEnd->SetEndTime( endtime );
	std::string name = io_pDriverAtEnd->GetName();
	std::string newname = name + "+";
	io_pDriverAtEnd->SetName( newname.c_str() );
}

//--------------------------------------------------------------------
//	calculate the percent this time is into the driver.
//--------------------------------------------------------------------
float tmlnDriver::CalculatePercent( const maTime& i_Time )
{
	maTime duration = (m_EndTime.GetValue() - m_BeginTime.GetValue()) + tmlnTimeLine::GetFrameIncrement();
	if (duration > maTime::c_ZeroTime)
	{
		return ((i_Time - m_BeginTime.GetValue()) / duration);
	}
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
	o_Info.m_ChannelName = this->m_ChannelName;
	//TIME - using old file format and converting between seconds and maTime
	o_Info.m_BeginTime = this->m_BeginTime.GetValue().AsSeconds();
	o_Info.m_EndTime = this->m_EndTime.GetValue().AsSeconds();
	o_Info.m_BlendType = this->m_Blend.GetValue();
	//TIME - using old file format and converting between seconds and maTime
	o_Info.m_BlendTime = this->m_BlendTime.GetValue().AsSeconds();
	o_Info.m_bRestoreOriginal = this->m_bRestoreOriginal.GetValue();
	o_Info.m_EaseInWeight = this->m_EaseInWeight.GetValue();
	o_Info.m_EaseOutWeight = this->m_EaseOutWeight.GetValue();

	o_Info.m_DriverId = this->m_DriverId;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriver::SetBaseDriverInfo(const tmlnDriverInfo& i_Info)
{
	this->m_Name.SetValue( i_Info.m_Name );
	this->m_ChannelName = i_Info.m_ChannelName;
	//TIME - using old file format and converting between seconds and maTime
	this->m_BeginTime.SetValue( maTime::FromSeconds(i_Info.m_BeginTime) );
	this->m_EndTime.SetValue( maTime::FromSeconds(i_Info.m_EndTime) );
	this->m_Blend.SetValue( (tmlnDriver::BlendType)i_Info.m_BlendType );
	this->m_BlendTime.SetValue( maTime::FromSeconds(i_Info.m_BlendTime) );
	this->m_bRestoreOriginal.SetValue( i_Info.m_bRestoreOriginal );
	this->m_EaseInWeight.SetValue( i_Info.m_EaseInWeight );
	this->m_EaseOutWeight.SetValue( i_Info.m_EaseOutWeight );

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
	guiDialogTabbedMgr::BuildForm( "Driver", "Base", GetListContainer(), true );
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
	//	check if the value falls within a frame time.  If not, adjust it.
	//
	//float begin_time = tmlnTimeUtil::AdjustTimeToFrame( m_BeginTime.GetValue() );
	//if (fabsf(begin_time - m_BeginTime.GetValue()) > maConstants::c_fEpsilon)
	//{
	//	m_BeginTime.SetValue( begin_time );
	//	return;
	//}
	//maConstants::c_fRoundOff

	// If the begin time changes, move the end time also in order to
	// preserve the duration of the clip. In situations where the
	// begin and end time are being edited, the begin time call is first
	// so that we can then later set the end time explicitly.
	maTime end_time = this->m_BeginTime.GetValue() + (this->m_Duration.GetValue() - tmlnTimeLine::GetFrameIncrement());
	//TIME - old code used float comparison with epsilon, new code can do exact comparison
	//if (fabsf(end_time - this->m_EndTime.GetValue()) > maConstants::c_fEpsilon)
	if (end_time != this->m_EndTime.GetValue())
	{
		//DBG_TRACE("BEGIN_TIME_CHANGED: begin time=" << m_BeginTime.GetValue() << "  new end=" << end_time << " orig end=" << m_EndTime.GetValue());

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
	maTime duration = (this->m_EndTime.GetValue() - (this->m_BeginTime.GetValue()) + tmlnTimeLine::GetFrameIncrement());
	//TIME - old code used float comparison with epsilon, new code can do exact comparison
	//if (fabsf(duration - this->m_Duration.GetValue()) > maConstants::c_fEpsilon)
	if (duration != this->m_Duration.GetValue())
	{
		//DBG_TRACE("END_TIME_CHANGED: end time=" << m_EndTime.GetValue() << "  new duration=" << duration << " orig duration=" << m_Duration.GetValue() << " duration frames=" << (duration / tmlnTimeLine::GetFrameIncrement()));

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
	maTime end_time = this->m_BeginTime.GetValue() + (this->m_Duration.GetValue() - tmlnTimeLine::GetFrameIncrement());
	//TIME - old code used float comparison with epsilon, new code can do exact comparison
	//if (fabsf(end_time - this->m_EndTime.GetValue()) > maConstants::c_fEpsilon)
	if (end_time != this->m_EndTime.GetValue())
	{
		//DBG_TRACE("DURATION_CHANGED: duration time=" << m_Duration.GetValue() << "  new end=" << end_time << " orig end=" << m_EndTime.GetValue());

		// undo was set when Duration was changed before, now we can do
		// the end time set quietly without undo
		this->m_EndTime.SetValue(end_time);
	}

	// only need to mark dirty if end time was changed, which is already
	// handled in the EndTimeChanged callback
}

//--------------------------------------------------------------------
//	TimeFormatChanged - timeline time display format has changed
//--------------------------------------------------------------------
void tmlnDriver::TimeFormatChanged( int i_TimeFormat )
{
	maTime duration = (this->m_EndTime.GetValue() - this->m_BeginTime.GetValue()) + tmlnTimeLine::GetFrameIncrement();
	//DBG_TRACE("OLD duration = " << m_Duration.GetValue() << "  NEW duration = " << duration);

	m_Duration.SetValue(duration);
}

