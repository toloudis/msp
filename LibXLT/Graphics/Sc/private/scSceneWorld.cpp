/*****************************************************************************
**	scSceneWorld.cpp
**
**		The scSceneWorld manages a single scene graph for world objects.
**	This is just an example scSceneWorld implementation for simple programs.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/sc/scSceneWorld.hpp"

#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Graphics/sc/scObjectMgr.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
scSceneWorld::scSceneWorld()
{
	m_pRoot = new g3dSceneNode();
	this->AppendLayer(new g3dLayer(m_pRoot)); // root layer, owned by base class
	m_pObjMgr = new scObjectMgr();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
scSceneWorld::~scSceneWorld()
{
	delete m_pObjMgr;
	delete m_pRoot;
}

//----------------------------------------------------------------------------
//	AddObject adds a object to the world root
//----------------------------------------------------------------------------
void scSceneWorld::AddObject(scObject* i_Obj)
{
	m_pRoot->AddChild(i_Obj->GetBase());
	m_pObjMgr->Add(i_Obj);
}


//----------------------------------------------------------------------------
//	RemoveObject removes an object from the world root
//----------------------------------------------------------------------------
void scSceneWorld::RemoveObject(scObject* i_Obj)
{
	m_pRoot->RemoveChild(i_Obj->GetBase());
	m_pObjMgr->Remove(i_Obj);
}


//------------------------------------------------------------------------
//	Think calls animate on all scObjects in the list and any event
//	handlers associated with the object
//------------------------------------------------------------------------
void scSceneWorld::Think( float i_fSimulationTime )
{
	// set ActiveScene() pointer to allow other libraries access
	// to this scene during "Think()"
	this->SetAsActiveScene();

	m_pObjMgr->Think(i_fSimulationTime);
}
