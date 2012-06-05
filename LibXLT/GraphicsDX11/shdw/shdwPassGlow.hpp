/****************************************************************************\
**	shdwPassGlow.hpp
**
**	Render glowing objects into a buffer and then smear that buffer over 
**	the scene.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SHDW_PASSGLOW_HPP
#error shdwPassGlow.hpp multiply included
#endif
#define SHDW_PASSGLOW_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

class camCamera;
class g3dLayer;
class g3dLight;
class g3dProjectedLight;
class shdwPassTraversal;

class shdwPassGlow : public g3dRenderPass
{
public:
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassGlow();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassGlow(g3dLayer* i_pLayer,
		const camCamera* i_pCamera, 
		matRenderTargetTexture* i_SceneTarget,	//Primary non MSAA render target w/depth
		matRenderTargetTexture* i_glowTarget );

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	~shdwPassGlow();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	virtual int Render(float i_time);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}

	static void InitStates();
	static void CleanupStates();

protected:
	shdwPassTraversal* m_sceneInfo;

	// Render objects to this texture, which will then be blurred and superimposed on scene:
	// This is intended to be a downsampled framebuffer (make size a fraction of main wnd)
	// then draw as full screen quad with random sample filtering.
	matRenderTargetTexture* m_glowTarget;

	//This is the primary target that glow get overlayed onto
	matRenderTargetTexture* m_SceneTarget;

	g3dLayer*				m_pLayer;
	const camCamera*		m_pCamera;

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	void DrawNode(const g3dSceneNode* i_pNode, bool i_bLit, g3dLight* i_pLight, const g3dProjectedLight* i_pProjLight);
};
