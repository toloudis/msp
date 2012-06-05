/*****************************************************************************
**  demG3dTestVertexColoring.hpp
**
**		This mode displays a demonstration/test of vertex coloring 
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTVERTEXCOLORING_HPP
#error demG3dTestVertexColoring.hpp multiply included
#endif
#define DEM_G3DTESTVERTEXCOLORING_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
#endif

#ifndef AN_KEYANIMATION_HPP
#include "anKeyAnimation.hpp"
#endif

#ifndef G3D_FRAGMENT_HPP
#include "g3dFragment.hpp"
#endif

#ifndef MAT_MATERIAL_HPP
#include "matMaterial.hpp"
#endif

class g3dViewer;
class g3dDirectionalLight;
class g3dScene;
class g3dSceneNode;

class demG3dTestVertexColoring : public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestVertexColoring(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestVertexColoring();

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

		g3dFragment* m_Fragment;
		matMaterial m_Material;

		anKeyAnimation<maFloatRGBA> *m_pColorAnim1, *m_pColorAnim2, *m_pColorAnim3;

		g3dDirectionalLight* m_pLight;
};
