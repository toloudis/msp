#pragma once

#ifndef DBG_LOG_HPP
#include "Core/Dbg/dbgLog.hpp"
#endif
#ifndef DOC_SINGLETYPEMGR_HPP
#include "Tool/doc/docSingleTypeMgr.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef FS_RESOURCETRACKER_HPP
#include "Core/fs/fsResourceTracker.hpp"
#endif
#ifndef GF_PACKAGE_HPP
#include "Core/gf/gfPackage.hpp"
#endif
#ifndef GUI_MESSAGEBOX_HPP
#include "Tool/gui/guiMessageBox.hpp"
#endif
#ifndef TMA_REGISTRYUTIL_HPP
#include "ToolUIManaged/tma/tmaRegistryUtil.hpp"
#endif
#ifndef RSTK_DOCUMENTINTEREST_HPP
#include "Tool/rstk/rstkDocumentInterest.hpp"
#endif
#ifndef RSTK_DATAMGR_HPP
#include "Tool/rstk/rstkDataMgr.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifndef GUI_SINGLEDOCHANDLER_HPP
#include "Tool/gui/guiSingleDocHandler.hpp"
#endif

#include <string>
#include <set>
#include <fstream>

//
//	SourceSafe includes
//
// Be sure to link with the following libraries:
// user32.lib uuid.lib oleaut32.lib ole32.lib
//#include "stdafx.h"
#define INITGUID
#include <windows.h>
#include <ocidl.h>
//#include <atlbase.h>
//#include <atlconv.h>
#include "ssauto.h"
#include <comdef.h>



