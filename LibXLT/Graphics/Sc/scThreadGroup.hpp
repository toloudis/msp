/*****************************************************************************
**	scThreadGroup.hpp
**
**		scThreadGroup provides an API for doing animation in parallel
**	on separate threads and then waiting for them to finish before 
**	continuing with rendering.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef SC_THREADGROUP_HPP
#error scThreadGroup.hpp multiply included
#endif
#define SC_THREADGROUP_HPP

#ifndef ENV_THREADGROUP_HPP
#include "Core/env/envThreadGroup.hpp"
#endif


//============================================================================
//============================================================================
namespace scThreadGroup
{
	//--------------------------------------------------------------------
	// Add a task to the group to be managed. The argument could be
	//	a function pointer or a function object.
	//--------------------------------------------------------------------
	void AddThread(const envThreadGroup::ThreadFunc& i_ThreadFunc);

	//-------------------------------------------------------------------- 
	// Wait for all tasks to finish.
	//--------------------------------------------------------------------
	void WaitForAll();
}

