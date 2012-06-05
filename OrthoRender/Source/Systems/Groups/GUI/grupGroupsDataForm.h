/********************************************************************************************\
**  grpsGroupsDataForm.h
**
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifndef GRPS_GROUPMGR_HPP
#include "Support/grps/grpsGroupMgr.hpp"
#endif
#ifndef GRUP_OPERATIONS_HPP
#include "Systems/Groups/Undo/grupOperations.hpp"
#endif
#ifndef GSUP_TREEVIEWUTIL_HPP
#include "Support/gsup/gsupTreeViewUtil.hpp"
#endif

#ifndef DBG_LOG_HPP
#include "Core/Dbg/dbgLog.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif
#ifndef TMA_MANAGEDCONTROLUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedControlUtil.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
#endif

#include "Systems/Common/GUI/cmmEnterNameForm.h"

#ifdef _MANAGED

//============================================================================
//============================================================================
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace StudioFramework;


//============================================================================
//============================================================================
namespace SystemGroups
{
	namespace
	{
		nameString get_name_from_node(TreeNode ^i_Node)
		{
			if (i_Node == nullptr) return nameString();

			nameUID uid = Convert::ToInt32(i_Node->Tag);
			//DBG_LOG1("Name uid: %d", uid);
			std::string str;
			tmaManagedStringUtils::ManagedStringToStdString(i_Node->Text, str);
			return nameString(str, uid);
		}
		nameString get_name_from_radio(RadioButton ^i_Radio)
		{
			if (i_Radio == nullptr) return nameString();

			nameUID uid = Convert::ToInt32(i_Radio->Tag);
			//DBG_LOG1("Name uid: %d", uid);
			return nameString("", uid);
		}
	}

	/// <summary>
	/// Summary for grpsGroupsDataForm
	///
	/// WARNING: If you change the name of this class, yada yada...
	/// </summary>
	public ref class grpsGroupsDataForm : public System::Windows::Forms::Form
	{
	public:
		static grpsGroupsDataForm^ FormInstance = nullptr;

		grpsGroupsDataForm()
			: m_bSelfEdit(false)
		{
			m_bDisableNotify = true;

			InitializeComponent();

			// Dialog memory remembers size, location, visiblity of dialog 
			m_pMemory = gcnew tmaDialogMemory( this );

			m_bDisableNotify = false;
		}

		// Call this to update dialog to new data
		void Update()
		{
			if (m_bSelfEdit) return;

			m_bDisableNotify = true;

			// Before removing old nodes, get "expanded" states
			// of old object sets
			ArrayList^ compressed = gcnew ArrayList();
			for (int i=0; i<treeViewSets->Nodes->Count; i++)
			{
				TreeNode ^node = treeViewSets->Nodes[i];
				if ((node->Nodes->Count > 0) && (!node->IsExpanded))
					compressed->Add(node->Text);
			}

			populate_controls();

			configure_group_buttons();

			// Default node state is expanded, but if the node was compressed
			// before, collapse it now
			for (int i=0; i<treeViewSets->Nodes->Count; i++)
			{
				TreeNode ^node = treeViewSets->Nodes[i];
				if (compressed->Contains(node->Text))
					node->Collapse();
			}

			m_bDisableNotify = false;
		}

		void populate_controls()
		{
			//	save the selected node
			System::String^ selected_text = nullptr;
			TreeNode^ pSTN = treeViewSets->SelectedNode;
			if (pSTN != nullptr)
			{
				selected_text = System::String::Copy(pSTN->Text);
			}

			// TODO Improve so the list doesn't have to be cleared and completely rebuilt each time
			//
			treeViewSets->BeginUpdate();
			treeViewSets->Sorted = true;

			this->treeViewSets->Controls->Clear();
			tmaManagedControlUtil::DisposeOfControls(treeViewSets->Controls);
			treeViewSets->Nodes->Clear();

			std::vector<nameString> set_names;
			grpsGroupMgr::GetGroupNames(set_names);
			const int num_set_names = set_names.size();
			for (int i=0; i<num_set_names; ++i)
			{
				nameString &name = set_names[i];
				TreeNode ^node = treeViewSets->Nodes->Add(gcnew System::String(name.GetString().c_str()));
				node->BackColor = System::Drawing::Color::LightGray;
				node->Tag = name.GetUID();
				//DBG_LOG1("Name uid: %d", name.GetUID());
				populate_set_node(node, set_names[i]);
				node->Expand();

				create_property_control(i, set_names[i]);
			}

			//gsupTreeViewUtil::SortTreeView(treeViewSets, true, false);

			treeViewSets->EndUpdate();

			//	reselect the node
			if (selected_text != nullptr)
			{
				TreeNode^ pTN = gsupTreeViewUtil::FindTreeNode(treeViewSets->Nodes,selected_text);
				if (pTN != nullptr)
					treeViewSets->SelectedNode = pTN;
			}
		}

		void populate_set_node(TreeNode ^i_Node, const nameString &i_SetName)
		{
			std::vector<nameString> object_names;
			grpsGroupMgr::GetObjectsInGroup(i_SetName, object_names);
			const int num_object_names = object_names.size();
			for (int i=0; i<num_object_names; ++i)
			{
				nameString &name = object_names[i];
				TreeNode ^node = i_Node->Nodes->Add(gcnew System::String(name.GetString().c_str()));
				node->Tag = name.GetUID();
			}
		}

		 void create_property_control(int i_Index, const nameString &name)
		 {
			System::Windows::Forms::Label ^  labelGroupName = gcnew Label();
			System::Windows::Forms::GroupBox ^  groupBox = gcnew GroupBox();

			 int y_loc =  8 + 34*i_Index;
			// 
			// labelGroupName
			// 
			labelGroupName->Location = System::Drawing::Point(8, 8);
			labelGroupName->Size = System::Drawing::Size(96, 24);
			labelGroupName->Text = gcnew System::String(name.GetString().c_str());
			// 
			// groupBox
			// 
			groupBox->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			groupBox->Controls->Add(labelGroupName);
			groupBox->Location = System::Drawing::Point(8, y_loc);
			groupBox->Size = System::Drawing::Size(368, 34);
			groupBox->TabStop = false;
		 }

		TabPage^ GetTabPage()
		{
			return this->tabPage_groups;
		}

	protected:
		~grpsGroupsDataForm()
		{
			// clear instance
			if (grpsGroupsDataForm::FormInstance == this)
				grpsGroupsDataForm::FormInstance = nullptr;

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
	private: bool m_bDisableNotify;
	private: tmaDialogMemory^ m_pMemory;
	private: bool m_bSelfEdit;

private: System::Windows::Forms::Button ^  buttonDeleteGroup;
private: System::Windows::Forms::Button ^  buttonCreateGroup;
private: System::Windows::Forms::TreeView ^  treeViewSets;

private: System::Windows::Forms::TabControl ^  tabControl_groups;
private: System::Windows::Forms::TabPage ^  tabPage_groups;
private: System::Windows::Forms::Button ^  button_renamegroup;
private: System::Windows::Forms::Button ^  button_updategroup;
private: System::Windows::Forms::Button ^  button_expand;
private: System::Windows::Forms::Button ^  button_collapse;


private: System::Windows::Forms::GroupBox ^  groupBox_groupcontrols;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->buttonDeleteGroup = gcnew System::Windows::Forms::Button();
			this->buttonCreateGroup = gcnew System::Windows::Forms::Button();
			this->treeViewSets = gcnew System::Windows::Forms::TreeView();
			this->tabControl_groups = gcnew System::Windows::Forms::TabControl();
			this->tabPage_groups = gcnew System::Windows::Forms::TabPage();
			this->button_collapse = gcnew System::Windows::Forms::Button();
			this->button_expand = gcnew System::Windows::Forms::Button();
			this->groupBox_groupcontrols = gcnew System::Windows::Forms::GroupBox();
			this->button_renamegroup = gcnew System::Windows::Forms::Button();
			this->button_updategroup = gcnew System::Windows::Forms::Button();
			this->tabControl_groups->SuspendLayout();
			this->tabPage_groups->SuspendLayout();
			this->groupBox_groupcontrols->SuspendLayout();
			this->SuspendLayout();
			// 
			// buttonDeleteGroup
			// 
			this->buttonDeleteGroup->Location = System::Drawing::Point(224, 16);
			this->buttonDeleteGroup->Name = "buttonDeleteGroup";
			this->buttonDeleteGroup->Size = System::Drawing::Size(64, 24);
			this->buttonDeleteGroup->TabIndex = 6;
			this->buttonDeleteGroup->Text = "Delete";
			this->buttonDeleteGroup->Click += gcnew System::EventHandler(this, &grpsGroupsDataForm::buttonDeleteGroup_Click);
			// 
			// buttonCreateGroup
			// 
			this->buttonCreateGroup->Location = System::Drawing::Point(8, 16);
			this->buttonCreateGroup->Name = "buttonCreateGroup";
			this->buttonCreateGroup->Size = System::Drawing::Size(64, 24);
			this->buttonCreateGroup->TabIndex = 5;
			this->buttonCreateGroup->Text = "New";
			this->buttonCreateGroup->Click += gcnew System::EventHandler(this, &grpsGroupsDataForm::buttonCreateGroup_Click);
			// 
			// treeViewSets
			// 
			this->treeViewSets->AllowDrop = true;
			this->treeViewSets->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->treeViewSets->FullRowSelect = true;
			this->treeViewSets->HideSelection = false;
			this->treeViewSets->HotTracking = true;
			this->treeViewSets->ImageIndex = -1;
			this->treeViewSets->Indent = 20;
			this->treeViewSets->Location = System::Drawing::Point(8, 64);
			this->treeViewSets->Name = "treeViewSets";
			this->treeViewSets->SelectedImageIndex = -1;
			this->treeViewSets->Size = System::Drawing::Size(296, 184);
			this->treeViewSets->Sorted = true;
			this->treeViewSets->TabIndex = 4;
			this->treeViewSets->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &grpsGroupsDataForm::treeViewSets_KeyDown);
			this->treeViewSets->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &grpsGroupsDataForm::treeViewSets_AfterSelect);
			// 
			// tabControl_groups
			// 
			this->tabControl_groups->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_groups->Controls->Add(this->tabPage_groups);
			this->tabControl_groups->Location = System::Drawing::Point(8, 8);
			this->tabControl_groups->Name = "tabControl_groups";
			this->tabControl_groups->SelectedIndex = 0;
			this->tabControl_groups->Size = System::Drawing::Size(320, 296);
			this->tabControl_groups->TabIndex = 8;
			// 
			// tabPage_groups
			// 
			this->tabPage_groups->Controls->Add(this->button_collapse);
			this->tabPage_groups->Controls->Add(this->button_expand);
			this->tabPage_groups->Controls->Add(this->groupBox_groupcontrols);
			this->tabPage_groups->Controls->Add(this->treeViewSets);
			this->tabPage_groups->Location = System::Drawing::Point(4, 22);
			this->tabPage_groups->Name = "tabPage_groups";
			this->tabPage_groups->Size = System::Drawing::Size(312, 270);
			this->tabPage_groups->TabIndex = 0;
			this->tabPage_groups->Text = "Groups";
			// 
			// button_collapse
			// 
			this->button_collapse->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_collapse->Location = System::Drawing::Point(35, 251);
			this->button_collapse->Name = "button_collapse";
			this->button_collapse->Size = System::Drawing::Size(16, 16);
			this->button_collapse->TabIndex = 12;
			this->button_collapse->Text = "-";
			this->button_collapse->Click += gcnew System::EventHandler(this, &grpsGroupsDataForm::button_collapse_Click);
			// 
			// button_expand
			// 
			this->button_expand->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_expand->Location = System::Drawing::Point(12, 251);
			this->button_expand->Name = "button_expand";
			this->button_expand->Size = System::Drawing::Size(16, 16);
			this->button_expand->TabIndex = 11;
			this->button_expand->Text = "+";
			this->button_expand->Click += gcnew System::EventHandler(this, &grpsGroupsDataForm::button_expand_Click);
			// 
			// groupBox_groupcontrols
			// 
			this->groupBox_groupcontrols->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupBox_groupcontrols->Controls->Add(this->buttonCreateGroup);
			this->groupBox_groupcontrols->Controls->Add(this->button_renamegroup);
			this->groupBox_groupcontrols->Controls->Add(this->button_updategroup);
			this->groupBox_groupcontrols->Controls->Add(this->buttonDeleteGroup);
			this->groupBox_groupcontrols->Location = System::Drawing::Point(8, 8);
			this->groupBox_groupcontrols->Name = "groupBox_groupcontrols";
			this->groupBox_groupcontrols->Size = System::Drawing::Size(296, 48);
			this->groupBox_groupcontrols->TabIndex = 10;
			this->groupBox_groupcontrols->TabStop = false;
			this->groupBox_groupcontrols->Text = "Group/Object Functionality";
			// 
			// button_renamegroup
			// 
			this->button_renamegroup->Location = System::Drawing::Point(152, 16);
			this->button_renamegroup->Name = "button_renamegroup";
			this->button_renamegroup->Size = System::Drawing::Size(64, 24);
			this->button_renamegroup->TabIndex = 8;
			this->button_renamegroup->Text = "Rename";
			this->button_renamegroup->Click += gcnew System::EventHandler(this, &grpsGroupsDataForm::button_renamegroup_Click);
			// 
			// button_updategroup
			// 
			this->button_updategroup->Location = System::Drawing::Point(80, 16);
			this->button_updategroup->Name = "button_updategroup";
			this->button_updategroup->Size = System::Drawing::Size(64, 24);
			this->button_updategroup->TabIndex = 9;
			this->button_updategroup->Text = "Update";
			this->button_updategroup->Click += gcnew System::EventHandler(this, &grpsGroupsDataForm::button_updategroup_Click);
			// 
			// grpsGroupsDataForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(336, 310);
			this->Controls->Add(this->tabControl_groups);
			this->Name = "grpsGroupsDataForm";
			this->ShowInTaskbar = false;
			this->Text = "Groups";
			this->tabControl_groups->ResumeLayout(false);
			this->tabPage_groups->ResumeLayout(false);
			this->groupBox_groupcontrols->ResumeLayout(false);
			this->ResumeLayout(false);

		}

		//
private: System::Void buttonCreateGroup_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			create_group_name(gcnew System::String("Default"));
		}

private: System::Void buttonDeleteGroup_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			delete_item_in_list();
		 }

private: System::Void button_updategroup_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
			{
				update_group(treeViewSets->SelectedNode->Text);
			}
		 }

private: System::Void treeViewSets_AfterSelect(System::Object ^  sender, System::Windows::Forms::TreeViewEventArgs ^  e)
		 {
 			configure_group_buttons();

			if (!m_bDisableNotify)
			{
				// select the objects
				if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
				{
					//
					std::string str;
					tmaManagedStringUtils::ManagedStringToStdString(treeViewSets->SelectedNode->Text, str);
					nameUID uid = Convert::ToInt32(treeViewSets->SelectedNode->Tag);	// keep the same name UID
					nameString group_name(str, uid);

					grpsGroupMgr::SelectGroupObjects(group_name);
				}
			}
		 }

private: System::Void treeViewSets_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
		 {
			 //	if delete key is pressed and it is okay for the user to delete this object
			 //
			 if ((e->KeyCode == Keys::Delete) && (buttonDeleteGroup->Enabled))
			 {
				delete_item_in_list();
			 }
		 }

private: System::Void button_expand_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 treeViewSets->ExpandAll();
		 }

private: System::Void button_collapse_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 treeViewSets->CollapseAll();
		 }

private: System::Void delete_item_in_list()
		 {
			 if (treeViewSets->SelectedNode)
			 {
				nameString nodename = get_name_from_node(treeViewSets->SelectedNode);
				if (treeViewSets->SelectedNode->Parent == nullptr)
				{
					grupOperations::DeleteGroup(nodename);
				}
				else
				{
					nameString groupname = get_name_from_node(treeViewSets->SelectedNode->Parent);
					grupOperations::RemoveObjectFromGroup(groupname,nodename);
				}
			 }
		 }

private: System::Void button_renamegroup_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			// See if root node is selected
			if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
			{
				std::string str;
				tmaManagedStringUtils::ManagedStringToStdString(treeViewSets->SelectedNode->Text, str);
				nameUID uid = Convert::ToInt32(treeViewSets->SelectedNode->Tag);	// keep the same name UID
				nameString name(str, uid);
				edit_group_name(name);
			}
		 }

private: void create_group_name(System::String^ i_pGroupName)
		 {
			 cmmEnterNameForm ^dialog = gcnew cmmEnterNameForm(gcnew System::String("Group Name"), i_pGroupName);
			 dialog->SetNameCheckFunction( &grpsGroupMgr::IsValidGroupName );
			if (dialog->ShowDialog() == ::DialogResult::OK)
			{
				std::string str;
				tmaManagedStringUtils::ManagedStringToStdString(dialog->GetName(), str);
				nameString name(str);
				grupOperations::CreateGroupFromSelected(name);
			}
			delete dialog;
		 }

private: void edit_group_name(nameString& i_GroupName)
		 {
			 cmmEnterNameForm ^dialog = gcnew cmmEnterNameForm( gcnew System::String("Group Name"), 
																gcnew System::String(i_GroupName.GetString().c_str()));
			if (dialog->ShowDialog() == ::DialogResult::OK)
			{
				std::string str;
				tmaManagedStringUtils::ManagedStringToStdString(dialog->GetName(), str);
				nameString new_name(str,i_GroupName.GetUID());

				grupOperations::RenameGroup(i_GroupName, new_name);
			}
			delete dialog;
		 }

private: void update_group(System::String^ i_pGroupName)
		 {
			std::string str;
			tmaManagedStringUtils::ManagedStringToStdString(i_pGroupName, str);
			nameString name(str);
			grupOperations::UpdateGroupFromSelected(name);
		 }

private: void configure_group_buttons()
		 {
			 // only allowed on group names
			bool enabled = (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr));
			this->button_renamegroup->Enabled = enabled;
			this->button_updategroup->Enabled = enabled;

			// allowed on group or object names
			enabled = (treeViewSets->SelectedNode != nullptr);
			this->buttonDeleteGroup->Enabled = enabled;
		}
};
}

#endif // _MANAGED
