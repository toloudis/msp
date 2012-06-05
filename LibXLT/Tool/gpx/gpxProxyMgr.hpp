/*****************************************************************************
**	gpxProxyMgr.hpp
**
**	This manager maintains a list of proxy objects such that their 
**	changes can be pushed into the graphics objects when the update
**	thread is ready for the changes.
**
**	By tracking if any proxy objects have made changes, this manager
**	can also decide if a new render is needed or not.
**
**	A future implementation may want to have the proxy manager as a class
**	such that each scene has its own proxy manager.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_PROXYMGR_HPP
#error gpxProxyMgr.hpp multiply included
#endif
#define GPX_PROXYMGR_HPP


//============================================================================
//	forward references
//============================================================================
class gpxProxyObject;


//============================================================================
//============================================================================
namespace gpxProxyMgr 
{
	//--------------------------------------------------------------------
	// AddProxyObject to management
	//--------------------------------------------------------------------
	void AddProxy(gpxProxyObject *i_pProxy);

	//--------------------------------------------------------------------
	//	RemoveProxyObject from management
	//--------------------------------------------------------------------
	void RemoveProxy(gpxProxyObject *i_pProxy);

	//--------------------------------------------------------------------
	// Call Update on all proxy objects that have requested it.
	//	Returns true if any proxy object changed any graphical objects
	//	(which means a new render is needed).
	//--------------------------------------------------------------------
	bool Update();

}
