#include "StdAfx.h"
#include "MainForm.h"

#include "mnmApp.hpp"
#include "Tool/gui/guiCustomDocHandler.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"

#ifdef _MANAGED

using namespace SimpleViewer;

//----------------------------------------------------------------------------
//	OnTimer - updates every frame
//----------------------------------------------------------------------------
System::Void MainForm::OnTimer(System::Object ^  sender, System::Timers::ElapsedEventArgs ^  e)
{
	if (!mnmApp::IsActive())
	{
		this->timer1->Enabled = true;
		return;
	}

	this->timer1->Enabled = false;

	mnmApp::ThinkApp();

	render_3dWindow();

	this->timer1->Enabled = true;
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
System::Void MainForm::menuFileExit_Click(System::Object ^  sender, System::EventArgs ^  e)
{	
	if (guiCustomDocHandler::SaveIfDirty())
	{
		Application::Exit();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::panelRender_Resize(System::Object ^  sender, System::EventArgs ^  e)
{
	System::Drawing::Rectangle rect = this->panelRender->ClientRectangle;
	tma3dScreenUtil::SetWindowSize( maPoint2d( rect.Left, rect.Top ),
		maPoint2d( rect.Width, rect.Height ) );
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
#endif // _MANAGED
