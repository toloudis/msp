/*****************************************************************************
**  demG3dTestFX.hpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTFX_HPP
#error demG3dTestFX.hpp multiply included
#endif
#define DEM_G3DTESTFX_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
#endif

class demG3dTestRenderer;
class g3dScene;
class g3dSceneNode;
class g3dSceneRenderer;
class g3dViewer;

class demG3dTestFX 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestFX(g3dViewer &i_Viewer, demG3dTestRenderer* i_Renderer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestFX();

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
		demG3dTestRenderer* m_pRenderer;
		g3dSceneRenderer* m_oldRenderer;
};