//============================================================================
//============================================================================
namespace Source
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Collections::Specialized;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Diagnostics;
	using namespace System::Drawing;

	const char* lc_Reg_Folder		= "VersionControl";
	const char* lc_Key_VCDir		= "VCDir";
	const char* lc_Key_VCDB			= "VCDatabase";
	const char* lc_Key_VCLogin		= "VCLogin";
	const char* lc_Key_VCPassword	= "VCPassword";

	const char* lc_VC_App			= "SS.exe";
	
	const char *c_BatchFileName = "C:\\Projects\\ResourceSyncBatch.bat";
	const char *c_WorkingFolder = "C:\\Projects\\";

	/// <summary> 
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{	
	public:

		Form1(void)
		{
			InitializeComponent();

			// Add the "version" document chunk LAST so it gets written at the end.
			//std::string exestr;
			//tmaManagedStringUtils::ManagedStringToStdString(main_form->GetExecutableVersion(), exestr);
			//docSingleTypeMgr::AddDocumentInterest(new mainDocumentInterest(exestr));

			m_bViewFlat = this->radioButton_flat->Checked;

			//	set up the controls
			//
			SetUpControls();

			//	check the command-line arguments
			//
			cli::array<String^> ^args = Environment::GetCommandLineArgs();
			for (int i = 0; i < args->Length; ++i)
			{
				std::string arg;
				tmaManagedStringUtils::ManagedStringToStdString(args[i], arg);
				DBG_LOG2("argument %d (%s)", i, arg.c_str());
			}
		}
  
	protected:
		~Form1()
		{
			 //write_to_registry();
		}

	private: TerawattManagedControls::FileChooser ^  fileChooser_file;
	private: System::Windows::Forms::Label ^  label_file;
	private: System::Windows::Forms::TreeView ^  treeView_resources;
	private: System::Windows::Forms::Label ^  label_resources;
	private: System::Windows::Forms::Button ^  button_sync;

	private: System::Windows::Forms::Label ^  label_sourceproject;
	private: System::Windows::Forms::RadioButton ^  radioButton_flat;
	private: System::Windows::Forms::RadioButton ^  radioButton_hierarchy;
	private: System::Windows::Forms::GroupBox ^  groupBox_view;
	private: System::Windows::Forms::TabControl ^  tabControl_sync;
	private: System::Windows::Forms::TabPage ^  tabPage_db;
	private: System::Windows::Forms::TabPage ^  tabPage_resources;
	private: System::Windows::Forms::Label ^  label_login;
	private: System::Windows::Forms::TextBox ^  textBox_login;
	private: System::Windows::Forms::Label ^  label_pwd;
	private: System::Windows::Forms::TextBox ^  textBox_password;

	private: System::Windows::Forms::TabPage ^  tabPage_versioncontrol;
	private: System::Windows::Forms::Label ^  label_vcapp;
	private: TerawattManagedControls::FolderChooser ^  folderChooser_vcdir;
	private: TerawattManagedControls::FolderChooser ^  folderChooser_vcdb;
	private: System::Windows::Forms::TabPage ^  tabPage_output;

	private: System::Windows::Forms::Label ^  label_output;
	private: System::Windows::Forms::TextBox ^  textBox_output;

	private: System::Windows::Forms::Label ^  label_vcproj_desc;
	private: System::Windows::Forms::Label ^  label_vcdir_desc;
	private: System::Windows::Forms::TabPage ^  tabPage_instructions;
	private: System::Windows::Forms::TextBox ^  textBox_instructions;
	private: System::Windows::Forms::CheckBox ^  checkBox_usecommandline;
	private: System::Windows::Forms::GroupBox ^  groupBox_login;
	private: System::Windows::Forms::Button^  button_BatchFile;
	private: System::Windows::Forms::Button^  button_verifylogin;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^ components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->fileChooser_file = (gcnew TerawattManagedControls::FileChooser());
			this->label_file = (gcnew System::Windows::Forms::Label());
			this->treeView_resources = (gcnew System::Windows::Forms::TreeView());
			this->label_resources = (gcnew System::Windows::Forms::Label());
			this->button_sync = (gcnew System::Windows::Forms::Button());
			this->label_sourceproject = (gcnew System::Windows::Forms::Label());
			this->radioButton_flat = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_hierarchy = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox_view = (gcnew System::Windows::Forms::GroupBox());
			this->tabControl_sync = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_instructions = (gcnew System::Windows::Forms::TabPage());
			this->textBox_instructions = (gcnew System::Windows::Forms::TextBox());
			this->tabPage_versioncontrol = (gcnew System::Windows::Forms::TabPage());
			this->label_vcdir_desc = (gcnew System::Windows::Forms::Label());
			this->folderChooser_vcdir = (gcnew TerawattManagedControls::FolderChooser());
			this->label_vcapp = (gcnew System::Windows::Forms::Label());
			this->tabPage_db = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_login = (gcnew System::Windows::Forms::GroupBox());
			this->button_verifylogin = (gcnew System::Windows::Forms::Button());
			this->textBox_password = (gcnew System::Windows::Forms::TextBox());
			this->label_pwd = (gcnew System::Windows::Forms::Label());
			this->textBox_login = (gcnew System::Windows::Forms::TextBox());
			this->label_login = (gcnew System::Windows::Forms::Label());
			this->label_vcproj_desc = (gcnew System::Windows::Forms::Label());
			this->folderChooser_vcdb = (gcnew TerawattManagedControls::FolderChooser());
			this->tabPage_resources = (gcnew System::Windows::Forms::TabPage());
			this->button_BatchFile = (gcnew System::Windows::Forms::Button());
			this->checkBox_usecommandline = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage_output = (gcnew System::Windows::Forms::TabPage());
			this->label_output = (gcnew System::Windows::Forms::Label());
			this->textBox_output = (gcnew System::Windows::Forms::TextBox());
			this->groupBox_view->SuspendLayout();
			this->tabControl_sync->SuspendLayout();
			this->tabPage_instructions->SuspendLayout();
			this->tabPage_versioncontrol->SuspendLayout();
			this->tabPage_db->SuspendLayout();
			this->groupBox_login->SuspendLayout();
			this->tabPage_resources->SuspendLayout();
			this->tabPage_output->SuspendLayout();
			this->SuspendLayout();
			// 
			// fileChooser_file
			// 
			this->fileChooser_file->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->fileChooser_file->Filter = L"Scenes (*.mab)|*.mab";
			this->fileChooser_file->Location = System::Drawing::Point(64, 16);
			this->fileChooser_file->Name = L"fileChooser_file";
			this->fileChooser_file->Size = System::Drawing::Size(512, 24);
			this->fileChooser_file->TabIndex = 0;
			this->fileChooser_file->ValueChanged += gcnew System::EventHandler(this, &Form1::fileChooser_file_ValueChanged);
			// 
			// label_file
			// 
			this->label_file->Location = System::Drawing::Point(16, 14);
			this->label_file->Name = L"label_file";
			this->label_file->Size = System::Drawing::Size(40, 23);
			this->label_file->TabIndex = 1;
			this->label_file->Text = L"Scene";
			this->label_file->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// treeView_resources
			// 
			this->treeView_resources->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->treeView_resources->Location = System::Drawing::Point(16, 80);
			this->treeView_resources->Name = L"treeView_resources";
			this->treeView_resources->Size = System::Drawing::Size(560, 480);
			this->treeView_resources->Sorted = true;
			this->treeView_resources->TabIndex = 2;
			// 
			// label_resources
			// 
			this->label_resources->Location = System::Drawing::Point(16, 48);
			this->label_resources->Name = L"label_resources";
			this->label_resources->Size = System::Drawing::Size(64, 23);
			this->label_resources->TabIndex = 3;
			this->label_resources->Text = L"Resources";
			this->label_resources->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// button_sync
			// 
			this->button_sync->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->button_sync->Location = System::Drawing::Point(56, 568);
			this->button_sync->Name = L"button_sync";
			this->button_sync->Size = System::Drawing::Size(93, 23);
			this->button_sync->TabIndex = 4;
			this->button_sync->Text = L"Sync!";
			this->button_sync->Click += gcnew System::EventHandler(this, &Form1::button_sync_Click);
			// 
			// label_sourceproject
			// 
			this->label_sourceproject->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label_sourceproject->Location = System::Drawing::Point(8, 42);
			this->label_sourceproject->Name = L"label_sourceproject";
			this->label_sourceproject->Size = System::Drawing::Size(128, 16);
			this->label_sourceproject->TabIndex = 6;
			this->label_sourceproject->Text = L"Version Control Project";
			this->label_sourceproject->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// radioButton_flat
			// 
			this->radioButton_flat->Location = System::Drawing::Point(10, 13);
			this->radioButton_flat->Name = L"radioButton_flat";
			this->radioButton_flat->Size = System::Drawing::Size(46, 16);
			this->radioButton_flat->TabIndex = 7;
			this->radioButton_flat->Text = L"Flat";
			this->radioButton_flat->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton_flat_CheckedChanged);
			// 
			// radioButton_hierarchy
			// 
			this->radioButton_hierarchy->Checked = true;
			this->radioButton_hierarchy->Location = System::Drawing::Point(88, 13);
			this->radioButton_hierarchy->Name = L"radioButton_hierarchy";
			this->radioButton_hierarchy->Size = System::Drawing::Size(88, 16);
			this->radioButton_hierarchy->TabIndex = 8;
			this->radioButton_hierarchy->TabStop = true;
			this->radioButton_hierarchy->Text = L"Hierarchical";
			this->radioButton_hierarchy->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton_hierarchy_CheckedChanged);
			// 
			// groupBox_view
			// 
			this->groupBox_view->Controls->Add(this->radioButton_flat);
			this->groupBox_view->Controls->Add(this->radioButton_hierarchy);
			this->groupBox_view->Location = System::Drawing::Point(206, 40);
			this->groupBox_view->Name = L"groupBox_view";
			this->groupBox_view->Size = System::Drawing::Size(180, 32);
			this->groupBox_view->TabIndex = 9;
			this->groupBox_view->TabStop = false;
			this->groupBox_view->Text = L"View";
			// 
			// tabControl_sync
			// 
			this->tabControl_sync->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl_sync->Controls->Add(this->tabPage_instructions);
			this->tabControl_sync->Controls->Add(this->tabPage_versioncontrol);
			this->tabControl_sync->Controls->Add(this->tabPage_db);
			this->tabControl_sync->Controls->Add(this->tabPage_resources);
			this->tabControl_sync->Controls->Add(this->tabPage_output);
			this->tabControl_sync->Location = System::Drawing::Point(0, 0);
			this->tabControl_sync->Name = L"tabControl_sync";
			this->tabControl_sync->SelectedIndex = 0;
			this->tabControl_sync->Size = System::Drawing::Size(592, 624);
			this->tabControl_sync->TabIndex = 10;
			// 
			// tabPage_instructions
			// 
			this->tabPage_instructions->Controls->Add(this->textBox_instructions);
			this->tabPage_instructions->Location = System::Drawing::Point(4, 22);
			this->tabPage_instructions->Name = L"tabPage_instructions";
			this->tabPage_instructions->Size = System::Drawing::Size(584, 598);
			this->tabPage_instructions->TabIndex = 4;
			this->tabPage_instructions->Text = L"Instructions";
			// 
			// textBox_instructions
			// 
			this->textBox_instructions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_instructions->BackColor = System::Drawing::SystemColors::Control;
			this->textBox_instructions->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->textBox_instructions->Location = System::Drawing::Point(8, 8);
			this->textBox_instructions->Multiline = true;
			this->textBox_instructions->Name = L"textBox_instructions";
			this->textBox_instructions->Size = System::Drawing::Size(568, 584);
			this->textBox_instructions->TabIndex = 0;
			this->textBox_instructions->Text = L"Instructions";
			this->textBox_instructions->WordWrap = false;
			// 
			// tabPage_versioncontrol
			// 
			this->tabPage_versioncontrol->Controls->Add(this->label_vcdir_desc);
			this->tabPage_versioncontrol->Controls->Add(this->folderChooser_vcdir);
			this->tabPage_versioncontrol->Controls->Add(this->label_vcapp);
			this->tabPage_versioncontrol->Location = System::Drawing::Point(4, 22);
			this->tabPage_versioncontrol->Name = L"tabPage_versioncontrol";
			this->tabPage_versioncontrol->Size = System::Drawing::Size(584, 598);
			this->tabPage_versioncontrol->TabIndex = 2;
			this->tabPage_versioncontrol->Text = L"Version Control";
			this->tabPage_versioncontrol->Leave += gcnew System::EventHandler(this, &Form1::tabPage_versioncontrol_Leave);
			// 
			// label_vcdir_desc
			// 
			this->label_vcdir_desc->Location = System::Drawing::Point(112, 16);
			this->label_vcdir_desc->Name = L"label_vcdir_desc";
			this->label_vcdir_desc->Size = System::Drawing::Size(448, 32);
			this->label_vcdir_desc->TabIndex = 17;
			this->label_vcdir_desc->Text = L"Location of the SS.exe application.   (\\program files\\microsoft visual sourcesafe" 
				L"\\...)";
			// 
			// folderChooser_vcdir
			// 
			this->folderChooser_vcdir->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->folderChooser_vcdir->Location = System::Drawing::Point(104, 56);
			this->folderChooser_vcdir->Name = L"folderChooser_vcdir";
			this->folderChooser_vcdir->Size = System::Drawing::Size(464, 24);
			this->folderChooser_vcdir->TabIndex = 16;
			this->folderChooser_vcdir->ValueChanged += gcnew System::EventHandler(this, &Form1::folderChooser_vcdir_ValueChanged);
			// 
			// label_vcapp
			// 
			this->label_vcapp->Location = System::Drawing::Point(16, 56);
			this->label_vcapp->Name = L"label_vcapp";
			this->label_vcapp->Size = System::Drawing::Size(80, 23);
			this->label_vcapp->TabIndex = 15;
			this->label_vcapp->Text = L"VC Directory";
			this->label_vcapp->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tabPage_db
			// 
			this->tabPage_db->Controls->Add(this->groupBox_login);
			this->tabPage_db->Controls->Add(this->label_vcproj_desc);
			this->tabPage_db->Controls->Add(this->folderChooser_vcdb);
			this->tabPage_db->Controls->Add(this->label_sourceproject);
			this->tabPage_db->Location = System::Drawing::Point(4, 22);
			this->tabPage_db->Name = L"tabPage_db";
			this->tabPage_db->Size = System::Drawing::Size(584, 598);
			this->tabPage_db->TabIndex = 0;
			this->tabPage_db->Text = L"Database";
			this->tabPage_db->Leave += gcnew System::EventHandler(this, &Form1::tabPage_db_Leave);
			// 
			// groupBox_login
			// 
			this->groupBox_login->Controls->Add(this->button_verifylogin);
			this->groupBox_login->Controls->Add(this->textBox_password);
			this->groupBox_login->Controls->Add(this->label_pwd);
			this->groupBox_login->Controls->Add(this->textBox_login);
			this->groupBox_login->Controls->Add(this->label_login);
			this->groupBox_login->Location = System::Drawing::Point(16, 88);
			this->groupBox_login->Name = L"groupBox_login";
			this->groupBox_login->Size = System::Drawing::Size(264, 128);
			this->groupBox_login->TabIndex = 14;
			this->groupBox_login->TabStop = false;
			this->groupBox_login->Text = L"Project Version Control Login";
			// 
			// button_verifylogin
			// 
			this->button_verifylogin->Location = System::Drawing::Point(155, 87);
			this->button_verifylogin->Name = L"button_verifylogin";
			this->button_verifylogin->Size = System::Drawing::Size(73, 35);
			this->button_verifylogin->TabIndex = 11;
			this->button_verifylogin->Text = L"Verify Login";
			this->button_verifylogin->UseVisualStyleBackColor = true;
			this->button_verifylogin->Click += gcnew System::EventHandler(this, &Form1::button_verifylogin_Click);
			// 
			// textBox_password
			// 
			this->textBox_password->Location = System::Drawing::Point(145, 61);
			this->textBox_password->Name = L"textBox_password";
			this->textBox_password->PasswordChar = 'X';
			this->textBox_password->Size = System::Drawing::Size(100, 20);
			this->textBox_password->TabIndex = 10;
			// 
			// label_pwd
			// 
			this->label_pwd->Location = System::Drawing::Point(17, 61);
			this->label_pwd->Name = L"label_pwd";
			this->label_pwd->Size = System::Drawing::Size(100, 16);
			this->label_pwd->TabIndex = 9;
			this->label_pwd->Text = L"Password";
			this->label_pwd->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox_login
			// 
			this->textBox_login->Location = System::Drawing::Point(145, 29);
			this->textBox_login->Name = L"textBox_login";
			this->textBox_login->Size = System::Drawing::Size(100, 20);
			this->textBox_login->TabIndex = 8;
			// 
			// label_login
			// 
			this->label_login->Location = System::Drawing::Point(17, 29);
			this->label_login->Name = L"label_login";
			this->label_login->Size = System::Drawing::Size(100, 16);
			this->label_login->TabIndex = 7;
			this->label_login->Text = L"Login";
			this->label_login->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_vcproj_desc
			// 
			this->label_vcproj_desc->Location = System::Drawing::Point(128, 9);
			this->label_vcproj_desc->Name = L"label_vcproj_desc";
			this->label_vcproj_desc->Size = System::Drawing::Size(280, 23);
			this->label_vcproj_desc->TabIndex = 13;
			this->label_vcproj_desc->Text = L"Find the folder with the srcsafe.ini file for the project";
			this->label_vcproj_desc->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// folderChooser_vcdb
			// 
			this->folderChooser_vcdb->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->folderChooser_vcdb->Location = System::Drawing::Point(128, 40);
			this->folderChooser_vcdb->Name = L"folderChooser_vcdb";
			this->folderChooser_vcdb->Size = System::Drawing::Size(440, 24);
			this->folderChooser_vcdb->TabIndex = 11;
			this->folderChooser_vcdb->ValueChanged += gcnew System::EventHandler(this, &Form1::folderChooser_vcdb_ValueChanged);
			// 
			// tabPage_resources
			// 
			this->tabPage_resources->Controls->Add(this->button_BatchFile);
			this->tabPage_resources->Controls->Add(this->checkBox_usecommandline);
			this->tabPage_resources->Controls->Add(this->groupBox_view);
			this->tabPage_resources->Controls->Add(this->button_sync);
			this->tabPage_resources->Controls->Add(this->label_resources);
			this->tabPage_resources->Controls->Add(this->treeView_resources);
			this->tabPage_resources->Controls->Add(this->label_file);
			this->tabPage_resources->Controls->Add(this->fileChooser_file);
			this->tabPage_resources->Location = System::Drawing::Point(4, 22);
			this->tabPage_resources->Name = L"tabPage_resources";
			this->tabPage_resources->Size = System::Drawing::Size(584, 598);
			this->tabPage_resources->TabIndex = 1;
			this->tabPage_resources->Text = L"Resources";
			// 
			// button_BatchFile
			// 
			this->button_BatchFile->Location = System::Drawing::Point(422, 568);
			this->button_BatchFile->Name = L"button_BatchFile";
			this->button_BatchFile->Size = System::Drawing::Size(119, 23);
			this->button_BatchFile->TabIndex = 11;
			this->button_BatchFile->Text = L"Create Batch File";
			this->button_BatchFile->UseVisualStyleBackColor = true;
			this->button_BatchFile->Click += gcnew System::EventHandler(this, &Form1::button_BatchFile_Click);
			// 
			// checkBox_usecommandline
			// 
			this->checkBox_usecommandline->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->checkBox_usecommandline->Location = System::Drawing::Point(192, 568);
			this->checkBox_usecommandline->Name = L"checkBox_usecommandline";
			this->checkBox_usecommandline->Size = System::Drawing::Size(224, 24);
			this->checkBox_usecommandline->TabIndex = 10;
			this->checkBox_usecommandline->Text = L"Use Command Line instead of API";
			// 
			// tabPage_output
			// 
			this->tabPage_output->Controls->Add(this->label_output);
			this->tabPage_output->Controls->Add(this->textBox_output);
			this->tabPage_output->Location = System::Drawing::Point(4, 22);
			this->tabPage_output->Name = L"tabPage_output";
			this->tabPage_output->Size = System::Drawing::Size(584, 598);
			this->tabPage_output->TabIndex = 3;
			this->tabPage_output->Text = L"Output";
			// 
			// label_output
			// 
			this->label_output->Location = System::Drawing::Point(8, 5);
			this->label_output->Name = L"label_output";
			this->label_output->Size = System::Drawing::Size(232, 23);
			this->label_output->TabIndex = 1;
			this->label_output->Text = L"Information about asset synchronization";
			this->label_output->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox_output
			// 
			this->textBox_output->AcceptsReturn = true;
			this->textBox_output->AcceptsTab = true;
			this->textBox_output->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_output->Location = System::Drawing::Point(8, 32);
			this->textBox_output->Multiline = true;
			this->textBox_output->Name = L"textBox_output";
			this->textBox_output->ReadOnly = true;
			this->textBox_output->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->textBox_output->Size = System::Drawing::Size(568, 560);
			this->textBox_output->TabIndex = 0;
			this->textBox_output->WordWrap = false;
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(592, 622);
			this->Controls->Add(this->tabControl_sync);
			this->Name = L"Form1";
			this->Text = L"Resource Sync";
			this->groupBox_view->ResumeLayout(false);
			this->tabControl_sync->ResumeLayout(false);
			this->tabPage_instructions->ResumeLayout(false);
			this->tabPage_instructions->PerformLayout();
			this->tabPage_versioncontrol->ResumeLayout(false);
			this->tabPage_db->ResumeLayout(false);
			this->groupBox_login->ResumeLayout(false);
			this->groupBox_login->PerformLayout();
			this->tabPage_resources->ResumeLayout(false);
			this->tabPage_output->ResumeLayout(false);
			this->tabPage_output->PerformLayout();
			this->ResumeLayout(false);

		}	

