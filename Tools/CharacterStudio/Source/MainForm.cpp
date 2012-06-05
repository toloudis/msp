#include "StdAfx.h"
#include "MainForm.h"
#include "SwitchModelDialog.h"

#include "GraphicsDX9/eff/effShaderArray.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "nonGUI/chrApp.hpp"
#include "chrDialogUtil.hpp"
#include "nonGUI/chrLevel.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "ToolUIManaged/mui/muiMessageBox.hpp"
#include "Tool/gui/guiCustomDocHandler.hpp"
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"

// for PeekMessage()
#include <windows.h>

using namespace CharacterStudio;

namespace
{
	// History:
	// 1.2 - Support for subdivision surfaces
	// 1.2.4 - Support for triangle sort and double-sided flags
	// 1.2.5 - Fix so that unskinned verts use position from original mesh
	// 1.2.6 - Look for textures in ../Textures or in local directory
	// 1.2.7 - Loads .cha animation files
	// 1.3 - Expressions
	// 1.3.1 - Additive blending for subanims
	// 1.3.2 - Switch model, keeping expressions in place
	// 1.3.3 - Load multiple expressions at once
	// 1.4 - Can mix meshes and subdivs in character model
	// 1.4.0.1 - rebuild
	// 1.5 - Vertex animation
	// 1.5.1 - Rebuild for new material format
	// 1.5.2 - Can handle static models
	// 1.6.0 - Rebuilt with VS2005
	// 1.6.0.1 - Reads in joint's "RotateAxis" field
	// 1.6.1.0 - Handles Low-res models, can view joints
	// 1.6.1.1 - Automatic low-res generation
	// 1.6.1.2 - New CLR syntax
	// 1.6.2.0 - Rebuild for new shaders
	// 1.6.2.1 - Fix for vertex anims not attached to skeletons
	// 1.7.0.0 - Can group expressions so that two are controlled by a single slider
	// 1.7.1.0 - Display error dialog when animation is incompatible with model
	const char* c_CharacterStudioVersion = "CharacterStudio Version 1.7.1.0";
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::MainForm_Load(System::Object^  sender, System::EventArgs^  e)
{
	// Set-up the application idle event handler
	//
	System::Windows::Forms::Application::Idle += gcnew System::EventHandler(this, &MainForm::OnApplicationIdle);				
}

//----------------------------------------------------------------------------
//	OnApplicationIdle - when there are no messages pending, keep doing redraws
//----------------------------------------------------------------------------
System::Void MainForm::OnApplicationIdle(System::Object ^  sender, System::EventArgs ^  e)
{
	if (m_bInIdleCallback) 
		return;
	
	m_bInIdleCallback = true;
	
	MSG msg;
	while (!::PeekMessage(&msg, NULL, 0,0,0))
	{
		if (!chrApp::IsActive())
		{
			break;
		}

		chrApp::ThinkApp();

		render_3dWindow();
	}

	m_bInIdleCallback = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuFileNew_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	guiCustomDocHandler::New();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuFileOpen_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	guiCustomDocHandler::Open();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuFileSave_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	guiCustomDocHandler::Save();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuFileSaveAs_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	guiCustomDocHandler::SaveAs();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuSwitchModel_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	SwitchModelDialog ^dialog = gcnew SwitchModelDialog();
	if (dialog->ShowDialog() == ::DialogResult::OK)
	{
		fsLocator model_loc;
		tmaManagedStringUtils::ManagedStringToLocator(dialog->GetModelFilename(), model_loc);
		fsLocator anim_loc;
		tmaManagedStringUtils::ManagedStringToLocator(dialog->GetAnimationFilename(), anim_loc);
		
		chrLevel::SwitchModel(model_loc, anim_loc);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuLoadAnimation_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	const std::string filter = "Animation files (*.*a*)|*.*a*|Joint Animation (*.jna)|*.jna|Character Animation (*.cha)|*.cha|All files (*.*)|*.*";
	fsLocator initial_dir;
	fsLocator file_loc;
	if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
	{
		chrLevel::LoadAnimation(file_loc);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuLoadSubAnimation_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	const std::string filter = "Animation files (*.*a*)|*.*a*|Joint Animation (*.jna)|*.jna|Character Animation (*.cha)|*.cha|All files (*.*)|*.*";
	fsLocator initial_dir;
	fsLocator file_loc;
	if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
	{
		chrLevel::LoadSubAnimation(file_loc);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuFileExit_Click(System::Object ^  sender, System::EventArgs ^  e)
{	
	if (guiCustomDocHandler::SaveIfDirty())
	{
		Application::Exit();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuView_Popup(System::Object ^  sender, System::EventArgs ^  e)
{
	this->menuViewShaders->Checked = matShaderMgr::GetUseShaderArray();
	this->menuViewShadows->Checked = g3dSingleLightRendering::GetDoSingleLightRendering();
	this->menuViewWireframe->Checked = chrApp::GetWireframe();
	this->menuViewLowRes->Enabled = chrLevel::HasLowResModel();
	this->menuViewLowRes->Checked = chrLevel::GetUseLowResModel();
	this->menuViewJoints->Enabled = chrLevel::HasBoneDisplay();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuViewShaders_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	matShaderMgr::SetUseShaderArray(!matShaderMgr::GetUseShaderArray());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuViewShadows_Click(System::Object ^  sender, System::EventArgs ^  e)
{	
	g3dSingleLightRendering::SetDoSingleLightRendering(!g3dSingleLightRendering::GetDoSingleLightRendering());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuViewResetCamera_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	chrLevel::FocusCamera();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuViewWireframe_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	chrApp::SetWireframe(!chrApp::GetWireframe());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuViewLowRes_Click(System::Object^  sender, System::EventArgs^  e) 
{
	chrLevel::SetUseLowResModel( !chrLevel::GetUseLowResModel() );		 
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuViewJoints_Popup(System::Object^  sender, System::EventArgs^  e) 
{
	int display = chrLevel::GetBoneDisplay();
	this->menuViewModelOnly->Checked = (display == 0); 
	this->menuViewJointsOnly->Checked = (display == 1); 
	this->menuViewModelAndJoints->Checked = (display == 2); 
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuViewJoints_ChangeDisplay(System::Object^  sender, System::EventArgs^  e) 
{
	System::Windows::Forms::MenuItem ^pMenuItem = safe_cast<MenuItem^>(sender);
	int value = System::Int16::Parse( pMenuItem->Tag->ToString() );
	chrLevel::SetBoneDisplay( value );	 
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuWindowLights_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	chrDialogUtil::ShowLightsDialog(true);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuWindowExpressions_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	chrDialogUtil::ShowExpressionsDialog(true);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuHelpAbout_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	MessageBox::Show(gcnew System::String(c_CharacterStudioVersion), 
		gcnew System::String("CharacterStudio"), 
		MessageBoxButtons::OK);

	// the about box
	//
	//	the first two digits after the period is the internal "stage" the programmers are working
	//	on.  the number after it is for sequencing.
	//
	//System::Reflection::Assembly^ pSRAss = System::Reflection::Assembly::GetExecutingAssembly();
	//
	//String^ fulltitle	= pSRAss->GetName()->ToString();
	//int found = fulltitle->IndexOf(", ");
	//String^ title = fulltitle->Substring(0, found);
	//
	//String^ msg;
	//msg = msg->Concat(	S"Character Studio version ", 
	//					pSRAss->GetName()->get_Version()->ToString(), 
	//					S"\n\nCopyright (c) 2003-6 Extra Large Technology\n", 
	//					S"All rights reserved\n" );
	//
	//std::string stdMsg;
	//std::string stdTitle;
	//tmaManagedStringUtils::ManagedStringToStdString(title,stdTitle);
	//tmaManagedStringUtils::ManagedStringToStdString(msg,stdMsg);
	//
	//muiMessageBox::Show( stdMsg.c_str(), stdTitle.c_str(), muiMessageBox::e_OKOnly );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::panelRender_Resize(System::Object ^  sender, System::EventArgs ^  e)
{
	System::Drawing::Rectangle rect = this->panelRender->ClientRectangle;
	tma3dScreenUtil::SetWindowSize( maPoint2d( (float)rect.Left, (float)rect.Top ),
		maPoint2d( (float)rect.Width, (float)rect.Height ) );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::panelRender_MouseEnter(System::Object ^  sender, System::EventArgs ^  e)
{
	tma3dCursorMgr::CursorOverViewCallback(true);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::panelRender_MouseLeave(System::Object ^  sender, System::EventArgs ^  e)
{
	tma3dCursorMgr::CursorOverViewCallback(false);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::panelRender_MouseMove(System::Object ^  sender, System::Windows::Forms::MouseEventArgs ^  e)
{
	tma3dCursorMgr::CursorPosChangedCallback( e->X, e->Y );
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuSubdiv_Popup(System::Object ^  sender, System::EventArgs ^  e)
{
	int level = chrLevel::GetCurrentSubdivLevel();
	this->menuSubdivLevel0->Checked = (level == 0);
	this->menuSubdivLevel1->Checked = (level == 1);
	this->menuSubdivLevel2->Checked = (level == 2);
	this->menuSubdivLevel3->Checked = (level == 3);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuSubdivLevel0_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	chrLevel::SetCurrentSubdivLevel(0);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuSubdivLevel1_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	chrLevel::SetCurrentSubdivLevel(1);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuSubdivLevel2_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	chrLevel::SetCurrentSubdivLevel(2);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuSubdivLevel3_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	chrLevel::SetCurrentSubdivLevel(3);
}
