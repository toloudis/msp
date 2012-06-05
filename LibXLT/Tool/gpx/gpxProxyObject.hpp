/*****************************************************************************
**	gpxProxyObject.hpp
**
**	This class is the base class for graphics proxy objects. Each proxy
**	class provides a thread-safe way to access some class in the Graphics
**	library.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_PROXYOBJECT_HPP
#error gpxProxyObject.hpp multiply included
#endif
#define GPX_PROXYOBJECT_HPP

#ifndef GPX_PROXYDEFS_HPP
#include "Tool/gpx/gpxProxyDefs.hpp"
#endif 


//============================================================================
//============================================================================
class gpxProxyObject 
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	gpxProxyObject();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxProxyObject();

	//--------------------------------------------------------------------
	// Returns true if this proxy object needs to have "Update()" called.
	//--------------------------------------------------------------------
	inline bool NeedsUpdate() const;

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. This should be called from the update/render
	//	thread (through gpxProxyMgr) when it is ready for changes.
	//	Should return true if changes were made.
	//	Derived classes should clear the "needs update" flag within
	//	this function.
	//--------------------------------------------------------------------
	virtual bool Update() = 0;

protected:
	//--------------------------------------------------------------------
	// Return access to mutex, one per proxy object.
	//--------------------------------------------------------------------
	//inline envMutex& GetMutex();

	//--------------------------------------------------------------------
	// Call this to set the flag for whether the proxy object needs
	//	to have "Update()" called. This should only be set within
	//	an existing envScopedLock on the envMutex for this class.
	//--------------------------------------------------------------------
	inline void SetNeedsUpdate(bool i_bNeedsUpdate);

private:
	//envMutex m_Mutex;
	bool m_bNeedsUpdate;

};

//--------------------------------------------------------------------
// Returns true if this proxy object needs to have "Update()" called.
//--------------------------------------------------------------------
inline bool gpxProxyObject::NeedsUpdate() const
{
	return m_bNeedsUpdate;
}

//--------------------------------------------------------------------
// Return access to mutex, one per proxy object.
//--------------------------------------------------------------------
//inline envMutex& gpxProxyObject::GetMutex()
//{
//	return m_Mutex;
//}

//--------------------------------------------------------------------
// Call this to set the flag for whether the proxy object needs
//	to have "Update()" called. This should only be set within
//	an existing envScopedLock on the envMutex for this class.
//--------------------------------------------------------------------
inline void gpxProxyObject::SetNeedsUpdate(bool i_bNeedsUpdate)
{
	m_bNeedsUpdate = i_bNeedsUpdate;
}

