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

#ifdef _MANAGED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace SimpleViewer
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
		MainForm(void)
		{
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

	protected: 
		~MainForm()
		{
			muiPackage::DeInitialize();

			if (components)
			{
				delete components;
			}
		}

	private: MRUManager^ m_pMRUManager;				// MRU list manager
	private: System::Windows::Forms::MainMenu ^  mainMenu1;
	private: System::Windows::Forms::Panel ^  panelInit;
	private: System::Windows::Forms::MenuItem ^  menuItem1;

	private: System::Windows::Forms::Panel ^  panelRender;
	private: System::Timers::Timer ^  timer1;
	private: System::Windows::Forms::MenuItem ^  menuFileExit;
	private: System::Windows::Forms::MenuItem ^  menuFileNew;
	private: System::Windows::Forms::MenuItem ^  menuFileOpen;
	private: System::Windows::Forms::MenuItem ^  menuItem2;

	private: System::Windows::Forms::MenuItem ^  menuItem4;
	private: System::Windows::Forms::MenuItem ^  menuItem5;
	private: System::Windows::Forms::MenuItem ^  menuItem_file_recent;


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
			this->menuItem2 = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem_file_recent = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem4 = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem5 = (gcnew System::Windows::Forms::MenuItem());
			this->menuFileExit = (gcnew System::Windows::Forms::MenuItem());
			this->panelInit = (gcnew System::Windows::Forms::Panel());
			this->panelRender = (gcnew System::Windows::Forms::Panel());
			this->timer1 = (gcnew System::Timers::Timer());
			(safe_cast<System::ComponentModel::ISupportInitialize^  >(this->timer1))->BeginInit();
			this->SuspendLayout();
			// 
			// mainMenu1
			// 
			this->mainMenu1->MenuItems->Add(this->menuItem1);
			// 
			// menuItem1
			// 
			this->menuItem1->Index = 0;
			this->menuItem1->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(6) 
				{	this->menuFileNew,
					this->menuFileOpen,
					this->menuItem2,
					this->menuItem_file_recent,
					this->menuItem5,
					this->menuFileExit });
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
			// menuItem2
			// 
			this->menuItem2->Index = 2;
			this->menuItem2->Text = "-";
			// 
			// menuItem_file_recent
			// 
			this->menuItem_file_recent->Index = 3;
			this->menuItem_file_recent->MenuItems->Add(this->menuItem4);
			this->menuItem_file_recent->Text = "Recent Files";
			// 
			// menuItem4
			// 
			this->menuItem4->Index = 0;
			this->menuItem4->Text = "MRU SubMenu";
			// 
			// menuItem5
			// 
			this->menuItem5->Index = 4;
			this->menuItem5->Text = "-";
			// 
			// menuFileExit
			// 
			this->menuFileExit->Index = 5;
			this->menuFileExit->Text = "Exit";
			this->menuFileExit->Click += gcnew System::EventHandler(this, &MainForm::menuFileExit_Click);
			// 
			// panelInit
			// 
			this->panelInit->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->panelInit->Location = System::Drawing::Point(7, 7);
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
			this->panelRender->Location = System::Drawing::Point(7, 7);
			this->panelRender->Name = "panelRender";
			this->panelRender->Size = System::Drawing::Size(533, 416);
			this->panelRender->TabIndex = 15;
			this->panelRender->MouseLeave += gcnew System::EventHandler(this, &MainForm::panelRender_MouseLeave);
			this->panelRender->Resize += gcnew System::EventHandler(this, &MainForm::panelRender_Resize);
			this->panelRender->MouseEnter += gcnew System::EventHandler(this, &MainForm::panelRender_MouseEnter);
			// 
			// timer1
			// 
			this->timer1->AutoReset = false;
			this->timer1->Enabled = true;
			this->timer1->Interval = 20;
			this->timer1->SynchronizingObject = this;
			this->timer1->Elapsed += gcnew System::Timers::ElapsedEventHandler(this, &MainForm::OnTimer);
			// 
			// MainForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(546, 434);
			this->Controls->Add(this->panelRender);
			this->Controls->Add(this->panelInit);
			this->Menu = this->mainMenu1;
			this->Name = "MainForm";
			this->Text = "Simple Viewer";
			(safe_cast<System::ComponentModel::ISupportInitialize^  >(this->timer1))->EndInit();
			this->ResumeLayout(false);

		}		
		//

	private: System::Void OnTimer(System::Object ^  sender, System::Timers::ElapsedEventArgs ^  e);
	private: System::Void menuFileNew_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuFileOpen_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuFileExit_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_Resize(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_MouseEnter(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_MouseLeave(System::Object ^  sender, System::EventArgs ^  e);

};
}
#endif // _MANAGED

