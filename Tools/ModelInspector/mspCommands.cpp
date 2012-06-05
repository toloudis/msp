/*****************************************************************************
**  mspCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "mspCommands.hpp"
#include "mspCamAnim.hpp"
#include "mspCommandSubdivLevel.hpp"
#include "mspModel.hpp"
#include "mspVersion.hpp"
#include "mspViewSettings.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"

#include <sstream>

//============================================================================
//
//============================================================================
namespace
{
#ifdef USE_WXWIDGETS
	#define MENU_NAME(mName, wxName) wxName
#else
	#define MENU_NAME(mName, wxName) mName
#endif

	//
	//	Command Functions
	//

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_About()
	{	
		// the about box
		std::ostringstream str;
		str << c_AssemblyTitle << " " << c_AssemblyVersion;
		str << "\n\n";
		str << "Copyright (c) 2009, Studio GPU\n";
		str <<  "All rights reserved\n";

		guiMessageBox::Show( str.str().c_str(), c_AssemblyTitle, guiMessageBox::e_OKOnly );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_LoadAnim(cmaCommand* i_pCommand)
	{	
		const std::string filter = "Animation files (*.*a*)|*.*a*|General Animation (*.gab)|*.gab|Joint Animation (*.jna)|*.jna|Character Animation (*.cha)|*.cha|All files (*.*)|*.*";
		fsLocator initial_dir = mspModel::GetAnimationDir();
		fsLocator file_loc;
		if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
		{
			mspModel::LoadAnimation(file_loc);
		}
	}
	void Update_LoadAnim(cmaCommand* i_pCommand)
	{
		// Enable menu item only if an animatable model is loaded
		cmaCommandMgr::CommandSetEnabled( i_pCommand, mspModel::CanLoadAnimation() );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_LoadCameraAnim()
	{	
		const std::string filter = "Camera Animation files (*.cam)|*.cam|All files (*.*)|*.*";
		fsLocator initial_dir = mspModel::GetAnimationDir();
		fsLocator file_loc;
		if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
		{
			mspCamAnim::LoadAnimation(file_loc);
		}
	}
	void Execute_ClearCameraAnim()
	{	
		mspCamAnim::Clear();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_TimeSlider()
	{
#ifdef USE_WXWIDGETS
		twxPaneMgr::Show("TimeSlider");
#endif
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_AnimationSettings()
	{
#ifdef USE_WXWIDGETS
		twxPaneMgr::Show("AnimationSettings");
#endif
	}
}

//--------------------------------------------------------------------
// SetupMenu
//--------------------------------------------------------------------
void mspCommands::SetupMenu()
{
	//
	//	commands
	//
	int menu_id;
	cmaCommand* pCmd;
	
	guiMenuMgr::AddMenu("Animation", "");
	guiMenuMgr::AddMenu("View", "");
#ifdef USE_WXWIDGETS
	guiMenuMgr::AddMenu("Window", "");
#endif
	guiMenuMgr::AddMenu("Help", "");

	//	COMMAND: Load Animation
	pCmd = new cmaCommand("Load Animation", 
							&Execute_LoadAnim,
							&Update_LoadAnim );
	pCmd->SetCategory("Animation");
	pCmd->SetDescription("Load animation file");
	menu_id = guiMenuMgr::AddMenuItem( "Animation", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	//guiMenuMgr::MenuObjectsEnable(menu_id, false);

	//	COMMAND: Pause Animation
	pCmd = new cmaCommandToggle("Pause Animation", 
								"Animation", 
								"Toggle playback of animation",
								&mspViewSettings::SetPauseAnimation,
								&mspViewSettings::GetPauseAnimation);

	menu_id = guiMenuMgr::AddCheckableMenuItem( "Animation", 
		MENU_NAME("Pause Animation", "Pause Animation \tCtrl-P"));
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	//guiMenuMgr::MenuObjectsCheck(menu_id, true);

	//	COMMAND: Load Camera Animation
	pCmd = new cmaCommandSimple("Load Camera Animation", 
								"View", 
								"Load a camera animation file",
								Execute_LoadCameraAnim );
	menu_id = guiMenuMgr::AddMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Clear Camera Animation
	pCmd = new cmaCommandSimple("Clear Camera Animation", 
								"View", 
								"Remove current camera animation file",
								Execute_ClearCameraAnim );
	menu_id = guiMenuMgr::AddMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Focus Camera
	pCmd = new cmaCommandSimple("Focus Camera", 
								"View", 
								"Focus camera on model",
								&mspModel::FocusCamera );
	menu_id = guiMenuMgr::AddMenuItem( "View", 
		MENU_NAME("Focus Camera", "Focus Camera \tCtrl-F"));
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: View Wireframe
	pCmd = new cmaCommandToggle("Wireframe", 
								"View", 
								"Toggle Wireframe Visibility",
								&mspViewSettings::SetViewWireframe,
								&mspViewSettings::GetViewWireframe );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", 
		MENU_NAME("Wireframe", "Wireframe \tCtrl-W"));
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: View Low resolution
	pCmd = new cmaCommandToggle("Low Resolution", 
								"View", 
								"Toggle Low Resolution",
								&mspModel::SetUseLowResModel,
								&mspModel::GetUseLowResModel );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", 
		MENU_NAME("Low Resolution", "Low Resolution \tCtrl-L"));
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	// Subdivision level - 3 menu items in a sub menu
	guiMenuMgr::AddMenu("View", "Subdivision Level");

	pCmd = new mspCommandSubdivLevel("Base Mesh", 0);
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Subdivision Level", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	pCmd = new mspCommandSubdivLevel("Subdiv Level 1", 1);
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Subdivision Level", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	pCmd = new mspCommandSubdivLevel("Subdiv Level2", 2);
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Subdivision Level", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	
#ifdef USE_WXWIDGETS
	//	COMMAND: View Time Slider
	pCmd = new cmaCommandSimple("Time Slider", 
								"Window", 
								"Show the time slider",
								Execute_TimeSlider );
	menu_id = guiMenuMgr::AddMenuItem( "Window", "Time Slider \tCtrl-T" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: View Animation Settings
	pCmd = new cmaCommandSimple("Animation Settings", 
								"Window", 
								"Edit the animation settings",
								Execute_AnimationSettings );
	menu_id = guiMenuMgr::AddMenuItem( "Window", "Animation Settings \tCtrl-A" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
#endif

	//	COMMAND: About dialog
	pCmd = new cmaCommandSimple("About", 
								"Help", 
								"About dialog",
								&Execute_About );
	menu_id = guiMenuMgr::AddMenuItem( "Help", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
}
