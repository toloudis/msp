/****************************************************************************\
**	shdwAOPreviewRendererDX11.hpp
**
**	The shdwAOPreviewRendererDX11 renders the scene with ambient occlusion
**	texture only.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_AOPREVIEWRENDERERDX11_HPP
#error shdwAOPreviewRendererDX11.hpp multiply included
#endif
#define SHDW_AOPREVIEWRENDERERDX11_HPP

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

class shdwAOPreviewRendererDX11 : public shdwSceneRendererDX11
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwAOPreviewRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwAOPreviewRendererDX11();

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

	shdwPassTraversal m_SceneDatabase;
	int m_Width, m_Height;
	int m_OverscanSize;
	void CreateSurfaces(g2dRenderTarget* i_pWindow);
};


