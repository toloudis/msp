/*****************************************************************************
**  demG3dTestHDRAmbientEnvMap.hpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTHDRAMBIENTENVMAP_HPP
#error demG3dTestHDRAmbientEnvMap.hpp multiply included
#endif
#define DEM_G3DTESTHDRAMBIENTENVMAP_HPP

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
struct g3dAmbientEnvState;
struct g3dRenderState;
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
class shdwHDRRendererDX9;
class entModelTemplate;
class scObject;

#define NMODELS 9
class demG3dTestHDRAmbientEnvMap 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestHDRAmbientEnvMap(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestHDRAmbientEnvMap();

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
		g3dSceneNode *m_Root;
		shdwHDRRendererDX9* m_pRenderer;
		g3dSceneRenderer* m_oldRenderer;

		g3dFragment* m_RectFragment;
		matTexture* m_Texture1;
		matMaterial m_TexMat;

		matTexture* m_AmbEnvMap;
		g3dRenderState* m_pStateRoot;
		g3dAmbientEnvState* m_pEnvRoot;

		g3dSceneNode* m_WorldRect;

		entModelTemplate* m_entModels[NMODELS];
		scObject* m_scObjs[NMODELS];

		g3dFragment* m_SmallSphereFragment;
		matMaterial m_SmallSphereMat;
		g3dSceneNode* m_Light1;
		g3dSceneNode* m_Light2;
		g3dPointLight* m_PointLight1;
		g3dPointLight* m_PointLight2;
		
};
