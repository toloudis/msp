#pragma once


#include "Drivers/Attach/tmlnRefNameForm.h"

#ifndef MNM_OBJECT_HPP
#include "Support/mnm/mnmObject.hpp"
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

namespace SystemProjectedLights
{
	/// <summary> 
	/// Summary for prjltCreateKeyLight
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class prjltCreateKeyLight : public System::Windows::Forms::Form
	{
	public: 
		prjltCreateKeyLight(void)
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

		void GetAttachNode(std::string& o_Node) 
		{
			tmaManagedStringUtils::ManagedStringToStdString(this->textAttachName->Text, o_Node);
		}
		
		void GetTargetOffset(maVector3d& o_Offset)
		{
			o_Offset.Set( (float) this->vector3Edit_TargetOffset->ValueX,
						  (float) this->vector3Edit_TargetOffset->ValueY,
						  (float) this->vector3Edit_TargetOffset->ValueZ );
		}

		void GetPositionOffset(maVector3d& o_Offset)
		{
			o_Offset.Set( (float) this->vector3Edit_PositionOffset->ValueX,
						  (float) this->vector3Edit_PositionOffset->ValueY,
						  (float) this->vector3Edit_PositionOffset->ValueZ );
		}
        

	public: 
		~prjltCreateKeyLight()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::ComboBox ^  comboBox_attachobject;
	private: System::Windows::Forms::TextBox ^  textAttachName;
	private: System::Windows::Forms::Label ^  label_attachobject;
	private: System::Windows::Forms::Button ^  butLookUp;
	private: System::Windows::Forms::Label ^  label_attachnode;

	private: System::Windows::Forms::Button ^  button2;
	private: System::Windows::Forms::Button ^  buttonOk;
	private: System::Windows::Forms::ComboBox ^  comboBox_lightSet;
	private: System::Windows::Forms::Label ^  label1;
	private: System::Windows::Forms::GroupBox ^  groupBoxTarget;
	private: System::Windows::Forms::GroupBox ^  groupBoxPosition;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_PositionOffset;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_TargetOffset;
	private: System::Windows::Forms::Label ^  label3;
	private: System::Windows::Forms::Label ^  label4;
	private: System::Windows::Forms::Label ^  label2;
	private: System::Windows::Forms::TextBox ^  textBox_Name;



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
			this->comboBox_attachobject = gcnew System::Windows::Forms::ComboBox();
			this->textAttachName = gcnew System::Windows::Forms::TextBox();
			this->label_attachobject = gcnew System::Windows::Forms::Label();
			this->butLookUp = gcnew System::Windows::Forms::Button();
			this->label_attachnode = gcnew System::Windows::Forms::Label();
			this->buttonOk = gcnew System::Windows::Forms::Button();
			this->button2 = gcnew System::Windows::Forms::Button();
			this->comboBox_lightSet = gcnew System::Windows::Forms::ComboBox();
			this->label1 = gcnew System::Windows::Forms::Label();
			this->groupBoxTarget = gcnew System::Windows::Forms::GroupBox();
			this->label3 = gcnew System::Windows::Forms::Label();
			this->vector3Edit_TargetOffset = gcnew TerawattManagedControls::Vector3Edit();
			this->groupBoxPosition = gcnew System::Windows::Forms::GroupBox();
			this->label4 = gcnew System::Windows::Forms::Label();
			this->vector3Edit_PositionOffset = gcnew TerawattManagedControls::Vector3Edit();
			this->label2 = gcnew System::Windows::Forms::Label();
			this->textBox_Name = gcnew System::Windows::Forms::TextBox();
			this->groupBoxTarget->SuspendLayout();
			this->groupBoxPosition->SuspendLayout();
			this->SuspendLayout();
			// 
			// comboBox_attachobject
			// 
			this->comboBox_attachobject->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->comboBox_attachobject->Location = System::Drawing::Point(88, 64);
			this->comboBox_attachobject->Name = "comboBox_attachobject";
			this->comboBox_attachobject->Size = System::Drawing::Size(256, 21);
			this->comboBox_attachobject->TabIndex = 2;
			this->comboBox_attachobject->SelectedIndexChanged += gcnew System::EventHandler(this, &prjltCreateKeyLight::comboBox_attachobject_SelectedIndexChanged);
			// 
			// textAttachName
			// 
			this->textAttachName->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textAttachName->Location = System::Drawing::Point(96, 32);
			this->textAttachName->Name = "textAttachName";
			this->textAttachName->Size = System::Drawing::Size(168, 20);
			this->textAttachName->TabIndex = 4;
			this->textAttachName->Text = "";
			// 
			// label_attachobject
			// 
			this->label_attachobject->Location = System::Drawing::Point(8, 64);
			this->label_attachobject->Name = "label_attachobject";
			this->label_attachobject->Size = System::Drawing::Size(80, 28);
			this->label_attachobject->TabIndex = 8;
			this->label_attachobject->Text = "Attach Object:";
			// 
			// butLookUp
			// 
			this->butLookUp->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->butLookUp->Location = System::Drawing::Point(272, 32);
			this->butLookUp->Name = "butLookUp";
			this->butLookUp->Size = System::Drawing::Size(67, 20);
			this->butLookUp->TabIndex = 11;
			this->butLookUp->Text = "Look up...";
			this->butLookUp->Click += gcnew System::EventHandler(this, &prjltCreateKeyLight::butLookUp_Click);
			// 
			// label_attachnode
			// 
			this->label_attachnode->Location = System::Drawing::Point(16, 24);
			this->label_attachnode->Name = "label_attachnode";
			this->label_attachnode->Size = System::Drawing::Size(80, 27);
			this->label_attachnode->TabIndex = 9;
			this->label_attachnode->Text = "Attachment Node:";
			// 
			// buttonOk
			// 
			this->buttonOk->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonOk->Location = System::Drawing::Point(88, 320);
			this->buttonOk->Name = "buttonOk";
			this->buttonOk->Size = System::Drawing::Size(88, 24);
			this->buttonOk->TabIndex = 7;
			this->buttonOk->Text = "Create Light";
			this->buttonOk->Click += gcnew System::EventHandler(this, &prjltCreateKeyLight::buttonOk_Click);
			// 
			// button2
			// 
			this->button2->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button2->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button2->Location = System::Drawing::Point(200, 320);
			this->button2->Name = "button2";
			this->button2->Size = System::Drawing::Size(72, 24);
			this->button2->TabIndex = 8;
			this->button2->Text = "Cancel";
			// 
			// comboBox_lightSet
			// 
			this->comboBox_lightSet->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->comboBox_lightSet->Location = System::Drawing::Point(88, 96);
			this->comboBox_lightSet->Name = "comboBox_lightSet";
			this->comboBox_lightSet->Size = System::Drawing::Size(256, 21);
			this->comboBox_lightSet->TabIndex = 3;
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 96);
			this->label1->Name = "label1";
			this->label1->Size = System::Drawing::Size(80, 28);
			this->label1->TabIndex = 15;
			this->label1->Text = "Light Set:";
			// 
			// groupBoxTarget
			// 
			this->groupBoxTarget->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupBoxTarget->Controls->Add(this->label3);
			this->groupBoxTarget->Controls->Add(this->vector3Edit_TargetOffset);
			this->groupBoxTarget->Controls->Add(this->textAttachName);
			this->groupBoxTarget->Controls->Add(this->butLookUp);
			this->groupBoxTarget->Controls->Add(this->label_attachnode);
			this->groupBoxTarget->Location = System::Drawing::Point(8, 136);
			this->groupBoxTarget->Name = "groupBoxTarget";
			this->groupBoxTarget->Size = System::Drawing::Size(360, 104);
			this->groupBoxTarget->TabIndex = 17;
			this->groupBoxTarget->TabStop = false;
			this->groupBoxTarget->Text = "Target Attachment";
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(16, 64);
			this->label3->Name = "label3";
			this->label3->Size = System::Drawing::Size(72, 16);
			this->label3->TabIndex = 13;
			this->label3->Text = "World Offset:";
			// 
			// vector3Edit_TargetOffset
			// 
			this->vector3Edit_TargetOffset->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->vector3Edit_TargetOffset->Location = System::Drawing::Point(96, 64);
			this->vector3Edit_TargetOffset->Name = "vector3Edit_TargetOffset";
			this->vector3Edit_TargetOffset->Precision = (System::Int16)2;
			this->vector3Edit_TargetOffset->Size = System::Drawing::Size(248, 24);
			this->vector3Edit_TargetOffset->TabIndex = 5;
			// 
			// groupBoxPosition
			// 
			this->groupBoxPosition->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupBoxPosition->Controls->Add(this->label4);
			this->groupBoxPosition->Controls->Add(this->vector3Edit_PositionOffset);
			this->groupBoxPosition->Location = System::Drawing::Point(8, 240);
			this->groupBoxPosition->Name = "groupBoxPosition";
			this->groupBoxPosition->Size = System::Drawing::Size(360, 64);
			this->groupBoxPosition->TabIndex = 18;
			this->groupBoxPosition->TabStop = false;
			this->groupBoxPosition->Text = "Position Attachment (always to \"root\")";
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(16, 24);
			this->label4->Name = "label4";
			this->label4->Size = System::Drawing::Size(72, 16);
			this->label4->TabIndex = 14;
			this->label4->Text = "World Offset:";
			// 
			// vector3Edit_PositionOffset
			// 
			this->vector3Edit_PositionOffset->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->vector3Edit_PositionOffset->Location = System::Drawing::Point(96, 24);
			this->vector3Edit_PositionOffset->Name = "vector3Edit_PositionOffset";
			this->vector3Edit_PositionOffset->Precision = (System::Int16)2;
			this->vector3Edit_PositionOffset->Size = System::Drawing::Size(248, 24);
			this->vector3Edit_PositionOffset->TabIndex = 6;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 16);
			this->label2->Name = "label2";
			this->label2->Size = System::Drawing::Size(112, 28);
			this->label2->TabIndex = 19;
			this->label2->Text = "Name for new light:";
			// 
			// textBox_Name
			// 
			this->textBox_Name->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textBox_Name->Location = System::Drawing::Point(136, 16);
			this->textBox_Name->Name = "textBox_Name";
			this->textBox_Name->Size = System::Drawing::Size(208, 20);
			this->textBox_Name->TabIndex = 1;
			this->textBox_Name->Text = "";
			// 
			// prjltCreateKeyLight
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(376, 358);
			this->Controls->Add(this->textBox_Name);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->groupBoxPosition);
			this->Controls->Add(this->groupBoxTarget);
			this->Controls->Add(this->comboBox_lightSet);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->buttonOk);
			this->Controls->Add(this->comboBox_attachobject);
			this->Controls->Add(this->label_attachobject);
			this->Name = "prjltCreateKeyLight";
			this->Text = "Create Key Projected Light";
			this->groupBoxTarget->ResumeLayout(false);
			this->groupBoxPosition->ResumeLayout(false);
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

		void GetReferenceList(std::vector<std::string> &o_RefNames)
		{
			nameObject* pNameObj = this->GetNameObject();
			if (pNameObj)
			{
				//mnmObject *pObj = dynamic_cast<mnmObject*>(pNameObj);
				mnmObject *pObj = dynamic_cast<mnmObject*>(pNameObj);
				if (pObj)
					pObj->GetReferenceList(o_RefNames);
			}
		}

private: System::Void buttonOk_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (this->comboBox_attachobject->Text->Length > 0)
			 {
				 this->DialogResult = ::DialogResult::OK;
				 this->Close();
			 }
			 else
			 {
				 MessageBox::Show("Choose an object to which to attach the key light");
			 }
		 }

private: System::Void butLookUp_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
				std::vector<std::string> ref_names;
				this->GetReferenceList(ref_names);

				StudioFramework::tmlnRefNameForm ^dialog = gcnew StudioFramework::tmlnRefNameForm(ref_names);
				if (dialog->ShowDialog() == ::DialogResult::OK)
				{
					this->textAttachName->Text = gcnew System::String(ref_names[dialog->GetSelected()].c_str());
				}

				delete dialog;
		 }

private: System::Void comboBox_attachobject_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			this->textAttachName->Clear();
		 }

};
}
#endif // _MANAGED

