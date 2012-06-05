#pragma once


#ifndef EFF_SHADERARRAY_HPP
#include "effShaderArray.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "matShaderEffect.hpp"
#endif
#ifndef MTR_LEVEL_HPP
#include "mtrLevel.hpp"
#endif
#ifndef MTR_MATERIALTEMPLATE_HPP
#include "mtrMaterialTemplate.hpp"
#endif
#ifndef MTR_OPERATIONS_HPP
#include "mtrOperations.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "tmaManagedConversionUtil.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "fsLocator.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "fsFileUtil.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "itStringUtil.hpp"
#endif
#ifndef	TMA_MANAGEDSTRINGUTILS_HPP
#include "tmaManagedStringUtils.hpp"
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
	/// Summary for MaterialDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class MaterialDialog : public System::Windows::Forms::Form
	{
	public: 
		static MaterialDialog *FormInstance = 0;

		MaterialDialog(mtrMaterialTemplate &i_Data)
			: m_Data(i_Data)
		{
			m_bOurChange = false;
			m_bDisableNotify = true;

			InitializeComponent();

			SetupData(m_Data);
			m_bDisableNotify = false;
		}

		void UpdateData()
		{
			SetupData(m_Data);
		}
        
	protected: 
		void Dispose(Boolean disposing)
		{
			// clear instance
			if (disposing && MaterialDialog::FormInstance == this)
				MaterialDialog::FormInstance = 0;

			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}

	private: mtrMaterialTemplate& m_Data;
		// Turns off notify callbacks when setting up form
	private: bool m_bDisableNotify;
		// Turns off setting of data fields when we make the change
	private: bool m_bOurChange;

	private: System::Windows::Forms::TabControl *  tabControl1;
	private: System::Windows::Forms::TabPage *  tabPage_Base;
	private: System::Windows::Forms::TabPage *  tabPage_Shader;
	private: System::Windows::Forms::Label *  label7;
	private: TerawattManagedControls::RangedFloat *  rangedFloatReflectivity;
	private: System::Windows::Forms::Label *  label6;
	private: TerawattManagedControls::RangedFloat *  rangedFloatBumpScale;
	private: System::Windows::Forms::GroupBox *  groupBox2;
	private: System::Windows::Forms::Button *  buttonMoveDown;
	private: System::Windows::Forms::Button *  buttonMoveUp;
	private: System::Windows::Forms::Button *  buttonRemove;
	private: System::Windows::Forms::Button *  buttonEdit;
	private: System::Windows::Forms::Button *  buttonNew;
	private: System::Windows::Forms::ListBox *  listTextures;
	private: System::Windows::Forms::GroupBox *  groupBox1;
	private: TerawattManagedControls::FloatEdit *  floatPower;
	private: System::Windows::Forms::Label *  label5;
	private: System::Windows::Forms::CheckBox *  checkSpecular;
	private: TerawattManagedControls::ColorRGBAEdit *  colorSpecular;
	private: System::Windows::Forms::Label *  label3;
	private: TerawattManagedControls::ColorRGBAEdit *  colorEmissive;
	private: System::Windows::Forms::Label *  label4;
	private: TerawattManagedControls::ColorRGBAEdit *  colorDiffuse;
	private: System::Windows::Forms::Label *  label2;
	private: TerawattManagedControls::ColorRGBAEdit *  colorAmbient;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::ComboBox *  comboBox_shaderName;
	private: System::Windows::Forms::Label *  label17;
	private: System::Windows::Forms::Label *  label_separator;
	private: System::Windows::Forms::Panel *  panel_shaderControls;
	private: System::Windows::Forms::TabPage *  tabPage_Fur;
	private: System::Windows::Forms::CheckBox *  checkBox_FurEnable;
	private: System::Windows::Forms::Label *  label_separatorFur;
	private: System::Windows::Forms::Panel *  panel_furControls;
	private: System::Windows::Forms::TabPage *  tabPage_Glow;
	private: System::Windows::Forms::Panel *  panel_glowControls;
	private: System::Windows::Forms::CheckBox *  checkBox_enableGlow;
	private: System::Windows::Forms::Label *  label_separatorGlow;
	private: TerawattManagedControls::FileChooser *  fileChooser_glowMask;
	private: System::Windows::Forms::Label *  label_glowMask;
	private: System::Windows::Forms::Label *  label8;
	private: System::Windows::Forms::Label *  label9;
	private: TerawattManagedControls::FloatEdit *  floatEdit_UScale;
	private: TerawattManagedControls::FloatEdit *  floatEdit_VScale;
































	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container* components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl1 = new System::Windows::Forms::TabControl();
			this->tabPage_Base = new System::Windows::Forms::TabPage();
			this->label7 = new System::Windows::Forms::Label();
			this->rangedFloatReflectivity = new TerawattManagedControls::RangedFloat();
			this->label6 = new System::Windows::Forms::Label();
			this->rangedFloatBumpScale = new TerawattManagedControls::RangedFloat();
			this->groupBox2 = new System::Windows::Forms::GroupBox();
			this->buttonMoveDown = new System::Windows::Forms::Button();
			this->buttonMoveUp = new System::Windows::Forms::Button();
			this->buttonRemove = new System::Windows::Forms::Button();
			this->buttonEdit = new System::Windows::Forms::Button();
			this->buttonNew = new System::Windows::Forms::Button();
			this->listTextures = new System::Windows::Forms::ListBox();
			this->groupBox1 = new System::Windows::Forms::GroupBox();
			this->floatPower = new TerawattManagedControls::FloatEdit();
			this->label5 = new System::Windows::Forms::Label();
			this->checkSpecular = new System::Windows::Forms::CheckBox();
			this->colorSpecular = new TerawattManagedControls::ColorRGBAEdit();
			this->label3 = new System::Windows::Forms::Label();
			this->colorEmissive = new TerawattManagedControls::ColorRGBAEdit();
			this->label4 = new System::Windows::Forms::Label();
			this->colorDiffuse = new TerawattManagedControls::ColorRGBAEdit();
			this->label2 = new System::Windows::Forms::Label();
			this->colorAmbient = new TerawattManagedControls::ColorRGBAEdit();
			this->label1 = new System::Windows::Forms::Label();
			this->tabPage_Shader = new System::Windows::Forms::TabPage();
			this->panel_shaderControls = new System::Windows::Forms::Panel();
			this->label_separator = new System::Windows::Forms::Label();
			this->label17 = new System::Windows::Forms::Label();
			this->comboBox_shaderName = new System::Windows::Forms::ComboBox();
			this->tabPage_Fur = new System::Windows::Forms::TabPage();
			this->panel_furControls = new System::Windows::Forms::Panel();
			this->label_separatorFur = new System::Windows::Forms::Label();
			this->checkBox_FurEnable = new System::Windows::Forms::CheckBox();
			this->tabPage_Glow = new System::Windows::Forms::TabPage();
			this->label_glowMask = new System::Windows::Forms::Label();
			this->fileChooser_glowMask = new TerawattManagedControls::FileChooser();
			this->label_separatorGlow = new System::Windows::Forms::Label();
			this->checkBox_enableGlow = new System::Windows::Forms::CheckBox();
			this->panel_glowControls = new System::Windows::Forms::Panel();
			this->label8 = new System::Windows::Forms::Label();
			this->label9 = new System::Windows::Forms::Label();
			this->floatEdit_UScale = new TerawattManagedControls::FloatEdit();
			this->floatEdit_VScale = new TerawattManagedControls::FloatEdit();
			this->tabControl1->SuspendLayout();
			this->tabPage_Base->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->tabPage_Shader->SuspendLayout();
			this->tabPage_Fur->SuspendLayout();
			this->tabPage_Glow->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl1
			// 
			this->tabControl1->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl1->Controls->Add(this->tabPage_Base);
			this->tabControl1->Controls->Add(this->tabPage_Shader);
			this->tabControl1->Controls->Add(this->tabPage_Fur);
			this->tabControl1->Controls->Add(this->tabPage_Glow);
			this->tabControl1->Location = System::Drawing::Point(8, 8);
			this->tabControl1->Name = S"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(368, 491);
			this->tabControl1->TabIndex = 0;
			// 
			// tabPage_Base
			// 
			this->tabPage_Base->Controls->Add(this->label9);
			this->tabPage_Base->Controls->Add(this->label8);
			this->tabPage_Base->Controls->Add(this->label7);
			this->tabPage_Base->Controls->Add(this->rangedFloatReflectivity);
			this->tabPage_Base->Controls->Add(this->label6);
			this->tabPage_Base->Controls->Add(this->rangedFloatBumpScale);
			this->tabPage_Base->Controls->Add(this->groupBox2);
			this->tabPage_Base->Controls->Add(this->groupBox1);
			this->tabPage_Base->Controls->Add(this->colorSpecular);
			this->tabPage_Base->Controls->Add(this->label3);
			this->tabPage_Base->Controls->Add(this->colorEmissive);
			this->tabPage_Base->Controls->Add(this->label4);
			this->tabPage_Base->Controls->Add(this->colorDiffuse);
			this->tabPage_Base->Controls->Add(this->label2);
			this->tabPage_Base->Controls->Add(this->colorAmbient);
			this->tabPage_Base->Controls->Add(this->label1);
			this->tabPage_Base->Controls->Add(this->floatEdit_UScale);
			this->tabPage_Base->Controls->Add(this->floatEdit_VScale);
			this->tabPage_Base->Location = System::Drawing::Point(4, 22);
			this->tabPage_Base->Name = S"tabPage_Base";
			this->tabPage_Base->Size = System::Drawing::Size(360, 465);
			this->tabPage_Base->TabIndex = 0;
			this->tabPage_Base->Text = S"Base";
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(16, 400);
			this->label7->Name = S"label7";
			this->label7->Size = System::Drawing::Size(94, 21);
			this->label7->TabIndex = 27;
			this->label7->Text = S"Reflectivity";
			// 
			// rangedFloatReflectivity
			// 
			this->rangedFloatReflectivity->Exponent = (System::Int16)1;
			this->rangedFloatReflectivity->Location = System::Drawing::Point(128, 392);
			this->rangedFloatReflectivity->Name = S"rangedFloatReflectivity";
			this->rangedFloatReflectivity->NumTicks = (System::Int16)100;
			this->rangedFloatReflectivity->Precision = (System::Int16)4;
			this->rangedFloatReflectivity->Size = System::Drawing::Size(206, 28);
			this->rangedFloatReflectivity->TabIndex = 26;
			this->rangedFloatReflectivity->Value = 1;
			this->rangedFloatReflectivity->ValueChanged += new System::EventHandler(this, rangedFloatReflectivity_ValueChanged);
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(16, 360);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(94, 21);
			this->label6->TabIndex = 25;
			this->label6->Text = S"Bump Map Scale";
			// 
			// rangedFloatBumpScale
			// 
			this->rangedFloatBumpScale->Exponent = (System::Int16)3;
			this->rangedFloatBumpScale->Location = System::Drawing::Point(128, 360);
			this->rangedFloatBumpScale->Maximum = 4;
			this->rangedFloatBumpScale->Name = S"rangedFloatBumpScale";
			this->rangedFloatBumpScale->NumTicks = (System::Int16)100;
			this->rangedFloatBumpScale->Precision = (System::Int16)4;
			this->rangedFloatBumpScale->Size = System::Drawing::Size(206, 28);
			this->rangedFloatBumpScale->TabIndex = 24;
			this->rangedFloatBumpScale->ValueChanged += new System::EventHandler(this, rangedFloatBumpScale_ValueChanged);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->buttonMoveDown);
			this->groupBox2->Controls->Add(this->buttonMoveUp);
			this->groupBox2->Controls->Add(this->buttonRemove);
			this->groupBox2->Controls->Add(this->buttonEdit);
			this->groupBox2->Controls->Add(this->buttonNew);
			this->groupBox2->Controls->Add(this->listTextures);
			this->groupBox2->Location = System::Drawing::Point(8, 208);
			this->groupBox2->Name = S"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(346, 146);
			this->groupBox2->TabIndex = 23;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = S"Textures";
			// 
			// buttonMoveDown
			// 
			this->buttonMoveDown->Location = System::Drawing::Point(133, 118);
			this->buttonMoveDown->Name = S"buttonMoveDown";
			this->buttonMoveDown->Size = System::Drawing::Size(80, 21);
			this->buttonMoveDown->TabIndex = 5;
			this->buttonMoveDown->Text = S"Move Down";
			this->buttonMoveDown->Click += new System::EventHandler(this, buttonMoveDown_Click);
			// 
			// buttonMoveUp
			// 
			this->buttonMoveUp->Location = System::Drawing::Point(40, 118);
			this->buttonMoveUp->Name = S"buttonMoveUp";
			this->buttonMoveUp->Size = System::Drawing::Size(80, 21);
			this->buttonMoveUp->TabIndex = 4;
			this->buttonMoveUp->Text = S"Move Up";
			this->buttonMoveUp->Click += new System::EventHandler(this, buttonMoveUp_Click);
			// 
			// buttonRemove
			// 
			this->buttonRemove->Location = System::Drawing::Point(267, 76);
			this->buttonRemove->Name = S"buttonRemove";
			this->buttonRemove->Size = System::Drawing::Size(66, 21);
			this->buttonRemove->TabIndex = 3;
			this->buttonRemove->Text = S"Remove";
			this->buttonRemove->Click += new System::EventHandler(this, buttonRemove_Click);
			// 
			// buttonEdit
			// 
			this->buttonEdit->Location = System::Drawing::Point(267, 49);
			this->buttonEdit->Name = S"buttonEdit";
			this->buttonEdit->Size = System::Drawing::Size(66, 20);
			this->buttonEdit->TabIndex = 2;
			this->buttonEdit->Text = S"Edit";
			this->buttonEdit->Click += new System::EventHandler(this, buttonEdit_Click);
			// 
			// buttonNew
			// 
			this->buttonNew->Location = System::Drawing::Point(267, 21);
			this->buttonNew->Name = S"buttonNew";
			this->buttonNew->Size = System::Drawing::Size(66, 21);
			this->buttonNew->TabIndex = 1;
			this->buttonNew->Text = S"New";
			this->buttonNew->Click += new System::EventHandler(this, buttonNew_Click);
			// 
			// listTextures
			// 
			this->listTextures->Location = System::Drawing::Point(7, 21);
			this->listTextures->Name = S"listTextures";
			this->listTextures->Size = System::Drawing::Size(240, 82);
			this->listTextures->TabIndex = 0;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->floatPower);
			this->groupBox1->Controls->Add(this->label5);
			this->groupBox1->Controls->Add(this->checkSpecular);
			this->groupBox1->Location = System::Drawing::Point(8, 144);
			this->groupBox1->Name = S"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(346, 55);
			this->groupBox1->TabIndex = 22;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = S"Specular Settings";
			// 
			// floatPower
			// 
			this->floatPower->Location = System::Drawing::Point(193, 21);
			this->floatPower->Name = S"floatPower";
			this->floatPower->Precision = (System::Int16)2;
			this->floatPower->Size = System::Drawing::Size(74, 21);
			this->floatPower->TabIndex = 2;
			this->floatPower->ValueChanged += new System::EventHandler(this, floatPower_ValueChanged);
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(133, 21);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(54, 21);
			this->label5->TabIndex = 1;
			this->label5->Text = S"Power:";
			// 
			// checkSpecular
			// 
			this->checkSpecular->Location = System::Drawing::Point(7, 21);
			this->checkSpecular->Name = S"checkSpecular";
			this->checkSpecular->Size = System::Drawing::Size(106, 21);
			this->checkSpecular->TabIndex = 0;
			this->checkSpecular->Text = S"Enabled";
			this->checkSpecular->CheckedChanged += new System::EventHandler(this, checkSpecular_CheckedChanged);
			// 
			// colorSpecular
			// 
			this->colorSpecular->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->colorSpecular->Color = System::Drawing::Color::FromArgb((System::Byte)0, (System::Byte)0, (System::Byte)0, (System::Byte)0);
			this->colorSpecular->Location = System::Drawing::Point(80, 112);
			this->colorSpecular->Name = S"colorSpecular";
			this->colorSpecular->Size = System::Drawing::Size(272, 28);
			this->colorSpecular->TabIndex = 21;
			this->colorSpecular->ValueChanged += new System::EventHandler(this, colorSpecular_ValueChanged);
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 112);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(60, 21);
			this->label3->TabIndex = 20;
			this->label3->Text = S"Specular";
			// 
			// colorEmissive
			// 
			this->colorEmissive->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->colorEmissive->Color = System::Drawing::Color::FromArgb((System::Byte)0, (System::Byte)0, (System::Byte)0, (System::Byte)0);
			this->colorEmissive->Location = System::Drawing::Point(80, 80);
			this->colorEmissive->Name = S"colorEmissive";
			this->colorEmissive->Size = System::Drawing::Size(272, 28);
			this->colorEmissive->TabIndex = 19;
			this->colorEmissive->ValueChanged += new System::EventHandler(this, colorEmissive_ValueChanged);
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(8, 80);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(60, 21);
			this->label4->TabIndex = 18;
			this->label4->Text = S"Emissive";
			// 
			// colorDiffuse
			// 
			this->colorDiffuse->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->colorDiffuse->Color = System::Drawing::Color::FromArgb((System::Byte)0, (System::Byte)0, (System::Byte)0, (System::Byte)0);
			this->colorDiffuse->Location = System::Drawing::Point(80, 48);
			this->colorDiffuse->Name = S"colorDiffuse";
			this->colorDiffuse->Size = System::Drawing::Size(272, 28);
			this->colorDiffuse->TabIndex = 17;
			this->colorDiffuse->ValueChanged += new System::EventHandler(this, colorDiffuse_ValueChanged);
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 48);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(60, 21);
			this->label2->TabIndex = 16;
			this->label2->Text = S"Diffuse";
			// 
			// colorAmbient
			// 
			this->colorAmbient->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->colorAmbient->Color = System::Drawing::Color::FromArgb((System::Byte)0, (System::Byte)0, (System::Byte)0, (System::Byte)0);
			this->colorAmbient->Location = System::Drawing::Point(80, 8);
			this->colorAmbient->Name = S"colorAmbient";
			this->colorAmbient->Size = System::Drawing::Size(272, 28);
			this->colorAmbient->TabIndex = 15;
			this->colorAmbient->ValueChanged += new System::EventHandler(this, colorAmbient_ValueChanged);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 8);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(60, 21);
			this->label1->TabIndex = 14;
			this->label1->Text = S"Ambient";
			// 
			// tabPage_Shader
			// 
			this->tabPage_Shader->Controls->Add(this->panel_shaderControls);
			this->tabPage_Shader->Controls->Add(this->label_separator);
			this->tabPage_Shader->Controls->Add(this->label17);
			this->tabPage_Shader->Controls->Add(this->comboBox_shaderName);
			this->tabPage_Shader->Location = System::Drawing::Point(4, 22);
			this->tabPage_Shader->Name = S"tabPage_Shader";
			this->tabPage_Shader->Size = System::Drawing::Size(360, 465);
			this->tabPage_Shader->TabIndex = 1;
			this->tabPage_Shader->Text = S"Shader";
			// 
			// panel_shaderControls
			// 
			this->panel_shaderControls->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->panel_shaderControls->Location = System::Drawing::Point(8, 40);
			this->panel_shaderControls->Name = S"panel_shaderControls";
			this->panel_shaderControls->Size = System::Drawing::Size(344, 424);
			this->panel_shaderControls->TabIndex = 21;
			// 
			// label_separator
			// 
			this->label_separator->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->label_separator->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_separator->Location = System::Drawing::Point(8, 32);
			this->label_separator->Name = S"label_separator";
			this->label_separator->Size = System::Drawing::Size(344, 2);
			this->label_separator->TabIndex = 20;
			// 
			// label17
			// 
			this->label17->Location = System::Drawing::Point(8, 8);
			this->label17->Name = S"label17";
			this->label17->TabIndex = 19;
			this->label17->Text = S"Shader";
			// 
			// comboBox_shaderName
			// 
			this->comboBox_shaderName->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox_shaderName->Location = System::Drawing::Point(136, 8);
			this->comboBox_shaderName->Name = S"comboBox_shaderName";
			this->comboBox_shaderName->Size = System::Drawing::Size(216, 21);
			this->comboBox_shaderName->TabIndex = 18;
			this->comboBox_shaderName->SelectedIndexChanged += new System::EventHandler(this, comboBox_shaderName_SelectedIndexChanged);
			// 
			// tabPage_Fur
			// 
			this->tabPage_Fur->Controls->Add(this->panel_furControls);
			this->tabPage_Fur->Controls->Add(this->label_separatorFur);
			this->tabPage_Fur->Controls->Add(this->checkBox_FurEnable);
			this->tabPage_Fur->Location = System::Drawing::Point(4, 22);
			this->tabPage_Fur->Name = S"tabPage_Fur";
			this->tabPage_Fur->Size = System::Drawing::Size(360, 465);
			this->tabPage_Fur->TabIndex = 2;
			this->tabPage_Fur->Text = S"Fur";
			// 
			// panel_furControls
			// 
			this->panel_furControls->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->panel_furControls->Enabled = false;
			this->panel_furControls->Location = System::Drawing::Point(8, 40);
			this->panel_furControls->Name = S"panel_furControls";
			this->panel_furControls->Size = System::Drawing::Size(344, 424);
			this->panel_furControls->TabIndex = 2;
			// 
			// label_separatorFur
			// 
			this->label_separatorFur->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_separatorFur->Location = System::Drawing::Point(8, 32);
			this->label_separatorFur->Name = S"label_separatorFur";
			this->label_separatorFur->Size = System::Drawing::Size(344, 2);
			this->label_separatorFur->TabIndex = 1;
			// 
			// checkBox_FurEnable
			// 
			this->checkBox_FurEnable->Location = System::Drawing::Point(8, 8);
			this->checkBox_FurEnable->Name = S"checkBox_FurEnable";
			this->checkBox_FurEnable->Size = System::Drawing::Size(64, 24);
			this->checkBox_FurEnable->TabIndex = 0;
			this->checkBox_FurEnable->Text = S"enable";
			this->checkBox_FurEnable->CheckedChanged += new System::EventHandler(this, checkBox_FurEnable_CheckedChanged);
			// 
			// tabPage_Glow
			// 
			this->tabPage_Glow->Controls->Add(this->label_glowMask);
			this->tabPage_Glow->Controls->Add(this->fileChooser_glowMask);
			this->tabPage_Glow->Controls->Add(this->label_separatorGlow);
			this->tabPage_Glow->Controls->Add(this->checkBox_enableGlow);
			this->tabPage_Glow->Controls->Add(this->panel_glowControls);
			this->tabPage_Glow->Location = System::Drawing::Point(4, 22);
			this->tabPage_Glow->Name = S"tabPage_Glow";
			this->tabPage_Glow->Size = System::Drawing::Size(360, 465);
			this->tabPage_Glow->TabIndex = 3;
			this->tabPage_Glow->Text = S"Glow";
			// 
			// label_glowMask
			// 
			this->label_glowMask->Location = System::Drawing::Point(8, 40);
			this->label_glowMask->Name = S"label_glowMask";
			this->label_glowMask->Size = System::Drawing::Size(72, 16);
			this->label_glowMask->TabIndex = 7;
			this->label_glowMask->Text = S"Glow Mask";
			this->label_glowMask->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			// 
			// fileChooser_glowMask
			// 
			this->fileChooser_glowMask->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->fileChooser_glowMask->Location = System::Drawing::Point(88, 40);
			this->fileChooser_glowMask->Name = S"fileChooser_glowMask";
			this->fileChooser_glowMask->Size = System::Drawing::Size(264, 24);
			this->fileChooser_glowMask->TabIndex = 6;
			this->fileChooser_glowMask->ValueChanged += new System::EventHandler(this, fileChooser_glowMask_ValueChanged);
			// 
			// label_separatorGlow
			// 
			this->label_separatorGlow->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->label_separatorGlow->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_separatorGlow->Location = System::Drawing::Point(8, 72);
			this->label_separatorGlow->Name = S"label_separatorGlow";
			this->label_separatorGlow->Size = System::Drawing::Size(344, 2);
			this->label_separatorGlow->TabIndex = 5;
			// 
			// checkBox_enableGlow
			// 
			this->checkBox_enableGlow->Location = System::Drawing::Point(8, 8);
			this->checkBox_enableGlow->Name = S"checkBox_enableGlow";
			this->checkBox_enableGlow->Size = System::Drawing::Size(64, 24);
			this->checkBox_enableGlow->TabIndex = 4;
			this->checkBox_enableGlow->Text = S"enable";
			this->checkBox_enableGlow->CheckedChanged += new System::EventHandler(this, checkBox_enableGlow_CheckedChanged);
			// 
			// panel_glowControls
			// 
			this->panel_glowControls->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->panel_glowControls->Enabled = false;
			this->panel_glowControls->Location = System::Drawing::Point(8, 80);
			this->panel_glowControls->Name = S"panel_glowControls";
			this->panel_glowControls->Size = System::Drawing::Size(344, 384);
			this->panel_glowControls->TabIndex = 3;
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(16, 432);
			this->label8->Name = S"label8";
			this->label8->Size = System::Drawing::Size(48, 21);
			this->label8->TabIndex = 28;
			this->label8->Text = S"U Scale";
			// 
			// label9
			// 
			this->label9->Location = System::Drawing::Point(176, 432);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(48, 21);
			this->label9->TabIndex = 29;
			this->label9->Text = S"V Scale";
			// 
			// floatEdit_UScale
			// 
			this->floatEdit_UScale->Location = System::Drawing::Point(72, 432);
			this->floatEdit_UScale->Name = S"floatEdit_UScale";
			this->floatEdit_UScale->Precision = (System::Int16)2;
			this->floatEdit_UScale->Size = System::Drawing::Size(56, 21);
			this->floatEdit_UScale->TabIndex = 3;
			this->floatEdit_UScale->ValueChanged += new System::EventHandler(this, floatEdit_UScale_ValueChanged);
			// 
			// floatEdit_VScale
			// 
			this->floatEdit_VScale->Location = System::Drawing::Point(232, 432);
			this->floatEdit_VScale->Name = S"floatEdit_VScale";
			this->floatEdit_VScale->Precision = (System::Int16)2;
			this->floatEdit_VScale->Size = System::Drawing::Size(56, 21);
			this->floatEdit_VScale->TabIndex = 3;
			this->floatEdit_VScale->ValueChanged += new System::EventHandler(this, floatEdit_VScale_ValueChanged);
			// 
			// MaterialDialog
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(384, 509);
			this->Controls->Add(this->tabControl1);
			this->Name = S"MaterialDialog";
			this->Text = S"Material Editor";
			this->tabControl1->ResumeLayout(false);
			this->tabPage_Base->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->groupBox1->ResumeLayout(false);
			this->tabPage_Shader->ResumeLayout(false);
			this->tabPage_Fur->ResumeLayout(false);
			this->tabPage_Glow->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//
			
		//============================================================================
		//============================================================================
		const char* get_blend_string(mtrTextureLayer::TextureType i_Type)
		{
			switch (i_Type)
			{
			default:
				return "Unknown";
			case mtrTextureLayer::e_Normal:
				return "";
			case mtrTextureLayer::e_AlphaBlend:
				return "(Alpha Blend)";
			case mtrTextureLayer::e_BumpHeight:
				return "(Bump Map)";
			case mtrTextureLayer::e_SpecularMap:
				return "(Specular Map)";
			case mtrTextureLayer::e_EnvMap:
				return "(Env Map)";
			case mtrTextureLayer::e_GlossMap:
				return "(Gloss Map)";
			}
		}

		//============================================================================
		//	get_layer_string takes texture layer reference and creates
		//	std::string to describe it to users
		//============================================================================
		String* get_layer_string(const mtrTextureLayer& i_Layer)
		{
			std::string str;
			switch (i_Layer.GetLayerType())
			{
			default:
				str = "Unknown Layer Type";
				break;
			case mtrTextureLayer::e_Basic:
				str += i_Layer.GetTextureName();
				str += " ";
				str += get_blend_string(i_Layer.GetTextureType());
				break;
			}
			return new String(str.c_str());
		}

		//============================================================================
		//============================================================================
		void SetupTextureList(const mtrMaterialTemplate &i_Data)
		{
			// Setup list of texture layers
			listTextures->Items->Clear();
			int num_layers = i_Data.GetNumTextureLayers();
			for (int i=0; i<num_layers; i++)
			{
				listTextures->Items->Add(get_layer_string(i_Data.GetTextureLayer(i)));
			}
		}

		//============================================================================
		//============================================================================
		void SetupShaderList()
		{
			comboBox_shaderName->Items->Clear();
			comboBox_shaderName->Items->Add(new System::String("<NONE>"));
			const std::list<std::string>& names = effShaderArray::GetSpecialEffectNames();
			std::list<std::string>::const_iterator ni;
			for (ni = names.begin(); ni != names.end(); ni++)
			{
				comboBox_shaderName->Items->Add(new System::String((ni)->c_str()));
			}
		}

		//============================================================================
		//============================================================================
		void SetupShaderPanel(mtrMaterialTemplate& i_Data)
		{
			panel_shaderControls->Controls->Clear();

			// read list of params from data and auto-layout the dialog box
			mtrShaderParams& params = i_Data.ShaderParams();

			matShaderEffect* eff = effShaderArray::GetSpecialEffect(params.m_shaderName);
			if (eff != NULL) 
			{
				int index = comboBox_shaderName->FindString(new System::String(params.m_shaderName.c_str()));
				if (index < 1) index = 0;
				comboBox_shaderName->SelectedIndex = index;

				panel_shaderControls->Enabled = true;
				AddShaderUI(params, panel_shaderControls, 100, 8, 0, 32);
			}
			else
				panel_shaderControls->Enabled = false;
		}
		void SetupFurPanel(mtrMaterialTemplate& i_Data)
		{
			panel_furControls->Controls->Clear();

			mtrShaderParams& furParams = i_Data.FurParams();
			matShaderEffect* eff = effShaderArray::GetSpecialEffect(furParams.m_shaderName);
			bool showFur = ((eff != NULL) && i_Data.GetHasFur());

			panel_furControls->Enabled = showFur;
			if (showFur)
			{
				AddShaderUI(furParams, panel_furControls, 100, 8, 0, 32);
			}
			checkBox_FurEnable->Checked = showFur;

		}
		void SetupGlowPanel(mtrMaterialTemplate& i_Data)
		{
			panel_glowControls->Controls->Clear();

			mtrShaderParams& glowParams = i_Data.GlowParams();
			matShaderEffect* eff = effShaderArray::GetSpecialEffect(glowParams.m_shaderName);
			bool showGlow = ((eff != NULL) && i_Data.GetHasGlow());

			panel_glowControls->Enabled = showGlow;
			if (showGlow)
			{
				AddShaderUI(glowParams, panel_glowControls, 100, 8, 0, 32);
			}
			checkBox_enableGlow->Checked = showGlow;
			fileChooser_glowMask->Enabled = showGlow;

			fsLocator locPath = mtrLevel::GetTextureDir();
			locPath.Push(i_Data.GetGlowMask().GetTextureName().c_str());
			if (fsFileUtil::FileExists(locPath))
				fileChooser_glowMask->Fullpath = tmaManagedStringUtils::LocatorToManagedString(locPath);
		}

		//============================================================================
		//============================================================================
		void AddShaderUI(mtrShaderParams &params, System::Windows::Forms::Panel* panel, 
			int startX, int startLabelX, int startY, int ctrlHt)
		{
			// auto-layout the dialog box
			matShaderEffect* eff = effShaderArray::GetSpecialEffect(params.m_shaderName);
			if (eff == NULL) 
				return;

			std::list<matShaderParamUI> paramUIList;
			eff->GetAllParamUIs(paramUIList);

			float val;
			maVector4d vval;
			bool bval;
			std::string sval;

			int curX = startX;
			int curLabelX = startLabelX;
			int curY = startY;
			int ctrlLabelWid = curX-curLabelX;
			int ctrlWid = panel->Width - curX;

			std::list<matShaderParamUI>::iterator i = paramUIList.begin();
			while (i != paramUIList.end())
			{
				matShaderParamUI paramUI = *i;
				if (paramUI.Visible())
				{
					// first, the label
					std::string ctrlLabelName("label_");
					ctrlLabelName += paramUI.m_name;
					System::Windows::Forms::Label *	ctrlLabel = new System::Windows::Forms::Label();
//					this->tabPage_Shader->Controls->Add(ctrlLabel);
					panel->Controls->Add(ctrlLabel);
					ctrlLabel->Location = System::Drawing::Point(curLabelX, curY);
					ctrlLabel->Name = ctrlLabelName.c_str();
					ctrlLabel->Text = paramUI.m_label.c_str();
					ctrlLabel->Size = System::Drawing::Size(ctrlLabelWid, ctrlHt);
					
					// now build the control
					switch (paramUI.m_uiType)
					{
					case matShaderParamUI::e_Checkbox:
						{
							DBG_ASSERT0(paramUI.m_dataType == matShaderParamUI::e_Bool, "Shader param using Checkbox control is not bool value!");

							std::string ctrlName("checkBox_");
							ctrlName += paramUI.m_name;
							System::Windows::Forms::CheckBox*  ctrl = new System::Windows::Forms::CheckBox();
//							this->tabPage_Shader->Controls->Add(ctrl);
							panel->Controls->Add(ctrl);
							ctrl->Location = System::Drawing::Point(curX, curY);
							ctrl->Name = ctrlName.c_str();
							ctrl->Size = System::Drawing::Size(ctrlWid, ctrlHt);
							if (params.GetBool(paramUI.m_name, bval))
							{
								ctrl->Checked = bval;
							}
							else
							{
								bval = (paramUI.m_fval[0] == 1) ? true : false;
								ctrl->Checked = bval;
								params.m_bools[paramUI.m_name] = bval;
							}
							ctrl->CheckedChanged += new System::EventHandler(this, shaderControl_BoolValueChanged);
							ctrl->Enabled = panel->Enabled;
						}
						break;
					case matShaderParamUI::e_Numeric:
						{
							DBG_ASSERT0(paramUI.m_dataType == matShaderParamUI::e_Float, "Shader param using Numeric control is not float value!");

							std::string ctrlName("floatEdit_");
							ctrlName += paramUI.m_name;
							System::Windows::Forms::NumericUpDown*  ctrl = new System::Windows::Forms::NumericUpDown();
//							this->tabPage_Shader->Controls->Add(ctrl);
							panel->Controls->Add(ctrl);
							ctrl->Location = System::Drawing::Point(curX, curY);
							ctrl->Maximum = paramUI.m_max;
							ctrl->Minimum = paramUI.m_min;
							ctrl->Name = ctrlName.c_str();
							ctrl->DecimalPlaces = (System::Int16)3;
							ctrl->Size = System::Drawing::Size(ctrlWid, ctrlHt);
							if (params.GetFloat(paramUI.m_name, val))
							{
								ctrl->Value = val;
							}
							else
							{
								ctrl->Value = paramUI.m_fval[0];
								params.m_floats[paramUI.m_name] = paramUI.m_fval[0];
							}
							ctrl->ValueChanged += new System::EventHandler(this, shaderControl_FloatValueChanged);
							ctrl->Enabled = panel->Enabled;
						}
						break;
					case matShaderParamUI::e_Slider:
						{
							DBG_ASSERT0(paramUI.m_dataType == matShaderParamUI::e_Float, "Shader param using Slider control is not float value!");

							std::string ctrlName("rangedFloat_");
							ctrlName += paramUI.m_name;
							TerawattManagedControls::RangedFloat *  ctrl = new TerawattManagedControls::RangedFloat();
//							this->tabPage_Shader->Controls->Add(ctrl);
							panel->Controls->Add(ctrl);
							ctrl->Exponent = (System::Int16)((int)paramUI.m_power);
							ctrl->Location = System::Drawing::Point(curX, curY);
							ctrl->Maximum = paramUI.m_max;
							ctrl->Minimum = paramUI.m_min;
							ctrl->Name = ctrlName.c_str();
							ctrl->NumTicks = (System::Int16)((int)paramUI.m_steps);
							ctrl->Precision = (System::Int16)2;
							ctrl->Size = System::Drawing::Size(ctrlWid, ctrlHt);
							if (params.GetFloat(paramUI.m_name, val))
							{
								ctrl->Value = val;
							}
							else
							{
								ctrl->Value = paramUI.m_fval[0];
								params.m_floats[paramUI.m_name] = paramUI.m_fval[0];
							}
							ctrl->ValueChanged += new System::EventHandler(this, shaderControl_SliderValueChanged);
							ctrl->Enabled = panel->Enabled;
						}
						break;
					case matShaderParamUI::e_ColorPicker:
						{
							DBG_ASSERT0(paramUI.m_dataType == matShaderParamUI::e_Vector, "Shader param using Colorpicker is not vector value!");

							std::string ctrlName("colorRGBAEdit_");
							ctrlName += paramUI.m_name;
							TerawattManagedControls::ColorRGBAEdit *  ctrl = new TerawattManagedControls::ColorRGBAEdit();
//							this->tabPage_Shader->Controls->Add(ctrl);
							panel->Controls->Add(ctrl);
							ctrl->Location = System::Drawing::Point(curX, curY);
							ctrl->Name = ctrlName.c_str();
							ctrl->Size = System::Drawing::Size(ctrlWid, ctrlHt);
							if (params.GetVector(paramUI.m_name, vval))
							{
								ctrl->Color = tmaManagedConversionUtil::SetColorRGBA(
									maFloatRGBA(vval.m_X, vval.m_Y, vval.m_Z, vval.m_W));
							}
							else
							{
								ctrl->Color = tmaManagedConversionUtil::SetColorRGBA(
									maFloatRGBA(paramUI.m_fval[0],paramUI.m_fval[1],
									paramUI.m_fval[2],paramUI.m_fval[3]));
								params.m_vectors[paramUI.m_name] = maVector4d(paramUI.m_fval[0],paramUI.m_fval[1],
									paramUI.m_fval[2],paramUI.m_fval[3]);
							}
							ctrl->ValueChanged += new System::EventHandler(this, shaderControl_ColorValueChanged);
							ctrl->Enabled = panel->Enabled;
						}
						break;
					case matShaderParamUI::e_FolderPicker:
						{
							DBG_ASSERT0(paramUI.m_dataType == matShaderParamUI::e_String, "Shader param using FolderChooser control is not string value!");

							std::string ctrlName("folderChooser_");
							ctrlName += paramUI.m_name;
							TerawattManagedControls::FolderChooser *  ctrl = new TerawattManagedControls::FolderChooser();
//							this->tabPage_Shader->Controls->Add(ctrl);
							panel->Controls->Add(ctrl);
							ctrl->Location = System::Drawing::Point(curX, curY);
							ctrl->Name = ctrlName.c_str();
							ctrl->Size = System::Drawing::Size(ctrlWid, ctrlHt);
							if (params.GetString(paramUI.m_name, sval))
							{
								ctrl->Directory = sval.c_str();
							}
							else
							{
								ctrl->Directory = paramUI.m_sval.c_str();
								params.m_strings[paramUI.m_name] = paramUI.m_sval;
							}
							ctrl->ValueChanged += new System::EventHandler(this, shaderControl_FolderValueChanged);
							ctrl->Enabled = panel->Enabled;
						}
						break;
					case matShaderParamUI::e_TexturePicker:
						{
							DBG_ASSERT0(paramUI.m_dataType == matShaderParamUI::e_Texture, "Shader param using FileChooser control is not texture value!");

							std::string ctrlName("fileChooser_");
							ctrlName += paramUI.m_name;
							TerawattManagedControls::FileChooser*  ctrl = new TerawattManagedControls::FileChooser();
							panel->Controls->Add(ctrl);
							ctrl->Location = System::Drawing::Point(curX, curY);
							ctrl->Name = ctrlName.c_str();
							ctrl->Size = System::Drawing::Size(ctrlWid, ctrlHt);
							if (params.GetTexture(paramUI.m_name, sval))
							{
								fsLocator locPath = mtrLevel::GetTextureDir();
								locPath.Push(sval.c_str());
								ctrl->Fullpath = tmaManagedStringUtils::LocatorToManagedString(locPath);
							}
							else
							{
								fsLocator locPath = mtrLevel::GetTextureDir();
								locPath.Push(paramUI.m_sval.c_str());
								ctrl->Fullpath = tmaManagedStringUtils::LocatorToManagedString(locPath);
								std::string fullpath;
								fsFileUtil::LocatorToANSIFilename(locPath, fullpath);
								params.m_textures[paramUI.m_name] = paramUI.m_sval;
							}
							ctrl->ValueChanged += new System::EventHandler(this, shaderControl_FileValueChanged);
							ctrl->Enabled = panel->Enabled;
						}
						break;
					case matShaderParamUI::e_Direction:
						{
							DBG_ASSERT0(paramUI.m_dataType == matShaderParamUI::e_Vector, "Shader param using Direction is not vector value!");

							std::string ctrlName("vector3Edit_");
							ctrlName += paramUI.m_name;
							TerawattManagedControls::Vector3Edit *  ctrl = new TerawattManagedControls::Vector3Edit();
//							this->tabPage_Shader->Controls->Add(ctrl);
							panel->Controls->Add(ctrl);
							ctrl->Location = System::Drawing::Point(curX, curY);
							ctrl->Name = ctrlName.c_str();
							ctrl->Size = System::Drawing::Size(ctrlWid, ctrlHt);
							if (params.GetVector(paramUI.m_name, vval))
							{
								ctrl->ValueX = vval.m_X;
								ctrl->ValueY = vval.m_Y;
								ctrl->ValueZ = vval.m_Z;
							}
							else
							{
								ctrl->ValueX = paramUI.m_fval[0];
								ctrl->ValueY = paramUI.m_fval[1];
								ctrl->ValueZ = paramUI.m_fval[2];
								params.m_vectors[paramUI.m_name] = maVector4d(
									paramUI.m_fval[0],paramUI.m_fval[1],paramUI.m_fval[2],1);
							}
							ctrl->ValueChanged += new System::EventHandler(this, shaderControl_VectorValueChanged);
							ctrl->Enabled = panel->Enabled;
						}
						break;
					}

					// move down one row if we added a control
					curY += ctrlHt;
				}
				i++;
			}
		}


		//============================================================================
		//============================================================================
		void SetupData(mtrMaterialTemplate &i_Data)
		{
			if (m_bOurChange) return;

			m_bDisableNotify = true;

			colorAmbient->Color = tmaManagedConversionUtil::SetColorRGBA(i_Data.GetAmbient());
			colorDiffuse->Color = tmaManagedConversionUtil::SetColorRGBA(i_Data.GetDiffuse());
			colorEmissive->Color = tmaManagedConversionUtil::SetColorRGBA(i_Data.GetEmissive());
			colorSpecular->Color = tmaManagedConversionUtil::SetColorRGBA(i_Data.GetSpecular());
			checkSpecular->Checked = i_Data.GetHasSpecular();
			floatPower->Value = i_Data.GetSpecularPower();
			rangedFloatBumpScale->Value = i_Data.GetBumpMapScale();
			rangedFloatReflectivity->Value = i_Data.GetReflectivity();
			floatEdit_UScale->Value = i_Data.GetUScale();
			floatEdit_VScale->Value = i_Data.GetVScale();

			// Setup list of texture layers
			SetupTextureList(i_Data);

			SetupShaderList();

			SetupShaderPanel(i_Data);
			SetupFurPanel(i_Data);
			SetupGlowPanel(i_Data);

			m_bDisableNotify = false;
		}

		//============================================================================
		//============================================================================
