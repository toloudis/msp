/*****************************************************************************
**  demTestLoadFile.hpp
**
**		This mode tests loading files through api3dImport
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_TESTLOADFILE_HPP
#error demTestLoadFile.hpp multiply included
#endif
#define DEM_TESTLOADFILE_HPP

#ifndef DEM_TESTMODE_HPP
#include "demTestMode.hpp"
#endif

#ifndef APP_MOUSEEVENTHANDLER_HPP
#include "Core/app/appMouseEventHandler.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif

class g3dViewer;
class api3dObject;
class g3dDirectionalLight;
class pick3dPickBuffer;

class demTestLoadFile 
:	public demTestMode,
	public appMouseEventHandler
{
	public:

		//====================================================================
		//====================================================================
		demTestLoadFile(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demTestLoadFile();

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

		api3dObject* m_pObject;
		g3dDirectionalLight* m_pDirLight;
		pick3dPickBuffer* m_pPickBuffer;
		//camCamera m_OrthoCamera;

};
