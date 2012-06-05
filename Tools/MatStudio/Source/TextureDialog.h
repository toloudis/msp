#pragma once

#ifndef MTR_TEXTURELAYER_HPP
#include "mtrTextureLayer.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
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
	/// Summary for TextureDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class TextureDialog : public System::Windows::Forms::Form
	{
	public: 
		TextureDialog(mtrTextureLayer &i_Layer)
			: m_Layer(i_Layer)
		{
			InitializeComponent();

			this->fileTextureName->Fullpath = new String(m_Layer.GetTextureName().c_str());
			this->comboBlendType->SelectedIndex = int(m_Layer.GetTextureType());
		}
        
	protected: 
		void Dispose(Boolean disposing)
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private: mtrTextureLayer &m_Layer;
	private: TerawattManagedControls::FileChooser *  fileTextureName;
	private: System::Windows::Forms::ComboBox *  comboBlendType;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::Button *  buttonOK;
	private: System::Windows::Forms::Button *  buttonCancel;

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
			this->fileTextureName = new TerawattManagedControls::FileChooser();
			this->comboBlendType = new System::Windows::Forms::ComboBox();
			this->label1 = new System::Windows::Forms::Label();
			this->label2 = new System::Windows::Forms::Label();
			this->buttonOK = new System::Windows::Forms::Button();
			this->buttonCancel = new System::Windows::Forms::Button();
			this->SuspendLayout();
			// 
			// fileTextureName
			// 
			this->fileTextureName->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->fileTextureName->Location = System::Drawing::Point(73, 7);
			this->fileTextureName->Name = S"fileTextureName";
			this->fileTextureName->Size = System::Drawing::Size(214, 28);
			this->fileTextureName->TabIndex = 0;
			// 
			// comboBlendType
			// 
			this->comboBlendType->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->comboBlendType->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			System::Object* __mcTemp__1[] = new System::Object*[6];
			__mcTemp__1[0] = S"Normal Blend";
			__mcTemp__1[1] = S"Alpha Blend";
			__mcTemp__1[2] = S"Bump Map";
			__mcTemp__1[3] = S"Specular Map";
			__mcTemp__1[4] = S"Environment Map";
			__mcTemp__1[5] = S"Gloss Map";
			this->comboBlendType->Items->AddRange(__mcTemp__1);
			this->comboBlendType->Location = System::Drawing::Point(73, 35);
			this->comboBlendType->Name = S"comboBlendType";
			this->comboBlendType->Size = System::Drawing::Size(154, 21);
			this->comboBlendType->TabIndex = 1;
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(7, 7);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(60, 21);
			this->label1->TabIndex = 2;
			this->label1->Text = S"Filename:";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(7, 42);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(60, 13);
			this->label2->TabIndex = 3;
			this->label2->Text = S"Layer Type:";
			// 
			// buttonOK
			// 
			this->buttonOK->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonOK->Location = System::Drawing::Point(40, 76);
			this->buttonOK->Name = S"buttonOK";
			this->buttonOK->Size = System::Drawing::Size(67, 28);
			this->buttonOK->TabIndex = 4;
			this->buttonOK->Text = S"OK";
			this->buttonOK->Click += new System::EventHandler(this, buttonOK_Click);
			// 
			// buttonCancel
			// 
			this->buttonCancel->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonCancel->Location = System::Drawing::Point(140, 76);
			this->buttonCancel->Name = S"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(87, 28);
			this->buttonCancel->TabIndex = 5;
			this->buttonCancel->Text = S"Cancel";
			// 
			// TextureDialog
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(299, 117);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->buttonOK);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->comboBlendType);
			this->Controls->Add(this->fileTextureName);
			this->Name = S"TextureDialog";
			this->Text = S"TextureDialog";
			this->ResumeLayout(false);

		}		
		//

	private: System::Void buttonOK_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				 if (this->fileTextureName->Filename->Length > 0)
				 {
						std::string tex_name;
						tmaManagedStringUtils::ManagedStringToStdString(fileTextureName->Filename, tex_name);
						m_Layer.SetTextureName(tex_name);
						m_Layer.SetTextureType(mtrTextureLayer::TextureType(comboBlendType->SelectedIndex));

						this->DialogResult = DialogResult::OK;
						this->Close();
				 }
				 else
				 {
						MessageBox::Show("Please assign a texture filename.");
				 }
			 }

};
}