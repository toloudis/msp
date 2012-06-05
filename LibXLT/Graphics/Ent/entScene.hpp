/*****************************************************************************
**  entScene.hpp
**
**      The entScene is the base class for objects that contain and manage
**	objects in a scene.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef ENT_SCENE_HPP
#error entScene.hpp multiply included
#endif
#define ENT_SCENE_HPP

#ifndef SC_SCENE_HPP
#include "Graphics/sc/scScene.hpp"
#endif


//============================================================================
//============================================================================
class entEntity;


//============================================================================
//============================================================================
class entScene : public scScene
{
public:
	//----------------------------------------------------------------------------
	//	AddEntity adds a object to the scene.
	//----------------------------------------------------------------------------
	virtual void AddEntity(entEntity* i_Obj) = 0;

	//----------------------------------------------------------------------------
	//	RemoveEntity removes an object from the scene.
	//----------------------------------------------------------------------------
	virtual void RemoveEntity(entEntity* i_Obj) = 0;
};

