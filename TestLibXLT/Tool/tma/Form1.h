#pragma once

#ifndef TMA_MENUMGR_HPP
#include "ToolUIManaged/tma/tmaMenuMgr.hpp"
#endif
#ifndef TMA_STATUSBARMGR_HPP
#include "ToolUIManaged/tma/tmaStatusBarMgr.hpp"
#endif
#ifndef TMA_SYSTEM_HPP
#include "ToolUIManaged/tma/tmaSystem.hpp"
#endif

#include "tabs.h"


//============================================================================
//============================================================================
namespace tmaTest
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();

			tmaSystem::g_pMainForm = this;
			tmaStatusBarMgr::g_pMgr = new tmaStatusBarMgr();
			tmaStatusBarMgr::g_pMgr->Initialize();

			tmaMenuMgr::g_pMgr = new tmaMenuMgr();
			tmaMenuMgr::g_pMgr->Initialize();
			tmaMenuMgr::g_pMgr->AddMenuItem("Test","Scut1",false,"");
		}

	protected:
		void Dispose(Boolean disposing)
		{
			tmaStatusBarMgr::g_pMgr->DeInitialize();

			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private: System::Windows::Forms::Button *  button_tabs;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::TabControl *  tabControl1;
	private: System::Windows::Forms::TabPage *  tabPage_tabs;
	private: System::Windows::Forms::TabPage *  tabPage_statusbar;
	private: System::Windows::Forms::StatusBar *  statusBar1;
	private: System::Windows::Forms::GroupBox *  groupBox_statusbar_add;
	private: System::Windows::Forms::GroupBox *  groupBox_paneltext;
	private: System::Windows::Forms::Button *  button_statusbar_add1;
	private: System::Windows::Forms::Button *  button_statusbar_add2;
	private: System::Windows::Forms::Button *  button_statusbar_add3;
	private: System::Windows::Forms::Button *  button_statusbar_add4;
	private: System::Windows::Forms::Button *  button_statusbar_text1;
	private: System::Windows::Forms::Button *  button_statusbar_text2;
	private: System::Windows::Forms::Button *  button_statusbar_text3;
	private: System::Windows::Forms::Button *  button_statusbar_text4;
	private: System::Windows::Forms::Label *  label_statusbar_info;
	private: System::Windows::Forms::TabPage*  tabPage_shortcuts;
	private: System::Windows::Forms::MenuStrip*  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem*  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem*  newToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem*  editToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem*  extraToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem*  oneToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem*  twoToolStripMenuItem;
	private: System::Windows::Forms::Label*  label_scut_textagain;
	private: System::Windows::Forms::Label*  label_scut_code;
	private: System::Windows::Forms::Label*  label_scut_text;
	private: System::Windows::Forms::GroupBox*  groupBox_scut_conversion;
	private: System::Windows::Forms::Button*  button_extra1b;
	private: System::Windows::Forms::Button*  button_extra1a;
	private: System::Windows::Forms::ToolStripMenuItem*  undoToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem*  redoToolStripMenuItem;
	private: System::Windows::Forms::Button*  button_scut_conv1;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container * components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->button_tabs = (new System::Windows::Forms::Button());
			this->label1 = (new System::Windows::Forms::Label());
			this->tabControl1 = (new System::Windows::Forms::TabControl());
			this->tabPage_tabs = (new System::Windows::Forms::TabPage());
			this->tabPage_statusbar = (new System::Windows::Forms::TabPage());
			this->label_statusbar_info = (new System::Windows::Forms::Label());
			this->groupBox_paneltext = (new System::Windows::Forms::GroupBox());
			this->button_statusbar_text4 = (new System::Windows::Forms::Button());
			this->button_statusbar_text2 = (new System::Windows::Forms::Button());
			this->button_statusbar_text3 = (new System::Windows::Forms::Button());
			this->button_statusbar_text1 = (new System::Windows::Forms::Button());
			this->groupBox_statusbar_add = (new System::Windows::Forms::GroupBox());
			this->button_statusbar_add1 = (new System::Windows::Forms::Button());
			this->button_statusbar_add3 = (new System::Windows::Forms::Button());
			this->button_statusbar_add2 = (new System::Windows::Forms::Button());
			this->button_statusbar_add4 = (new System::Windows::Forms::Button());
			this->tabPage_shortcuts = (new System::Windows::Forms::TabPage());
			this->button_extra1b = (new System::Windows::Forms::Button());
			this->button_extra1a = (new System::Windows::Forms::Button());
			this->groupBox_scut_conversion = (new System::Windows::Forms::GroupBox());
			this->button_scut_conv1 = (new System::Windows::Forms::Button());
			this->label_scut_text = (new System::Windows::Forms::Label());
			this->label_scut_textagain = (new System::Windows::Forms::Label());
			this->label_scut_code = (new System::Windows::Forms::Label());
			this->statusBar1 = (new System::Windows::Forms::StatusBar());
			this->menuStrip1 = (new System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (new System::Windows::Forms::ToolStripMenuItem());
			this->newToolStripMenuItem = (new System::Windows::Forms::ToolStripMenuItem());
			this->editToolStripMenuItem = (new System::Windows::Forms::ToolStripMenuItem());
			this->undoToolStripMenuItem = (new System::Windows::Forms::ToolStripMenuItem());
			this->redoToolStripMenuItem = (new System::Windows::Forms::ToolStripMenuItem());
			this->extraToolStripMenuItem = (new System::Windows::Forms::ToolStripMenuItem());
			this->oneToolStripMenuItem = (new System::Windows::Forms::ToolStripMenuItem());
			this->twoToolStripMenuItem = (new System::Windows::Forms::ToolStripMenuItem());
			this->tabControl1->SuspendLayout();
			this->tabPage_tabs->SuspendLayout();
			this->tabPage_statusbar->SuspendLayout();
			this->groupBox_paneltext->SuspendLayout();
			this->groupBox_statusbar_add->SuspendLayout();
			this->tabPage_shortcuts->SuspendLayout();
			this->groupBox_scut_conversion->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// button_tabs
			// 
			this->button_tabs->Location = System::Drawing::Point(16, 16);
			this->button_tabs->Name = S"button_tabs";
			this->button_tabs->Size = System::Drawing::Size(75, 23);
			this->button_tabs->TabIndex = 0;
			this->button_tabs->Text = S"Tabs";
			this->button_tabs->Click += new System::EventHandler(this, &Form1::button_tabs_Click);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(80, 35);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(312, 16);
			this->label1->TabIndex = 1;
			this->label1->Text = S"This application tests the various aspects of the TMA project";
			// 
			// tabControl1
			// 
			this->tabControl1->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl1->Controls->Add(this->tabPage_tabs);
			this->tabControl1->Controls->Add(this->tabPage_statusbar);
			this->tabControl1->Controls->Add(this->tabPage_shortcuts);
			this->tabControl1->Location = System::Drawing::Point(0, 54);
			this->tabControl1->Name = S"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(504, 370);
			this->tabControl1->TabIndex = 2;
			// 
			// tabPage_tabs
			// 
			this->tabPage_tabs->Controls->Add(this->button_tabs);
			this->tabPage_tabs->Location = System::Drawing::Point(4, 22);
			this->tabPage_tabs->Name = S"tabPage_tabs";
			this->tabPage_tabs->Size = System::Drawing::Size(496, 326);
			this->tabPage_tabs->TabIndex = 0;
			this->tabPage_tabs->Text = S"Tabs";
			// 
			// tabPage_statusbar
			// 
			this->tabPage_statusbar->Controls->Add(this->label_statusbar_info);
			this->tabPage_statusbar->Controls->Add(this->groupBox_paneltext);
			this->tabPage_statusbar->Controls->Add(this->groupBox_statusbar_add);
			this->tabPage_statusbar->Location = System::Drawing::Point(4, 22);
			this->tabPage_statusbar->Name = S"tabPage_statusbar";
			this->tabPage_statusbar->Size = System::Drawing::Size(496, 326);
			this->tabPage_statusbar->TabIndex = 1;
			this->tabPage_statusbar->Text = S"StatusBar";
			// 
			// label_statusbar_info
			// 
			this->label_statusbar_info->Location = System::Drawing::Point(16, 200);
			this->label_statusbar_info->Name = S"label_statusbar_info";
			this->label_statusbar_info->Size = System::Drawing::Size(232, 23);
			this->label_statusbar_info->TabIndex = 10;
			// 
			// groupBox_paneltext
			// 
			this->groupBox_paneltext->Controls->Add(this->button_statusbar_text4);
			this->groupBox_paneltext->Controls->Add(this->button_statusbar_text2);
			this->groupBox_paneltext->Controls->Add(this->button_statusbar_text3);
			this->groupBox_paneltext->Controls->Add(this->button_statusbar_text1);
			this->groupBox_paneltext->Location = System::Drawing::Point(128, 8);
			this->groupBox_paneltext->Name = S"groupBox_paneltext";
			this->groupBox_paneltext->Size = System::Drawing::Size(112, 160);
			this->groupBox_paneltext->TabIndex = 9;
			this->groupBox_paneltext->TabStop = false;
			this->groupBox_paneltext->Text = S"Panel Text";
			// 
			// button_statusbar_text4
			// 
			this->button_statusbar_text4->Location = System::Drawing::Point(19, 120);
			this->button_statusbar_text4->Name = S"button_statusbar_text4";
			this->button_statusbar_text4->Size = System::Drawing::Size(72, 24);
			this->button_statusbar_text4->TabIndex = 7;
			this->button_statusbar_text4->Text = S"Add To 4";
			this->button_statusbar_text4->Click += new System::EventHandler(this, &Form1::button_statusbar_text4_Click);
			// 
			// button_statusbar_text2
			// 
			this->button_statusbar_text2->Location = System::Drawing::Point(16, 56);
			this->button_statusbar_text2->Name = S"button_statusbar_text2";
			this->button_statusbar_text2->Size = System::Drawing::Size(75, 23);
			this->button_statusbar_text2->TabIndex = 5;
			this->button_statusbar_text2->Text = S"Add To 2";
			this->button_statusbar_text2->Click += new System::EventHandler(this, &Form1::button_statusbar_text2_Click);
			// 
			// button_statusbar_text3
			// 
			this->button_statusbar_text3->Location = System::Drawing::Point(16, 88);
			this->button_statusbar_text3->Name = S"button_statusbar_text3";
			this->button_statusbar_text3->Size = System::Drawing::Size(75, 23);
			this->button_statusbar_text3->TabIndex = 6;
			this->button_statusbar_text3->Text = S"Add To 3";
			this->button_statusbar_text3->Click += new System::EventHandler(this, &Form1::button_statusbar_text3_Click);
			// 
			// button_statusbar_text1
			// 
			this->button_statusbar_text1->Location = System::Drawing::Point(16, 24);
			this->button_statusbar_text1->Name = S"button_statusbar_text1";
			this->button_statusbar_text1->Size = System::Drawing::Size(75, 23);
			this->button_statusbar_text1->TabIndex = 4;
			this->button_statusbar_text1->Text = S"Add To 1";
			this->button_statusbar_text1->Click += new System::EventHandler(this, &Form1::button_statusbar_text1_Click);
			// 
			// groupBox_statusbar_add
			// 
			this->groupBox_statusbar_add->Controls->Add(this->button_statusbar_add1);
			this->groupBox_statusbar_add->Controls->Add(this->button_statusbar_add3);
			this->groupBox_statusbar_add->Controls->Add(this->button_statusbar_add2);
			this->groupBox_statusbar_add->Controls->Add(this->button_statusbar_add4);
			this->groupBox_statusbar_add->Location = System::Drawing::Point(8, 8);
			this->groupBox_statusbar_add->Name = S"groupBox_statusbar_add";
			this->groupBox_statusbar_add->Size = System::Drawing::Size(104, 160);
			this->groupBox_statusbar_add->TabIndex = 8;
			this->groupBox_statusbar_add->TabStop = false;
			this->groupBox_statusbar_add->Text = S"Add Panels";
			// 
			// button_statusbar_add1
			// 
			this->button_statusbar_add1->Location = System::Drawing::Point(16, 24);
			this->button_statusbar_add1->Name = S"button_statusbar_add1";
			this->button_statusbar_add1->Size = System::Drawing::Size(75, 23);
			this->button_statusbar_add1->TabIndex = 0;
			this->button_statusbar_add1->Text = S"Add Panel";
			this->button_statusbar_add1->Click += new System::EventHandler(this, &Form1::button_statusbar_add1_Click);
			// 
			// button_statusbar_add3
			// 
			this->button_statusbar_add3->Enabled = false;
			this->button_statusbar_add3->Location = System::Drawing::Point(16, 88);
			this->button_statusbar_add3->Name = S"button_statusbar_add3";
			this->button_statusbar_add3->Size = System::Drawing::Size(75, 23);
			this->button_statusbar_add3->TabIndex = 2;
			this->button_statusbar_add3->Text = S"button3";
			this->button_statusbar_add3->Click += new System::EventHandler(this, &Form1::button_statusbar_add3_Click);
			// 
			// button_statusbar_add2
			// 
			this->button_statusbar_add2->Enabled = false;
			this->button_statusbar_add2->Location = System::Drawing::Point(16, 56);
			this->button_statusbar_add2->Name = S"button_statusbar_add2";
			this->button_statusbar_add2->Size = System::Drawing::Size(75, 23);
			this->button_statusbar_add2->TabIndex = 1;
			this->button_statusbar_add2->Text = S"button2";
			this->button_statusbar_add2->Click += new System::EventHandler(this, &Form1::button_statusbar_add2_Click);
			// 
			// button_statusbar_add4
			// 
			this->button_statusbar_add4->Enabled = false;
			this->button_statusbar_add4->Location = System::Drawing::Point(16, 120);
			this->button_statusbar_add4->Name = S"button_statusbar_add4";
			this->button_statusbar_add4->Size = System::Drawing::Size(75, 23);
			this->button_statusbar_add4->TabIndex = 3;
			this->button_statusbar_add4->Text = S"button4";
			this->button_statusbar_add4->Click += new System::EventHandler(this, &Form1::button_statusbar_add4_Click);
			// 
			// tabPage_shortcuts
			// 
			this->tabPage_shortcuts->Controls->Add(this->button_extra1b);
			this->tabPage_shortcuts->Controls->Add(this->button_extra1a);
			this->tabPage_shortcuts->Controls->Add(this->groupBox_scut_conversion);
			this->tabPage_shortcuts->Location = System::Drawing::Point(4, 22);
			this->tabPage_shortcuts->Name = S"tabPage_shortcuts";
			this->tabPage_shortcuts->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_shortcuts->Size = System::Drawing::Size(496, 344);
			this->tabPage_shortcuts->TabIndex = 2;
			this->tabPage_shortcuts->Text = S"Shortcuts";
			this->tabPage_shortcuts->UseVisualStyleBackColor = true;
			this->tabPage_shortcuts->Enter += new System::EventHandler(this, &Form1::tabPage_shortcuts_Enter);
			// 
			// button_extra1b
			// 
			this->button_extra1b->Location = System::Drawing::Point(181, 35);
			this->button_extra1b->Name = S"button_extra1b";
			this->button_extra1b->Size = System::Drawing::Size(75, 23);
			this->button_extra1b->TabIndex = 5;
			this->button_extra1b->Text = S"extra 1b";
			this->button_extra1b->UseVisualStyleBackColor = true;
			this->button_extra1b->Click += new System::EventHandler(this, &Form1::button_extra1b_Click);
			// 
			// button_extra1a
			// 
			this->button_extra1a->Location = System::Drawing::Point(181, 6);
			this->button_extra1a->Name = S"button_extra1a";
			this->button_extra1a->Size = System::Drawing::Size(75, 23);
			this->button_extra1a->TabIndex = 4;
			this->button_extra1a->Text = S"extra1a";
			this->button_extra1a->UseVisualStyleBackColor = true;
			this->button_extra1a->Click += new System::EventHandler(this, &Form1::button_extra1a_Click);
			// 
			// groupBox_scut_conversion
			// 
			this->groupBox_scut_conversion->Controls->Add(this->button_scut_conv1);
			this->groupBox_scut_conversion->Controls->Add(this->label_scut_text);
			this->groupBox_scut_conversion->Controls->Add(this->label_scut_textagain);
			this->groupBox_scut_conversion->Controls->Add(this->label_scut_code);
			this->groupBox_scut_conversion->Location = System::Drawing::Point(6, 6);
			this->groupBox_scut_conversion->Name = S"groupBox_scut_conversion";
			this->groupBox_scut_conversion->Size = System::Drawing::Size(155, 142);
			this->groupBox_scut_conversion->TabIndex = 3;
			this->groupBox_scut_conversion->TabStop = false;
			this->groupBox_scut_conversion->Text = S"Shortcut Conversions";
			// 
			// button_scut_conv1
			// 
			this->button_scut_conv1->Location = System::Drawing::Point(14, 102);
			this->button_scut_conv1->Name = S"button_scut_conv1";
			this->button_scut_conv1->Size = System::Drawing::Size(75, 23);
			this->button_scut_conv1->TabIndex = 6;
			this->button_scut_conv1->Text = S"test conv";
			this->button_scut_conv1->UseVisualStyleBackColor = true;
			this->button_scut_conv1->Click += new System::EventHandler(this, &Form1::button_scut_conv1_Click);
			// 
			// label_scut_text
			// 
			this->label_scut_text->AutoSize = true;
			this->label_scut_text->Location = System::Drawing::Point(11, 21);
			this->label_scut_text->Name = S"label_scut_text";
			this->label_scut_text->Size = System::Drawing::Size(65, 13);
			this->label_scut_text->TabIndex = 0;
			this->label_scut_text->Text = S"shortcut text";
			// 
			// label_scut_textagain
			// 
			this->label_scut_textagain->AutoSize = true;
			this->label_scut_textagain->Location = System::Drawing::Point(11, 73);
			this->label_scut_textagain->Name = S"label_scut_textagain";
			this->label_scut_textagain->Size = System::Drawing::Size(80, 13);
			this->label_scut_textagain->TabIndex = 2;
			this->label_scut_textagain->Text = S"shortcut text ag";
			// 
			// label_scut_code
			// 
			this->label_scut_code->AutoSize = true;
			this->label_scut_code->Location = System::Drawing::Point(11, 45);
			this->label_scut_code->Name = S"label_scut_code";
			this->label_scut_code->Size = System::Drawing::Size(72, 13);
			this->label_scut_code->TabIndex = 1;
			this->label_scut_code->Text = S"shortcut code";
			// 
			// statusBar1
			// 
			this->statusBar1->Location = System::Drawing::Point(0, 431);
			this->statusBar1->Name = S"statusBar1";
			this->statusBar1->Size = System::Drawing::Size(504, 22);
			this->statusBar1->TabIndex = 3;
			this->statusBar1->Text = S"statusBar1";
			// 
			// menuStrip1
			// 
			System::Windows::Forms::ToolStripItem* __mcTemp__1[] = new System::Windows::Forms::ToolStripItem*[3];
			__mcTemp__1[0] = this->fileToolStripMenuItem;
			__mcTemp__1[1] = this->editToolStripMenuItem;
			__mcTemp__1[2] = this->extraToolStripMenuItem;
			this->menuStrip1->Items->AddRange(__mcTemp__1);
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = S"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(504, 24);
			this->menuStrip1->TabIndex = 4;
			this->menuStrip1->Text = S"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			System::Windows::Forms::ToolStripItem* __mcTemp__2[] = new System::Windows::Forms::ToolStripItem*[1];
			__mcTemp__2[0] = this->newToolStripMenuItem;
			this->fileToolStripMenuItem->DropDownItems->AddRange(__mcTemp__2);
			this->fileToolStripMenuItem->Name = S"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(35, 20);
			this->fileToolStripMenuItem->Text = S"File";
			// 
			// newToolStripMenuItem
			// 
			this->newToolStripMenuItem->Name = S"newToolStripMenuItem";
			this->newToolStripMenuItem->Size = System::Drawing::Size(106, 22);
			this->newToolStripMenuItem->Text = S"New";
			// 
			// editToolStripMenuItem
			// 
			System::Windows::Forms::ToolStripItem* __mcTemp__3[] = new System::Windows::Forms::ToolStripItem*[2];
			__mcTemp__3[0] = this->undoToolStripMenuItem;
			__mcTemp__3[1] = this->redoToolStripMenuItem;
			this->editToolStripMenuItem->DropDownItems->AddRange(__mcTemp__3);
			this->editToolStripMenuItem->Name = S"editToolStripMenuItem";
			this->editToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			this->editToolStripMenuItem->Text = S"Edit";
			// 
			// undoToolStripMenuItem
			// 
			this->undoToolStripMenuItem->Name = S"undoToolStripMenuItem";
			this->undoToolStripMenuItem->ShortcutKeys = (System::Windows::Forms::Keys)(System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Z);
			this->undoToolStripMenuItem->Size = System::Drawing::Size(148, 22);
			this->undoToolStripMenuItem->Text = S"Undo";
			// 
			// redoToolStripMenuItem
			// 
			this->redoToolStripMenuItem->Name = S"redoToolStripMenuItem";
			this->redoToolStripMenuItem->ShortcutKeys = (System::Windows::Forms::Keys)(System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Y);
			this->redoToolStripMenuItem->Size = System::Drawing::Size(148, 22);
			this->redoToolStripMenuItem->Text = S"Redo";
			// 
			// extraToolStripMenuItem
			// 
			System::Windows::Forms::ToolStripItem* __mcTemp__4[] = new System::Windows::Forms::ToolStripItem*[2];
			__mcTemp__4[0] = this->oneToolStripMenuItem;
			__mcTemp__4[1] = this->twoToolStripMenuItem;
			this->extraToolStripMenuItem->DropDownItems->AddRange(__mcTemp__4);
			this->extraToolStripMenuItem->Name = S"extraToolStripMenuItem";
			this->extraToolStripMenuItem->Size = System::Drawing::Size(45, 20);
			this->extraToolStripMenuItem->Text = S"Extra";
			// 
			// oneToolStripMenuItem
			// 
			this->oneToolStripMenuItem->Name = S"oneToolStripMenuItem";
			this->oneToolStripMenuItem->Size = System::Drawing::Size(105, 22);
			this->oneToolStripMenuItem->Text = S"One";
			// 
			// twoToolStripMenuItem
			// 
			this->twoToolStripMenuItem->Name = S"twoToolStripMenuItem";
			this->twoToolStripMenuItem->Size = System::Drawing::Size(105, 22);
			this->twoToolStripMenuItem->Text = S"Two";
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(504, 453);
			this->Controls->Add(this->statusBar1);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->menuStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = S"Form1";
			this->Text = S"Test App for TMA";
			this->tabControl1->ResumeLayout(false);
			this->tabPage_tabs->ResumeLayout(false);
			this->tabPage_statusbar->ResumeLayout(false);
			this->groupBox_paneltext->ResumeLayout(false);
			this->groupBox_statusbar_add->ResumeLayout(false);
			this->tabPage_shortcuts->ResumeLayout(false);
			this->groupBox_scut_conversion->ResumeLayout(false);
			this->groupBox_scut_conversion->PerformLayout();
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}

	private: System::Void button_tabs_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				 System::Windows::Forms::Form * l_pForm;

				 l_pForm = new tabs();

				 l_pForm->Show();
			 }

