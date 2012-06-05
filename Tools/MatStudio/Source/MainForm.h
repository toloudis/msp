#pragma once

#ifndef MRU_MANAGER_HPP
#include "MRUManager.hpp"
#endif
#ifndef TEMPLATE_MANAGER_HPP
#include "TemplateManager.hpp"
#endif

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace MatStudio
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

		void CreateTemplatesMenu()
		{
			// Material templates list
			m_pTemplateManager = new TemplateManager( this->menuTemplates );
		}
        
		System::Windows::Forms::Panel *  GetRenderWindow()
		{
			return panelRender;
		}

		System::Windows::Forms::Panel *  GetInitRenderWindow()
		{
			return panelInit;
		}

		void ClearMaterialNames()
		{
			this->comboBox_materials->Items->Clear();
		}

		void AddMaterialName(const char * i_Name)
		{
			this->comboBox_materials->Items->Add(new String(i_Name));
		}

		void SelectMaterial(int i_Index)
		{
			this->comboBox_materials->SelectedIndex = i_Index;
		}

		void EnableUI(bool i_bEnabled)
		{
			this->buttonEditMaterial->Enabled = i_bEnabled;
		}

	public: void UpdateTitleBar();

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

	private: System::Windows::Forms::MainMenu *  mainMenu1;
	private: System::Windows::Forms::Panel *  panelInit;
	private: System::Windows::Forms::Panel *  panelRender;

	private: System::Windows::Forms::MenuItem *  menuFileExit;
	private: System::Windows::Forms::MenuItem *  menuFileNew;
	private: System::Windows::Forms::MenuItem *  menuFileOpen;
	private: System::Windows::Forms::MenuItem *  menuItem_file_recent;
	private: System::Windows::Forms::Button *  buttonEditMaterial;
	private: System::Windows::Forms::CheckBox *  checkHighlight;
	private: System::Windows::Forms::MenuItem *  menuFileSave;
	private: System::Windows::Forms::MenuItem *  menuFileSaveAs;
	private: System::Windows::Forms::MenuItem *  menuView;
	private: System::Windows::Forms::MenuItem *  menuViewShaders;
	private: System::Windows::Forms::MenuItem *  menuViewShadows;
	private: System::Windows::Forms::MenuItem *  menuWindowLights;
	private: System::Windows::Forms::MenuItem *  menuHelpAbout;
	private: System::Windows::Forms::MenuItem *  menuViewResetCamera;
	private: System::Windows::Forms::MenuItem *  menuEdit;
	private: System::Windows::Forms::MenuItem *  menuEditCopyMaterial;
	private: System::Windows::Forms::MenuItem *  menuEditPasteMaterial;
	private: System::Windows::Forms::MenuItem *  menuEditImportMaterials;
	private: System::Windows::Forms::MenuItem *  menuEditReloadTextures;
	private: System::Windows::Forms::MenuItem *  menuEditStaticCube;
	private: System::Windows::Forms::MenuItem *  menuTemplates;
	private: System::Windows::Forms::MenuItem *  menuItem_window;
	private: System::Windows::Forms::MenuItem *  menuItem_help;
	private: System::Windows::Forms::ComboBox *  comboBox_materials;
	private: System::Windows::Forms::Label *  label_materials;
	private: System::Windows::Forms::MenuItem *  menuItem_file;
	private: System::Windows::Forms::MenuItem *  menuItem_MRUsub;
	private: System::Windows::Forms::MenuItem *  menuItem_seperator1;
	private: System::Windows::Forms::MenuItem *  menuItem_seperator2;

	private: System::ComponentModel::IContainer *  components;

	private: MRUManager* m_pMRUManager;				// MRU list manager
	private: TemplateManager* m_pTemplateManager;	// Template list manager

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
			this->menuItem_MRUsub = new System::Windows::Forms::MenuItem();
			this->menuItem_seperator2 = new System::Windows::Forms::MenuItem();
			this->menuFileExit = new System::Windows::Forms::MenuItem();
			this->menuEdit = new System::Windows::Forms::MenuItem();
			this->menuEditCopyMaterial = new System::Windows::Forms::MenuItem();
			this->menuEditPasteMaterial = new System::Windows::Forms::MenuItem();
			this->menuEditImportMaterials = new System::Windows::Forms::MenuItem();
			this->menuEditReloadTextures = new System::Windows::Forms::MenuItem();
			this->menuEditStaticCube = new System::Windows::Forms::MenuItem();
			this->menuView = new System::Windows::Forms::MenuItem();
			this->menuViewShaders = new System::Windows::Forms::MenuItem();
			this->menuViewShadows = new System::Windows::Forms::MenuItem();
			this->menuViewResetCamera = new System::Windows::Forms::MenuItem();
			this->menuTemplates = new System::Windows::Forms::MenuItem();
			this->menuItem_window = new System::Windows::Forms::MenuItem();
			this->menuWindowLights = new System::Windows::Forms::MenuItem();
			this->menuItem_help = new System::Windows::Forms::MenuItem();
			this->menuHelpAbout = new System::Windows::Forms::MenuItem();
			this->panelInit = new System::Windows::Forms::Panel();
			this->panelRender = new System::Windows::Forms::Panel();
			this->comboBox_materials = new System::Windows::Forms::ComboBox();
			this->label_materials = new System::Windows::Forms::Label();
			this->buttonEditMaterial = new System::Windows::Forms::Button();
			this->checkHighlight = new System::Windows::Forms::CheckBox();
			this->SuspendLayout();
			// 
			// mainMenu1
			// 
			System::Windows::Forms::MenuItem* __mcTemp__1[] = new System::Windows::Forms::MenuItem*[6];
			__mcTemp__1[0] = this->menuItem_file;
			__mcTemp__1[1] = this->menuEdit;
			__mcTemp__1[2] = this->menuView;
			__mcTemp__1[3] = this->menuTemplates;
			__mcTemp__1[4] = this->menuItem_window;
			__mcTemp__1[5] = this->menuItem_help;
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
			this->menuFileNew->Click += new System::EventHandler(this, menuFileNew_Click);
			// 
			// menuFileOpen
			// 
			this->menuFileOpen->Index = 1;
			this->menuFileOpen->Text = S"Open";
			this->menuFileOpen->Click += new System::EventHandler(this, menuFileOpen_Click);
			// 
			// menuFileSave
			// 
			this->menuFileSave->Index = 2;
			this->menuFileSave->Text = S"Save";
			this->menuFileSave->Click += new System::EventHandler(this, menuFileSave_Click);
			// 
			// menuFileSaveAs
			// 
			this->menuFileSaveAs->Index = 3;
			this->menuFileSaveAs->Text = S"Save as...";
			this->menuFileSaveAs->Click += new System::EventHandler(this, menuFileSaveAs_Click);
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
			__mcTemp__3[0] = this->menuItem_MRUsub;
			this->menuItem_file_recent->MenuItems->AddRange(__mcTemp__3);
			this->menuItem_file_recent->Text = S"Recent Files";
			// 
			// menuItem_MRUsub
			// 
			this->menuItem_MRUsub->Index = 0;
			this->menuItem_MRUsub->Text = S"MRU SubMenu";
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
			this->menuFileExit->Click += new System::EventHandler(this, menuFileExit_Click);
			// 
			// menuEdit
			// 
			this->menuEdit->Index = 1;
			System::Windows::Forms::MenuItem* __mcTemp__4[] = new System::Windows::Forms::MenuItem*[5];
			__mcTemp__4[0] = this->menuEditCopyMaterial;
			__mcTemp__4[1] = this->menuEditPasteMaterial;
			__mcTemp__4[2] = this->menuEditImportMaterials;
			__mcTemp__4[3] = this->menuEditReloadTextures;
			__mcTemp__4[4] = this->menuEditStaticCube;
			this->menuEdit->MenuItems->AddRange(__mcTemp__4);
			this->menuEdit->Text = S"Edit";
			this->menuEdit->Popup += new System::EventHandler(this, menuEdit_Popup);
			// 
			// menuEditCopyMaterial
			// 
			this->menuEditCopyMaterial->Index = 0;
			this->menuEditCopyMaterial->Shortcut = System::Windows::Forms::Shortcut::CtrlC;
			this->menuEditCopyMaterial->Text = S"Copy Material";
			this->menuEditCopyMaterial->Click += new System::EventHandler(this, menuEditCopyMaterial_Click);
			// 
			// menuEditPasteMaterial
			// 
			this->menuEditPasteMaterial->Index = 1;
			this->menuEditPasteMaterial->Shortcut = System::Windows::Forms::Shortcut::CtrlV;
			this->menuEditPasteMaterial->Text = S"Paste Material";
			this->menuEditPasteMaterial->Click += new System::EventHandler(this, menuEditPasteMaterial_Click);
			// 
			// menuEditImportMaterials
			// 
			this->menuEditImportMaterials->Index = 2;
			this->menuEditImportMaterials->Text = S"Import Materials...";
			this->menuEditImportMaterials->Click += new System::EventHandler(this, menuEditImportMaterials_Click);
			// 
			// menuEditReloadTextures
			// 
			this->menuEditReloadTextures->Index = 3;
			this->menuEditReloadTextures->Text = S"Reload Textures";
			this->menuEditReloadTextures->Click += new System::EventHandler(this, menuEditReloadTextures_Click);
			// 
			// menuEditStaticCube
			// 
			this->menuEditStaticCube->Index = 4;
			this->menuEditStaticCube->Text = S"Create Static Cube Texture...";
			this->menuEditStaticCube->Click += new System::EventHandler(this, menuEditStaticCube_Click);
			// 
			// menuView
			// 
			this->menuView->Index = 2;
			System::Windows::Forms::MenuItem* __mcTemp__5[] = new System::Windows::Forms::MenuItem*[3];
			__mcTemp__5[0] = this->menuViewShaders;
			__mcTemp__5[1] = this->menuViewShadows;
			__mcTemp__5[2] = this->menuViewResetCamera;
			this->menuView->MenuItems->AddRange(__mcTemp__5);
			this->menuView->Text = S"View";
			this->menuView->Popup += new System::EventHandler(this, menuView_Popup);
			// 
			// menuViewShaders
			// 
			this->menuViewShaders->Index = 0;
			this->menuViewShaders->Text = S"Use Shader Array";
			this->menuViewShaders->Click += new System::EventHandler(this, menuViewShaders_Click);
			// 
			// menuViewShadows
			// 
			this->menuViewShadows->Index = 1;
			this->menuViewShadows->Text = S"Shadows";
			this->menuViewShadows->Click += new System::EventHandler(this, menuViewShadows_Click);
			// 
			// menuViewResetCamera
			// 
			this->menuViewResetCamera->Index = 2;
			this->menuViewResetCamera->Text = S"Reset Camera";
			this->menuViewResetCamera->Click += new System::EventHandler(this, menuViewResetCamera_Click);
			// 
			// menuTemplates
			// 
			this->menuTemplates->Index = 3;
			this->menuTemplates->Text = S"Templates";
			// 
			// menuItem_window
			// 
			this->menuItem_window->Index = 4;
			System::Windows::Forms::MenuItem* __mcTemp__6[] = new System::Windows::Forms::MenuItem*[1];
			__mcTemp__6[0] = this->menuWindowLights;
			this->menuItem_window->MenuItems->AddRange(__mcTemp__6);
			this->menuItem_window->Text = S"Window";
			// 
			// menuWindowLights
			// 
			this->menuWindowLights->Index = 0;
			this->menuWindowLights->Text = S"Lights...";
			this->menuWindowLights->Click += new System::EventHandler(this, menuWindowLights_Click);
			// 
			// menuItem_help
			// 
			this->menuItem_help->Index = 5;
			System::Windows::Forms::MenuItem* __mcTemp__7[] = new System::Windows::Forms::MenuItem*[1];
			__mcTemp__7[0] = this->menuHelpAbout;
			this->menuItem_help->MenuItems->AddRange(__mcTemp__7);
			this->menuItem_help->Text = S"Help";
			// 
			// menuHelpAbout
			// 
			this->menuHelpAbout->Index = 0;
			this->menuHelpAbout->Text = S"About...";
			this->menuHelpAbout->Click += new System::EventHandler(this, menuHelpAbout_Click);
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
			this->panelRender->TabStop = true;
			this->panelRender->Resize += new System::EventHandler(this, panelRender_Resize);
			this->panelRender->MouseEnter += new System::EventHandler(this, panelRender_MouseEnter);
			this->panelRender->MouseMove += new System::Windows::Forms::MouseEventHandler(this, panelRender_MouseMove);
			this->panelRender->MouseLeave += new System::EventHandler(this, panelRender_MouseLeave);
			// 
			// comboBox_materials
			// 
			this->comboBox_materials->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox_materials->ImeMode = System::Windows::Forms::ImeMode::NoControl;
			this->comboBox_materials->Location = System::Drawing::Point(72, 7);
			this->comboBox_materials->Name = S"comboBox_materials";
			this->comboBox_materials->Size = System::Drawing::Size(320, 21);
			this->comboBox_materials->TabIndex = 16;
			this->comboBox_materials->SelectedIndexChanged += new System::EventHandler(this, comboBox_materials_SelectedIndexChanged);
			// 
			// label_materials
			// 
			this->label_materials->Location = System::Drawing::Point(7, 7);
			this->label_materials->Name = S"label_materials";
			this->label_materials->Size = System::Drawing::Size(57, 21);
			this->label_materials->TabIndex = 17;
			this->label_materials->Text = S"Materials:";
			// 
			// buttonEditMaterial
			// 
			this->buttonEditMaterial->Enabled = false;
			this->buttonEditMaterial->Location = System::Drawing::Point(408, 7);
			this->buttonEditMaterial->Name = S"buttonEditMaterial";
			this->buttonEditMaterial->Size = System::Drawing::Size(87, 21);
			this->buttonEditMaterial->TabIndex = 18;
			this->buttonEditMaterial->Text = S"Edit Material";
			this->buttonEditMaterial->Click += new System::EventHandler(this, buttonEditMaterial_Click);
			// 
			// checkHighlight
			// 
			this->checkHighlight->Location = System::Drawing::Point(520, 7);
			this->checkHighlight->Name = S"checkHighlight";
			this->checkHighlight->Size = System::Drawing::Size(113, 21);
			this->checkHighlight->TabIndex = 19;
			this->checkHighlight->Text = S"Highlight Material";
			this->checkHighlight->CheckedChanged += new System::EventHandler(this, checkHighlight_CheckedChanged);
			// 
			// MainForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(679, 565);
			this->Controls->Add(this->checkHighlight);
			this->Controls->Add(this->buttonEditMaterial);
			this->Controls->Add(this->label_materials);
			this->Controls->Add(this->comboBox_materials);
			this->Controls->Add(this->panelRender);
			this->Controls->Add(this->panelInit);
			this->Menu = this->mainMenu1;
			this->Name = S"MainForm";
			this->Text = S"Material Studio";
			this->ResumeLayout(false);
			System::Windows::Forms::Application::Idle += new System::EventHandler(this, OnApplicationIdle);				

		}		
		//

	private: System::Void OnApplicationIdle(System::Object *  sender, System::EventArgs *  e);
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
	private: System::Void comboBox_materials_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e);
	private: System::Void checkHighlight_CheckedChanged(System::Object *  sender, System::EventArgs *  e);
	private: System::Void buttonEditMaterial_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuWindowLights_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuHelpAbout_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuViewResetCamera_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuEdit_Popup(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuEditCopyMaterial_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuEditPasteMaterial_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuEditImportMaterials_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuEditReloadTextures_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void menuEditStaticCube_Click(System::Object *  sender, System::EventArgs *  e);

};
}