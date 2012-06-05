#pragma once

#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif
#ifndef NAME_MGR_HPP
#include "Core/name/nameMgr.hpp"
#endif
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
#endif
#ifndef LTST_LIGHTSETMGR_HPP
#include "Support/ltst/ltstLightSetMgr.hpp"
#endif

#ifdef _MANAGED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace SystemPointLights
{
	/// <summary> 
	/// Summary for ptltCreateFillLight
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ptltCreateFillLight : public System::Windows::Forms::Form
	{
	public: 
		ptltCreateFillLight(void)
		{
			InitializeComponent();

			SetupComponents();
		}
        
		void GetLightName(std::string &o_Name)
		{
			tmaManagedStringUtils::ManagedStringToStdString( this->textBox_Name->Text, o_Name );
		}

		void GetObjectName(nameString &o_Name)
		{
			std::string name;
			tmaManagedStringUtils::ManagedStringToStdString( this->comboBox_attachobject->Text, name );

			// search for object using string only by using InvalidUID
			nameString name_str( name, nameString::e_InvalidUID );
			//DBG_LOG2( "combobox attach object UID-%02d (%s)", name.GetUID(), name.GetString().c_str() );

			// fetch the object associated with this name
			nameObject* pNO = nameMgr::GetObjectByName( name_str );
			if (pNO)
				o_Name = pNO->GetName();
			else
				o_Name = name_str;
		}
	
		nameObject* GetNameObject()
		{
			std::string name;
			tmaManagedStringUtils::ManagedStringToStdString( this->comboBox_attachobject->Text, name );

			// search for object using string only by using InvalidUID
			nameString name_str( name, nameString::e_InvalidUID );
			//DBG_LOG2( "combobox attach object UID-%02d (%s)", name.GetUID(), name.GetString().c_str() );

			// fetch the object associated with this name
			nameObject* pNO = nameMgr::GetObjectByName( name_str );
			return pNO;
		}

		void GetLightSet(std::string& o_LightSet)
		{
			tmaManagedStringUtils::ManagedStringToStdString( this->comboBox_lightSet->Text, o_LightSet );
		}

		float GetRadius()
		{
			return (float) this->floatEdit_Radius->Value;
		}

		void GetColor(maFloatRGBA& o_Color)
		{
			o_Color = tmaManagedConversionUtil::ConvertColorRGB( this->colorRGBEdit_light->Color );
		}

		bool GetDiffuseEnabled()
		{
			return this->checkBox_diffuse->Checked;
		}

		bool GetSpecularEnabled()
		{
			return this->checkBox_specular->Checked;
		}
		
		void GetFalloff(maVector3d& o_Falloff)
		{
			o_Falloff.Set( (float) this->vector3EditUpDown_falloff->ValueX,
						   (float) this->vector3EditUpDown_falloff->ValueY,
						   (float) this->vector3EditUpDown_falloff->ValueZ );
		}

	public: 
		~ptltCreateFillLight()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox ^  textBox_Name;
	private: System::Windows::Forms::Label ^  label2;
	private: System::Windows::Forms::ComboBox ^  comboBox_lightSet;
	private: System::Windows::Forms::Label ^  label1;
	private: System::Windows::Forms::ComboBox ^  comboBox_attachobject;
	private: System::Windows::Forms::Label ^  label_attachobject;
	private: System::Windows::Forms::Label ^  labelNameFront;
	private: System::Windows::Forms::Label ^  labelNameBack;
	private: System::Windows::Forms::Label ^  labelNameLeft;
	private: System::Windows::Forms::Label ^  labelNameRight;
	private: System::Windows::Forms::Label ^  label3;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_Radius;
	private: System::Windows::Forms::Button ^  button2;
	private: System::Windows::Forms::Button ^  buttonOk;
	private: System::Windows::Forms::Label ^  label_color;
	private: TerawattManagedControls::ColorRGBEdit ^  colorRGBEdit_light;
	private: System::Windows::Forms::CheckBox ^  checkBox_diffuse;
	private: System::Windows::Forms::CheckBox ^  checkBox_specular;
	private: System::Windows::Forms::Label ^  label_falloff;
	private: TerawattManagedControls::Vector3EditUpDown ^  vector3EditUpDown_falloff;

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
			this->textBox_Name = gcnew System::Windows::Forms::TextBox();
			this->label2 = gcnew System::Windows::Forms::Label();
			this->comboBox_lightSet = gcnew System::Windows::Forms::ComboBox();
			this->label1 = gcnew System::Windows::Forms::Label();
			this->comboBox_attachobject = gcnew System::Windows::Forms::ComboBox();
			this->label_attachobject = gcnew System::Windows::Forms::Label();
			this->labelNameFront = gcnew System::Windows::Forms::Label();
			this->labelNameBack = gcnew System::Windows::Forms::Label();
			this->labelNameLeft = gcnew System::Windows::Forms::Label();
			this->labelNameRight = gcnew System::Windows::Forms::Label();
			this->label3 = gcnew System::Windows::Forms::Label();
			this->floatEdit_Radius = gcnew TerawattManagedControls::FloatEdit();
			this->button2 = gcnew System::Windows::Forms::Button();
			this->buttonOk = gcnew System::Windows::Forms::Button();
			this->label_color = gcnew System::Windows::Forms::Label();
			this->colorRGBEdit_light = gcnew TerawattManagedControls::ColorRGBEdit();
			this->checkBox_diffuse = gcnew System::Windows::Forms::CheckBox();
			this->checkBox_specular = gcnew System::Windows::Forms::CheckBox();
			this->label_falloff = gcnew System::Windows::Forms::Label();
			this->vector3EditUpDown_falloff = gcnew TerawattManagedControls::Vector3EditUpDown();
			this->SuspendLayout();
			// 
			// textBox_Name
			// 
			this->textBox_Name->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textBox_Name->Location = System::Drawing::Point(144, 16);
			this->textBox_Name->Name = "textBox_Name";
			this->textBox_Name->Size = System::Drawing::Size(208, 20);
			this->textBox_Name->TabIndex = 20;
			this->textBox_Name->Text = "";
			this->textBox_Name->TextChanged += gcnew System::EventHandler(this, &ptltCreateFillLight::textBox_Name_TextChanged);
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(16, 16);
			this->label2->Name = "label2";
			this->label2->Size = System::Drawing::Size(112, 28);
			this->label2->TabIndex = 25;
			this->label2->Text = "Name for new lights:";
			// 
			// comboBox_lightSet
			// 
			this->comboBox_lightSet->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->comboBox_lightSet->Location = System::Drawing::Point(96, 144);
			this->comboBox_lightSet->Name = "comboBox_lightSet";
			this->comboBox_lightSet->Size = System::Drawing::Size(256, 21);
			this->comboBox_lightSet->TabIndex = 22;
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(16, 144);
			this->label1->Name = "label1";
			this->label1->Size = System::Drawing::Size(80, 28);
			this->label1->TabIndex = 24;
			this->label1->Text = "Light Set:";
			// 
			// comboBox_attachobject
			// 
			this->comboBox_attachobject->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->comboBox_attachobject->Location = System::Drawing::Point(96, 112);
			this->comboBox_attachobject->Name = "comboBox_attachobject";
			this->comboBox_attachobject->Size = System::Drawing::Size(256, 21);
			this->comboBox_attachobject->TabIndex = 21;
			// 
			// label_attachobject
			// 
			this->label_attachobject->Location = System::Drawing::Point(16, 112);
			this->label_attachobject->Name = "label_attachobject";
			this->label_attachobject->Size = System::Drawing::Size(80, 28);
			this->label_attachobject->TabIndex = 23;
			this->label_attachobject->Text = "Attach Object:";
			// 
			// labelNameFront
			// 
			this->labelNameFront->Location = System::Drawing::Point(48, 48);
			this->labelNameFront->Name = "labelNameFront";
			this->labelNameFront->Size = System::Drawing::Size(128, 16);
			this->labelNameFront->TabIndex = 26;
			this->labelNameFront->Text = "FILL__FRONT";
			// 
			// labelNameBack
			// 
			this->labelNameBack->Location = System::Drawing::Point(48, 72);
			this->labelNameBack->Name = "labelNameBack";
			this->labelNameBack->Size = System::Drawing::Size(128, 16);
			this->labelNameBack->TabIndex = 27;
			this->labelNameBack->Text = "FILL__BACK";
			// 
			// labelNameLeft
			// 
			this->labelNameLeft->Location = System::Drawing::Point(200, 72);
			this->labelNameLeft->Name = "labelNameLeft";
			this->labelNameLeft->Size = System::Drawing::Size(128, 16);
			this->labelNameLeft->TabIndex = 29;
			this->labelNameLeft->Text = "FILL__LEFT";
			// 
			// labelNameRight
			// 
			this->labelNameRight->Location = System::Drawing::Point(200, 48);
			this->labelNameRight->Name = "labelNameRight";
			this->labelNameRight->Size = System::Drawing::Size(128, 16);
			this->labelNameRight->TabIndex = 28;
			this->labelNameRight->Text = "FILL__RIGHT";
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(16, 184);
			this->label3->Name = "label3";
			this->label3->Size = System::Drawing::Size(112, 24);
			this->label3->TabIndex = 31;
			this->label3->Text = "Radius of ring:";
			// 
			// floatEdit_Radius
			// 
			this->floatEdit_Radius->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->floatEdit_Radius->Location = System::Drawing::Point(144, 184);
			this->floatEdit_Radius->Name = "floatEdit_Radius";
			this->floatEdit_Radius->Precision = (System::Int16)2;
			this->floatEdit_Radius->Size = System::Drawing::Size(208, 24);
			this->floatEdit_Radius->TabIndex = 32;
			// 
			// button2
			// 
			this->button2->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button2->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button2->Location = System::Drawing::Point(192, 336);
			this->button2->Name = "button2";
			this->button2->Size = System::Drawing::Size(72, 24);
			this->button2->TabIndex = 34;
			this->button2->Text = "Cancel";
			// 
			// buttonOk
			// 
			this->buttonOk->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonOk->Location = System::Drawing::Point(80, 336);
			this->buttonOk->Name = "buttonOk";
			this->buttonOk->Size = System::Drawing::Size(88, 24);
			this->buttonOk->TabIndex = 33;
			this->buttonOk->Text = "Create Lights";
			this->buttonOk->Click += gcnew System::EventHandler(this, &ptltCreateFillLight::buttonOk_Click);
			// 
			// label_color
			// 
			this->label_color->Location = System::Drawing::Point(32, 224);
			this->label_color->Name = "label_color";
			this->label_color->Size = System::Drawing::Size(73, 20);
			this->label_color->TabIndex = 35;
			this->label_color->Text = "Color:";
			// 
			// colorRGBEdit_light
			// 
			this->colorRGBEdit_light->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->colorRGBEdit_light->Color = System::Drawing::Color::FromArgb((System::Byte)255, (System::Byte)255, (System::Byte)255);
			this->colorRGBEdit_light->Location = System::Drawing::Point(112, 224);
			this->colorRGBEdit_light->Name = "colorRGBEdit_light";
			this->colorRGBEdit_light->Size = System::Drawing::Size(237, 24);
			this->colorRGBEdit_light->TabIndex = 36;
			// 
			// checkBox_diffuse
			// 
			this->checkBox_diffuse->Checked = true;
			this->checkBox_diffuse->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_diffuse->Location = System::Drawing::Point(80, 264);
			this->checkBox_diffuse->Name = "checkBox_diffuse";
			this->checkBox_diffuse->Size = System::Drawing::Size(96, 21);
			this->checkBox_diffuse->TabIndex = 37;
			this->checkBox_diffuse->Text = "Diffuse Light";
			// 
			// checkBox_specular
			// 
			this->checkBox_specular->Location = System::Drawing::Point(200, 264);
			this->checkBox_specular->Name = "checkBox_specular";
			this->checkBox_specular->Size = System::Drawing::Size(127, 21);
			this->checkBox_specular->TabIndex = 38;
			this->checkBox_specular->Text = "Specular Light";
			// 
			// label_falloff
			// 
			this->label_falloff->Location = System::Drawing::Point(24, 296);
			this->label_falloff->Name = "label_falloff";
			this->label_falloff->Size = System::Drawing::Size(83, 21);
			this->label_falloff->TabIndex = 39;
			this->label_falloff->Text = "Falloff:";
			// 
			// vector3EditUpDown_falloff
			// 
			this->vector3EditUpDown_falloff->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->vector3EditUpDown_falloff->Location = System::Drawing::Point(109, 296);
			this->vector3EditUpDown_falloff->Name = "vector3EditUpDown_falloff";
			this->vector3EditUpDown_falloff->Precision = (System::Int16)5;
			this->vector3EditUpDown_falloff->Size = System::Drawing::Size(243, 21);
			this->vector3EditUpDown_falloff->TabIndex = 40;
			
			// 
			// ptltCreateFillLight
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(368, 374);
			this->Controls->Add(this->label_falloff);
			this->Controls->Add(this->vector3EditUpDown_falloff);
			this->Controls->Add(this->checkBox_diffuse);
			this->Controls->Add(this->checkBox_specular);
			this->Controls->Add(this->label_color);
			this->Controls->Add(this->colorRGBEdit_light);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->buttonOk);
			this->Controls->Add(this->floatEdit_Radius);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->labelNameLeft);
			this->Controls->Add(this->labelNameRight);
			this->Controls->Add(this->labelNameBack);
			this->Controls->Add(this->labelNameFront);
			this->Controls->Add(this->textBox_Name);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->comboBox_lightSet);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->comboBox_attachobject);
			this->Controls->Add(this->label_attachobject);
			this->Name = "ptltCreateFillLight";
			this->Text = "Create Fill Ring";
			this->ResumeLayout(false);

		}		
		//

