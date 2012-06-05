/*****************************************************************************
**  fgtPackage.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/FilmGates/fgtPackage.hpp"

#include "Features/FilmGates/fgtFrameMgr.hpp"

//	tools
#include "Core/dbg/dbgLog.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


//============================================================================
//============================================================================
namespace fgtPackage
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void create_menuitem_frames()
	{
		guiMenuMgr::AddMenu( "View", "Film Gates" );
	}

	//--------------------------------------------------------------------
	// Init -- initialize the package
	//--------------------------------------------------------------------
	void Init()
	{
//WXGUI
/*
		create_menuitem_frames();
*/

		fgtFrameMgr::Init();

		//	Add the frames
		//
		fgtFrameMgr::AddFrames();
	}

	//--------------------------------------------------------------------
	// CleanUp -
	//--------------------------------------------------------------------
	void CleanUp()
	{
		fgtFrameMgr::CleanUp();
	}
}
