#pragma once
#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif
#ifndef GUI_MENUMGR_HPP
#include "Tool/gui/guiMenuMgr.hpp"
#endif
#ifndef MNM_CONSTANTS_HPP
#include "Support/mnm/mnmConstants.hpp"
#endif


#ifdef _MANAGED

//============================================================================
//============================================================================
ref class tmaDialogMemory;


//============================================================================
//============================================================================
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


//============================================================================
//============================================================================
namespace StudioFramework
{
//============================================================================
//============================================================================
ref class rpnRenderPane;

//============================================================================
//============================================================================
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

		MainForm( void );

		~MainForm();

	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		System::Windows::Forms::Panel ^  GetInitRenderWindow()
		{
			return panelInit;
		}

		//--------------------------------------------------------------------
		//	public functions
		//--------------------------------------------------------------------
		rpnRenderPane ^  GetRenderPane(int i_Index);
	
		void Exit();
		void FileClosed();
		void FileOpened();
		void updateTimelineRange();
		void updateViewTimeline(bool bVisible);
		void updateViewActionToolbar(bool bVisible);
		void updateViewModeToolbar(bool bVisible);
		void updateViewStatusBar(bool bVisible);
		bool IsViewTimeline();
		bool IsViewActionToolbar();
		bool IsViewModeToolbar();
		bool IsViewStatusBar();
		void ResizeRenderWindow(int i_Width, int i_Height);

		String^ GetExecutableVersion();

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		System::Void AboutDialog();

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		System::Void HelpDialog();

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
	protected: virtual bool ProcessCmdKey(Message% msg, Keys keyData) override;  

