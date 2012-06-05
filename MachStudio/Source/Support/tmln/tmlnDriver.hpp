/*****************************************************************************
**	tmlnDriver.hpp
**
**	Base class for animation drivers.  Defines begin/end frame
**	info and Operate function
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVER_HPP
#error tmlnDriver.hpp multiply included
#endif
#define TMLN_DRIVER_HPP

#ifndef TMLN_DRIVERIDMGR_HPP
#include "Support/tmln/tmlnDriverIdMgr.hpp"
#endif
#ifndef TMLN_TIMEINTEREST_HPP
#include "Support/tmln/tmlnTimeInterest.hpp"
#endif 

#ifndef ENV_AUDITOR_HPP
#include "Core/env/envAuditor.hpp"
#endif
#ifndef FS_RESOURCETRACKERDATA_HPP
#include "Core/fs/fsResourceTrackerData.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PRTY_PROPERTY_HPP
#include "Core/prty/prtyProperty.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif
#ifndef PRTY_TIME_HPP
#include "Core/prty/prtyTime.hpp"
#endif 
#ifndef REL_OBJECT_HPP
#include "Core/rel/relObject.hpp"
#endif 

#include <string>
#include <map>


//============================================================================
//============================================================================
class tmlnDriverInfo;


//============================================================================
//============================================================================
class tmlnDriver : public prtyObject, public relObject, public envAuditable, public tmlnTimeInterest
{
public:
	enum BlendType
	{
		e_NoBlending = 0,	// driver starts at its initial value at start time
		e_Previous,			// driver starts blending when previous driver ends
		e_BlendTime,		// driver blends a fixed amount of time ahead of it start
		e_Overwrite,		// driver holds at its start value if no other driver is active
		e_SmoothBlend		// driver uses spline tangents to ease in
	};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriver();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriver();

	//--------------------------------------------------------------------
	// Name of the driver, displayed in the trax editor
	//--------------------------------------------------------------------
	void SetName(const char* i_Name);
	inline const std::string& GetName() const;

	//--------------------------------------------------------------------
	// ChannelName of the driver, used to separate drivers for
	// materials and custom properties where the chunk name does not
	// signify the channel.
	//--------------------------------------------------------------------
	inline void SetChannelName(const char* i_ChannelName);
	inline const std::string& GetChannelName() const;

	//--------------------------------------------------------------------
	// Category of the driver, used to separate drivers in multichannel
	//	channels
	//--------------------------------------------------------------------
	inline void SetCategory(const char* i_Category);
	inline const std::string& GetCategory() const;

	//--------------------------------------------------------------------
	// Return unique id for this driver.
	//--------------------------------------------------------------------
	tmlnDriverId GetDriverId() const;

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	virtual std::string GetHoverDescription();

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse interacts with the clip.
	//--------------------------------------------------------------------
	virtual std::string GetInteractionDescription(const maTime& i_InteractStart, const maTime& i_InteractDuration);

	//--------------------------------------------------------------------
	//	ShowIcons - show or hide icons that are not part of real scene.
	//--------------------------------------------------------------------
	virtual void ShowIcons( bool i_bVisible );

	//--------------------------------------------------------------------
	//  GetDriverInfo - return data structure representing state of
	//		this driver suitable for writing to a file.
	//	The returned value should be created with "new" and will
	//		be deleted by the caller.
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo*  GetDriverInfo() const = 0;

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(const maTime& i_Time) = 0;

	//--------------------------------------------------------------------
	//	AlterKey - look at the values in the channels to which this 
	//	driver is connected and alter the driver in order to match 
	//	these values.
	//	Returns true if this driver was able to alter its value.
	//	Default behavior is to return false.
	//--------------------------------------------------------------------
	virtual bool AlterKey();

	//--------------------------------------------------------------------
	//	Returns true if dirty bit has been set and Operate is needed.
	//--------------------------------------------------------------------
	inline bool IsDirty() const;

	//--------------------------------------------------------------------
	// MarkDirty - set dirty bit, meaning an new Operate call is needed.
	//--------------------------------------------------------------------
	inline void MarkDirty();

	//--------------------------------------------------------------------
	// ClearDirty - clear dirty bit, called by ChannelSet when Operate
	//	is called.
	//--------------------------------------------------------------------
	inline void ClearDirty();

	//--------------------------------------------------------------------
	//  Show dialog that allows user to edit this driver's properties
	//--------------------------------------------------------------------
	virtual void  DoEditProperties();

	//--------------------------------------------------------------------
	//	CreateReferenceForProperty - given a property, create a
	//	shared_ptr to a prtyPropertyReference to this property.
	//--------------------------------------------------------------------
	virtual shared_ptr<prtyPropertyReference> CreateReferenceForProperty(prtyProperty& i_Property);

	//--------------------------------------------------------------------
	//  This is called when a driver is selected. It lets the driver
	//		select one of its icons or whatever else it wants to do.
	//--------------------------------------------------------------------
	virtual void DoSelect();

	//--------------------------------------------------------------------
	//  Driver should select its 3D icon
	//--------------------------------------------------------------------
	virtual void DoSelectIcon();

	//--------------------------------------------------------------------
	//	NotActive() - called ONCE when a driver goes from active to not
	//	active.  This function needs to reset m_bNotifyNotActive so it
	//	doesn't get anymore calls.
	//--------------------------------------------------------------------
	virtual void NotActive();

	//--------------------------------------------------------------------
	//	Active() - called ONCE when a driver first goes active.  This
	//	function needs to reset m_bNotifyNotActive so it doesn't get
	//	anymore calls.
	//--------------------------------------------------------------------
	virtual void Active();

	//--------------------------------------------------------------------
	//	Check if the time is before, within, or after the driver's time
	//	including a small epsilon to handle round-off errors.
	//--------------------------------------------------------------------
	bool IsBefore(const maTime& i_Time) const;
	bool IsWithin(const maTime& i_Time) const;
	bool IsAfter(const maTime& i_Time) const;

	//--------------------------------------------------------------------
	//  get a list of resources.  the resources will be appended to the
	//	passed in list.
	//--------------------------------------------------------------------
	virtual void GetResourceList( fsResourceTrackerData& io_List );

	//--------------------------------------------------------------------
	//	Remap internal name attachments using the given map.
	//  This is part of the duplication process and makes sures 
	//	internal attachments are passed onto the duplicated objects.
	//--------------------------------------------------------------------
	virtual void RemapNames(const std::map<nameString, nameString> &i_DuplicateNameMap);

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

	//
	// gets/sets
	//

	//--------------------------------------------------------------------
	// BeginTime
	//--------------------------------------------------------------------
	const maTime&  GetBeginTime() const;
	virtual void  SetBeginTime(const maTime& i_BeginTime);

	//--------------------------------------------------------------------
	// EndTime
	//--------------------------------------------------------------------
	const maTime&  GetEndTime() const;
	virtual void  SetEndTime(const maTime& i_EndTime);

	//--------------------------------------------------------------------
	// A convenience function for setting the begin/end times of a newly
	//	created driver.  It will create the driver so that it begins at
	//	the current timeline time and lasts for the given duration.
	//	If the duration is negative, then the duration will be set so
	//	that the end time of driver equals the end time in the timeline.
	//--------------------------------------------------------------------
	void SetInitialTime(float i_Duration = -1.0f);

	//--------------------------------------------------------------------
	// Duration is end time - begin time
	//--------------------------------------------------------------------
	maTime GetDuration() const;

	//--------------------------------------------------------------------
	// BlendType - type of blending from old value to this driver
	//--------------------------------------------------------------------
	BlendType  GetBlendType() const;
	void  SetBlendType(BlendType i_BlendType);

	//--------------------------------------------------------------------
	// BlendTime - time to blend from last value to this driver.
	//	This is only relevant for BlendType::e_BlendTime
	//--------------------------------------------------------------------
	const maTime&  GetBlendTime() const;
	void  SetBlendTime(const maTime& i_BlendTime);

	//--------------------------------------------------------------------
	// EaseInWeight - modifier to tangent into this driver.
	//	This is only relevant for BlendType::e_SmoothBlend
	//--------------------------------------------------------------------
	float  GetEaseInWeight() const;
	void  SetEaseInWeight(float i_Weight);

	//--------------------------------------------------------------------
	// EaseOutWeight - modifier to tangent out of this driver.
	//	This is only relevant for BlendType::e_SmoothBlend
	//--------------------------------------------------------------------
	float  GetEaseOutWeight() const;
	void  SetEaseOutWeight(float i_Weight);

	//--------------------------------------------------------------------
	//	RestoreOriginalValue - when driver is finished, should the
	//	 value of the channels be restored to their "original" value,
	//	 or should the final state of the driver remain?
	//--------------------------------------------------------------------
	bool IsRestoreOriginalValue() const;
	void SetRestoreOriginalValue(bool i_bRestore);

	//--------------------------------------------------------------------
	// GetBlendAlpha - get percentage from 0-1 for blending from
	//	previous time to the given time, using this driver's values
	//	for BeginTime, BlendType and BlendTime
	//--------------------------------------------------------------------
	float  GetBlendAlpha(const maTime& i_PrevTime, const maTime& i_Time) const;

	//--------------------------------------------------------------------
	//	GetActive/NotActive
	//--------------------------------------------------------------------
	bool GetNotifyActive() const;
	bool GetNotifyNotActive() const;

	//--------------------------------------------------------------------
	//	Some drivers, like attachment need to be delayed to make sure
	//	that other drivers such as position and animation are updated
	//	first. The default value of this function is false, override
	//	this function in the derived class if you need to be delayed.
	//--------------------------------------------------------------------
	virtual bool NeedsDelayedOperate();

	//--------------------------------------------------------------------
	//	AutoPopUpEditProperties - if true, the edit properties dialog
	//	will be forced open on a new driver.  If false, the user will
	//	need to open it (unless it is already open).
	//--------------------------------------------------------------------
	inline bool IsAutoPopUpEditProperties() const;
	inline void SetAutoPopUpEditProperties(bool i_bAutoPopUp);

	//--------------------------------------------------------------------
	//	Split - split this driver at time i_Time.
	//	This function will clone the driver, change the appropriate
	//	values in each driver instance and return the new driver that is 
	//	the other half of the split.
	//--------------------------------------------------------------------
	void Split(tmlnDriver* io_pDriverAtEnd, const maTime& i_fTime);

	//--------------------------------------------------------------------
	// Property Access
	//--------------------------------------------------------------------
	prtyTime&		PropertyBeginTime() { return m_BeginTime; }
	prtyTime&		PropertyEndTime() { return m_EndTime; }
	prtyEnum&		PropertyBlendType() { return m_Blend; }
	prtyTime&		PropertyBlendTime() { return m_BlendTime; }
	prtyFloat&		PropertyEaseInWeight() { return m_EaseInWeight; }
	prtyFloat&		PropertyEaseOutWeight() { return m_EaseOutWeight; }
	prtyBoolean&	PropertyRestoreOriginal() { return m_bRestoreOriginal; }
	
	//--------------------------------------------------------------------
	//	Add the base driver properties as a seperate tab to the driver
	//	dialog.  This is only to be used for drivers that have a custom
	//	dialog.
	//--------------------------------------------------------------------
	void AddBasePropertiesTab();

	//--------------------------------------------------------------------
	//	Value equal to the default type of blend for any driver
	//--------------------------------------------------------------------
	static void SetDefaultBlendType(const tmlnDriver::BlendType i_BlendType);
	static const tmlnDriver::BlendType GetDefaultBlendType();

protected:
	//--------------------------------------------------------------------
	//	PerformSplit - this is the heart of the split.  It is called by
	//	Split() and it make the decisions on how to split up the
	//	drivers.  It then sets the appropriate values for each driver.
	//
	//	Note: Special drivers will implement a version of this.
	//--------------------------------------------------------------------
	virtual void PerformSplit( tmlnDriver* io_pDriverAtEnd, const maTime& i_Time );

	//--------------------------------------------------------------------
	//	calculate the percent this time is into the driver.
	//--------------------------------------------------------------------
	float CalculatePercent( const maTime& i_fTime );

	//--------------------------------------------------------------------
	// Get/Set base driver information for derived classes Get/Set
	//	from tmlnDriverInfo.
	//
	// Note: SetBaseDriverInfo calls MarkDirty(), so you don't
	//	have to call it in the derived class's SetDriverInfo() function
	//--------------------------------------------------------------------
	void GetBaseDriverInfo(tmlnDriverInfo& o_Info) const;
	void SetBaseDriverInfo(	const tmlnDriverInfo& i_Info);

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void BeginTimeChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void EndTimeChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void DurationChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	//
	//	tmlnTimeInterest interface
	//

	//--------------------------------------------------------------------
	//	TimeChanged - timeline current time has changed
	//--------------------------------------------------------------------
	void TimeChanged( const maTime& i_Time ) {};

	//--------------------------------------------------------------------
	//	TimeRangeChanged - timeline minimum and maximum time has changed
	//--------------------------------------------------------------------
	void TimeRangeChanged( const maTime& i_MinTime, const maTime& i_MaxTime ) {};

	//--------------------------------------------------------------------
	//	TimeFormatChanged - timeline time display format has changed
	//--------------------------------------------------------------------
	void TimeFormatChanged( int i_TimeFormat );

	//--------------------------------------------------------------------
	//	FrameRateChanged - timeline frame rate (frames per second) changed
	//--------------------------------------------------------------------
	void FrameRateChanged( float i_FrameRate ) {};

private:
	prtyText	m_Name;
	prtyTime	m_BeginTime;
	prtyTime	m_EndTime;
	prtyEnum	m_Blend;
	prtyTime	m_BlendTime;
	prtyFloat	m_EaseInWeight;
	prtyFloat	m_EaseOutWeight;
	prtyBoolean	m_bRestoreOriginal;

	//	defaults
	static prtyEnum	m_BlendDefault;

	// Duration exists only to provide an alternative UI for
	// altering end time (only end time is written to a file).
	prtyTime	m_Duration;

	std::string	m_ChannelName;
	std::string	m_Category;
	bool		m_bNotifyNotActive;
	bool		m_bNotifyActive;
	bool		m_bNeedsOperate;
	bool		m_bAutoPopUpEditProperties;

	// Driver id for keeping track of a driver uniquely
	tmlnDriverId m_DriverId;
};


//--------------------------------------------------------------------
//	Returns true if dirty bit has been set and Operate is needed.
//--------------------------------------------------------------------
inline bool tmlnDriver::IsDirty() const
{
	return m_bNeedsOperate;
}

//--------------------------------------------------------------------
// MarkDirty - set dirty bit, meaning an new Operate call is needed.
//--------------------------------------------------------------------
inline void tmlnDriver::MarkDirty()
{
	m_bNeedsOperate = true;
}

//--------------------------------------------------------------------
// ClearDirty - clear dirty bit, called by ChannelSet when Operate
//	is called.
//--------------------------------------------------------------------
inline void tmlnDriver::ClearDirty()
{
	m_bNeedsOperate = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const std::string& tmlnDriver::GetName() const
{
	return m_Name.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline void tmlnDriver::SetChannelName(const char* i_ChannelName)
{
	m_ChannelName = i_ChannelName;
}
inline const std::string& tmlnDriver::GetChannelName() const
{
	return m_ChannelName;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline void tmlnDriver::SetCategory(const char* i_Category)
{
	m_Category = i_Category;
}
inline const std::string& tmlnDriver::GetCategory() const
{
	return m_Category;
}

//--------------------------------------------------------------------
//	GetActive/NotActive
//--------------------------------------------------------------------
inline bool tmlnDriver::GetNotifyActive() const
{
	return m_bNotifyActive;
}
inline bool tmlnDriver::GetNotifyNotActive() const
{
	return m_bNotifyNotActive;
}

//--------------------------------------------------------------------
//	AutoPopUpEditProperties - if true, the edit properties dialog
//	will be forced open on a new driver.  If false, the user will
//	need to open it (unless it is already open).
//--------------------------------------------------------------------
inline bool tmlnDriver::IsAutoPopUpEditProperties() const
{
	return m_bAutoPopUpEditProperties;
}
inline void tmlnDriver::SetAutoPopUpEditProperties(bool i_bAutoPopUp)
{
	m_bAutoPopUpEditProperties = i_bAutoPopUp;
}

