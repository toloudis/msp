/****************************************************************************\
**	shdwPassLPVGI.hpp
**
**  This pass generates Global Illumination from using light propogation
**  volume technique
**
**	GraphicsVulkan port of GraphicsDX11's: the radiance volumes are 3D
**	images rendered slice by slice through their layered views
**	(vkdImage::LayeredView), the VPL and volume points a vertex buffer
**	with a vkdVertexLayout.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_SHDW_PASSLPVGI_HPP
#error shdwPassLPVGI.hpp multiply included
#endif
#define GVK_SHDW_PASSLPVGI_HPP

#ifndef GVK_G3D_RENDERPASS_HPP
#include "GraphicsVulkan/G3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class camCamera;
class effMaskAlphaData;
class g3dLight;
class g3dProjectedLight;
class g3dScene;
class g3dSceneRenderer;
class g3dTargetRenderer;

namespace gvk
{

class g3dVkBuffer;
class matRenderTargetTexture;

class shdwPassLPVGI : public g3dRenderPass
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwPassLPVGI(const ssgiParams& i_SSGIParams,
				matRenderTargetTexture* i_Scratch,
				matRenderTargetTexture* i_Depth,
				g2dRenderTarget* i_Target,
				const camCamera* i_pCamera,
				const g3dScene* i_pScene,
				bool i_bIsDeferRender = false);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwPassLPVGI();

	//--------------------------------------------------------------------
	// free any locally allocated shared (re-entrant?) resources
	//--------------------------------------------------------------------
	static void InitStates();
	static void CleanupStates();

	void ReleaseResources();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	virtual int Render(float i_time);
	void DeferRenderForBaking(const sNodePlusState& i_Node);

	void SetSceneInfo(shdwPassTraversal* i_Traversal);

protected:
	shdwPassTraversal* m_sceneInfo;

	void SetupGIParam();
	void BlurGI(g2dRenderTarget* i_pRenderTarget);
	void BlendGI(g2dRenderTarget* i_pRenderTarget);
	void RenderNodes();
	static int RenderNode(const sNodePlusState& i_Node);

	void RenderRSMPerLight(const g3dProjectedLight* i_pProjLight);
	void DownSampleRSM();
	void InjectRSM(const g3dSceneNode* i_rootNode, const g3dProjectedLight* i_pProjLight, int i_totalVPLNum, float i_maxLightScale);
	void LightPropogation(int i_NumIteration, const g3dSceneNode* i_rootNode);
	void ApplyLPV(const g3dSceneNode* i_rootNode);
	void ApplyLPVOneNode(const g3dSceneNode* i_rootNode, const sNodePlusState& i_Node);

private:
	static void drawVolumeVertex(g3dVkBuffer* i_buffer, int i_numVertex);

	const camCamera*		m_pCamera;
	const g3dScene*			m_pScene;

	ssgiParams m_giParam;

	matRenderTargetTexture* m_scratchTex;
	matRenderTargetTexture* m_depthTex;

	g3dSceneRenderer* m_pRSMRenderer;
	g3dTargetRenderer* m_pRSMTargetRenderer;
	bool m_bSwapedBuffer;
	bool m_bDeferRender;
};

}