private: System::Void buttonNew_Click(System::Object *  sender, System::EventArgs *  e);
private: System::Void buttonEdit_Click(System::Object *  sender, System::EventArgs *  e);
private: System::Void buttonRemove_Click(System::Object *  sender, System::EventArgs *  e);
private: System::Void buttonMoveUp_Click(System::Object *  sender, System::EventArgs *  e);
private: System::Void buttonMoveDown_Click(System::Object *  sender, System::EventArgs *  e);

		//============================================================================
		//============================================================================
private: System::Void colorAmbient_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		{
			if (!m_bDisableNotify) {
				m_Data.SetAmbient(tmaManagedConversionUtil::ConvertColorRGBA(colorAmbient->Color));
				mtrOperations::ChangeMaterialData(m_Data);
			}
		}

private: System::Void colorDiffuse_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			if (!m_bDisableNotify) {
				m_Data.SetDiffuse(tmaManagedConversionUtil::ConvertColorRGBA(colorDiffuse->Color));
				mtrOperations::ChangeMaterialData(m_Data);
			}
		 }

private: System::Void colorEmissive_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			if (!m_bDisableNotify) {
				m_Data.SetEmissive(tmaManagedConversionUtil::ConvertColorRGBA(colorEmissive->Color));
				mtrOperations::ChangeMaterialData(m_Data);
			}
		 }

