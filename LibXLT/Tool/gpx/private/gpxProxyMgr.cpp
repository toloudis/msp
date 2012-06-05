/*****************************************************************************
**	gpxProxyMgr.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxProxyMgr.hpp"

#include "Core/Env/envSTLHelpers.hpp"
#include "Core/Env/envThread.hpp"
#include "Tool/gpx/gpxProxyObject.hpp"

#include <list>


//============================================================================
//============================================================================
namespace gpxProxyMgr
{
	namespace
	{
		std::list<gpxProxyObject*> l_Proxies;
		envMutex l_Mutex;

	}	// end of namespace

	//--------------------------------------------------------------------
	// AddProxyObject to management
	//--------------------------------------------------------------------
	void gpxProxyMgr::AddProxy(gpxProxyObject *i_pProxy)
	{
		envScopedLock proxy_mgr_lock(l_Mutex);

		l_Proxies.push_back( i_pProxy );
	}

	//--------------------------------------------------------------------
	//	RemoveProxyObject from management
	//--------------------------------------------------------------------
	void gpxProxyMgr::RemoveProxy(gpxProxyObject *i_pProxy)
	{
		envScopedLock proxy_mgr_lock(l_Mutex);

		envSTLHelpers::RemoveOneValue( l_Proxies, i_pProxy );
	}

	//--------------------------------------------------------------------
	// Call Update on all proxy objects that have requested it.
	//	Returns true if any proxy object changed any graphical objects
	//	(which means a new render is needed).
	//--------------------------------------------------------------------
	bool gpxProxyMgr::Update()
	{
#if USE_PROXIES
		envScopedLock proxy_mgr_lock(l_Mutex);

		bool bChanged = false;
		std::list<gpxProxyObject*>::iterator it;
		for (it = l_Proxies.begin(); it != l_Proxies.end(); ++it)
		{
			if ((*it)->NeedsUpdate())
			{
				bChanged |= (*it)->Update();
			}
		}
		return bChanged;
#else
		// When changes aren´t proxied, can´t tell if a new render is needed or not.
		// So we have to return true here, as if we changed something.
		return true;
#endif
	}

}
