#pragma once

#ifndef MRU_MANAGER_HPP
#include "MRUManager.hpp"
#endif
#ifndef MUI_PACKAGE_HPP
#include "ToolUIManaged/mui/muiPackage.hpp"
#endif


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


//
ref class tmaDialogMemory;


namespace ParticleStudio
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

		MainForm(void);
        
		System::Windows::Forms::Panel ^  GetRenderWindow()
		{
			return panelRender;
		}

		System::Windows::Forms::Panel ^  GetInitRenderWindow()
		{
			return panelInit;
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
	private: tmaDialogMemory^ m_pMemory;			// Dialog memory remembers size, location, visiblity of dialog 

	private: System::Windows::Forms::Panel ^  panelInit;
	private: System::Windows::Forms::Panel ^  panelRender;

	private: System::Windows::Forms::MainMenu ^  mainMenu1;
	private: System::Windows::Forms::MenuItem ^  menuItem_file;
	private: System::Windows::Forms::MenuItem ^  menuFileExit;
	private: System::Windows::Forms::MenuItem ^  menuFileNew;
	private: System::Windows::Forms::MenuItem ^  menuFileOpen;
	private: System::Windows::Forms::MenuItem ^  menuFileSave;
	private: System::Windows::Forms::MenuItem ^  menuFileSaveAs;
	private: System::Windows::Forms::MenuItem ^  menuItem_seperator1;
	private: System::Windows::Forms::MenuItem ^  menuItem_seperator2;
	private: System::Windows::Forms::MenuItem ^  menuItem_MRUsubmenu;
	private: System::Windows::Forms::MenuItem ^  menuItem_file_recent;
	private: System::Windows::Forms::MenuItem ^  menuItem_help;
	private: System::Windows::Forms::MenuItem ^  menuHelpAbout;
	private: System::Windows::Forms::MenuItem ^  menuItem_view;
	private: System::Windows::Forms::MenuItem ^  menu_view_ground;
	private: System::Windows::Forms::CheckBox ^  checkBox_pause;
	private: System::ComponentModel::IContainer ^  components;
	private: System::Windows::Forms::MenuItem^  menuFileAnimation;
	private: System::Windows::Forms::MenuItem^  menuItem2;
	private: System::Windows::Forms::MenuItem^  menu_focusCamera;

	private: bool m_bInIdleCallback;

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
			this->menuItem_file = (gcnew System::Windows::Forms::MenuItem());
			this->menuFileNew = (gcnew System::Windows::Forms::MenuItem());
			this->menuFileOpen = (gcnew System::Windows::Forms::MenuItem());
			this->menuFileSave = (gcnew System::Windows::Forms::MenuItem());
			this->menuFileSaveAs = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem_seperator1 = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem_file_recent = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem_MRUsubmenu = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem_seperator2 = (gcnew System::Windows::Forms::MenuItem());
			this->menuFileAnimation = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem2 = (gcnew System::Windows::Forms::MenuItem());
			this->menuFileExit = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem_view = (gcnew System::Windows::Forms::MenuItem());
			this->menu_view_ground = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem_help = (gcnew System::Windows::Forms::MenuItem());
			this->menuHelpAbout = (gcnew System::Windows::Forms::MenuItem());
			this->panelInit = (gcnew System::Windows::Forms::Panel());
			this->panelRender = (gcnew System::Windows::Forms::Panel());
			this->checkBox_pause = (gcnew System::Windows::Forms::CheckBox());
			this->menu_focusCamera = (gcnew System::Windows::Forms::MenuItem());
			this->SuspendLayout();
			// 
			// mainMenu1
			// 
			this->mainMenu1->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(3) {this->menuItem_file, this->menuItem_view, 
				this->menuItem_help});
			// 
			// menuItem_file
			// 
			this->menuItem_file->Index = 0;
			this->menuItem_file->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(10) {this->menuFileNew, this->menuFileOpen, 
				this->menuFileSave, this->menuFileSaveAs, this->menuItem_seperator1, this->menuItem_file_recent, this->menuItem_seperator2, this->menuFileAnimation, 
				this->menuItem2, this->menuFileExit});
			this->menuItem_file->Text = L"File";
			// 
			// menuFileNew
			// 
			this->menuFileNew->Enabled = false;
			this->menuFileNew->Index = 0;
			this->menuFileNew->Text = L"New";
			this->menuFileNew->Click += gcnew System::EventHandler(this, &MainForm::menuFileNew_Click);
			// 
			// menuFileOpen
			// 
			this->menuFileOpen->Enabled = false;
			this->menuFileOpen->Index = 1;
			this->menuFileOpen->Text = L"Open";
			this->menuFileOpen->Click += gcnew System::EventHandler(this, &MainForm::menuFileOpen_Click);
			// 
			// menuFileSave
			// 
			this->menuFileSave->Enabled = false;
			this->menuFileSave->Index = 2;
			this->menuFileSave->Text = L"Save";
			this->menuFileSave->Click += gcnew System::EventHandler(this, &MainForm::menuFileSave_Click);
			// 
			// menuFileSaveAs
			// 
			this->menuFileSaveAs->Enabled = false;
			this->menuFileSaveAs->Index = 3;
			this->menuFileSaveAs->Text = L"Save as...";
			this->menuFileSaveAs->Click += gcnew System::EventHandler(this, &MainForm::menuFileSaveAs_Click);
			// 
			// menuItem_seperator1
			// 
			this->menuItem_seperator1->Index = 4;
			this->menuItem_seperator1->Text = L"-";
			// 
			// menuItem_file_recent
			// 
			this->menuItem_file_recent->Enabled = false;
			this->menuItem_file_recent->Index = 5;
			this->menuItem_file_recent->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(1) {this->menuItem_MRUsubmenu});
			this->menuItem_file_recent->Text = L"Recent Files";
			// 
			// menuItem_MRUsubmenu
			// 
			this->menuItem_MRUsubmenu->Index = 0;
			this->menuItem_MRUsubmenu->Text = L"MRU SubMenu";
			// 
			// menuItem_seperator2
			// 
			this->menuItem_seperator2->Index = 6;
			this->menuItem_seperator2->Text = L"-";
			// 
			// menuFileAnimation
			// 
			this->menuFileAnimation->Index = 7;
			this->menuFileAnimation->Text = L"Load Animation ...";
			this->menuFileAnimation->Click += gcnew System::EventHandler(this, &MainForm::menuFileAnimation_Click);
			// 
			// menuItem2
			// 
			this->menuItem2->Index = 8;
			this->menuItem2->Text = L"-";
			// 
			// menuFileExit
			// 
			this->menuFileExit->Index = 9;
			this->menuFileExit->Text = L"Exit";
			this->menuFileExit->Click += gcnew System::EventHandler(this, &MainForm::menuFileExit_Click);
			// 
			// menuItem_view
			// 
			this->menuItem_view->Index = 1;
			this->menuItem_view->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(2) {this->menu_view_ground, 
				this->menu_focusCamera});
			this->menuItem_view->Text = L"View";
			// 
			// menu_view_ground
			// 
			this->menu_view_ground->Checked = true;
			this->menu_view_ground->Index = 0;
			this->menu_view_ground->Text = L"View Ground";
			this->menu_view_ground->Click += gcnew System::EventHandler(this, &MainForm::menu_view_ground_Click);
			// 
			// menuItem_help
			// 
			this->menuItem_help->Index = 2;
			this->menuItem_help->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(1) {this->menuHelpAbout});
			this->menuItem_help->Text = L"Help";
			// 
			// menuHelpAbout
			// 
			this->menuHelpAbout->Index = 0;
			this->menuHelpAbout->Text = L"About...";
			this->menuHelpAbout->Click += gcnew System::EventHandler(this, &MainForm::menuHelpAbout_Click);
			// 
			// panelInit
			// 
			this->panelInit->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->panelInit->Location = System::Drawing::Point(80, 0);
			this->panelInit->Name = L"panelInit";
			this->panelInit->Size = System::Drawing::Size(26, 29);
			this->panelInit->TabIndex = 14;
			this->panelInit->Visible = false;
			// 
			// panelRender
			// 
			this->panelRender->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->panelRender->BackColor = System::Drawing::Color::Black;
			this->panelRender->Location = System::Drawing::Point(7, 35);
			this->panelRender->Name = L"panelRender";
			this->panelRender->Size = System::Drawing::Size(666, 520);
			this->panelRender->TabIndex = 15;
			this->panelRender->MouseLeave += gcnew System::EventHandler(this, &MainForm::panelRender_MouseLeave);
			this->panelRender->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::panelRender_MouseMove);
			this->panelRender->Resize += gcnew System::EventHandler(this, &MainForm::panelRender_Resize);
			this->panelRender->MouseEnter += gcnew System::EventHandler(this, &MainForm::panelRender_MouseEnter);
			// 
			// checkBox_pause
			// 
			this->checkBox_pause->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox_pause->Location = System::Drawing::Point(8, 5);
			this->checkBox_pause->Name = L"checkBox_pause";
			this->checkBox_pause->Size = System::Drawing::Size(64, 24);
			this->checkBox_pause->TabIndex = 17;
			this->checkBox_pause->Text = L"Pause";
			this->checkBox_pause->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBox_pause->CheckedChanged += gcnew System::EventHandler(this, &MainForm::checkBox_pause_CheckedChanged);
			// 
			// menu_focusCamera
			// 
			this->menu_focusCamera->Index = 1;
			this->menu_focusCamera->Text = L"Focus Camera";
			this->menu_focusCamera->Click += gcnew System::EventHandler(this, &MainForm::menu_focusCamera_Click);
			// 
			// MainForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(679, 565);
			this->Controls->Add(this->checkBox_pause);
			this->Controls->Add(this->panelRender);
			this->Controls->Add(this->panelInit);
			this->Menu = this->mainMenu1;
			this->Name = L"MainForm";
			this->Text = L"Particle Studio";
			this->Closing += gcnew System::ComponentModel::CancelEventHandler(this, &MainForm::MainForm_Closing);
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			this->ResumeLayout(false);

		}		
		//

	private: System::Void OnApplicationIdle(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuFileNew_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuFileOpen_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuFileSave_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuFileSaveAs_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuFileAnimation_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void menuFileExit_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_Resize(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_MouseEnter(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_MouseLeave(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_MouseMove(System::Object ^  sender, System::Windows::Forms::MouseEventArgs ^  e);
	private: System::Void menuHelpAbout_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menu_view_ground_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menu_focusCamera_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void MainForm_Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e);
	private: System::Void MainForm_Load(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void checkBox_pause_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e);
};
}
