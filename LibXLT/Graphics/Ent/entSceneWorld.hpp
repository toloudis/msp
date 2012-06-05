/*****************************************************************************
**  entSceneWorld.hpp
**
**      The entSceneWorld manages a single scene graph for world objects.
**	This is just an example entSceneWorld implementation for simple programs.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef ENT_SCENEWORLD_HPP
#error entSceneWorld.hpp multiply included
#endif
#define ENT_SCENEWORLD_HPP

#ifndef ENT_SCENE_HPP
#include "Graphics/ent/entScene.hpp"
#endif

#include <list>


//============================================================================
//============================================================================
class g3dSceneNode;
class scObjectMgr;


//============================================================================
//============================================================================
class entSceneWorld : public entScene
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	entSceneWorld();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual ~entSceneWorld();

	//----------------------------------------------------------------------------
	//	AddObject adds a object to the world root
	//----------------------------------------------------------------------------
	virtual void AddObject(scObject* i_Obj);

	//----------------------------------------------------------------------------
	//	RemoveObject removes an object from the world root
	//----------------------------------------------------------------------------
	virtual void RemoveObject(scObject* i_Obj);

	//----------------------------------------------------------------------------
	//	AddEntity adds a object to the scene.
	//----------------------------------------------------------------------------
	virtual void AddEntity(entEntity* i_Obj);

	//----------------------------------------------------------------------------
	//	RemoveEntity removes an object from the scene.
	//----------------------------------------------------------------------------
	virtual void RemoveEntity(entEntity* i_Obj);

	//------------------------------------------------------------------------
	//	Think calls animate on all scObjects in the list and any event
	//	handlers associated with the object
	//------------------------------------------------------------------------
	virtual void Think( float i_fSimulationTime );

private:
		g3dSceneNode *m_pRoot;
		scObjectMgr *m_pObjMgr;
		std::list<entEntity*> m_EntityList;
};
