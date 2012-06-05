#pragma once

#ifndef EVMT_ENVIRONMENTMGR_HPP
#include "evmtEnvironmentMgr.hpp"
#endif
#ifndef ENVT_OPERATIONS_HPP
#include "envtOperations.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "nameString.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "tmaManagedConversionUtil.hpp"
#endif

#include "envtTextureChooser.h"

#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace SystemEnvironments
{
	/// <summary> 
	/// Summary for envtEnvironmentPrtyForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class envtEnvironmentPrtyForm : public System::Windows::Forms::Form
	{
	public: 
		envtEnvironmentPrtyForm(const nameString &i_EnvironmentName)
			: m_EnvironmentName(i_EnvironmentName)
		{
			InitializeComponent();

			m_bDisableNotify = true;
			labelEnvironmentName->Text = gcnew System::String(i_EnvironmentName.GetString().c_str());
			// populate UI controls
//			textBox_TextureFileDiffuse->Text = tmaManagedStringUtils::ItStringToManagedString(
//				evmtEnvironmentMgr::GetDiffuseMapName(i_EnvironmentName));
//			textBox_TextureFileSpecular->Text = tmaManagedStringUtils::ItStringToManagedString(
//				evmtEnvironmentMgr::GetSpecularMapName(i_EnvironmentName));
//			rangedFloat_diffuseFactor->Value = evmtEnvironmentMgr::GetDiffuseFactor(i_EnvironmentName);
//			rangedFloat_specularFactor->Value = evmtEnvironmentMgr::GetSpecularFactor(i_EnvironmentName);
//			rangedFloat_diffuseAngle->Value = evmtEnvironmentMgr::GetDiffuseAngle(i_EnvironmentName);
//			rangedFloat_specularAngle->Value = evmtEnvironmentMgr::GetSpecularAngle(i_EnvironmentName);

			m_bDisableNotify = false;
		}
        
	public: 
		~envtEnvironmentPrtyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label ^  labelEnvironmentName;




		// Turns off notify callbacks when setting up form
	private: bool m_bDisableNotify;
	private: const nameString &m_EnvironmentName;
	private: System::Windows::Forms::Button ^  button_BrowseTextureDiffuse;
	private: System::Windows::Forms::TextBox ^  textBox_TextureFileDiffuse;
	private: System::Windows::Forms::Button ^  button_BrowseTextureSpecular;
	private: System::Windows::Forms::TextBox ^  textBox_TextureFileSpecular;
	private: System::Windows::Forms::Label ^  label1;
	private: System::Windows::Forms::Label ^  label2;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_diffuseFactor;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_specularFactor;
	private: System::Windows::Forms::Button ^  button_diffuseRemove;
	private: System::Windows::Forms::Button ^  button_specularRemove;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_specularAngle;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_diffuseAngle;
	private: System::Windows::Forms::Label ^  label3;
	private: System::Windows::Forms::Label ^  label4;


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
			this->labelEnvironmentName = gcnew System::Windows::Forms::Label();
			this->button_BrowseTextureDiffuse = gcnew System::Windows::Forms::Button();
			this->textBox_TextureFileDiffuse = gcnew System::Windows::Forms::TextBox();
			this->button_BrowseTextureSpecular = gcnew System::Windows::Forms::Button();
			this->textBox_TextureFileSpecular = gcnew System::Windows::Forms::TextBox();
			this->label1 = gcnew System::Windows::Forms::Label();
			this->label2 = gcnew System::Windows::Forms::Label();
			this->rangedFloat_diffuseFactor = gcnew TerawattManagedControls::RangedFloat();
			this->rangedFloat_specularFactor = gcnew TerawattManagedControls::RangedFloat();
			this->button_diffuseRemove = gcnew System::Windows::Forms::Button();
			this->button_specularRemove = gcnew System::Windows::Forms::Button();
			this->rangedFloat_specularAngle = gcnew TerawattManagedControls::RangedFloat();
			this->rangedFloat_diffuseAngle = gcnew TerawattManagedControls::RangedFloat();
			this->label3 = gcnew System::Windows::Forms::Label();
			this->label4 = gcnew System::Windows::Forms::Label();
			this->SuspendLayout();
			// 
			// labelEnvironmentName
			// 
			this->labelEnvironmentName->Location = System::Drawing::Point(8, 8);
			this->labelEnvironmentName->Name = "labelEnvironmentName";
			this->labelEnvironmentName->Size = System::Drawing::Size(264, 16);
			this->labelEnvironmentName->TabIndex = 2;
			this->labelEnvironmentName->Text = "EnvironmentName";
			// 
			// button_BrowseTextureDiffuse
			// 
			this->button_BrowseTextureDiffuse->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->button_BrowseTextureDiffuse->Location = System::Drawing::Point(240, 40);
			this->button_BrowseTextureDiffuse->Name = "button_BrowseTextureDiffuse";
			this->button_BrowseTextureDiffuse->Size = System::Drawing::Size(60, 21);
			this->button_BrowseTextureDiffuse->TabIndex = 33;
			this->button_BrowseTextureDiffuse->Text = "Browse...";
			this->button_BrowseTextureDiffuse->Click += gcnew System::EventHandler(this, &envtEnvironmentPrtyForm::button_BrowseTextureDiffuse_Click);
			// 
			// textBox_TextureFileDiffuse
			// 
			this->textBox_TextureFileDiffuse->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textBox_TextureFileDiffuse->Location = System::Drawing::Point(88, 40);
			this->textBox_TextureFileDiffuse->Name = "textBox_TextureFileDiffuse";
			this->textBox_TextureFileDiffuse->ReadOnly = true;
			this->textBox_TextureFileDiffuse->Size = System::Drawing::Size(139, 20);
			this->textBox_TextureFileDiffuse->TabIndex = 32;
			this->textBox_TextureFileDiffuse->Text = "";
			// 
			// button_BrowseTextureSpecular
			// 
			this->button_BrowseTextureSpecular->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->button_BrowseTextureSpecular->Location = System::Drawing::Point(240, 184);
			this->button_BrowseTextureSpecular->Name = "button_BrowseTextureSpecular";
			this->button_BrowseTextureSpecular->Size = System::Drawing::Size(60, 21);
			this->button_BrowseTextureSpecular->TabIndex = 35;
			this->button_BrowseTextureSpecular->Text = "Browse...";
			this->button_BrowseTextureSpecular->Click += gcnew System::EventHandler(this, &envtEnvironmentPrtyForm::button_BrowseTextureSpecular_Click);
			// 
			// textBox_TextureFileSpecular
			// 
			this->textBox_TextureFileSpecular->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textBox_TextureFileSpecular->Location = System::Drawing::Point(88, 184);
			this->textBox_TextureFileSpecular->Name = "textBox_TextureFileSpecular";
			this->textBox_TextureFileSpecular->ReadOnly = true;
			this->textBox_TextureFileSpecular->Size = System::Drawing::Size(139, 20);
			this->textBox_TextureFileSpecular->TabIndex = 34;
			this->textBox_TextureFileSpecular->Text = "";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 40);
			this->label1->Name = "label1";
			this->label1->Size = System::Drawing::Size(40, 16);
			this->label1->TabIndex = 36;
			this->label1->Text = "Diffuse";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 184);
			this->label2->Name = "label2";
			this->label2->Size = System::Drawing::Size(56, 16);
			this->label2->TabIndex = 37;
			this->label2->Text = "Specular";
			// 
			// rangedFloat_diffuseFactor
			// 
			this->rangedFloat_diffuseFactor->Exponent = (System::Int16)1;
			this->rangedFloat_diffuseFactor->Location = System::Drawing::Point(80, 72);
			this->rangedFloat_diffuseFactor->Name = "rangedFloat_diffuseFactor";
			this->rangedFloat_diffuseFactor->NumTicks = (System::Int16)100;
			this->rangedFloat_diffuseFactor->Precision = (System::Int16)2;
			this->rangedFloat_diffuseFactor->Size = System::Drawing::Size(216, 32);
			this->rangedFloat_diffuseFactor->TabIndex = 38;
			this->rangedFloat_diffuseFactor->Value = 1;
			this->rangedFloat_diffuseFactor->ValueChanged += gcnew System::EventHandler(this, &envtEnvironmentPrtyForm::rangedFloat_diffuseFactor_ValueChanged);
			// 
			// rangedFloat_specularFactor
			// 
			this->rangedFloat_specularFactor->Exponent = (System::Int16)1;
			this->rangedFloat_specularFactor->Location = System::Drawing::Point(80, 216);
			this->rangedFloat_specularFactor->Name = "rangedFloat_specularFactor";
			this->rangedFloat_specularFactor->NumTicks = (System::Int16)100;
			this->rangedFloat_specularFactor->Precision = (System::Int16)2;
			this->rangedFloat_specularFactor->Size = System::Drawing::Size(216, 32);
			this->rangedFloat_specularFactor->TabIndex = 39;
			this->rangedFloat_specularFactor->Value = 1;
			this->rangedFloat_specularFactor->ValueChanged += gcnew System::EventHandler(this, &envtEnvironmentPrtyForm::rangedFloat_specularFactor_ValueChanged);
			// 
			// button_diffuseRemove
			// 
			this->button_diffuseRemove->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->button_diffuseRemove->Location = System::Drawing::Point(8, 64);
			this->button_diffuseRemove->Name = "button_diffuseRemove";
			this->button_diffuseRemove->Size = System::Drawing::Size(60, 21);
			this->button_diffuseRemove->TabIndex = 40;
			this->button_diffuseRemove->Text = "Remove";
			this->button_diffuseRemove->Click += gcnew System::EventHandler(this, &envtEnvironmentPrtyForm::button_diffuseRemove_Click);
			// 
			// button_specularRemove
			// 
			this->button_specularRemove->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->button_specularRemove->Location = System::Drawing::Point(8, 208);
			this->button_specularRemove->Name = "button_specularRemove";
			this->button_specularRemove->Size = System::Drawing::Size(60, 21);
			this->button_specularRemove->TabIndex = 41;
			this->button_specularRemove->Text = "Remove";
			this->button_specularRemove->Click += gcnew System::EventHandler(this, &envtEnvironmentPrtyForm::button_specularRemove_Click);
			// 
			// rangedFloat_specularAngle
			// 
			this->rangedFloat_specularAngle->Exponent = (System::Int16)1;
			this->rangedFloat_specularAngle->Location = System::Drawing::Point(80, 256);
			this->rangedFloat_specularAngle->Maximum = 180;
			this->rangedFloat_specularAngle->Minimum = -180;
			this->rangedFloat_specularAngle->Name = "rangedFloat_specularAngle";
			this->rangedFloat_specularAngle->NumTicks = (System::Int16)360;
			this->rangedFloat_specularAngle->Precision = (System::Int16)0;
			this->rangedFloat_specularAngle->Size = System::Drawing::Size(216, 32);
			this->rangedFloat_specularAngle->TabIndex = 42;
			this->rangedFloat_specularAngle->ValueChanged += gcnew System::EventHandler(this, &envtEnvironmentPrtyForm::rangedFloat_specularAngle_ValueChanged);
			// 
			// rangedFloat_diffuseAngle
			// 
			this->rangedFloat_diffuseAngle->Exponent = (System::Int16)1;
			this->rangedFloat_diffuseAngle->Location = System::Drawing::Point(80, 112);
			this->rangedFloat_diffuseAngle->Maximum = 180;
			this->rangedFloat_diffuseAngle->Minimum = -180;
			this->rangedFloat_diffuseAngle->Name = "rangedFloat_diffuseAngle";
			this->rangedFloat_diffuseAngle->NumTicks = (System::Int16)360;
			this->rangedFloat_diffuseAngle->Precision = (System::Int16)0;
			this->rangedFloat_diffuseAngle->Size = System::Drawing::Size(216, 32);
			this->rangedFloat_diffuseAngle->TabIndex = 43;
			this->rangedFloat_diffuseAngle->ValueChanged += gcnew System::EventHandler(this, &envtEnvironmentPrtyForm::rangedFloat_diffuseAngle_ValueChanged);
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(40, 120);
			this->label3->Name = "label3";
			this->label3->Size = System::Drawing::Size(32, 16);
			this->label3->TabIndex = 44;
			this->label3->Text = "angle";
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(40, 264);
			this->label4->Name = "label4";
			this->label4->Size = System::Drawing::Size(32, 16);
			this->label4->TabIndex = 45;
			this->label4->Text = "angle";
			// 
			// envtEnvironmentPrtyForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(312, 397);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->rangedFloat_diffuseAngle);
			this->Controls->Add(this->rangedFloat_specularAngle);
			this->Controls->Add(this->button_specularRemove);
			this->Controls->Add(this->button_diffuseRemove);
			this->Controls->Add(this->rangedFloat_specularFactor);
			this->Controls->Add(this->rangedFloat_diffuseFactor);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button_BrowseTextureSpecular);
			this->Controls->Add(this->textBox_TextureFileSpecular);
			this->Controls->Add(this->button_BrowseTextureDiffuse);
			this->Controls->Add(this->textBox_TextureFileDiffuse);
			this->Controls->Add(this->labelEnvironmentName);
			this->Name = "envtEnvironmentPrtyForm";
			this->Text = "Environment Properties";
			this->ResumeLayout(false);

		}		
		//

	private: System::Void button_BrowseTextureDiffuse_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				envtTextureChooser ^dialog = gcnew envtTextureChooser();
				if (dialog->ShowDialog() == ::DialogResult::OK)
				{
					textBox_TextureFileDiffuse->Text = dialog->GetResultString();
					itString fname;
					tmaManagedStringUtils::ManagedStringToItString(dialog->GetResultString(), fname);
//					envtOperations::ChangeDiffuseMap(m_EnvironmentName, fname);
				}

				delete dialog;
			 }

