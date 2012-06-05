/*****************************************************************************
**	tmlnDriver.hpp
**
**	Base class for animation drivers.  Defines begin/end frame
**	info and Operate function
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVER_HPP
#error tmlnDriver.hpp multiply included
#endif
#define TMLN_DRIVER_HPP

#ifndef TMLN_DRIVERIDMGR_HPP
#include "Support/tmln/tmlnDriverIdMgr.hpp"
#endif
#ifndef FS_RESOURCETRACKERDATA_HPP
#include "Core/fs/fsResourceTrackerData.hpp"
#endif
#ifndef ENV_AUDITOR_HPP
#include "Core/env/envAuditor.hpp"
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

#include <string>


//============================================================================
//============================================================================
class tmlnDriverInfo;


//============================================================================
//============================================================================
class tmlnDriver : public prtyObject, public envAuditable
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
	inline void SetName(const char* i_Name, prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);
	inline const std::string& GetName() const;

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
	virtual std::string GetInteractionDescription(double i_InteractStart, double i_InteractDuration);

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
	virtual void  Operate(float i_Time) = 0;

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
	//  This is called when a driver is selected. It lets the driver
	//		select one of its icons or whatever else it wants to do.
	//--------------------------------------------------------------------
	virtual void  DoSelect();

	//--------------------------------------------------------------------
	//  Driver should select its 3D icon
	//--------------------------------------------------------------------
	virtual void  DoSelectIcon();

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
	bool IsBefore(const float i_Time);
	bool IsWithin(const float i_Time);
	bool IsAfter(const float i_Time);

	//--------------------------------------------------------------------
	//  get a list of resources.  the resources will be appended to the
	//	passed in list.
	//--------------------------------------------------------------------
	virtual void GetResourceList( fsResourceTrackerData& io_List );

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
	float  GetBeginTime() const;
	virtual void  SetBeginTime(float i_BeginTime, prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	// EndTime
	//--------------------------------------------------------------------
	float  GetEndTime() const;
	virtual void  SetEndTime(float i_EndTime, prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

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
	inline float  GetDuration() const;

	//--------------------------------------------------------------------
	// BlendType - type of blending from old value to this driver
	//--------------------------------------------------------------------
	BlendType  GetBlendType() const;
	void  SetBlendType(BlendType i_BlendType, prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	// BlendTime - time to blend from last value to this driver.
	//	This is only relevant for BlendType::e_BlendTime
	//--------------------------------------------------------------------
	float  GetBlendTime() const;
	void  SetBlendTime(float i_BlendTime, prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	// EaseInWeight - modifier to tangent into this driver.
	//	This is only relevant for BlendType::e_SmoothBlend
	//--------------------------------------------------------------------
	float  GetEaseInWeight() const;
	void  SetEaseInWeight(float i_Weight, prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	// EaseOutWeight - modifier to tangent out of this driver.
	//	This is only relevant for BlendType::e_SmoothBlend
	//--------------------------------------------------------------------
	float  GetEaseOutWeight() const;
	void  SetEaseOutWeight(float i_Weight, prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//	RestoreOriginalValue - when driver is finished, should the
	//	 value of the channels be restored to their "original" value,
	//	 or should the final state of the driver remain?
	//--------------------------------------------------------------------
	bool IsRestoreOriginalValue() const;
	void SetRestoreOriginalValue(bool i_bRestore, prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	// GetBlendAlpha - get percentage from 0-1 for blending from
	//	previous time to the given time, using this driver's values
	//	for BeginTime, BlendType and BlendTime
	//--------------------------------------------------------------------
	float  GetBlendAlpha(float i_PrevTime, float i_Time) const;

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
	//	Split - split this driver at time i_fTime.
	//	This function will clone the driver, change the appropriate
	//	values in each driver instance and return the new driver that is 
	//	the other half of the split.
	//--------------------------------------------------------------------
	void Split(tmlnDriver* io_pDriverAtEnd, float i_fTime);

	//--------------------------------------------------------------------
	// Property Access
	//--------------------------------------------------------------------
	prtyFloat&		PropertyBeginTime() { return m_BeginTime; }
	prtyFloat&		PropertyEndTime() { return m_EndTime; }
	prtyEnum&		PropertyBlendType() { return m_Blend; }
	prtyFloat&		PropertyBlendTime() { return m_BlendTime; }
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
	virtual void PerformSplit( tmlnDriver* io_pDriverAtEnd, float i_fTime );

	//--------------------------------------------------------------------
	//	calculate the percent this time is into the driver.
	//--------------------------------------------------------------------
	float CalculatePercent( float i_fTime );

	//--------------------------------------------------------------------
	// Get/Set base driver information for derived classes Get/Set
	//	from tmlnDriverInfo.
	//
	// Note: SetBaseDriverInfo calls MarkDirty(), so you don't
	//	have to call it in the derived class's SetDriverInfo() function
	//--------------------------------------------------------------------
	void GetBaseDriverInfo(tmlnDriverInfo& o_Info) const;
	void SetBaseDriverInfo(	const tmlnDriverInfo& i_Info, 
							prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);


private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void BeginTimeChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void EndTimeChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void DurationChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	prtyText	m_Name;
	prtyFloat	m_BeginTime;
	prtyFloat	m_EndTime;
	prtyEnum	m_Blend;
	prtyFloat	m_BlendTime;
	prtyFloat	m_EaseInWeight;
	prtyFloat	m_EaseOutWeight;
	prtyBoolean	m_bRestoreOriginal;

	//	defaults
	static prtyEnum	m_BlendDefault;

	// Duration exists only to provide an alternative UI for
	// altering end time (only end time is written to a file).
	prtyFloat	m_Duration;

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
// Duration is end time - begin time
//--------------------------------------------------------------------
inline float  tmlnDriver::GetDuration() const
{
	return m_EndTime.GetValue() - m_BeginTime.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline void tmlnDriver::SetName(const char* i_Name, prtyProperty::UndoFlags i_Undoable)
{
	if (_stricmp(i_Name, m_Name.GetValue().c_str()) != 0)
		m_Name.SetValue(i_Name, i_Undoable);
}
inline const std::string& tmlnDriver::GetName() const
{
	return m_Name.GetValue();
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

