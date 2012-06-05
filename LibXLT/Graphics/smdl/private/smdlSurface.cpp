/*****************************************************************************
**	smdlSurface.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlSurface.hpp"

#include "Graphics/G3d/g3dFragment.hpp"
#include "Graphics/G3d/g3dVertexBuffer.hpp"


//============================================================================
// Static variables
//============================================================================
envMutex smdlSurface::sm_SurfaceMutex;


//--------------------------------------------------------------------
// Get access to the mutex to use to synchronize all 
// animation threads.
//--------------------------------------------------------------------
envMutex& smdlSurface::GetSurfaceMutex()
{
	return sm_SurfaceMutex;
}

//--------------------------------------------------------------------
// UnlockFragment - Only the "Unlock" actually makes any D3D calls.
//	So, any synchronization of threads needs to be done on the
//	unlock call.
//--------------------------------------------------------------------
unsigned char* smdlSurface::LockFragment(g3dFragment *i_pFragment)
{
	// Lock all animation threads while we update the fragment.
	// Only the "Unlock" actually makes any D3D calls.
	// Only needed when the DirectX device is not
	// created with the multithreading flag.
	envScopedLock scene_lock(smdlSurface::GetSurfaceMutex());

	// Unlock the vertex buffer
	return i_pFragment->Lock();
}

//--------------------------------------------------------------------
// UnlockFragment - Only the "Unlock" actually makes any D3D calls.
//	So, any synchronization of threads needs to be done on the
//	unlock call.
//--------------------------------------------------------------------
void smdlSurface::UnlockFragment(g3dFragment *i_pFragment)
{
	// Lock all animation threads while we update the fragment.
	// Only the "Unlock" actually makes any D3D calls.
	// Only needed when the DirectX device is not
	// created with the multithreading flag.
	envScopedLock scene_lock(smdlSurface::GetSurfaceMutex());

	// Unlock the vertex buffer
	i_pFragment->Unlock();
}

//--------------------------------------------------------------------
// LockVertexBuffer - Only the "Unlock" actually makes any D3D calls.
//	So, any synchronization of threads needs to be done on the
//	unlock call.
//--------------------------------------------------------------------
unsigned char* smdlSurface::LockVertexBuffer(g3dVertexBuffer *i_pVertexBuffer)
{
	// Lock all animation threads while we update the fragment.
	// Only needed when the DirectX device is not
	// created with the multithreading flag.
	envScopedLock scene_lock(smdlSurface::GetSurfaceMutex());

	// lock the vertex buffer
	return i_pVertexBuffer->Lock();
}

//--------------------------------------------------------------------
// UnlockVertexBuffer - Only the "Unlock" actually makes any D3D calls.
//	So, any synchronization of threads needs to be done on the
//	unlock call.
//--------------------------------------------------------------------
void smdlSurface::UnlockVertexBuffer(g3dVertexBuffer *i_pVertexBuffer)
{
	// Lock all animation threads while we update the fragment.
	// Only the "Unlock" actually makes any D3D calls.
	// Only needed when the DirectX device is not
	// created with the multithreading flag.
	envScopedLock scene_lock(smdlSurface::GetSurfaceMutex());

	// Unlock the vertex buffer
	i_pVertexBuffer->Unlock();
}