private:
		//	Set-up the components on construction
		//
		void SetupComponents()
		{
			//	fill-in character name combobox
			//
			nameList aNameList;
			nameMgr::GetNameList( aNameList );
			const int num_names = aNameList.size();
			for ( int i = 0 ; i < num_names; i++ )
			{
				//DBG_LOG2( "%02d) %s", i, aNameList[i]->GetString().c_str() );
		
				comboBox_attachobject->Items->Add( gcnew String(aNameList[i]->GetString().c_str()) );
			}
			
			// Fill in Light sets combo box
			std::vector<nameString> set_names;
			ltstLightSetMgr::GetLightSetNames(set_names);
			const int num_set_names = set_names.size();
			for (int i=0; i<num_set_names; ++i)
			{
				nameString &name = set_names[i];
				comboBox_lightSet->Items->Add( gcnew String(name.GetString().c_str()) );
			}
		}

	private: System::Void textBox_Name_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 String ^stem = this->textBox_Name->Text;
				 this->labelNameFront->Text	= String::Format("FILL_{0}_FRONT", stem);
				 this->labelNameBack->Text	= String::Format("FILL_{0}__BACK", stem);
				 this->labelNameLeft->Text	= String::Format("FILL_{0}__LEFT", stem);
				 this->labelNameRight->Text	= String::Format("FILL_{0}__RIGHT", stem);
			 }

	private: System::Void buttonOk_Click(System::Object ^  sender, System::EventArgs ^  e)
			{
				if (this->textBox_Name->Text->Length == 0)
				{
					MessageBox::Show("Choose a string to use with the FILL names.");
				}
				else if (this->comboBox_attachobject->Text->Length == 0)
				{
					MessageBox::Show("Choose an object to which to attach the fill lights");
				}
				else if (this->floatEdit_Radius->Value <= 0)
				{
					MessageBox::Show("Choose a radius for spacing the lights in a ring");
				}
				else
				{
					this->DialogResult = ::DialogResult::OK;
					this->Close();
				}
			}

};
}
#endif // _MANAGED
