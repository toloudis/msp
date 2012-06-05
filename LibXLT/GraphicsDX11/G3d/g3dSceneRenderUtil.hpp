/****************************************************************************\
**	g3dSceneRenderUtil.hpp
**
**	The g3dSceneRenderUtil is a collection of reusable code for use in scene 
**  rendering.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_SCENERENDERUTIL_HPP
#error g3dSceneRenderUtil.hpp multiply included
#endif
#define G3D_SCENERENDERUTIL_HPP

#ifndef G3D_RENDERSTATECACHE_HPP
#include "GraphicsDX11/g3d/g3dRenderStateCache.hpp"
#endif

#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif

#ifndef G3D_LAYER_HPP
#include "Graphics/g3d/g3dLayer.hpp"
#endif

#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif

#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#include <vector>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class camCamera;
class g3dRenderStateTraverser;
class g3dProjectedLight;
class g3dSceneNode;
class maAxisBox;

enum ClipResult
{
	e_Reject,
	e_Clip,
	e_NoClip
};
typedef std::vector<g3dSceneNode*> SceneNodeVector;
namespace g3dSceneRenderUtil
{
	void set_transforms(g3dLayer::ModelSpace i_ModelSpace);

	void SetViewingTransforms(const camCamera& i_Camera, g3dLayer::ModelSpace i_ModelSpace = g3dLayer::e_World);

	void enable_lights(  );
	void gather_fragment_nodes( g3dSceneNode* i_pNode, SceneNodeVector& o_FragNodes );
	//void update_world_data( g3dSceneNode* i_pNode ); // move to g3dScene
	int nonworld_space_render( g3dSceneNode* i_pNode );
	bool screen_space_sort( g3dSceneNode* i_pNode1, g3dSceneNode* i_pNode2 );

	ClipResult get_box_vis( const maAxisBox& i_Box, const maMatrix4x4* i_CameraProjection = NULL, bool i_bIgnoreFarPlane = false );

	//------------------------------------------------------------------------
	// See if we can cull from resolution. Returns false if resolutions
	//	don't match. May alter the io_LowRes flag if a node has an
	//	override flag set.
	//------------------------------------------------------------------------
	bool check_resolution(g3dSceneNode *i_pNode, bool &io_LowRes);
	
	//------------------------------------------------------------------------
	//	return number of primitives drawn. 
	//------------------------------------------------------------------------
	int DrawNode(const g3dSceneNode* i_pNode);

	//------------------------------------------------------------------------
	//	return number of primitives drawn
	//------------------------------------------------------------------------
	int DrawNodeAmbient(const g3dSceneNode* i_pNode, const g3dAmbientEnvState& i_AmbientEnvironment);

	//------------------------------------------------------------------------
	//	return number of primitives drawn
	//------------------------------------------------------------------------
	int DrawNodeEnvironment(const g3dSceneNode* i_pNode, const g3dAmbientEnvState& i_AmbientEnvironment);

	//------------------------------------------------------------------------
	//	return number of primitives drawn. 
	//------------------------------------------------------------------------
	int DrawNodeLit(const g3dSceneNode* i_pNode, g3dLight* i_pLight, const g3dProjectedLight* i_pProjLight, const g3dAmbientEnvState* i_pAmbientEnvironment = NULL );

	//------------------------------------------------------------------------
	//	decide which shadow technique to use 
	//------------------------------------------------------------------------
	matShaderEffect::Technique SelectShadowTechnique(const g3dProjectedLight* i_pProjLight);

}; // namespace shdwRenderUtil
