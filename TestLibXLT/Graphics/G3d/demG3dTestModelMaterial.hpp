/*****************************************************************************
**  demG3dTestModelMaterial.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTMODELMATERIAL_HPP
#error demG3dTestModelMaterial.hpp multiply included
#endif
#define DEM_G3DTESTMODELMATERIAL_HPP

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
class matPlainTexture;
class g3dWorldStaticModel;

class demG3dTestModelMaterial 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestModelMaterial(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestModelMaterial();

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

		g3dFragment* m_SphereFragment;

		g3dSceneNode* m_Sphere1;
		g3dSceneNode* m_Sphere2;
		g3dSceneNode* m_Sphere3;

		matMaterial m_Mat1;
		matMaterial m_Mat2;
		matMaterial m_Mat3;

		matTexture* m_Texture1;
		matTexture* m_Texture2;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
};
