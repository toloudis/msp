/*****************************************************************************
**  demTestClonesMode.hpp
**
**		This mode tests creating more than one entity from a template
**	and making sure animations run independently.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_TESTCLONESMODE_HPP
#error demTestClonesMode.hpp multiply included
#endif
#define DEM_TESTCLONESMODE_HPP

#ifndef DEM_VIEWERMODE_HPP
#include "demViewerMode.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "fsLocator.hpp"
#endif

#include <vector>

class g3dViewer;
class g3dDirectionalLight;
class entEntity;
class entEntityTemplate;
class entScene;

class demTestClonesMode 
:	public demViewerMode
{
	public:

		//====================================================================
		//====================================================================
		demTestClonesMode(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demTestClonesMode();

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
		std::vector<entEntity*>				m_Entities;
		std::vector<entEntityTemplate*>		m_EntityTemplates;
};