private: System::Void button_sync_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			write_to_registry();

			this->tabControl_sync->SelectedTab = this->tabPage_output;
			clear_output_textbox();

			if (this->checkBox_usecommandline->Checked)
			{
				execute_commands_COMMANDLINE();
			}
			else
			{
				execute_commands();
			}
		 }

private: void execute_commands()
		 {
			CLSID clsid;
			IClassFactory *pClf;
			IVSSDatabase *pVdb;

			std::string tempstr;
			tmaManagedStringUtils::ManagedStringToStdString( this->folderChooser_vcdb->Directory, tempstr );
			tempstr += "\\";
			tempstr += "srcsafe.ini";
			BSTR bstrPath = std_string_to_BSTR(tempstr);
			tmaManagedStringUtils::ManagedStringToStdString( this->textBox_login->Text, tempstr );
			BSTR bstrUName = std_string_to_BSTR(tempstr);
			tmaManagedStringUtils::ManagedStringToStdString( this->textBox_password->Text, tempstr );
			BSTR bstrUPass = std_string_to_BSTR(tempstr);

			HRESULT result;
			CoInitialize(0);
			result = CLSIDFromProgID(L"SourceSafe", &clsid );
			if (result == S_OK)
			{
				result = CoGetClassObject( clsid, CLSCTX_ALL, NULL, IID_IClassFactory, (void**)&pClf );
				if (result == S_OK)
				{
					result = pClf->CreateInstance( NULL, IID_IVSSDatabase,(void **) &pVdb );
					if (result == S_OK)
					{
						HRESULT result = pVdb->Open(bstrPath, bstrUName, bstrUPass);
						if(S_OK == result)
						{
							//Database Successfully Opened!
							//Add code here to use the open database.
							//
							DBG_LOG0("Opened the database!");

							//
							fsLocator filepath;
							fsResourceTrackerData& rd = rstkDataMgr::Data();

							//	HACK - figure out a way to cut off the first few directories.
							//
							filepath = rd.m_Resources[0].GetFilePath();
							int index = get_prefix(filepath);

							//	Perform a version control "get" on each item in the list
							//
							//set by last cmd --- proc->StartInfo->FileName = System::String::Concat(this->folderChooser_vcdir->Directory, "\\", new System::String(lc_VC_App));
							for (int i = 0; i < rd.m_Resources.size(); ++i)
							{
								filepath = rd.m_Resources[i].GetFilePath();
								filepath.RemoveBefore(index);
								fsFileUtil::LocatorToANSIFilename(filepath,tempstr);
								tempstr.insert(0, "$\\");
								IVSSItem* pVSSItem;
								BSTR bstrItem = std_string_to_BSTR(tempstr);
								HRESULT hr = pVdb->get_VSSItem(bstrItem, false, &pVSSItem);
								if (pVSSItem != 0)
								{
									hr = pVSSItem->Get(0,0);
									add_to_output_textbox(gcnew System::String(tempstr.c_str()),true);
									add_to_output_textbox("   Updated", true);
								}
							}
						}
						else
						{
							// error result from pVdb->Open
						}
						pVdb->Release();
					}
					else
					{
						// error result from CreateInstance
					}
					pClf->Release();
				}
				else
				{
					// error result from CoGetClassObject
					switch(result)
					{
						case REGDB_E_CLASSNOTREG:
						{
							guiMessageBox::Show("Class identifier is not properly registered. Can also indicate that the value you specified in dwClsContext is not in the registry.","App Error", guiMessageBox::e_OKOnly);
							break;
						}
						case E_NOINTERFACE:
						{
 							guiMessageBox::Show("Either the object pointed to by ppv does not support the interface identified by riid, or the QueryInterface operation on the class object returned E_NOINTERFACE.","App Error", guiMessageBox::e_OKOnly);
							break;
						}
						case REGDB_E_READREGDB:
						{
							guiMessageBox::Show("Error reading the registration database.", "App Error", guiMessageBox::e_OKOnly);
							break;
						}
						case CO_E_APPNOTFOUND:
						{
 							guiMessageBox::Show("EXE not found (CLSCTX_LOCAL_SERVER only).", "App Error", guiMessageBox::e_OKOnly);
							break;
						}
						case CO_E_DLLNOTFOUND:
						{
 							guiMessageBox::Show("In-process DLL or handler DLL not found (depends on context).", "App Error", guiMessageBox::e_OKOnly);
							break;
						}
						case E_ACCESSDENIED:
						{
 							guiMessageBox::Show("General access failure (returned from LoadLibrary or CreateProcess).", "App Error", guiMessageBox::e_OKOnly);
							break;
						}
						case CO_E_ERRORINDLL:
						{
 							guiMessageBox::Show("EXE has error in image.", "App Error", guiMessageBox::e_OKOnly);
							break;
						}
						case CO_E_APPDIDNTREG:
						{
 							guiMessageBox::Show("EXE was launched, but it did not register class object (may or may not have shut down).", "App Error", guiMessageBox::e_OKOnly);
							break;
						}
					}
				}
			}
			else
			{
				// error result from CLSIDFromProgID
				switch(result)
				{
					case CO_E_CLASSSTRING:
					{
						guiMessageBox::Show("The registered CLSID for the ProgID is invalid.","App Error", guiMessageBox::e_OKOnly);
						break;
					}
					case REGDB_E_WRITEREGDB:
					{
 						guiMessageBox::Show("An error occurred writing the CLSID to the registry. See Remarks below.","App Error", guiMessageBox::e_OKOnly);
						break;
					}
				}
			}

			CoUninitialize();

			SysFreeString(bstrPath);
			SysFreeString(bstrUName);
			SysFreeString(bstrUPass);
	   }
