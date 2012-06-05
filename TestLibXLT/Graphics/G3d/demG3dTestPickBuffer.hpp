/*****************************************************************************
**  demG3dTestPickBuffer.hpp
**
**		This mode displays a demonstration/test of using a renderer
**	to encode what object is visible at what pixel. 
**	A way to use the GPU to do picking.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTPICKBUFFER_HPP
#error demG3dTestPickBuffer.hpp multiply included
#endif
#define DEM_G3DTESTPICKBUFFER_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
#endif

#ifndef APP_MOUSEEVENTHANDLER_HPP
#include "Core/app/appMouseEventHandler.hpp"
#endif

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif

#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif

class g3dViewer;
class scObject;
class g3dDirectionalLight;
class g3dScene;
class g3dSceneNode;
class g3dSceneRenderer;
class g3dTargetRenderer;

class demG3dTestPickBuffer : public demG3dTestMode,
							 public appMouseEventHandler	
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestPickBuffer(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestPickBuffer();

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

		//====================================================================
		//	Override this function to get appMouseUpEvents.
		//====================================================================
		virtual void ReceiveMouseUpEvent(appMouseUpEvent& i_Event);

		//====================================================================
		//	Override this function to get appMouseDownEvents.
		//====================================================================
		virtual void ReceiveMouseDownEvent(appMouseDownEvent& i_Event);

		//====================================================================
		//	Override this function to get appMouseDownEvents.
		//====================================================================
		virtual void ReceiveMouseMoveEvent(appMouseMoveEvent& i_Event);

	private:
		g3dViewer &m_Viewer;
		g3dScene *m_Scene;
		g3dSceneNode *m_Root;

		g3dSceneRenderer* m_pPickRenderer;
		g3dSceneRenderer* m_oldRenderer;
		int m_X, m_Y;
		envType::UInt32 m_ObjectCode;
		std::string m_ShapeName;
		g3dTargetRenderer* m_pTargetRenderer;
		//matTexture* m_PickBuffer;
		camCamera m_PickCamera;

		std::vector<g3dFragment*> m_Fragments;
		std::vector<scObject*> m_Objects;
		std::vector<matMaterial*> m_Materials;
		std::vector<matTexture*> m_Textures;

		g3dDirectionalLight* m_pDirLight;
};
