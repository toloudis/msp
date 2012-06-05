/*****************************************************************************
**  demG3dTestTextureCompression.hpp
**
**		This mode displays a demonstration/test of the different texturing
**	options available in the Terawatt 3D engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTTEXTURECOMPRESSION_HPP
#error demG3dTestTextureCompression.hpp multiply included
#endif
#define DEM_G3DTESTTEXTURECOMPRESSION_HPP

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
class g3dPointLight;
class matPlainTexture;
class matUVATexture;

class demG3dTestTextureCompression 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestTextureCompression(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestTextureCompression();

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

		// We display two textured rectangles, one with a non-compressed texture
		// the other with a compressed or DDS texture
		g3dFragment*	m_RectFragment;
		matMaterial			m_RectMat;
		g3dSceneNode*		m_Rect;
		matTexture*			m_RectTexture;

		g3dFragment*	m_DDSRectFragment;
		matMaterial			m_DDSRectMat;
		g3dSceneNode*		m_DDSRect;
		matTexture*			m_DDSRectTexture;


		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
};
