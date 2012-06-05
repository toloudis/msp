/*****************************************************************************
**  demG3dTestHairShader.hpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTHAIRSHADER_HPP
#error demG3dTestHairShader.hpp multiply included
#endif
#define DEM_G3DTESTHAIRSHADER_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
#endif

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif

#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif

class effHairData;
class entModelTemplate;
class g3dPointLight;
class g3dProjectedLight;
struct g3dRenderState;
class g3dScene;
class g3dSceneNode;
class g3dSceneRenderer;
class g3dTargetRenderer;
class g3dViewer;
class scObject;

#define NMODELS 10

class demG3dTestHairShader 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestHairShader(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestHairShader();

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

		g3dRenderState* m_pStateRoot;

		// the model to draw
		entModelTemplate* m_entModel;
		scObject* m_scObj;

		// hair shader data
		matMaterial* m_pHairMaterial;
		effHairData* m_pHairData;

		// the lights
		g3dFragment* m_SmallSphereFragment;
		matMaterial m_SmallSphereMat;
		g3dSceneNode* m_Light1;
		g3dSceneNode* m_Light2;
		g3dPointLight* m_PointLight1;
		g3dPointLight* m_PointLight2;

		void PositionLights();

};
