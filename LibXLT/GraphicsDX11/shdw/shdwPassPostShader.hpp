/****************************************************************************\
**	shdwPassPostShader.hpp
**
**	Post processing with an user selected shader
**
**  Riva Chang
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef SHDW_PASSPOSTSHADER_HPP
#error shdwPassPostShader.hpp multiply included
#endif
#define SHDW_PASSPOSTSHADER_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

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
