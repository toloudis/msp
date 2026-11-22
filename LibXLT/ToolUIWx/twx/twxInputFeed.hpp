/****************************************************************************\
**	twxInputFeed.hpp
**
**	Feeds the input package's keyboard and mouse (inDeviceMgr) from
**	wxWidgets. Key and mouse button events anywhere in the application are
**	passed on (twxAppUtil's wxApp hands them to FilterEvent), and the
**	mouse's position and buttons are read from wxWidgets each frame.
**	Wheel rotation is not handled here: the render panels report it, so
**	only the view under the cursor zooms.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWX_INPUTFEED_HPP
#error twxInputFeed.hpp multiply included
#endif
#define TWX_INPUTFEED_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

namespace twxInputFeed
{
	//--------------------------------------------------------------------
	// Init starts feeding the input package. Call it after inPackage::Init.
	//--------------------------------------------------------------------
	void Init();

	//--------------------------------------------------------------------
	// CleanUp stops feeding the input package. Call it before
	// inPackage::CleanUp.
	//--------------------------------------------------------------------
	void CleanUp();

	//--------------------------------------------------------------------
	// FilterEvent looks at every event in the application, before it is
	// processed. It never stops an event.
	//--------------------------------------------------------------------
	void FilterEvent(wxEvent& i_Event);
}

#endif // USE_WXWIDGETS
