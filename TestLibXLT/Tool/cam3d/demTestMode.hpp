/*****************************************************************************
**  demTestMode.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_TESTMODE_HPP
#error demTestMode.hpp multiply included
#endif
#define DEM_TESTMODE_HPP

#ifndef APP_CHAREVENTHANDLER_HPP
#include "Core/app/appCharEventHandler.hpp"
#endif

#ifndef APP_MODE_HPP
#include "Core/app/appMode.hpp"
#endif

#ifndef G2D_FONTHANDLE_HPP
#include "Graphics/g2d/g2dFontHandle.hpp"
#endif

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

class demTestMode 
:	public appMode,
	public appCharEventHandler	
{
	public:

		//====================================================================
		//====================================================================
		demTestMode();

		//====================================================================
		//====================================================================
		virtual ~demTestMode();

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

	protected:

		//====================================================================
		//	GetQuitSignaled returns true if the user is done with this mode 
		//	(pressed the space bar).
		//====================================================================
		bool GetQuitSignaled() const;

	private:

		bool m_QuitSignaled;
};

