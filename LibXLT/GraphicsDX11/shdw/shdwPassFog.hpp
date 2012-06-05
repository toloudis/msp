#ifdef SHDW_PASSFOG_HPP
#error shdwPassFog.hpp multiply included
#endif
#define SHDW_PASSFOG_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif 

class camCamera;
class g3dScene;
class matRenderTargetTexture;
class shdwPassTraversal;
struct fogParams;

class shdwPassFog : public g3dRenderPass
{
public:
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassFog();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassFog(g2dRenderTarget* io_ColorBuffer,
		matRenderTargetTexture* i_DepthBuffer,
		matRenderTargetTexture* i_TempBuffer,
		const g3dScene* i_Scene,
		const camCamera* i_Camera
		);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	~shdwPassFog();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	void SetBuffers(g2dRenderTarget* io_ColorBuffer,
		matRenderTargetTexture* i_DepthBuffer,
		matRenderTargetTexture* i_TempBuffer
		);

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
	const camCamera* m_Camera;
	const g3dScene* m_Scene;
	g2dRenderTarget* m_ColorBuffer;
	matRenderTargetTexture* m_DepthBuffer;
	matRenderTargetTexture* m_TempBuffer;

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	int DrawNodeFog(const g3dSceneNode* i_pNode);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	void GetFogOrientation(fogParams& io_FogParams);
};
