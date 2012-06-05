/*****************************************************************************
**  demG3dTestVertexShader.hpp
**
**		This mode displays a demonstration/test of vertex shaders
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTVERTEXSHADER_HPP
#error demG3dTestVertexShader.hpp multiply included
#endif
#define DEM_G3DTESTVERTEXSHADER_HPP

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
class g3dPointLight;
class g3dWorldStaticModel;

class demG3dTestVertexShader 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestVertexShader(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestVertexShader();

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

		g3dFragment* m_SphereFragment;
		g3dFragment* m_SmallSphereFragment;
		g3dFragment* m_ShipFragment;

		g3dSceneNode* m_Sphere1;
		g3dSceneNode* m_Sphere2;
		g3dSceneNode* m_Sphere3;
		g3dSceneNode* m_LightBall1;
		g3dSceneNode* m_LightBall2;

		matMaterial m_ShaderMats[5];
		matMaterial m_Mat2;
		matMaterial m_Mat3;

		matTexture* m_Texture1;
		matTexture* m_Texture2;
		matTexture* m_Texture3;
		matTexture* m_Texture4;
		matTexture* m_Texture5;
		matStaticCubeTexture* m_Texture6;
		matTexture* m_Texture7;
		matTexture* m_Texture8;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
		g3dPointLight* m_Light3;
		g3dPointLight* m_Light4;

		std::vector<matMaterial*> m_Materials;
		std::vector<matTexture*> m_Textures;
};
