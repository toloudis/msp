#include "StdAfx.h"
#include "MainForm.h"
#include "HostnameDialog.h"
#include "SwitchModelDialog.h"

#include "chrLevel.hpp"
#include "dbgLog.hpp"
#include "effShaderArray.hpp"
#include "fsLocator.hpp"
#include "g3dSingleLightRendering.hpp"
#include "mcpApp.hpp"
#include "mcpDialogUtil.hpp"
#include "mcpHTRData.hpp"
#include "mcpRealTimeConnect.hpp"
#include "mcpSkeleton.hpp"
#include "muiFileDialogUtils.hpp"
#include "muiMessageBox.hpp"
#include "tmaCustomDocHandler.hpp"
#include "tmaManagedStringUtils.hpp"
#include "tma3dCursorMgr.hpp"
#include "tma3dScreenUtil.hpp"

using namespace System::Net;
using namespace MotionAnalysisStudio;

namespace
{
	// History:
	// 1.0 - reads HTR files as animation
	// 1.9 - streams real-time data through EVaRT SDK version 1,
	//			but has errors
	// 2.0 - HTR files and streaming fixed. We will have issues in scaling.
	// 2.1 - Adding EVaRT SDK2 support
	// 2.2 - Allow choice between multiple bodies through combo box
	const char* c_MotionAnalysisStudioVersion = "MotionAnalysisStudio Version 2.2";
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::IdentifyVersion()
{
	int version = mcpRealTimeConnect::GetSDKVersion();
	this->statusBar1->Text = System::String::Format("EVaRT SDK Version {0}" , __box(version));
}

//----------------------------------------------------------------------------
//	OnTimer - updates every frame
//----------------------------------------------------------------------------
System::Void MainForm::OnTimer(System::Object *  sender, System::Timers::ElapsedEventArgs *  e)
{
	if (!mcpApp::IsActive())
	{
		this->timer1->Enabled = true;
		return;
	}

	this->timer1->Enabled = false;

	if (mcpRealTimeConnect::HaveNewHierarchy())
	{
		// Put body names into combo box
		const int num_bodies = mcpRealTimeConnect::GetNumBodies();
		int prev_sel = this->comboBox1->SelectedIndex;
		this->comboBox1->Items->Clear();
		for (int i=0; i<num_bodies; ++i)
		{
			std::string name = mcpRealTimeConnect::GetBodyName(i);
			this->comboBox1->Items->Add(new System::String(name.c_str()));

			int num_segs = mcpRealTimeConnect::GetNumSegments(name);
			DBG_LOG1("New hierarchy with %d segments.", num_segs);
		}
		if (num_bodies > 0)
		{
			if (prev_sel >= 0 && prev_sel < num_bodies)
				this->comboBox1->SelectedIndex = prev_sel;
			else
				this->comboBox1->SelectedIndex = 0;
		}
		this->statusBar1->Text = "Have new hierarchy";

		if (chrLevel::HasModel())
		{
			chrLevel::RemoveController();
		}
	}
	if (mcpRealTimeConnect::HaveNewFrameData())
	{
		static int frame_count = 0;
		frame_count++;
		//DBG_LOG1("New frame data %d.", frame_count);
		this->statusBar1->Text = System::String::Format("Have frame data {0}", __box(frame_count) );

		std::vector<mcpHTRSegmentData> segment_data;
		// See if we have a body choice in the combo box
		System::String *combo_text = this->comboBox1->Text;
		if (combo_text->Length > 0)
		{
			std::string body_name;
			tmaManagedStringUtils::ManagedStringToStdString(combo_text, body_name);
			mcpRealTimeConnect::GetSegmentData(body_name, segment_data);
		}
		else
		{
			mcpRealTimeConnect::GetSegmentData(segment_data);
		}

		if (chrLevel::HasModel())
		{
			chrLevel::UpdateHierarchy(segment_data);
		}
		else
		{
			if (!mcpSkeleton::HasModel())
			{
				// Create a new skeleton
				mcpSkeleton::CreateSkeleton(segment_data);
			}
			else
			{
				// update existing skeleton
				mcpSkeleton::UpdateHierarchy(segment_data);
			}
		}
		
	}

	mcpApp::ThinkApp();

	render_3dWindow();

	this->timer1->Enabled = true;
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
System::Void MainForm::menuLoadAnimation_Click(System::Object *  sender, System::EventArgs *  e)
{
	const std::string filter = "Motion Capture Anims (*.htr)|*.htr|Animation files (*.*a)|*.*a|All files (*.*)|*.*";
	fsLocator initial_dir;
	fsLocator file_loc;
	if (muiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
	{
		//itString filename = file_loc.GetLastName();
		//if (chrLevel::HasModel() &&
		//	(filename.HasSubString(itString(".htr")) ||
		//	 filename.HasSubString(itString(".HTR")) ) )
		//{	
		//	// Try to get display of skeleton and model at same time
		//	// in order to debug

		//	// Load HTR model into skeleton and animation
		//	mcpSkeleton::LoadModel( file_loc );

		//	// Load animation into chrLevel
		//	chrLevel::LoadAnimation( file_loc );
		//}
		//else
		//{
			mcpSkeleton::LoadAnimation(file_loc);
		//}
	}
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
System::Void MainForm::menuView_Popup(System::Object *  sender, System::EventArgs *  e)
{
	this->menuViewShaders->Checked = matShaderMgr::GetUseShaderArray();
	this->menuViewShadows->Checked = g3dSingleLightRendering::GetDoSingleLightRendering();
	this->menuViewWireframe->Checked = mcpApp::GetWireframe();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuViewShaders_Click(System::Object *  sender, System::EventArgs *  e)
{
	matShaderMgr::SetUseShaderArray(!matShaderMgr::GetUseShaderArray());
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
	mcpSkeleton::FocusCamera();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuViewWireframe_Click(System::Object *  sender, System::EventArgs *  e)
{
	mcpApp::SetWireframe(!mcpApp::GetWireframe());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuWindowLights_Click(System::Object *  sender, System::EventArgs *  e)
{
	mcpDialogUtil::ShowLightsDialog(true);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuHelpAbout_Click(System::Object *  sender, System::EventArgs *  e)
{
	int version = mcpRealTimeConnect::GetSDKVersion();
	if (version == 2)
	{
		System::String *msg = System::String::Format("{0} \nEVaRT SDK Version {1}\nLocalhost: {2}",
			new System::String(c_MotionAnalysisStudioVersion), __box(version), System::Net::Dns::GetHostName());
		MessageBox::Show(msg, "MotionAnalysisStudio", MessageBoxButtons::OK);
	}
	else
	{
		System::String *msg = System::String::Format("{0} \nEVaRT SDK Version {1}",
			new System::String(c_MotionAnalysisStudioVersion), __box(version));
		MessageBox::Show(msg, "MotionAnalysisStudio", MessageBoxButtons::OK);
	}

	// the about box
	//
	//	the first two digits after the period is the internal "stage" the programmers are working
	//	on.  the number after it is for sequencing.
	//
	//System::Reflection::Assembly* pSRAss = System::Reflection::Assembly::GetExecutingAssembly();
	//
	//String* fulltitle	= pSRAss->GetName()->ToString();
	//int found = fulltitle->IndexOf(", ");
	//String* title = fulltitle->Substring(0, found);
	//
	//String* msg;
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
System::Void MainForm::menuSubdiv_Popup(System::Object *  sender, System::EventArgs *  e)
{
	int level = chrLevel::GetCurrentSubdivLevel();
	this->menuSubdivLevel0->Checked = (level == 0);
	this->menuSubdivLevel1->Checked = (level == 1);
	this->menuSubdivLevel2->Checked = (level == 2);
	this->menuSubdivLevel3->Checked = (level == 3);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuSubdivLevel0_Click(System::Object *  sender, System::EventArgs *  e)
{
	chrLevel::SetCurrentSubdivLevel(0);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuSubdivLevel1_Click(System::Object *  sender, System::EventArgs *  e)
{
	chrLevel::SetCurrentSubdivLevel(1);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuSubdivLevel2_Click(System::Object *  sender, System::EventArgs *  e)
{
	chrLevel::SetCurrentSubdivLevel(2);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuSubdivLevel3_Click(System::Object *  sender, System::EventArgs *  e)
{
	chrLevel::SetCurrentSubdivLevel(3);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuRealTimeConnect_Click(System::Object *  sender, System::EventArgs *  e)
{
	HostnameDialog *dialog = new HostnameDialog();
	dialog->SetScaleFactor( mcpRealTimeConnect::GetScaleFactor() );
	if (dialog->ShowDialog() == DialogResult::OK)
	{
		std::string hostname;
		tmaManagedStringUtils::ManagedStringToStdString(dialog->GetHostname(), hostname);
		if (mcpRealTimeConnect::Connect(hostname.c_str()))
		{
			//MessageBox::Show(this, "Connected to EVaRT", "Connected", MessageBoxButtons::OK);
			this->statusBar1->Text = "Connected.";

			mcpRealTimeConnect::SetScaleFactor( dialog->GetScaleFactor() );

			// Not sure if we should do this here or in the start streaming callback
			mcpRealTimeConnect::RequestHierarchy();
		}
		else
		{
			MessageBox::Show(this, "Could not connect to EVaRT", "No connection", MessageBoxButtons::OK, MessageBoxIcon::Error);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuRealTimeDisconnect_Click(System::Object *  sender, System::EventArgs *  e)
{
	mcpRealTimeConnect::Disconnect();
	this->statusBar1->Text = "Not connected.";
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuRealTime_Popup(System::Object *  sender, System::EventArgs *  e)
{
	if (mcpRealTimeConnect::IsConnected())
	{
		this->menuRealTimeConnect->Enabled = false;
		this->menuRealTimeDisconnect->Enabled = true;

		if (mcpRealTimeConnect::IsStreaming())
		{
			this->menuRealTimeStartStream->Enabled = false;
			this->menuRealTimeStopStream->Enabled = true;
		}
		else
		{
			this->menuRealTimeStartStream->Enabled = true;
			this->menuRealTimeStopStream->Enabled = false;

		}
	}
	else
	{
		this->menuRealTimeConnect->Enabled = true;
		this->menuRealTimeDisconnect->Enabled = false;		
		this->menuRealTimeStartStream->Enabled = false;
		this->menuRealTimeStopStream->Enabled = false;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuRealTimeStartStream_Click(System::Object *  sender, System::EventArgs *  e)
{
	// Should the request for the hierarchy be separate, or earlier?
	//mcpRealTimeConnect::RequestHierarchy();

	// This starts the actual streaming
	mcpRealTimeConnect::RequestFrameData();
	this->statusBar1->Text = "Requesting frame data.";
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::menuRealTimeStopStream_Click(System::Object *  sender, System::EventArgs *  e)
{
	mcpRealTimeConnect::StopStreaming();
	this->statusBar1->Text = "Stopping stream.";
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::comboBox1_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
{
	if (chrLevel::HasModel())
	{
		DBG_LOG0("Clearing out old controller");
		chrLevel::RemoveController();
	}
	if (mcpSkeleton::HasModel())
	{
		DBG_LOG0("Clearing out old skeleton");
		mcpSkeleton::Clear();
	}

	if (mcpRealTimeConnect::IsStreaming())
	{
		DBG_LOG0("Need to restart streaming with new character.");

		// Restart the stream
		mcpRealTimeConnect::StopStreaming();
		mcpRealTimeConnect::RequestFrameData();
	}
}