	private: System::ComponentModel::IContainer ^  components;

private: System::Windows::Forms::MenuStrip ^  MainMenu;
private: System::Windows::Forms::ToolStripMenuItem ^  menuItem_file;

private: System::Windows::Forms::ToolStripMenuItem ^  menuItem_file_recent;
private: System::Windows::Forms::ToolStripMenuItem ^  menuItem_mru_submenu;

private: System::Windows::Forms::StatusStrip ^  statusStripMain;
private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel1;
private: System::Windows::Forms::TrackBar ^  trackBar_TimeLine;

private: System::Windows::Forms::Panel ^  panelInit;
private: System::Windows::Forms::Panel ^  panelTime;
private: System::Windows::Forms::Label ^  label_maxtime;
private: System::Windows::Forms::Label ^  label_currtime;
private: System::Windows::Forms::TextBox ^  textBox_currtime;
private: System::Windows::Forms::TextBox ^  textBox_MaxTime;

private: TerawattManagedControls::PanelLayout ^  panelLayoutRender;


//--------------------------------------------------------------------
//--------------------------------------------------------------------
private: tmaDialogMemory^ m_pMemory;// Dialog memory remembers size, location, visiblity of dialog 

private: bool m_bUpdatingTimeline;
private: bool m_bInIdleCallback;
private: System::Windows::Forms::ToolStripSeparator^  menuItem_file_seperator1;
private: bool m_bDisableNotify;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
			this->MainMenu = (gcnew System::Windows::Forms::MenuStrip());
			this->menuItem_file = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem_file_recent = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem_mru_submenu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->statusStripMain = (gcnew System::Windows::Forms::StatusStrip());
			this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->trackBar_TimeLine = (gcnew System::Windows::Forms::TrackBar());
			this->panelTime = (gcnew System::Windows::Forms::Panel());
			this->textBox_currtime = (gcnew System::Windows::Forms::TextBox());
			this->label_currtime = (gcnew System::Windows::Forms::Label());
			this->label_maxtime = (gcnew System::Windows::Forms::Label());
			this->textBox_MaxTime = (gcnew System::Windows::Forms::TextBox());
			this->panelInit = (gcnew System::Windows::Forms::Panel());
			this->panelLayoutRender = (gcnew TerawattManagedControls::PanelLayout());
			this->menuItem_file_seperator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->MainMenu->SuspendLayout();
			this->statusStripMain->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar_TimeLine))->BeginInit();
			this->panelTime->SuspendLayout();
			this->SuspendLayout();
			// 
			// MainMenu
			// 
			this->MainMenu->BackColor = System::Drawing::SystemColors::Control;
			this->MainMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->menuItem_file});
			this->MainMenu->Location = System::Drawing::Point(0, 0);
			this->MainMenu->Name = L"MainMenu";
			this->MainMenu->RenderMode = System::Windows::Forms::ToolStripRenderMode::Professional;
			this->MainMenu->Size = System::Drawing::Size(654, 24);
			this->MainMenu->TabIndex = 0;
			// 
			// menuItem_file
			// 
			this->menuItem_file->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->menuItem_file_recent, 
				this->menuItem_file_seperator1});
			this->menuItem_file->MergeIndex = 0;
			this->menuItem_file->Name = L"menuItem_file";
			this->menuItem_file->Size = System::Drawing::Size(35, 20);
			this->menuItem_file->Text = L"File";
			// 
			// menuItem_file_recent
			// 
			this->menuItem_file_recent->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->menuItem_mru_submenu});
			this->menuItem_file_recent->MergeIndex = 4;
			this->menuItem_file_recent->Name = L"menuItem_file_recent";
			this->menuItem_file_recent->Size = System::Drawing::Size(152, 22);
			this->menuItem_file_recent->Text = L"Recent Files";
			// 
			// menuItem_mru_submenu
			// 
			this->menuItem_mru_submenu->MergeIndex = 0;
			this->menuItem_mru_submenu->Name = L"menuItem_mru_submenu";
			this->menuItem_mru_submenu->Size = System::Drawing::Size(154, 22);
			this->menuItem_mru_submenu->Text = L"MRU SubMenu";
			// 
			// statusStripMain
			// 
			this->statusStripMain->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripStatusLabel1});
			this->statusStripMain->Location = System::Drawing::Point(0, 457);
			this->statusStripMain->Name = L"statusStripMain";
			this->statusStripMain->RenderMode = System::Windows::Forms::ToolStripRenderMode::Professional;
			this->statusStripMain->Size = System::Drawing::Size(654, 22);
			this->statusStripMain->TabIndex = 0;
			this->statusStripMain->Text = L"status:";
			// 
			// toolStripStatusLabel1
			// 
			this->toolStripStatusLabel1->Name = L"toolStripStatusLabel1";
			this->toolStripStatusLabel1->Size = System::Drawing::Size(109, 17);
			this->toolStripStatusLabel1->Text = L"toolStripStatusLabel1";
			// 
			// trackBar_TimeLine
			// 
			this->trackBar_TimeLine->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->trackBar_TimeLine->LargeChange = 10;
			this->trackBar_TimeLine->Location = System::Drawing::Point(0, 0);
			this->trackBar_TimeLine->Maximum = 100;
			this->trackBar_TimeLine->Name = L"trackBar_TimeLine";
			this->trackBar_TimeLine->Size = System::Drawing::Size(470, 45);
			this->trackBar_TimeLine->TabIndex = 11;
			this->trackBar_TimeLine->TabStop = false;
			this->trackBar_TimeLine->TickFrequency = 20;
			this->trackBar_TimeLine->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::trackBar_TimeLine_MouseDown);
			this->trackBar_TimeLine->ValueChanged += gcnew System::EventHandler(this, &MainForm::trackBar_timeline_ValueChanged);
			this->trackBar_TimeLine->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::trackBar_TimeLine_MouseUp);
			// 
			// panelTime
			// 
			this->panelTime->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->panelTime->BackColor = System::Drawing::SystemColors::ControlLight;
			this->panelTime->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panelTime->Controls->Add(this->textBox_currtime);
			this->panelTime->Controls->Add(this->label_currtime);
			this->panelTime->Controls->Add(this->label_maxtime);
			this->panelTime->Controls->Add(this->trackBar_TimeLine);
			this->panelTime->Controls->Add(this->textBox_MaxTime);
			this->panelTime->Location = System::Drawing::Point(8, 406);
			this->panelTime->Name = L"panelTime";
			this->panelTime->Size = System::Drawing::Size(638, 48);
			this->panelTime->TabIndex = 0;
			// 
			// textBox_currtime
			// 
			this->textBox_currtime->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->textBox_currtime->Location = System::Drawing::Point(506, 4);
			this->textBox_currtime->Name = L"textBox_currtime";
			this->textBox_currtime->Size = System::Drawing::Size(48, 20);
			this->textBox_currtime->TabIndex = 17;
			this->textBox_currtime->Text = L"0";
			this->textBox_currtime->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &MainForm::textBox_currtime_KeyDown);
			// 
			// label_currtime
			// 
			this->label_currtime->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label_currtime->Location = System::Drawing::Point(498, 28);
			this->label_currtime->Name = L"label_currtime";
			this->label_currtime->Size = System::Drawing::Size(76, 16);
			this->label_currtime->TabIndex = 16;
			this->label_currtime->Text = L"Current Time";
			// 
			// label_maxtime
			// 
			this->label_maxtime->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label_maxtime->Location = System::Drawing::Point(574, 28);
			this->label_maxtime->Name = L"label_maxtime";
			this->label_maxtime->Size = System::Drawing::Size(56, 16);
			this->label_maxtime->TabIndex = 15;
			this->label_maxtime->Text = L"Max Time";
			// 
			// textBox_MaxTime
			// 
			this->textBox_MaxTime->AcceptsReturn = true;
			this->textBox_MaxTime->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->textBox_MaxTime->Location = System::Drawing::Point(574, 4);
			this->textBox_MaxTime->Name = L"textBox_MaxTime";
			this->textBox_MaxTime->Size = System::Drawing::Size(48, 20);
			this->textBox_MaxTime->TabIndex = 12;
			this->textBox_MaxTime->TabStop = false;
			this->textBox_MaxTime->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &MainForm::textBox_MaxTime_KeyDown);
			// 
			// panelInit
			// 
			this->panelInit->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->panelInit->Location = System::Drawing::Point(0, 0);
			this->panelInit->Name = L"panelInit";
			this->panelInit->Size = System::Drawing::Size(26, 28);
			this->panelInit->TabIndex = 13;
			this->panelInit->Visible = false;
			// 
			// panelLayoutRender
			// 
			this->panelLayoutRender->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->panelLayoutRender->BackColor = System::Drawing::SystemColors::Control;
			this->panelLayoutRender->FocusColor = System::Drawing::Color::Blue;
			this->panelLayoutRender->LayoutStyle = TerawattManagedControls::PanelLayout::Layouts::e_SinglePane;
			this->panelLayoutRender->Location = System::Drawing::Point(8, 40);
			this->panelLayoutRender->Name = L"panelLayoutRender";
			this->panelLayoutRender->Separation = 2;
			this->panelLayoutRender->Size = System::Drawing::Size(638, 358);
			this->panelLayoutRender->TabIndex = 14;
			// 
			// menuItem_file_seperator1
			// 
			this->menuItem_file_seperator1->MergeIndex = 5;
			this->menuItem_file_seperator1->Name = L"menuItem_file_seperator1";
			this->menuItem_file_seperator1->Size = System::Drawing::Size(149, 6);
			// 
			// MainForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->BackColor = System::Drawing::SystemColors::Control;
			this->ClientSize = System::Drawing::Size(654, 479);
			this->Controls->Add(this->MainMenu);
			this->Controls->Add(this->panelLayoutRender);
			this->Controls->Add(this->panelInit);
			this->Controls->Add(this->statusStripMain);
			this->Controls->Add(this->panelTime);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->KeyPreview = true;
			this->MainMenuStrip = this->MainMenu;
			this->MinimumSize = System::Drawing::Size(320, 240);
			this->Name = L"MainForm";
			this->Text = gcnew System::String( mnmConstants::c_PRODUCT );
			this->Activated += gcnew System::EventHandler(this, &MainForm::MainForm_Activated);
			this->Closing += gcnew System::ComponentModel::CancelEventHandler(this, &MainForm::MainForm_Closing);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &MainForm::MainForm_KeyDown);
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			this->MainMenu->ResumeLayout(false);
			this->MainMenu->PerformLayout();
			this->statusStripMain->ResumeLayout(false);
			this->statusStripMain->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar_TimeLine))->EndInit();
			this->panelTime->ResumeLayout(false);
			this->panelTime->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
		//

	private: System::Void MainForm_Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e);
	private: System::Void MainForm_Load(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_Resize(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_MouseEnter(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_MouseLeave(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void OnApplicationIdle(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void trackBar_timeline_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void trackBar_TimeLine_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void trackBar_TimeLine_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void textBox_MaxTime_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e);
	private: System::Void textBox_currtime_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e);
	private: System::Void MainForm_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e);
	private: System::Void MainForm_Activated(System::Object ^  sender, System::EventArgs ^  e);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	private: void prefs_ReadAndApply();
	private: void prefs_UpdateAndWrite();
	private: void CreateDialogsAndTabs();
	public: void resize_mainwindow();
	private: bool do_app_closing_work();

};

}

#endif // _MANAGED
