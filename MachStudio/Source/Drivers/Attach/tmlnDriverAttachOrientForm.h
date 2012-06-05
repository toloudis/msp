#pragma once

#ifndef TMLN_DRIVERATTACHORIENT_HPP
#include "Drivers/Attach/tmlnDriverAttachOrient.hpp"
#endif
#ifndef TMLN_DRIVERATTACHORIENTINFO_HPP
#include "Drivers/Attach/tmlnDriverAttachOrientInfo.hpp"
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
#ifndef MA_CONSTANTS_HPP
#include "Core/ma/maConstants.hpp"
#endif
#include "Drivers/Attach/tmlnRefNameForm.h"

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
	/// Summary for tmlnDriverAttachOrientForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class tmlnDriverAttachOrientForm : public System::Windows::Forms::Form
	{
	public: 
		static tmlnDriverAttachOrientForm ^FormInstance = nullptr;
		static bool sm_bPreservePosition = false;
	public: 
		tmlnDriverAttachOrientForm(tmlnDriverAttachOrient &i_Driver)
			: m_Driver(i_Driver)
		{
			m_bDisableNotify = true;

			InitializeComponent();

			FormInstance = this;

			SetUpComponents();

			m_bDisableNotify = false;
		}
        
		void UpdateForm()
		{
			SetUpComponents();
			this->Invalidate();
		}

	protected: 
		~tmlnDriverAttachOrientForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: tmlnDriverAttachOrient &m_Driver;
	private: bool m_bDisableNotify;

	private: System::Windows::Forms::CheckBox ^  checkBox_PreservePosition;
	private: System::Windows::Forms::TextBox ^  textAttachName;
	private: System::Windows::Forms::Button ^  butLookUp;
	private: System::Windows::Forms::TabControl ^  tabControl_attach;
	private: System::Windows::Forms::TabPage ^  tabPage_attach;
	private: System::Windows::Forms::Label ^  label_attachobject;
	private: System::Windows::Forms::ComboBox ^  comboBox_attachobject;
	private: System::Windows::Forms::Label ^  label_attachnode;
	private: System::Windows::Forms::Label ^  label_attachoffset;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_offset;
	private: System::Windows::Forms::Label ^  label1;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_orient;
	private: System::Windows::Forms::Label ^  label2;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_worldOffset;

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
			this->label_attachobject = gcnew System::Windows::Forms::Label();
			this->textAttachName = gcnew System::Windows::Forms::TextBox();
			this->label_attachnode = gcnew System::Windows::Forms::Label();
			this->vector3Edit_offset = gcnew TerawattManagedControls::Vector3Edit();
			this->label_attachoffset = gcnew System::Windows::Forms::Label();
			this->butLookUp = gcnew System::Windows::Forms::Button();
			this->tabControl_attach = gcnew System::Windows::Forms::TabControl();
			this->tabPage_attach = gcnew System::Windows::Forms::TabPage();
			this->checkBox_PreservePosition = gcnew System::Windows::Forms::CheckBox();
			this->label2 = gcnew System::Windows::Forms::Label();
			this->vector3Edit_worldOffset = gcnew TerawattManagedControls::Vector3Edit();
			this->vector3Edit_orient = gcnew TerawattManagedControls::Vector3Edit();
			this->label1 = gcnew System::Windows::Forms::Label();
			this->comboBox_attachobject = gcnew System::Windows::Forms::ComboBox();
			this->tabControl_attach->SuspendLayout();
			this->tabPage_attach->SuspendLayout();
			this->SuspendLayout();
			// 
			// label_attachobject
			// 
			this->label_attachobject->Location = System::Drawing::Point(16, 40);
			this->label_attachobject->Name = "label_attachobject";
			this->label_attachobject->Size = System::Drawing::Size(104, 28);
			this->label_attachobject->TabIndex = 0;
			this->label_attachobject->Text = "Attach Object:";
			// 
			// textAttachName
			// 
			this->textAttachName->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textAttachName->Location = System::Drawing::Point(128, 80);
			this->textAttachName->Name = "textAttachName";
			this->textAttachName->Size = System::Drawing::Size(184, 20);
			this->textAttachName->TabIndex = 3;
			this->textAttachName->Text = "";
			this->textAttachName->TextChanged += gcnew System::EventHandler(this, &tmlnDriverAttachOrientForm::textAttachName_TextChanged);
			// 
			// label_attachnode
			// 
			this->label_attachnode->Location = System::Drawing::Point(16, 72);
			this->label_attachnode->Name = "label_attachnode";
			this->label_attachnode->Size = System::Drawing::Size(104, 27);
			this->label_attachnode->TabIndex = 2;
			this->label_attachnode->Text = "Attachment Node:";
			// 
			// vector3Edit_offset
			// 
			this->vector3Edit_offset->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->vector3Edit_offset->Location = System::Drawing::Point(128, 120);
			this->vector3Edit_offset->Name = "vector3Edit_offset";
			this->vector3Edit_offset->Precision = (System::Int16)2;
			this->vector3Edit_offset->Size = System::Drawing::Size(264, 28);
			this->vector3Edit_offset->TabIndex = 4;
			this->vector3Edit_offset->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &tmlnDriverAttachOrientForm::vector3Edit_offset_KeyPressChild);
			this->vector3Edit_offset->LeaveChild += gcnew System::EventHandler(this, &tmlnDriverAttachOrientForm::vector3Edit_offset_LeaveChild);
			// 
			// label_attachoffset
			// 
			this->label_attachoffset->Location = System::Drawing::Point(16, 120);
			this->label_attachoffset->Name = "label_attachoffset";
			this->label_attachoffset->Size = System::Drawing::Size(104, 28);
			this->label_attachoffset->TabIndex = 5;
			this->label_attachoffset->Text = "Attach Offset:";
			// 
			// butLookUp
			// 
			this->butLookUp->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->butLookUp->Location = System::Drawing::Point(328, 80);
			this->butLookUp->Name = "butLookUp";
			this->butLookUp->Size = System::Drawing::Size(67, 20);
			this->butLookUp->TabIndex = 6;
			this->butLookUp->Text = "Look up...";
			this->butLookUp->Click += gcnew System::EventHandler(this, &tmlnDriverAttachOrientForm::butLookUp_Click);
			// 
			// tabControl_attach
			// 
			this->tabControl_attach->Controls->Add(this->tabPage_attach);
			this->tabControl_attach->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl_attach->Location = System::Drawing::Point(0, 0);
			this->tabControl_attach->Name = "tabControl_attach";
			this->tabControl_attach->SelectedIndex = 0;
			this->tabControl_attach->Size = System::Drawing::Size(416, 278);
			this->tabControl_attach->TabIndex = 7;
			// 
			// tabPage_attach
			// 
			this->tabPage_attach->Controls->Add(this->checkBox_PreservePosition);
			this->tabPage_attach->Controls->Add(this->label2);
			this->tabPage_attach->Controls->Add(this->vector3Edit_worldOffset);
			this->tabPage_attach->Controls->Add(this->vector3Edit_orient);
			this->tabPage_attach->Controls->Add(this->label1);
			this->tabPage_attach->Controls->Add(this->comboBox_attachobject);
			this->tabPage_attach->Controls->Add(this->textAttachName);
			this->tabPage_attach->Controls->Add(this->label_attachobject);
			this->tabPage_attach->Controls->Add(this->butLookUp);
			this->tabPage_attach->Controls->Add(this->label_attachoffset);
			this->tabPage_attach->Controls->Add(this->vector3Edit_offset);
			this->tabPage_attach->Controls->Add(this->label_attachnode);
			this->tabPage_attach->Location = System::Drawing::Point(4, 22);
			this->tabPage_attach->Name = "tabPage_attach";
			this->tabPage_attach->Size = System::Drawing::Size(408, 252);
			this->tabPage_attach->TabIndex = 0;
			this->tabPage_attach->Text = "Attach";
			// 
			// checkBox_PreservePosition
			// 
			this->checkBox_PreservePosition->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->checkBox_PreservePosition->Location = System::Drawing::Point(272, 8);
			this->checkBox_PreservePosition->Name = "checkBox_PreservePosition";
			this->checkBox_PreservePosition->Size = System::Drawing::Size(120, 24);
			this->checkBox_PreservePosition->TabIndex = 15;
			this->checkBox_PreservePosition->Text = "Preserve Position";
			this->checkBox_PreservePosition->CheckedChanged += gcnew System::EventHandler(this, &tmlnDriverAttachOrientForm::checkBox_PreservePosition_CheckedChanged);
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(16, 200);
			this->label2->Name = "label2";
			this->label2->Size = System::Drawing::Size(104, 32);
			this->label2->TabIndex = 11;
			this->label2->Text = "World Space Offset:";
			// 
			// vector3Edit_worldOffset
			// 
			this->vector3Edit_worldOffset->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->vector3Edit_worldOffset->Location = System::Drawing::Point(128, 200);
			this->vector3Edit_worldOffset->Name = "vector3Edit_worldOffset";
			this->vector3Edit_worldOffset->Precision = (System::Int16)2;
			this->vector3Edit_worldOffset->Size = System::Drawing::Size(264, 28);
			this->vector3Edit_worldOffset->TabIndex = 10;
			this->vector3Edit_worldOffset->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &tmlnDriverAttachOrientForm::vector3Edit_worldOffset_KeyPressChild);
			this->vector3Edit_worldOffset->LeaveChild += gcnew System::EventHandler(this, &tmlnDriverAttachOrientForm::vector3Edit_worldOffset_LeaveChild);
			// 
			// vector3Edit_orient
			// 
			this->vector3Edit_orient->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->vector3Edit_orient->Location = System::Drawing::Point(128, 160);
			this->vector3Edit_orient->Name = "vector3Edit_orient";
			this->vector3Edit_orient->Precision = (System::Int16)2;
			this->vector3Edit_orient->Size = System::Drawing::Size(264, 24);
			this->vector3Edit_orient->TabIndex = 9;
			this->vector3Edit_orient->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &tmlnDriverAttachOrientForm::vector3Edit_orient_KeyPressChild);
			this->vector3Edit_orient->LeaveChild += gcnew System::EventHandler(this, &tmlnDriverAttachOrientForm::vector3Edit_orient_LeaveChild);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(16, 160);
			this->label1->Name = "label1";
			this->label1->Size = System::Drawing::Size(104, 28);
			this->label1->TabIndex = 8;
			this->label1->Text = "Attach Orientation:";
			// 
			// comboBox_attachobject
			// 
			this->comboBox_attachobject->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->comboBox_attachobject->Location = System::Drawing::Point(128, 40);
			this->comboBox_attachobject->Name = "comboBox_attachobject";
			this->comboBox_attachobject->Size = System::Drawing::Size(264, 21);
			this->comboBox_attachobject->TabIndex = 7;
			this->comboBox_attachobject->SelectedIndexChanged += gcnew System::EventHandler(this, &tmlnDriverAttachOrientForm::comboBox_attachobject_SelectedIndexChanged);
			// 
			// tmlnDriverAttachOrientForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(416, 278);
			this->Controls->Add(this->tabControl_attach);
			this->Name = "tmlnDriverAttachOrientForm";
			this->Text = "Attach-Orient Properties";
			this->tabControl_attach->ResumeLayout(false);
			this->tabPage_attach->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

