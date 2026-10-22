/****************************************************************************\
**	shdwPassAOVolumes.hpp
**
**		Ambient occlusion from volumes: view-space normals and positions
**		(shdwPassNormals), then an occlusion volume extruded from each
**		triangle by a geometry shader into a reduced-resolution AO buffer,
**		blended (or blurred and upsampled) over the target
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_SHDW_PASSAOVOLUMES_HPP
#error shdwPassAOVolumes.hpp multiply included
#endif
#define GVK_SHDW_PASSAOVOLUMES_HPP

#ifndef GVK_G3D_RENDERPASS_HPP
#include "GraphicsVulkan/G3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif

class camCamera;

namespace gvk
{

class matRenderTargetTexture;
class shdwPassTraversal;
struct sNodePlusState;

class shdwPassAOVolumes : public g3dRenderPass
{
public:
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassAOVolumes();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassAOVolumes(g2dRenderTarget* i_pFinalTarget, matRenderTargetTexture* i_pAO,
		matRenderTargetTexture* i_pPositions,
		matRenderTargetTexture* i_pNormals,
		matRenderTargetTexture* i_pDepth,
		const camCamera* i_pCamera,
		const ssaoParams& i_Params);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	~shdwPassAOVolumes();

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
	matRenderTargetTexture* m_AOBuffer;
	matRenderTargetTexture* m_DepthBuffer;

	ssaoParams m_Params;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void BlendAO(g2dRenderTarget* i_pRenderTarget, matRenderTargetTexture* i_pAOSrc );

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
	void BlurAO(g2dRenderTarget* i_pRenderTarget);
};

}
