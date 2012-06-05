/*****************************************************************************
**	todDataMgr.hpp
**
**	Keeps track of the current displayed TOD data.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef TOD_DATAMGR_HPP
#error todDataMgr.hpp multiply included
#endif
#define TOD_DATAMGR_HPP

#ifndef TOD_TIMEOFDAYDATA_HPP
#include "todTimeOfDayData.hpp"
#endif

class fsLocator;

namespace todDataMgr
{
	// EventCallback when any data in this manager changes
	class DataChangedCallback
	{
	public:
		virtual void DataChanged() = 0;
	};

	//--------------------------------------------------------------------
	// Clear all items
	//--------------------------------------------------------------------
	void  Clear();

	//--------------------------------------------------------------------
	//	auto rotation or fixed time of day
	//--------------------------------------------------------------------
	void SetTimeOfDayAutoOrFixed(bool i_bAutoSet);

	//--------------------------------------------------------------------
	//	lenghth of a day in minutes
	//--------------------------------------------------------------------
	void SetTimeOfDayLength(float i_fMinutes);

	//--------------------------------------------------------------------
	//	starting time of day
	//--------------------------------------------------------------------
	void SetTimeOfDayStartTimeOfDay(float i_fTimeOfDayStart);

	//--------------------------------------------------------------------
	//	enable auto rotation
	//--------------------------------------------------------------------
	void SetTimeOfDayEnabled(bool i_bEnableAuto);

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	const todTimeOfDayData & GetData();
	void SetData(const todTimeOfDayData &i_Data);

	//--------------------------------------------------------------------
	// This callback will be called when the data changes
	//--------------------------------------------------------------------
	void AddDataChangedCallback(DataChangedCallback *i_Func);
	void RemoveDataChangedCallback(DataChangedCallback *i_Func);

}	// end of namespace