private: System::Void textAttachName_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
				if (!m_bDisableNotify)
				{
					m_bDisableNotify = true;
					tmlnDriverAttachOrientInfo *pInfo = dynamic_cast<tmlnDriverAttachOrientInfo*>(m_Driver.GetDriverInfo());
					tmaManagedStringUtils::ManagedStringToStdString(this->textAttachName->Text, pInfo->m_AttachName);

					// if true, keep object in place
					const bool bPreservePosition = sm_bPreservePosition; 
					m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo, bPreservePosition);
					delete pInfo;
					m_bDisableNotify = false;
				}
		 }

private: System::Void vector3Edit_offset_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
				if (!m_bDisableNotify)
				{
					m_bDisableNotify = true;
					maPoint3d offset( (float)this->vector3Edit_offset->ValueX, 
									  (float)this->vector3Edit_offset->ValueY, 
									  (float)this->vector3Edit_offset->ValueZ);
					m_Driver.SetAttachOffset( offset );
					m_bDisableNotify = false;
				}
		 }

private: System::Void vector3Edit_orient_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
				 if (!m_bDisableNotify)
				 {
					 m_bDisableNotify = true;
					 float yaw		= (float)(vector3Edit_orient->ValueX * maConstants::c_fAngleToRad);
					 float pitch	= (float)(vector3Edit_orient->ValueY * maConstants::c_fAngleToRad);
					 float roll		= (float)(vector3Edit_orient->ValueZ * maConstants::c_fAngleToRad);
					 maRotation euler;
					 euler.SetEuler(yaw, pitch, roll);
					 m_Driver.SetAttachOrientation( euler );
					 m_bDisableNotify = false;
				 }
		 }