private: void execute_commands_COMMANDLINE()
		 {
			std::string cmdline;
			std::string args;
			std::string tempstr;
			System::Diagnostics::Process^ proc = gcnew System::Diagnostics::Process();
			System::Diagnostics::ProcessStartInfo^ psinfo = proc->StartInfo; //new System::Diagnostics::ProcessStartInfo();

			//	set the path - this is only good for one command session
			//	1) PATH=%PATH%;C:\Program Files\My Application
			//
			//	Although you cannot set the EnvironmentVariables property, 
			//	you can modify the StringDictionary returned by the property. 
			//	For example, the following code adds a TempPath environment variable: 
			//	myProcess.StartInfo.EnvironmentVariables.Add("TempPath", "C:\\Temp"). 
			//	You must set the UseShellExecute property to false to start the process 
			//	after changing the EnvironmentVariables property. 
			//	If UseShellExecute is true, an InvalidOperationException is thrown when 
			//	the Start method is called.
			//
			cmdline = "set";
			std::string newpath;
			tmaManagedStringUtils::ManagedStringToStdString( this->folderChooser_vcdir->Directory, newpath );
			cmdline = newpath;
			StringDictionary^ pSD = psinfo->EnvironmentVariables;
			System::String^ pStr = pSD["Path"];
			System::String::Concat(pStr,gcnew System::String(newpath.c_str()));
			psinfo->EnvironmentVariables["Path"] = pStr;
			psinfo->UseShellExecute = false;
			psinfo->CreateNoWindow = true;
			//DBG_LOG1("procedure (%s)", cmdline.c_str());
			//DBG_LOG1("     args (%s)", args.c_str());

			//	set the current project
			//	2) set SSDIR = "U:\\Bratz02\\SourceSafe"
			//
			cmdline = "set SSDIR=";
			tmaManagedStringUtils::ManagedStringToStdString( this->folderChooser_vcdb->Directory, newpath );
			cmdline += newpath;
			if (psinfo->EnvironmentVariables->ContainsKey("SSDIR"))
			{
				psinfo->EnvironmentVariables["SSDIR"] = gcnew System::String(newpath.c_str());
			}
			else
			{
				psinfo->EnvironmentVariables->Add("SSDIR", gcnew System::String(newpath.c_str()));
			}
			add_to_output_textbox(newpath.c_str(),true);	// output project
			psinfo->UseShellExecute = false;
			//DBG_LOG1("procedure (%s)", cmdline.c_str());
			//DBG_LOG1("     args (%s)", args.c_str());

			//	Create login command parameter
			//
			std::string loginstr;
			get_login_string(loginstr);

			//	set the working folder for the project
			//	ss workfold E:\	
			//
			//	TO DO: set the working folder correctly
			//
			tmaManagedStringUtils::ManagedStringToStdString(this->folderChooser_vcdir->Directory, cmdline);
			cmdline += "\\";
			cmdline += lc_VC_App;
			proc->StartInfo->FileName = System::String::Concat(this->folderChooser_vcdir->Directory, "\\", gcnew System::String(lc_VC_App));
			args = "workfold $ ";
			args += c_WorkingFolder;
			args += loginstr;
			psinfo->Arguments = gcnew System::String(args.c_str());
			//DBG_LOG1("procedure (%s)", cmdline.c_str());
			//DBG_LOG1("     args (%s)", args.c_str());
			psinfo->RedirectStandardOutput = true;
			psinfo->RedirectStandardError = true;
			psinfo->UseShellExecute = false;
			psinfo->CreateNoWindow = true;

			proc->Start();
			System::String^ pOutput = proc->StandardError->ReadToEnd();

			pStr = pSD["SSDIR"];
			//tmaManagedStringUtils::ManagedStringToStdString(pStr,tempstr);
			//DBG_LOG1("EnvVar SSDIR=(%s)", tempstr.c_str());

			proc->WaitForExit();

			tmaManagedStringUtils::ManagedStringToStdString(pOutput,tempstr);
			//DBG_LOG1("   output [%s]", tempstr.c_str());

			add_to_output_textbox(gcnew System::String(cmdline.c_str()),true);
			add_to_output_textbox(gcnew System::String(args.c_str()),true);
			add_to_output_textbox(gcnew System::String(tempstr.c_str()),true);

			//	get an item from the DB (GF = force dir)
			//	ss Get $/path/file -GF -YUser,Pass (or just -YUser)
			//
			fsLocator filepath;
			fsResourceTrackerData& rd = rstkDataMgr::Data();

			//	HACK - figure out a way to cut off the first few directories.
			//
			filepath = rd.m_Resources[0].GetFilePath();
			int index = get_prefix(filepath);

			//	Perform a version control "get" on each item in the list
			//
			//set by last cmd --- proc->StartInfo->FileName = System::String::Concat(this->folderChooser_vcdir->Directory, "\\", new System::String(lc_VC_App));
			for (int i = 0; i < rd.m_Resources.size(); ++i)
			{
				filepath = rd.m_Resources[i].GetFilePath();
				filepath.RemoveBefore(index);
				fsFileUtil::LocatorToANSIFilename(filepath,tempstr);
				args = "Get \"$\\";
				args += tempstr;
				args += "\"";
				args += " -GF";
				args += loginstr;
				psinfo->Arguments = gcnew System::String(args.c_str());
				//DBG_LOG1("procedure (%s)", cmdline.c_str());
				//DBG_LOG1("     args (%s)", args.c_str());
				psinfo->RedirectStandardOutput = true;
				psinfo->RedirectStandardError = true;
				psinfo->UseShellExecute = false;
				psinfo->CreateNoWindow = true;

				proc->Start();
				System::String^ pOutput = proc->StandardError->ReadToEnd();
				proc->WaitForExit();

				tmaManagedStringUtils::ManagedStringToStdString(pOutput,tempstr);
				//DBG_LOG1("   output [%s]", tempstr.c_str());
				add_to_output_textbox(args.c_str(),true);
				add_to_output_textbox(tempstr.c_str(),(tempstr.empty()?false:true));
			}
		 }

