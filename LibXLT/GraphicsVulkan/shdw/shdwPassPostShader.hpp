/****************************************************************************\
**	shdwPassPostShader.hpp
**
**	Post processing with an user selected shader
**
**  Riva Chang
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_SHDW_PASSPOSTSHADER_HPP
#error shdwPassPostShader.hpp multiply included
#endif
#define GVK_SHDW_PASSPOSTSHADER_HPP

#ifndef GVK_G3D_RENDERPASS_HPP
#include "GraphicsVulkan/G3d/g3dRenderPass.hpp"
#endif

class g3dSceneNode;

namespace gvk
{

class matRenderTargetTexture;
class shdwPassTraversal;

class shdwPassPostShader : public g3dRenderPass
{
public:
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassPostShader(matRenderTargetTexture* i_ScratchTex, matRenderTargetTexture* i_destination);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	~shdwPassPostShader();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	virtual int Render(float i_time);

	static void InitStates();
	static void CleanupStates();

protected:
	matRenderTargetTexture* m_pDestTex;
	matRenderTargetTexture* m_pScratchTex;

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	void DrawNode(const g3dSceneNode* i_pNode);
};

}
