/*****************************************************************************
**  demG3dTestSpriteGroup.hpp
**
**		This mode displays a demonstration/test of the g3dSpriteGroupModel.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTSPRITEGROUP_HPP
#error demG3dTestSpriteGroup.hpp multiply included
#endif
#define DEM_G3DTESTSPRITEGROUP_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "g3dFragment.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "matMaterial.hpp"
#endif
#ifndef G3D_SPRITEDATA_HPP
#include "g3dSpriteData.hpp"
#endif

class g3dSpriteGroupModel;
class g3dViewer;
class g3dScene;
class g3dSceneNode;
class g3dDirectionalLight;
class matPlainTexture;
class matUVATexture;

class demG3dTestSpriteGroup 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestSpriteGroup(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestSpriteGroup();

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

		g3dSpriteGroupModel* m_Model1;
		g3dSpriteGroupModel* m_Model2;
		g3dSceneNode* m_WorldRect;

		std::vector<g3dSpriteData> m_Sprites1;
		std::vector<g3dSpriteData> m_Sprites2;

		matMaterial m_Mat1;
		matMaterial m_Mat2;

		matTexture* m_PlainTexture;
		matUVATexture* m_UVATexture;
		std::vector<matTexture*> m_UVABlockTextures;

		matMaterial m_RectMat;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
};
