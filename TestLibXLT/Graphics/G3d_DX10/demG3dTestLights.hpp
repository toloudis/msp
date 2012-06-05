/*****************************************************************************
**  demG3dTestLights.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTLIGHTS_HPP
#error demG3dTestLights.hpp multiply included
#endif
#define DEM_G3DTESTLIGHTS_HPP

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
class g3dDirectionalLight;
class g3dPointLight;
class g3dScene;
class g3dSceneNode;

class demG3dTestLights 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestLights(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestLights();

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

	private:
		g3dViewer &m_Viewer;
		g3dScene *m_Scene;
		g3dSceneNode *m_Root;

		g3dFragment* m_RectFragment;
		g3dFragment* m_BigSphereFragment;
		g3dFragment* m_SmallSphereFragment;

		matMaterial m_RectMat;
		matMaterial m_BigSphereMat;
		matMaterial m_SmallSphereMat;

		g3dSceneNode* m_BaseRect;
		g3dSceneNode* m_Sphere;

		g3dSceneNode* m_Light1;
		g3dSceneNode* m_Light2;
		g3dSceneNode* m_Light3;

		matTexture* m_RectTexture;
		matTexture* m_SphereTexture;

		g3dDirectionalLight* m_DirectionalLight;
		g3dPointLight* m_PointLight1;
		g3dPointLight* m_PointLight2;
		g3dPointLight* m_PointLight3;
};