private: System::Void button_BatchFile_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			std::string ssuser, sspath, ssdir;
			tmaManagedStringUtils::ManagedStringToStdString( this->textBox_login->Text, ssuser );
			tmaManagedStringUtils::ManagedStringToStdString( this->folderChooser_vcdir->Directory, sspath );
			tmaManagedStringUtils::ManagedStringToStdString( this->folderChooser_vcdb->Directory, ssdir );

			std::ofstream ofile(c_BatchFileName);
			ofile << "cd c:\\" << std::endl;
			ofile << "echo This batch file is: " << c_BatchFileName << std::endl;
			ofile << "set ssuser=" << ssuser << std::endl;
			ofile << "set sspath=" << sspath << std::endl;
			ofile << "set ssdir=" << ssdir << std::endl << std::endl;

			ofile << "echo off" << std::endl;
			ofile << "set path=%sspath%;%path%" << std::endl;
			ofile << "Workfold $ \"" << c_WorkingFolder << "\"" << std::endl;
			// Get the resource list
			fsLocator filepath, filedir;
			std::string filename;
			fsResourceTrackerData& rd = rstkDataMgr::Data();

			DBG_ASSERT0( rd.m_Resources.size() > 0, "No resource directories");

			// TODO remove Assert and change to if statement.  if size is 0, go to output tab, display message, return out

			//	HACK - figure out a way to cut off the first few directories.
			//
			filepath = rd.m_Resources[0].GetFilePath();
			int index = get_prefix(filepath);

			// Find directories that doe not exists and make them
			ofile << "echo Checking for directories that do not exist..." << std::endl;

			std::set<std::string> dirs_to_make;
			for (int i = 0; i < rd.m_Resources.size(); ++i)
			{
				filedir = rd.m_Resources[i].GetFilePath();
				filedir.Pop(); // pop off filename to get directory
				filedir.ReplaceName( itString("c:\\"), 0 );
				if (!fsFileUtil::DirectoryExists(filedir))
				{
					fsFileUtil::LocatorToANSIFilename(filedir, filename);
					to_lower(filename);
					if (dirs_to_make.find(filename) == dirs_to_make.end())
					{
						ofile << "echo " << filename << std::endl;
						dirs_to_make.insert(filename);
					}
				}
			}

			if (dirs_to_make.empty())
			{
				ofile << "echo all directories exist." << std::endl;
			}
			else
			{
				ofile << "echo If these directories are correct, press any key to create them." << std::endl;
				ofile << "pause" << std::endl;

				std::set<std::string>::iterator it;
				for (it = dirs_to_make.begin(); it != dirs_to_make.end(); ++it)
				{
					ofile << "mkdir " << (*it) << std::endl;
				}
			}

			ofile << "echo Ready to start fetch." << std::endl;
			ofile << "pause" << std::endl;
			
			int num = rd.m_Resources.size();
			std::vector<fsLocator> exist_locs, nonexist_locs;
			for (int i = 0; i < rd.m_Resources.size(); ++i)
			{
				filepath = rd.m_Resources[i].GetFilePath();
				filepath.RemoveBefore(index);

				// Create separate lists for whether the filename exists
				if (fsFileUtil::FileExists(rd.m_Resources[i].GetFilePath()))
					exist_locs.push_back(filepath);
				else
					nonexist_locs.push_back(filepath);
			}
				
			int num_non_exist = nonexist_locs.size();
			ofile << "echo number of non-existing: " << num_non_exist << std::endl;
	
			// Do the files that do not exist first, this allows us to
			// quit earlier when we don't have to have the latest data
			// to run the scene file
			for (int i = 0; i < num_non_exist; ++i)
			{
				fsFileUtil::LocatorToANSIFilename(nonexist_locs[i], filename);
				ofile << "echo new " << i << " of " << num << " $/" << filename << std::endl;
				ofile << "ss Get \"$/" << filename << "\" -GF" << std::endl;
			}
			// Then, update the files of which we already had old versions 
			for (int i = 0; i < exist_locs.size(); ++i)
			{
				fsFileUtil::LocatorToANSIFilename(exist_locs[i], filename);
				ofile << "echo updating " << i+num_non_exist << " of " << num << " $/" << filename << std::endl;
				ofile << "ss Get \"$/" << filename << "\" -GF" << std::endl;
			}

			ofile << "echo Finished." << std::endl;
			ofile << "pause" << std::endl;
			ofile.close();

			System::Diagnostics::Process^ proc = gcnew System::Diagnostics::Process();
			System::Diagnostics::ProcessStartInfo^ psinfo = proc->StartInfo; //new System::Diagnostics::ProcessStartInfo();

			psinfo->UseShellExecute = true;
			//psinfo->CreateNoWindow = true;
			proc->StartInfo->FileName = gcnew System::String(c_BatchFileName);
			//psinfo->Arguments = gcnew System::String("");
			//DBG_LOG1("procedure (%s)", cmdline.c_str());
			//DBG_LOG1("     args (%s)", args.c_str());
			//psinfo->RedirectStandardOutput = true;
			//psinfo->RedirectStandardError = true;	

			proc->Start();
			//System::String^ pOutput = proc->StandardError->ReadToEnd();
			proc->WaitForExit();

			//tmaManagedStringUtils::ManagedStringToStdString(pOutput,tempstr);
			//DBG_LOG1("   output [%s]", tempstr.c_str());
			add_to_output_textbox(psinfo->Arguments, true);
			//add_to_output_textbox(pOutput, pOutput->Empty ? true : false);		
		 }
