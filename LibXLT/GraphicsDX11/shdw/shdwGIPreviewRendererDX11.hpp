/****************************************************************************\
**	shdwGIPreviewRendererDX11.hpp
**
**	The shdwGIPreviewRendererDX11 renders the scene with global illumination
**	texture only.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_GIPREVIEWRENDERERDX11_HPP
#error shdwGIPreviewRendererDX11.hpp multiply included
#endif
#define SHDW_GIPREVIEWRENDERERDX11_HPP

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
class matMaterial;
class matRenderTargetTexture;
class g2dWindowDX11;

class shdwGIPreviewRendererDX11 : public shdwSceneRendererDX11
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwGIPreviewRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwGIPreviewRendererDX11();

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

	g2dWindowDX11* m_pWindow;
	int m_Width, m_Height;
	int m_OverscanSize;
	void CreateSurfaces(g2dRenderTarget* i_pWindow);
};