/*****************************************************************************
**  mainCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/Commands/mainCommands.hpp"

#include "MainApp/Commands/mainCommandSetResolution.hpp"
#include "MainApp/MainForm.h"
#include "MainApp/wxGUI/wxHelpUtil.hpp"
#include "MainApp/wxGUI/wxMainForm.hpp"

#include "Features/Capture/cptrCategoryConfigFileUtil.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/ObjectManip/mnpConstants.hpp"
#include "Features/Prefs/prefsMgr.hpp"
#include "Features/ProjectSetup/Data/ProjectSetupData.hpp"
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Features/SceneSetup/GUI/SceneSetupDialogUtil.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Support/fgmt/GUI/fgmtOperations.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmAutoSaveMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmSecurityMgr.hpp"
#include "Support/mode/modeConstants.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "AudioDS/sn/snSoundManager.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/gui/guiToolbarMgr.hpp"
#include "ToolUIManaged/tma/tmaCommandTabControlUtil.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"


#include <assert.h>


//============================================================================
//============================================================================
#ifdef _MANAGED
using namespace StudioFramework;
//using namespace System::Xml;
#endif

//============================================================================
//
//============================================================================
namespace
{
	const char* lc_Resolutions_FileName = "Resolutions.cfg";

	//
	//	Command Functions
	//

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_MuteAudio(bool i_bChecked)
	{
		PrefsData& prefsdata = PrefsMgr::Data();
		prefsdata.m_bMuteAudio.SetValue(i_bChecked);

		snSoundManager::Mute( i_bChecked );

//WXGUI
/*
		guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mute, i_bChecked?"Mute":"" );
		guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Mute, 
			i_bChecked ? "Audio: Off" : "Audio: On" );
*/
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_Help()
	{
#ifdef _MANAGED
		MainForm::FormInstance->HelpDialog();
#endif
#ifdef USE_WXWIDGETS
//WXGUI
/*
		wxHelpUtil::ShowHelpInfo();
*/
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_About()
	{
#ifdef _MANAGED
		MainForm::FormInstance->AboutDialog();
#endif
#ifdef USE_WXWIDGETS
//WXGUI
/*
		wxHelpUtil::ShowHelpAbout();
*/
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_File_New()
	{
//WXGUI
		/*
				SceneSetupDialogUtil::Show();
*/

		//	only perform if the dongle is present
		if (mnmSecurityMgr::CheckForDongle())
			guiSingleDocHandler::New();

#ifdef _MANAGED
		MainForm::FormInstance->FileClosed();
#endif

//WXGUI
/*
		cmmSystemDialogUtil::UpdateDialog();
*/
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_File_Open()
	{
		//WXGUI
		/*
		// FIX: - shouldn't need these two lines once all of the scene files have the project chunk
		//
		ProjectSetupData data = ProjectSetupMgr::Data();
		fsFileUtil::LocatorToANSIFilename( gfPaths::GetPath( gfPaths::e_AppPath ), data.m_ProjectDirectory );

		//	only perform if the dongle is present
		if (mnmSecurityMgr::CheckForDongle())
		{
			guiSingleDocHandler::Open();
			mnmAppUtil::UpdateTitleBar( false );
		}

#ifdef _MANAGED
		MainForm::FormInstance->FileOpened();
#endif
	*/
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_File_Close()
	{
		//WXGUI
		/*
		//	only perform if the dongle is present
		if (mnmSecurityMgr::CheckForDongle())
			guiSingleDocHandler::New();
		
#ifdef _MANAGED
		MainForm::FormInstance->FileClosed();
#endif
	*/
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_File_Revert()
	{
		//WXGUI
		/*
		fsLocator curfile = docSingleDocumentMgr::GetFilename();
		if (curfile.GetNumNames() > 0)
		{
			//	only perform if the dongle is present
			if (mnmSecurityMgr::CheckForDongle())
				guiSingleDocHandler::Open(curfile);
			
#ifdef _MANAGED
			MainForm::FormInstance->FileOpened();
#endif
		}
		*/
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_File_Save()
	{
		//WXGUI
		/*
		//	only perform if the dongle is present
		if (!mnmSecurityMgr::CheckForDongle())
			return;

		mnmAutoSaveMgr::BackupSavedFile();
		guiSingleDocHandler::Save();
		mnmAppUtil::UpdateTitleBar( false );
	*/
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_File_SaveAs()
	{
		//WXGUI
		/*
		//	only perform if the dongle is present
		if (!mnmSecurityMgr::CheckForDongle())
			return;

		guiSingleDocHandler::SaveAs();
		mnmAppUtil::UpdateTitleBar( false );
	*/
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_File_Exit()
	{
		//WXGUI
		/*
#ifdef _MANAGED
		MainForm::FormInstance->Exit();
#endif
#ifdef USE_WXWIDGETS
		wxMainForm::Exit();
#endif
	*/
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_ActionToolbar(bool i_bChecked)
	{
		//WXGUI
		/*
		guiToolbarMgr::Show(mnpConstants::mc_Toolbar_Actions_Name, i_bChecked);
	*/
	}
	bool Get_ActionToolbar()
	{
		//WXGUI
		/*
		return guiToolbarMgr::IsVisible(mnpConstants::mc_Toolbar_Actions_Name);
	*/
		return false;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_PickModeToolbar(bool i_bChecked)
	{
		//WXGUI
		/*
		guiToolbarMgr::Show(mnpConstants::mc_Toolbar_PickMode_Name, i_bChecked);
	*/
	}
	bool Get_PickModeToolbar()
	{
		//WXGUI
		/*
		return guiToolbarMgr::IsVisible(mnpConstants::mc_Toolbar_PickMode_Name);
	*/
		return false;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_ModeToolbar(bool i_bChecked)
	{
		//WXGUI
		/*
		guiToolbarMgr::Show(modeConstants::mc_Toolbar_Modes_Name, i_bChecked);
	*/
	}
	bool Get_ModeToolbar()
	{
		//WXGUI
		/*
		return guiToolbarMgr::IsVisible(modeConstants::mc_Toolbar_Modes_Name);
	*/
		return false;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_TimeLine(bool i_bChecked)
	{
		//WXGUI
		/*
#ifdef _MANAGED
		MainForm::FormInstance->updateViewTimeline( i_bChecked );
#endif
#ifdef USE_WXWIDGETS
		twxPaneMgr::Show("Time Slider", i_bChecked);
#endif
	*/
	}
	bool Get_TimeLine()
	{
		//WXGUI
		/*
#ifdef _MANAGED
		return MainForm::FormInstance->IsViewTimeline();
#else
		
	#ifdef USE_WXWIDGETS
		return twxPaneMgr::IsVisible("Time Slider");
	#else
		return false;
	#endif

#endif
	*/
		return false;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_StatusBar(bool i_bChecked)
	{
		//WXGUI
		/*
#ifdef _MANAGED
		MainForm::FormInstance->updateViewStatusBar( i_bChecked );
#endif
#ifdef USE_WXWIDGETS
		if (twxSystem::g_pMainForm && twxSystem::g_pStatusBar)
		{
			if (i_bChecked)
				twxSystem::g_pMainForm->SetStatusBar(twxSystem::g_pStatusBar);
			else
				twxSystem::g_pMainForm->SetStatusBar(NULL);

			twxSystem::g_pStatusBar->Show(i_bChecked);
			twxPaneMgr::Update();
		}
#endif
	*/
	}
	bool Get_StatusBar()
	{
		//WXGUI
		/*
#ifdef _MANAGED
		return MainForm::FormInstance->IsViewStatusBar();
#else
	#ifdef USE_WXWIDGETS
		return (twxSystem::g_pStatusBar) ? twxSystem::g_pStatusBar->IsShown() : false;
	#else
		return false;
	#endif
#endif
	*/
		return false;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_AxisCompass(bool i_bChecked)
	{
		cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_World, i_bChecked );
	}
	bool Get_AxisCompass()
	{
		return 	cmpsCompassMgr::GetRenderable( cmpsCompassMgr::e_World );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Shadows(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bShadowsOn.SetValue(i_bChecked);
	}
	bool Get_Shadows()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bShadowsOn.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//void Set_Headlight(bool i_bChecked)
	//{
	//	rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bHeadlightOn.SetValue(i_bChecked);
	//}
	//bool Get_Headlight()
	//{
	//	return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bHeadlightOn.GetValue();
	//}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_SSAO(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableSSAO.SetValue(i_bChecked);
	}
	bool Get_SSAO()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableSSAO.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Fur(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableFur.SetValue(i_bChecked);
	}
	bool Get_Fur()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableFur.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_HDR_Renderer()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RendererType.SetValue(rndrPrefsMgr::GetRendererIndex(g3dSceneRendererCreate::e_HDR));
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_AO_Renderer()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RendererType.SetValue(rndrPrefsMgr::GetRendererIndex(g3dSceneRendererCreate::e_AmbientOcclusion));
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_SSAO_Low()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_SSAOQuality.SetValue(0);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_SSAO_Med()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_SSAOQuality.SetValue(1);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_SSAO_High()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_SSAOQuality.SetValue(2);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Reflections(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableReflection.SetValue(i_bChecked);
	}
	bool Get_Reflections()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableReflection.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_DeferParticles(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableDeferredTransparency.SetValue(i_bChecked);
	}
	bool Get_DeferParticles()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableDeferredTransparency.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Environments(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableEnvironment.SetValue(i_bChecked);
	}
	bool Get_Environments()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableEnvironment.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Glow(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableGlow.SetValue(i_bChecked);
	}
	bool Get_Glow()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableGlow.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Outline(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableOutline.SetValue(i_bChecked);
	}
	bool Get_Outline()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableOutline.GetValue();
	}


	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_AO(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableAO.SetValue(i_bChecked);
	}
	bool Get_AO()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableAO.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_LowRes(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bLowResolution.SetValue(i_bChecked);
	}
	bool Get_LowRes()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bLowResolution.GetValue();
	}

}

//--------------------------------------------------------------------
// SetupMenu
//--------------------------------------------------------------------
void mainCommands::SetupMenu()
{
	//WXGUI
	/*
	PrefsData prefsdata = PrefsMgr::Data();

	//
	//	commands
	//
	int menu_id;
	cmaCommand* pCmd;

	//
	//	Set Resolutions
	//
	guiMenuMgr::AddMenu( "View", "Resolutions" );
	guiMenuMgr::AddMenu( "View", "Toolbars" );

	//	read in the configuration file for resolutions
	//
	fsLocator cfgdir;
	cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
	cfgdir.Push( lc_Resolutions_FileName );
	category_list_type resolutions(2, std::vector<std::string>(0));
	cptrCategoryConfigFileUtil::SetCategoryTag(std::string("Category"));
	cptrCategoryConfigFileUtil::SetElementTag(std::string("Resolution"));
	cptrCategoryConfigFileUtil::ReadConfigFile( cfgdir, resolutions );
	
	//	Set-up the various parts of the application with each resolution
	//	from the list
	//
	for (int i=0; i < resolutions[0].size(); ++i)
	{
		//DBG_LOG3("%02d. %s - %s", i, resolutions[0][i].c_str(), resolutions[1][i].c_str());

		//	add the category (if it isn't already added)
		guiMenuMgr::AddMenu( "Resolutions", resolutions[0][i].c_str() );
		
		//	add the resolution to the menu, command, and system
		char tempstr[64];
		sprintf(tempstr,"panel res %s", resolutions[1][i].c_str());
		menu_id = guiMenuMgr::AddMenuItem( resolutions[0][i].c_str(), tempstr );
		int width,height;
		sscanf( resolutions[1][i].c_str(), "%dx%d ", &width, &height );
		pCmd = new mainCommandSetResolution( width, height );
		const std::string tag(pCmd->GetTag());
		guiCommandMgr::Add( pCmd, tag, menu_id );
		sprintf(tempstr,"%s - %s", resolutions[0][i].c_str(), resolutions[1][i].c_str());
		cmmSystemDialogUtil::AddSystemCommand( "Resolutions", tempstr, pCmd );
	}

	//	COMMAND: File -> New
	pCmd = new cmaCommandSimple("New", 
								"File", 
								"Start a new scene",
								&Execute_File_New );
	menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "File", "New", pCmd );

	//	COMMAND: File -> Open
	pCmd = new cmaCommandSimple("Open", 
								"File", 
								"Open a scene",
								&Execute_File_Open );
	menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "File", "Open", pCmd );

	//	COMMAND: File -> Close
	pCmd = new cmaCommandSimple("Close", 
								"File", 
								"Close the current scene",
								&Execute_File_Close );
	menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "File", "Close", pCmd );

	//	COMMAND: File -> Revert
	pCmd = new cmaCommandSimple("Revert", 
								"File", 
								"Revert the current scene to the version saved",
								&Execute_File_Revert );
	menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "File", "Revert", pCmd );

	//	COMMAND: File -> Save
	pCmd = new cmaCommandSimple("Save", 
								"File", 
								"Save the current scene",
								&Execute_File_Save );
	menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "File", "Save", pCmd );

	//	COMMAND: File -> SaveAs
	pCmd = new cmaCommandSimple("Save As", 
								"File", 
								"Save the current scene under a new name",
								&Execute_File_SaveAs );
	menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "File", "Save As", pCmd );

	//	COMMAND: Bake
	//pCmd = new cmaCommandSimple("Bake", 
	//							"File", 
	//							"Bake materials and lighting",
	//							&fgmtOperations::Bake );
	//menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	//cmmSystemDialogUtil::AddSystemCommand( "File", "Bake", pCmd );

	//	COMMAND: File -> Exit
	pCmd = new cmaCommandSimple("Exit", 
								"File", 
								"Exit the application",
								&Execute_File_Exit );
	menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "File", "Exit", pCmd );

	//	COMMAND: Redo
	pCmd = new cmaCommandSimple("Redo", 
								"Edit", 
								"Redo the last undone action",
								&undoUndoMgr::Redo );
	menu_id = guiMenuMgr::AddMenuItem( "Edit", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Edit", "Redo", pCmd );

	//	COMMAND: Undo
	pCmd = new cmaCommandSimple("Undo", 
								"Edit", 
								"Undo the last action",
								&undoUndoMgr::Undo );
	menu_id = guiMenuMgr::AddMenuItem( "Edit", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Edit", "Undo", pCmd );

	//	COMMAND: Action toolbar
	pCmd = new cmaCommandToggle("Action Toolbar", 
								"Toolbars", 
								"View the action toolbar",
								&Set_ActionToolbar, 
								&Get_ActionToolbar );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Toolbars", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View Action Toolbar", pCmd );

	//	COMMAND: Mode toolbar
	pCmd = new cmaCommandToggle("Mode Toolbar", 
								"Toolbars", 
								"View the Mode toolbar",
								&Set_ModeToolbar, 
								&Get_ModeToolbar );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Toolbars", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View Mode Toolbar", pCmd );

#ifndef _MANAGED
	//	COMMAND: Pick Mode toolbar
	pCmd = new cmaCommandToggle("Pick Mode Toolbar", 
								"Toolbars", 
								"View the PickMode toolbar",
								&Set_PickModeToolbar, 
								&Get_PickModeToolbar );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Toolbars", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View PickMode Toolbar", pCmd );
#endif

	//	COMMAND:  TimeLine
	pCmd = new cmaCommandToggle("TimeLine", 
								"View", 
								"View the TimeLine",
								&Set_TimeLine, 
								&Get_TimeLine );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View TimeLine", pCmd );

	//	COMMAND:  StatusBar
	pCmd = new cmaCommandToggle("StatusBar", 
								"View", 
								"View the StatusBar",
								&Set_StatusBar, 
								&Get_StatusBar );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View StatusBar", pCmd );

	//	COMMAND:  Channels
	pCmd = new cmaCommandSimple("Channels", 
								"Windows", 
								"View the Channels dialog",
								&chnlDialogUtil::ShowChannelEditor );
	menu_id = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View Channels", pCmd );

	//	COMMAND: View AxisCompass
	pCmd = new cmaCommandToggle("Axis Compass", 
								"View", 
								"Toggle Axis Compass Visibility",
								&Set_AxisCompass,
								&Get_AxisCompass );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle AxisCompass Visibility", pCmd );


	guiMenuMgr::AddMenu( "View", "Renderer" );
	//	COMMAND: Use HDR renderer
	pCmd = new cmaCommandSimple("HDR", 
								"Renderer", 
								"Use the HDR renderer",
								&Set_HDR_Renderer );
	menu_id = guiMenuMgr::AddMenuItem( "Renderer", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Use HDR renderer", pCmd );
	//	COMMAND: Use AO-Only renderer
	pCmd = new cmaCommandSimple("AO Only", 
								"Renderer", 
								"Use the AO Only renderer",
								&Set_AO_Renderer );
	menu_id = guiMenuMgr::AddMenuItem( "Renderer", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Use AO Only renderer", pCmd );

	//	COMMAND: View Shadows
	pCmd = new cmaCommandToggle("Shadows", 
								"View", 
								"Toggle Shadows Visibility",
								&Set_Shadows,
								&Get_Shadows );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Shadows Visibility", pCmd );

	guiMenuMgr::AddMenu( "View", "SSAO Sampling" );
	//	COMMAND: SSAO low sampling 
	pCmd = new cmaCommandSimple("Low", 
								"SSAO", 
								"SSAO low sampling",
								&Set_SSAO_Low );
	menu_id = guiMenuMgr::AddMenuItem( "SSAO Sampling", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "SSAO low sampling", pCmd );
	//	COMMAND: SSAO med sampling 
	pCmd = new cmaCommandSimple("Med", 
								"SSAO", 
								"SSAO medium sampling",
								&Set_SSAO_Med );
	menu_id = guiMenuMgr::AddMenuItem( "SSAO Sampling", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "SSAO medium sampling", pCmd );
	//	COMMAND: SSAO high sampling 
	pCmd = new cmaCommandSimple("High", 
								"SSAO", 
								"SSAO high sampling",
								&Set_SSAO_High );
	menu_id = guiMenuMgr::AddMenuItem( "SSAO Sampling", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "SSAO high sampling", pCmd );

	//	COMMAND: View SSAO
	pCmd = new cmaCommandToggle("SSAO", 
								"SSAO", 
								"Toggle SSAO Visibility",
								&Set_SSAO,
								&Get_SSAO );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle SSAO Visibility", pCmd );

	//	COMMAND: View Headlight
	pCmd = new cmaCommandToggle("Headlight", 
								"View", 
								"Toggle Headlight Enabled",
								&Set_Headlight,
								&Get_Headlight );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Headlight Enabled", pCmd );

	//	COMMAND: View Fur
	pCmd = new cmaCommandToggle("Fur", 
								"View", 
								"Toggle Fur Visibility",
								&Set_Fur,
								&Get_Fur );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Fur Visibility", pCmd );

	//	COMMAND: View Reflections
	pCmd = new cmaCommandToggle("Reflections", 
								"View", 
								"Toggle Reflections Visibility",
								&Set_Reflections,
								&Get_Reflections );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Reflections Visibility", pCmd );

	//	COMMAND: Defer Particles
	pCmd = new cmaCommandToggle("DeferParticles", 
								"View", 
								"Defer Particle rendering",
								&Set_DeferParticles,
								&Get_DeferParticles );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Defer Particle rendering", pCmd );

	//	COMMAND: View Environments
	pCmd = new cmaCommandToggle("Environments", 
								"View", 
								"Toggle Environments Visibility",
								&Set_Environments,
								&Get_Environments );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Environments Visibility", pCmd );

	//	COMMAND: View Glow
	pCmd = new cmaCommandToggle("Glow", 
								"View", 
								"Toggle Glow Visibility",
								&Set_Glow,
								&Get_Glow );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Glow Visibility", pCmd );

	//	COMMAND: View Fur
	pCmd = new cmaCommandToggle("AO", 
								"View", 
								"Toggle Ambient Occlusion Visibility",
								&Set_AO,
								&Get_AO );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Ambient Occlusion Visibility", pCmd );

	//	COMMAND: View Outline
	pCmd = new cmaCommandToggle("Outline", 
		"View", 
		"Toggle Outline Visibility",
		&Set_Outline,

		&Get_Outline );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Outline Visibility", pCmd );

	//	COMMAND: View LowRes
	pCmd = new cmaCommandToggle("LowRes", 
								"View", 
								"Toggle Low Resolution",
								&Set_LowRes,
								&Get_LowRes );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Low Resolution", pCmd );

	//	COMMAND: Mute Audio
	pCmd = new cmaCommandToggle("Mute Audio", 
								"Actions", 
								"Mute the audio during scrubbing or playback",
								&Set_MuteAudio, 
								&snSoundManager::IsMuted );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Actions", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Actions", "Mute Audio", pCmd );

	//	COMMAND: Help dialog
	pCmd = new cmaCommandSimple("Help Info", 
								"Help", 
								"Help dialog",
								&Execute_Help );
	menu_id = guiMenuMgr::AddMenuItem( "Help", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Help", "Help Info", pCmd );

	//	COMMAND: About dialog
	pCmd = new cmaCommandSimple("About", 
								"Help", 
								"About dialog",
								&Execute_About );
	menu_id = guiMenuMgr::AddMenuItem( "Help", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Help", "About", pCmd );
*/
}
