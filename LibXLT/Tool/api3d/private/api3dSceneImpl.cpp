/*****************************************************************************
**	api3dSceneImpl.cpp
**
**		The api3dSceneImpl manages a scene graph with some layers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/private/api3dSceneImpl.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/ent/entEntity.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/sc/scObjectMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	//============================================================================
	//	constants
	//============================================================================
	const int c_INDEX_WORLDROOT = 0;
	const int c_INDEX_CAMERAROOT = 1;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
api3dSceneImpl::api3dSceneImpl()
{
	//	initially we have just the world and camera layers
	m_pRoots.resize( 2 );

	// root world layer, owned by base class
	//
	m_pRoots[c_INDEX_WORLDROOT] = new g3dSceneNode();
	this->AppendLayer(new g3dLayer( m_pRoots[c_INDEX_WORLDROOT], g3dLayer::e_ZBuffer, g3dLayer::e_World,
									g3dLayer::e_Multiplicative, true, true, false) );

	// camera layer, owned by base class
	//
	m_pRoots[c_INDEX_CAMERAROOT] = new g3dSceneNode();
	this->AppendLayer(new g3dLayer( m_pRoots[c_INDEX_CAMERAROOT] , g3dLayer::e_ZBuffer,
									g3dLayer::e_Camera ) );

	//	create the object manager too
	m_pObjMgr = new scObjectMgr();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
api3dSceneImpl::~api3dSceneImpl()
{
	delete m_pObjMgr;
	envSTLHelpers::DeleteContainer( m_pRoots );
}

//----------------------------------------------------------------------------
//	AddObject adds a object to the world root
//----------------------------------------------------------------------------
void api3dSceneImpl::AddObject(scObject* i_Obj, int i_LayerIndex)
{
	 // negative number means put it into the world root
	int layer = (i_LayerIndex < 0) ? c_INDEX_WORLDROOT : i_LayerIndex;
	DBG_ASSERT(layer < m_pRoots.size(), "Layer Index out of range.");
	m_pRoots[layer]->AddChild(i_Obj->GetBase());
	m_pObjMgr->Add(i_Obj);

	// Check for LOD
	scAnimatableLODObject* pALODObj	= dynamic_cast<scAnimatableLODObject*>(i_Obj);
	if (pALODObj) 
		m_AnimatableLODObjects.push_back( pALODObj );
	scLODObject* pLODObj		= dynamic_cast<scLODObject*>(i_Obj);
	if (pLODObj) 
		m_LODObjects.push_back( pLODObj );
}


//----------------------------------------------------------------------------
//	RemoveObject removes an object from the world root
//----------------------------------------------------------------------------
void api3dSceneImpl::RemoveObject(scObject* i_Obj, int i_LayerIndex)
{
	int layer = i_LayerIndex;
	if (i_LayerIndex < 0)
	{
		// negative number means look for it in all roots
		for (int i=0; i<m_pRoots.size(); i++)
		{
			// It is actually safe to call remove child if it isn't a child, but 
			// this seems safer in the long run.
			if (m_pRoots[i]->HasChild(i_Obj->GetBase()))
			{
				layer = i;
				break;
			}
		}
	}
		
	if (layer >= 0)
	{
		DBG_ASSERT(layer < m_pRoots.size(), "Layer Index out of range.");
		m_pRoots[layer]->RemoveChild(i_Obj->GetBase());
	}
	m_pObjMgr->Remove(i_Obj);
	
	// Check for LOD
	scAnimatableLODObject* pALODObj	= dynamic_cast<scAnimatableLODObject*>(i_Obj);
	if (pALODObj) 
		envSTLHelpers::RemoveOneValue(m_AnimatableLODObjects, pALODObj);
	scLODObject* pLODObj		= dynamic_cast<scLODObject*>(i_Obj);
	if (pLODObj) 
		envSTLHelpers::RemoveOneValue(m_LODObjects, pLODObj);
}

//----------------------------------------------------------------------------
//	AddEntity adds a object to the scene.
//----------------------------------------------------------------------------
void api3dSceneImpl::AddEntity(entEntity* i_Obj)
{
	AddObject(i_Obj->Object(), c_INDEX_WORLDROOT);
	m_EntityList.push_back(i_Obj);
}

//----------------------------------------------------------------------------
//	RemoveEntity removes an object from the scene.
//----------------------------------------------------------------------------
void api3dSceneImpl::RemoveEntity(entEntity* i_Obj)
{
	RemoveObject(i_Obj->Object(), c_INDEX_WORLDROOT);
	envSTLHelpers::RemoveOneValue(m_EntityList, i_Obj);
}


//------------------------------------------------------------------------
//	AddLayer - add a layer to the scene.  keep the index to access the
//	scene node
//------------------------------------------------------------------------
//virtual
int api3dSceneImpl::AddLayer(g3dLayer::SortMethod i_SortMethod,
							  g3dLayer::ModelSpace i_ModelSpace,
							  g3dLayer::BlendMethod i_BlendMethod,
							  bool i_bFogEnabled,
							  bool i_bShadows,
							  bool i_bPreLit,
							  bool i_bClearDepth)
{
	int index = m_pRoots.size();
	m_pRoots.push_back( new g3dSceneNode() );

	this->AppendLayer( new g3dLayer( m_pRoots[index],
									i_SortMethod,
									i_ModelSpace,
									i_BlendMethod,
									i_bFogEnabled,
									i_bShadows,
									i_bPreLit,
									i_bClearDepth) );
	return index;
}

//------------------------------------------------------------------------
//	Think calls animate on all scObjects in the list and any event
//	handlers associated with the object
//------------------------------------------------------------------------
void api3dSceneImpl::Think( float i_fSimulationTime )
{
	// set ActiveScene() pointer to allow other libraries access
	// to this scene during "Think()"
	//
	this->SetAsActiveScene();

	m_pObjMgr->Think(i_fSimulationTime);

	std::list<entEntity*>::iterator	it, end = m_EntityList.end();
	for (it = m_EntityList.begin(); it != end; ++it)
	{
		(*it)->Think(i_fSimulationTime);
	}

	// Update transform matrices within layers
	this->UpdateWorldData();
}

//--------------------------------------------------------------------
//	allow any LOD objects to update themselves (i.e. swap models)
//--------------------------------------------------------------------
void api3dSceneImpl::UpdateLOD( const maPoint3d& i_OriginPoint )
{
	//	let the animatable objects consider updating their level of detail
	//
	int i;
	for ( i = 0 ; i < m_AnimatableLODObjects.size() ; ++i )
	{
		m_AnimatableLODObjects[i]->UpdateLOD( i_OriginPoint );
	}

	//	let the objects consider updating their level of detail
	//
	for ( i = 0 ; i < m_LODObjects.size() ; ++i )
	{
		m_LODObjects[i]->UpdateLOD( i_OriginPoint );
	}
}

//------------------------------------------------------------------------
// These direct acess functions are just for the certain old style 
//	packages and should be removed when those package is reorganized.
//------------------------------------------------------------------------
g3dSceneNode* api3dSceneImpl::GetRoot( int i_Index )
{
	return m_pRoots[i_Index];
}
int api3dSceneImpl::GetNumberOfRoots()
{
	return m_pRoots.size();
}

//--------------------------------------------------------------------
// These functions return the index of the default world layer
//	and the camera space layer.
//--------------------------------------------------------------------
int	api3dSceneImpl::WorldLayerIndex()
{
	return c_INDEX_WORLDROOT;
}
int	api3dSceneImpl::CameraLayerIndex()
{
	return c_INDEX_CAMERAROOT;
}

