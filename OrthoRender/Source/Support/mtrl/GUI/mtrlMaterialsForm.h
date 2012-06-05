/********************************************************************************************\
**  mtrlMaterialsForm.h
**
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/

#pragma once

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_SCRIPTOBJECT_HPP
#include "Support/mtrl/mtrlScriptObject.hpp"
#endif
#ifndef MTRL_OPERATIONS_HPP
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifndef MA_CONSTANTS_HPP
#include "Core/ma/maConstants.hpp"
#endif

#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef MDL_MATERIALINFO_HPP
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#endif
#ifndef PRTY_FORMCONTROLBUILDER_HPP
#include "ToolUIManaged/prtym/prtyFormControlBuilder.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef GUI_MESSAGEBOX_HPP
#include "Tool/gui/guiMessageBox.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef G3D_EXCEPTIONX_HPP
#include "Graphics/g3d/g3dExceptionX.hpp"
#endif

#ifdef _MANAGED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace StudioFramework
{
	/// <summary> 
	/// Summary for mtrlMaterialsForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class mtrlMaterialsForm : public System::Windows::Forms::Form
	{
	public: 
		static mtrlMaterialsForm^ FormInstance = nullptr;

		mtrlMaterialsForm() : m_pObject(NULL), m_MaterialIndex(0)
		{
			m_bOurChange = false;
			m_bDisableNotify = true;

			InitializeComponent();

			SetupData(m_pObject);
			m_bDisableNotify = false;
		}

		// Call this to update form data
		void Update(mtrlScriptObject *i_pObject)
		{
			m_bDisableNotify = true;
			SetupData(i_pObject);
			m_pObject = i_pObject;
			m_bDisableNotify = false;

			if (i_pObject)
			{
				// Set selected material index based on what was selected last
				int sel_index = i_pObject->GetLastSelectedMaterialIndex();
				if (sel_index < i_pObject->GetNumMaterials())
				{
					this->comboBox_Materials->SelectedIndex = sel_index;
				}
			}
		}

		 void UpdateMaterialData(int i_Index)
		 {
			const mdlMaterialInfo &current_material = m_pObject->GetMaterialData(i_Index);
			m_MaterialIndex = i_Index;

			m_bDisableNotify = true;

			SetupShaderUI( m_pObject->GetShaderDataObject(i_Index), current_material );

			bool bHasFur = current_material.GetHasFur();
			checkBox_EnableFur->Checked = bHasFur;
			SetupFurUI(bHasFur ? m_pObject->GetFurDataObject(i_Index) : NULL);

			bool bHasGlow = current_material.GetHasGlow();
			checkBox_EnableGlow->Checked = bHasGlow;
			SetupGlowUI(bHasGlow ? m_pObject->GetGlowDataObject(i_Index) : NULL);

			m_bDisableNotify = false;
		 }

		void UpdateHighlightToggle()
		{
			// set highlight to current state in operations
			this->checkBox_HighlightMaterial->Checked = mtrlOperations::IsHighlightMaterial();
		}
        
	protected: 
		~mtrlMaterialsForm()
		{
			// clear instance
			if (mtrlMaterialsForm::FormInstance == this)
				mtrlMaterialsForm::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}


	private: mtrlScriptObject* m_pObject;
	private: int m_MaterialIndex;
		// Turns off notify callbacks when setting up form
	private: bool m_bDisableNotify;
		// Turns off setting of data fields when we make the change
	private: bool m_bOurChange;
	private: System::Windows::Forms::TabControl^  tabControl_materials;

	private: System::Windows::Forms::TabPage^  tabPage_materials;

	private: System::Windows::Forms::Button ^  button_CopyMaterials;
	private: System::Windows::Forms::Button ^  button_PasteMaterials;
	private: System::Windows::Forms::ComboBox ^  comboBox_Materials;
	private: System::Windows::Forms::TabControl ^  tabControl_Material;
	private: System::Windows::Forms::TabPage ^  tabPage_Surface;

	private: System::Windows::Forms::Label^  label_shader;

	private: System::Windows::Forms::Panel ^  panel_Shader;
	private: System::Windows::Forms::TabPage ^  tabPage_Glow;
	private: System::Windows::Forms::TabPage ^  tabPage_Fur;
	private: System::Windows::Forms::CheckBox ^  checkBox_EnableGlow;
	private: System::Windows::Forms::CheckBox ^  checkBox_EnableFur;
	private: System::Windows::Forms::Panel ^  panel_Glow;
	private: System::Windows::Forms::Panel ^  panel_Fur;
	private: System::Windows::Forms::Label^  label_material;


	private: System::Windows::Forms::CheckBox ^  checkBox_HighlightMaterial;
	private: System::Windows::Forms::CheckBox^  checkBox_lockmaterials;
	private: System::Windows::Forms::Button^  button_ImportFromLibrary;
	private: System::Windows::Forms::Button^  button_ExportToLibrary;
private: System::Windows::Forms::Button^  button_ImportMaterials;
private: System::Windows::Forms::Button^  button_SaveMaterials;
private: System::Windows::Forms::Button^  button_OverrideMaterials;
private: TerawattManagedControls::FileChooser^  fileChooser1;



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl_materials = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_materials = (gcnew System::Windows::Forms::TabPage());
			this->button_ImportFromLibrary = (gcnew System::Windows::Forms::Button());
			this->button_ExportToLibrary = (gcnew System::Windows::Forms::Button());
			this->checkBox_lockmaterials = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_HighlightMaterial = (gcnew System::Windows::Forms::CheckBox());
			this->label_material = (gcnew System::Windows::Forms::Label());
			this->tabControl_Material = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_Surface = (gcnew System::Windows::Forms::TabPage());
			this->panel_Shader = (gcnew System::Windows::Forms::Panel());
			this->label_shader = (gcnew System::Windows::Forms::Label());
			this->tabPage_Glow = (gcnew System::Windows::Forms::TabPage());
			this->panel_Glow = (gcnew System::Windows::Forms::Panel());
			this->checkBox_EnableGlow = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage_Fur = (gcnew System::Windows::Forms::TabPage());
			this->panel_Fur = (gcnew System::Windows::Forms::Panel());
			this->checkBox_EnableFur = (gcnew System::Windows::Forms::CheckBox());
			this->comboBox_Materials = (gcnew System::Windows::Forms::ComboBox());
			this->button_PasteMaterials = (gcnew System::Windows::Forms::Button());
			this->button_CopyMaterials = (gcnew System::Windows::Forms::Button());
			this->button_ImportMaterials = (gcnew System::Windows::Forms::Button());
			this->button_SaveMaterials = (gcnew System::Windows::Forms::Button());
			this->button_OverrideMaterials = (gcnew System::Windows::Forms::Button());
			this->fileChooser1 = (gcnew TerawattManagedControls::FileChooser());
			this->tabControl_materials->SuspendLayout();
			this->tabPage_materials->SuspendLayout();
			this->tabControl_Material->SuspendLayout();
			this->tabPage_Surface->SuspendLayout();
			this->tabPage_Glow->SuspendLayout();
			this->tabPage_Fur->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_materials
			// 
			this->tabControl_materials->Controls->Add(this->tabPage_materials);
			this->tabControl_materials->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl_materials->Location = System::Drawing::Point(0, 0);
			this->tabControl_materials->Name = L"tabControl_materials";
			this->tabControl_materials->SelectedIndex = 0;
			this->tabControl_materials->Size = System::Drawing::Size(413, 454);
			this->tabControl_materials->TabIndex = 0;
			// 
			// tabPage_materials
			// 
			this->tabPage_materials->Controls->Add(this->button_ImportFromLibrary);
			this->tabPage_materials->Controls->Add(this->button_ExportToLibrary);
			this->tabPage_materials->Controls->Add(this->checkBox_lockmaterials);
			this->tabPage_materials->Controls->Add(this->checkBox_HighlightMaterial);
			this->tabPage_materials->Controls->Add(this->label_material);
			this->tabPage_materials->Controls->Add(this->tabControl_Material);
			this->tabPage_materials->Controls->Add(this->comboBox_Materials);
			this->tabPage_materials->Controls->Add(this->button_PasteMaterials);
			this->tabPage_materials->Controls->Add(this->button_CopyMaterials);
			this->tabPage_materials->Controls->Add(this->button_ImportMaterials);
			this->tabPage_materials->Controls->Add(this->button_SaveMaterials);
			this->tabPage_materials->Controls->Add(this->button_OverrideMaterials);
			this->tabPage_materials->Location = System::Drawing::Point(4, 22);
			this->tabPage_materials->Name = L"tabPage_materials";
			this->tabPage_materials->Size = System::Drawing::Size(405, 428);
			this->tabPage_materials->TabIndex = 0;
			this->tabPage_materials->Text = L"Materials";
			// 
			// button_ImportFromLibrary
			// 
			this->button_ImportFromLibrary->Location = System::Drawing::Point(246, 71);
			this->button_ImportFromLibrary->Name = L"button_ImportFromLibrary";
			this->button_ImportFromLibrary->Size = System::Drawing::Size(120, 24);
			this->button_ImportFromLibrary->TabIndex = 13;
			this->button_ImportFromLibrary->Text = L"Import from Library";
			this->button_ImportFromLibrary->Click += gcnew System::EventHandler(this, &mtrlMaterialsForm::button_ImportFromLibrary_Click);
			// 
			// button_ExportToLibrary
			// 
			this->button_ExportToLibrary->Location = System::Drawing::Point(128, 71);
			this->button_ExportToLibrary->Name = L"button_ExportToLibrary";
			this->button_ExportToLibrary->Size = System::Drawing::Size(112, 24);
			this->button_ExportToLibrary->TabIndex = 13;
			this->button_ExportToLibrary->Text = L"Export to Library";
			this->button_ExportToLibrary->Click += gcnew System::EventHandler(this, &mtrlMaterialsForm::button_ExportToLibrary_Click);
			// 
			// checkBox_lockmaterials
			// 
			this->checkBox_lockmaterials->AutoSize = true;
			this->checkBox_lockmaterials->ForeColor = System::Drawing::SystemColors::ControlText;
			this->checkBox_lockmaterials->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->checkBox_lockmaterials->Location = System::Drawing::Point(16, 76);
			this->checkBox_lockmaterials->Name = L"checkBox_lockmaterials";
			this->checkBox_lockmaterials->Size = System::Drawing::Size(95, 17);
			this->checkBox_lockmaterials->TabIndex = 12;
			this->checkBox_lockmaterials->Text = L"Lock Materials";
			this->checkBox_lockmaterials->UseVisualStyleBackColor = true;
			this->checkBox_lockmaterials->CheckedChanged += gcnew System::EventHandler(this, &mtrlMaterialsForm::checkBox_lockmaterials_CheckedChanged);
			// 
			// checkBox_HighlightMaterial
			// 
			this->checkBox_HighlightMaterial->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox_HighlightMaterial->Location = System::Drawing::Point(248, 40);
			this->checkBox_HighlightMaterial->Name = L"checkBox_HighlightMaterial";
			this->checkBox_HighlightMaterial->Size = System::Drawing::Size(120, 24);
			this->checkBox_HighlightMaterial->TabIndex = 11;
			this->checkBox_HighlightMaterial->Text = L"Highlight Material";
			this->checkBox_HighlightMaterial->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBox_HighlightMaterial->CheckedChanged += gcnew System::EventHandler(this, &mtrlMaterialsForm::checkBox_HighlightMaterial_CheckedChanged);
			// 
			// label_material
			// 
			this->label_material->Location = System::Drawing::Point(9, 106);
			this->label_material->Name = L"label_material";
			this->label_material->Size = System::Drawing::Size(48, 17);
			this->label_material->TabIndex = 9;
			this->label_material->Text = L"Material";
			this->label_material->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tabControl_Material
			// 
			this->tabControl_Material->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl_Material->Controls->Add(this->tabPage_Surface);
			this->tabControl_Material->Controls->Add(this->tabPage_Glow);
			this->tabControl_Material->Controls->Add(this->tabPage_Fur);
			this->tabControl_Material->Location = System::Drawing::Point(8, 136);
			this->tabControl_Material->Name = L"tabControl_Material";
			this->tabControl_Material->SelectedIndex = 0;
			this->tabControl_Material->Size = System::Drawing::Size(389, 288);
			this->tabControl_Material->TabIndex = 8;
			// 
			// tabPage_Surface
			// 
			this->tabPage_Surface->Controls->Add(this->fileChooser1);
			this->tabPage_Surface->Controls->Add(this->panel_Shader);
			this->tabPage_Surface->Controls->Add(this->label_shader);
			this->tabPage_Surface->Location = System::Drawing::Point(4, 22);
			this->tabPage_Surface->Name = L"tabPage_Surface";
			this->tabPage_Surface->Size = System::Drawing::Size(381, 262);
			this->tabPage_Surface->TabIndex = 0;
			this->tabPage_Surface->Text = L"Surface";
			// 
			// panel_Shader
			// 
			this->panel_Shader->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel_Shader->AutoScroll = true;
			this->panel_Shader->Location = System::Drawing::Point(8, 32);
			this->panel_Shader->Name = L"panel_Shader";
			this->panel_Shader->Size = System::Drawing::Size(365, 227);
			this->panel_Shader->TabIndex = 2;
			// 
			// label_shader
			// 
			this->label_shader->Location = System::Drawing::Point(8, 8);
			this->label_shader->Name = L"label_shader";
			this->label_shader->Size = System::Drawing::Size(48, 16);
			this->label_shader->TabIndex = 1;
			this->label_shader->Text = L"Shader";
			this->label_shader->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tabPage_Glow
			// 
			this->tabPage_Glow->Controls->Add(this->panel_Glow);
			this->tabPage_Glow->Controls->Add(this->checkBox_EnableGlow);
			this->tabPage_Glow->Location = System::Drawing::Point(4, 22);
			this->tabPage_Glow->Name = L"tabPage_Glow";
			this->tabPage_Glow->Size = System::Drawing::Size(381, 262);
			this->tabPage_Glow->TabIndex = 1;
			this->tabPage_Glow->Text = L"Glow";
			// 
			// panel_Glow
			// 
			this->panel_Glow->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel_Glow->AutoScroll = true;
			this->panel_Glow->Location = System::Drawing::Point(8, 32);
			this->panel_Glow->Name = L"panel_Glow";
			this->panel_Glow->Size = System::Drawing::Size(365, 224);
			this->panel_Glow->TabIndex = 2;
			// 
			// checkBox_EnableGlow
			// 
			this->checkBox_EnableGlow->Location = System::Drawing::Point(8, 8);
			this->checkBox_EnableGlow->Name = L"checkBox_EnableGlow";
			this->checkBox_EnableGlow->Size = System::Drawing::Size(104, 24);
			this->checkBox_EnableGlow->TabIndex = 1;
			this->checkBox_EnableGlow->Text = L"Enable Glow";
			this->checkBox_EnableGlow->CheckedChanged += gcnew System::EventHandler(this, &mtrlMaterialsForm::checkBox_EnableGlow_CheckedChanged);
			// 
			// tabPage_Fur
			// 
			this->tabPage_Fur->Controls->Add(this->panel_Fur);
			this->tabPage_Fur->Controls->Add(this->checkBox_EnableFur);
			this->tabPage_Fur->Location = System::Drawing::Point(4, 22);
			this->tabPage_Fur->Name = L"tabPage_Fur";
			this->tabPage_Fur->Size = System::Drawing::Size(381, 262);
			this->tabPage_Fur->TabIndex = 2;
			this->tabPage_Fur->Text = L"Fur";
			// 
			// panel_Fur
			// 
			this->panel_Fur->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel_Fur->AutoScroll = true;
			this->panel_Fur->Location = System::Drawing::Point(8, 32);
			this->panel_Fur->Name = L"panel_Fur";
			this->panel_Fur->Size = System::Drawing::Size(365, 224);
			this->panel_Fur->TabIndex = 1;
			// 
			// checkBox_EnableFur
			// 
			this->checkBox_EnableFur->Location = System::Drawing::Point(8, 8);
			this->checkBox_EnableFur->Name = L"checkBox_EnableFur";
			this->checkBox_EnableFur->Size = System::Drawing::Size(104, 24);
			this->checkBox_EnableFur->TabIndex = 0;
			this->checkBox_EnableFur->Text = L"Enable Fur";
			this->checkBox_EnableFur->CheckedChanged += gcnew System::EventHandler(this, &mtrlMaterialsForm::checkBox_EnableFur_CheckedChanged);
			// 
			// comboBox_Materials
			// 
			this->comboBox_Materials->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBox_Materials->Location = System::Drawing::Point(57, 104);
			this->comboBox_Materials->Name = L"comboBox_Materials";
			this->comboBox_Materials->Size = System::Drawing::Size(333, 21);
			this->comboBox_Materials->TabIndex = 7;
			this->comboBox_Materials->SelectedIndexChanged += gcnew System::EventHandler(this, &mtrlMaterialsForm::comboBox_Materials_SelectedIndexChanged);
			// 
			// button_PasteMaterials
			// 
			this->button_PasteMaterials->Location = System::Drawing::Point(128, 40);
			this->button_PasteMaterials->Name = L"button_PasteMaterials";
			this->button_PasteMaterials->Size = System::Drawing::Size(112, 24);
			this->button_PasteMaterials->TabIndex = 6;
			this->button_PasteMaterials->Text = L"Paste Material";
			this->button_PasteMaterials->Click += gcnew System::EventHandler(this, &mtrlMaterialsForm::button_PasteMaterials_Click);
			// 
			// button_CopyMaterials
			// 
			this->button_CopyMaterials->Location = System::Drawing::Point(8, 40);
			this->button_CopyMaterials->Name = L"button_CopyMaterials";
			this->button_CopyMaterials->Size = System::Drawing::Size(112, 24);
			this->button_CopyMaterials->TabIndex = 5;
			this->button_CopyMaterials->Text = L"Copy Material";
			this->button_CopyMaterials->Click += gcnew System::EventHandler(this, &mtrlMaterialsForm::button_CopyMaterials_Click);
			// 
			// button_ImportMaterials
			// 
			this->button_ImportMaterials->Location = System::Drawing::Point(248, 10);
			this->button_ImportMaterials->Name = L"button_ImportMaterials";
			this->button_ImportMaterials->Size = System::Drawing::Size(120, 24);
			this->button_ImportMaterials->TabIndex = 4;
			this->button_ImportMaterials->Text = L"Import Materials...";
			this->button_ImportMaterials->Click += gcnew System::EventHandler(this, &mtrlMaterialsForm::button_ImportMaterials_Click);
			// 
			// button_SaveMaterials
			// 
			this->button_SaveMaterials->Location = System::Drawing::Point(128, 10);
			this->button_SaveMaterials->Name = L"button_SaveMaterials";
			this->button_SaveMaterials->Size = System::Drawing::Size(112, 24);
			this->button_SaveMaterials->TabIndex = 3;
			this->button_SaveMaterials->Text = L"Save Materials";
			this->button_SaveMaterials->Click += gcnew System::EventHandler(this, &mtrlMaterialsForm::button_SaveMaterials_Click);
			// 
			// button_OverrideMaterials
			// 
			this->button_OverrideMaterials->Location = System::Drawing::Point(8, 10);
			this->button_OverrideMaterials->Name = L"button_OverrideMaterials";
			this->button_OverrideMaterials->Size = System::Drawing::Size(112, 24);
			this->button_OverrideMaterials->TabIndex = 1;
			this->button_OverrideMaterials->Text = L"Override Materials";
			this->button_OverrideMaterials->Click += gcnew System::EventHandler(this, &mtrlMaterialsForm::button_OverrideMaterials_Click);
			// 
			// fileChooser1
			// 
			this->fileChooser1->Location = System::Drawing::Point(62, 3);
			this->fileChooser1->Name = L"fileChooser1";
			this->fileChooser1->Size = System::Drawing::Size(264, 24);
			this->fileChooser1->TabIndex = 3;
			// 
			// mtrlMaterialsForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(413, 454);
			this->Controls->Add(this->tabControl_materials);
			this->Name = L"mtrlMaterialsForm";
			this->Text = L"mtrlMaterialsForm";
			this->tabControl_materials->ResumeLayout(false);
			this->tabPage_materials->ResumeLayout(false);
			this->tabPage_materials->PerformLayout();
			this->tabControl_Material->ResumeLayout(false);
			this->tabPage_Surface->ResumeLayout(false);
			this->tabPage_Glow->ResumeLayout(false);
			this->tabPage_Fur->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

		void SetupData(mtrlScriptObject *i_pObject)
		{
			UpdateHighlightToggle();

			if (m_bOurChange) return;

			ClearCurrentMaterialData();
			fill_list(i_pObject);

			if (i_pObject != nullptr)
			{
				if (m_pObject != i_pObject)
					m_bOurChange = true;
				m_pObject = i_pObject;
				this->checkBox_lockmaterials->Checked = i_pObject->IsLockMaterials();
				m_bOurChange = false;
			}
		}

		void ClearCurrentMaterialData()
		{
			prtyFormControlBuilder::InitForm( panel_Shader );
			prtyFormControlBuilder::InitForm( panel_Fur );
			prtyFormControlBuilder::InitForm( panel_Glow );
		}

		void SetupShaderUI(prtyObject *i_pShaderObject, 
							const mdlMaterialInfo &i_Info)
		{
			// clear form UI 
			prtyFormControlBuilder::InitForm( panel_Shader );

			fileChooser1->Text = tmaManagedStringUtils::ItStringToManagedString(i_Info.GetShaderParams()->GetShaderName());
			prtyFormControlBuilder::BuildForm( panel_Shader, (i_pShaderObject->GetList()), true );
		}

		void SetupFurUI(prtyObject *i_pFurObject)
		{
			// clear form UI 
			prtyFormControlBuilder::InitForm( panel_Fur );
			if (i_pFurObject != NULL)
			{
				prtyFormControlBuilder::BuildForm( panel_Fur, (i_pFurObject->GetList()), true );
			}
			panel_Fur->Enabled = (i_pFurObject != NULL);
		}
		void SetupGlowUI(prtyObject *i_pGlowObject)
		{
			// clear form UI 
			prtyFormControlBuilder::InitForm( panel_Glow );
			if (i_pGlowObject != NULL)
			{
				prtyFormControlBuilder::BuildForm( panel_Glow, (i_pGlowObject->GetList()), true );
			}
			panel_Glow->Enabled = (i_pGlowObject != NULL);
		}


	public:
    	//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_materials;
		}

	private:
		 int get_selected_index()
		 {
			return this->comboBox_Materials->SelectedIndex;;
		 }

		 void update_controls_for_lock_materials( bool i_bLocked, bool i_bNotifyUser )
		 {
			 if (i_bLocked)
			 {
				//this->comboBox_Materials->Enabled = false;
				this->button_OverrideMaterials->Enabled = false;
				this->button_SaveMaterials->Enabled = false;
				this->button_ImportMaterials->Enabled = false;
				this->button_CopyMaterials->Enabled = false;
				this->button_PasteMaterials->Enabled = false;
				this->button_ExportToLibrary->Enabled = false;
				this->button_ImportFromLibrary->Enabled = false;
				//this->checkBox_HighlightMaterial->Checked = false;
				//this->checkBox_HighlightMaterial->Enabled = false;

				//	disable all the tabs, but still make them viewable
				System::Collections::IEnumerator^ myEnumerator = this->tabControl_Material->Controls->GetEnumerator();
				while ( myEnumerator->MoveNext() )
				{
					System::Windows::Forms::Control^ pControl = dynamic_cast<System::Windows::Forms::Control^>(myEnumerator->Current);
					if (pControl != nullptr)
					{
						pControl->Enabled = false;
					}
				}
			 }
			 else
			 {
				 if (i_bNotifyUser)
				 {
					// Initializes the variables to pass to the MessageBox.Show method.
					System::String^ message = "The materials are locked and should not be changed.  Are you sure you want to do this?";
					System::String^ caption = "Attemping to unlock materials";
					System::Windows::Forms::DialogResult result;
					result = MessageBox::Show(message, caption, MessageBoxButtons::YesNo);
					if (result == System::Windows::Forms::DialogResult::No)
					{
						return;
					}
				 }

				//	enable the controls
				//this->comboBox_Materials->Enabled = true;
				if (this->comboBox_Materials->Items->Count > 0)
				{
					this->button_SaveMaterials->Enabled = true;
					this->button_ImportMaterials->Enabled = true;
					this->button_CopyMaterials->Enabled = true;
					this->button_PasteMaterials->Enabled = mtrlOperations::HaveClipboardData();
					this->button_ExportToLibrary->Enabled = true;
					this->button_ImportFromLibrary->Enabled = true;
					//this->checkBox_HighlightMaterial->Checked = true;
					//this->checkBox_HighlightMaterial->Enabled = true;

					//	enable all the tabs, but still make them viewable
					System::Collections::IEnumerator^ myEnumerator = this->tabControl_Material->Controls->GetEnumerator();
					while ( myEnumerator->MoveNext() )
					{
						System::Windows::Forms::Control^ pControl = dynamic_cast<System::Windows::Forms::Control^>(myEnumerator->Current);
						if (pControl != nullptr)
						{
							pControl->Enabled = true;
						}
					}
				}
				else
				{
					this->button_OverrideMaterials->Enabled = true;
				}
			 }
		}				

		 // Get string to display in list for material's name
		 std::string construct_display_name(mtrlScriptObject* i_pObject, int i_MatIndex)
		 {
			// Use prefix to mark which materials are using the material library
			//const std::string c_LibrarySignifier("* ");
					
			std::string material_name = i_pObject->GetMaterialName(i_MatIndex);
			if (i_pObject->IsLibraryMaterial(i_MatIndex))
			{
				//material_name = c_LibrarySignifier + material_name;

				// Add relative path to library file to end of material name
				material_name += " : ";
				std::string rel_path;
				fsFileUtil::LocatorToANSIFilename(i_pObject->GetMaterialData(i_MatIndex).GetLibraryFilename(), rel_path);
				material_name += rel_path;
			}
			return material_name;
		 }

		// Fill objectList
		void fill_list(mtrlScriptObject *i_pObject)
		{
			this->comboBox_Materials->Items->Clear();
			this->comboBox_Materials->Text = "";

			// Disable until a material is selected
			this->button_OverrideMaterials->Enabled = false;
			this->button_SaveMaterials->Enabled = false;
			this->button_ImportMaterials->Enabled = false;
			this->button_CopyMaterials->Enabled = false;
			this->button_PasteMaterials->Enabled = false;
			this->button_ExportToLibrary->Enabled = false;
			this->button_ImportFromLibrary->Enabled = false;
			//this->checkBox_HighlightMaterial->Checked = false;
			//this->checkBox_HighlightMaterial->Enabled = false;

			if (i_pObject)
			{
				const int num_items = i_pObject->GetNumMaterials();
				for (int i=0; i<num_items; i++)
				{	
					std::string material_name = construct_display_name(i_pObject, i);
					this->comboBox_Materials->Items->Add( gcnew String(material_name.c_str()) );
				}	

				// Only allow override if we have't done it already
				if (num_items == 0)
				{
					this->button_OverrideMaterials->Enabled = true;
				}
				else
				{
					// Only enable save and other functions if there are overriden materials
					this->button_SaveMaterials->Enabled = true;
					this->button_ImportMaterials->Enabled = true;
					this->button_CopyMaterials->Enabled = true;
					//this->checkBox_HighlightMaterial->Enabled = true;
					this->button_ExportToLibrary->Enabled = true;
					this->button_ImportFromLibrary->Enabled = true;
					this->button_PasteMaterials->Enabled = mtrlOperations::HaveClipboardData();

					//	enable all the tabs, but still make them viewable
					System::Collections::IEnumerator^ myEnumerator = this->tabControl_Material->Controls->GetEnumerator();
					while ( myEnumerator->MoveNext() )
					{
						System::Windows::Forms::Control^ pControl = dynamic_cast<System::Windows::Forms::Control^>(myEnumerator->Current);
						if (pControl != nullptr)
						{
							pControl->Enabled = true;
						}
					}
				}

				update_controls_for_lock_materials( i_pObject->IsLockMaterials(), false );
			}
			
		}

		 // variation of fill_list that just changes the names of the existing materials
		 // in the combo box for the material list
		void update_list(mtrlScriptObject *i_pObject)
		{
			m_bDisableNotify = true;
			if (i_pObject)
			{
				int sel_index = this->comboBox_Materials->SelectedIndex;
				this->comboBox_Materials->BeginUpdate();
				this->comboBox_Materials->Items->Clear();
				const int num_items = i_pObject->GetNumMaterials();
				for (int i=0; i<num_items; i++)
				{	
					std::string material_name = construct_display_name(i_pObject, i);
					this->comboBox_Materials->Items->Add( gcnew String(material_name.c_str()) );
				}	
				this->comboBox_Materials->SelectedIndex = sel_index;
				this->comboBox_Materials->EndUpdate();
			}
			m_bDisableNotify = false;
		}

private: System::Void button_OverrideMaterials_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (m_pObject)
			 {
				 mtrlOperations::OverrideMaterials(m_pObject);
				 fill_list(m_pObject);
			  }
		 }

private: System::Void button_SaveMaterials_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if ( m_pObject )
			 {
				 // this line saves to the current filename:
				 //mtrlOperations::SaveMaterials(m_pObject);
				 // this line prompts for a save to a new file
				 mtrlOperations::PromptAndSaveMaterials(m_pObject);
				 this->ClearCurrentMaterialData();
				 fill_list(m_pObject);
			 }
		 }

private: System::Void button_ImportMaterials_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if ( m_pObject )
			 {
				if (m_pObject->HasMaterialAnimation())
				{
					guiMessageBox::Show( "This material is animated and cannot be changed in this way.", "Material AnimationExists", guiMessageBox::e_OK );
				}
				else
				{
					int sel_index = this->comboBox_Materials->SelectedIndex;

					if ( sel_index >= 0 )
						ClearCurrentMaterialData();

					mtrlOperations::ImportMaterials(m_pObject);

					if ( sel_index >= 0 )
						UpdateMaterialData(sel_index);
				}
			 }
		 }

private: System::Void button_CopyMaterials_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			int sel_index = this->comboBox_Materials->SelectedIndex;
			if ( m_pObject && (sel_index >= 0) )
			{
				mtrlOperations::CopyMaterial();
				fill_list(m_pObject);	// just to set enabled state of "Paste Materials" button
			}
		 }

private: System::Void button_PasteMaterials_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			int sel_index = this->comboBox_Materials->SelectedIndex;
			if ( mtrlOperations::HaveClipboardData() )
			{
				if ( m_pObject && (sel_index >= 0) )
				{
					if (m_pObject->HasMaterialAnimation(sel_index))
					{
						guiMessageBox::Show( "This material is animated and cannot be changed in this way.", "Material AnimationExists", guiMessageBox::e_OK );
					}
					else
					{
						ClearCurrentMaterialData();
						mtrlOperations::PasteMaterial();
						UpdateMaterialData(sel_index);
					}
				}
			}
		 }
private: System::Void button_ExportToLibrary_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			int sel_index = this->comboBox_Materials->SelectedIndex;
			if ( m_pObject && (sel_index >= 0) )
			{
				mtrlOperations::ExportToLibrary();
				update_list(m_pObject);
			}
		 }
private: System::Void button_ImportFromLibrary_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			int sel_index = this->comboBox_Materials->SelectedIndex;
			if ( m_pObject && (sel_index >= 0) )
			{
				if (m_pObject->HasMaterialAnimation(sel_index))
				{
					guiMessageBox::Show( "This material is animated and cannot be changed in this way.", "Material AnimationExists", guiMessageBox::e_OK );
				}
				else
				{
					this->ClearCurrentMaterialData();
					mtrlOperations::ImportFromLibrary();
					UpdateMaterialData(sel_index);
					update_list(m_pObject);
				}
			}
		 }

private: System::Void comboBox_Materials_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (m_bDisableNotify) 
				 return;

			 int sel_index = this->comboBox_Materials->SelectedIndex;
			 mtrlOperations::SetSelectedMaterialIndex(m_pObject, sel_index);
			 //this->checkBox_HighlightMaterial->Checked = false;
			 if ( m_pObject && (sel_index >= 0) )
			 {
				 UpdateMaterialData(sel_index);
			 }	
		 }

private: System::Void comboBox_Shader_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 int sel_material = get_selected_index(); // material, not shader index
			 if (!m_bDisableNotify && sel_material >= 0)
			 {

			 itString old_shader = m_pObject->GetMaterialData(sel_material).GetShaderParams()->GetShaderName();
			// compare with last name of the file path.
			// that means, for now, only search the main shaders directory!!
			 itString new_shader;
			 tmaManagedStringUtils::ManagedStringToItString(fileChooser1->Filename, new_shader);

			if (new_shader != old_shader)
			{
				bool bAbortChange = true;
				if (m_pObject->HasMaterialAnimation(sel_material))
				{
					guiMessageBox::Show( "This material is animated and cannot be changed in this way.", "Material AnimationExists", guiMessageBox::e_OK );
				}
				else
				{
					int retval = guiMessageBox::Show( "Changing shader will lose all old shader parameters. Continue?", "Shader Change", guiMessageBox::e_YesNo );
					if ( retval == guiMessageBox::e_Yes )
					{
						matShaderEffect* eff = NULL;
						try
						{
							eff = matShaderMgr::GetEffect(new_shader);
						}
						catch(const g3dShaderLoadX& /*ex*/)
						{
							guiMessageBox::Show( "Shader could not be loaded. See debug.log for compilation error.", "Shader Change", guiMessageBox::e_OK );
							eff = NULL;
						}
						if (eff)
						{
							shared_ptr<effShaderParams> params(new effShaderParams());
							params->SetShaderName(new_shader, eff);
							eff->BuildPrtyObject(params.get());

							prtyFormControlBuilder::InitForm( panel_Shader );

							mdlMaterialInfo new_material(m_pObject->GetMaterialData(sel_material));
							new_material.SetShaderParams(params);
							//new_material.SetShader(shaderTable[new_shader_index].m_Name, shaderTable[new_shader_index].m_DataTemplate);

							const bool bUpdateProperties = true; 
							mtrlOperations::ChangeMaterialData(new_material, bUpdateProperties);


						prtyObject *pShaderData = m_pObject->GetShaderDataObject(sel_material);
						prtyFormControlBuilder::BuildForm( panel_Shader, (pShaderData->GetList()), true );

							bAbortChange = false;
						}
						else
						{
							// any extra handling of bad shader load here.
						}
					}

					 }
				 }
			 }


		 }

