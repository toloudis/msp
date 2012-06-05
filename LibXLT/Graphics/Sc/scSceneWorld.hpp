/*****************************************************************************
**	scSceneWorld.hpp
**
**		The scSceneWorld manages a single scene graph for world objects.
**	This is just an example scSceneWorld implementation for simple programs.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SC_SCENEWORLD_HPP
#error scSceneWorld.hpp multiply included
#endif
#define SC_SCENEWORLD_HPP

#ifndef SC_SCENE_HPP
#include "Graphics/sc/scScene.hpp"
#endif


//============================================================================
//============================================================================
class g3dSceneNode;
class scObjectMgr;


//============================================================================
//============================================================================
class scSceneWorld : public scScene
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	scSceneWorld();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual ~scSceneWorld();

	//----------------------------------------------------------------------------
	//	AddObject adds a object to the world root
	//----------------------------------------------------------------------------
	virtual void AddObject(scObject* i_Obj);

	//----------------------------------------------------------------------------
	//	RemoveObject removes an object from the world root
	//----------------------------------------------------------------------------
	virtual void RemoveObject(scObject* i_Obj);

	//------------------------------------------------------------------------
	//	Think calls animate on all scObjects in the list and any event
	//	handlers associated with the object
	//------------------------------------------------------------------------
	virtual void Think( float i_fSimulationTime );

private:
		g3dSceneNode *m_pRoot;
		scObjectMgr *m_pObjMgr;
};

