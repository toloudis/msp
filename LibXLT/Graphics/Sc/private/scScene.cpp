/*****************************************************************************
**	scScene.cpp
**
**		The scScene is the base class for objects that contain and manage
**	objects in a scene.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/sc/scScene.hpp"

#include "Graphics/sc/scObject.hpp"


//============================================================================
//============================================================================
scScene* scScene::sm_pActiveScene = NULL;


//----------------------------------------------------------------------------
// ActiveScene is singleton access to current active scene so that
// libraries know how to get access to it when Think()ing.
// This may return NULL.
//----------------------------------------------------------------------------
//static
scScene* scScene::ActiveScene()
{
	return sm_pActiveScene;
}

//------------------------------------------------------------------------
// Set this scene to be the active scene
//------------------------------------------------------------------------
void scScene::SetAsActiveScene()
{
	sm_pActiveScene = this;
}

//----------------------------------------------------------------------------
//	DestroyObject removes an object from the scene AND deletes it
//----------------------------------------------------------------------------
void scScene::DestroyObject(scObject* i_Obj, int i_LayerIndex)
{
	RemoveObject(i_Obj, i_LayerIndex);
	delete i_Obj;
}
