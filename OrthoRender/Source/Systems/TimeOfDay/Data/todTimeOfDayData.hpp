/********************************************************************************************\
**  todTimeOfDayData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef TOD_TIMEOFDAYDATA_HPP
#error todTimeOfDayData.hpp multiply included
#endif
#define TOD_TIMEOFDAYDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "prtyBoolean.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "prtyFloat.hpp"
#endif


class todTimeOfDayData
{
public:

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	todTimeOfDayData()
	:	m_bAutoRotation("Auto Rotation", false),
		m_fLengthOfDay("Length of Day", 15.0f ),
		m_fStartTimeOfDay("Start Time of Day", 14.0f ),
		m_bEnableAutoRotation("Enable Auto Rotation", false )
	{
	}

	prtyFloat	m_bAutoRotation;	// why a float? [rjk]
	prtyFloat	m_fLengthOfDay;
	prtyFloat	m_fStartTimeOfDay;
	prtyBoolean	m_bEnableAutoRotation;
};