private: System::Void checkBox_EnableFur_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
				 mdlMaterialInfo new_material(m_pObject->GetMaterialData(m_MaterialIndex));
				 new_material.SetHasFur(checkBox_EnableFur->Checked);
				 const bool bUpdateProperties = false; 
				 mtrlOperations::ChangeMaterialData(new_material, bUpdateProperties);
				 SetupFurUI(checkBox_EnableFur->Checked ? m_pObject->GetFurDataObject(get_selected_index()) : NULL );
			 }
		 }

private: System::Void checkBox_EnableGlow_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
				 mdlMaterialInfo new_material(m_pObject->GetMaterialData(m_MaterialIndex));
				 new_material.SetHasGlow(checkBox_EnableGlow->Checked);
				 const bool bUpdateProperties = false; 
				 mtrlOperations::ChangeMaterialData(new_material, bUpdateProperties);
				 SetupGlowUI(checkBox_EnableGlow->Checked ? m_pObject->GetGlowDataObject(get_selected_index()) : NULL );
			 }
		 }

private: System::Void checkBox_HighlightMaterial_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			mtrlOperations::HighlightMaterial(checkBox_HighlightMaterial->Checked);
		 }

private: System::Void checkBox_lockmaterials_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			if (m_bOurChange) return;

			System::Windows::Forms::CheckBox^ pCB = dynamic_cast<System::Windows::Forms::CheckBox^>(sender);

			 if (pCB != nullptr)
			 {
				 if (m_pObject != nullptr)
					m_pObject->LockMaterials(pCB->Checked);
				 //mtrlOperations::LockMaterials(pCB->Checked);

				 update_controls_for_lock_materials( pCB->Checked, true );
			 }
		 }
};
}
#endif // _MANAGED
