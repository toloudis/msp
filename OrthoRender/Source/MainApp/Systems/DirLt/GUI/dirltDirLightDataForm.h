/********************************************************************************************\
**  dirltDirLightDataForm.h
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifndef DIRLT_SCRIPTDATA_HPP
#include "dirltScriptData.hpp"
#endif
#ifndef DIRLT_OPERATIONS_HPP
#include "dirltOperations.hpp"
#endif

#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "tmaManagedConversionUtil.hpp"
#endif


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace GeneratedForms
{
	/// <summary>
	/// Summary for dirltDirLightDataForm
	///
	/// WARNING: If you change the name of this class, yada yada...
	/// </summary>
	public __gc class dirltDirLightDataForm : public System::Windows::Forms::Form
	{
	public:
		static dirltDirLightDataForm* FormInstance = 0;

		dirltDirLightDataForm(dirltScriptData &i_Data) : m_Data(i_Data)
		{
			m_bOurChange = false;

			m_bDisableNotify = true;
			InitializeComponent();
			SetupData(m_Data);

			m_bDisableNotify = false;
		}

		// Call this to update form data
		void Update()
		{
			SetupData(m_Data);
		}

		void Enable(bool i_Enabled)
		{
			colorRGBEdit_lightcolor->Enabled	= i_Enabled;
			checkBox_enabled->Enabled			= i_Enabled;
			checkBox_shadow->Enabled			= i_Enabled;
			vector3Edit_dir->Enabled			= i_Enabled;
		}

	protected:
		void Dispose(Boolean disposing)
		{
			// clear instance
			if (disposing && dirltDirLightDataForm::FormInstance == this)
				dirltDirLightDataForm::FormInstance = 0;

			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container* components;

		// Turns off notify callbacks when setting up form
	private: bool m_bDisableNotify;
		// Turns off setting of data fields when we make the change
	private: bool m_bOurChange;

	private: System::Windows::Forms::TabControl *  tabControl_dirlt;
	private: System::Windows::Forms::TabPage *  tabPage_dirlt;
	private: TerawattManagedControls::ColorRGBEdit *  colorRGBEdit_lightcolor;
	private: System::Windows::Forms::CheckBox *  checkBox_enabled;
	private: System::Windows::Forms::CheckBox *  checkBox_shadow;
	private: System::Windows::Forms::Label *  label_color;
	private: System::Windows::Forms::Label *  label_direction;
	private: TerawattManagedControls::Vector3Edit *  vector3Edit_dir;
	private: System::Windows::Forms::CheckBox *  checkBox_diffuse;
	private: System::Windows::Forms::CheckBox *  checkBox_specular;

	private: dirltScriptData& m_Data;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl_dirlt = new System::Windows::Forms::TabControl();
			this->tabPage_dirlt = new System::Windows::Forms::TabPage();
			this->label_color = new System::Windows::Forms::Label();
			this->colorRGBEdit_lightcolor = new TerawattManagedControls::ColorRGBEdit();
			this->checkBox_enabled = new System::Windows::Forms::CheckBox();
			this->checkBox_shadow = new System::Windows::Forms::CheckBox();
			this->label_direction = new System::Windows::Forms::Label();
			this->vector3Edit_dir = new TerawattManagedControls::Vector3Edit();
			this->checkBox_diffuse = new System::Windows::Forms::CheckBox();
			this->checkBox_specular = new System::Windows::Forms::CheckBox();
			this->tabControl_dirlt->SuspendLayout();
			this->tabPage_dirlt->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_dirlt
			// 
			this->tabControl_dirlt->Controls->Add(this->tabPage_dirlt);
			this->tabControl_dirlt->Location = System::Drawing::Point(8, 8);
			this->tabControl_dirlt->Name = S"tabControl_dirlt";
			this->tabControl_dirlt->SelectedIndex = 0;
			this->tabControl_dirlt->Size = System::Drawing::Size(365, 312);
			this->tabControl_dirlt->TabIndex = 1;
			// 
			// tabPage_dirlt
			// 
			this->tabPage_dirlt->Controls->Add(this->checkBox_diffuse);
			this->tabPage_dirlt->Controls->Add(this->checkBox_specular);
			this->tabPage_dirlt->Controls->Add(this->label_color);
			this->tabPage_dirlt->Controls->Add(this->colorRGBEdit_lightcolor);
			this->tabPage_dirlt->Controls->Add(this->checkBox_enabled);
			this->tabPage_dirlt->Controls->Add(this->checkBox_shadow);
			this->tabPage_dirlt->Controls->Add(this->label_direction);
			this->tabPage_dirlt->Controls->Add(this->vector3Edit_dir);
			this->tabPage_dirlt->Location = System::Drawing::Point(4, 22);
			this->tabPage_dirlt->Name = S"tabPage_dirlt";
			this->tabPage_dirlt->Size = System::Drawing::Size(357, 286);
			this->tabPage_dirlt->TabIndex = 0;
			this->tabPage_dirlt->Text = S"Dir Light";
			// 
			// label_color
			// 
			this->label_color->Location = System::Drawing::Point(32, 24);
			this->label_color->Name = S"label_color";
			this->label_color->Size = System::Drawing::Size(40, 20);
			this->label_color->TabIndex = 12;
			this->label_color->Text = S"Color:";
			// 
			// colorRGBEdit_lightcolor
			// 
			this->colorRGBEdit_lightcolor->Color = System::Drawing::Color::FromArgb((System::Byte)224, (System::Byte)224, (System::Byte)224);
			this->colorRGBEdit_lightcolor->Location = System::Drawing::Point(80, 24);
			this->colorRGBEdit_lightcolor->Name = S"colorRGBEdit_lightcolor";
			this->colorRGBEdit_lightcolor->Size = System::Drawing::Size(237, 24);
			this->colorRGBEdit_lightcolor->TabIndex = 13;
			this->colorRGBEdit_lightcolor->ValueChanged += new System::EventHandler(this, colorRGBEdit_lightcolor_ValueChanged);
			// 
			// checkBox_enabled
			// 
			this->checkBox_enabled->Location = System::Drawing::Point(72, 56);
			this->checkBox_enabled->Name = S"checkBox_enabled";
			this->checkBox_enabled->Size = System::Drawing::Size(104, 21);
			this->checkBox_enabled->TabIndex = 14;
			this->checkBox_enabled->Text = S"Enabled";
			this->checkBox_enabled->CheckedChanged += new System::EventHandler(this, checkBox_enabled_CheckedChanged);
			// 
			// checkBox_shadow
			// 
			this->checkBox_shadow->Location = System::Drawing::Point(192, 56);
			this->checkBox_shadow->Name = S"checkBox_shadow";
			this->checkBox_shadow->Size = System::Drawing::Size(127, 21);
			this->checkBox_shadow->TabIndex = 15;
			this->checkBox_shadow->Text = S"Shadow Source";
			this->checkBox_shadow->CheckedChanged += new System::EventHandler(this, checkBox_shadow_CheckedChanged);
			// 
			// label_direction
			// 
			this->label_direction->Location = System::Drawing::Point(32, 136);
			this->label_direction->Name = S"label_direction";
			this->label_direction->Size = System::Drawing::Size(56, 20);
			this->label_direction->TabIndex = 16;
			this->label_direction->Text = S"Direction:";
			// 
			// vector3Edit_dir
			// 
			this->vector3Edit_dir->Location = System::Drawing::Point(96, 136);
			this->vector3Edit_dir->Name = S"vector3Edit_dir";
			this->vector3Edit_dir->Precision = (System::Int16)2;
			this->vector3Edit_dir->Size = System::Drawing::Size(231, 20);
			this->vector3Edit_dir->TabIndex = 17;
			this->vector3Edit_dir->ValueChanged += new System::EventHandler(this, vector3Edit_dir_ValueChanged);
			// 
			// checkBox_diffuse
			// 
			this->checkBox_diffuse->Location = System::Drawing::Point(72, 88);
			this->checkBox_diffuse->Name = S"checkBox_diffuse";
			this->checkBox_diffuse->Size = System::Drawing::Size(96, 21);
			this->checkBox_diffuse->TabIndex = 25;
			this->checkBox_diffuse->Text = S"Diffuse Light";
			this->checkBox_diffuse->CheckedChanged += new System::EventHandler(this, checkBox_diffuse_CheckedChanged);
			// 
			// checkBox_specular
			// 
			this->checkBox_specular->Location = System::Drawing::Point(192, 88);
			this->checkBox_specular->Name = S"checkBox_specular";
			this->checkBox_specular->Size = System::Drawing::Size(127, 21);
			this->checkBox_specular->TabIndex = 26;
			this->checkBox_specular->Text = S"Specular Light";
			this->checkBox_specular->CheckedChanged += new System::EventHandler(this, checkBox_specular_CheckedChanged);
			// 
			// dirltDirLightDataForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(393, 324);
			this->Controls->Add(this->tabControl_dirlt);
			this->Name = S"dirltDirLightDataForm";
			this->Text = S"Directional Light Dialog";
			this->tabControl_dirlt->ResumeLayout(false);
			this->tabPage_dirlt->ResumeLayout(false);
			this->ResumeLayout(false);

		}

		//

		void SetupData(const dirltScriptData &i_Data)
		{
			if (m_bOurChange) return;

			m_bDisableNotify = true;
			colorRGBEdit_lightcolor->Color = tmaManagedConversionUtil::SetColorRGB(i_Data.m_BaseData.m_Color.GetValue());
			checkBox_enabled->Checked = i_Data.m_BaseData.m_Enabled.GetValue();
			checkBox_shadow->Checked = i_Data.m_BaseData.m_ShadowSource.GetValue();
			checkBox_diffuse->Checked = i_Data.m_BaseData.m_bDiffuseEnabled.GetValue();
			checkBox_specular->Checked = i_Data.m_BaseData.m_bSpecularEnabled.GetValue();
			tmaManagedConversionUtil::SetPoint3(i_Data.m_BaseData.m_Direction.GetValue(), vector3Edit_dir);
			m_bDisableNotify = false;
		}

		System::Void colorRGBEdit_lightcolor_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		{
			if (!m_bDisableNotify) {
				m_Data.m_BaseData.m_Color = tmaManagedConversionUtil::ConvertColorRGB(colorRGBEdit_lightcolor->Color);
				dirltOperations::ChangeLightData(m_Data);
			}
		}

		System::Void checkBox_enabled_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
		{
			if (!m_bDisableNotify) {
				m_Data.m_BaseData.m_Enabled.SetValue(checkBox_enabled->Checked);
				dirltOperations::ChangeLightData(m_Data);
			}
		}

		System::Void checkBox_shadow_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
		{
			if (!m_bDisableNotify) {
				m_Data.m_BaseData.m_ShadowSource.SetValue(checkBox_shadow->Checked);
				dirltOperations::ChangeLightData(m_Data);
			}
		}

		System::Void checkBox_diffuse_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
		{
			if (!m_bDisableNotify) {
				m_Data.m_BaseData.m_bDiffuseEnabled.SetValue(checkBox_diffuse->Checked);
				dirltOperations::ChangeLightData(m_Data);
			}
		}

		System::Void checkBox_specular_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
		{
			if (!m_bDisableNotify) {
				m_Data.m_BaseData.m_bSpecularEnabled.SetValue(checkBox_specular->Checked);
				dirltOperations::ChangeLightData(m_Data);
			}
		}

		System::Void vector3Edit_dir_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		{
			if (!m_bDisableNotify) {
				m_bOurChange = true;
				m_Data.m_BaseData.m_Direction.SetValue( maVector3d((float)vector3Edit_dir->ValueX, (float)vector3Edit_dir->ValueY, (float)vector3Edit_dir->ValueZ));
				dirltOperations::ChangeLightData(m_Data);
				m_bOurChange = false;
			}
		}

	public:
    	//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage * GetTabPage( int i_Index )
		{
			return tabPage_dirlt;
		}
};
}
