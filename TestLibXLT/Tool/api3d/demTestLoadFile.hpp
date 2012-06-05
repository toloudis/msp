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

class g3dFragment;
class g3dViewer;
class api3dObject;
class g3dDirectionalLight;

class demTestLoadFile 
:	public demTestMode
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

	private:
		g3dViewer &m_Viewer;

		api3dObject* m_pObject;
		g3dDirectionalLight* m_pDirLight;

};