private: System::Void colorSpecular_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			if (!m_bDisableNotify) {
				m_Data.SetSpecular(tmaManagedConversionUtil::ConvertColorRGBA(colorSpecular->Color));
				mtrOperations::ChangeMaterialData(m_Data);
			}
		 }

private: System::Void checkSpecular_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			 bool bSpecEnabled = this->checkSpecular->Checked;
			 this->floatPower->Enabled = bSpecEnabled;
			 this->colorSpecular->Enabled = bSpecEnabled;

			if (!m_bDisableNotify) {
				m_Data.SetHasSpecular(bSpecEnabled);
				mtrOperations::ChangeMaterialData(m_Data);
			}
		 }

private: System::Void floatPower_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			if (!m_bDisableNotify) {
				m_Data.SetSpecularPower((float)floatPower->Value);
				mtrOperations::ChangeMaterialData(m_Data);
			}
		 }

private: System::Void rangedFloatBumpScale_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			if (!m_bDisableNotify) {
				m_Data.SetBumpMapScale((float)rangedFloatBumpScale->Value);
				mtrOperations::ChangeMaterialData(m_Data);
			}
		 }

private: System::Void rangedFloatReflectivity_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			if (!m_bDisableNotify) {
				m_Data.SetReflectivity((float)rangedFloatReflectivity->Value);
				mtrOperations::ChangeMaterialData(m_Data);
			}
		 }