private: System::Void button_BrowseTextureSpecular_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
				envtTextureChooser ^dialog = gcnew envtTextureChooser();
				if (dialog->ShowDialog() == ::DialogResult::OK)
				{
					textBox_TextureFileSpecular->Text = dialog->GetResultString();
					itString fname;
					tmaManagedStringUtils::ManagedStringToItString(dialog->GetResultString(), fname);
//					envtOperations::ChangeSpecularMap(m_EnvironmentName, fname);
				}

				delete dialog;
		 }

private: System::Void button_specularRemove_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			textBox_TextureFileSpecular->Text = "";
			itString fname;
//			envtOperations::ChangeSpecularMap(m_EnvironmentName, fname);
		 }

private: System::Void button_diffuseRemove_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			textBox_TextureFileDiffuse->Text = "";
			itString fname;
//			envtOperations::ChangeDiffuseMap(m_EnvironmentName, fname);
		 }

private: System::Void rangedFloat_diffuseFactor_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
//				 envtOperations::ChangeDiffuseFactor(m_EnvironmentName, (float)rangedFloat_diffuseFactor->Value);
			 }
		 }

private: System::Void rangedFloat_specularFactor_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
//				 envtOperations::ChangeSpecularFactor(m_EnvironmentName, (float)rangedFloat_specularFactor->Value);
			 }
		 }

private: System::Void rangedFloat_specularAngle_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
//				 envtOperations::ChangeSpecularAngle(m_EnvironmentName, (float)rangedFloat_specularAngle->Value);
			 }
		 }

private: System::Void rangedFloat_diffuseAngle_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
//				 envtOperations::ChangeDiffuseAngle(m_EnvironmentName, (float)rangedFloat_diffuseAngle->Value);
			 }
		 }

};
}
#endif // _MANAGED
