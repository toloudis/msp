/*****************************************************************************
**	todDataMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "todDataMgr.hpp"

#include "dayTimeOfDay.hpp"
#include "dbgLog.hpp"
#include "envSTLHelpers.hpp"
#include "api3dScene.hpp"

#include <vector>

//
namespace todDataMgr
{
	namespace
	{
		todTimeOfDayData l_Data;
		std::vector<DataChangedCallback*> l_CallbackFuncs;

		//--------------------------------------------------------------------
		void set_tod()
		{
			dayTimeOfDay::SetEnabled( l_Data.m_bEnableAutoRotation.GetValue() );
			dayTimeOfDay::SetDayLength( (int)l_Data.m_fLengthOfDay.GetValue() );
			dayTimeOfDay::SetTime( l_Data.m_fStartTimeOfDay.GetValue() );
		}

		//--------------------------------------------------------------------
		void notify_callbacks()
		{
			static bool notifying = false;
			if (notifying) return;

			notifying = true;
			for (int i=0; i<l_CallbackFuncs.size(); i++)
				l_CallbackFuncs[i]->DataChanged();
			notifying = false;
		}

	}	// end of namespace


	//--------------------------------------------------------------------
	// Clear all items
	//--------------------------------------------------------------------
	void  Clear()
	{
		l_Data = todTimeOfDayData();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//	auto rotation or fixed time of day
	//--------------------------------------------------------------------
	void SetTimeOfDayAutoOrFixed(bool i_bAutoSet)
	{
		l_Data.m_bAutoRotation.SetValue(i_bAutoSet);
		set_tod();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//	lenghth of a day in minutes
	//--------------------------------------------------------------------
	void SetTimeOfDayLength(float i_fMinutes)
	{
		l_Data.m_fLengthOfDay.SetValue(i_fMinutes);
		set_tod();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//	starting time of day
	//--------------------------------------------------------------------
	void SetTimeOfDayStartTimeOfDay(float i_fTimeOfDayStart)
	{
		l_Data.m_fStartTimeOfDay.SetValue(i_fTimeOfDayStart);
		set_tod();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//	enable auto rotation
	//--------------------------------------------------------------------
	void SetTimeOfDayEnabled(bool i_bEnableAuto)
	{
		l_Data.m_bEnableAutoRotation.SetValue(i_bEnableAuto);
		set_tod();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	const todTimeOfDayData & GetData()
	{
		return l_Data;
	}

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	void SetData(const todTimeOfDayData &i_Data)
	{
		l_Data = i_Data;
		set_tod();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	// This callback will be called when the data changes
	//--------------------------------------------------------------------
	void AddDataChangedCallback(DataChangedCallback *i_Func)
	{
		l_CallbackFuncs.push_back(i_Func);

	}
	void RemoveDataChangedCallback(DataChangedCallback *i_Func)
	{
		envSTLHelpers::RemoveOneValue(l_CallbackFuncs, i_Func);
	}
}	// end of namespace
