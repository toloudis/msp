/*****************************************************************************
**	scThreadGroup.cpp
**
**		scThreadGroup provides an API for doing animation in parallel
**	on separate threads and then waiting for them to finish before 
**	continuing with rendering.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/sc/scThreadGroup.hpp"


//============================================================================
//============================================================================
namespace scThreadGroup
{
	namespace
	{
		envThreadGroup* l_pThreadGroup = NULL;
		envMutex l_ThreadGroupMutex;
	}	// end of namespace


	//--------------------------------------------------------------------
	// Add a task to the group to be managed. The argument could be
	//	a function pointer or a function object.
	//--------------------------------------------------------------------
	void AddThread(const envThreadGroup::ThreadFunc& i_ThreadFunc)
	{	
		envScopedLock group_lock(l_ThreadGroupMutex);

		if (!l_pThreadGroup)
		{
			l_pThreadGroup = new envThreadGroup();
		}
		l_pThreadGroup->AddThread(i_ThreadFunc);
	}

	//-------------------------------------------------------------------- 
	// Wait for all tasks to finish.
	//--------------------------------------------------------------------
	void WaitForAll()
	{
		envScopedLock group_lock(l_ThreadGroupMutex);

		if (l_pThreadGroup)
		{
			l_pThreadGroup->WaitForAll();
			delete l_pThreadGroup;
			l_pThreadGroup = NULL;
		}
	}
}	// end of namespace
