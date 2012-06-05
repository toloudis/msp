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


//
__gc class tmaDialogMemory;


namespace LODStudio
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

		MainForm(void);

		System::Windows::Forms::Panel *  GetRenderWindow()
		{
			return panelRender;
		}

		System::Windows::Forms::Panel *  GetInitRenderWindow()
		{
			return panelInit;
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
	private: tmaDialogMemory* m_pMemory;			// Dialog memory remembers size, location, visiblity of dialog

	private: System::Windows::Forms::Panel *  panelInit;
	private: System::Windows::Forms::Panel *  panelRender;

	private: System::Windows::Forms::MainMenu *  mainMenu1;
	private: System::Windows::Forms::MenuItem *  menuItem_file;
	private: System::Windows::Forms::MenuItem *  menuFileExit;
	private: System::Windows::Forms::MenuItem *  menuFileNew;
	private: System::Windows::Forms::MenuItem *  menuFileOpen;
	private: System::Windows::Forms::MenuItem *  menuFileSave;
	private: System::Windows::Forms::MenuItem *  menuFileSaveAs;
	private: System::Windows::Forms::MenuItem *  menuItem_seperator1;
	private: System::Windows::Forms::MenuItem *  menuItem_seperator2;
	private: System::Windows::Forms::MenuItem *  menuItem_MRUsubmenu;
	private: System::Windows::Forms::MenuItem *  menuItem_file_recent;
	private: System::Windows::Forms::MenuItem *  menuItem_help;
	private: System::Windows::Forms::MenuItem *  menuHelpAbout;
	private: System::Windows::Forms::MenuItem *  menuItem_view;
	private: System::Windows::Forms::MenuItem *  menu_view_ground;

	private: System::ComponentModel::IContainer *  components;

			 bool m_bInIdleCallback;

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
			this->menuItem_file = new System::Windows::Forms::MenuItem();
			this->menuFileNew = new System::Windows::Forms::MenuItem();
			this->menuFileOpen = new System::Windows::Forms::MenuItem();
			this->menuFileSave = new System::Windows::Forms::MenuItem();
			this->menuFileSaveAs = new System::Windows::Forms::MenuItem();
			this->menuItem_seperator1 = new System::Windows::Forms::MenuItem();
			this->menuItem_file_recent = new System::Windows::Forms::MenuItem();
			this->menuItem_MRUsubmenu = new System::Windows::Forms::MenuItem();
			this->menuItem_seperator2 = new System::Windows::Forms::MenuItem();
			this->menuFileExit = new System::Windows::Forms::MenuItem();
			this->menuItem_view = new System::Windows::Forms::MenuItem();
			this->menu_view_ground = new System::Windows::Forms::MenuItem();
			this->menuItem_help = new System::Windows::Forms::MenuItem();
			this->menuHelpAbout = new System::Windows::Forms::MenuItem();
			this->panelInit = new System::Windows::Forms::Panel();
			this->panelRender = new System::Windows::Forms::Panel();
			this->SuspendLayout();
			// 
			// mainMenu1
			// 
			System::Windows::Forms::MenuItem* __mcTemp__1[] = new System::Windows::Forms::MenuItem*[3];
			__mcTemp__1[0] = this->menuItem_file;
			__mcTemp__1[1] = this->menuItem_view;
			__mcTemp__1[2] = this->menuItem_help;
			this->mainMenu1->MenuItems->AddRange(__mcTemp__1);
			// 
			// menuItem_file
			// 
			this->menuItem_file->Index = 0;
			System::Windows::Forms::MenuItem* __mcTemp__2[] = new System::Windows::Forms::MenuItem*[8];
			__mcTemp__2[0] = this->menuFileNew;
			__mcTemp__2[1] = this->menuFileOpen;
			__mcTemp__2[2] = this->menuFileSave;
			__mcTemp__2[3] = this->menuFileSaveAs;
			__mcTemp__2[4] = this->menuItem_seperator1;
			__mcTemp__2[5] = this->menuItem_file_recent;
			__mcTemp__2[6] = this->menuItem_seperator2;
			__mcTemp__2[7] = this->menuFileExit;
			this->menuItem_file->MenuItems->AddRange(__mcTemp__2);
			this->menuItem_file->Text = S"File";
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
			this->menuFileSave->Index = 2;
			this->menuFileSave->Text = S"Save";
			this->menuFileSave->Click += new System::EventHandler(this, &MainForm::menuFileSave_Click);
			// 
			// menuFileSaveAs
			// 
			this->menuFileSaveAs->Index = 3;
			this->menuFileSaveAs->Text = S"Save as...";
			this->menuFileSaveAs->Click += new System::EventHandler(this, &MainForm::menuFileSaveAs_Click);
			// 
			// menuItem_seperator1
			// 
			this->menuItem_seperator1->Index = 4;
			this->menuItem_seperator1->Text = S"-";
			// 
			// menuItem_file_recent
			// 
			this->menuItem_file_recent->Index = 5;
			System::Windows::Forms::MenuItem* __mcTemp__3[] = new System::Windows::Forms::MenuItem*[1];
			__mcTemp__3[0] = this->menuItem_MRUsubmenu;
			this->menuItem_file_recent->MenuItems->AddRange(__mcTemp__3);
			this->menuItem_file_recent->Text = S"Recent Files";
			// 
			// menuItem_MRUsubmenu
			// 
			this->menuItem_MRUsubmenu->Index = 0;
			this->menuItem_MRUsubmenu->Text = S"MRU SubMenu";
			// 
			// menuItem_seperator2
			// 
			this->menuItem_seperator2->Index = 6;
			this->menuItem_seperator2->Text = S"-";
			// 
			// menuFileExit
			// 
			this->menuFileExit->Index = 7;
			this->menuFileExit->Text = S"Exit";
			this->menuFileExit->Click += new System::EventHandler(this, &MainForm::menuFileExit_Click);
			// 
			// menuItem_view
			// 
			this->menuItem_view->Index = 1;
			System::Windows::Forms::MenuItem* __mcTemp__4[] = new System::Windows::Forms::MenuItem*[1];
			__mcTemp__4[0] = this->menu_view_ground;
			this->menuItem_view->MenuItems->AddRange(__mcTemp__4);
			this->menuItem_view->Text = S"View";
			// 
			// menu_view_ground
			// 
			this->menu_view_ground->Checked = true;
			this->menu_view_ground->Index = 0;
			this->menu_view_ground->Text = S"View Ground";
			this->menu_view_ground->Click += new System::EventHandler(this, &MainForm::menu_view_ground_Click);
			// 
			// menuItem_help
			// 
			this->menuItem_help->Index = 2;
			System::Windows::Forms::MenuItem* __mcTemp__5[] = new System::Windows::Forms::MenuItem*[1];
			__mcTemp__5[0] = this->menuHelpAbout;
			this->menuItem_help->MenuItems->AddRange(__mcTemp__5);
			this->menuItem_help->Text = S"Help";
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
			this->panelInit->Location = System::Drawing::Point(7, 0);
			this->panelInit->Name = S"panelInit";
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
			this->panelRender->Location = System::Drawing::Point(7, 35);
			this->panelRender->Name = S"panelRender";
			this->panelRender->Size = System::Drawing::Size(666, 520);
			this->panelRender->TabIndex = 15;
			this->panelRender->Resize += new System::EventHandler(this, &MainForm::panelRender_Resize);
			this->panelRender->MouseEnter += new System::EventHandler(this, &MainForm::panelRender_MouseEnter);
			this->panelRender->MouseMove += new System::Windows::Forms::MouseEventHandler(this, &MainForm::panelRender_MouseMove);
			this->panelRender->MouseLeave += new System::EventHandler(this, &MainForm::panelRender_MouseLeave);
			// 
			// MainForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(679, 565);
			this->Controls->Add(this->panelRender);
			this->Controls->Add(this->panelInit);
			this->Menu = this->mainMenu1;
			this->Name = S"MainForm";
			this->Text = S"LOD Studio";
			this->ResumeLayout(false);
			System::Windows::Forms::Application::Idle += new System::EventHandler(this, &MainForm::OnApplicationIdle);				

		}
		//

	private: System::Void OnApplicationIdle(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuFileNew_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuFileOpen_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuFileSave_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuFileSaveAs_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuFileExit_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void panelRender_Resize(System::Object *  sender, System::EventArgs *  e);
	private: System::Void panelRender_MouseEnter(System::Object *  sender, System::EventArgs *  e);
	private: System::Void panelRender_MouseLeave(System::Object *  sender, System::EventArgs *  e);
	private: System::Void panelRender_MouseMove(System::Object *  sender, System::Windows::Forms::MouseEventArgs *  e);
	private: System::Void menuHelpAbout_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menu_view_ground_Click(System::Object *  sender, System::EventArgs *  e);
};
}
