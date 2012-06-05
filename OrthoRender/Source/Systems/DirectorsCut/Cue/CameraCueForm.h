#pragma once

#ifndef DCUT_SCRIPTDATA_HPP
#include "Systems/DirectorsCut/Data/dcutScriptData.hpp"
#endif
#ifndef DCUT_CUEDATAUTIL_HPP
#include "Systems/DirectorsCut/Cue/dcutCueDataUtil.hpp"
#endif
#ifndef DCUT_OBJECTMGR_HPP
#include "Systems/DirectorsCut/Object/dcutObjectMgr.hpp"
#endif
#ifndef DCUT_VIEWER_HPP
#include "Systems/DirectorsCut/Cue/dcutViewer.hpp"
#endif
#ifndef RCD_RECORDUTIL_HPP
#include "Record/rcdRecordUtil.hpp"
#endif
#ifndef TMLN_TIMELINE_HPP
#include "Support/tmln/tmlnTimeline.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifndef TMA3D_VIEWERMGR_HPP
#include "Tool/tma3d/tma3dViewerMgr.hpp"
#endif

#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace SystemCameras
{
	/// <summary> 
	/// Summary for CameraCueForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CameraCueForm : public System::Windows::Forms::Form
	{
	public: 
		static CameraCueForm ^FormInstance = nullptr;

		CameraCueForm(float i_AspectRatio) 
			: m_bRecording(false), m_CameraIndex(0), m_EndTime(tmlnTimeLine::GetMaximum())
		{
			m_pViewer = new dcutViewer*[4];
			for (int i=0; i<4; i++)
				m_pViewer[i] = NULL;

			m_bDisableNotify = true;
			InitializeComponent();

			if (i_AspectRatio >= 4.0/3.0)
				SetViewerSize(128, (int)(128 / i_AspectRatio));	// width defines size
			else
				SetViewerSize((int)(96 * i_AspectRatio), 96);	// height defines size
			

			this->Update(tmlnTimeLine::GetValue());
			m_bDisableNotify = false;
		}

		void Update(float i_Time)
		{
			this->textCurTime->Text = String::Format("{0}", (i_Time));

			if (m_bRecording)
			{
				dcutCueDataUtil::AddCue(i_Time, m_CameraIndex);
				if (i_Time >= m_EndTime )
				{
					m_bRecording = false;
					this->checkRecord->Checked = false;
				}
			}
			else
			{
				int ind = dcutCueDataUtil::GetCameraForTime(i_Time);
				this->HighlightCamera(ind);
			}
		}

		void CreateViewers(g2dSystem &i_System)
		{
			m_pViewer[0] = new dcutViewer(i_System, (void*)this->renderView1->Handle);
			tma3dViewerMgr::AddViewer(m_pViewer[0]);
			m_pViewer[1] = new dcutViewer(i_System, (void*)this->renderView2->Handle);
			tma3dViewerMgr::AddViewer(m_pViewer[1]);
			m_pViewer[2] = new dcutViewer(i_System, (void*)this->renderView3->Handle);
			tma3dViewerMgr::AddViewer(m_pViewer[2]);
			m_pViewer[3] = new dcutViewer(i_System, (void*)this->renderView4->Handle);
			tma3dViewerMgr::AddViewer(m_pViewer[3]);
		}

		void SetupViewers(const dcutCueFormData& i_Data)
		{
			m_bDisableNotify = true;
			SetupViewer(comboCamera1, i_Data.m_CameraNames, i_Data.m_Index[0]);
			if (m_pViewer[0])	m_pViewer[0]->SetCameraIndex(i_Data.m_Index[0]);
			SetupViewer(comboCamera2, i_Data.m_CameraNames, i_Data.m_Index[1]);
			if (m_pViewer[1])	m_pViewer[1]->SetCameraIndex(i_Data.m_Index[1]);
			SetupViewer(comboCamera3, i_Data.m_CameraNames, i_Data.m_Index[2]);
			if (m_pViewer[2])	m_pViewer[2]->SetCameraIndex(i_Data.m_Index[2]);
			SetupViewer(comboCamera4, i_Data.m_CameraNames, i_Data.m_Index[3]);
			if (m_pViewer[3])	m_pViewer[3]->SetCameraIndex(i_Data.m_Index[3]);
			m_bDisableNotify = false;
		}
        
	protected: 
		~CameraCueForm()
		{
			if (this == FormInstance)
				FormInstance = nullptr;

			for (int i=0; i<4; i++)
			{
				tma3dViewerMgr::RemoveViewer(m_pViewer[i]);
				delete m_pViewer[i];
				m_pViewer[i] = NULL;
			}
			delete [] m_pViewer;

			if (components)
			{
				delete components;
			}
		}

	private:
		bool m_bDisableNotify;
		bool m_bRecording;
		dcutViewer** m_pViewer/*[4]*/;
		int m_CameraIndex;
		float m_EndTime;
	private: System::Windows::Forms::PictureBox ^  renderView1;
	private: System::Windows::Forms::ComboBox ^  comboCamera1;


	private: System::Windows::Forms::CheckBox ^  checkRecord;
	private: System::Windows::Forms::Label ^  label1;
	private: System::Windows::Forms::TextBox ^  textBeginTime;
	private: System::Windows::Forms::TextBox ^  textEndTime;
	private: System::Windows::Forms::Label ^  label2;
	private: System::Windows::Forms::Panel ^  panelTimeline;
	private: System::Windows::Forms::Label ^  label3;
	private: TimelineControls::TimeLabel ^  timeLabel1;
	private: System::Windows::Forms::Label ^  label4;


	private: System::Windows::Forms::Panel ^  panelHighlight1;
	private: System::Windows::Forms::TextBox ^  textCurTime;
	private: System::Windows::Forms::Panel ^  panelHighlight2;
	private: System::Windows::Forms::ComboBox ^  comboCamera2;
	private: System::Windows::Forms::PictureBox ^  renderView2;
	private: System::Windows::Forms::Panel ^  panelHighlight4;
	private: System::Windows::Forms::ComboBox ^  comboCamera4;
	private: System::Windows::Forms::PictureBox ^  renderView4;
	private: System::Windows::Forms::Panel ^  panelHighlight3;
	private: System::Windows::Forms::ComboBox ^  comboCamera3;
	private: System::Windows::Forms::PictureBox ^  renderView3;
	private: System::Windows::Forms::Button ^  buttonResetViews;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->renderView1 = gcnew System::Windows::Forms::PictureBox();
			this->comboCamera1 = gcnew System::Windows::Forms::ComboBox();
			this->checkRecord = gcnew System::Windows::Forms::CheckBox();
			this->label1 = gcnew System::Windows::Forms::Label();
			this->textBeginTime = gcnew System::Windows::Forms::TextBox();
			this->textEndTime = gcnew System::Windows::Forms::TextBox();
			this->label2 = gcnew System::Windows::Forms::Label();
			this->panelTimeline = gcnew System::Windows::Forms::Panel();
			this->timeLabel1 = gcnew TimelineControls::TimeLabel();
			this->label3 = gcnew System::Windows::Forms::Label();
			this->label4 = gcnew System::Windows::Forms::Label();
			this->textCurTime = gcnew System::Windows::Forms::TextBox();
			this->panelHighlight1 = gcnew System::Windows::Forms::Panel();
			this->panelHighlight2 = gcnew System::Windows::Forms::Panel();
			this->comboCamera2 = gcnew System::Windows::Forms::ComboBox();
			this->renderView2 = gcnew System::Windows::Forms::PictureBox();
			this->panelHighlight4 = gcnew System::Windows::Forms::Panel();
			this->comboCamera4 = gcnew System::Windows::Forms::ComboBox();
			this->renderView4 = gcnew System::Windows::Forms::PictureBox();
			this->panelHighlight3 = gcnew System::Windows::Forms::Panel();
			this->comboCamera3 = gcnew System::Windows::Forms::ComboBox();
			this->renderView3 = gcnew System::Windows::Forms::PictureBox();
			this->buttonResetViews = gcnew System::Windows::Forms::Button();
			this->panelTimeline->SuspendLayout();
			this->panelHighlight1->SuspendLayout();
			this->panelHighlight2->SuspendLayout();
			this->panelHighlight4->SuspendLayout();
			this->panelHighlight3->SuspendLayout();
			this->SuspendLayout();
			// 
			// renderView1
			// 
			this->renderView1->BackColor = System::Drawing::SystemColors::Desktop;
			this->renderView1->Location = System::Drawing::Point(7, 35);
			this->renderView1->Name = "renderView1";
			this->renderView1->Size = System::Drawing::Size(106, 83);
			this->renderView1->TabIndex = 0;
			this->renderView1->TabStop = false;
			this->renderView1->Click += gcnew System::EventHandler(this, &CameraCueForm::renderView1_Click);
			// 
			// comboCamera1
			// 
			this->comboCamera1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboCamera1->Location = System::Drawing::Point(7, 7);
			this->comboCamera1->Name = "comboCamera1";
			this->comboCamera1->Size = System::Drawing::Size(106, 21);
			this->comboCamera1->TabIndex = 1;
			this->comboCamera1->SelectedIndexChanged += gcnew System::EventHandler(this, &CameraCueForm::comboCamera_SelectedIndexChanged);
			// 
			// checkRecord
			// 
			this->checkRecord->Location = System::Drawing::Point(180, 180);
			this->checkRecord->Name = "checkRecord";
			this->checkRecord->Size = System::Drawing::Size(67, 28);
			this->checkRecord->TabIndex = 8;
			this->checkRecord->Text = "Record";
			this->checkRecord->CheckedChanged += gcnew System::EventHandler(this, &CameraCueForm::checkRecord_CheckedChanged);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(260, 187);
			this->label1->Name = "label1";
			this->label1->Size = System::Drawing::Size(73, 21);
			this->label1->TabIndex = 9;
			this->label1->Text = "Begin Time:";
			// 
			// textBeginTime
			// 
			this->textBeginTime->Location = System::Drawing::Point(340, 187);
			this->textBeginTime->Name = "textBeginTime";
			this->textBeginTime->Size = System::Drawing::Size(53, 20);
			this->textBeginTime->TabIndex = 10;
			this->textBeginTime->Text = "";
			// 
			// textEndTime
			// 
			this->textEndTime->Location = System::Drawing::Point(480, 187);
			this->textEndTime->Name = "textEndTime";
			this->textEndTime->Size = System::Drawing::Size(53, 20);
			this->textEndTime->TabIndex = 12;
			this->textEndTime->Text = "";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(413, 187);
			this->label2->Name = "label2";
			this->label2->Size = System::Drawing::Size(60, 21);
			this->label2->TabIndex = 11;
			this->label2->Text = "End Time:";
			// 
			// panelTimeline
			// 
			this->panelTimeline->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->panelTimeline->AutoScroll = true;
			this->panelTimeline->Controls->Add(this->timeLabel1);
			this->panelTimeline->Controls->Add(this->label3);
			this->panelTimeline->Location = System::Drawing::Point(27, 222);
			this->panelTimeline->Name = "panelTimeline";
			this->panelTimeline->Size = System::Drawing::Size(520, 111);
			this->panelTimeline->TabIndex = 13;
			// 
			// timeLabel1
			// 
			this->timeLabel1->Location = System::Drawing::Point(7, 49);
			this->timeLabel1->Name = "timeLabel1";
			this->timeLabel1->Size = System::Drawing::Size(466, 27);
			this->timeLabel1->TabIndex = 1;
			this->timeLabel1->TabStop = false;
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(7, 7);
			this->label3->Name = "label3";
			this->label3->Size = System::Drawing::Size(180, 28);
			this->label3->TabIndex = 0;
			this->label3->Text = "Timeline display will go here";
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(13, 187);
			this->label4->Name = "label4";
			this->label4->Size = System::Drawing::Size(74, 21);
			this->label4->TabIndex = 14;
			this->label4->Text = "Current Time:";
			// 
			// textCurTime
			// 
			this->textCurTime->Location = System::Drawing::Point(93, 187);
			this->textCurTime->Name = "textCurTime";
			this->textCurTime->ReadOnly = true;
			this->textCurTime->Size = System::Drawing::Size(54, 20);
			this->textCurTime->TabIndex = 15;
			this->textCurTime->Text = "";
			// 
			// panelHighlight1
			// 
			this->panelHighlight1->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->panelHighlight1->Controls->Add(this->comboCamera1);
			this->panelHighlight1->Controls->Add(this->renderView1);
			this->panelHighlight1->Location = System::Drawing::Point(20, 49);
			this->panelHighlight1->Name = "panelHighlight1";
			this->panelHighlight1->Size = System::Drawing::Size(120, 124);
			this->panelHighlight1->TabIndex = 2;
			// 
			// panelHighlight2
			// 
			this->panelHighlight2->BackColor = System::Drawing::SystemColors::Control;
			this->panelHighlight2->Controls->Add(this->comboCamera2);
			this->panelHighlight2->Controls->Add(this->renderView2);
			this->panelHighlight2->Location = System::Drawing::Point(153, 49);
			this->panelHighlight2->Name = "panelHighlight2";
			this->panelHighlight2->Size = System::Drawing::Size(120, 124);
			this->panelHighlight2->TabIndex = 16;
			// 
			// comboCamera2
			// 
			this->comboCamera2->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboCamera2->Location = System::Drawing::Point(7, 7);
			this->comboCamera2->Name = "comboCamera2";
			this->comboCamera2->Size = System::Drawing::Size(106, 21);
			this->comboCamera2->TabIndex = 1;
			this->comboCamera2->SelectedIndexChanged += gcnew System::EventHandler(this, &CameraCueForm::comboCamera_SelectedIndexChanged);
			// 
			// renderView2
			// 
			this->renderView2->BackColor = System::Drawing::SystemColors::Desktop;
			this->renderView2->Location = System::Drawing::Point(7, 35);
			this->renderView2->Name = "renderView2";
			this->renderView2->Size = System::Drawing::Size(106, 83);
			this->renderView2->TabIndex = 0;
			this->renderView2->TabStop = false;
			this->renderView2->Click += gcnew System::EventHandler(this, &CameraCueForm::renderView2_Click);
			// 
			// panelHighlight4
			// 
			this->panelHighlight4->BackColor = System::Drawing::SystemColors::Control;
			this->panelHighlight4->Controls->Add(this->comboCamera4);
			this->panelHighlight4->Controls->Add(this->renderView4);
			this->panelHighlight4->Location = System::Drawing::Point(420, 49);
			this->panelHighlight4->Name = "panelHighlight4";
			this->panelHighlight4->Size = System::Drawing::Size(120, 124);
			this->panelHighlight4->TabIndex = 18;
			// 
			// comboCamera4
			// 
			this->comboCamera4->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboCamera4->Location = System::Drawing::Point(7, 7);
			this->comboCamera4->Name = "comboCamera4";
			this->comboCamera4->Size = System::Drawing::Size(106, 21);
			this->comboCamera4->TabIndex = 1;
			this->comboCamera4->SelectedIndexChanged += gcnew System::EventHandler(this, &CameraCueForm::comboCamera_SelectedIndexChanged);
			// 
			// renderView4
			// 
			this->renderView4->BackColor = System::Drawing::SystemColors::Desktop;
			this->renderView4->Location = System::Drawing::Point(7, 35);
			this->renderView4->Name = "renderView4";
			this->renderView4->Size = System::Drawing::Size(106, 83);
			this->renderView4->TabIndex = 0;
			this->renderView4->TabStop = false;
			this->renderView4->Click += gcnew System::EventHandler(this, &CameraCueForm::renderView4_Click);
			// 
			// panelHighlight3
			// 
			this->panelHighlight3->BackColor = System::Drawing::SystemColors::Control;
			this->panelHighlight3->Controls->Add(this->comboCamera3);
			this->panelHighlight3->Controls->Add(this->renderView3);
			this->panelHighlight3->Location = System::Drawing::Point(287, 49);
			this->panelHighlight3->Name = "panelHighlight3";
			this->panelHighlight3->Size = System::Drawing::Size(120, 124);
			this->panelHighlight3->TabIndex = 17;
			// 
			// comboCamera3
			// 
			this->comboCamera3->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboCamera3->Location = System::Drawing::Point(7, 7);
			this->comboCamera3->Name = "comboCamera3";
			this->comboCamera3->Size = System::Drawing::Size(106, 21);
			this->comboCamera3->TabIndex = 1;
			this->comboCamera3->SelectedIndexChanged += gcnew System::EventHandler(this, &CameraCueForm::comboCamera_SelectedIndexChanged);
			// 
			// renderView3
			// 
			this->renderView3->BackColor = System::Drawing::SystemColors::Desktop;
			this->renderView3->Location = System::Drawing::Point(7, 35);
			this->renderView3->Name = "renderView3";
			this->renderView3->Size = System::Drawing::Size(106, 83);
			this->renderView3->TabIndex = 0;
			this->renderView3->TabStop = false;
			this->renderView3->Click += gcnew System::EventHandler(this, &CameraCueForm::renderView3_Click);
			// 
			// buttonResetViews
			// 
			this->buttonResetViews->Location = System::Drawing::Point(13, 7);
			this->buttonResetViews->Name = "buttonResetViews";
			this->buttonResetViews->Size = System::Drawing::Size(134, 21);
			this->buttonResetViews->TabIndex = 19;
			this->buttonResetViews->Text = "Reset Views";
			this->buttonResetViews->Click += gcnew System::EventHandler(this, &CameraCueForm::buttonResetViews_Click);
			// 
			// CameraCueForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(579, 345);
			this->Controls->Add(this->buttonResetViews);
			this->Controls->Add(this->panelHighlight4);
			this->Controls->Add(this->panelHighlight3);
			this->Controls->Add(this->panelHighlight2);
			this->Controls->Add(this->textCurTime);
			this->Controls->Add(this->textEndTime);
			this->Controls->Add(this->textBeginTime);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->panelTimeline);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->checkRecord);
			this->Controls->Add(this->panelHighlight1);
			this->Name = "CameraCueForm";
			this->ShowInTaskbar = false;
			this->Text = "Camera Cue Recording";
			this->panelTimeline->ResumeLayout(false);
			this->panelHighlight1->ResumeLayout(false);
			this->panelHighlight2->ResumeLayout(false);
			this->panelHighlight4->ResumeLayout(false);
			this->panelHighlight3->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

		void SetViewerSize(int width, int height)
		{
			this->renderView1->Size = System::Drawing::Size(width, height);
			this->renderView2->Size = System::Drawing::Size(width, height);
			this->renderView3->Size = System::Drawing::Size(width, height);
			this->renderView4->Size = System::Drawing::Size(width, height);
		}

		void SetupViewer(System::Windows::Forms::ComboBox ^  comboCamera,
			const std::vector<nameString>& i_CameraNames, int i_Index)
		{
			comboCamera->Items->Clear();
			int num = i_CameraNames.size();
			for (int i=0; i<num; i++)
			{
				comboCamera->Items->Add( gcnew System::String(i_CameraNames[i].GetString().c_str()) );
			}
			if (i_Index>=0 && i_Index<comboCamera1->Items->Count)
			{
				comboCamera->SelectedIndex = i_Index;
			}
		}

		System::Drawing::Color Highlight(bool i_bVal)
		{
			return (i_bVal) ? System::Drawing::SystemColors::ActiveCaption : 
					System::Drawing::SystemColors::Control;
		}

		void HighlightCamera(int i_CameraIndex)
		{
			m_CameraIndex = i_CameraIndex;
			this->panelHighlight1->BackColor = Highlight(dcutCueDataUtil::GetViewingIndex(0) == i_CameraIndex);
			this->panelHighlight2->BackColor = Highlight(dcutCueDataUtil::GetViewingIndex(1) == i_CameraIndex);
			this->panelHighlight3->BackColor = Highlight(dcutCueDataUtil::GetViewingIndex(2) == i_CameraIndex);
			this->panelHighlight4->BackColor = Highlight(dcutCueDataUtil::GetViewingIndex(3) == i_CameraIndex);
		}
		
	private: System::Void comboCamera_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 if (!m_bDisableNotify)
				 {
					dcutCueDataUtil::SetViewingIndex(0, this->comboCamera1->SelectedIndex);
					if (m_pViewer[0])	m_pViewer[0]->SetCameraIndex(this->comboCamera1->SelectedIndex);
					dcutCueDataUtil::SetViewingIndex(1, this->comboCamera2->SelectedIndex);
					if (m_pViewer[1])	m_pViewer[1]->SetCameraIndex(this->comboCamera2->SelectedIndex);
					dcutCueDataUtil::SetViewingIndex(2, this->comboCamera3->SelectedIndex);
					if (m_pViewer[2])	m_pViewer[2]->SetCameraIndex(this->comboCamera3->SelectedIndex);
					dcutCueDataUtil::SetViewingIndex(3, this->comboCamera4->SelectedIndex);
					if (m_pViewer[3])	m_pViewer[3]->SetCameraIndex(this->comboCamera4->SelectedIndex);
				}
			 }

