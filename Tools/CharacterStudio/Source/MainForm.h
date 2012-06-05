#pragma once

#ifndef MRU_MANAGER_HPP
#include "MRUManager.hpp"
#endif
#ifndef MUI_PACKAGE_HPP
#include "ToolUIManaged/mui/muiPackage.hpp"
#endif
#ifndef TMA_SYSTEM_HPP
#include "ToolUIManaged/tma/tmaSystem.hpp"
#endif

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace CharacterStudio
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
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public: 
		static MainForm ^FormInstance = nullptr;

		MainForm(void)
			: m_bInIdleCallback(false)
		{
			FormInstance = this;

			InitializeComponent();
	
			//	initialize the managed stuff
			//
			tmaSystem::g_pMainForm = this;
			muiPackage::Initialize();

			// MRU File list
			m_pMRUManager = gcnew MRUManager(this->menuItem_file_recent, 5 );
		}
        
		System::Windows::Forms::Panel ^  GetRenderWindow()
		{
			return panelRender;
		}

		System::Windows::Forms::Panel ^  GetInitRenderWindow()
		{
			return panelInit;
		}

		void EnableAnimationUI(bool i_bEnabled)
		{
			this->menuLoadAnimation->Enabled = i_bEnabled;
			this->menuLoadSubAnimation->Enabled = i_bEnabled;
		}

		void EnableSubdivUI(bool i_bEnabled)
		{
			this->menuSubdiv->Visible = i_bEnabled;
		}

	protected: 
		~MainForm()
		{
			// clear instance
			if (MainForm::FormInstance == this)
				MainForm::FormInstance = nullptr;

			muiPackage::DeInitialize();

			if (components)
			{
				delete components;
			}
		}

	private: MRUManager^ m_pMRUManager;				// MRU list manager
	private: bool m_bInIdleCallback;
	private: System::Windows::Forms::MainMenu ^  mainMenu1;
	private: System::Windows::Forms::Panel ^  panelInit;
	private: System::Windows::Forms::MenuItem ^  menuItem1;

	private: System::Windows::Forms::Panel ^  panelRender;

	private: System::Windows::Forms::MenuItem ^  menuFileExit;
	private: System::Windows::Forms::MenuItem ^  menuFileNew;
	private: System::Windows::Forms::MenuItem ^  menuFileOpen;
	private: System::Windows::Forms::MenuItem ^  menuItem2;

	private: System::Windows::Forms::MenuItem ^  menuItem4;
	private: System::Windows::Forms::MenuItem ^  menuItem5;
	private: System::Windows::Forms::MenuItem ^  menuItem_file_recent;




	private: System::Windows::Forms::MenuItem ^  menuFileSave;
	private: System::Windows::Forms::MenuItem ^  menuFileSaveAs;
	private: System::Windows::Forms::MenuItem ^  menuView;
	private: System::Windows::Forms::MenuItem ^  menuViewShaders;
	private: System::Windows::Forms::MenuItem ^  menuViewShadows;
	private: System::Windows::Forms::MenuItem ^  menuItem3;
	private: System::Windows::Forms::MenuItem ^  menuWindowLights;
	private: System::Windows::Forms::MenuItem ^  menuItem6;
	private: System::Windows::Forms::MenuItem ^  menuHelpAbout;
	private: System::Windows::Forms::MenuItem ^  menuViewResetCamera;
	private: System::Windows::Forms::MenuItem ^  menuLoadAnimation;
	private: System::Windows::Forms::MenuItem ^  menuItem8;
	private: System::Windows::Forms::MenuItem ^  menuSubdiv;
	private: System::Windows::Forms::MenuItem ^  menuSubdivLevel0;
	private: System::Windows::Forms::MenuItem ^  menuSubdivLevel1;
	private: System::Windows::Forms::MenuItem ^  menuSubdivLevel2;
	private: System::Windows::Forms::MenuItem ^  menuSubdivLevel3;
	private: System::Windows::Forms::MenuItem ^  menuViewWireframe;
	private: System::Windows::Forms::MenuItem ^  menuLoadSubAnimation;
	private: System::Windows::Forms::MenuItem ^  menuSwitchModel;
	private: System::Windows::Forms::MenuItem ^  menuWindowExpressions;
	private: System::Windows::Forms::MenuItem^  menuViewLowRes;
	private: System::Windows::Forms::MenuItem^  menuViewJoints;
	private: System::Windows::Forms::MenuItem^  menuViewModelOnly;
	private: System::Windows::Forms::MenuItem^  menuViewJointsOnly;

	private: System::Windows::Forms::MenuItem^  menuViewModelAndJoints;





	private: System::ComponentModel::IContainer ^  components;

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
			this->components = (gcnew System::ComponentModel::Container());
			this->mainMenu1 = (gcnew System::Windows::Forms::MainMenu(this->components));
			this->menuItem1 = (gcnew System::Windows::Forms::MenuItem());
			this->menuFileNew = (gcnew System::Windows::Forms::MenuItem());
			this->menuFileOpen = (gcnew System::Windows::Forms::MenuItem());
			this->menuFileSave = (gcnew System::Windows::Forms::MenuItem());
			this->menuFileSaveAs = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem8 = (gcnew System::Windows::Forms::MenuItem());
			this->menuSwitchModel = (gcnew System::Windows::Forms::MenuItem());
			this->menuLoadAnimation = (gcnew System::Windows::Forms::MenuItem());
			this->menuLoadSubAnimation = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem2 = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem_file_recent = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem4 = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem5 = (gcnew System::Windows::Forms::MenuItem());
			this->menuFileExit = (gcnew System::Windows::Forms::MenuItem());
			this->menuView = (gcnew System::Windows::Forms::MenuItem());
			this->menuViewShaders = (gcnew System::Windows::Forms::MenuItem());
			this->menuViewShadows = (gcnew System::Windows::Forms::MenuItem());
			this->menuViewResetCamera = (gcnew System::Windows::Forms::MenuItem());
			this->menuViewWireframe = (gcnew System::Windows::Forms::MenuItem());
			this->menuViewLowRes = (gcnew System::Windows::Forms::MenuItem());
			this->menuViewJoints = (gcnew System::Windows::Forms::MenuItem());
			this->menuViewModelOnly = (gcnew System::Windows::Forms::MenuItem());
			this->menuViewJointsOnly = (gcnew System::Windows::Forms::MenuItem());
			this->menuViewModelAndJoints = (gcnew System::Windows::Forms::MenuItem());
			this->menuSubdiv = (gcnew System::Windows::Forms::MenuItem());
			this->menuSubdivLevel0 = (gcnew System::Windows::Forms::MenuItem());
			this->menuSubdivLevel1 = (gcnew System::Windows::Forms::MenuItem());
			this->menuSubdivLevel2 = (gcnew System::Windows::Forms::MenuItem());
			this->menuSubdivLevel3 = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem3 = (gcnew System::Windows::Forms::MenuItem());
			this->menuWindowLights = (gcnew System::Windows::Forms::MenuItem());
			this->menuWindowExpressions = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem6 = (gcnew System::Windows::Forms::MenuItem());
			this->menuHelpAbout = (gcnew System::Windows::Forms::MenuItem());
			this->panelInit = (gcnew System::Windows::Forms::Panel());
			this->panelRender = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// mainMenu1
			// 
			this->mainMenu1->MenuItems->AddRange(gcnew cli::array<System::Windows::Forms::MenuItem^>(5)
			{ this->menuItem1, this->menuView, this->menuSubdiv, this->menuItem3, this->menuItem6 } );
			// 
			// menuItem1
			// 
			this->menuItem1->Index = 0;
			this->menuItem1->MenuItems->AddRange(gcnew cli::array<System::Windows::Forms::MenuItem^>(12)
			{ this->menuFileNew, this->menuFileOpen, this->menuFileSave, this->menuFileSaveAs,
			  this->menuItem8, this->menuSwitchModel, this->menuLoadAnimation, 
			  this->menuLoadSubAnimation, this->menuItem2, this->menuItem_file_recent, 
			  this->menuItem5, this->menuFileExit});
			this->menuItem1->Text = "File";
			// 
			// menuFileNew
			// 
			this->menuFileNew->Index = 0;
			this->menuFileNew->Text = "New";
			this->menuFileNew->Click += gcnew System::EventHandler(this, &MainForm::menuFileNew_Click);
			// 
			// menuFileOpen
			// 
			this->menuFileOpen->Index = 1;
			this->menuFileOpen->Text = "Open";
			this->menuFileOpen->Click += gcnew System::EventHandler(this, &MainForm::menuFileOpen_Click);
			// 
			// menuFileSave
			// 
			this->menuFileSave->Index = 2;
			this->menuFileSave->Text = "Save";
			this->menuFileSave->Click += gcnew System::EventHandler(this, &MainForm::menuFileSave_Click);
			// 
			// menuFileSaveAs
			// 
			this->menuFileSaveAs->Index = 3;
			this->menuFileSaveAs->Text = "Save as...";
			this->menuFileSaveAs->Click += gcnew System::EventHandler(this, &MainForm::menuFileSaveAs_Click);
			// 
			// menuItem8
			// 
			this->menuItem8->Index = 4;
			this->menuItem8->Text = "-";
			// 
			// menuSwitchModel
			// 
			this->menuSwitchModel->Index = 5;
			this->menuSwitchModel->Text = "Switch Model...";
			this->menuSwitchModel->Click += gcnew System::EventHandler(this, &MainForm::menuSwitchModel_Click);
			// 
			// menuLoadAnimation
			// 
			this->menuLoadAnimation->Enabled = false;
			this->menuLoadAnimation->Index = 6;
			this->menuLoadAnimation->Text = "Load Animation...";
			this->menuLoadAnimation->Click += gcnew System::EventHandler(this, &MainForm::menuLoadAnimation_Click);
			// 
			// menuLoadSubAnimation
			// 
			this->menuLoadSubAnimation->Enabled = false;
			this->menuLoadSubAnimation->Index = 7;
			this->menuLoadSubAnimation->Text = "Load Sub-Animation...";
			this->menuLoadSubAnimation->Click += gcnew System::EventHandler(this, &MainForm::menuLoadSubAnimation_Click);
			// 
			// menuItem2
			// 
			this->menuItem2->Index = 8;
			this->menuItem2->Text = "-";
			// 
			// menuItem_file_recent
			// 
			this->menuItem_file_recent->Index = 9;
			this->menuItem_file_recent->MenuItems->Add( this->menuItem4);
			this->menuItem_file_recent->Text = "Recent Files";
			// 
			// menuItem4
			// 
			this->menuItem4->Index = 0;
			this->menuItem4->Text = "MRU SubMenu";
			// 
			// menuItem5
			// 
			this->menuItem5->Index = 10;
			this->menuItem5->Text = "-";
			// 
			// menuFileExit
			// 
			this->menuFileExit->Index = 11;
			this->menuFileExit->Text = "Exit";
			this->menuFileExit->Click += gcnew System::EventHandler(this, &MainForm::menuFileExit_Click);
			// 
			// menuView
			// 
			this->menuView->Index = 1;
			this->menuView->MenuItems->AddRange(gcnew cli::array<System::Windows::Forms::MenuItem^>(6)
			 {this->menuViewShaders, this->menuViewShadows, this->menuViewResetCamera,
			  this->menuViewWireframe, this->menuViewLowRes, this->menuViewJoints } );
			this->menuView->Text = "View";
			this->menuView->Popup += gcnew System::EventHandler(this, &MainForm::menuView_Popup);
			// 
			// menuViewShaders
			// 
			this->menuViewShaders->Index = 0;
			this->menuViewShaders->Text = "Use Shader Array";
			this->menuViewShaders->Click += gcnew System::EventHandler(this, &MainForm::menuViewShaders_Click);
			// 
			// menuViewShadows
			// 
			this->menuViewShadows->Index = 1;
			this->menuViewShadows->Text = "Shadows";
			this->menuViewShadows->Click += gcnew System::EventHandler(this, &MainForm::menuViewShadows_Click);
			// 
			// menuViewResetCamera
			// 
			this->menuViewResetCamera->Index = 2;
			this->menuViewResetCamera->Text = "Reset Camera";
			this->menuViewResetCamera->Click += gcnew System::EventHandler(this, &MainForm::menuViewResetCamera_Click);
			// 
			// menuViewWireframe
			// 
			this->menuViewWireframe->Index = 3;
			this->menuViewWireframe->Text = "Wireframe";
			this->menuViewWireframe->Click += gcnew System::EventHandler(this, &MainForm::menuViewWireframe_Click);
			// 
			// menuViewLowRes
			// 
			this->menuViewLowRes->Index = 4;
			this->menuViewLowRes->Text = "Low Res Model";
			this->menuViewLowRes->Click += gcnew System::EventHandler(this, &MainForm::menuViewLowRes_Click);
			// 
			// menuViewJoints
			// 
			this->menuViewJoints->Index = 5;
			this->menuViewJoints->MenuItems->Add(this->menuViewModelOnly);
			this->menuViewJoints->MenuItems->Add(this->menuViewJointsOnly);
			this->menuViewJoints->MenuItems->Add(this->menuViewModelAndJoints);
			this->menuViewJoints->Text = "Joints";
			this->menuViewJoints->Popup += gcnew System::EventHandler(this, &MainForm::menuViewJoints_Popup);
			// 
			// menuViewModelOnly
			// 
			this->menuViewModelOnly->Index = 0;
			this->menuViewModelOnly->Tag = "0";
			this->menuViewModelOnly->Text = "Model Only";
			this->menuViewModelOnly->Click += gcnew System::EventHandler(this, &MainForm::menuViewJoints_ChangeDisplay);
			// 
			// menuViewJointsOnly
			// 
			this->menuViewJointsOnly->Index = 1;
			this->menuViewJointsOnly->Tag = "1";
			this->menuViewJointsOnly->Text = "Joints Only";
			this->menuViewJointsOnly->Click += gcnew System::EventHandler(this, &MainForm::menuViewJoints_ChangeDisplay);
			// 
			// menuViewModelAndJoints
			// 
			this->menuViewModelAndJoints->Index = 2;
			this->menuViewModelAndJoints->Tag = "2";
			this->menuViewModelAndJoints->Text = "Model and Joints";
			this->menuViewModelAndJoints->Click += gcnew System::EventHandler(this, &MainForm::menuViewJoints_ChangeDisplay);
			// 
			// menuSubdiv
			// 
			this->menuSubdiv->Index = 2;
			this->menuSubdiv->MenuItems->Add(this->menuSubdivLevel0);
			this->menuSubdiv->MenuItems->Add(this->menuSubdivLevel1);
			this->menuSubdiv->MenuItems->Add(this->menuSubdivLevel2);
			this->menuSubdiv->MenuItems->Add(this->menuSubdivLevel3);
			this->menuSubdiv->Text = "Subdiv";
			this->menuSubdiv->Visible = false;
			this->menuSubdiv->Popup += gcnew System::EventHandler(this, &MainForm::menuSubdiv_Popup);
			// 
			// menuSubdivLevel0
			// 
			this->menuSubdivLevel0->Index = 0;
			this->menuSubdivLevel0->Text = "View Base Mesh";
			this->menuSubdivLevel0->Click += gcnew System::EventHandler(this, &MainForm::menuSubdivLevel0_Click);
			// 
			// menuSubdivLevel1
			// 
			this->menuSubdivLevel1->Index = 1;
			this->menuSubdivLevel1->Text = "View Subdiv Level 1";
			this->menuSubdivLevel1->Click += gcnew System::EventHandler(this, &MainForm::menuSubdivLevel1_Click);
			// 
			// menuSubdivLevel2
			// 
			this->menuSubdivLevel2->Index = 2;
			this->menuSubdivLevel2->Text = "View Subdiv Level 2";
			this->menuSubdivLevel2->Click += gcnew System::EventHandler(this, &MainForm::menuSubdivLevel2_Click);
			// 
			// menuSubdivLevel3
			// 
			this->menuSubdivLevel3->Index = 3;
			this->menuSubdivLevel3->Text = "View Subdiv Level 3";
			this->menuSubdivLevel3->Click += gcnew System::EventHandler(this, &MainForm::menuSubdivLevel3_Click);
			// 
			// menuItem3
			// 
			this->menuItem3->Index = 3;
			this->menuItem3->MenuItems->Add(this->menuWindowLights);
			this->menuItem3->MenuItems->Add(this->menuWindowExpressions);
			this->menuItem3->Text = "Window";
			// 
			// menuWindowLights
			// 
			this->menuWindowLights->Index = 0;
			this->menuWindowLights->Text = "Lights...";
			this->menuWindowLights->Click += gcnew System::EventHandler(this, &MainForm::menuWindowLights_Click);
			// 
			// menuWindowExpressions
			// 
			this->menuWindowExpressions->Index = 1;
			this->menuWindowExpressions->Text = "Expressions...";
			this->menuWindowExpressions->Click += gcnew System::EventHandler(this, &MainForm::menuWindowExpressions_Click);
			// 
			// menuItem6
			// 
			this->menuItem6->Index = 4;
			this->menuItem6->MenuItems->Add(this->menuHelpAbout);
			this->menuItem6->Text = "Help";
			// 
			// menuHelpAbout
			// 
			this->menuHelpAbout->Index = 0;
			this->menuHelpAbout->Text = "About...";
			this->menuHelpAbout->Click += gcnew System::EventHandler(this, &MainForm::menuHelpAbout_Click);
			// 
			// panelInit
			// 
			this->panelInit->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->panelInit->Location = System::Drawing::Point(7, 0);
			this->panelInit->Name = "panelInit";
			this->panelInit->Size = System::Drawing::Size(26, 29);
			this->panelInit->TabIndex = 14;
			this->panelInit->Visible = false;
			// 
			// panelRender
			// 
			this->panelRender->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->panelRender->BackColor = System::Drawing::Color::Black;
			this->panelRender->Location = System::Drawing::Point(7, 0);
			this->panelRender->Name = "panelRender";
			this->panelRender->Size = System::Drawing::Size(666, 520);
			this->panelRender->TabIndex = 15;
			this->panelRender->MouseLeave += gcnew System::EventHandler(this, &MainForm::panelRender_MouseLeave);
			this->panelRender->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::panelRender_MouseMove);
			this->panelRender->Resize += gcnew System::EventHandler(this, &MainForm::panelRender_Resize);
			this->panelRender->MouseEnter += gcnew System::EventHandler(this, &MainForm::panelRender_MouseEnter);
			// 
			// MainForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(679, 531);
			this->Controls->Add(this->panelRender);
			this->Controls->Add(this->panelInit);
			this->Menu = this->mainMenu1;
			this->Name = "MainForm";
			this->Text = "Character Studio";
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			this->ResumeLayout(false);

		}		
		//

	private: System::Void MainForm_Load(System::Object^  sender, System::EventArgs^  e);
	private: System::Void OnApplicationIdle(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuFileNew_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuFileOpen_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuFileSave_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuFileSaveAs_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuFileExit_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuView_Popup(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuViewShaders_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuViewShadows_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_Resize(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_MouseEnter(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_MouseLeave(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_MouseMove(System::Object ^  sender, System::Windows::Forms::MouseEventArgs ^  e);
	private: System::Void menuWindowLights_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuHelpAbout_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuViewResetCamera_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuViewWireframe_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuViewLowRes_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void menuViewJoints_Popup(System::Object^  sender, System::EventArgs^  e);
	private: System::Void menuViewJoints_ChangeDisplay(System::Object^  sender, System::EventArgs^  e);
	private: System::Void menuSwitchModel_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuLoadAnimation_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuLoadSubAnimation_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuSubdiv_Popup(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuSubdivLevel0_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuSubdivLevel1_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuSubdivLevel2_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuSubdivLevel3_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuWindowExpressions_Click(System::Object ^  sender, System::EventArgs ^  e);
};
}