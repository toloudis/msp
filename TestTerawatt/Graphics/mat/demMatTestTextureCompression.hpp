/*****************************************************************************
**  demMatTestTextureCompression.hpp
**
**		This mode displays a demonstration/test of the different texturing
**	options available in the Terawatt 3D engine.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_mattestTEXTURECOMPRESSION_HPP
#error demMatTestTextureCompression.hpp multiply included
#endif
#define DEM_mattestTEXTURECOMPRESSION_HPP

#ifndef DEM_MATTESTMODE_HPP
#include "demMatTestMode.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "fsLocator.hpp"
#endif

#ifndef G3D_FRAGMENT_HPP
#include "g3dFragment.hpp"
#endif

#ifndef MAT_MATERIAL_HPP
#include "matMaterial.hpp"
#endif

#include <vector>


//============================================================================
//	forward references
//============================================================================
class g3dViewer;
class g3dScene;
class g3dSceneNode;
class g3dDirectionalLight;
class g3dPointLight;
class matPlainTexture;
class matUVATexture;


//============================================================================
//============================================================================
class demMatTestTextureCompression
:	public demMatTestMode
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		demMatTestTextureCompression(g3dViewer &i_Viewer);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~demMatTestTextureCompression();

		//--------------------------------------------------------------------
		//	Think
		//--------------------------------------------------------------------
		virtual void Think();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Initialize();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void DeInitialize();

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetUpTexture( int i_Index, fsLocator& i_Loc );

	private:
		g3dViewer &m_Viewer;
		g3dScene *m_Scene;
		g3dSceneNode *m_Root;

		g2dFontHandle m_TextureFont;

		g3dDirectionalLight* m_pLight1;
		g3dDirectionalLight* m_pLight2;

		// We display two textured rectangles, one with a non-compressed texture
		// the other with a compressed or DDS texture
		struct TextureData
		{
			g3dFragment*		m_pFragment;
			matMaterial			m_Material;
			g3dSceneNode*		m_pSceneNode;
			matTexture*			m_pTexture;
		};

		std::vector<TextureData>	m_TData;
};
