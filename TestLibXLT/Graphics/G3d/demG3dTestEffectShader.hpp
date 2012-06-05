/*****************************************************************************
**  demG3dTestEffectShader.hpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTEFFECTSHADER_HPP
#error demG3dTestEffectShader.hpp multiply included
#endif
#define DEM_G3DTESTEFFECTSHADER_HPP

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
class matStaticCubeTexture;
class g3dDirectionalLight;
class matPlainTexture;
class matShaderEffect;
class g3dPointLight;

class demG3dTestEffectShader 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestEffectShader(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestEffectShader();

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

		//====================================================================
		//	Override this function to get appCharEvents.
		//====================================================================
		virtual void ReceiveCharEvent(appCharEvent& i_Event);

	private:
		g3dViewer &m_Viewer;
		g3dScene *m_Scene;
		g3dSceneNode *m_Root;

		g3dFragment* m_ShipFragment;

		g3dSceneNode* m_Ship;

		matMaterial m_ShaderMat;
		matShaderEffect *m_pEffect;

//		matStaticCubeTexture* m_Texture1;
//		matTexture* m_Texture2;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
		g3dPointLight* m_Light3;
		g3dPointLight* m_Light4;

		std::vector<matMaterial*> m_Materials;
		std::vector<matTexture*> m_Textures;
};
