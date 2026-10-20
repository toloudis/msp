/****************************************************************************\
**	shdwPassHair.hpp
**
**	Render hair.
**
**	GraphicsVulkan stand-in for GraphicsDX11's shdwPassHair until hair is
**	ported (Plan 7): DX11's constructors and Render calls; it draws
**	nothing and logs GVK_NOT_YET: shdwPassHair once.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_SHDW_PASSHAIR_HPP
#error shdwPassHair.hpp multiply included
#endif
#define GVK_SHDW_PASSHAIR_HPP

#ifndef GVK_G3D_RENDERPASS_HPP
#include "GraphicsVulkan/G3d/g3dRenderPass.hpp"
#endif

class camCamera;
class g3dLayer;

namespace gvk
{

class matRenderTargetTexture;
class shdwPassTraversal;

class shdwPassHair : public g3dRenderPass
{
public:
	shdwPassHair();

	shdwPassHair(
		const camCamera* i_pCamera,
		matRenderTargetTexture* (&i_Targets)[4],
		g2dRenderTarget* i_destination,
		g2dRenderTarget* i_ZBuffer,
		matRenderTargetTexture* i_Depth,
		matRenderTargetTexture* i_depthTarget1,
		matRenderTargetTexture* i_depthTarget2,
		matRenderTargetTexture* i_depthAux,
		matRenderTargetTexture* i_ScratchTarget1,
		matRenderTargetTexture* i_ScratchTarget2
	);

	~shdwPassHair();

	virtual int Render(float i_time);

	int RenderDepthPeeled(float i_time );

	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}

	static void InitStates();
	static void CleanupStates();

protected:
	shdwPassTraversal*		m_sceneInfo;
};

}
