/****************************************************************************\
**  appTimePACPS2.cpp
**
**      appTimePACPS2.cpp defines the appTime component
**	PAC for the PS2.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "appTimePACPS2.hpp"

#include "dbgAssert.hpp"
#include "dbgLog.hpp"
#include "envInitX.hpp"

#include <eekernel.h>
#include <eeregs.h>

namespace appTimePAC
{

namespace
{

	volatile int l_TimerCounter = 0;
	const float l_TimerFreq = 576000.0f; // 147456000 / 256; 
	
	// ==================================================================

	int timer_interrupt_handler(int i_Cause) 
	{
	    // Cause will be INTC_TIM0 in this case. It is passed in because the same
	    // handler may be used for different interrupt causes, so it may need to know
	    // the source of the interrupt. In our case, we know it will be INTC_TIM0
	    // so we don't do any checks.

	    envType::UInt32 mode = DGET_T0_MODE();    // See what caused this interrupt

	    DPUT_T0_MODE(mode);			// This will clear the EQUF and/or OVFE flags

		if( mode & T_MODE_OVFF_M )	// If overflow caused interrupt
	        l_TimerCounter++;

	    ExitHandler();  // Must call this just before returning from handler
	    return 0;
	}
}

//====================================================================
//	GetTime returns a floating point number representing the time
//	in seconds.  The 0 point for this time can be anywhere; the appTime
//	component corrects for different time conventions.
//====================================================================
float GetTime()
{
//	DBG_LOG1("l_TimerCounter: %d", l_TimerCounter);
	envType::Int64 rollovers_check = l_TimerCounter;	
	envType::Int64 timer_val = DGET_T0_COUNT();
	envType::Int64 rollovers = l_TimerCounter;
	
	if( rollovers != rollovers_check )	//	timer interrupt was called in between statements
	{
		//	it should be OK to just set timer_val to zero,
		//	as it must be very near to zero since it just rolled over
		timer_val = 0;
	}
	
	envType::Int64 total_timer = rollovers << 16 | timer_val;
	return float(total_timer) / l_TimerFreq;
}


//========================================================================
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//========================================================================
void Init()
{
	//	we'll use timer zero for out main game timer.
	//	Since it's only 16-bit, we set up an interrupt to count overflows
	//	This way we can effectively make a 48-bit (or however many we wanted)
	//	counter

	// Install handler
	int ret_val = AddIntcHandler(INTC_TIM0, timer_interrupt_handler, 0);
	if( ret_val == -1 )
		throw envInitX("app");

	DPUT_T0_MODE((1 << 10) | (1 << 11));    // Halt the timer and clear any pending interrupts
	DPUT_T0_COUNT(0);           // Reset T0_COUNT to 0

	EnableIntc(INTC_TIM0);      // Enable Timer0 interrupts

	DPUT_T0_MODE(	(1 << 1) |    // CLKS = 2, Bus clock/256
					(1 << 7) |  // CUE = 1, start counting
	                T_MODE_OVFE_M   // OVFE = 1, Overflow interrupts enabled
	            );
	
}

void CleanUp()
{
}


}

