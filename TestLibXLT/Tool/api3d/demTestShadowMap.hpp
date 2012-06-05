/*****************************************************************************
**  demTestShadowMap.hpp
**
**		This mode tests shadowing through projected lights
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_TESTSHADOWMAP_HPP
#error demTestShadowMap.hpp multiply included
#endif
#define DEM_TESTSHADOWMAP_HPP

#ifndef DEM_TESTMODE_HPP
#include "demTestMode.hpp"
#endif

class g3dViewer;
class api3dObject;
class g3dProjectedLight;
class api3dProjectedLightWrapper;

class demTestShadowMap 
:	public demTestMode
{
	public:

		//====================================================================
		//====================================================================
		demTestShadowMap(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demTestShadowMap();

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
		api3dObject* m_pObject2;
		g3dProjectedLight* m_pLight;
		api3dProjectedLightWrapper *m_pPrjLightWrapper;

};
