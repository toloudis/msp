/*****************************************************************************
**	todOperations.hpp
**
**	Utility for operations that are undoable in tod system
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef TOD_OPERATIONS_HPP
#error todOperations.hpp multiply included
#endif
#define TOD_OPERATIONS_HPP


#ifndef TOD_TIMEOFDAYDATA_HPP
#include "todTimeOfDayData.hpp"
#endif


namespace todOperations
{
	//--------------------------------------------------------------------
	//	TimeOfDay operations are joined together if the same type of operation.
	//	Call this to force a new operation
	//--------------------------------------------------------------------
	void StartNewOp();

	//--------------------------------------------------------------------
	//  Change auto or fixed
	//--------------------------------------------------------------------
	void  ChangeTimeOfDayAutoOrFixed( bool i_bAutoSet );

	//--------------------------------------------------------------------
	//  Change length of the day
	//--------------------------------------------------------------------
	void  ChangeTimeOfDayLength(float i_fMinutes);

	//--------------------------------------------------------------------
	//  Change the actual start time of day
	//--------------------------------------------------------------------
	void  ChangeTimeOfDayStartTimeOfDay(float i_fTimeOfDayStart);

	//--------------------------------------------------------------------
	//  Change enabled auto rotation
	//--------------------------------------------------------------------
	void  ChangeTimeOfDayEnabled(bool i_bEnableAuto);
}
