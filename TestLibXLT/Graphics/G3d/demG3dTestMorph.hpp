/*****************************************************************************
**  demG3dTestMorph.hpp
**
**		This mode displays a demonstration/test of the fragment morphing
**	functionality of the Terawatt G3d package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTMORPH_HPP
#error demG3dTestMorph.hpp multiply included
#endif
#define DEM_G3DTESTMORPH_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
#endif

#ifndef MA_POINT3D_HPP
#include "maPoint3d.hpp"
#endif

#ifndef G3D_FRAGMENT_HPP
#include "g3dFragment.hpp"
#endif

#ifndef MAT_MATERIAL_HPP
#include "matMaterial.hpp"
#endif

#include <vector>

class g3dViewer;
class g3dScene;
class g3dSceneNode;
class g3dDirectionalLight;
class g3dPointLight;
class matPlainTexture;
class matMatAnim;

class demG3dTestMorph 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestMorph(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestMorph();

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

		matMaterial m_RectMat;

		g3dSceneNode* m_Rect;

		matTexture* m_RectTexture1;
		matTexture* m_RectTexture2;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;

		matMatAnim* m_TexturePosAnim1;
		matMatAnim* m_TexturePosAnim2;

		std::vector<maPoint3d> m_Vertices;
		std::vector<maPoint3d> m_Normals;
};