private: System::Void shaderControl_SliderValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			 if (!m_bDisableNotify) 
			 {
				 // extract name of shader param
				TerawattManagedControls::RangedFloat* ctrl = dynamic_cast<TerawattManagedControls::RangedFloat*>(sender);
				std::string name = tmaManagedConversionUtil::ConvertString(ctrl->Name);
				int i = name.find("rangedFloat_");
				DBG_ASSERT0((i == 0), "Bad event handler for float control");
				std::string pname = name.substr(12, name.length()-12);

				// then extract value from control
				float val = (float)ctrl->Value;

				 // then set it!
				if (ctrl->Parent == panel_furControls)
					m_Data.FurParams().m_floats[pname] = val;
				else if (ctrl->Parent == panel_glowControls)
					m_Data.GlowParams().m_floats[pname] = val;
				else
					m_Data.ShaderParams().m_floats[pname] = val;
				 mtrOperations::ChangeMaterialData(m_Data);
			 }
		 }
private: System::Void shaderControl_FolderValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			 if (!m_bDisableNotify) 
			 {
				 // extract name of shader param
				TerawattManagedControls::FolderChooser* ctrl = dynamic_cast<TerawattManagedControls::FolderChooser*>(sender);
				std::string name = tmaManagedConversionUtil::ConvertString(ctrl->Name);
				int i = name.find("folderChooser_");
				DBG_ASSERT0((i == 0), "Bad event handler for folder chooser control");
				std::string pname = name.substr(14, name.length()-14);

				// then extract value from control
				std::string val = tmaManagedConversionUtil::ConvertString(ctrl->Directory);

				// make sure the path exists
				fsLocator locPath;
				locPath.Push(val.c_str());
				bool ok = fsFileUtil::DirectoryExists(locPath);

				// special behavior. this is assumed to be a TEXTURE PATH.
				// strip all but the last part of the path, and make sure it's in a valid place
				size_t extnPos = val.find_last_of('\\');
				std::string subval = val.substr(extnPos + 1);

				if (ok)
				{
					// then set it!
					if (ctrl->Parent == panel_furControls)
					{
						m_Data.FurParams().m_strings[pname] = subval;
					}
					else if (ctrl->Parent == panel_glowControls)
					{
						m_Data.GlowParams().m_strings[pname] = subval;
					}
					else
					{
						m_Data.ShaderParams().m_strings[pname] = subval;
					}
					mtrOperations::ChangeMaterialData(m_Data);
				}
				else
				{
					System::Windows::Forms::MessageBox::Show("Please press Browse to pick an existing folder");
					// reset back to original value:
					if (ctrl->Parent == panel_furControls)
					{
						val = m_Data.FurParams().m_strings[pname];
					}
					else if (ctrl->Parent == panel_glowControls)
					{
						val = m_Data.GlowParams().m_strings[pname];
					}
					else
					{
						val = m_Data.ShaderParams().m_strings[pname];
					}
					ctrl->Directory = tmaManagedStringUtils::ItStringToManagedString(itString(val.c_str()));
				}
			 }
		 }