private: System::Void fileChooser_file_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 System::String^ pfname = fileChooser_file->Fullpath; //->Filename;
			 read_file(pfname);

			 if (this->treeView_resources->Nodes->Count > 0)
				button_sync->Enabled = true;
			 else
				button_sync->Enabled = false;
		 }

private: void read_file( System::String^ i_pFileName )
		 {
			fsLocator filepath;
			tmaManagedStringUtils::ManagedStringToLocator(i_pFileName, filepath);

			// read in the file
			guiSingleDocHandler::Open(filepath);

			build_treeview();

			//	test if there is 
			fsResourceTrackerData& rd = rstkDataMgr::Data();
			if (rd.GetList().size() == 0)
			{
				std::string tempstr;
				tmaManagedStringUtils::ManagedStringToStdString(i_pFileName, tempstr);
				char buffer[256];
				sprintf(buffer, "no resource chunk in file (%s)", tempstr.c_str());
				add_to_output_textbox(buffer,true);
			}
		 }


private: System::Void radioButton_hierarchy_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (this->radioButton_hierarchy->Checked)
			 {
				 this->m_bViewFlat = false;
				 build_treeview();
			 }
		 }

private: System::Void radioButton_flat_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (this->radioButton_flat->Checked)
			 {
				 this->m_bViewFlat = true;
				 build_treeview();
			 }
		 }

