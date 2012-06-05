/*****************************************************************************
**	api3dScene.hpp
**
**	Adds and removes objects from scene
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_SCENE_HPP
#error api3dScene.hpp multiply included
#endif
#define API3D_SCENE_HPP

#ifndef G3D_LAYER_HPP
#include "Graphics/g3d/g3dLayer.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class api3dObject;
class api3dParticleGenerator;
class camCamera;
class g3dProjectedLight;
class g3dScene;
class g3dSceneNode;
struct fogParams;
struct ssaoParams;
struct ssgiParams;
struct MotionBlurParams;
struct g3dAmbientEnvState;

//============================================================================
//============================================================================
namespace api3dScene
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Initialize();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Think(float i_fSimulationTime);

	//--------------------------------------------------------------------
	//	allow any LOD objects to update themselves (i.e. swap models)
	//--------------------------------------------------------------------
	void UpdateLOD( const maPoint3d& i_OriginPoint );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g3dScene*	GetScene();

	//------------------------------------------------------------------------
	//	SetFog sets fog parameters for this scene
	//------------------------------------------------------------------------
	void SetFog( const fogParams& i_FogParams );

	//------------------------------------------------------------------------
	//	SetSSAO sets ssao parameters for this scene
	//------------------------------------------------------------------------
	void SetSSAO(const ssaoParams& i_SSAOParams);

	//------------------------------------------------------------------------
	//	SetSSGI sets ssgi parameters for this scene
	//------------------------------------------------------------------------
	void SetSSGI(const ssgiParams& i_SSGIParams);

	//------------------------------------------------------------------------
	//	SetMotionBlur sets Motion Blur parameters for this scene
	//------------------------------------------------------------------------
	void SetMotionBlur(const MotionBlurParams& i_MotionBlurParams);

	//------------------------------------------------------------------------
	// Settings of global ambient state
	//------------------------------------------------------------------------
	void SetGlobalAmbient(const g3dAmbientEnvState& i_GlobalAmbient);

	//------------------------------------------------------------------------
	//	AddLayer - add a layer to the scene.  keep the index to access the
	//	scene node
	//------------------------------------------------------------------------
	int AddLayer(g3dLayer::SortMethod i_SortMethod,
				 g3dLayer::ModelSpace i_ModelSpace,
				 g3dLayer::BlendMethod i_BlendMethod,
				 bool i_bFogEnabled,
				 bool i_bShadows,
				 bool i_bPreLit,
				 bool i_bClearDepth);

	//------------------------------------------------------------------------
	//	get the scene root for a given layer
	//------------------------------------------------------------------------
	g3dSceneNode*	GetRoot( int i_LayerIndex );

	//--------------------------------------------------------------------
	// These functions return the index of the default world layer
	//	and the camera space layer.
	//--------------------------------------------------------------------
	int	WorldLayerIndex();
	int	CameraLayerIndex();

	//
	//	object
	//

	//--------------------------------------------------------------------
	//  Adds object to scene
	//--------------------------------------------------------------------
	void  AddObject(api3dObject *i_pObj, int i_LayerIndex = -1);

	//--------------------------------------------------------------------
	//  Removes object from scene
	//--------------------------------------------------------------------
	void  RemoveObject(api3dObject *i_pObj, int i_LayerIndex = -1);

	//
	//	ParticleGenerator
	//

	//--------------------------------------------------------------------
	//  Adds ParticleGenerator to scene
	//--------------------------------------------------------------------
	void  AddParticleGenerator(api3dParticleGenerator *i_pObj);

	//--------------------------------------------------------------------
	//  Removes ParticleGenerator from scene
	//--------------------------------------------------------------------
	void  RemoveParticleGenerator(api3dParticleGenerator *i_pObj);

	//--------------------------------------------------------------------
	// Register/deregister a light to be resized to view whole scene
	//--------------------------------------------------------------------
	void AddAutoResizeLight(g3dProjectedLight *i_pLight, camCamera *i_pShadowCamera);
	void RemoveAutoResizeLight(g3dProjectedLight *i_pLight);

}	// end of namespace
