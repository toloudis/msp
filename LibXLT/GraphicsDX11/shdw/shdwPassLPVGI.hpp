/****************************************************************************\
**	shdwPassLPVGI.hpp
**
**  This pass generates Global Illumination from using light propogation 
**  volume technique
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_PASSLPVGI_HPP
#error shdwPassLPVGI.hpp multiply included
#endif
#define SHDW_PASSLPVGI_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif

#ifndef SHDW_PASSTRAVERSAL_HPP
#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class matRenderTargetTexture;
class effMaskAlphaData;
class g2dRenderTargetDX11;
class g3dLight;
class g3dProjectedLight;
class g3dScene;
class g3dSceneRenderer;
class g3dTargetRenderer;

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
	//shdwPassRSMGI(const ssgiParams& i_SSGIParams);

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
	static void drawVolumeVertex(ID3D11Buffer* i_buffer, int i_numVertex);

	const camCamera*		m_pCamera;
	const g3dScene*			m_pScene;
	//const g3dScene			m_pScene;

	ssgiParams m_giParam;

	matRenderTargetTexture* m_scratchTex;
	matRenderTargetTexture* m_depthTex;

	//matTexture* m_pReflectiveMap;
	//matTexture* m_pDownSampledRSM;
	g3dSceneRenderer* m_pRSMRenderer;
	g3dTargetRenderer* m_pRSMTargetRenderer;
	bool m_bSwapedBuffer;
	bool m_bDeferRender;
};
