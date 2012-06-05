/****************************************************************************\
**	mnmThinkMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/mnm/mnmThinkMgr.hpp"

#include "Support/mnm/mnmThinkInterest.hpp"

//	library
#include "Core/dbg/dbgAssert.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"

#include <vector>


namespace
{
	std::vector<mnmThinkInterest*>	l_ThinkInterestList;
	std::vector<mnmThinkMgr::DelayedFunction> l_DelayedFunctions;

}


//--------------------------------------------------------------------
//	Call Think() callbacks on all interests
//--------------------------------------------------------------------
void mnmThinkMgr::Think()
{
	// Make a copy so that new registered functions happen next frame
	std::vector<DelayedFunction> delayed = l_DelayedFunctions;
	l_DelayedFunctions.clear();

	// Call the delayed functions
	std::vector<DelayedFunction>::iterator func_it;
	for (func_it = delayed.begin(); func_it != delayed.end(); ++func_it)
	{
		//DelayedFunction func = (*func_it);
		//(*func)();	// this version was function pointer
		(*func_it)();			// this uses boost function object
	}

	// Notify the think interests
	std::vector<mnmThinkInterest*>::iterator int_it;
	for (int_it = l_ThinkInterestList.begin(); int_it != l_ThinkInterestList.end(); ++int_it)
	{
		(*int_it)->Think();
	}
}

//--------------------------------------------------------------------
//	Pass a function to the think manager so that it gets called
//	the next time Think() is called. This is to do updates
//	that would destroy an object that we are currently in the 
//	callback of.
//--------------------------------------------------------------------
void mnmThinkMgr::CallFunctionDelayed(const DelayedFunction& i_FuncPtr)
{
	l_DelayedFunctions.push_back(i_FuncPtr);
}

//--------------------------------------------------------------------
//	RegisterThinkInterest() - add a Think interest to the system
//--------------------------------------------------------------------
void mnmThinkMgr::RegisterThinkInterest( mnmThinkInterest* i_pInterest )
{
	DBG_ASSERT0( i_pInterest != 0, "Cannot register a NULL Think Interest" );
	l_ThinkInterestList.push_back( i_pInterest );
}

//--------------------------------------------------------------------
//	UnRegisterThinkInterest() - remove a Think interest from the system.
//
//	Note: this will NOT delete the Think interest.  It is up to the
//	registerer.
//--------------------------------------------------------------------
void mnmThinkMgr::UnRegisterThinkInterest( mnmThinkInterest* i_pInterest )
{
	envSTLHelpers::RemoveOneValue( l_ThinkInterestList, i_pInterest );
}
