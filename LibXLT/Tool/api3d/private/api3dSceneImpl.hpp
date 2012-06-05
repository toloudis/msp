/*****************************************************************************
**	api3dSceneImpl.hpp
**
**		The api3dSceneImpl manages a scene graph with some layers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_SCENEIMPL_HPP
#error api3dSceneImpl.hpp multiply included
#endif
#define API3D_SCENEIMPL_HPP

#ifndef G3D_LAYER_HPP
#include "Graphics/g3d/g3dLayer.hpp"
#endif
#ifndef ENT_SCENE_HPP
#include "Graphics/ent/entScene.hpp"
#endif
#ifndef SC_ANIMATABLELODOBJECT_HPP
#include "Graphics/sc/scAnimatableLODObject.hpp"
#endif
#ifndef SC_LODOBJECT_HPP
#include "Graphics/sc/scLODObject.hpp"
#endif

#include <list>


//============================================================================
//	forward references
//============================================================================
class g3dSceneNode;
class scObjectMgr;
class prtParticleGenerator;


//============================================================================
//
//============================================================================
class api3dSceneImpl : public entScene
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	api3dSceneImpl();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual ~api3dSceneImpl();

	//
	//	World functions
	//

	//----------------------------------------------------------------------------
	//	AddObject adds a object to the world root
	//----------------------------------------------------------------------------
	virtual void AddObject(scObject* i_Obj, int i_LayerIndex = -1);

	//----------------------------------------------------------------------------
	//	RemoveObject removes an object from the world root
	//----------------------------------------------------------------------------
	virtual void RemoveObject(scObject* i_Obj, int i_LayerIndex = -1);

	//----------------------------------------------------------------------------
	//	AddEntity adds a object to the scene.
	//----------------------------------------------------------------------------
	virtual void AddEntity(entEntity* i_Obj);

	//----------------------------------------------------------------------------
	//	RemoveEntity removes an object from the scene.
	//----------------------------------------------------------------------------
	virtual void RemoveEntity(entEntity* i_Obj);

	//
	// scene impl functions
	//

	//------------------------------------------------------------------------
	//	AddLayer - add a layer to the scene.  keep the index to access the
	//	scene node
	//------------------------------------------------------------------------
	virtual int api3dSceneImpl::AddLayer(g3dLayer::SortMethod i_SortMethod,
										g3dLayer::ModelSpace i_ModelSpace,
										g3dLayer::BlendMethod i_BlendMethod,
										bool i_bFogEnabled,
										bool i_bShadows,
										bool i_bPreLit,
										bool i_bClearDepth);

	//------------------------------------------------------------------------
	//	Think calls animate on all scObjects in the list and any event
	//	handlers associated with the object
	//------------------------------------------------------------------------
	virtual void Think( float i_fSimulationTime );

	//--------------------------------------------------------------------
	//	allow any LOD objects to update themselves (i.e. swap models)
	//--------------------------------------------------------------------
	void UpdateLOD( const maPoint3d& i_OriginPoint );

	//
	//  Direct Access to scene nodes
	//

	//------------------------------------------------------------------------
	// These direct acess functions are just for the certain old style 
	//	packages and should be removed when those package is reorganized.
	//------------------------------------------------------------------------
	g3dSceneNode*	GetRoot( int i_Index );
	int GetNumberOfRoots();

	//--------------------------------------------------------------------
	// These functions return the index of the default world layer
	//	and the camera space layer.
	//--------------------------------------------------------------------
	int	WorldLayerIndex();
	int	CameraLayerIndex();

private:
	scObjectMgr*				m_pObjMgr;
	std::list<entEntity*>		m_EntityList;
	std::vector<g3dSceneNode*>	m_pRoots;

	std::vector<scAnimatableLODObject*> m_AnimatableLODObjects;
	std::vector<scLODObject*>			m_LODObjects;
};