private: System::Void vector3Edit_worldOffset_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
				if (!m_bDisableNotify)
				{
					m_bDisableNotify = true;
					maPoint3d offset( (float)this->vector3Edit_worldOffset->ValueX, 
									  (float)this->vector3Edit_worldOffset->ValueY, 
									  (float)this->vector3Edit_worldOffset->ValueZ);
					m_Driver.SetWorldSpaceOffset( offset );
					m_bDisableNotify = false;
				}
		 }

private: System::Void butLookUp_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 //if (!m_bDisableNotify)
			 {
				std::vector<std::string> ref_names;
				m_Driver.GetReferenceList(ref_names);

				tmlnRefNameForm ^dialog = gcnew tmlnRefNameForm(ref_names);
				if (dialog->ShowDialog() == ::DialogResult::OK)
				{
					//Note: In this case, we want to notify when the text changes, because
					// that is what is desired when coming from the LookUp button.
					//m_bDisableNotify = true;
					this->textAttachName->Text = gcnew System::String(ref_names[dialog->GetSelected()].c_str());
					//m_bDisableNotify = false;
				}

				delete dialog;

				//DBG_LOG1("Num refs: %d", ref_names.size());
				//for (int i=0; i<ref_names.size(); i++)
				//{
				//	DBG_LOG1("Ref name: %s", ref_names[i].c_str());
				//}
			}
		 }

