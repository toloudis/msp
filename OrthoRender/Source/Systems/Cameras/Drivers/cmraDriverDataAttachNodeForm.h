#pragma once

#ifndef CMRA_DRIVERDATAATTACHNODEINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataAttachNodeInfo.hpp"
#endif
#ifndef CMRA_OBJECTMGR_HPP
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#endif

#ifndef DBG_LOG_HPP
#include "Core/Dbg/dbgLog.hpp"
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
#ifndef NAME_TYPES_HPP
#include "Core/name/nameTypes.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif

#include "Systems/Cameras/Timeline/cmraRefNameForm.h"

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
	/// Summary for cmraDriverDataAttachNodeForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmraDriverDataAttachNodeForm : public System::Windows::Forms::Form
	{
	public: 
		static cmraDriverDataAttachNodeForm ^FormInstance = nullptr;
	public: 
		cmraDriverDataAttachNodeForm(cmraDriverDataAttachNodeInfo &i_Data)
			: m_Data(i_Data)
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
		~cmraDriverDataAttachNodeForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: cmraDriverDataAttachNodeInfo &m_Data;
	private: bool m_bDisableNotify;
	private: System::Windows::Forms::TextBox ^  textAttachName;
	private: System::Windows::Forms::Button ^  butLookUp;
	private: System::Windows::Forms::TabControl ^  tabControl_target;
	private: System::Windows::Forms::Label ^  label_targetobject;
	private: System::Windows::Forms::Label ^  label_attachnode;
	private: System::Windows::Forms::Label ^  label_targetoffset;
	private: System::Windows::Forms::ComboBox ^  comboBox_targetobject;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_targetoffset;
	private: System::Windows::Forms::TabPage ^  tabPage_camerasubject;

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
			this->label_targetobject = gcnew System::Windows::Forms::Label();
			this->textAttachName = gcnew System::Windows::Forms::TextBox();
			this->label_attachnode = gcnew System::Windows::Forms::Label();
			this->vector3Edit_targetoffset = gcnew TerawattManagedControls::Vector3Edit();
			this->label_targetoffset = gcnew System::Windows::Forms::Label();
			this->butLookUp = gcnew System::Windows::Forms::Button();
			this->tabControl_target = gcnew System::Windows::Forms::TabControl();
			this->tabPage_camerasubject = gcnew System::Windows::Forms::TabPage();
			this->comboBox_targetobject = gcnew System::Windows::Forms::ComboBox();
			this->tabControl_target->SuspendLayout();
			this->tabPage_camerasubject->SuspendLayout();
			this->SuspendLayout();
			// 
			// label_targetobject
			// 
			this->label_targetobject->Location = System::Drawing::Point(8, 18);
			this->label_targetobject->Name = "label_targetobject";
			this->label_targetobject->Size = System::Drawing::Size(80, 16);
			this->label_targetobject->TabIndex = 0;
			this->label_targetobject->Text = "AttachNode:";
			// 
			// textAttachName
			// 
			this->textAttachName->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textAttachName->Location = System::Drawing::Point(104, 56);
			this->textAttachName->Name = "textAttachName";
			this->textAttachName->Size = System::Drawing::Size(232, 20);
			this->textAttachName->TabIndex = 2;
			this->textAttachName->Text = "";
			this->textAttachName->TextChanged += gcnew System::EventHandler(this, &cmraDriverDataAttachNodeForm::textAttachName_TextChanged);
			// 
			// label_attachnode
			// 
			this->label_attachnode->Location = System::Drawing::Point(8, 56);
			this->label_attachnode->Name = "label_attachnode";
			this->label_attachnode->Size = System::Drawing::Size(96, 16);
			this->label_attachnode->TabIndex = 2;
			this->label_attachnode->Text = "Attachment Node:";
			// 
			// vector3Edit_targetoffset
			// 
			this->vector3Edit_targetoffset->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->vector3Edit_targetoffset->Location = System::Drawing::Point(104, 88);
			this->vector3Edit_targetoffset->Name = "vector3Edit_targetoffset";
			this->vector3Edit_targetoffset->Precision = (System::Int16)2;
			this->vector3Edit_targetoffset->Size = System::Drawing::Size(288, 28);
			this->vector3Edit_targetoffset->TabIndex = 4;
			this->vector3Edit_targetoffset->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &cmraDriverDataAttachNodeForm::vector3Edit_targetoffset_KeyPressChild);
			this->vector3Edit_targetoffset->LeaveChild += gcnew System::EventHandler(this, &cmraDriverDataAttachNodeForm::vector3Edit_targetoffset_LeaveChild);
			// 
			// label_targetoffset
			// 
			this->label_targetoffset->Location = System::Drawing::Point(8, 88);
			this->label_targetoffset->Name = "label_targetoffset";
			this->label_targetoffset->Size = System::Drawing::Size(80, 16);
			this->label_targetoffset->TabIndex = 5;
			this->label_targetoffset->Text = "Target Offset:";
			// 
			// butLookUp
			// 
			this->butLookUp->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->butLookUp->Location = System::Drawing::Point(344, 56);
			this->butLookUp->Name = "butLookUp";
			this->butLookUp->Size = System::Drawing::Size(67, 20);
			this->butLookUp->TabIndex = 3;
			this->butLookUp->Text = "Look up...";
			this->butLookUp->Click += gcnew System::EventHandler(this, &cmraDriverDataAttachNodeForm::butLookUp_Click);
			// 
			// tabControl_target
			// 
			this->tabControl_target->Controls->Add(this->tabPage_camerasubject);
			this->tabControl_target->Location = System::Drawing::Point(8, 8);
			this->tabControl_target->Name = "tabControl_target";
			this->tabControl_target->SelectedIndex = 0;
			this->tabControl_target->Size = System::Drawing::Size(424, 152);
			this->tabControl_target->TabIndex = 7;
			// 
			// tabPage_camerasubject
			// 
			this->tabPage_camerasubject->Controls->Add(this->comboBox_targetobject);
			this->tabPage_camerasubject->Controls->Add(this->label_attachnode);
			this->tabPage_camerasubject->Controls->Add(this->vector3Edit_targetoffset);
			this->tabPage_camerasubject->Controls->Add(this->label_targetoffset);
			this->tabPage_camerasubject->Controls->Add(this->butLookUp);
			this->tabPage_camerasubject->Controls->Add(this->label_targetobject);
			this->tabPage_camerasubject->Controls->Add(this->textAttachName);
			this->tabPage_camerasubject->Location = System::Drawing::Point(4, 22);
			this->tabPage_camerasubject->Name = "tabPage_camerasubject";
			this->tabPage_camerasubject->Size = System::Drawing::Size(416, 126);
			this->tabPage_camerasubject->TabIndex = 0;
			this->tabPage_camerasubject->Text = "AttachNode";
			this->tabPage_camerasubject->ToolTipText = "Camera Focus AttachNode";
			// 
			// comboBox_targetobject
			// 
			this->comboBox_targetobject->Location = System::Drawing::Point(104, 16);
			this->comboBox_targetobject->Name = "comboBox_targetobject";
			this->comboBox_targetobject->Size = System::Drawing::Size(200, 21);
			this->comboBox_targetobject->TabIndex = 1;
			this->comboBox_targetobject->SelectedIndexChanged += gcnew System::EventHandler(this, &cmraDriverDataAttachNodeForm::comboBox_targetobject_SelectedIndexChanged);
			// 
			// cmraDriverDataAttachNodeForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(440, 165);
			this->Controls->Add(this->tabControl_target);
			this->Name = "cmraDriverDataAttachNodeForm";
			this->Text = "Camera AttachNode Properties";
			this->tabControl_target->ResumeLayout(false);
			this->tabPage_camerasubject->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