private: System::Void button_statusbar_add1_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int index = tmaStatusBarMgr::g_pMgr->AddPanel();
			 label_statusbar_info->Text = String::Format("Added StatusBar {0:D}", __box(index));
		 }

private: System::Void button_statusbar_add2_Click(System::Object *  sender, System::EventArgs *  e)
		 {
		 }

private: System::Void button_statusbar_add3_Click(System::Object *  sender, System::EventArgs *  e)
		 {
		 }

private: System::Void button_statusbar_add4_Click(System::Object *  sender, System::EventArgs *  e)
		 {
		 }

private: System::Void button_statusbar_text1_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 tmaStatusBarMgr::g_pMgr->SetText(0, "ERERER");
		 }

private: System::Void button_statusbar_text2_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 tmaStatusBarMgr::g_pMgr->SetText(1, "XXXXXX");
		 }

private: System::Void button_statusbar_text3_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 tmaStatusBarMgr::g_pMgr->SetText(2, "YYYYYY");
		 }

private: System::Void button_statusbar_text4_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 tmaStatusBarMgr::g_pMgr->SetText(3, "ZZZZZZZ");
		 }

private: System::Void tabPage_shortcuts_Enter(System::Object*  sender, System::EventArgs*  e) 
		 {
		 }
private: System::Void button_extra1a_Click(System::Object*  sender, System::EventArgs*  e) 
		 {
			 this->oneToolStripMenuItem->ShortcutKeys = (System::Windows::Forms::Keys)(System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::O);
			
		 }