private: System::Void shaderControl_FileValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			 if (!m_bDisableNotify) 
			 {
				 // extract name of shader param
				TerawattManagedControls::FileChooser* ctrl = dynamic_cast<TerawattManagedControls::FileChooser*>(sender);
				std::string name = tmaManagedConversionUtil::ConvertString(ctrl->Name);
				int i = name.find("fileChooser_");
				DBG_ASSERT0((i == 0), "Bad event handler for file chooser control");
				std::string pname = name.substr(12, name.length()-12);

				// then extract value from control
				std::string val = tmaManagedConversionUtil::ConvertString(ctrl->Filename);

				fsLocator locPath = mtrLevel::GetTextureDir();
				locPath.Push(val.c_str());
				bool ok = fsFileUtil::FileExists(locPath);
				if (ok)
				{
					// then set it!
					if (ctrl->Parent == panel_furControls)
					{
						m_Data.FurParams().m_textures[pname] = val;
					}
					else if (ctrl->Parent == panel_glowControls)
					{
						m_Data.GlowParams().m_textures[pname] = val;
					}
					else
					{
						m_Data.ShaderParams().m_textures[pname] = val;
					}
					mtrOperations::ChangeMaterialData(m_Data);
				}
				else
				{
					System::Windows::Forms::MessageBox::Show("Please press Browse to pick an existing folder");
					// reset back to original value:
					if (ctrl->Parent == panel_furControls)
					{
						val = m_Data.FurParams().m_textures[pname];
					}
					else if (ctrl->Parent == panel_glowControls)
					{
						val = m_Data.GlowParams().m_textures[pname];
					}
					else
					{
						val = m_Data.ShaderParams().m_textures[pname];
					}
					locPath.Pop();
					locPath.Push(val.c_str());
					std::string fullpath;
					fsFileUtil::LocatorToANSIFilename(locPath, fullpath);
					ctrl->Fullpath = tmaManagedStringUtils::LocatorToManagedString(locPath);
				}
			 }
		 }

