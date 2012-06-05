/*****************************************************************************
**	cptrRenderMrayLiveUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderMrayLiveUtil.hpp"

#include "Features/Capture/cptrRenderMrayLiveMgr.hpp"

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
void cptrRenderMrayLiveUtil::CreateDialog()
{
	//
	guiDialogTabbedMgr::Create("mrayLivePrefs", "mental ray Live");
	guiDialogTabbedMgr::AddTabPage("mrayLivePrefs","Render Prefs");

	//	Main
	//prtyObject* pDO = PrefsMgr::GetDataObject();
	prtyObject* pDO = cptrRenderMrayLiveMgr::GetDataObject();
	//pDO->SortListByCategory();
	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	guiDialogTabbedMgr::BuildForm( "mrayLivePrefs", "Render Prefs", (pDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderMrayLiveUtil::Show()
{
	// In widgets, the dialog is created once when the main form is created
	// and then it is just shown and hidden from then on
	guiDialogTabbedMgr::Show("mrayLivePrefs");
}

//------------------------------------------------------------------------
//  AddToMenu() - add cptrRenderStats actions to menus
//------------------------------------------------------------------------
void cptrRenderMrayLiveUtil::AddToMenu()
{
	//	commands
	int menu_id;
	cmaCommand* pCmd;

	guiMenuMgr::AddSeparator( "Render" );

	//	COMMAND: open mray Live preferences
	pCmd = new cmaCommandSimple("mental ray Live Preferences", 
								"Render", 
								"View the mental ray Live Preferences",
								&cptrRenderMrayLiveUtil::Show );
	menu_id = guiMenuMgr::AddMenuItem( "Render", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Render", "mental ray Live Preferences", pCmd );

	//	COMMAND: launch mray Live preview
	pCmd = new cmaCommandSimple("Render with mental ray Live", 
								"Render", 
								"View mental ray Live Preview",
								&cptrRenderMrayLiveMgr::TryMentalRayLive );
	menu_id = guiMenuMgr::AddMenuItem( "Render", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Render", "Render with mental ray Live", pCmd );
}