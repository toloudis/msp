/*****************************************************************************
**  demG3dTestTextureReduce.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTTEXTUREREDUCE_HPP
#error demG3dTestTextureReduce.hpp multiply included
#endif
#define DEM_G3DTESTTEXTUREREDUCE_HPP

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
class matPlainTexture;
class g3dWorldStaticModel;

class demG3dTestTextureReduce 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestTextureReduce(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestTextureReduce();

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

		g3dFragment* m_Rect1Fragment;
		g3dFragment* m_Rect2Fragment;
		g3dFragment* m_Rect3Fragment;

		g3dSceneNode* m_Rect1Model;
		g3dSceneNode* m_Rect2Model;
		g3dSceneNode* m_Rect3Model;

		matMaterial m_Rect1Mat;
		matMaterial m_Rect2Mat;
		matMaterial m_Rect3Mat;

		matTexture* m_Texture1;
		matTexture* m_Texture2;
		matTexture* m_Texture3;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
};
