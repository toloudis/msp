/********************************************************************************************\
**  dynControlDataForm.h
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#pragma once

#ifndef DYN_CONTROLDATA_HPP
#include "Support/dyn/dynControlData.hpp"
#endif
#ifndef DYN_SCRIPTOBJECT_HPP
#include "Support/dyn/dynScriptObject.hpp"
#endif
#ifndef DYN_REFNAMEFORM_HPP
#include "Support/dyn/GUI/dynRefNameForm.h"
#endif

#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
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
	/// Summary for dynControlDataForm
	///
	/// WARNING: If you change the name of this class, yada yada...
	/// </summary>
	public ref class dynControlDataForm : public System::Windows::Forms::Form
	{
	public:
		dynControlDataForm(dynControlData &i_Data, dynScriptObject& i_Object)
			: m_Data(i_Data), m_Object(i_Object)
		{
			InitializeComponent();
			SetupData(i_Data);
		}

	public:
		~dynControlDataForm()
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

		// Holds reference to data, changing the data
		// within the caller's structure
		dynControlData &m_Data;
		dynScriptObject &m_Object;
	private: System::Windows::Forms::GroupBox ^  groupBox1;
	private: System::Windows::Forms::Label ^  label1;

	private: System::Windows::Forms::Label ^  label2;

	private: System::Windows::Forms::Label ^  label3;

	private: System::Windows::Forms::Label ^  label4;

	private: System::Windows::Forms::Label ^  label5;
	private: System::Windows::Forms::Button ^  buttonOK;
	private: System::Windows::Forms::ComboBox ^  comboBox1;
	private: System::Windows::Forms::Button ^  buttonAttach;
	private: System::Windows::Forms::TextBox ^  textName;
	private: System::Windows::Forms::TextBox ^  textNode;
	private: System::Windows::Forms::TextBox ^  textMin;
	private: System::Windows::Forms::TextBox ^  textMax;
	private: System::Windows::Forms::Label ^  label6;
	private: System::Windows::Forms::Button ^  buttonCancel;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->groupBox1 = gcnew System::Windows::Forms::GroupBox();
			this->buttonAttach = gcnew System::Windows::Forms::Button();
			this->comboBox1 = gcnew System::Windows::Forms::ComboBox();
			this->label1 = gcnew System::Windows::Forms::Label();
			this->textName = gcnew System::Windows::Forms::TextBox();
			this->label2 = gcnew System::Windows::Forms::Label();
			this->textNode = gcnew System::Windows::Forms::TextBox();
			this->label3 = gcnew System::Windows::Forms::Label();
			this->textMin = gcnew System::Windows::Forms::TextBox();
			this->label4 = gcnew System::Windows::Forms::Label();
			this->textMax = gcnew System::Windows::Forms::TextBox();
			this->label5 = gcnew System::Windows::Forms::Label();
			this->buttonOK = gcnew System::Windows::Forms::Button();
			this->buttonCancel = gcnew System::Windows::Forms::Button();
			this->label6 = gcnew System::Windows::Forms::Label();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupBox1->Controls->Add(this->label6);
			this->groupBox1->Controls->Add(this->buttonAttach);
			this->groupBox1->Controls->Add(this->comboBox1);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->textName);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->textNode);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->textMin);
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Controls->Add(this->textMax);
			this->groupBox1->Controls->Add(this->label5);
			this->groupBox1->Location = System::Drawing::Point(7, 7);
			this->groupBox1->Name = "groupBox1";
			this->groupBox1->Size = System::Drawing::Size(266, 270);
			this->groupBox1->TabIndex = 2;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = "Data";
			// 
			// buttonAttach
			// 
			this->buttonAttach->Location = System::Drawing::Point(153, 97);
			this->buttonAttach->Name = "buttonAttach";
			this->buttonAttach->Size = System::Drawing::Size(87, 21);
			this->buttonAttach->TabIndex = 10;
			this->buttonAttach->Text = "Look Up...";
			this->buttonAttach->Click += gcnew System::EventHandler(this, &dynControlDataForm::buttonAttach_Click);
			// 
			// comboBox1
			// 
			this->comboBox1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(3)
				{ "X", "Y", "Z"} );
			this->comboBox1->Location = System::Drawing::Point(100, 168);
			this->comboBox1->Name = "comboBox1";
			this->comboBox1->Size = System::Drawing::Size(140, 21);
			this->comboBox1->TabIndex = 9;
			this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &dynControlDataForm::comboBox1_SelectedIndexChanged);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(20, 35);
			this->label1->Name = "label1";
			this->label1->Size = System::Drawing::Size(73, 20);
			this->label1->TabIndex = 0;
			this->label1->Text = "Name:";
			// 
			// textName
			// 
			this->textName->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textName->Location = System::Drawing::Point(100, 35);
			this->textName->Name = "textName";
			this->textName->Size = System::Drawing::Size(140, 20);
			this->textName->TabIndex = 1;
			this->textName->Text = "";
			this->textName->TextChanged += gcnew System::EventHandler(this, &dynControlDataForm::textName_TextChanged);
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(20, 69);
			this->label2->Name = "label2";
			this->label2->Size = System::Drawing::Size(73, 21);
			this->label2->TabIndex = 2;
			this->label2->Text = "Attach Node:";
			// 
			// textNode
			// 
			this->textNode->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textNode->Location = System::Drawing::Point(100, 69);
			this->textNode->Name = "textNode";
			this->textNode->ReadOnly = true;
			this->textNode->Size = System::Drawing::Size(140, 20);
			this->textNode->TabIndex = 3;
			this->textNode->Text = "";
			this->textNode->TextChanged += gcnew System::EventHandler(this, &dynControlDataForm::textNode_TextChanged);
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(20, 201);
			this->label3->Name = "label3";
			this->label3->Size = System::Drawing::Size(73, 21);
			this->label3->TabIndex = 4;
			this->label3->Text = "MinAngle:";
			// 
			// textMin
			// 
			this->textMin->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textMin->Location = System::Drawing::Point(100, 201);
			this->textMin->Name = "textMin";
			this->textMin->Size = System::Drawing::Size(140, 20);
			this->textMin->TabIndex = 5;
			this->textMin->Text = "";
			this->textMin->TextChanged += gcnew System::EventHandler(this, &dynControlDataForm::textMin_TextChanged);
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(20, 236);
			this->label4->Name = "label4";
			this->label4->Size = System::Drawing::Size(73, 21);
			this->label4->TabIndex = 6;
			this->label4->Text = "MaxAngle:";
			// 
			// textMax
			// 
			this->textMax->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textMax->Location = System::Drawing::Point(100, 236);
			this->textMax->Name = "textMax";
			this->textMax->Size = System::Drawing::Size(140, 20);
			this->textMax->TabIndex = 7;
			this->textMax->Text = "";
			this->textMax->TextChanged += gcnew System::EventHandler(this, &dynControlDataForm::textMax_TextChanged);
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(20, 168);
			this->label5->Name = "label5";
			this->label5->Size = System::Drawing::Size(73, 20);
			this->label5->TabIndex = 8;
			this->label5->Text = "Axis:";
			// 
			// buttonOK
			// 
			this->buttonOK->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonOK->Location = System::Drawing::Point(40, 291);
			this->buttonOK->Name = "buttonOK";
			this->buttonOK->Size = System::Drawing::Size(73, 28);
			this->buttonOK->TabIndex = 1;
			this->buttonOK->Text = "OK";
			this->buttonOK->Click += gcnew System::EventHandler(this, &dynControlDataForm::buttonOK_Click);
			// 
			// buttonCancel
			// 
			this->buttonCancel->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonCancel->Location = System::Drawing::Point(133, 291);
			this->buttonCancel->Name = "buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(80, 28);
			this->buttonCancel->TabIndex = 0;
			this->buttonCancel->Text = "Cancel";
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(16, 128);
			this->label6->Name = "label6";
			this->label6->Size = System::Drawing::Size(232, 32);
			this->label6->TabIndex = 11;
			this->label6->Text = "The following controls are for single-axis rotation only";
			// 
			// dynControlDataForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(286, 331);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->buttonOK);
			this->Controls->Add(this->groupBox1);
			this->Name = "dynControlDataForm";
			this->Text = "Control Data Form";
			this->groupBox1->ResumeLayout(false);
			this->ResumeLayout(false);

		}

		//

		void SetupData(const dynControlData &i_Data)
		{
			textName->Text = gcnew System::String(i_Data.m_Name.c_str());
			textNode->Text = gcnew System::String(i_Data.m_Node.c_str());

			if (i_Data.m_ControlType == dynControlData::e_RotateControl)
			{
				textMin->Enabled = true;
				System::Decimal min_angle = (System::Decimal)i_Data.m_MinAngle;
				textMin->Text = min_angle.ToString("F2");
				textMax->Enabled = true;
				System::Decimal max_angle = (System::Decimal)i_Data.m_MaxAngle;
				textMax->Text = max_angle.ToString("F2");
				comboBox1->Enabled = true;
				comboBox1->SelectedIndex = i_Data.m_Axis;
			}
			else
			{
				textMin->Enabled = false;
				textMax->Enabled = false;
				comboBox1->Enabled = false;
			}
		}

		System::Void textName_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			tmaManagedConversionUtil::ConvertString(textName->Text, m_Data.m_Name);
		}

		System::Void textNode_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			tmaManagedConversionUtil::ConvertString(textNode->Text, m_Data.m_Node);
		}

		System::Void textMin_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			if (this->textMin->Text->Length > 0)
			{
				try {
					m_Data.m_MinAngle = (float)System::Double::Parse(textMin->Text);
				} catch (FormatException ^) {}
			}
		}

		System::Void textMax_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			if (this->textMax->Text->Length > 0)
			{
				try {
					m_Data.m_MaxAngle = (float)System::Double::Parse(textMax->Text);
				} catch (FormatException ^) {}
			}
		}

		System::Void buttonAttach_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			std::vector<std::string> ref_names;
			m_Object.GetReferenceList(ref_names);

			dynRefNameForm ^dialog = gcnew dynRefNameForm(ref_names);
			if (dialog->ShowDialog() == ::DialogResult::OK)
			{
				this->textNode->Text = gcnew System::String(ref_names[dialog->GetSelected()].c_str());
			}

			delete dialog;
		 }

		System::Void comboBox1_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 m_Data.m_Axis = dynControlData::Axis(comboBox1->SelectedIndex);
		 }

		System::Void buttonOK_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (this->textNode->Text->Length == 0)
				 MessageBox::Show("Need to assign a node for attaching the controller.");
			 else if (this->textName->Text->Length == 0)
				 MessageBox::Show("Need to assign a name for the controller.");
			 else if (m_Data.m_MinAngle > m_Data.m_MaxAngle)
				 MessageBox::Show("Choose valid max and min angles");
			 else
			 {
				 this->DialogResult = ::DialogResult::OK;
				 this->Close();
			 }
		 }

};
}

#endif // _MANAGED
