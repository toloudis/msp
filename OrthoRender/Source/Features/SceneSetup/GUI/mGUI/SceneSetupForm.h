#pragma once

#ifndef SCENESETUPDIALOGUTIL_HPP
#include "Features/SceneSetup/GUI/SceneSetupDialogUtil.hpp"
#endif
#ifndef SCENESETUPDATA_HPP
#include "Features/SceneSetup/Data/SceneSetupData.hpp"
#endif
#ifndef SCENESETUPDATAPARSER_HPP
#include "Features/SceneSetup/Data/SceneSetupDataParser.hpp"
#endif
#ifndef SCENESETUPDIALOGUTIL_HPP
#include "Features/SceneSetup/GUI/SceneSetupDialogUtil.hpp"
#endif

#ifndef MNM_PATHS_HPP
#include "Support/mnm/mnmPaths.hpp"
#endif

#ifndef CH_Reader_HPP
#include "Core/ch/chReader.hpp"
#endif
#ifndef CH_Writer_HPP
#include "Core/ch/chWriter.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "Core/dbg/dbgLog.hpp"
#endif
#ifndef DOC_SINGLEDOCUMENTMGR_HPP
#include "Tool/doc/docSingleDocumentMgr.hpp"
#endif
#ifndef DOC_SINGLETYPEMGR_HPP
#include "Tool/doc/docSingleTypeMgr.hpp"
#endif
#ifndef ENV_STLHELPERS_HPP
#include "Core/env/envSTLHelpers.hpp"
#endif
#ifndef FS_FILEENUM_HPP
#include "Core/fs/fsFileEnum.hpp"
#endif
#ifndef FS_FILESTREAM_HPP
#include "Core/fs/fsFileStream.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_FILEX_HPP
#include "Core/fs/fsFileX.hpp"
#endif
#ifndef GF_FILETXT_HPP
#include "Core/gf/gfFileTxt.hpp"
#endif
#ifndef GF_PATHS_HPP
#include "Core/gf/gfPaths.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/it/itStringUtil.hpp"
#endif
#ifndef GUI_FILEDIALOGUTILS_HPP
#include "Tool/gui/guiFileDialogUtils.hpp"
#endif
#ifndef GUI_MESSAGEBOX_HPP
#include "Tool/gui/guiMessageBox.hpp"
#endif
#ifndef PREFSMGR_HPP
#include "Features/Prefs/PrefsMgr.hpp"
#endif
#ifndef PROJECTSETUPMGR_HPP
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"
#endif
#ifndef PROJECTSETUPDATA_HPP
#include "Features/ProjectSetup/Data/ProjectSetupData.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifndef TMA_SINGLEDOCHANDLER_HPP
#include "Tool/gui/guiSingleDocHandler.hpp"
#endif