private: System::Void textAttachName_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				tmaManagedStringUtils::ManagedStringToStdString(this->textAttachName->Text, m_Data.m_AttachName);
				m_bDisableNotify = false;
			}
		 }

private: System::Void vector3Edit_targetoffset_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				m_Data.m_TargetOffset.Set( (float)this->vector3Edit_targetoffset->ValueX, (float)this->vector3Edit_targetoffset->ValueY, (float)this->vector3Edit_targetoffset->ValueZ );
				m_bDisableNotify = false;
			}
		 }

private: System::Void butLookUp_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			//std::vector<std::string> ref_names;
			//m_Data.GetReferenceList(ref_names);

			//cmraRefNameForm ^dialog = gcnew cmraRefNameForm(ref_names);
			//if (dialog->ShowDialog() == ::DialogResult::OK)
			//{
			//	this->textAttachName->Text = gcnew System::String(ref_names[dialog->GetSelected()].c_str());
			//}

			//DBG_LOG1("Num refs: %d", ref_names.size());
			//for (int i=0; i<ref_names.size(); i++)
			//{
			//	DBG_LOG1("Ref name: %s", ref_names[i].c_str());
			//}
		 }

private: System::Void comboBox_targetobject_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				std::string name;
				tmaManagedStringUtils::ManagedStringToStdString( this->comboBox_targetobject->Text, name );

				//	set the name + invalidate the UID
				nameString theName;
				theName.SetUID( nameString::e_InvalidUID );
				theName.SetString( name );
				m_Data.m_ObjectName = theName;
				//DBG_LOG2( "combobox target object UID-%02d (%s)", m_Data.m_ObjectName.GetUID(), m_Data.m_ObjectName.GetString().c_str() );

				//	get the UID based on the selected text and set it.
				//
				nameObject* pNO = nameMgr::GetObjectByName( m_Data.m_ObjectName );
				if ( pNO != 0 )
				{
					m_Data.m_ObjectName.SetUID( pNO->GetName().GetUID() );
					m_Data.m_ObjectName.SetString( pNO->GetName().GetString() );
				}
				m_bDisableNotify = false;
			}
		 }

