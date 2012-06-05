/****************************************************************************\
**	shdwPassOutline.hpp
**
**	Render outlined objects into a buffer.
**
**  John Schwab
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef SHDW_PASSOUTLINE_HPP
#error shdwPassOutline.hpp multiply included
#endif
#define SHDW_PASSOUTLINE_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

class g3dLight;
class g3dProjectedLight;
class shdwPassTraversal;

class shdwPassOutline : public g3dRenderPass
{
public:
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassOutline();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassOutline( matRenderTargetTexture* i_outlineTarget,  matRenderTargetTexture* i_AATarget, g2dRenderTarget* i_destination, shdwPassTraversal* i_SceneInfo );

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	~shdwPassOutline();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	void SetBuffers( matRenderTargetTexture* i_outlineTarget,  matRenderTargetTexture* i_AATarget, g2dRenderTarget* i_destination );

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

	// Render object to this texture, add in each object to get final outlines:
	//  This will pass through a Sobel Filter and is blended with the AAtarget.
	matRenderTargetTexture* m_outlineTarget;
	//  Last the AAtarget is passed through an Anit-Aliasing Edge filter and blened with the render tager
	matRenderTargetTexture* m_AATarget;

	matMaterial*		m_pDepthMat;
	matMaterial*		m_pNormalMat;

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	void DrawNode(const g3dSceneNode* i_pNode, bool i_bLit, g3dLight* i_pLight, const g3dProjectedLight* i_pProjLight);
};
