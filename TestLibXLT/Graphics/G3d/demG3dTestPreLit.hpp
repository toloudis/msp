/*****************************************************************************
**  demG3dTestPreLit.hpp
**
**		This mode displays a demonstration/test of prelit textured and 
**		non-textured fragments 
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTPRELIT_HPP
#error demG3dTestPreLit.hpp multiply included
#endif
#define DEM_G3DTESTPRELIT_HPP

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

class demG3dTestPreLit : public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestPreLit(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestPreLit();

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

		g3dFragment* m_Fragment1;
		g3dFragment* m_Fragment2;
		matMaterial m_Material1;
		matMaterial m_Material2;

		matTexture* m_Texture;

		g3dDirectionalLight* m_pLight;
};