private:
		//
		void SetUpComponents()
		{
			//	build the camera list
			//
			nameList aNameList;
			nameMgr::GetNameList( aNameList );
			for ( int i = 0 ; i < aNameList.size() ; i++ )
			{
				//DBG_LOG2( "%02d) %s", i, aNameList[i]->GetString().c_str() );
		
				comboBox_targetobject->Items->Add( gcnew String(aNameList[i]->GetString().c_str()) );
			}

			if ( !m_Data.m_ObjectName.GetString().empty() )
			{
				//	if there is a valid name UID then get the name text associated
				//	with it in case it changed since the item grabbed it last.
				//
				//DBG_LOG1( "object UID %d", m_Data.m_ObjectName.GetUID() );

				if ( m_Data.m_ObjectName.GetUID() != nameString::e_InvalidUID )
				{
					std::string name;
					nameMgr::GetNameString( m_Data.m_ObjectName.GetUID(), name );
					m_Data.m_ObjectName.SetString( name );

					//DBG_LOG1( "attach object (%s)", name.c_str() );
				}

				System::String ^mstr = comboBox_targetobject->Text;
				std::string str;
				tmaManagedStringUtils::ManagedStringToStdString(mstr, str);

				//DBG_LOG2( "object (%s) vs object (%s)", str.c_str(), m_Data.m_ObjectName.GetString().c_str() );

				if ( m_Data.m_ObjectName.GetString() != str )
				{
					m_bDisableNotify = true;
					comboBox_targetobject->Text = gcnew String( m_Data.m_ObjectName.GetString().c_str() );
					m_bDisableNotify = false;
				}
			}

			this->textAttachName->Text = gcnew System::String(m_Data.m_AttachName.c_str());
			tmaManagedConversionUtil::SetPoint3(m_Data.m_TargetOffset, this->vector3Edit_targetoffset);
		}

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_camerasubject;
		}

private: System::Void vector3Edit_targetoffset_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
 			if (e->KeyChar != (char)13)
				return;
			vector3Edit_targetoffset_ValueChanged(sender,e);
		 }

private: System::Void vector3Edit_targetoffset_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			vector3Edit_targetoffset_ValueChanged(sender,e);
		 }

};
}
#endif // _MANAGED
