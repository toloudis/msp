/*****************************************************************************
**	gpxReference.hpp
**
**		This class is a thread-safe proxy for a api3dReference.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_REFERENCE_HPP
#error gpxReference.hpp multiply included
#endif
#define GPX_REFERENCE_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 
#ifndef API3D_REFERENCE_HPP
#include "Tool/api3d/api3dReference.hpp"
#endif 


//============================================================================
//============================================================================
class gpxReference : public gpxProxyObject, public api3dReference
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to instance it will control
	//--------------------------------------------------------------------
	explicit gpxReference(api3dReference &i_Reference);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxReference();

	//--------------------------------------------------------------------
	// GetMatrix for this named refence
	//--------------------------------------------------------------------
	virtual maMatrix4x4 GetMatrix() const;

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	api3dReference &m_Reference;

#if USE_PROXIES
	// matrix from the last update
	mutable maMatrix4x4 m_Matrix;
#endif
};
