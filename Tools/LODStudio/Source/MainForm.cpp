//
#include "StdAfx.h"
#include "MainForm.h"

#include "lodApp.hpp"
#include "lodDialogUtil.hpp"
#include "lodLevel.hpp"

#include "effShaderArray.hpp"
//#include "fsLocator.hpp"
#include "g3dSingleLightRendering.hpp"

#include "muiFileDialogUtils.hpp"
#include "muiMessageBox.hpp"
#include "tma3dCursorMgr.hpp"
#include "tma3dScreenUtil.hpp"
#include "tmaCustomDocHandler.hpp"
#include "tmaDialogMemory.hpp"
//#include "tmaDialogMemoryMgr.hpp"

// for PeekMessage()
#include <windows.h>


using namespace LODStudio;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
MainForm::MainForm(void)
{
	m_bInIdleCallback = false;
	FormInstance = this;

	InitializeComponent();

	//	system dialog used by all the systems.
	tmaDialogMemoryMgr::Initialize();
	//tmaDialogTabbedMgr::Initialize();

	// MRU File list
	m_pMRUManager = new MRUManager(this->menuItem_file_recent, 5 );

	// Dialog memory remembers size, location, visiblity of dialog
	m_pMemory = new tmaDialogMemory( this );
}

//----------------------------------------------------------------------------
//	OnApplicationIdle - when there are no messages pending, keep doing redraws
//----------------------------------------------------------------------------
System::Void MainForm::OnApplicationIdle(System::Object *  sender, System::EventArgs *  e)
{
	if (m_bInIdleCallback) 
		return;

	m_bInIdleCallback = true;

	MSG msg;
	while (!::PeekMessage(&msg, NULL, 0,0,0))
	{
		if (!lodApp::IsActive())
		{
			m_bInIdleCallback = false;
			return;
		}

		lodApp::ThinkApp();

		render_3dWindow();
	}
	
	m_bInIdleCallback = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuFileNew_Click(System::Object *  sender, System::EventArgs *  e)
{
	tmaCustomDocHandler::New();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuFileOpen_Click(System::Object *  sender, System::EventArgs *  e)
{
	tmaCustomDocHandler::Open();

//	prtLODGeneratorTemplate * pPGTemplate = (lodLODTemplate::GetTemplate());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuFileSave_Click(System::Object *  sender, System::EventArgs *  e)
{
	tmaCustomDocHandler::Save();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuFileSaveAs_Click(System::Object *  sender, System::EventArgs *  e)
{
	tmaCustomDocHandler::SaveAs();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuFileExit_Click(System::Object *  sender, System::EventArgs *  e)
{
	if (tmaCustomDocHandler::SaveIfDirty())
	{
		Application::Exit();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuHelpAbout_Click(System::Object *  sender, System::EventArgs *  e)
{
	//MessageBox::Show(c_LODStudioVersion, "LODStudio", MessageBoxButtons::OK);

	// the about box
	//
	//	the first two digits after the period is the internal "stage" the programmers are working
	//	on.  the number after it is for sequencing.
	//
	System::Reflection::Assembly* pSRAss = System::Reflection::Assembly::GetExecutingAssembly();

	String* fulltitle	= pSRAss->GetName()->ToString();
    int found = fulltitle->IndexOf(", ");
	String* title = fulltitle->Substring(0, found);

	String* msg;
	msg = msg->Concat(	S"Level Of Detail (LOD) Studio version ", 
						pSRAss->GetName()->get_Version()->ToString(), 
						S"\n\nCopyright (c) 2003-6 Extra Large Technology\n", 
						S"All rights reserved\n" );

	std::string stdMsg;
	std::string stdTitle;
	tmaManagedStringUtils::ManagedStringToStdString(title,stdTitle);
	tmaManagedStringUtils::ManagedStringToStdString(msg,stdMsg);

	muiMessageBox::Show( stdMsg.c_str(), stdTitle.c_str(), muiMessageBox::e_OKOnly );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::panelRender_Resize(System::Object *  sender, System::EventArgs *  e)
{
	System::Drawing::Rectangle rect = this->panelRender->get_ClientRectangle();
	tma3dScreenUtil::SetWindowSize( maPoint2d( (float)rect.Left, (float)rect.Top ),
		maPoint2d( (float)rect.get_Width(), (float)rect.get_Height() ) );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::panelRender_MouseEnter(System::Object *  sender, System::EventArgs *  e)
{
	tma3dCursorMgr::CursorOverViewCallback(true);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::panelRender_MouseLeave(System::Object *  sender, System::EventArgs *  e)
{
	tma3dCursorMgr::CursorOverViewCallback(false);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::panelRender_MouseMove(System::Object *  sender, System::Windows::Forms::MouseEventArgs *  e)
{
	tma3dCursorMgr::CursorPosChangedCallback( e->X, e->Y );
}

System::Void MainForm::menu_view_ground_Click(System::Object *  sender, System::EventArgs *  e)
{
	lodLevel::ShowGround( menu_view_ground->Checked );
}
