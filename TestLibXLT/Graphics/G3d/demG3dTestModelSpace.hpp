/*****************************************************************************
**  demG3dTestModelSpace.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTMODELSPACE_HPP
#error demG3dTestModelSpace.hpp multiply included
#endif
#define DEM_G3DTESTMODELSPACE_HPP

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
class g3dScene;
class g3dSceneNode;

class demG3dTestModelSpace 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestModelSpace(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestModelSpace();

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
		g3dSceneNode *m_WorldRoot, *m_CameraRoot, *m_ScreenRoot;

		g3dSceneNode *m_Cube1, *m_Cube2;

		g3dFragment* m_CubeFragment;
		g3dFragment* m_SphereFragment;
		g3dFragment* m_RectFragment;
		g3dFragment* m_TetraFragment;
		g3dFragment* m_ScreenRectFragment;

		matMaterial m_CubeMat;
		matMaterial m_SphereMat;
		matMaterial m_RectMat;

		matTexture* m_CubeTexture;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
};
