/*****************************************************************************
**  demTestShadowShapes.hpp
**
**		This mode tests shadowing through projected lights
**	with shapes created through api3dShape.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_TESTSHADOWSHAPES_HPP
#error demTestShadowShapes.hpp multiply included
#endif
#define DEM_TESTSHADOWSHAPES_HPP

#ifndef DEM_TESTMODE_HPP
#include "demTestMode.hpp"
#endif

class g3dViewer;
class api3dObjectSimple;
class g3dProjectedLight;
class api3dProjectedLightWrapper;

class demTestShadowShapes 
:	public demTestMode
{
	public:

		//====================================================================
		//====================================================================
		demTestShadowShapes(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demTestShadowShapes();

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

		api3dObjectSimple* m_pShape1;
		api3dObjectSimple* m_pShape2;
		g3dProjectedLight* m_pLight;
		api3dProjectedLightWrapper *m_pPrjLightWrapper;

};
