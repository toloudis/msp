/****************************************************************************\
**	shdwMaterialsRendererVk.hpp
**
**	The shdwMaterialsRendererVk is a rendering algorithm that uses a floating
**  point frame buffer and tone mapping to allow high dynamic range lighting.
**
**	GraphicsVulkan port of GraphicsDX11's shdwMaterialsRendererDX11.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_SHDW_MATERIALSRENDERERVK_HPP
#error shdwMaterialsRendererVk.hpp multiply included
#endif
#define GVK_SHDW_MATERIALSRENDERERVK_HPP

#ifndef GVK_SHDW_SCENERENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwSceneRendererVk.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif



#include <map>

class effSolidData;
class maVector4d;

namespace gvk
{

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class matRenderTargetTexture;
class g2dWindowVk;

class shdwMaterialsRendererVk : public shdwSceneRendererVk
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwMaterialsRendererVk();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwMaterialsRendererVk();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
		const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ReleaseResources();

	//--------------------------------------------------------------------
	//	Sometimes we need to access the pre-buffer pixels that don't get
	//	drawn to a window.
	//--------------------------------------------------------------------
	virtual g2dRenderTarget* GetRawBuffer();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual g2dPFD::PixelFormat GetRawBufferPFD();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float GenerateNextNumSequential(float i_min, float i_max, float i_LastNum);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float GenerateNextNumAlternating(float i_min, float i_max, float i_LastNum);

	static void InitStates();
	static void CleanupStates();

protected:
	// lazy evaluate whether to recreate temp surfaces
	// based on whether width/height of window have changed.
	int m_Width, m_Height;

	g2dWindowVk* m_pWindow;

	void CreateSurfaces(g2dRenderTarget* i_pWindow);

	shdwPassTraversal m_SceneDatabase;

	matMaterial* m_pMaterialsMat;
	effSolidData* m_pMaterialsData;

	void DrawNode(const g3dSceneNode* i_pNode);
	void CopyToBackBuf(matRenderTargetTexture* pTex);

	std::map<std::string, float> m_ColorMap;

	std::vector<std::string>* m_SceneMaterials;

	float m_LastNumGenerated;
	int m_TotalColors;
};

}
