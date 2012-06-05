/*****************************************************************************
**	gpxReference.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxReference.hpp"

#include "Graphics/G3d/g3dThreadControl.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxReference::gpxReference(api3dReference &i_Object)
:	m_Reference(i_Object)
{
#if USE_PROXIES
	m_Matrix = m_Reference.GetMatrix();
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxReference::~gpxReference()
{
	PROXY_REMOVE();
}

//--------------------------------------------------------------------
// GetMatrix for this named refence
//--------------------------------------------------------------------
maMatrix4x4 gpxReference::GetMatrix() const
{
#if USE_PROXIES
	//bga - this is the big hack that is holding the multi-threaded
	// version together right now. A correct way to do this
	// is to proxy the entire scene graph and run all animations on that
	// scene graph and then push only the transforms through to the
	// rendering scene graph copy. 
	// This hack prevents conflicts where two threads alter the
	// total transform and bboxs of the scene graph and 
	// allows the capture mode to be correct, but
	// makes the interactive version one frame behind.
	//
	if (g3dThreadControl::IsRenderThreadActive())
	{
		//oh boy...
		const_cast<gpxReference*>(this)->SetNeedsUpdate(true);

		// This is not going to be exactly right, it is going to be
		// the world box from the last update, which will not
		// be the current one if the transformation has changed.
		return m_Matrix;
	}
	else
	{
		m_Reference.SetUseBoundingBox(this->GetUseBoundingBox());
		m_Matrix = m_Reference.GetMatrix();
		return m_Matrix;
	}
#else
	return m_Reference.GetMatrix();
#endif
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxReference::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	// store the world box in order to return it later
	m_Reference.SetUseBoundingBox(this->GetUseBoundingBox());
	m_Matrix = m_Reference.GetMatrix();

	this->SetNeedsUpdate(false);
#endif

	// This class doesn´t actually change anything, so return false
	return false;
}
