/*****************************************************************************
**  demG3dTestTextures.hpp
**
**		This mode displays a demonstration/test of the different texturing
**	options available in the Terawatt 3D engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTTEXTURES_HPP
#error demG3dTestTextures.hpp multiply included
#endif
#define DEM_G3DTESTTEXTURES_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
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
class matPlainTexture;
class matUVATexture;

class demG3dTestTextures 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestTextures(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestTextures();

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

		g2dFontHandle m_TextureFont;

		g3dFragment* m_RectFragment;
		g3dFragment* m_CubeFragment;
		g3dFragment* m_TextBlockFragment;
		g3dFragment* m_UVABlockFragment;

		matMaterial m_GroundMat;
		matMaterial m_CubeMat1;
		matMaterial m_CubeMat2;
		matMaterial m_TextBlockMat;
		matMaterial m_UVABlockMat;

		g3dSceneNode* m_Ground;

		std::vector<g3dSceneNode*> m_Cubes;
		
		g3dSceneNode* m_TextBlock;
		g3dSceneNode* m_UVABlock;

		matTexture* m_GroundBaseTexture;
		matTexture* m_GroundDetailTexture;
		matTexture* m_CubeTexture1;
		matTexture* m_CubeTexture2;
		matTexture* m_TextBlockTexture;
		matUVATexture* m_UVABlockTexture;
		std::vector<matTexture*> m_UVABlockTextures;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
};