private: System::Void shaderControl_FloatValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			 if (!m_bDisableNotify) 
			 {
				 // extract name of shader param
				System::Windows::Forms::NumericUpDown* ctrl = dynamic_cast<System::Windows::Forms::NumericUpDown*>(sender);
				std::string name = tmaManagedConversionUtil::ConvertString(ctrl->Name);
				int i = name.find("floatEdit_");
				DBG_ASSERT0((i == 0), "Bad event handler for float control");
				std::string pname = name.substr(10, name.length()-10);

				// then extract value from control
				float val = (float)ctrl->Value;

				 // then set it!
				if (ctrl->Parent == panel_furControls)
					m_Data.FurParams().m_floats[pname] = val;
				else if (ctrl->Parent == panel_glowControls)
					m_Data.GlowParams().m_floats[pname] = val;
				else
					m_Data.ShaderParams().m_floats[pname] = val;
				mtrOperations::ChangeMaterialData(m_Data);
			 }
		 }

private: System::Void shaderControl_BoolValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			 if (!m_bDisableNotify) 
			 {
				 // extract name of shader param
				System::Windows::Forms::CheckBox* ctrl = dynamic_cast<System::Windows::Forms::CheckBox*>(sender);
				std::string name = tmaManagedConversionUtil::ConvertString(ctrl->Name);
				int i = name.find("checkBox_");
				DBG_ASSERT0((i == 0), "Bad event handler for bool control");
				std::string pname = name.substr(9, name.length()-9);

				// then extract value from control
				bool val = ctrl->Checked;

				 // then set it!
				if (ctrl->Parent == panel_furControls)
					m_Data.FurParams().m_bools[pname] = val;
				else if (ctrl->Parent == panel_glowControls)
					m_Data.GlowParams().m_bools[pname] = val;
				else
					m_Data.ShaderParams().m_bools[pname] = val;
				mtrOperations::ChangeMaterialData(m_Data);
			 }
		 }

