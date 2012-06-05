/*****************************************************************************
**  demG3dTestRenderState.hpp
**
**		This mode displays a demonstration/test of using render states
**	to assign different lighting to different models.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTRENDERSTATE_HPP
#error demG3dTestRenderState.hpp multiply included
#endif
#define DEM_G3DTESTRENDERSTATE_HPP

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
class g3dDirectionalLight;
class g3dPointLight;
class g3dScene;
class g3dSceneNode;
struct g3dRenderState;

class demG3dTestRenderState 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestRenderState(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestRenderState();

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
		void ReceiveCharEvent(appCharEvent& i_Event);

	private:
		g3dViewer &m_Viewer;
		g3dScene *m_Scene;
		g3dSceneNode *m_Root;

		g3dFragment* m_RectFragment;
		g3dFragment* m_BigSphereFragment;
		g3dFragment* m_SmallSphereFragment;

		matMaterial m_RectMat;
		matMaterial m_BigSphereMat;
		matMaterial m_SmallSphereMat;

		g3dSceneNode* m_BaseRect;
		g3dSceneNode* m_Sphere1;
		g3dSceneNode* m_Sphere2;
		g3dSceneNode* m_Sphere3;

		g3dSceneNode* m_Light1;
		g3dSceneNode* m_Light2;

		matTexture* m_SphereTexture;

		g3dDirectionalLight* m_DirectionalLight;
		g3dPointLight* m_PointLight1;
		g3dPointLight* m_PointLight2;

		g3dRenderState* m_pStateRoot;
		g3dRenderState* m_pStateLights;
		g3dRenderState* m_pStateBlue;
		g3dRenderState* m_pStateRed;
		g3dRenderState* m_pStateAmbient;
};
