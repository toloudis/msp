/****************************************************************************\
**	shdwGIPreviewRendererVk.hpp
**
**	The shdwGIPreviewRendererVk renders the scene with global illumination
**	texture only.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_SHDW_GIPREVIEWRENDERERVK_HPP
#error shdwGIPreviewRendererVk.hpp multiply included
#endif
#define GVK_SHDW_GIPREVIEWRENDERERVK_HPP

#ifndef GVK_SHDW_SCENERENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwSceneRendererVk.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif
#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

class effTexturedData;
class g3dSceneNode;
class g3dLayer;
class matMaterial;

namespace gvk
{

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class matRenderTargetTexture;

class shdwGIPreviewRendererVk : public shdwSceneRendererVk
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwGIPreviewRendererVk();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwGIPreviewRendererVk();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
				 const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
				 float i_fSimTime );

	void ReleaseResources();

	virtual g2dPFD::PixelFormat GetRawBufferPFD();

	static void InitStates();
	static void CleanupStates();

private:
	//int DrawNode(const g3dSceneNode* i_pNode);
	void CopyToBackBuf(matRenderTargetTexture* pTex);

	shdwPassTraversal m_SceneDatabase;

	g2dRenderTarget* m_pWindow;
	int m_Width, m_Height;
	int m_OverscanSize;
	void CreateSurfaces(g2dRenderTarget* i_pWindow);
};

}
