/*****************************************************************************
**  demTestOrthographic.hpp
**
**		This mode tests orthographic camera settings
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_TESTORTHOGRAPHIC_HPP
#error demTestOrthographic.hpp multiply included
#endif
#define DEM_TESTORTHOGRAPHIC_HPP

#ifndef DEM_TESTMODE_HPP
#include "demTestMode.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif

class g3dViewer;
class api3dObject;
class g3dDirectionalLight;

class demTestOrthographic 
:	public demTestMode
{
	public:

		//====================================================================
		//====================================================================
		demTestOrthographic(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demTestOrthographic();

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

		api3dObject* m_pObject;
		g3dDirectionalLight* m_pDirLight;
		camCamera m_OrthoCamera;

};
