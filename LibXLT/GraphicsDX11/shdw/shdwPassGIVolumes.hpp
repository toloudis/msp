/****************************************************************************\
**	shdwPassGIVolumes.hpp
**
**		Render pass to draw normal vectors x,y,z to target r,g,b
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef SHDW_PASSGIVOLUMES_HPP
#error shdwPassGIVolumes.hpp multiply included
#endif
#define SHDW_PASSGIVOLUMES_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif 
#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif 

class shdwPassTraversal;
struct sNodePlusState;

class shdwPassGIVolumes : public g3dRenderPass
{
public:
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassGIVolumes();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassGIVolumes(g2dRenderTarget* i_pFinalTarget, matRenderTargetTexture* i_pGI,
		matRenderTargetTexture* i_pPositions,
		matRenderTargetTexture* i_pNormals, 
		matRenderTargetTexture* i_pDepth, 
		const camCamera* i_pCamera,
		const ssgiParams& i_Params,
		bool i_bReuseBuff = false,
		matRenderTargetTexture* i_pAOBackup = NULL);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	~shdwPassGIVolumes();

	//----------------------------------------------------------------------------------------
	// This function initializes the globle materials used in RenderNode func
	//----------------------------------------------------------------------------------------
	static void InitializeMaterial();	

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	virtual int Render(float i_time);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	void SetSceneInfo(shdwPassTraversal* i_Traversal) {m_SceneInfo = i_Traversal;}

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	int RenderNode(const sNodePlusState& i_Node);

	//----------------------------------------------------------------------------------------
	// This function renders a single node with the special hair shader
	//----------------------------------------------------------------------------------------
	int RenderHairNode(const sNodePlusState& i_Node);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	static void InitStates();
	static void CleanupStates();

protected:
	shdwPassTraversal* m_SceneInfo;
	const camCamera* m_pCamera;

	matRenderTargetTexture* m_PositionBuffer;
	matRenderTargetTexture* m_NormalBuffer;
	matRenderTargetTexture* m_GIBuffer;
	matRenderTargetTexture* m_DepthBuffer;
	matRenderTargetTexture* m_AOBuffer;

	ssgiParams m_Params;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void BlurGI(g2dRenderTarget* i_pRenderTarget);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void BlendGI(g2dRenderTarget* i_pRenderTarget, 
				matRenderTargetTexture* i_pGISrc, 
				matRenderTargetTexture* i_pAOSrc);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DrawStencilPass(g2dRenderTarget* i_pTarget);

	//--------------------------------------------------------------------
	// clear the mask.
	//--------------------------------------------------------------------
	void ClearStencil(g2dRenderTarget* i_pTarget);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int RenderNodeStencil(const sNodePlusState& i_Node);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetupNodeColor(const sNodePlusState& i_Node, matShaderEffect* i_pEffect);

private: 
	bool m_bReuseBuff;
};

