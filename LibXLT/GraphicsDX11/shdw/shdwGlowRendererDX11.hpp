/****************************************************************************\
**	shdwGlowRendererDX11.hpp
**
**	The shdwGlowRendererDX11 is a rendering algorithm that uses a floating 
**  point frame buffer and tone mapping to allow high dynamic range lighting.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_GLOWRENDERERDX11_HPP
#error shdwGlowRendererDX11.hpp multiply included
#endif
#define SHDW_GLOWRENDERERDX11_HPP

#ifndef SHDW_SCENERENDERERDX11_HPP
#include "GraphicsDX11/shdw/shdwSceneRendererDX11.hpp" 
#endif

#ifndef SHDW_PASSTRAVERSAL_HPP
#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"
#endif

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

//#include c_g2dD3DX11EFFECT_H

#include <map>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class matRenderTargetTexture;
class effSolidData;
class maVector4d;
class g2dWindowDX11;

class shdwGlowRendererDX11 : public shdwSceneRendererDX11
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwGlowRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwGlowRendererDX11();

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

	g2dWindowDX11* m_pWindow;

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