private: System::Void buttonResetViews_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 // reset views so that first camera is in first window, etc.
			 int num_cameras = dcutObjectMgr::GetNumObjects();
			 if (num_cameras > 4) num_cameras = 4;
			 for (int i=0; i<num_cameras; i++)
			 {					
				 dcutCueDataUtil::SetViewingIndex(i, i);
			 }
			 for (int i=num_cameras; i<4; i++)
			 {					
				 dcutCueDataUtil::SetViewingIndex(i, -1);
			 }
			 this->SetupViewers(dcutCueDataUtil::GetCurrentData());
			 this->Update(tmlnTimeLine::GetValue());
			 this->Invalidate();
		 }

private: System::Void renderView1_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 int camera_index = dcutCueDataUtil::GetViewingIndex(0);
			 if (camera_index >= 0)
			 {
				dcutCueDataUtil::AddCue(tmlnTimeLine::GetValue(), camera_index);
				this->HighlightCamera(camera_index);
			 }
		 }

private: System::Void renderView2_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 int camera_index = dcutCueDataUtil::GetViewingIndex(1);
			 if (camera_index >= 0)
			 {
				dcutCueDataUtil::AddCue(tmlnTimeLine::GetValue(), camera_index);
				this->HighlightCamera(camera_index);
			 }
		 }

