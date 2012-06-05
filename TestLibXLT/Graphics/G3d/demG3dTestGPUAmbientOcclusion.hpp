/*****************************************************************************
**  demG3dTestGPUAmbientOcclusion.hpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Gigawatt Studios
**	Copyright(C) 2000 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTGPUAMBIETNOCCLUSION_HPP
#error demG3dTestGPUAmbientOcclusion.hpp multiply included
#endif
#define DEM_G3DTESTGPUAMBIETNOCCLUSION_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
#endif

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif

#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif

class AOEngine;
class g3dViewer;
class g3dScene;
class g3dSceneNode;
class g3dDirectionalLight;
class g3dPointLight;
class g3dSceneRenderer;
class shdwAOPreviewRendererDX9;
class entModelTemplate;
class scObject;

class demG3dTestGPUAmbientOcclusion 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestGPUAmbientOcclusion(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestGPUAmbientOcclusion();

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

		g3dDirectionalLight* m_Light0;
		g3dDirectionalLight* m_Light1;

		void LoadModel(std::string i_name);
		int m_CurModel;
		int m_CurMesh;
		// the model to draw
		entModelTemplate* m_entModel;
		scObject* m_scObj;

		shdwAOPreviewRendererDX9* m_pRenderer;
		g3dSceneRenderer* m_oldRenderer;

		int m_AOTexRes;

		void SetDisplayInfo();

};