#ifdef _MANAGED

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
	/// <summary>
	/// Summary for SceneSetupForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class SceneSetupForm : public System::Windows::Forms::Form
	{
	public:
		SceneSetupForm(SceneSetupData& i_Data) : m_SceneSetupData(i_Data)
		{
			InitializeComponent();

			SetComponentInitialValues();
		}

	public:
		~SceneSetupForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

	private: System::Windows::Forms::TabControl ^  tabControl_scenesetup;

	private: System::Windows::Forms::Button ^  button_prev;
	private: System::Windows::Forms::Button ^  button_next;
	private: System::Windows::Forms::Button ^  button_finish;

	private: System::Windows::Forms::TabPage ^  tabPage_project;
	private: System::Windows::Forms::Label ^  label_project;
	private: System::Windows::Forms::ListBox ^  listBox_project;
	private: System::Windows::Forms::GroupBox^  groupBox_projectdata;
	private: System::Windows::Forms::Label^  label_projectname;
	private: TerawattManagedControls::FolderChooser^  folderChooser_projectdir;
	private: System::Windows::Forms::TextBox^  textBox_projectname;
	private: System::Windows::Forms::TextBox^  textBox_projectdesc;
	private: System::Windows::Forms::Label^  label_projectdir;
	private: System::Windows::Forms::Label^  label_projectdesc;

	private: System::Windows::Forms::TabPage ^  tabPage_scene;
	private: System::Windows::Forms::Label ^  label_scene;
	private: System::Windows::Forms::Button ^  button_importscenelist;
	private: System::Windows::Forms::Label ^  label_importscenelist;
	private: System::Windows::Forms::Button ^  button_removescene;
	private: System::Windows::Forms::ListBox ^  listBox_scene;
	private: System::Windows::Forms::GroupBox^  groupBox_scenedata;
	private: System::Windows::Forms::Label^  label_scenename;
	private: System::Windows::Forms::TextBox^  textBox_scenedesc;
	private: System::Windows::Forms::TextBox^  textBox_scenename;
	private: System::Windows::Forms::Label^  label_sceneondesc;

	private: System::Windows::Forms::TabPage ^  tabPage_summary;
	private: System::Windows::Forms::Button ^  button_createdirs;
	private: System::Windows::Forms::Label ^  label_summary;
	private: System::Windows::Forms::GroupBox ^  groupBox_summary;

	private: SceneSetupData& m_SceneSetupData;

	private: bool m_bDataInitialized;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl_scenesetup = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_project = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_projectdata = (gcnew System::Windows::Forms::GroupBox());
			this->label_projectname = (gcnew System::Windows::Forms::Label());
			this->folderChooser_projectdir = (gcnew TerawattManagedControls::FolderChooser());
			this->textBox_projectname = (gcnew System::Windows::Forms::TextBox());
			this->textBox_projectdesc = (gcnew System::Windows::Forms::TextBox());
			this->label_projectdir = (gcnew System::Windows::Forms::Label());
			this->label_projectdesc = (gcnew System::Windows::Forms::Label());
			this->listBox_project = (gcnew System::Windows::Forms::ListBox());
			this->label_project = (gcnew System::Windows::Forms::Label());
			this->tabPage_scene = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_scenedata = (gcnew System::Windows::Forms::GroupBox());
			this->label_scenename = (gcnew System::Windows::Forms::Label());
			this->textBox_scenedesc = (gcnew System::Windows::Forms::TextBox());
			this->textBox_scenename = (gcnew System::Windows::Forms::TextBox());
			this->label_sceneondesc = (gcnew System::Windows::Forms::Label());
			this->listBox_scene = (gcnew System::Windows::Forms::ListBox());
			this->button_removescene = (gcnew System::Windows::Forms::Button());
			this->label_importscenelist = (gcnew System::Windows::Forms::Label());
			this->button_importscenelist = (gcnew System::Windows::Forms::Button());
			this->label_scene = (gcnew System::Windows::Forms::Label());
			this->tabPage_summary = (gcnew System::Windows::Forms::TabPage());
			this->button_createdirs = (gcnew System::Windows::Forms::Button());
			this->groupBox_summary = (gcnew System::Windows::Forms::GroupBox());
			this->label_summary = (gcnew System::Windows::Forms::Label());
			this->button_prev = (gcnew System::Windows::Forms::Button());
			this->button_next = (gcnew System::Windows::Forms::Button());
			this->button_finish = (gcnew System::Windows::Forms::Button());
			this->tabControl_scenesetup->SuspendLayout();
			this->tabPage_project->SuspendLayout();
			this->groupBox_projectdata->SuspendLayout();
			this->tabPage_scene->SuspendLayout();
			this->groupBox_scenedata->SuspendLayout();
			this->tabPage_summary->SuspendLayout();
			this->groupBox_summary->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_scenesetup
			// 
			this->tabControl_scenesetup->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_scenesetup->Appearance = System::Windows::Forms::TabAppearance::FlatButtons;
			this->tabControl_scenesetup->Controls->Add(this->tabPage_project);
			this->tabControl_scenesetup->Controls->Add(this->tabPage_scene);
			this->tabControl_scenesetup->Controls->Add(this->tabPage_summary);
			this->tabControl_scenesetup->Location = System::Drawing::Point(8, 8);
			this->tabControl_scenesetup->Name = "tabControl_scenesetup";
			this->tabControl_scenesetup->SelectedIndex = 0;
			this->tabControl_scenesetup->Size = System::Drawing::Size(530, 481);
			this->tabControl_scenesetup->TabIndex = 1;
			this->tabControl_scenesetup->SelectedIndexChanged += gcnew System::EventHandler(this, &SceneSetupForm::tabControl_scenesetup_SelectedIndexChanged);
			// 
			// tabPage_project
			// 
			this->tabPage_project->Controls->Add(this->groupBox_projectdata);
			this->tabPage_project->Controls->Add(this->listBox_project);
			this->tabPage_project->Controls->Add(this->label_project);
			this->tabPage_project->Location = System::Drawing::Point(4, 25);
			this->tabPage_project->Name = "tabPage_project";
			this->tabPage_project->Size = System::Drawing::Size(522, 452);
			this->tabPage_project->TabIndex = 0;
			this->tabPage_project->Text = "project";
			this->tabPage_project->Enter += gcnew System::EventHandler(this, &SceneSetupForm::tabPage_project_Enter);
			this->tabPage_project->Leave += gcnew System::EventHandler(this, &SceneSetupForm::tabPage_project_Leave);
			// 
			// groupBox_projectdata
			// 
			this->groupBox_projectdata->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupBox_projectdata->Controls->Add(this->label_projectname);
			this->groupBox_projectdata->Controls->Add(this->folderChooser_projectdir);
			this->groupBox_projectdata->Controls->Add(this->textBox_projectname);
			this->groupBox_projectdata->Controls->Add(this->textBox_projectdesc);
			this->groupBox_projectdata->Controls->Add(this->label_projectdir);
			this->groupBox_projectdata->Controls->Add(this->label_projectdesc);
			this->groupBox_projectdata->Location = System::Drawing::Point(16, 258);
			this->groupBox_projectdata->Name = "groupBox_projectdata";
			this->groupBox_projectdata->Size = System::Drawing::Size(487, 191);
			this->groupBox_projectdata->TabIndex = 13;
			this->groupBox_projectdata->TabStop = false;
			this->groupBox_projectdata->Text = "Project Data";
			// 
			// label_projectname
			// 
			this->label_projectname->Location = System::Drawing::Point(10, 22);
			this->label_projectname->Name = "label_projectname";
			this->label_projectname->Size = System::Drawing::Size(224, 23);
			this->label_projectname->TabIndex = 7;
			this->label_projectname->Text = "Project Name (this will be the file name too)";
			// 
			// folderChooser_projectdir
			// 
			this->folderChooser_projectdir->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->folderChooser_projectdir->Location = System::Drawing::Point(10, 158);
			this->folderChooser_projectdir->Name = "folderChooser_projectdir";
			this->folderChooser_projectdir->Size = System::Drawing::Size(465, 24);
			this->folderChooser_projectdir->TabIndex = 12;
			// 
			// textBox_projectname
			// 
			this->textBox_projectname->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textBox_projectname->Location = System::Drawing::Point(10, 46);
			this->textBox_projectname->Name = "textBox_projectname";
			this->textBox_projectname->Size = System::Drawing::Size(465, 20);
			this->textBox_projectname->TabIndex = 8;
			// 
			// textBox_projectdesc
			// 
			this->textBox_projectdesc->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textBox_projectdesc->Location = System::Drawing::Point(10, 102);
			this->textBox_projectdesc->Name = "textBox_projectdesc";
			this->textBox_projectdesc->Size = System::Drawing::Size(465, 20);
			this->textBox_projectdesc->TabIndex = 10;
			// 
			// label_projectdir
			// 
			this->label_projectdir->Location = System::Drawing::Point(10, 134);
			this->label_projectdir->Name = "label_projectdir";
			this->label_projectdir->Size = System::Drawing::Size(100, 23);
			this->label_projectdir->TabIndex = 9;
			this->label_projectdir->Text = "Project Directory";
			// 
			// label_projectdesc
			// 
			this->label_projectdesc->Location = System::Drawing::Point(10, 78);
			this->label_projectdesc->Name = "label_projectdesc";
			this->label_projectdesc->Size = System::Drawing::Size(100, 23);
			this->label_projectdesc->TabIndex = 11;
			this->label_projectdesc->Text = "Project Description";
			// 
			// listBox_project
			// 
			this->listBox_project->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->listBox_project->Location = System::Drawing::Point(13, 40);
			this->listBox_project->Name = "listBox_project";
			this->listBox_project->Size = System::Drawing::Size(490, 199);
			this->listBox_project->TabIndex = 2;
			this->listBox_project->SelectedIndexChanged += gcnew System::EventHandler(this, &SceneSetupForm::listBox_project_SelectedIndexChanged);
			// 
			// label_project
			// 
			this->label_project->Location = System::Drawing::Point(13, 16);
			this->label_project->Name = "label_project";
			this->label_project->Size = System::Drawing::Size(208, 23);
			this->label_project->TabIndex = 0;
			this->label_project->Text = "What Project is this for\?";
			// 
			// tabPage_scene
			// 
			this->tabPage_scene->Controls->Add(this->groupBox_scenedata);
			this->tabPage_scene->Controls->Add(this->listBox_scene);
			this->tabPage_scene->Controls->Add(this->button_removescene);
			this->tabPage_scene->Controls->Add(this->label_importscenelist);
			this->tabPage_scene->Controls->Add(this->button_importscenelist);
			this->tabPage_scene->Controls->Add(this->label_scene);
			this->tabPage_scene->Location = System::Drawing::Point(4, 25);
			this->tabPage_scene->Name = "tabPage_scene";
			this->tabPage_scene->Size = System::Drawing::Size(522, 452);
			this->tabPage_scene->TabIndex = 2;
			this->tabPage_scene->Text = "scene";
			this->tabPage_scene->Enter += gcnew System::EventHandler(this, &SceneSetupForm::tabPage_scene_Enter);
			this->tabPage_scene->Leave += gcnew System::EventHandler(this, &SceneSetupForm::tabPage_scene_Leave);
			// 
			// groupBox_scenedata
			// 
			this->groupBox_scenedata->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupBox_scenedata->Controls->Add(this->label_scenename);
			this->groupBox_scenedata->Controls->Add(this->textBox_scenedesc);
			this->groupBox_scenedata->Controls->Add(this->textBox_scenename);
			this->groupBox_scenedata->Controls->Add(this->label_sceneondesc);
			this->groupBox_scenedata->Location = System::Drawing::Point(3, 309);
			this->groupBox_scenedata->Name = "groupBox_scenedata";
			this->groupBox_scenedata->Size = System::Drawing::Size(516, 138);
			this->groupBox_scenedata->TabIndex = 8;
			this->groupBox_scenedata->TabStop = false;
			this->groupBox_scenedata->Text = "Scene Data";
			// 
			// label_scenename
			// 
			this->label_scenename->Location = System::Drawing::Point(6, 26);
			this->label_scenename->Name = "label_scenename";
			this->label_scenename->Size = System::Drawing::Size(100, 23);
			this->label_scenename->TabIndex = 4;
			this->label_scenename->Text = "Scene Name";
			// 
			// textBox_scenedesc
			// 
			this->textBox_scenedesc->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textBox_scenedesc->Location = System::Drawing::Point(6, 106);
			this->textBox_scenedesc->Name = "textBox_scenedesc";
			this->textBox_scenedesc->Size = System::Drawing::Size(504, 20);
			this->textBox_scenedesc->TabIndex = 2;
			// 
			// textBox_scenename
			// 
			this->textBox_scenename->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textBox_scenename->Location = System::Drawing::Point(6, 50);
			this->textBox_scenename->Name = "textBox_scenename";
			this->textBox_scenename->Size = System::Drawing::Size(504, 20);
			this->textBox_scenename->TabIndex = 1;
			// 
			// label_sceneondesc
			// 
			this->label_sceneondesc->Location = System::Drawing::Point(6, 82);
			this->label_sceneondesc->Name = "label_sceneondesc";
			this->label_sceneondesc->Size = System::Drawing::Size(124, 23);
			this->label_sceneondesc->TabIndex = 6;
			this->label_sceneondesc->Text = "Scene Description";
			// 
			// listBox_scene
			// 
			this->listBox_scene->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->listBox_scene->Location = System::Drawing::Point(16, 40);
			this->listBox_scene->Name = "listBox_scene";
			this->listBox_scene->Size = System::Drawing::Size(378, 251);
			this->listBox_scene->TabIndex = 7;
			this->listBox_scene->SelectedIndexChanged += gcnew System::EventHandler(this, &SceneSetupForm::listBox_scene_SelectedIndexChanged);
			// 
			// button_removescene
			// 
			this->button_removescene->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->button_removescene->Location = System::Drawing::Point(410, 40);
			this->button_removescene->Name = "button_removescene";
			this->button_removescene->Size = System::Drawing::Size(96, 32);
			this->button_removescene->TabIndex = 6;
			this->button_removescene->Text = "Remove Scene";
			this->button_removescene->Click += gcnew System::EventHandler(this, &SceneSetupForm::button_removescene_Click);
			// 
			// label_importscenelist
			// 
			this->label_importscenelist->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right);
			this->label_importscenelist->Location = System::Drawing::Point(418, 327);
			this->label_importscenelist->Name = "label_importscenelist";
			this->label_importscenelist->Size = System::Drawing::Size(96, 120);
			this->label_importscenelist->TabIndex = 5;
			// 
			// button_importscenelist
			// 
			this->button_importscenelist->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->button_importscenelist->Location = System::Drawing::Point(410, 80);
			this->button_importscenelist->Name = "button_importscenelist";
			this->button_importscenelist->Size = System::Drawing::Size(96, 32);
			this->button_importscenelist->TabIndex = 4;
			this->button_importscenelist->Text = "Import Scene List";
			this->button_importscenelist->Click += gcnew System::EventHandler(this, &SceneSetupForm::button_importscenelist_Click);
			// 
			// label_scene
			// 
			this->label_scene->Location = System::Drawing::Point(16, 16);
			this->label_scene->Name = "label_scene";
			this->label_scene->Size = System::Drawing::Size(160, 24);
			this->label_scene->TabIndex = 2;
			this->label_scene->Text = "Pick the scene";
			// 
			// tabPage_summary
			// 
			this->tabPage_summary->Controls->Add(this->button_createdirs);
			this->tabPage_summary->Controls->Add(this->groupBox_summary);
			this->tabPage_summary->Location = System::Drawing::Point(4, 25);
			this->tabPage_summary->Name = "tabPage_summary";
			this->tabPage_summary->Size = System::Drawing::Size(522, 452);
			this->tabPage_summary->TabIndex = 4;
			this->tabPage_summary->Text = "summary";
			this->tabPage_summary->Enter += gcnew System::EventHandler(this, &SceneSetupForm::tabPage_summary_Enter);
			// 
			// button_createdirs
			// 
			this->button_createdirs->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_createdirs->Location = System::Drawing::Point(144, 407);
			this->button_createdirs->Name = "button_createdirs";
			this->button_createdirs->Size = System::Drawing::Size(112, 32);
			this->button_createdirs->TabIndex = 0;
			this->button_createdirs->Text = "Create Directories";
			this->button_createdirs->Click += gcnew System::EventHandler(this, &SceneSetupForm::button_createdirs_Click);
			// 
			// groupBox_summary
			// 
			this->groupBox_summary->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupBox_summary->Controls->Add(this->label_summary);
			this->groupBox_summary->Location = System::Drawing::Point(8, 8);
			this->groupBox_summary->Name = "groupBox_summary";
			this->groupBox_summary->Size = System::Drawing::Size(498, 391);
			this->groupBox_summary->TabIndex = 2;
			this->groupBox_summary->TabStop = false;
			this->groupBox_summary->Text = "Project-Scene Summary";
			// 
			// label_summary
			// 
			this->label_summary->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->label_summary->Location = System::Drawing::Point(16, 16);
			this->label_summary->Name = "label_summary";
			this->label_summary->Size = System::Drawing::Size(466, 367);
			this->label_summary->TabIndex = 1;
			// 
			// button_prev
			// 
			this->button_prev->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_prev->Location = System::Drawing::Point(112, 496);
			this->button_prev->Name = "button_prev";
			this->button_prev->Size = System::Drawing::Size(75, 23);
			this->button_prev->TabIndex = 1;
			this->button_prev->Text = "previous";
			this->button_prev->Click += gcnew System::EventHandler(this, &SceneSetupForm::button_prev_Click);
			// 
			// button_next
			// 
			this->button_next->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_next->Location = System::Drawing::Point(232, 496);
			this->button_next->Name = "button_next";
			this->button_next->Size = System::Drawing::Size(75, 23);
			this->button_next->TabIndex = 2;
			this->button_next->Text = "next";
			this->button_next->Click += gcnew System::EventHandler(this, &SceneSetupForm::button_next_Click);
			// 
			// button_finish
			// 
			this->button_finish->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_finish->Location = System::Drawing::Point(232, 495);
			this->button_finish->Name = "button_finish";
			this->button_finish->Size = System::Drawing::Size(75, 23);
			this->button_finish->TabIndex = 3;
			this->button_finish->Text = "finish";
			this->button_finish->Click += gcnew System::EventHandler(this, &SceneSetupForm::button_finish_Click);
			// 
			// SceneSetupForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(548, 527);
			this->Controls->Add(this->button_next);
			this->Controls->Add(this->button_prev);
			this->Controls->Add(this->tabControl_scenesetup);
			this->Controls->Add(this->button_finish);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->MinimumSize = System::Drawing::Size(424, 360);
			this->Name = "SceneSetupForm";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = "Scene SetUp";
			this->TopMost = true;
			this->tabControl_scenesetup->ResumeLayout(false);
			this->tabPage_project->ResumeLayout(false);
			this->groupBox_projectdata->ResumeLayout(false);
			this->groupBox_projectdata->PerformLayout();
			this->tabPage_scene->ResumeLayout(false);
			this->groupBox_scenedata->ResumeLayout(false);
			this->groupBox_scenedata->PerformLayout();
			this->tabPage_summary->ResumeLayout(false);
			this->groupBox_summary->ResumeLayout(false);
			this->ResumeLayout(false);

		}



	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void SetComponentInitialValues()
	{
		m_bDataInitialized = false;

		//	configure the GUI with the data
		//
		m_SceneSetupData = SceneSetupDialogUtil::Data();
		m_SceneSetupData.m_Data.m_bFinished = false;

		PrefsData prefsdata = PrefsMgr::Data();
		m_SceneSetupData.m_Data.m_ProjectName = prefsdata.m_LastProjectName.GetValue();

		//
		tabPage_scene->Enabled = false;

		tabControl_scenesetup->SelectedIndex = 0;

		//	set the text for the label describing the import file format.
		String^ msg;
		msg = msg->Concat(	"Import file format:\n\n", 
							"  scene1\n",
							"  scene2\n",
							"  ..." );
		this->label_importscenelist->Text = msg;

		// build the project list
		build_project_list();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void SetDataDirtyFlag()
	{
		//	set the data as "dirty"
		ProjectSetupData& PSData = ProjectSetupMgr::Data();
		PSData.m_bDirty = true;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void tabPage_project_Enter(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if ( !m_bDataInitialized )
			{
				m_bDataInitialized = true;

				if ( m_SceneSetupData.m_Data.m_ProjectName.size() > 0 )
				{
					int index = listBox_project->FindString(gcnew System::String( m_SceneSetupData.m_Data.m_ProjectName.c_str() ));
					listBox_project->SelectedIndex = index;

					//DBG_LOG2( "selecting string at index %d-(%s)", index, m_SceneSetupData.m_Data.m_ProjectName.c_str() );
				}
			}

			Set_button_prev_Visible( false );
			Set_button_next_Visible( true );
			Set_button_finish_Visible( false );

			//	if this is a new project then let the user change the name of the project.
			//
			this->textBox_projectname->Enabled = (listBox_project->SelectedIndex == 0);
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void tabPage_project_Leave(System::Object^  sender, System::EventArgs^  e)
		 {
			 SetProjectData();
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void listBox_project_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			std::string strProjFile;
			tmaManagedStringUtils::ManagedStringToStdString( listBox_project->Text, strProjFile );

			if ( listBox_project->SelectedIndex != 0 )
			{
				//	Read the Project File
				//
				ProjectSetupMgr::ReadProject( itString( strProjFile.c_str() ) );

				SetDataDirtyFlag();		//	set the data as "dirty"
			}
			else
			{
				//	"new project" was selected so clear out the scene list + data
				//
				ProjectSetupData& PSData = ProjectSetupMgr::Data();
				PSData.m_Scenes.clear();
				PSData.m_ProjectName = "New";
				PSData.m_CurrentSceneName = "";
				PSData.m_ProjectDesc = "";
				PSData.m_ProjectDirectory = "c:\\Projects";
			}

			SetSceneSetupData();
			SetProjectControls();
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void textBox_projectname_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			SetDataDirtyFlag();		//	set the data as "dirty"
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void textBox_projectdesc_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			SetDataDirtyFlag();		//	set the data as "dirty"
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void SetControlsBasedOnProjectDir()
		 {
			Set_button_next_Visible( true );

			if (folderChooser_projectdir->Directory->Length == 0)
			{
				button_next->Enabled = false;

				tabPage_scene->Enabled = false;
				tabPage_summary->Enabled = false;
			}
			else
			{
				tabPage_scene->Enabled = true;
				tabPage_summary->Enabled = true;
			}
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void folderChooser_projectdir_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			SetControlsBasedOnProjectDir();
			SetDataDirtyFlag();		//	set the data as "dirty"
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void SetSceneSetupData()
		 {
			//	Fill-in the Scene Data
			//
			ProjectSetupData& PSData = ProjectSetupMgr::Data();
			m_SceneSetupData.m_Data.m_ProjectName		= PSData.m_ProjectName;
			m_SceneSetupData.m_Data.m_ProjectDirectory	= PSData.m_ProjectDirectory;
			m_SceneSetupData.m_Data.m_SceneName			= PSData.m_CurrentSceneName;
		 }

		 System::Void SetProjectControls()
		 {
			ProjectSetupData& PSData = ProjectSetupMgr::Data();

			textBox_projectname->Text = gcnew System::String( PSData.m_ProjectName.c_str() );
			textBox_projectdesc->Text = gcnew System::String( PSData.m_ProjectDesc.c_str() );
			folderChooser_projectdir->Directory = gcnew System::String( PSData.m_ProjectDirectory.c_str() );

			this->textBox_projectname->Enabled = (listBox_project->SelectedIndex == 0);
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: void tabPage_scene_setup()
		{
			tabPage_scene->Enabled = true;

			build_scene_list();

			if ( m_SceneSetupData.m_Data.m_SceneName.size() > 0 )
			{
				int index = listBox_scene->FindString( gcnew System::String( m_SceneSetupData.m_Data.m_SceneName.c_str() ) );
				listBox_scene->SelectedIndex = index;
				listBox_scene->Focus();
			}

			Set_button_prev_Visible( true );
			Set_button_next_Visible( true );
			Set_button_finish_Visible( false );
		}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void tabControl_scenesetup_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (tabControl_scenesetup->SelectedTab == tabPage_scene)
			{
				tabPage_scene_setup();
			}
			else if (tabControl_scenesetup->SelectedTab == tabPage_summary)
			{
				tabPage_summary_setup();
			}
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void tabPage_scene_Enter(System::Object ^  sender, System::EventArgs ^  e)
		 {
			tabPage_scene_setup();

			//	if this is a new project then let the user change the name of the project.
			//
			this->textBox_scenename->Enabled = (this->listBox_scene->SelectedIndex == 0);
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void tabPage_scene_Leave(System::Object^  sender, System::EventArgs^  e)
		 {
			 SetSceneData();
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void SetSceneControls()
		 {
			 int index = listBox_scene->SelectedIndex - 1;
			 if ( index >= 0 )
			 {
 				ProjectSetupData& PSData = ProjectSetupMgr::Data();

				textBox_scenename->Text = gcnew System::String( PSData.m_Scenes[index].m_SceneName.c_str() );
				textBox_scenedesc->Text = gcnew System::String( PSData.m_Scenes[index].m_SceneDesc.c_str() );
			 }
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void listBox_scene_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			//	if scenes combo has "new scene" selected then add it to the project
			//
			if ( listBox_scene->SelectedIndex != 0 )
			{
				tmaManagedStringUtils::ManagedStringToStdString( listBox_scene->Text, m_SceneSetupData.m_Data.m_SceneName );
				ProjectSetupData& PSData = ProjectSetupMgr::Data();
				PSData.m_CurrentSceneName = m_SceneSetupData.m_Data.m_SceneName;
			}
			this->textBox_scenename->Enabled = (this->listBox_scene->SelectedIndex == 0);

			SetSceneControls();
			SetDataDirtyFlag();		//	set the data as "dirty"
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void textBox_scenename_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			SetDataDirtyFlag();		//	set the data as "dirty"
		 }


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void textBox_scenedesc_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			SetDataDirtyFlag();		//	set the data as "dirty"
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void button_removescene_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 int index = listBox_scene->SelectedIndex - 1;
			 if ( index >= 0 )
			 {
				ProjectSetupMgr::RemoveSceneFromProject(index);

				//	now rebuild the scene list
				build_scene_list();
			 }
		 }


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void button_importscenelist_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 //Note: this functionality is now in ProjectSetupMgr. Please use
			 // that version if changes are needed.

			 //	let the user browse for the file and then open/parse it.
			 //
			System::Windows::Forms::OpenFileDialog^ dialog = gcnew System::Windows::Forms::OpenFileDialog();
			dialog->Filter = "Scene List files (*.txt)|*.txt|All files (*.*)|*.*";
			dialog->InitialDirectory = tmaManagedStringUtils::LocatorToManagedString( gfPaths::GetAppPath() );
			if (dialog->ShowDialog() == ::DialogResult::OK)
			{
				fsLocator full_path;
				tmaManagedStringUtils::ManagedStringToLocator(dialog->FileName, full_path);
				itString filename = full_path.GetLastName();

				//	open the file and parse it
				//
				try
				{
					std::string one_line;
					gfFileTxt txt_file( full_path, fsFileStream::e_ReadOnly);
					while ( txt_file.ReadLine( one_line ) )
					{
						//DBG_LOG1( "reading scene line (%s)", one_line.c_str() );

						//	add the scene if it doesn't already exist
						//
						itStringUtil::TrimSpaces(one_line);
						AddSceneToProject( one_line );
					}

					//	now rebuild the scene list
					//
					build_scene_list();
				}
				catch( const fsFileDoesntExistX& i_Ex )
				{
					std::string filename;
					fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
					DBG_LOG1("fsFileDoesntExistX: %s", filename.c_str());
					std::string msg = "File does not exist: " + filename;
					MessageBox::Show(gcnew System::String(msg.c_str()), "Error");
				}
				catch( const fsDirectoryDoesntExistX& i_Ex )
				{
					std::string filename;
					fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
					DBG_LOG1("fsDirectoryDoesntExistX: %s", filename.c_str());
					std::string msg = "Directory does not exist: " + filename;
					MessageBox::Show(gcnew System::String(msg.c_str()), "Error");
				}
				catch( const fsReadOnlyX& i_Ex )
				{
					std::string filename;
					fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
					DBG_LOG1("fsReadOnlyX: %s", filename.c_str());
					std::string msg = "File does not exist or is read-only: " + filename;
					MessageBox::Show(gcnew System::String(msg.c_str()), "Error");
				}
				catch( ... )
				{
					MessageBox::Show("General exception error", "Error");
				    delete dialog;
					throw;
				}
			}

			SetDataDirtyFlag();		//	set the data as "dirty"
		    delete dialog;
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: void tabPage_summary_setup()
		{
			Set_button_prev_Visible( true );
			Set_button_next_Visible( false );
			Set_button_finish_Visible( true );

			SetData();

			//	scene-specific data
			ProjectSetupData& PSData = ProjectSetupMgr::Data();

			String^ msg;
			msg = msg->Concat(	"Project: ",
								gcnew String( m_SceneSetupData.m_Data.m_ProjectName.c_str() ),
								"\n\nDirectory: ",
								gcnew String( m_SceneSetupData.m_Data.m_ProjectDirectory.c_str() ), 
								"\n\nScene: ",
								gcnew String( PSData.m_CurrentSceneName.c_str() ),
								"\n" );
			this->label_summary->Text = msg;
		}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void tabPage_summary_Enter(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 tabPage_summary_setup();
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void button_createdirs_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 //Note: this functionality is now in ProjectSetupMgr. Please use
			 // that version if changes are needed.

			fsLocator dir;
			ProjectSetupData& PSData = ProjectSetupMgr::Data();
			fsFileUtil::ANSIFilenameToLocator(PSData.m_ProjectDirectory.c_str(), dir);

			//	set-up the main + stock project directories (if necessary)
			//
			mnmPaths::CreateProjectDirectories( dir );

			//	loop through each scene in the list and check if it has been created.
			//	if not, create it.
			//
			int i;
			int size = PSData.m_Scenes.size();
			for ( i = 0 ; i < size ; i++ )
			{
				try
				{
					//DBG_LOG1("Creating (%s)", PSData.m_Scenes[i].m_SceneName.c_str());
					mnmPaths::CreateSceneDirectories( dir, 
													itString(PSData.m_Scenes[i].m_SceneName.c_str()) );
				}
				catch( const fsDirectoryDoesntExistX& i_Ex )
				{
					std::string filename;
					fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
					DBG_WARNING1("fsDirectoryDoesntExistX: %s", filename.c_str());
					std::string msg = "Directory does not exist: " + filename;
					MessageBox::Show(gcnew System::String(msg.c_str()), "Error");
				}
			}
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void button_finish_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			m_bDataInitialized = false;

			//	grab the information from the GUI and put it in the data
			//
			FinalizeData();

			//	if this is a NEW project then ask the user to create the directories
			//
			if ( listBox_project->SelectedIndex == 0 )
			{
				int retval = guiMessageBox::Show( "This is a new project do you want to create the directories for this project?","New Project Creation", guiMessageBox::e_YesNo );
				if ( retval == guiMessageBox::e_Yes )
				{
					fsLocator dir;
					fsFileUtil::ANSIFilenameToLocator(m_SceneSetupData.m_Data.m_ProjectDirectory, dir );
					mnmPaths::CreateProjectDirectories( dir );
				}
			}

			//
			this->Close();
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void button_prev_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			select_prev_tab();
		 }

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
private: System::Void button_next_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			select_next_tab();
		 }

private:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void Set_button_prev_Visible( bool i_bVisible )
	{
		button_prev->Enabled =  i_bVisible;
		button_prev->Visible =  i_bVisible;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void Set_button_next_Visible( bool i_bVisible )
	{
		button_next->Enabled =  i_bVisible;
		button_next->Visible =  i_bVisible;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void Set_button_finish_Visible( bool i_bVisible )
	{
		button_finish->Enabled =  i_bVisible;
		button_finish->Visible =  i_bVisible;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void select_next_tab()
	{
		int index = tabControl_scenesetup->SelectedIndex;

		//switch (index)
		//{
		//	case 0: // project
		//		{
		//			int cbpindex = listBox_project->SelectedIndex;
		//			if ( cbpindex == 0 )
		//				tabPage_newproject->set_Visible( true );
		//			else
		//				index++;
		//		}
		//		break;
		//	case 1: // new project
		//		break;
			//case 2: // scene
			//	{
			//		int cblindex = listBox_scene->SelectedIndex;
			//		if ( cblindex == 1 )	// "No Scene"
			//			index++;
			//		break;
			//	}
		//	case 3: // new scene
		//		break;
		//	case 4: // scene
		//		break;
		//}
		index++;

		if ( index >= (tabControl_scenesetup->TabCount-1) )
		{
			//index = 0;
			index = tabControl_scenesetup->TabCount - 1;
		}

		tabControl_scenesetup->SelectedIndex = index;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void select_prev_tab()
	{
		int index = tabControl_scenesetup->SelectedIndex;

		//switch (index)
		//{
		//	case 0: // project
		//		break;
		//	case 1: // new project
		//		break;
		//	case 2: // scene
		//		{
		//			int cbpindex = listBox_project->SelectedIndex;
		//			if ( cbpindex == 0 )
		//				tabPage_newproject->set_Visible( true );
		//			else
		//				index--;
		//		}
		//		break;
		//	case 3: // new scene
		//		break;
			//case 4: // scene
			//	{
			//		int cblindex = listBox_scene->SelectedIndex;
			//		if ( cblindex == 1 )	// "No Scene"
			//			index--;
			//		break;
			//	}
		//}

		index--;

		if ( index <= 0 )
		{
			index = 0;
		}

		tabControl_scenesetup->SelectedIndex = index;
	}

	//------------------------------------------------------------------------
	//	add a scene to the project data.
	//------------------------------------------------------------------------
	System::Void AddSceneToProject( std::string& i_SceneName )
	{
		//	if the scene name is empty don't add it.
		//
		if (i_SceneName.length() == 0)
			return;

		ProjectSetupData& PSData = ProjectSetupMgr::Data();

		bool bFound = false;
		int size = PSData.m_Scenes.size();

		int i;
		for (i=0; i< size; ++i)
		{
			if ( strcmp( i_SceneName.c_str(), PSData.m_Scenes[i].m_SceneName.c_str() ) == 0 )
			{
				//DBG_LOG1( "already found scene %s", i_SceneName.c_str() );

				bFound = true;		// already exists, skip it.
				break;
			}
		}

		if ( !bFound )
		{
			//DBG_LOG1( "adding scene (%s)", i_SceneName.c_str() );

			PSData.m_Scenes.resize( size + 1 );
			PSData.m_Scenes[size].m_SceneName = i_SceneName;
			PSData.m_CurrentSceneName = i_SceneName;
			tmaManagedStringUtils::ManagedStringToStdString( textBox_scenedesc->Text, i_SceneName );
			PSData.m_Scenes[size].m_SceneDesc = i_SceneName;

			SetDataDirtyFlag();
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void build_project_list()
	{
		listBox_project->Items->Clear();

		//	set the built-in options
		//
		listBox_project->Items->Add( "New Project" );

		//	grab the saved ones
		fsFileEnum::fsFileList filelist;
		fsFileEnum::EnumerateFiles( gfPaths::GetPath(mnmPaths::e_SaveProjectFiles), filelist, &itString(".mpj") );

		// build the list
		int filenum = filelist.size();
		for ( int i = 0 ; i < filenum ; i++ )
		{
			itString name = filelist[i].GetLastName();
			listBox_project->Items->Add( tmaManagedStringUtils::ItStringToManagedString(name)->Replace(".mpj", "") );

			//DBG_LOG3( "build project list %d of %d (%s)", i, filenum, itStringUtil::GetStdString(name).c_str() );
		}

		if (m_SceneSetupData.m_Data.m_ProjectName.size() > 0)
		{
			int index = listBox_project->FindString(gcnew System::String( m_SceneSetupData.m_Data.m_ProjectName.c_str() ));
			//DBG_LOG2( "selecting string at index %d-(%s)", index, m_SceneSetupData.m_Data.m_ProjectName.c_str() );
			listBox_project->SelectedIndex = index;
			listBox_project->Focus();
		}
		else
		{
			listBox_project->SelectedIndex = 0;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void build_scene_list()
	{
		listBox_scene->Items->Clear();

		//	set the built-in options
		listBox_scene->Items->Add( "New Scene" );	// index 0	

		listBox_scene->SelectedIndex = 0;

		//	add the scenes to the combobox also
		ProjectSetupData& PSData = ProjectSetupMgr::Data();
		int i;
		for ( i = 0 ; i < PSData.m_Scenes.size() ; i++ )
		{
			listBox_scene->Items->Add( gcnew String(PSData.m_Scenes[i].m_SceneName.c_str()) );
		}

		listBox_scene->Invalidate();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void SetProjectData()
	{
		ProjectSetupData& PSData = ProjectSetupMgr::Data();

		//	project-specific data
		if ( listBox_project->SelectedIndex == 0 )
		{
			tmaManagedStringUtils::ManagedStringToStdString( textBox_projectname->Text, m_SceneSetupData.m_Data.m_ProjectName );
		}
		else
		{
			tmaManagedStringUtils::ManagedStringToStdString( listBox_project->Text, m_SceneSetupData.m_Data.m_ProjectName );
		}
		tmaManagedStringUtils::ManagedStringToStdString( folderChooser_projectdir->Directory, m_SceneSetupData.m_Data.m_ProjectDirectory );

		//	if projects combo has "new project" selected then add it to the project
		if ( listBox_project->SelectedIndex == 0 )
		{
			PSData.m_ProjectName = m_SceneSetupData.m_Data.m_ProjectName;
			tmaManagedStringUtils::ManagedStringToStdString( textBox_projectname->Text, PSData.m_ProjectName );
			tmaManagedStringUtils::ManagedStringToStdString( textBox_projectdesc->Text, PSData.m_ProjectDesc );
			tmaManagedStringUtils::ManagedStringToStdString( folderChooser_projectdir->Directory, PSData.m_ProjectDirectory );

			SetDataDirtyFlag();
		}
		else
		{
			// if not new, check if any of the data has changed
			//
			std::string info;
			tmaManagedStringUtils::ManagedStringToStdString( textBox_projectdesc->Text, info );
			if (info != PSData.m_ProjectDesc)
			{
				PSData.m_ProjectDesc = info;
				SetDataDirtyFlag();
			}
			if (m_SceneSetupData.m_Data.m_ProjectDirectory != PSData.m_ProjectDirectory)
			{
				PSData.m_ProjectDirectory = m_SceneSetupData.m_Data.m_ProjectDirectory;
				SetDataDirtyFlag();
			}
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void SetSceneData()
	{
		ProjectSetupData& PSData = ProjectSetupMgr::Data();

		//	scene-specific data
		if ( listBox_scene->SelectedIndex != 0 )
		{
			tmaManagedStringUtils::ManagedStringToStdString( listBox_scene->Text, m_SceneSetupData.m_Data.m_SceneName );
		}
		else
		{
			tmaManagedStringUtils::ManagedStringToStdString( textBox_scenename->Text, m_SceneSetupData.m_Data.m_SceneName );
		}

		//	if scenes combo has "new scene" selected then add it to the project
		//
		if ( listBox_scene->SelectedIndex == 0 )
		{
			std::string tempstring;
			tmaManagedStringUtils::ManagedStringToStdString( textBox_scenename->Text, tempstring );

			AddSceneToProject( tempstring );
			SetDataDirtyFlag();
		}
		else
		{
			PSData.m_CurrentSceneName = m_SceneSetupData.m_Data.m_SceneName;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void SetData()
	{
		SetProjectData();
		SetSceneData();

		// Write out the prefs
		//
		if ( listBox_project->SelectedIndex != 0 )
		{
			std::string tempstring;
			tmaManagedStringUtils::ManagedStringToStdString( this->textBox_projectname->Text, tempstring );

			PrefsData& prefsdata = PrefsMgr::Data();
			prefsdata.m_LastProjectName = tempstring;
			PrefsMgr::WritePrefs();
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void FinalizeData()
	{
		SetData();

		m_SceneSetupData.m_Data.m_bFinished = true;

		//
		//	Write the Project File
		//
		ProjectSetupData& PSData = ProjectSetupMgr::Data();
		if (PSData.m_bDirty)
		{
			ProjectSetupMgr::WriteProject( itString( PSData.m_ProjectName.c_str() ) );

			PSData.m_bDirty = false;
		}

		//	set up the initial directory
		//
		itString scene_name(m_SceneSetupData.m_Data.m_SceneName.c_str());
		fsLocator PDir;
		fsFileUtil::ANSIFilenameToLocator( m_SceneSetupData.m_Data.m_ProjectDirectory, PDir );
		mnmPaths::SetupPaths( PDir, scene_name );

		guiSingleDocHandler::SetInitialDirectory( gfPaths::GetPath( mnmPaths::e_SaveShots ) );
	}

};
}

#endif // _MANAGED
