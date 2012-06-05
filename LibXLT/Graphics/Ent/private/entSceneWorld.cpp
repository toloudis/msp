/*****************************************************************************
**  entSceneWorld.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/ent/entSceneWorld.hpp"
#include "Graphics/ent/entEntity.hpp"

#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Graphics/sc/scObjectMgr.hpp"
#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
entSceneWorld::entSceneWorld()
{
	m_pRoot = new g3dSceneNode();
	this->AppendLayer(new g3dLayer(m_pRoot)); // root layer, owned by base class
	m_pObjMgr = new scObjectMgr();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
entSceneWorld::~entSceneWorld()
{
	delete m_pObjMgr;
	delete m_pRoot;
}

//----------------------------------------------------------------------------
//	AddObject adds a object to the world root
//----------------------------------------------------------------------------
void entSceneWorld::AddObject(scObject* i_Obj)
{
	m_pRoot->AddChild(i_Obj->GetBase());
	m_pObjMgr->Add(i_Obj);
}


//----------------------------------------------------------------------------
//	RemoveObject removes an object from the world root
//----------------------------------------------------------------------------
void entSceneWorld::RemoveObject(scObject* i_Obj)
{
	m_pRoot->RemoveChild(i_Obj->GetBase());
	m_pObjMgr->Remove(i_Obj);
}

//----------------------------------------------------------------------------
//	AddEntity adds a object to the scene.
//----------------------------------------------------------------------------
void entSceneWorld::AddEntity(entEntity* i_Obj)
{
	AddObject(i_Obj->Object());
	m_EntityList.push_back(i_Obj);
}

//----------------------------------------------------------------------------
//	RemoveEntity removes an object from the scene.
//----------------------------------------------------------------------------
void entSceneWorld::RemoveEntity(entEntity* i_Obj)
{
	RemoveObject(i_Obj->Object());
	envSTLHelpers::RemoveOneValue(m_EntityList, i_Obj);
}

//------------------------------------------------------------------------
//	Think calls animate on all scObjects in the list and any event
//	handlers associated with the object
//------------------------------------------------------------------------
void entSceneWorld::Think( float i_fSimulationTime )
{
	// set ActiveScene() pointer to allow other libraries access
	// to this scene during "Think()"
	this->SetAsActiveScene();

	m_pObjMgr->Think(i_fSimulationTime);

	std::list<entEntity*>::iterator	it, end = m_EntityList.end();
	for (it = m_EntityList.begin(); it != end; ++it)
	{
		(*it)->Think(i_fSimulationTime);
	}
}