private: System::Void comboBox_attachobject_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				//this->textAttachName->Clear();

				tmlnDriverAttachOrientInfo *pInfo = dynamic_cast<tmlnDriverAttachOrientInfo*>(m_Driver.GetDriverInfo());
				std::string name;
				tmaManagedStringUtils::ManagedStringToStdString( this->comboBox_attachobject->Text, name );

				//	set the name + invalidate the UID
				pInfo->m_ObjectName.SetString( name );
				pInfo->m_ObjectName.SetUID( nameString::e_InvalidUID );
				DBG_LOG2( "combobox attach object UID-%02d (%s)", pInfo->m_ObjectName.GetUID(), pInfo->m_ObjectName.GetString().c_str() );

				//	get the UID based on the selected text and set it.
				//
				nameObject* pNO = nameMgr::GetObjectByName( pInfo->m_ObjectName );
				if ( pNO != 0 )
				{
					pInfo->m_ObjectName.SetUID( pNO->GetName().GetUID() );
					pInfo->m_ObjectName.SetString( pNO->GetName().GetString() );
				}

				DBG_LOG2( "combobox attach object UID=%02d (%s)", pInfo->m_ObjectName.GetUID(), pInfo->m_ObjectName.GetString().c_str() );

					// if true, keep object in place
					const bool bPreservePosition = sm_bPreservePosition; 
				m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo, bPreservePosition);
				delete pInfo;
				m_bDisableNotify = false;
			}
		 }

