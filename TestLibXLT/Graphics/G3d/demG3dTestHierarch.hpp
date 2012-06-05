/*****************************************************************************
**  demG3dTestHierarch.hpp
**
**		This mode displays a demonstration/test of hierarchical models in the
**	Terawatt G3D engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTHIERARCH_HPP
#error demG3dTestHierarch.hpp multiply included
#endif
#define DEM_G3DTESTHIERARCH_HPP

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

class demG3dTestHierarch 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestHierarch(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestHierarch();

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

		g3dFragment* m_BaseFragment;
		g3dFragment* m_Arm1Fragment;
		g3dFragment* m_Arm2Fragment;

		g3dSceneNode *m_Model;

		matMaterial m_Mat;

		matTexture* m_Texture;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
};
