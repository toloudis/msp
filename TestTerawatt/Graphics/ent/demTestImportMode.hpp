/*****************************************************************************
**  demTestImportMode.hpp
**
**		This mode tests the entImport namespace
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_TESTIMPORTMODE_HPP
#error demTestImportMode.hpp multiply included
#endif
#define DEM_TESTIMPORTMODE_HPP

#ifndef DEM_VIEWERMODE_HPP
#include "demViewerMode.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "fsLocator.hpp"
#endif

class g3dViewer;
class g3dDirectionalLight;
class entEntity;
class entModelTemplate;
class entScene;

class demTestImportMode 
:	public demViewerMode
{
	public:

		//====================================================================
		//====================================================================
		demTestImportMode(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demTestImportMode();

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
		std::vector<entModelTemplate*>		m_Templates;
};
