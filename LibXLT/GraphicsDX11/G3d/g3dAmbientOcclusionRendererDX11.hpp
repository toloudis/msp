/****************************************************************************\
**	g3dAmbientOcclusionRendererDX11.hpp
**
**	The g3dAmbientOcclusionRendererDX11 renders the scene with ambient occlusion
**	texture only.  This renderer does not know how to do the "compute ambient
**	occlusion" render pass.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_AMBIENTOCCLUSIONRENDERERDX11_HPP
#error g3dAmbientOcclusionRendererDX11.hpp multiply included
#endif
#define G3D_AMBIENTOCCLUSIONRENDERERDX11_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class effTexturedData;
class g3dSceneNode;
class g3dLayer;
class matMaterial;

class g3dAmbientOcclusionRendererDX11 : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g3dAmbientOcclusionRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~g3dAmbientOcclusionRendererDX11();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
				 const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
				 float i_fSimTime );

	void ReleaseResources();

private:
	void world_space_render( g3dSceneNode* i_pNode, 
							 const maPoint3d &i_CameraPos,
							 bool i_bRenderLowRes );
	void setup_layer(g3dLayer& i_Layer);
	void render_layer(g3dLayer& i_Layer, const camCamera& i_Camera);

	matMaterial* m_pAOMat;
	effTexturedData* m_pAOData;

	int DrawNode(const g3dSceneNode* i_pNode);
};

