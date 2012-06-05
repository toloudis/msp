/*****************************************************************************
**  demG3dTestProjectedLights.hpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTPROJECTEDLIGHTS_HPP
#error demG3dTestProjectedLights.hpp multiply included
#endif
#define DEM_G3DTESTPROJECTEDLIGHTS_HPP

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
class matStaticCubeTexture;
class g3dDirectionalLight;
class matPlainTexture;
class matShaderEffect;
class g3dPointLight;
class g3dProjectedLight;
class g3dTargetRenderer;
class g3dSceneRenderer;

class demG3dTestProjectedLights 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestProjectedLights(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestProjectedLights();

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
		g3dScene *m_Scene2;
		g3dSceneNode *m_Root;
		g3dSceneNode *m_Root2;

		g3dFragment* m_ShipFragment;
		g3dFragment* m_Ship2Fragment;
		g3dFragment* m_RectFragment;

		g3dSceneNode* m_Ship;
		g3dSceneNode* m_Ship2;
		g3dSceneNode* m_WorldRect;

		matMaterial m_ShaderMat;
		matMaterial m_TexMat;
		matShaderEffect *m_pEffect;
		matShaderEffect *m_pDepthEffect;

		g3dProjectedLight *m_pProjLight;
		camCamera m_ShadowCamera;
		matTexture* m_Texture1;
		matTexture* m_ShadowMap;
		maMatrix4x4 m_ProjMatrix;
		maPoint3d m_LightPos;
		bool m_FollowCamera;

		g3dTargetRenderer* m_pMapRenderer;
		g3dSceneRenderer* m_pDepthRenderer;

		std::vector<matMaterial*> m_Materials;
		std::vector<matTexture*> m_Textures;
};
