/*****************************************************************************
**  demTestSplineOrient.hpp
**
**		This mode tests spline code from MachStudio
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_TESTSPLINEORIENT_HPP
#error demTestSplineOrient.hpp multiply included
#endif
#define DEM_TESTSPLINEORIENT_HPP

#ifndef DEM_TESTMODE_HPP
#include "demTestMode.hpp"
#endif

class g3dFragment;
class g3dViewer;
class api3dObject;
class g3dDirectionalLight;
class splnSpline;

class demTestSplineOrient 
:	public demTestMode
{
	public:

		//====================================================================
		//====================================================================
		demTestSplineOrient(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demTestSplineOrient();

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

		// Tangent Line
		api3dObject* m_pTangentObj;
		g3dFragment* m_pLineFrag;

		// Spline stuff
		splnSpline* m_pCurve;
		float m_SplinePerc;
		bool m_bAnimate;
};