private: System::Void shaderControl_ColorValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			 if (!m_bDisableNotify) 
			 {
				 // extract name of shader param
				TerawattManagedControls::ColorRGBAEdit* ctrl = dynamic_cast<TerawattManagedControls::ColorRGBAEdit*>(sender);
				std::string name = tmaManagedConversionUtil::ConvertString(ctrl->Name);
				int i = name.find("colorRGBAEdit_");
				DBG_ASSERT0((i == 0), "Bad event handler for float control");
				std::string pname = name.substr(14, name.length()-14);

				// then extract value from control
				maFloatRGBA val = tmaManagedConversionUtil::ConvertColorRGBA(ctrl->Color);

				 // then set it!
				if (ctrl->Parent == panel_furControls)
					m_Data.FurParams().m_vectors[pname] = maVector4d(val.m_Red,val.m_Green,val.m_Blue,val.m_Alpha);
				else if (ctrl->Parent == panel_glowControls)
					m_Data.GlowParams().m_vectors[pname] = maVector4d(val.m_Red,val.m_Green,val.m_Blue,val.m_Alpha);
				else
					m_Data.ShaderParams().m_vectors[pname] = maVector4d(val.m_Red,val.m_Green,val.m_Blue,val.m_Alpha);
				mtrOperations::ChangeMaterialData(m_Data);
			 }
		 }
