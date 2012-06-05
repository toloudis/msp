#pragma once

#ifndef MRU_MANAGER_HPP
#include "MRUManager.hpp"
#endif

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace MotionAnalysisStudio
{
	/// <summary> 
	/// Summary for MainForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class MainForm : public System::Windows::Forms::Form
	{
	public: 
		static MainForm *FormInstance = 0;

		MainForm(void)
		{
			FormInstance = this;

			InitializeComponent();

			// MRU File list
			m_pMRUManager = new MRUManager(this->menuItem_file_recent, 5 );

			this->IdentifyVersion();
		}

		System::Void IdentifyVersion();
        
		System::Windows::Forms::Panel *  GetRenderWindow()
		{
			return panelRender;
		}

		System::Windows::Forms::Panel *  GetInitRenderWindow()
		{
			return panelInit;
		}

		void EnableUI(bool i_bEnabled)
		{
			//this->buttonEditMaterial->Enabled = i_bEnabled;
		}

	protected: 
		void Dispose(Boolean disposing)
		{
			// clear instance
			if (disposing && MainForm::FormInstance == this)
				MainForm::FormInstance = 0;

			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}

	private: MRUManager* m_pMRUManager;				// MRU list manager
	private: System::Windows::Forms::MainMenu *  mainMenu1;
	private: System::Windows::Forms::Panel *  panelInit;
	private: System::Windows::Forms::MenuItem *  menuItem1;

	private: System::Windows::Forms::Panel *  panelRender;
	private: System::Timers::Timer *  timer1;
	private: System::Windows::Forms::MenuItem *  menuFileExit;
	private: System::Windows::Forms::MenuItem *  menuFileNew;
	private: System::Windows::Forms::MenuItem *  menuFileOpen;
	private: System::Windows::Forms::MenuItem *  menuItem2;

	private: System::Windows::Forms::MenuItem *  menuItem4;
	private: System::Windows::Forms::MenuItem *  menuItem5;
	private: System::Windows::Forms::MenuItem *  menuItem_file_recent;




	private: System::Windows::Forms::MenuItem *  menuFileSave;
	private: System::Windows::Forms::MenuItem *  menuFileSaveAs;
	private: System::Windows::Forms::MenuItem *  menuView;
	private: System::Windows::Forms::MenuItem *  menuViewShaders;
	private: System::Windows::Forms::MenuItem *  menuViewShadows;
	private: System::Windows::Forms::MenuItem *  menuItem3;
	private: System::Windows::Forms::MenuItem *  menuWindowLights;
	private: System::Windows::Forms::MenuItem *  menuItem6;
	private: System::Windows::Forms::MenuItem *  menuHelpAbout;
	private: System::Windows::Forms::MenuItem *  menuViewResetCamera;
	private: System::Windows::Forms::MenuItem *  menuLoadAnimation;
	private: System::Windows::Forms::MenuItem *  menuItem8;
	private: System::Windows::Forms::MenuItem *  menuSubdiv;
	private: System::Windows::Forms::MenuItem *  menuSubdivLevel0;
	private: System::Windows::Forms::MenuItem *  menuSubdivLevel1;
	private: System::Windows::Forms::MenuItem *  menuSubdivLevel2;
	private: System::Windows::Forms::MenuItem *  menuSubdivLevel3;
	private: System::Windows::Forms::MenuItem *  menuViewWireframe;
	private: System::Windows::Forms::MenuItem *  menuRealTime;
	private: System::Windows::Forms::MenuItem *  menuRealTimeConnect;
	private: System::Windows::Forms::MenuItem *  menuRealTimeDisconnect;
	private: System::Windows::Forms::StatusBar *  statusBar1;
	private: System::Windows::Forms::MenuItem *  menuItem7;
	private: System::Windows::Forms::MenuItem *  menuRealTimeStartStream;
	private: System::Windows::Forms::MenuItem *  menuRealTimeStopStream;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::ComboBox *  comboBox1;
















	private: System::ComponentModel::IContainer *  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->mainMenu1 = new System::Windows::Forms::MainMenu();
			this->menuItem1 = new System::Windows::Forms::MenuItem();
			this->menuFileNew = new System::Windows::Forms::MenuItem();
			this->menuFileOpen = new System::Windows::Forms::MenuItem();
			this->menuFileSave = new System::Windows::Forms::MenuItem();
			this->menuFileSaveAs = new System::Windows::Forms::MenuItem();
			this->menuItem8 = new System::Windows::Forms::MenuItem();
			this->menuLoadAnimation = new System::Windows::Forms::MenuItem();
			this->menuItem2 = new System::Windows::Forms::MenuItem();
			this->menuItem_file_recent = new System::Windows::Forms::MenuItem();
			this->menuItem4 = new System::Windows::Forms::MenuItem();
			this->menuItem5 = new System::Windows::Forms::MenuItem();
			this->menuFileExit = new System::Windows::Forms::MenuItem();
			this->menuRealTime = new System::Windows::Forms::MenuItem();
			this->menuRealTimeConnect = new System::Windows::Forms::MenuItem();
			this->menuRealTimeDisconnect = new System::Windows::Forms::MenuItem();
			this->menuItem7 = new System::Windows::Forms::MenuItem();
			this->menuRealTimeStartStream = new System::Windows::Forms::MenuItem();
			this->menuRealTimeStopStream = new System::Windows::Forms::MenuItem();
			this->menuView = new System::Windows::Forms::MenuItem();
			this->menuViewShaders = new System::Windows::Forms::MenuItem();
			this->menuViewShadows = new System::Windows::Forms::MenuItem();
			this->menuViewResetCamera = new System::Windows::Forms::MenuItem();
			this->menuViewWireframe = new System::Windows::Forms::MenuItem();
			this->menuSubdiv = new System::Windows::Forms::MenuItem();
			this->menuSubdivLevel0 = new System::Windows::Forms::MenuItem();
			this->menuSubdivLevel1 = new System::Windows::Forms::MenuItem();
			this->menuSubdivLevel2 = new System::Windows::Forms::MenuItem();
			this->menuSubdivLevel3 = new System::Windows::Forms::MenuItem();
			this->menuItem3 = new System::Windows::Forms::MenuItem();
			this->menuWindowLights = new System::Windows::Forms::MenuItem();
			this->menuItem6 = new System::Windows::Forms::MenuItem();
			this->menuHelpAbout = new System::Windows::Forms::MenuItem();
			this->panelInit = new System::Windows::Forms::Panel();
			this->panelRender = new System::Windows::Forms::Panel();
			this->statusBar1 = new System::Windows::Forms::StatusBar();
			this->timer1 = new System::Timers::Timer();
			this->label1 = new System::Windows::Forms::Label();
			this->comboBox1 = new System::Windows::Forms::ComboBox();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->timer1))->BeginInit();
			this->SuspendLayout();
			// 
			// mainMenu1
			// 
			System::Windows::Forms::MenuItem* __mcTemp__1[] = new System::Windows::Forms::MenuItem*[6];
			__mcTemp__1[0] = this->menuItem1;
			__mcTemp__1[1] = this->menuRealTime;
			__mcTemp__1[2] = this->menuView;
			__mcTemp__1[3] = this->menuSubdiv;
			__mcTemp__1[4] = this->menuItem3;
			__mcTemp__1[5] = this->menuItem6;
			this->mainMenu1->MenuItems->AddRange(__mcTemp__1);
			// 
			// menuItem1
			// 
			this->menuItem1->Index = 0;
			System::Windows::Forms::MenuItem* __mcTemp__2[] = new System::Windows::Forms::MenuItem*[10];
			__mcTemp__2[0] = this->menuFileNew;
			__mcTemp__2[1] = this->menuFileOpen;
			__mcTemp__2[2] = this->menuFileSave;
			__mcTemp__2[3] = this->menuFileSaveAs;
			__mcTemp__2[4] = this->menuItem8;
			__mcTemp__2[5] = this->menuLoadAnimation;
			__mcTemp__2[6] = this->menuItem2;
			__mcTemp__2[7] = this->menuItem_file_recent;
			__mcTemp__2[8] = this->menuItem5;
			__mcTemp__2[9] = this->menuFileExit;
			this->menuItem1->MenuItems->AddRange(__mcTemp__2);
			this->menuItem1->Text = S"File";
			// 
			// menuFileNew
			// 
			this->menuFileNew->Index = 0;
			this->menuFileNew->Text = S"New";
			this->menuFileNew->Click += new System::EventHandler(this, &MainForm::menuFileNew_Click);
			// 
			// menuFileOpen
			// 
			this->menuFileOpen->Index = 1;
			this->menuFileOpen->Text = S"Open";
			this->menuFileOpen->Click += new System::EventHandler(this, &MainForm::menuFileOpen_Click);
			// 
			// menuFileSave
			// 
			this->menuFileSave->Enabled = false;
			this->menuFileSave->Index = 2;
			this->menuFileSave->Text = S"Save";
			this->menuFileSave->Click += new System::EventHandler(this, &MainForm::menuFileSave_Click);
			// 
			// menuFileSaveAs
			// 
			this->menuFileSaveAs->Enabled = false;
			this->menuFileSaveAs->Index = 3;
			this->menuFileSaveAs->Text = S"Save as...";
			this->menuFileSaveAs->Click += new System::EventHandler(this, &MainForm::menuFileSaveAs_Click);
			// 
			// menuItem8
			// 
			this->menuItem8->Index = 4;
			this->menuItem8->Text = S"-";
			// 
			// menuLoadAnimation
			// 
			this->menuLoadAnimation->Index = 5;
			this->menuLoadAnimation->Text = S"Load Animation...";
			this->menuLoadAnimation->Click += new System::EventHandler(this, &MainForm::menuLoadAnimation_Click);
			// 
			// menuItem2
			// 
			this->menuItem2->Index = 6;
			this->menuItem2->Text = S"-";
			// 
			// menuItem_file_recent
			// 
			this->menuItem_file_recent->Index = 7;
			System::Windows::Forms::MenuItem* __mcTemp__3[] = new System::Windows::Forms::MenuItem*[1];
			__mcTemp__3[0] = this->menuItem4;
			this->menuItem_file_recent->MenuItems->AddRange(__mcTemp__3);
			this->menuItem_file_recent->Text = S"Recent Files";
			// 
			// menuItem4
			// 
			this->menuItem4->Index = 0;
			this->menuItem4->Text = S"MRU SubMenu";
			// 
			// menuItem5
			// 
			this->menuItem5->Index = 8;
			this->menuItem5->Text = S"-";
			// 
			// menuFileExit
			// 
			this->menuFileExit->Index = 9;
			this->menuFileExit->Text = S"Exit";
			this->menuFileExit->Click += new System::EventHandler(this, &MainForm::menuFileExit_Click);
			// 
			// menuRealTime
			// 
			this->menuRealTime->Index = 1;
			System::Windows::Forms::MenuItem* __mcTemp__4[] = new System::Windows::Forms::MenuItem*[5];
			__mcTemp__4[0] = this->menuRealTimeConnect;
			__mcTemp__4[1] = this->menuRealTimeDisconnect;
			__mcTemp__4[2] = this->menuItem7;
			__mcTemp__4[3] = this->menuRealTimeStartStream;
			__mcTemp__4[4] = this->menuRealTimeStopStream;
			this->menuRealTime->MenuItems->AddRange(__mcTemp__4);
			this->menuRealTime->Text = S"Real Time";
			this->menuRealTime->Popup += new System::EventHandler(this, &MainForm::menuRealTime_Popup);
			// 
			// menuRealTimeConnect
			// 
			this->menuRealTimeConnect->Index = 0;
			this->menuRealTimeConnect->Text = S"Connect...";
			this->menuRealTimeConnect->Click += new System::EventHandler(this, &MainForm::menuRealTimeConnect_Click);
			// 
			// menuRealTimeDisconnect
			// 
			this->menuRealTimeDisconnect->Index = 1;
			this->menuRealTimeDisconnect->Text = S"Disconnect";
			this->menuRealTimeDisconnect->Click += new System::EventHandler(this, &MainForm::menuRealTimeDisconnect_Click);
			// 
			// menuItem7
			// 
			this->menuItem7->Index = 2;
			this->menuItem7->Text = S"-";
			// 
			// menuRealTimeStartStream
			// 
			this->menuRealTimeStartStream->Index = 3;
			this->menuRealTimeStartStream->Text = S"Start Streaming";
			this->menuRealTimeStartStream->Click += new System::EventHandler(this, &MainForm::menuRealTimeStartStream_Click);
			// 
			// menuRealTimeStopStream
			// 
			this->menuRealTimeStopStream->Index = 4;
			this->menuRealTimeStopStream->Text = S"Stop Streaming";
			this->menuRealTimeStopStream->Click += new System::EventHandler(this, &MainForm::menuRealTimeStopStream_Click);
			// 
			// menuView
			// 
			this->menuView->Index = 2;
			System::Windows::Forms::MenuItem* __mcTemp__5[] = new System::Windows::Forms::MenuItem*[4];
			__mcTemp__5[0] = this->menuViewShaders;
			__mcTemp__5[1] = this->menuViewShadows;
			__mcTemp__5[2] = this->menuViewResetCamera;
			__mcTemp__5[3] = this->menuViewWireframe;
			this->menuView->MenuItems->AddRange(__mcTemp__5);
			this->menuView->Text = S"View";
			this->menuView->Popup += new System::EventHandler(this, &MainForm::menuView_Popup);
			// 
			// menuViewShaders
			// 
			this->menuViewShaders->Index = 0;
			this->menuViewShaders->Text = S"Use Shader Array";
			this->menuViewShaders->Click += new System::EventHandler(this, &MainForm::menuViewShaders_Click);
			// 
			// menuViewShadows
			// 
			this->menuViewShadows->Index = 1;
			this->menuViewShadows->Text = S"Shadows";
			this->menuViewShadows->Click += new System::EventHandler(this, &MainForm::menuViewShadows_Click);
			// 
			// menuViewResetCamera
			// 
			this->menuViewResetCamera->Index = 2;
			this->menuViewResetCamera->Text = S"Reset Camera";
			this->menuViewResetCamera->Click += new System::EventHandler(this, &MainForm::menuViewResetCamera_Click);
			// 
			// menuViewWireframe
			// 
			this->menuViewWireframe->Index = 3;
			this->menuViewWireframe->Text = S"Wireframe";
			this->menuViewWireframe->Click += new System::EventHandler(this, &MainForm::menuViewWireframe_Click);
			// 
			// menuSubdiv
			// 
			this->menuSubdiv->Index = 3;
			System::Windows::Forms::MenuItem* __mcTemp__6[] = new System::Windows::Forms::MenuItem*[4];
			__mcTemp__6[0] = this->menuSubdivLevel0;
			__mcTemp__6[1] = this->menuSubdivLevel1;
			__mcTemp__6[2] = this->menuSubdivLevel2;
			__mcTemp__6[3] = this->menuSubdivLevel3;
			this->menuSubdiv->MenuItems->AddRange(__mcTemp__6);
			this->menuSubdiv->Text = S"Subdiv";
			this->menuSubdiv->Popup += new System::EventHandler(this, &MainForm::menuSubdiv_Popup);
			// 
			// menuSubdivLevel0
			// 
			this->menuSubdivLevel0->Index = 0;
			this->menuSubdivLevel0->Text = S"View Base Mesh";
			this->menuSubdivLevel0->Click += new System::EventHandler(this, &MainForm::menuSubdivLevel0_Click);
			// 
			// menuSubdivLevel1
			// 
			this->menuSubdivLevel1->Index = 1;
			this->menuSubdivLevel1->Text = S"View Subdiv Level 1";
			this->menuSubdivLevel1->Click += new System::EventHandler(this, &MainForm::menuSubdivLevel1_Click);
			// 
			// menuSubdivLevel2
			// 
			this->menuSubdivLevel2->Index = 2;
			this->menuSubdivLevel2->Text = S"View Subdiv Level 2";
			this->menuSubdivLevel2->Click += new System::EventHandler(this, &MainForm::menuSubdivLevel2_Click);
			// 
			// menuSubdivLevel3
			// 
			this->menuSubdivLevel3->Index = 3;
			this->menuSubdivLevel3->Text = S"View Subdiv Level 3";
			this->menuSubdivLevel3->Click += new System::EventHandler(this, &MainForm::menuSubdivLevel3_Click);
			// 
			// menuItem3
			// 
			this->menuItem3->Index = 4;
			System::Windows::Forms::MenuItem* __mcTemp__7[] = new System::Windows::Forms::MenuItem*[1];
			__mcTemp__7[0] = this->menuWindowLights;
			this->menuItem3->MenuItems->AddRange(__mcTemp__7);
			this->menuItem3->Text = S"Window";
			// 
			// menuWindowLights
			// 
			this->menuWindowLights->Index = 0;
			this->menuWindowLights->Text = S"Lights...";
			this->menuWindowLights->Click += new System::EventHandler(this, &MainForm::menuWindowLights_Click);
			// 
			// menuItem6
			// 
			this->menuItem6->Index = 5;
			System::Windows::Forms::MenuItem* __mcTemp__8[] = new System::Windows::Forms::MenuItem*[1];
			__mcTemp__8[0] = this->menuHelpAbout;
			this->menuItem6->MenuItems->AddRange(__mcTemp__8);
			this->menuItem6->Text = S"Help";
			// 
			// menuHelpAbout
			// 
			this->menuHelpAbout->Index = 0;
			this->menuHelpAbout->Text = S"About...";
			this->menuHelpAbout->Click += new System::EventHandler(this, &MainForm::menuHelpAbout_Click);
			// 
			// panelInit
			// 
			this->panelInit->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->panelInit->Location = System::Drawing::Point(32, 48);
			this->panelInit->Name = S"panelInit";
			this->panelInit->Size = System::Drawing::Size(26, 29);
			this->panelInit->TabIndex = 14;
			this->panelInit->Visible = false;
			// 
			// panelRender
			// 
			this->panelRender->BackColor = System::Drawing::Color::Black;
			this->panelRender->Location = System::Drawing::Point(8, 40);
			this->panelRender->Name = S"panelRender";
			this->panelRender->Size = System::Drawing::Size(640, 480);
			this->panelRender->TabIndex = 15;
			this->panelRender->Resize += new System::EventHandler(this, &MainForm::panelRender_Resize);
			this->panelRender->MouseEnter += new System::EventHandler(this, &MainForm::panelRender_MouseEnter);
			this->panelRender->MouseMove += new System::Windows::Forms::MouseEventHandler(this, &MainForm::panelRender_MouseMove);
			this->panelRender->MouseLeave += new System::EventHandler(this, &MainForm::panelRender_MouseLeave);
			// 
			// statusBar1
			// 
			this->statusBar1->Location = System::Drawing::Point(0, 523);
			this->statusBar1->Name = S"statusBar1";
			this->statusBar1->Size = System::Drawing::Size(656, 22);
			this->statusBar1->TabIndex = 0;
			this->statusBar1->Text = S"Not connected";
			// 
			// timer1
			// 
			this->timer1->AutoReset = false;
			this->timer1->Enabled = true;
			this->timer1->Interval = 20;
			this->timer1->SynchronizingObject = this;
			this->timer1->Elapsed += new System::Timers::ElapsedEventHandler(this, &MainForm::OnTimer);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(16, 8);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(136, 16);
			this->label1->TabIndex = 16;
			this->label1->Text = S"Live mode body choice:";
			// 
			// comboBox1
			// 
			this->comboBox1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox1->Location = System::Drawing::Point(168, 8);
			this->comboBox1->Name = S"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(192, 21);
			this->comboBox1->TabIndex = 17;
			this->comboBox1->SelectedIndexChanged += new System::EventHandler(this, &MainForm::comboBox1_SelectedIndexChanged);
			// 
			// MainForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(656, 545);
			this->Controls->Add(this->comboBox1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->panelRender);
			this->Controls->Add(this->statusBar1);
			this->Controls->Add(this->panelInit);
			this->Menu = this->mainMenu1;
			this->Name = S"MainForm";
			this->Text = S"Motion Analysis Studio";
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->timer1))->EndInit();
			this->ResumeLayout(false);

		}		
		//

	private: System::Void OnTimer(System::Object *  sender, System::Timers::ElapsedEventArgs *  e);
	private: System::Void menuFileNew_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuFileOpen_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuFileSave_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuFileSaveAs_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuFileExit_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuView_Popup(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuViewShaders_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuViewShadows_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void panelRender_Resize(System::Object *  sender, System::EventArgs *  e);
	private: System::Void panelRender_MouseEnter(System::Object *  sender, System::EventArgs *  e);
	private: System::Void panelRender_MouseLeave(System::Object *  sender, System::EventArgs *  e);
	private: System::Void panelRender_MouseMove(System::Object *  sender, System::Windows::Forms::MouseEventArgs *  e);
	private: System::Void menuWindowLights_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuHelpAbout_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuViewResetCamera_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuViewWireframe_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuLoadAnimation_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuSubdiv_Popup(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuSubdivLevel0_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuSubdivLevel1_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuSubdivLevel2_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuSubdivLevel3_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuRealTimeConnect_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuRealTimeDisconnect_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuRealTime_Popup(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuRealTimeStartStream_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuRealTimeStopStream_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void comboBox1_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e);

};
}