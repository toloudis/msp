/*****************************************************************************
**	gpxProxyUtil.hpp
**
**	gpxProxyUtil contains utilities and macros for patterns in 
**	proxy object code. 
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_PROXYUTIL_HPP
#error gpxProxyUtil.hpp multiply included
#endif
#define GPX_PROXYUTIL_HPP

#ifndef GPX_PROXYDEFS_HPP
#include "Tool/gpx/gpxProxyDefs.hpp"
#endif 
#ifndef GPX_PROXYMGR_HPP
#include "Tool/gpx/gpxProxyMgr.hpp"
#endif 


//============================================================================
//============================================================================
#if USE_PROXIES // Using proxies for multithreading

	#define PROXY_ADD()			gpxProxyMgr::AddProxy(this);
	#define PROXY_REMOVE()		gpxProxyMgr::RemoveProxy(this);

	// Store value into member variable for later update,
	//	and mark the proxy object as dirty
	#define PROXY_SET_OR_STORE(Object, Function, Member, Value) Member = Value; this->SetNeedsUpdate(true)

	// Set value variation into public variable
	#define PROXY_SET_VARIABLE(Variable, Member, Value) Member = Value; this->SetNeedsUpdate(true)

	// Return buffered value
	#define PROXY_GET(Object, Function, Member) Member

#else // Not using proxies, set data directly

	#define PROXY_ADD()			
	#define PROXY_REMOVE()		

	// Set value directly into object immediately
	#define PROXY_SET_OR_STORE(Object, Function, Member, Value) Object.Function(Value)

	// Set value variation into public variable
	#define PROXY_SET_VARIABLE(Variable, Member, Value) Variable = Value

	// Get value from object immediately
	#define PROXY_GET(Object, Function, Member) Object.Function()


#endif // USE_PROXIES
