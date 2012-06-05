/*****************************************************************************\
**	mnmThinkMgr.hpp
**
**		Manages the requested Think interests
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef MNM_THINKMGR_HPP
#error mnmThinkMgr.hpp multiply included
#endif
#define MNM_THINKMGR_HPP

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include <boost/function.hpp>

//============================================================================
//	Forward References
//============================================================================
class mnmThinkInterest;


//============================================================================
//============================================================================
namespace mnmThinkMgr
{
	//--------------------------------------------------------------------
	//	Call Think() callbacks on all interests
	//--------------------------------------------------------------------
	void Think();

	//--------------------------------------------------------------------
	//	Pass a function to the think manager so that it gets called
	//	the next time Think() is called. This is to do updates
	//	that would destroy an object that we are currently in the 
	//	callback of.
	//--------------------------------------------------------------------
	//typedef void (*DelayedFunction)();
	//void CallFunctionDelayed(DelayedFunction i_FuncPtr);
	typedef boost::function0<void> DelayedFunction;
	void CallFunctionDelayed(const DelayedFunction& i_FuncPtr);

	//--------------------------------------------------------------------
	//	RegisterThinkInterest() - add a Think interest to the system
	//--------------------------------------------------------------------
	void RegisterThinkInterest( mnmThinkInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterThinkInterest() - remove a Think interest from the system.
	//
	//	Note: this will NOT delete the Think interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterThinkInterest( mnmThinkInterest* i_pInterest );

};
