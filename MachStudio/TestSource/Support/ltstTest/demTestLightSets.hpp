/*****************************************************************************
**  demTestLightSets.hpp
**
**		This mode tests the light sets code from MachStudio
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_TESTLIGHTSETS_HPP
#error demTestLightSets.hpp multiply included
#endif
#define DEM_TESTLIGHTSETS_HPP

#ifndef DEM_TESTMODE_HPP
#include "demTestMode.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "nameString.hpp"
#endif

class g3dFragment;
class g3dViewer;
class api3dObjectSingle;
class g3dDirectionalLight;
class nameObject;

class demTestLightSets 
:	public demTestMode
{
	public:

		//====================================================================
		//====================================================================
		demTestLightSets(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demTestLightSets();

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

		api3dObjectSingle* m_pObject;
		g3dDirectionalLight* m_pDirLight;

		nameObject* m_pObjectName;
		nameObject* m_pLightName;

		nameString m_LightSetName;
};
