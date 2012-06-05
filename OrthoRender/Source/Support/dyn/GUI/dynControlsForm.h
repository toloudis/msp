/********************************************************************************************\
**  dynControlsForm.h
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#pragma once

#ifndef DYN_SCRIPTOBJECT_HPP
#include "Support/dyn/dynScriptObject.hpp"
#endif
#ifndef DYN_CONTROLDATAFORM_H
#include "Support/dyn/GUI/dynControlDataForm.h"
#endif
#ifndef CHNL_DIALOGUTIL_HPP
#include "Features/Channels/chnlDialogUtil.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifndef MA_CONSTANTS_HPP
#include "Core/ma/maConstants.hpp"
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
	/// Summary for dynControlsForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class dynControlsForm : public System::Windows::Forms::Form
	{
	public: 
		static dynControlsForm^ FormInstance = nullptr;

		dynControlsForm() : m_pObject(NULL)
		{
			m_bOurChange = false;
			m_bDisableNotify = true;
			InitializeComponent();
			SetupData(m_pObject);
			m_bDisableNotify = false;
		}

		// Call this to update form data
		void Update(dynScriptObject *i_pObject)
		{
			m_bDisableNotify = true;
			SetupData(i_pObject);
			m_pObject = i_pObject;
			m_bDisableNotify = false;

			// Set selected control index based on what was selected last
			int sel_index = i_pObject->GetLastSelectedControlIndex();
			if (sel_index < i_pObject->GetNumControls())
			{
				this->objectList1->SelectedIndex = sel_index;
			}
		}

		// Notify that control with given index has changed.
		//	Update sliders for selected control if the index matches.
		void UpdateControlData(dynScriptObject *i_pObject)
		{
			if (i_pObject == m_pObject)
			{
				int sel_index = this->objectList1->SelectedIndex;
				if (sel_index >= 0)
				{
					const dynControlData& data = m_pObject->GetControlData(sel_index);
					update_control_data(data);
				}
			}
		}


        
	protected: 
		~dynControlsForm()
		{
			// clear instance
			if (dynControlsForm::FormInstance == this)
				dynControlsForm::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}

	private: dynScriptObject* m_pObject;
		// Turns off notify callbacks when setting up form
	private: bool m_bDisableNotify;
		// Turns off setting of data fields when we make the change
	private: bool m_bOurChange;

	private: System::Windows::Forms::TabControl ^  tabControl1;
	private: System::Windows::Forms::TabPage ^  tabPage1;
	private: TerawattManagedControls::ObjectList ^  objectList1;
	private: System::Windows::Forms::Label ^  label1;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat1;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat2;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat3;
	private: System::Windows::Forms::Label ^  label2;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_Translate;
	private: TerawattManagedControls::Vector3Edit^  vector3Edit_Scale;
	private: System::Windows::Forms::Label^  label3;

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
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->vector3Edit_Translate = (gcnew TerawattManagedControls::Vector3Edit());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->rangedFloat3 = (gcnew TerawattManagedControls::RangedFloat());
			this->rangedFloat2 = (gcnew TerawattManagedControls::RangedFloat());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->rangedFloat1 = (gcnew TerawattManagedControls::RangedFloat());
			this->objectList1 = (gcnew TerawattManagedControls::ObjectList());
			this->vector3Edit_Scale = (gcnew TerawattManagedControls::Vector3Edit());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl1->Location = System::Drawing::Point(0, 0);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(312, 394);
			this->tabControl1->TabIndex = 0;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->vector3Edit_Scale);
			this->tabPage1->Controls->Add(this->label3);
			this->tabPage1->Controls->Add(this->vector3Edit_Translate);
			this->tabPage1->Controls->Add(this->label2);
			this->tabPage1->Controls->Add(this->rangedFloat3);
			this->tabPage1->Controls->Add(this->rangedFloat2);
			this->tabPage1->Controls->Add(this->label1);
			this->tabPage1->Controls->Add(this->rangedFloat1);
			this->tabPage1->Controls->Add(this->objectList1);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Size = System::Drawing::Size(304, 368);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Controls";
			// 
			// vector3Edit_Translate
			// 
			this->vector3Edit_Translate->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->vector3Edit_Translate->Location = System::Drawing::Point(7, 294);
			this->vector3Edit_Translate->Name = L"vector3Edit_Translate";
			this->vector3Edit_Translate->Precision = static_cast<System::Int16>(2);
			this->vector3Edit_Translate->Size = System::Drawing::Size(272, 32);
			this->vector3Edit_Translate->TabIndex = 6;
			this->vector3Edit_Translate->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &dynControlsForm::vector3Edit_Translate_KeyPressChild);
			this->vector3Edit_Translate->LeaveChild += gcnew System::EventHandler(this, &dynControlsForm::vector3Edit_Translate_LeaveChild);
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label2->Location = System::Drawing::Point(7, 274);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(160, 16);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Translation:";
			// 
			// rangedFloat3
			// 
			this->rangedFloat3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->rangedFloat3->Exponent = static_cast<System::Int16>(1);
			this->rangedFloat3->Location = System::Drawing::Point(8, 239);
			this->rangedFloat3->Maximum = 180;
			this->rangedFloat3->Minimum = 180;
			this->rangedFloat3->Name = L"rangedFloat3";
			this->rangedFloat3->NumTicks = static_cast<System::Int16>(200);
			this->rangedFloat3->Precision = static_cast<System::Int16>(2);
			this->rangedFloat3->ShowValue = true;
			this->rangedFloat3->Size = System::Drawing::Size(272, 28);
			this->rangedFloat3->TabIndex = 4;
			this->rangedFloat3->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &dynControlsForm::rangedFloat3_KeyPressChild);
			this->rangedFloat3->LeaveChild += gcnew System::EventHandler(this, &dynControlsForm::rangedFloat3_LeaveChild);
			// 
			// rangedFloat2
			// 
			this->rangedFloat2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->rangedFloat2->Exponent = static_cast<System::Int16>(1);
			this->rangedFloat2->Location = System::Drawing::Point(8, 207);
			this->rangedFloat2->Maximum = 180;
			this->rangedFloat2->Minimum = 180;
			this->rangedFloat2->Name = L"rangedFloat2";
			this->rangedFloat2->NumTicks = static_cast<System::Int16>(200);
			this->rangedFloat2->Precision = static_cast<System::Int16>(2);
			this->rangedFloat2->ShowValue = true;
			this->rangedFloat2->Size = System::Drawing::Size(272, 28);
			this->rangedFloat2->TabIndex = 3;
			this->rangedFloat2->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &dynControlsForm::rangedFloat2_KeyPressChild);
			this->rangedFloat2->LeaveChild += gcnew System::EventHandler(this, &dynControlsForm::rangedFloat2_LeaveChild);
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label1->Location = System::Drawing::Point(8, 159);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(160, 16);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Rotation Axes:";
			// 
			// rangedFloat1
			// 
			this->rangedFloat1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->rangedFloat1->Exponent = static_cast<System::Int16>(1);
			this->rangedFloat1->Location = System::Drawing::Point(8, 175);
			this->rangedFloat1->Maximum = 180;
			this->rangedFloat1->Minimum = 180;
			this->rangedFloat1->Name = L"rangedFloat1";
			this->rangedFloat1->NumTicks = static_cast<System::Int16>(200);
			this->rangedFloat1->Precision = static_cast<System::Int16>(2);
			this->rangedFloat1->ShowValue = true;
			this->rangedFloat1->Size = System::Drawing::Size(272, 28);
			this->rangedFloat1->TabIndex = 1;
			this->rangedFloat1->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &dynControlsForm::rangedFloat1_KeyPressChild);
			this->rangedFloat1->LeaveChild += gcnew System::EventHandler(this, &dynControlsForm::rangedFloat1_LeaveChild);
			// 
			// objectList1
			// 
			this->objectList1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->objectList1->Location = System::Drawing::Point(8, 7);
			this->objectList1->Name = L"objectList1";
			this->objectList1->SelectedIndex = -1;
			this->objectList1->Size = System::Drawing::Size(288, 143);
			this->objectList1->TabIndex = 0;
			this->objectList1->NewPressed += gcnew System::EventHandler(this, &dynControlsForm::objectList1_NewPressed);
			this->objectList1->EditPressed += gcnew System::EventHandler(this, &dynControlsForm::objectList1_EditPressed);
			this->objectList1->DeletePressed += gcnew System::EventHandler(this, &dynControlsForm::objectList1_DeletePressed);
			this->objectList1->SelectedIndexChanged += gcnew System::EventHandler(this, &dynControlsForm::objectList1_SelectedIndexChanged);
			// 
			// vector3Edit_Scale
			// 
			this->vector3Edit_Scale->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->vector3Edit_Scale->Location = System::Drawing::Point(8, 335);
			this->vector3Edit_Scale->Name = L"vector3Edit_Scale";
			this->vector3Edit_Scale->Precision = static_cast<System::Int16>(2);
			this->vector3Edit_Scale->Size = System::Drawing::Size(272, 32);
			this->vector3Edit_Scale->TabIndex = 8;
			this->vector3Edit_Scale->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &dynControlsForm::vector3Edit_Scale_KeyPressChild);
			this->vector3Edit_Scale->LeaveChild += gcnew System::EventHandler(this, &dynControlsForm::vector3Edit_Scale_LeaveChild);
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label3->Location = System::Drawing::Point(8, 315);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(160, 16);
			this->label3->TabIndex = 7;
			this->label3->Text = L"Scale:";
			// 
			// dynControlsForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(312, 394);
			this->Controls->Add(this->tabControl1);
			this->Name = L"dynControlsForm";
			this->Text = L"dynControlsForm";
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

		void SetupData(dynScriptObject *i_pObject)
		{
			if (m_bOurChange) return;

			fill_list(i_pObject);
		}


	public:
    	//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage1;
		}

	private:
		// Fill objectList
		void fill_list(dynScriptObject *i_pObject)
		{
			this->objectList1->Items->Clear();
			this->rangedFloat1->Enabled = false;
			this->rangedFloat2->Enabled = false;
			this->rangedFloat3->Enabled = false;
			this->vector3Edit_Translate->Enabled = false;
			
			if (i_pObject)
			{
				int num_items = i_pObject->GetNumControls();
				for (int i=0; i<num_items; i++)
				{	
					std::string str = i_pObject->GetControlData(i).m_Name;
					this->objectList1->Items->Add( gcnew String(str.c_str()) );
				}	
			}
		}

		void update_control_data(const dynControlData& i_Data)
		{
			m_bDisableNotify = true;
			if (i_Data.m_ControlType == dynControlData::e_RotateControl)
			{
				this->rangedFloat1->Enabled = true;
				this->rangedFloat1->Minimum = i_Data.m_MinAngle;
				this->rangedFloat1->Maximum = i_Data.m_MaxAngle;
				this->rangedFloat1->Value = i_Data.m_Value;
				this->rangedFloat2->Enabled = false;
				this->rangedFloat3->Enabled = false;
				this->vector3Edit_Translate->Enabled = false;
				this->vector3Edit_Scale->Enabled = false;
			}
			else
			{
				this->rangedFloat1->Enabled = true;
				this->rangedFloat1->Minimum = -180;
				this->rangedFloat1->Maximum = 180;
				this->rangedFloat1->Value = i_Data.m_Rotation.m_X;
				this->rangedFloat2->Enabled = true;
				this->rangedFloat2->Minimum = -180;
				this->rangedFloat2->Maximum = 180;
				this->rangedFloat2->Value = i_Data.m_Rotation.m_Y;
				this->rangedFloat3->Enabled = true;
				this->rangedFloat3->Minimum = -180;
				this->rangedFloat3->Maximum = 180;
				this->rangedFloat3->Value = i_Data.m_Rotation.m_Z;

				this->vector3Edit_Translate->Enabled = true;
				tmaManagedConversionUtil::SetPoint3(i_Data.m_Translation, this->vector3Edit_Translate);
				this->vector3Edit_Scale->Enabled = true;
				tmaManagedConversionUtil::SetPoint3(i_Data.m_Scale, this->vector3Edit_Scale);
			}
			m_bDisableNotify = false;
		}
		

private: System::Void objectList1_NewPressed(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (m_pObject)
			 {
				 // From now on, only create full transform controls.
				 // Could be two separate New buttons, or a dialog choice.
				 // But, changing the control type in the edit dialog would 
				 // likely cause problems.
				dynControlData data;
				data.m_ControlType = dynControlData::e_TransformControl;
				dynControlDataForm ^dialog = gcnew dynControlDataForm(data, *m_pObject);
				if (dialog->ShowDialog() == ::DialogResult::OK)
				{
					m_pObject->AddControl(data);
					fill_list(m_pObject);
					this->objectList1->SelectedIndex = m_pObject->GetNumControls()-1;
				}

				delete dialog;
			}
		 }

private: System::Void objectList1_EditPressed(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 int sel_index = this->objectList1->SelectedIndex;
			 if (sel_index >= 0)
			 {
				dynControlData data = m_pObject->GetControlData(sel_index);
				dynControlDataForm ^dialog = gcnew dynControlDataForm(data, *m_pObject);
				if (dialog->ShowDialog() == ::DialogResult::OK)
				{
					m_pObject->AlterControl(sel_index, data);
					fill_list(m_pObject);
					this->objectList1->SelectedIndex = sel_index;
				}

				delete dialog;
			 }
		 }

private: System::Void objectList1_DeletePressed(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 int sel_index = this->objectList1->SelectedIndex;
			 if (sel_index >= 0)
			 {
				 m_pObject->DeleteControl(sel_index, true);
				 fill_list(m_pObject);
				 chnlDialogUtil::UpdateChannels();
			 }
		 }
private: System::Void objectList1_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 int sel_index = this->objectList1->SelectedIndex;
			 if (sel_index >= 0)
			 {
				// Store last selected control index in order to
				// maintain the selection when re-selecting the object
				m_pObject->SetLastSelectedControlIndex(sel_index);

				const dynControlData& data = m_pObject->GetControlData(sel_index);
				update_control_data(data);
			 }
		 }

private: System::Void vector3Edit_Translate_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
 			if (e->KeyChar != (char)13)
				return;
			vector3Edit_Translate_ValueChanged(sender,e);
		 }
private: System::Void vector3Edit_Translate_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			vector3Edit_Translate_ValueChanged(sender,e);
		 }
private: System::Void vector3Edit_Translate_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
				int sel_index = this->objectList1->SelectedIndex;
				if (sel_index >= 0)
				{
					if (m_pObject->GetControlType(sel_index) == dynControlData::e_TransformControl)
					{
						maVector3d vec( (float) vector3Edit_Translate->ValueX, 
										(float) vector3Edit_Translate->ValueY, 
										(float) vector3Edit_Translate->ValueZ);
		
						m_pObject->SetTranslation(sel_index, vec);
					}
				}
			 }
		 }

private: System::Void vector3Edit_Scale_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
 			if (e->KeyChar != (char)13)
				return;
			vector3Edit_Scale_ValueChanged(sender,e);
		 }
private: System::Void vector3Edit_Scale_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			vector3Edit_Scale_ValueChanged(sender,e);
		 }
private: System::Void vector3Edit_Scale_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
				int sel_index = this->objectList1->SelectedIndex;
				if (sel_index >= 0)
				{
					if (m_pObject->GetControlType(sel_index) == dynControlData::e_TransformControl)
					{
						maVector3d vec( (float) vector3Edit_Scale->ValueX, 
										(float) vector3Edit_Scale->ValueY, 
										(float) vector3Edit_Scale->ValueZ);
		
						m_pObject->SetScale(sel_index, vec);
					}
				}
			 }
		 }

private: System::Void rangedFloat1_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
			if (e->KeyChar != (char)13)
				return;

			if (!m_bDisableNotify)
			{
				int sel_index = this->objectList1->SelectedIndex;
				if (sel_index >= 0)
				{
					if (m_pObject->GetControlType(sel_index) == dynControlData::e_RotateControl)
					{
						m_pObject->SetAngle(sel_index, (float)this->rangedFloat1->Value);
					}
					else
					{
						m_pObject->SetAngle(sel_index, dynControlData::e_X, (float)this->rangedFloat1->Value);
					}
				}
			}
		 }

private: System::Void rangedFloat1_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
				int sel_index = this->objectList1->SelectedIndex;
				if (sel_index >= 0)
				{
					if (m_pObject->GetControlType(sel_index) == dynControlData::e_RotateControl)
					{
						m_pObject->SetAngle(sel_index, (float)this->rangedFloat1->Value);
					}
					else
					{
						m_pObject->SetAngle(sel_index, dynControlData::e_X, (float)this->rangedFloat1->Value);
					}
				}
			 }
		 }

private: System::Void rangedFloat2_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
			if (e->KeyChar != (char)13)
				return;

			 if (!m_bDisableNotify)
			 {
				int sel_index = this->objectList1->SelectedIndex;
				if (sel_index >= 0)
				{
					if (m_pObject->GetControlType(sel_index) == dynControlData::e_TransformControl)
					{
						m_pObject->SetAngle(sel_index, dynControlData::e_Y, (float)this->rangedFloat2->Value);
					}
				}
			 }
		 }

private: System::Void rangedFloat2_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
				int sel_index = this->objectList1->SelectedIndex;
				if (sel_index >= 0)
				{
					if (m_pObject->GetControlType(sel_index) == dynControlData::e_TransformControl)
					{
						m_pObject->SetAngle(sel_index, dynControlData::e_Y, (float)this->rangedFloat2->Value);
					}
				}
			 }
		 }

private: System::Void rangedFloat3_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
			if (e->KeyChar != (char)13)
				return;

			 if (!m_bDisableNotify)
			 {
				int sel_index = this->objectList1->SelectedIndex;
				if (sel_index >= 0)
				{
					if (m_pObject->GetControlType(sel_index) == dynControlData::e_TransformControl)
					{
						m_pObject->SetAngle(sel_index, dynControlData::e_Z, (float)this->rangedFloat3->Value);
					}
				}
			 }
		 }

private: System::Void rangedFloat3_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
				int sel_index = this->objectList1->SelectedIndex;
				if (sel_index >= 0)
				{
					if (m_pObject->GetControlType(sel_index) == dynControlData::e_TransformControl)
					{
						m_pObject->SetAngle(sel_index, dynControlData::e_Z, (float)this->rangedFloat3->Value);
					}
				}
			 }
		 }

};
}
#endif // _MANAGED