private: System::Void renderView3_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 int camera_index = dcutCueDataUtil::GetViewingIndex(2);
			 if (camera_index >= 0)
			 {
				dcutCueDataUtil::AddCue(tmlnTimeLine::GetValue(), camera_index);
				this->HighlightCamera(camera_index);
			 }
		 }

private: System::Void renderView4_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 int camera_index = dcutCueDataUtil::GetViewingIndex(3);
			 if (camera_index >= 0)
			 {
				dcutCueDataUtil::AddCue(tmlnTimeLine::GetValue(), camera_index);
				this->HighlightCamera(camera_index);
			 }
		 }

private: System::Void checkRecord_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 this->m_bRecording = this->checkRecord->Checked;
			 if (this->m_bRecording)
			 {
				 float min_time = tmlnTimeLine::GetMinimum();
				 float max_time = tmlnTimeLine::GetMaximum();
				 if (this->textBeginTime->Text->Length > 0)
					 tmaManagedStringUtils::ManagedStringToFloat(textBeginTime->Text, min_time);
				 if (this->textEndTime->Text->Length > 0)
					 tmaManagedStringUtils::ManagedStringToFloat(textEndTime->Text, max_time);

				 if (min_time < max_time)
				 {
					rcdRecordUtil::StartRecording(min_time, max_time);
					m_EndTime = max_time;
				 }
				 else
				 {
					 MessageBox::Show("Begin time must be less than end time");
				 }
			 }
			 else
				 rcdRecordUtil::StopRecording();
		 }

};
}
#endif // _MANAGED