private:
	//	Set-up the components on construction
	//
	void SetUpComponents()
	{
		tmlnDriverAttachOrientInfo *pInfo = dynamic_cast<tmlnDriverAttachOrientInfo*>(m_Driver.GetDriverInfo());

		//	set the attachment offset
		tmaManagedConversionUtil::SetPoint3(pInfo->m_AttachOffset, this->vector3Edit_offset);
		tmaManagedConversionUtil::SetPoint3(pInfo->m_WorldSpaceOffset, this->vector3Edit_worldOffset);

		// attachment orientation
		tmaManagedConversionUtil::SetRotation3( pInfo->m_AttachOrientation, this->vector3Edit_orient );

		//	fill-in combobox
		//
		nameList aNameList;
		nameMgr::GetNameList( aNameList );
		for ( int i = 0 ; i < aNameList.size() ; i++ )
		{
			//DBG_LOG2( "%02d) %s", i, aNameList[i]->GetString().c_str() );
	
			comboBox_attachobject->Items->Add( gcnew String(aNameList[i]->GetString().c_str()) );
		}

		if ( !pInfo->m_ObjectName.GetString().empty() )
		{
			//	if there is a valid name UID then get the name text associated
			//	with it in case it changed since the item grabbed it last.
			//
			//DBG_LOG1( "object UID %d", pInfo->m_ObjectName.GetUID() );

			if ( pInfo->m_ObjectName.GetUID() != nameString::e_InvalidUID )
			{
				std::string name;
				nameMgr::GetNameString( pInfo->m_ObjectName.GetUID(), name );
				pInfo->m_ObjectName.SetString( name );
				m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);

				//DBG_LOG1( "attach object (%s)", name.c_str() );
			}

			System::String ^mstr = comboBox_attachobject->Text;
			std::string str;
			tmaManagedStringUtils::ManagedStringToStdString(mstr, str);

			//DBG_LOG2( "object (%s) vs object (%s)", str.c_str(), pInfo->m_ObjectName.GetString().c_str() );

			if ( pInfo->m_ObjectName.GetString() != str )
			{
				m_bDisableNotify = true;
				comboBox_attachobject->Text = gcnew String( pInfo->m_ObjectName.GetString().c_str() );
				m_bDisableNotify = false;
			}
		}

		//	set the attachment part name
		this->textAttachName->Text = gcnew System::String(pInfo->m_AttachName.c_str());

		// The checkbox "PreservePosition" is static for this class, not
		// based on the driver's info
		this->checkBox_PreservePosition->Checked = sm_bPreservePosition;

		delete pInfo;
	}

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_attach;
		}


private: System::Void vector3Edit_offset_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
 			if (e->KeyChar != (char)13)
				return;
			vector3Edit_offset_ValueChanged(sender,e);
		 }

private: System::Void vector3Edit_offset_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			vector3Edit_offset_ValueChanged(sender,e);
		 }

private: System::Void vector3Edit_orient_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
 			if (e->KeyChar != (char)13)
				return;
			vector3Edit_orient_ValueChanged(sender,e);
		 }

private: System::Void vector3Edit_orient_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			vector3Edit_orient_ValueChanged(sender,e);
		 }

private: System::Void vector3Edit_worldOffset_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
 			if (e->KeyChar != (char)13)
				return;
			vector3Edit_worldOffset_ValueChanged(sender,e);
		 }

private: System::Void vector3Edit_worldOffset_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			vector3Edit_worldOffset_ValueChanged(sender,e);
		 }

private: System::Void checkBox_PreservePosition_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
				// The checkbox "PreservePosition" is static for this class, not
				// based on the driver's info
				sm_bPreservePosition = this->checkBox_PreservePosition->Checked;
		 }

};
}
#endif // _MANAGED
