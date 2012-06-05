/*****************************************************************************
**	cptrRenderRmanLiveUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderRmanLiveUtil.hpp"

#include "Features/Capture/cptrRenderRmanLiveMgr.hpp"

#include "Features/Prefs/PrefsDialogUtil.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/Prefs/wxGUI/prefsLayoutChoice.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#include "ToolUIWx/twx/twxToolbarMgr.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiCommandMgr.hpp"

//------------------------------------------------------------------------
//  CreateDialog - common step of creating form whether to be
//	displayed modal or modeless
//------------------------------------------------------------------------
void cptrRenderRmanLiveUtil::CreateDialog()
{
	//
	guiDialogTabbedMgr::Create("rmanLivePrefs", "RenderMan Live");
	guiDialogTabbedMgr::AddTabPage("rmanLivePrefs","Render Prefs");

	//	Main
	//prtyObject* pDO = PrefsMgr::GetDataObject();
	prtyObject* pDO = cptrRenderRmanLiveMgr::GetDataObject();
	//pDO->SortListByCategory();
	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	guiDialogTabbedMgr::BuildForm( "rmanLivePrefs", "Render Prefs", (pDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderRmanLiveUtil::Show()
{
	// In widgets, the dialog is created once when the main form is created
	// and then it is just shown and hidden from then on
	guiDialogTabbedMgr::Show("rmanLivePrefs");
}

//------------------------------------------------------------------------
//  AddToMenu() - add cptrRenderStats actions to menus
//------------------------------------------------------------------------
void cptrRenderRmanLiveUtil::AddToMenu()
{
	//	commands
	int menu_id;
	cmaCommand* pCmd;

	guiMenuMgr::AddSeparator( "Render" );

	//	COMMAND: open rman Live preferences
	pCmd = new cmaCommandSimple("RenderMan Live Preferences", 
								"Render", 
								"View the RenderMan Live Preferences",
								&cptrRenderRmanLiveUtil::Show );
	menu_id = guiMenuMgr::AddMenuItem( "Render", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Render", "RenderMan Live Preferences", pCmd );

	//	COMMAND: launch rman Live preview
	pCmd = new cmaCommandSimple("Render with RenderMan Live", 
								"Render", 
								"View RenderMan Live Preview",
								&cptrRenderRmanLiveMgr::TryRenderManLive );
	menu_id = guiMenuMgr::AddMenuItem( "Render", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Render", "Render with RenderMan Live", pCmd );
}