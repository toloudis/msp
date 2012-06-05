/****************************************************************************\
**	g3dVSMRendererDX11.hpp
**
**	The g3dVSMRendererDX11 renders the scene from a light's point of 
**	view in order to get the depth map info for shadow mapping.
**  Summed area talbe is compute after obtain the depth map. 
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_VSMRENDERERDX11_HPP
#error g3dVSMRendererDX11.hpp multiply included
#endif
#define G3D_VSMRENDERERDX11_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class effMaskAlphaData;
class g3dSceneNode;
class g3dLayer;
class matMaterial;
class matShadowMap;

class g3dVSMRendererDX11 : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g3dVSMRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~g3dVSMRendererDX11();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
				 const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
				 float i_fSimTime );

	void ReleaseResources();

	static void InitStates();
	static void CleanupStates();

private:
	matShadowMap* m_tempHTarget;
	matShadowMap* m_tempWTarget;
	
	void world_space_render( g3dSceneNode* i_pNode, 
							 const maPoint3d &i_CameraPos,
							 bool i_bRenderLowRes,
							 bool i_bDoClip );
	void setup_layer(g3dLayer& i_Layer);
	void render_layer(g3dLayer& i_Layer, const camCamera& i_Camera);

	matMaterial* m_pAlphaMaskMat;
	effMaskAlphaData* m_pMaskAlphaData;

	int DrawNode(const g3dSceneNode* i_pNode);
	int DrawHairNode(const g3dSceneNode* i_pNode);

	void SwapVSMTarget(matShadowMap*& currentVSMTarget, matShadowMap*& currentVSMSrc);
};

