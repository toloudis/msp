/*****************************************************************************
**  MainForm.cpp
**
**		see .hpp
**
** History:
** 1.1 - Support for subdivision surfaces
** 1.1.3 - Support for triangle sort and double-sided flags
** 1.1.4 - Fixed error that didn't allow changing alpha diffuse color
**		- also has changes for single-light rendering with no shadows for point/dir lights
**		- also parses flags version 2.0 for poly and subdivs
** 1.1.5 - Fixed bug on material selection (again)
** 1.1.6 - pitch and yaw sliders stopped working...now fixed
**		 - moved the version to assemblyInfo like MachStudio
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "StdAfx.h"
#include "MainForm.h"
#include "StaticCubeDialog.h"

#include "docCustomDocumentMgr.hpp"
#include "effShaderArray.hpp"
//#include "fsLocator.hpp"
#include "g3dSingleLightRendering.hpp"
#include "itStringUtil.hpp"
#include "mtrApp.hpp"
#include "mtrDialogUtil.hpp"
#include "mtrLevel.hpp"
#include "muiAppTitleMgr.hpp"
#include "muiFileDialogUtils.hpp"
#include "muiMessageBox.hpp"
#include "muiPackage.hpp"
#include "tmaCustomDocHandler.hpp"
#include "tma3dCursorMgr.hpp"
#include "tma3dScreenUtil.hpp"
#include "tmaSystem.hpp"

// for PeekMessage()
#include <windows.h>

using namespace MatStudio;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
MainForm::MainForm()
{
	m_bInIdleCallback = false;
	FormInstance = this;

	InitializeComponent();

	// MRU File list
	m_pMRUManager = new MRUManager(this->menuItem_file_recent, 5 );

	//	initialize the managed stuff
	//
	tmaSystem::g_pMainForm = this;
	tmaSystem::g_nNumMenuOffset = 2;
	muiPackage::Initialize();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void MainForm::UpdateTitleBar()
{
	char buffer[128];
	std::string sceneFilename;
	docCustomDocumentMgr::GetFilenameOnly(sceneFilename);
	if ( sceneFilename.size() == 0 )
	{
		sceneFilename = "Untitled";
	}

	bool i_bDisplayDirtyFlag = docCustomDocumentMgr::NeedsSave();
	sprintf( buffer, "Material Studio - %s%s",
		sceneFilename.c_str(),
		(i_bDisplayDirtyFlag ? "*":" ") );
	muiAppTitleMgr::SetText( buffer );
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
		if (!mtrApp::IsActive())
		{
			m_bInIdleCallback = false;
			return;
		}

		mtrApp::ThinkApp();

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
	//tmaCustomDocHandler::SetInitialDirectory( dir );

	this->UpdateTitleBar();
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
System::Void MainForm::menuEdit_Popup(System::Object *  sender, System::EventArgs *  e)
{
	this->menuEditPasteMaterial->Enabled = mtrLevel::HaveClipboardData();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuEditCopyMaterial_Click(System::Object *  sender, System::EventArgs *  e)
{
	mtrLevel::CopyMaterial();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuEditPasteMaterial_Click(System::Object *  sender, System::EventArgs *  e)
{
	if ( mtrLevel::HaveClipboardData() )
	{
		mtrLevel::PasteMaterial();
		mtrDialogUtil::UpdateMaterialDialog();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuEditImportMaterials_Click(System::Object *  sender, System::EventArgs *  e)
{
	std::string filter = "Model files (*.*x)|*.*x";
	fsLocator initial_dir; // = mtrLevel::GetTextureDir();
	fsLocator file_loc;
	if (muiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
	{
		mtrLevel::ImportMaterials(file_loc);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuEditReloadTextures_Click(System::Object *  sender, System::EventArgs *  e)
{
	mtrLevel::ReloadTextures();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuEditStaticCube_Click(System::Object *  sender, System::EventArgs *  e)
{
	StaticCubeDialog *dialog = new StaticCubeDialog();
	dialog->ShowDialog();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuView_Popup(System::Object *  sender, System::EventArgs *  e)
{
	this->menuViewShaders->Checked = effShaderArray::GetUseShaderArray();
	this->menuViewShadows->Checked = g3dSingleLightRendering::GetDoSingleLightRendering();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuViewShaders_Click(System::Object *  sender, System::EventArgs *  e)
{
	effShaderArray::SetUseShaderArray(!effShaderArray::GetUseShaderArray());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuViewShadows_Click(System::Object *  sender, System::EventArgs *  e)
{	
	g3dSingleLightRendering::SetDoSingleLightRendering(!g3dSingleLightRendering::GetDoSingleLightRendering());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuViewResetCamera_Click(System::Object *  sender, System::EventArgs *  e)
{
	mtrLevel::FocusCamera();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuWindowLights_Click(System::Object *  sender, System::EventArgs *  e)
{
	mtrDialogUtil::ShowLightsDialog(true);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuHelpAbout_Click(System::Object *  sender, System::EventArgs *  e)
{
	System::Reflection::Assembly* pSRAss = System::Reflection::Assembly::GetExecutingAssembly();

	String* fulltitle	= pSRAss->GetName()->ToString();
    int found = fulltitle->IndexOf(", ");
	String* title = fulltitle->Substring(0, found);

	String* msg;
	msg = msg->Concat(	S"Mat Studio version ", 
						pSRAss->GetName()->get_Version()->ToString(), 
						S"\n\nCopyright (c) Extra Large Technology", 
						S" 2003-6   \n" );

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

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::comboBox_materials_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
{
	if (this->comboBox_materials->SelectedIndex >= 0)
		mtrLevel::SelectMaterial(this->comboBox_materials->SelectedIndex);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::checkHighlight_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
{
	mtrLevel::SetDoHighlight(this->checkHighlight->Checked);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::buttonEditMaterial_Click(System::Object *  sender, System::EventArgs *  e)
{
	mtrDialogUtil::ShowMaterialDialog(true);
}

