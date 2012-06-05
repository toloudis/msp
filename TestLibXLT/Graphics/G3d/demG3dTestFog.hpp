/*****************************************************************************
**  demG3dTestFog.hpp
**
**		This mode displays a demonstration/test of the fogging function of
**	the Terawatt engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTFOG_HPP
#error demG3dTestFog.hpp multiply included
#endif
#define DEM_G3DTESTFOG_HPP

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

class demG3dTestFog 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestFog(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestFog();

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
		g3dFragment* m_RectFragment;

		g3dSceneNode* m_WorldRect;

		std::vector<g3dSceneNode*> m_Cubes;

		matMaterial m_CubeMat;
		matMaterial m_RectMat;

		matTexture* m_CubeTexture;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
};