private: void build_treeview()
		 {
			fsResourceTrackerData& rd = rstkDataMgr::Data();
			int level = 0;

			//	DEBUG - output the data
			fsResourceTracker::SetData(rd);
			fsResourceTracker::Debug_OutputList();

			// update the tree
			treeView_resources->BeginUpdate();

			TreeNodeCollection^ pNodes = this->treeView_resources->Nodes;
			pNodes->Clear();

			//	build the tree
			//
			if (rd.m_Resources.size() > 0)
			{
				if (m_bViewFlat)
				{
					//	Flat View
					for (int i = 0; i < rd.m_Resources.size(); ++i)
					{
						String^ pFname = tmaManagedStringUtils::LocatorToManagedString( rd.m_Resources[i].GetFilePath() );
						TreeNode^ pTN = gcnew TreeNode( pFname );
						if (rd.m_Resources[i].GetDepth() <= 1)
							pTN->ForeColor = System::Drawing::Color::DarkBlue;
						else
							pTN->ForeColor = System::Drawing::Color::Black;
						pNodes->Add( pTN );
					}
				}
				else
				{
					//	Hierarchical View
					TreeNode^ pCurrentRoot = nullptr;
					for (int i = 0; i < rd.m_Resources.size(); ++i)
					{
						int depth = rd.m_Resources[i].GetDepth();

						if (depth <= level)
						{
							//	jump up the hierarchy to find the correct level to add nodes to
							//
							for (int j = depth; j <= level+1; ++j)
							{
								if (pCurrentRoot != nullptr)
								{
									//DBG_LOG3("%02d %02d %s", i,j, pCurrentRoot->Text);
									pCurrentRoot = pCurrentRoot->Parent;
									if (pCurrentRoot != nullptr)
									{
										pNodes = pCurrentRoot->Nodes;
										--level;
									}
								}
							}
						}

						String^ pFname = tmaManagedStringUtils::LocatorToManagedString( rd.m_Resources[i].GetFilePath() );
						TreeNode^ pTN = gcnew TreeNode( pFname );
						if (rd.m_Resources[i].GetDepth() <= 1)
							pTN->ForeColor = System::Drawing::Color::DarkBlue;
						else
							pTN->ForeColor = System::Drawing::Color::Black;

						if (pCurrentRoot == nullptr)
						{
							pNodes = this->treeView_resources->Nodes;
						}

						pNodes->Add( pTN );
						pCurrentRoot = pTN;
						pNodes = pCurrentRoot->Nodes;
						level = depth;
					}

					// DEBUG Output only
					//
					for (int i = 0; i < rd.m_Resources.size(); ++i)
					{
						std::string fullpath;
						std::string space;
						fsFileUtil::LocatorToANSIFilename(rd.m_Resources[i].GetFilePath(), fullpath);
						int depth = rd.m_Resources[i].GetDepth();
						for (int j=0; j < depth; ++j)
							space += "  ";
						DBG_LOG3( "%03d - %s%s", i, space.c_str(), fullpath.c_str() );
					}
				}

				treeView_resources->ExpandAll();
			}
			else
			{
				//	no elements (or an "old" format scene file
				//
				String^ pFname = gcnew System::String("Old scene format or no resources");
				TreeNode^ pTN = gcnew TreeNode( pFname );
				pNodes->Add( pTN );
			}

			treeView_resources->EndUpdate();
		}

private:
	void read_from_registry()
	{
		//	VC directory
		//
		std::string str_vcdir = tmaRegistryUtil::GetValue(tmaRegistryUtil::e_CurrentUser,
			lc_Reg_Folder, lc_Key_VCDir);
		this->folderChooser_vcdir->Directory = gcnew System::String(str_vcdir.c_str());

		//	VC database
		//
		std::string str_vcdb = tmaRegistryUtil::GetValue(tmaRegistryUtil::e_CurrentUser,
			lc_Reg_Folder, lc_Key_VCDB);
		this->folderChooser_vcdb->Directory = gcnew System::String(str_vcdb.c_str());

		//	VC login
		//
		std::string str_vcLogin = tmaRegistryUtil::GetValue(tmaRegistryUtil::e_CurrentUser,
			lc_Reg_Folder, lc_Key_VCLogin);
		this->textBox_login->Text = gcnew System::String(str_vcLogin.c_str());

		//	VC Password
		//
		std::string str_vcPassword = tmaRegistryUtil::GetValue(tmaRegistryUtil::e_CurrentUser,
			lc_Reg_Folder, lc_Key_VCPassword);
		this->textBox_password->Text = gcnew System::String(str_vcPassword.c_str());
	}

