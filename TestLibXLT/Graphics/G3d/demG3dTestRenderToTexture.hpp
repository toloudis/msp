/*****************************************************************************
**  demG3dTestRenderToTexture.hpp
**
**		This mode displays a demonstration/test of rendering to a texture.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTRENDERTOTEXTURE_HPP
#error demG3dTestRenderToTexture.hpp multiply included
#endif
#define DEM_G3DTESTRENDERTOTEXTURE_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
#endif

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif

#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif

class g3dViewer;
class g3dScene;
class g3dSceneNode;
class g3dDirectionalLight;
class g3dTargetRenderer;
class matPlainTexture;
class matRenderTargetTexture;
class shdwShadowLayerRendererDX9;

class demG3dTestRenderToTexture 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestRenderToTexture(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestRenderToTexture();

		//====================================================================
		//	Think
		//====================================================================
		virtual void Think();

		//====================================================================
		//====================================================================
		virtual void Initialize();

		//====================================================================
		//====================================================================
		virtual void DeInitialize();

		//====================================================================
		//	Override this function to get appCharEvents.
		//====================================================================
		virtual void ReceiveCharEvent(appCharEvent& i_Event);

	private:
		
		g3dViewer &m_Viewer;
		g3dScene *m_Scene;
		g3dSceneNode *m_WorldRoot;
		g3dSceneNode *m_ScreenRoot;

		g3dFragment* m_CubeFragment;
		g3dFragment* m_RectFragment;
		g3dFragment* m_ZoomFragment;

		g3dSceneNode* m_WorldRect;
		g3dSceneNode* m_ZoomRect;

		std::vector<g3dSceneNode*> m_Cubes;

		matMaterial m_CubeMat;
		matMaterial m_RectMat;
		matMaterial m_ZoomMat;

		matTexture* m_CubeTexture;
		matTexture* m_pMaskTexture;
		matTexture* m_pZoomTexture;

		g3dTargetRenderer* m_pZoomRenderer;
		shdwShadowLayerRendererDX9* m_pRenderer;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
};
