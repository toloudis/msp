/*****************************************************************************
**	scScene.hpp
**
**		The scScene is the base class for objects that contain and manage
**	objects in a scene.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SC_SCENE_HPP
#error scScene.hpp multiply included
#endif
#define SC_SCENE_HPP

#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif


//============================================================================
//============================================================================
class scObject;


//============================================================================
//============================================================================
class scScene : public g3dScene
{
public:
	//----------------------------------------------------------------------------
	// ActiveScene is singleton access to current active scene so that
	// libraries know how to get access to it when Think()ing.
	// This may return NULL.
	//----------------------------------------------------------------------------
	static scScene*	ActiveScene();

	//------------------------------------------------------------------------
	// Set this scene to be the active scene
	//------------------------------------------------------------------------
	void SetAsActiveScene();

	//----------------------------------------------------------------------------
	//	AddObject adds a object to the scene.
	//----------------------------------------------------------------------------
	virtual void AddObject(scObject* i_Obj, int i_LayerIndex = -1) = 0;

	//----------------------------------------------------------------------------
	//	RemoveObject removes an object from the scene.
	//----------------------------------------------------------------------------
	virtual void RemoveObject(scObject* i_Obj, int i_LayerIndex = -1) = 0;

	//----------------------------------------------------------------------------
	//	DestroyObject removes an object from the scene AND deletes it
	//----------------------------------------------------------------------------
	 virtual void DestroyObject(scObject* i_Obj, int i_LayerIndex = -1);

	//------------------------------------------------------------------------
	//	Think calls animate on all scObjects in the list and any event
	//	handlers associated with the object.
	//
	//	Note: Derived classes should set the "ActiveScene" pointer
	//	during Think().
	//------------------------------------------------------------------------
	virtual void Think( float i_fSimulationTime ) = 0;

private:
	static scScene* sm_pActiveScene;
};

