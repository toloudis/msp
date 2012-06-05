/*****************************************************************************
**  demG3dTestAnims.hpp
**
**		This mode displays a demonstration/test of some of the material
**	animation capabilities of the Terawatt G3d package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTANIMS_HPP
#error demG3dTestAnims.hpp multiply included
#endif
#define DEM_G3DTESTANIMS_HPP

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
class g3dPointLight;
class matTexture;
class matMatAnim;

class demG3dTestAnims 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestAnims(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestAnims();

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

		void setup_anims();

		g3dFragment* m_RectFragment;
		g3dFragment* m_SphereFragment;
		g3dFragment* m_TetraFragment;

		matMaterial m_RectMat;
		matMaterial m_SphereMat;
		matMaterial m_TetraMat;

		g3dSceneNode* m_Rect;
		g3dSceneNode* m_Sphere;
		g3dSceneNode* m_Tetra;

		matTexture* m_RectTexture1;
		matTexture* m_RectTexture2;
		matTexture* m_SphereTexture;
		matTexture* m_TetraTexture;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;

		matMatAnim* m_RotateAnim;
		matMatAnim* m_ColorAnim;
		matMatAnim* m_TexturePosAnim1;
		matMatAnim* m_TexturePosAnim2;
};
