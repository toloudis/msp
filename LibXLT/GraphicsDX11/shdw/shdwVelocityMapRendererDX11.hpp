/****************************************************************************\
**	shdwVelocityMapRendererDX11.hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_VELOCITYMAPRENDERERDX11_HPP
#error shdwVelocityMapRendererDX11.hpp multiply included
#endif
#define SHDW_VELOCITYMAPRENDERERDX11_HPP

#ifndef SHDW_SCENERENDERERDX11_HPP
#include "GraphicsDX11/shdw/shdwSceneRendererDX11.hpp" 
#endif

#ifndef SHDW_PASSTRAVERSAL_HPP
#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"
#endif

#ifndef TMESH_VERTEXBUFFER_HPP
#include "GraphicsDX11/tmesh/tmeshVertexBuffer.hpp"
#endif

#ifndef TMESH_FRAG_HPP
#include "GraphicsDX11/tmesh/tmeshFrag.hpp"
#endif

#ifndef SHDW_VELOCITYSTATEMANAGER_HPP
#include "GraphicsDX11/shdw/shdwVelocityStateManager.hpp"
#endif


//#include <D3DX11Effect.h>

#define USE_PASS

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class matRenderTargetTexture;
class effTexturedData;

class shdwVelocityMapRendererDX11 : public shdwSceneRendererDX11
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwVelocityMapRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwVelocityMapRendererDX11();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
		const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ReleaseResources();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual g2dRenderTarget* GetRawBuffer();
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual g2dPFD::PixelFormat GetRawBufferPFD();

	static void InitStates();
	static void CleanupStates();

protected:
	// lazy evaluate whether to recreate temp surfaces
	// based on whether width/height of window have changed.
	int m_Width, m_Height;

	maMatrix4x4					ViewProjectionTransform;
	maMatrix4x4					ViewProjectionTransform_Old;

	VelocityStateManager*		m_pVelocityStateManager;

	shared_ptr<tmeshVertexBuffer>	Vertices_Old;
	shared_ptr<tmeshVertexBuffer>	Vertices;

	g2dRenderTarget*			m_pWindow;	
	shdwPassTraversal			m_SceneDatabase;
	matMaterial*				m_pVelocityMat;
	effTexturedData*			m_pVelocityData;

	float m_lastTime;

	void DrawNode(const g3dSceneNode* i_pNode);

	void CreateSurfaces(g2dRenderTarget* i_pWindow);
};