private: System::Void button_extra1b_Click(System::Object*  sender, System::EventArgs*  e) 
		 {
			 this->oneToolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::None;
		 }
private: System::Void button_scut_conv1_Click(System::Object*  sender, System::EventArgs*  e) 
		 {
			 String* scut = new String("Ctrl+Shift+F");
			int mid = tmaMenuMgr::g_pMgr->GetMenuItemID("Test","Scut1");
			tmaMenuMgr::g_pMgr->SetMenuItemShortcut( mid,scut );

			this->label_scut_text->Text = scut;

			bool isvalid = tmaMenuMgr::g_pMgr->IsValidShortcut(scut);

			TypeConverter* conv = TypeDescriptor::GetConverter(__typeof(Keys));
			Keys* kys = dynamic_cast<Keys*>(conv->ConvertFromString( scut ));

			String* boolstr = (isvalid?S"True":S"False");
			int keyvalue = (int)(*kys);
			this->label_scut_code->Text = String::Format("{0} ({1})", __box(keyvalue), boolstr);

			tmaMenuObjects* pMOs = tmaMenuMgr::g_pMgr->GetMenuObjects(mid);
			ToolStripMenuItem* pTMI = pMOs->GetMenuItem();
			this->label_scut_textagain->Text = conv->ConvertToString(__box(pTMI->ShortcutKeys));
		 }
};
}
