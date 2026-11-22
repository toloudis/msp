/****************************************************************************\
**	shdwVelocityMapRendererVk.hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_SHDW_VELOCITYMAPRENDERERVK_HPP
#error shdwVelocityMapRendererVk.hpp multiply included
#endif
#define GVK_SHDW_VELOCITYMAPRENDERERVK_HPP

#ifndef GVK_SHDW_SCENERENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwSceneRendererVk.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

#ifndef GVK_TMESH_VERTEXBUFFER_HPP
#include "GraphicsVulkan/tmesh/tmeshVertexBuffer.hpp"
#endif

#ifndef GVK_TMESH_FRAG_HPP
#include "GraphicsVulkan/tmesh/tmeshFrag.hpp"
#endif

#ifndef GVK_SHDW_VELOCITYSTATEMANAGER_HPP
#include "GraphicsVulkan/shdw/shdwVelocityStateManager.hpp"
#endif

class effTexturedData;

namespace gvk
{

#define USE_PASS

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class matRenderTargetTexture;

class shdwVelocityMapRendererVk : public shdwSceneRendererVk
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwVelocityMapRendererVk();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwVelocityMapRendererVk();

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

}
