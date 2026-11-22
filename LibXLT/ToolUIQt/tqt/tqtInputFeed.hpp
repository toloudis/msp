/****************************************************************************\
**	tqtInputFeed.hpp
**
**	Feeds the input package's keyboard and mouse (inDeviceMgr) from Qt.
**	Key and mouse button events anywhere in the application are passed
**	on, and the mouse's position and buttons are read from Qt each frame.
**	Wheel rotation is not handled here: the render panels report it, so
**	only the view under the cursor zooms.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_INPUTFEED_HPP
#error tqtInputFeed.hpp multiply included
#endif
#define TQT_INPUTFEED_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

namespace tqtInputFeed
{
	//--------------------------------------------------------------------
	// Init starts feeding the input package. Call it after inPackage::Init
	// and tqtAppUtil::Init.
	//--------------------------------------------------------------------
	void Init();

	//--------------------------------------------------------------------
	// CleanUp stops feeding the input package. Call it before
	// inPackage::CleanUp.
	//--------------------------------------------------------------------
	void CleanUp();
}

#endif // USE_QT
