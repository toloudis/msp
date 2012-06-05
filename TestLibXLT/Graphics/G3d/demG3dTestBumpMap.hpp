/*****************************************************************************
**  demG3dTestBumpMap.hpp
**
**	This mode displays a demonstration/test of texture bump mapping.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTBUMPMAP_HPP
#error demG3dTestBumpMap.hpp multiply included
#endif
#define DEM_G3DTESTBUMPMAP_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
#endif

#ifndef G3D_FRAGMENT_HPP
#include "g3dFragment.hpp"
#endif

#ifndef MAT_MATERIAL_HPP
#include "matMaterial.hpp"
#endif

class g3dViewer;
class g3dScene;
class g3dSceneNode;
class g3dDirectionalLight;
class g3dPointLight;
class matPlainTexture;
class g3dWorldStaticModel;
class matStaticCubeTexture;

class demG3dTestBumpMap 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestBumpMap(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestBumpMap();

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

		g3dFragment* m_CubeFragment;
		g3dFragment* m_SphereFragment;
		g3dFragment* m_RectFragment;

		g3dSceneNode* m_CubeModel;
		g3dSceneNode* m_SphereModel;
		g3dSceneNode* m_RectModel;

		matMaterial m_CubeMat;
		matMaterial m_SphereMat;
		matMaterial m_RectMat;

		matTexture* m_BumpTexture;
		matTexture* m_BumpReflTexture;
		matTexture* m_RectTexture;
		matStaticCubeTexture* m_CubeTexture;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
};
