//
#include "StdAfx.h"
#include "MainForm.h"

#include "ptclApp.hpp"
#include "ptclDialogUtil.hpp"
#include "ptclLevel.hpp"

#include "GraphicsDX9/eff/effShaderArray.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"

#include "Tool/cma/cmaPackage.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"
#include "Tool/gui/guiCustomDocHandler.hpp"
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
//#include "ToolUIManaged/tma/tmaDialogMemoryMgr.hpp"
#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"

// for PeekMessage()
#include <windows.h>


//============================================================================
//============================================================================
using namespace ParticleStudio;


//============================================================================
//============================================================================
MainForm::MainForm(void)
{
	m_bInIdleCallback = false;
	FormInstance = this;

	InitializeComponent();

	// Set-up the application idle event handler
	//
	System::Windows::Forms::Application::Idle += gcnew System::EventHandler(this, &MainForm::OnApplicationIdle);				

	// MRU File list
	m_pMRUManager = gcnew MRUManager(this->menuItem_file_recent, 5 );

	// command package
	cmaPackage::Init();

	tmaSystem::g_pMainForm = this;
	muiPackage::Initialize();

	//	system dialog used by all the systems.
	tmaDialogMemoryMgr::Initialize();
	tmaDialogTabbedMgr::Initialize();

	// Dialog memory remembers size, location, visiblity of dialog 
	//m_pMemory = new tmaDialogMemory( this );
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
		if (!ptclApp::IsActive())
		{
			m_bInIdleCallback = false;
			return;
		}

		ptclApp::ThinkApp();

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
	ptclApp::SetActive( false );

	guiCustomDocHandler::Open();

//	prtParticleGeneratorTemplate * pPGTemplate = (ptclParticleTemplate::GetTemplate());
	ptclApp::SetActive( true );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuFileSave_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	ptclApp::SetActive( false );

	guiCustomDocHandler::Save();

	ptclApp::SetActive( true );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuFileSaveAs_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	ptclApp::SetActive( false );

	guiCustomDocHandler::SaveAs();

	ptclApp::SetActive( true );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuFileAnimation_Click(System::Object^  sender, System::EventArgs^  e) 
{
	ptclApp::SetActive( false );

	const std::string filter = "Particle Animation (*.pta)|*.pta|All files (*.*)|*.*";
	fsLocator initial_dir;
	fsLocator file_loc;
	if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
	{
		ptclLevel::LoadAnimation(file_loc);
	}

	ptclApp::SetActive( true );
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
System::Void MainForm::menuHelpAbout_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	//MessageBox::Show(c_ParticleStudioVersion, "ParticleStudio", MessageBoxButtons::OK);

	// the about box
	//
	//	the first two digits after the period is the internal "stage" the programmers are working
	//	on.  the number after it is for sequencing.
	//
	System::Reflection::Assembly^ pSRAss = System::Reflection::Assembly::GetExecutingAssembly();

	String^ fulltitle	= pSRAss->GetName()->ToString();
    int found = fulltitle->IndexOf(", ");
	String^ title = fulltitle->Substring(0, found);

	String^ msg;
	msg = msg->Concat(	"Particle Studio version ", 
						pSRAss->GetName()->Version->ToString(), 
						"\n\nCopyright (c) 2003-5 Extra Large Technology\n", 
						"All rights reserved\n" );

	std::string stdMsg;
	std::string stdTitle;
	tmaManagedStringUtils::ManagedStringToStdString(title,stdTitle);
	tmaManagedStringUtils::ManagedStringToStdString(msg,stdMsg);

	guiMessageBox::Show( stdMsg.c_str(), stdTitle.c_str(), guiMessageBox::e_OKOnly );
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
System::Void MainForm::menu_view_ground_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	ptclLevel::ShowGround( !ptclLevel::IsShowGround() );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menu_focusCamera_Click(System::Object^  sender, System::EventArgs^  e) 
{
	ptclLevel::FocusCamera();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::MainForm_Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e)
{
	ptclApp::SetActive( false );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::MainForm_Load(System::Object ^  sender, System::EventArgs ^  e)
{
	// Dialog memory remembers size, location, visiblity of dialog 
	m_pMemory = gcnew tmaDialogMemory( this );
}

System::Void MainForm::checkBox_pause_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	//ptclApp::SetActive( !checkBox_pause->Checked );
	ptclApp::SetTimePaused( checkBox_pause->Checked );

	if (checkBox_pause->Checked)
	{
		checkBox_pause->Text = "Resume";
	}
	else
	{
		checkBox_pause->Text = "Pause";
	}
}
