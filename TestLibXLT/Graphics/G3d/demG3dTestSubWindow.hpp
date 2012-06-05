/*****************************************************************************
**  demG3dTestSubWindow.hpp
**
**		This mode displays a demonstration/test of prelit textured and 
**		non-textured fragments 
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTSUBWINDOW_HPP
#error demG3dTestSubWindow.hpp multiply included
#endif
#define DEM_G3DTESTSUBWINDOW_HPP

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
class scObject;
class g3dDirectionalLight;
class g3dScene;
class g3dSceneNode;
class g2dSystem;
class g2dWindow;

class demG3dTestSubWindow : public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestSubWindow(g2dSystem &i_System, g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestSubWindow();

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
		g2dSystem &m_System;

		// Second window classes
		g2dWindow* m_pSubWindow;
		g3dViewer* m_pSubViewer;

		std::vector<g3dFragment*> m_Fragments;
		std::vector<scObject*> m_Objects;
		std::vector<matMaterial*> m_Materials;
		std::vector<matTexture*> m_Textures;

		g3dDirectionalLight* m_pDirLight;
};