private:
	void write_to_registry()
	{
		char buffer[128];

		//	VC directory
		//
		::sprintf(buffer, "%s", this->folderChooser_vcdir->Directory );
		tmaRegistryUtil::SetValue(tmaRegistryUtil::e_CurrentUser, lc_Reg_Folder, lc_Key_VCDir, buffer);

		//	VC database
		//
		::sprintf(buffer, "%s", this->folderChooser_vcdb->Directory );
		tmaRegistryUtil::SetValue(tmaRegistryUtil::e_CurrentUser, lc_Reg_Folder, lc_Key_VCDB, buffer);

		//	VC login
		//
		::sprintf(buffer, "%s", this->textBox_login->Text );
		tmaRegistryUtil::SetValue(tmaRegistryUtil::e_CurrentUser, lc_Reg_Folder, lc_Key_VCLogin, buffer);

		//	VC password
		//
		::sprintf(buffer, "%s", this->textBox_password->Text );
		tmaRegistryUtil::SetValue(tmaRegistryUtil::e_CurrentUser, lc_Reg_Folder, lc_Key_VCPassword, buffer);
	}

	void add_to_output_textbox(const std::string &i_String, bool i_bAddNewLine)
	{
		add_to_output_textbox(gcnew System::String(i_String.c_str()), i_bAddNewLine);
	}
	void add_to_output_textbox(System::String^ i_pText, bool i_bAddNewLine)
	{
		this->textBox_output->Text = System::String::Format("{0} {1} {2}", textBox_output->Text, i_pText, (i_bAddNewLine ? (System::Environment::NewLine):"") );
	}

	void clear_output_textbox()
	{
		this->textBox_output->Text = System::String::Empty;
	}

	BSTR std_string_to_BSTR(std::string& i_InputString)
	{
		return SysAllocString(_bstr_t(i_InputString.c_str()));

		//USES_CONVERSION;    // declare locals used by the ATL macros
		//return A2W(i_InputString);

		//std::wstring wstr;
		//wstr.resize(i_InputString.size());
		//for (unsigned int i = 0; i < i_InputString.size(); ++i)
		//	wstr[i] = i_InputString[i];
		//CComBSTR com_bstr(wstr.c_str());
		//return com_bstr.Detach();
	}

	void get_login_string(std::string& i_LoginStr)
	{
		i_LoginStr.clear();

		if (this->textBox_login->Text->Length > 0)
		{
			std::string tempstr;
			i_LoginStr = " -Y";
			tmaManagedStringUtils::ManagedStringToStdString( this->textBox_login->Text, tempstr );
			add_to_output_textbox(tempstr.c_str(),true);	// output login
			i_LoginStr += tempstr;
			if (this->textBox_password->Text->Length > 0)
			{
				i_LoginStr += ",";
				tmaManagedStringUtils::ManagedStringToStdString( this->textBox_password->Text, tempstr );
				add_to_output_textbox(tempstr.c_str(),true);	// output password
				i_LoginStr += tempstr;
			}
		}
	}

	System::String^ get_version_string()
	{
		System::Reflection::Assembly^ pSRAss = System::Reflection::Assembly::GetExecutingAssembly();
		return pSRAss->GetName()->Version->ToString();
	}
private:
	void SetUpControls()
	{
		button_sync->Enabled = false;

		read_from_registry();

		add_to_output_textbox( "ResourceSync ", false );
		add_to_output_textbox( get_version_string(), true );

		this->textBox_instructions->Text = "1) Version Control Tab\r\n- Verify Version Control Directory for Source Safe Executable.\r\n\r\n2) Database Tab\r\n- Select Database\r\n\r\n3) Resources Tab\r\n- Select Scene\r\n\r\n4) Hit Sync! button to get latest version of files.\r\n\r\n";
	}
	
	int get_prefix(const fsLocator &i_Filepath)
	{
		int index = i_Filepath.FindFirst(itString("Data"));
		if (index == -1)
		{
			index = i_Filepath.FindFirst(itString("data"));
			if (index == -1)
			{
				index = i_Filepath.FindFirst(itString("Shots"));
				if (index == -1)
				{
					index = i_Filepath.FindFirst(itString("shots"));
				}
			}
		}
		DBG_ASSERT0((index != -1), "Non-standard directories -- fix the code");
		--index;	// we want to keep the previous dir, it is the project name dir.
		return index;
	}

	void to_lower(std::string &io_Filename)
	{
		for (int i=0; i<io_Filename.size(); ++i)
		{
			io_Filename[i] = ::tolower(io_Filename[i]);
		}
	}

private:
	bool m_bViewFlat;

private: System::Void tabPage_versioncontrol_Leave(System::Object^  sender, System::EventArgs^  e) 
		 {
			 write_to_registry();
		 }
private: System::Void tabPage_db_Leave(System::Object^  sender, System::EventArgs^  e) 
		 {
			 write_to_registry();
		 }
private: System::Void button_verifylogin_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			// verify the login
			std::string ssuser;
			tmaManagedStringUtils::ManagedStringToStdString( this->textBox_login->Text, ssuser );

			std::string sspwd;
			tmaManagedStringUtils::ManagedStringToStdString( this->textBox_password->Text, sspwd );

			// TODO - check here if the login/password is correct
		 }
private: System::Void folderChooser_vcdir_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 // check if file exists -- SSEXP.EXE
			 // search for FileExists in Mach Studio
			fsLocator filename;
			if (folderChooser_vcdir->Directory->Length >= 0)
			{
				tmaManagedStringUtils::ManagedStringToLocator (folderChooser_vcdir->Directory, filename);
				filename.Push( "SSEXP.EXE");
		    }
			if (fsFileUtil::FileExists( filename ) )
			{
				//DBG_LOG0("                     - FOUND!" );
				return;
			}
			guiMessageBox::Show("SSEXP.EXE not found","File Not Found", guiMessageBox::e_OKOnly);
		 }
private: System::Void folderChooser_vcdb_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 // check if srcsafe.ini exists in the folder
			fsLocator filename;
			if (folderChooser_vcdb->Directory->Length >= 0)
			{
				tmaManagedStringUtils::ManagedStringToLocator (folderChooser_vcdb->Directory, filename);
				filename.Push( "srcsafe.ini");
			}
			//check if exists
			if (fsFileUtil::FileExists( filename ) )
			{
				//DBG_LOG0("                     - FOUND!" );
				return;
			}
			guiMessageBox::Show("srcsafe.ini not found","File Not Found", guiMessageBox::e_OKOnly);
		 }
};
}


