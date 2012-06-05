/*****************************************************************************
**  demTestAnimsMode.hpp
**
**		This mode tests properties of entity animations
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_TESTANIMSMODE_HPP
#error demTestAnimsMode.hpp multiply included
#endif
#define DEM_TESTANIMSMODE_HPP

#ifndef DEM_VIEWERMODE_HPP
#include "demViewerMode.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "fsLocator.hpp"
#endif


class g3dViewer;
class g3dDirectionalLight;
class entEntity;
class entEntityTemplate;
class entScene;

class demTestAnimsMode 
:	public demViewerMode
{
	public:

		//====================================================================
		//====================================================================
		demTestAnimsMode(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demTestAnimsMode();

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

		//====================================================================
		//====================================================================
		void reset_camera();

		g3dViewer &m_Viewer;
		entScene *m_Scene;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
		
		fsLocator				m_DataPath;

		// Entity stuff
		entEntity*				m_Entity;
		entEntityTemplate*		m_EntityTemplate;
};