private: System::Void shaderControl_VectorValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			 if (!m_bDisableNotify) 
			 {
				 // extract name of shader param
				TerawattManagedControls::Vector3Edit* ctrl = dynamic_cast<TerawattManagedControls::Vector3Edit*>(sender);
				std::string name = tmaManagedConversionUtil::ConvertString(ctrl->Name);
				int i = name.find("vector3Edit_");
				DBG_ASSERT0((i == 0), "Bad event handler for float control");
				std::string pname = name.substr(12, name.length()-12);

				// then extract value from control
				maVector4d val = maVector4d((float)ctrl->ValueX, (float)ctrl->ValueY, (float)ctrl->ValueZ, 1);

				 // then set it!
				if (ctrl->Parent == panel_furControls)
					m_Data.FurParams().m_vectors[pname] = val;
				else if (ctrl->Parent == panel_glowControls)
					m_Data.GlowParams().m_vectors[pname] = val;
				else
					m_Data.ShaderParams().m_vectors[pname] = val;
				mtrOperations::ChangeMaterialData(m_Data);
			 }
		 }

private: System::Void comboBox_shaderName_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			 if (!m_bDisableNotify)
			 {
				 std::string newName = tmaManagedConversionUtil::ConvertString(comboBox_shaderName->Text);
				 
				 if (newName != m_Data.GetShaderParams().m_shaderName)
				 {
					matShaderEffect* eff = effShaderArray::GetSpecialEffect(newName);

					m_Data.ShaderParams().Clear();
					m_Data.ShaderParams().m_shaderName = newName;

					SetupShaderPanel(m_Data);
					mtrOperations::ChangeMaterialData(m_Data);
				 }
			 }
		 }

private: System::Void checkBox_FurEnable_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
		 {
             if (!m_bDisableNotify)
			 {
				 m_Data.SetHasFur(checkBox_FurEnable->Checked);
				 if (checkBox_FurEnable->Checked)
					 m_Data.FurParams().m_shaderName = "Fur.fx";
				 else
					 m_Data.FurParams().m_shaderName = "";

				 // enable/disable the UI
				 panel_furControls->Enabled = checkBox_FurEnable->Checked;
				 SetupFurPanel(m_Data);
				 mtrOperations::ChangeMaterialData(m_Data);
			 }
		 }

private: System::Void checkBox_enableGlow_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
		 {
             if (!m_bDisableNotify)
			 {
				 m_Data.SetHasGlow(checkBox_enableGlow->Checked);
				 if (checkBox_enableGlow->Checked)
					 m_Data.GlowParams().m_shaderName = "Glow.fx";
				 else
					 m_Data.GlowParams().m_shaderName = "";


				 // enable/disable the UI
				 panel_glowControls->Enabled = checkBox_enableGlow->Checked;
				 fileChooser_glowMask->Enabled = checkBox_enableGlow->Checked;
				 SetupGlowPanel(m_Data);
				 mtrOperations::ChangeMaterialData(m_Data);
			 }
		 }

private: System::Void fileChooser_glowMask_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			TerawattManagedControls::FileChooser* ctrl = dynamic_cast<TerawattManagedControls::FileChooser*>(sender);
			// then extract value from control
			std::string val = tmaManagedConversionUtil::ConvertString(ctrl->Filename);

			fsLocator locPath = mtrLevel::GetTextureDir();
			locPath.Push(val.c_str());
			bool ok = fsFileUtil::FileExists(locPath);
			if (ok || val.empty())
			{
				// then set it!
				m_Data.GlowMask().SetTextureName(val);
				mtrOperations::ChangeMaterialData(m_Data);
			}
			else
			{
				System::Windows::Forms::MessageBox::Show("Please press Browse to pick an existing file");
				val = m_Data.GetGlowMask().GetTextureName();
				locPath.Pop();
				locPath.Push(val.c_str());
				std::string fullpath;
				fsFileUtil::LocatorToANSIFilename(locPath, fullpath);
				ctrl->Fullpath = tmaManagedStringUtils::LocatorToManagedString(locPath);
			}
		 }

private: System::Void floatEdit_UScale_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			if (!m_bDisableNotify) {
				m_Data.SetUScale((float)floatEdit_UScale->Value);
				mtrOperations::ChangeMaterialData(m_Data);
			}
		 }

private: System::Void floatEdit_VScale_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			if (!m_bDisableNotify) {
				m_Data.SetVScale((float)floatEdit_VScale->Value);
				mtrOperations::ChangeMaterialData(m_Data);
			}
		 }

private: void launch_texture_edit();

};
}