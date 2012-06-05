/****************************************************************************\
**	shdwShadowsOnlyRendererDX11.hpp
**
**	The shdwShadowsOnlyRendererDX11 renders the scene with ambient occlusion
**	texture only.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_SHADOWSONLYRENDERERDX11_HPP
#error shdwShadowsOnlyRendererDX11.hpp multiply included
#endif
#define SHDW_SHADOWSONLYRENDERERDX11_HPP

#ifndef SHDW_SCENERENDERERDX11_HPP
#include "GraphicsDX11/shdw/shdwSceneRendererDX11.hpp" 
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef SHDW_PASSTRAVERSAL_HPP
#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class effTexturedData;
class g3dSceneNode;
class g3dLayer;
class g3dLight;
class g3dProjectedLight;
class matMaterial;
class matRenderTargetTexture;
class camCamera;
class g2dWindowDX11;

class shdwShadowsOnlyRendererDX11 : public shdwSceneRendererDX11
{
public:
	//--------------------------------------------------------------------
	// true to make a multiplicative shadow masking render
	// false to make an additive grayscale light render (luminance values)
	//--------------------------------------------------------------------
	shdwShadowsOnlyRendererDX11(bool i_bMasking);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwShadowsOnlyRendererDX11();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
				 const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
				 float i_fSimTime );

	void ReleaseResources();

	//--------------------------------------------------------------------
	//	Sometimes we need to access the pre-buffer pixels that don't get 
	//	drawn to a window.
	//--------------------------------------------------------------------
	virtual g2dRenderTarget* GetRawBuffer();

	virtual g2dPFD::PixelFormat GetRawBufferPFD();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void InitStates();
	static void CleanupStates();

private:
	void world_space_render( g3dSceneNode* i_pNode, 
							 const maPoint3d &i_CameraPos,
							 bool i_bRenderLowRes );
	void setup_layer(g3dLayer& i_Layer);
	void render_layer(g3dLayer& i_Layer, const camCamera& i_Camera);

	bool m_bMasking;
	
	matMaterial* m_pIlluminationMat;
	matMaterial* m_pShadowMat;
	effTexturedData* m_pShadowData;
	matMaterial* m_pHairMat;

	//--------------------------------------------------------------------
	//	Test node against light set and light frustum. If it passes, draw it.
	//--------------------------------------------------------------------
	int CullOrDrawNode(const sNodePlusState& i_Node, 
		g3dLight* i_pLight, const g3dProjectedLight* i_pProjLight);

	int DrawNode(const g3dSceneNode* i_pNode, 
		g3dLight* i_pLight, const g3dProjectedLight* i_pProjLight);

	int DrawHairNode(const g3dSceneNode* i_pNode, 
		g3dLight* i_pLight, 
		const g3dProjectedLight* i_pProjLight);


	shdwPassTraversal m_SceneDatabase;
	int m_Width, m_Height;
	void CreateSurfaces(g2dRenderTarget* i_pWindow);
	void CopyToBackBuf(matRenderTargetTexture* pSrcTex);

	g2dWindowDX11* m_pWindow;
	const camCamera* m_pCamera;
};


