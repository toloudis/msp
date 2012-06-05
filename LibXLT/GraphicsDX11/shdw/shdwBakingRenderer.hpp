/****************************************************************************\
**	shdwBakingRenderer.hpp
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_BAKINGRENDERER_HPP
#error shdwBakingRenderer.hpp multiply included
#endif
#define SHDW_BAKINGRENDERER_HPP

#ifndef G3D_BAKE_HPP
#include "Graphics/g3d/g3dBake.hpp"
#endif

#ifndef SHDW_PASSTRAVERSAL_HPP
#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"
#endif

#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif

#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif

class g3dRenderStateTraverser;
class shdwPassLPVGI;
struct sNodePlusState;

class shdwBakingRenderer : public g3dBakeImpl
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwBakingRenderer();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~shdwBakingRenderer();

	//--------------------------------------------------------------------
	//	Init scene for baking, return number of nodes to bake
	//--------------------------------------------------------------------
	virtual int Init(const g2dPFD& i_PFD, const g3dScene* i_pScene, const fsLocator& i_OutputPath,
		float i_fSimTime, const camCamera& i_Camera, bool i_bIsSaveAndReplace,
		const std::string& i_OutputFormat, int i_Res, int i_TextureReduce);

	//--------------------------------------------------------------------
	//	CleanUp after baking
	//--------------------------------------------------------------------
	virtual int CleanUp();

	//--------------------------------------------------------------------
	//	Bake a node and return true if there are any left to bake.
	//--------------------------------------------------------------------
	virtual bool BakeNextNode(std::map<const g3dFragment*, std::string> &io_TextureNameMap);

	//--------------------------------------------------------------------
	//	Set the output directory
	//--------------------------------------------------------------------
	virtual void SetOutputDirectory(const fsLocator& i_OutputDir);

	static void InitStates();
	static void CleanupStates();

private:
	shdwPassTraversal m_SceneDatabase;

	//--------------------------------------------------------------------
	//	Bake a node and return num triangles rendered.
	//--------------------------------------------------------------------
	int BakeNonShadowNode(const sNodePlusState& i_Node, 
						  g3dRenderStateTraverser* i_pRenderStateCache,
						  std::map<const g3dFragment*, std::string> &io_TextureNameMap);
	int BakeShadowNode(const sNodePlusState& i_Node, 
					   g3dRenderStateTraverser* i_pRenderStateCache,
					   std::map<const g3dFragment*, std::string> &io_TextureNameMap);
	int BakeNonSolidNode(const sNodePlusState& i_Node, 
						 g3dRenderStateTraverser* i_pRenderStateCache,
						 std::map<const g3dFragment*, std::string> &io_TextureNameMap);

	int m_CurrentNode;
	g2dPFD m_PFD;

	fsLocator m_OutputPathLocator;
	//--------------------------------------------------------------------
	//	Generate baking texture's filename and resolution 
	//--------------------------------------------------------------------
	void GetTextureInfo(const g3dSceneNode* i_pNode, const g3dFragment* i_pFrag, 
		int& o_TexX, int& o_TexY, std::string& o_Name);

	//--------------------------------------------------------------------
	//	Duplicate pixels around uv seams
	//--------------------------------------------------------------------
	void FixupSeams(matTexture* io_pTexture, int i_Width, int i_Height);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void InitMultisample();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CleanupMultisample();

	matRenderTargetTexture* m_pHDRAATarget;
	g3dScene* m_pScene;
	camCamera m_Camera;

	// GI Buffer
	ssgiParams m_giParam;
	shdwPassLPVGI* m_pPassLPVGI;
	matRenderTargetTexture* m_GIScratchBuffer;
	matRenderTargetTexture* m_GIDepthBuffer;

	// AO Buffer
	ssaoParams m_aoParam;
	matRenderTargetTexture* m_AOBuffer;
	matRenderTargetTexture* m_AOPositionBuffer;
	matRenderTargetTexture* m_AONormalBuffer;
	matRenderTargetTexture* m_AODepthBuffer;
};
