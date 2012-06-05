/********************************************************************************************\
**  envtEnvironmentsDataForm.h
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifndef EVMT_ENVIRONMENTMGR_HPP
#include "evmtEnvironmentMgr.hpp"
#endif
#ifndef ENVT_OPERATIONS_HPP
#include "envtOperations.hpp"
#endif

#ifndef TMA_DIALOGMEMORY_HPP
#include "tmaDialogMemory.hpp"
#endif
#ifndef TMA_MANAGEDCONTROLUTIL_HPP
#include "tmaManagedControlUtil.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "tmaManagedConversionUtil.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "dbgLog.hpp"
#endif

#include "envtEnvironmentPrtyForm.h"
#include "cmmEnterNameForm.h"
#include "envtEnvironmentObjectsForm.h"

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
namespace SystemEnvironments
{
	namespace
	{
		nameString get_name_from_node(TreeNode ^i_Node)
		{
//			if (i_Node == 0) return nameString();

			nameUID uid = Convert::ToInt32(i_Node->Tag);
			//DBG_LOG1("Name uid: %d", uid);
			std::string str;
			tmaManagedStringUtils::ManagedStringToStdString(i_Node->Text, str);
			return nameString(str, uid);
		}
	}

	/// <summary>
	/// Summary for evmtEnvironmentsDataForm
	///
	/// WARNING: If you change the name of this class, yada yada...
	/// </summary>
	public ref class evmtEnvironmentsDataForm : public System::Windows::Forms::Form
	{
	public:
		static evmtEnvironmentsDataForm^ FormInstance = nullptr;

		evmtEnvironmentsDataForm()
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

			//this->treeViewSets->Controls->Clear();
			tmaManagedControlUtil::DisposeOfControls(treeViewSets->Controls);

			populate_controls();
			buttonDeleteEnvironment->Enabled = false;

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
			treeViewSets->Nodes->Clear();

			// Unassigned objects
//			TreeNode ^unassigned = treeViewSets->Nodes->Add(gcnew System::String("<unassigned>"));
//			unassigned->BackColor = System::Drawing::Color::LightGray;
//			populate_set_node(unassigned, nameString());
//			unassigned->Expand();

			// Environments
			std::vector<nameString> set_names;
			evmtEnvironmentMgr::GetEnvironmentNames(set_names);
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
			}
		}

		void populate_set_node(TreeNode ^i_Node, const nameString &i_SetName)
		{
			std::vector<nameString> object_names;
			evmtEnvironmentMgr::GetObjectsInSet(i_SetName, object_names);
			const int num_object_names = object_names.size();
			for (int i=0; i<num_object_names; ++i)
			{
				nameString &name = object_names[i];
				TreeNode ^node = i_Node->Nodes->Add(gcnew System::String(name.GetString().c_str()));
				node->Tag = name.GetUID();
			}
		}

		TabPage^ GetTabPage()
		{
			return tabPage_environment;
		}

	protected:
		~evmtEnvironmentsDataForm()
		{
			// clear instance
			if (evmtEnvironmentsDataForm::FormInstance == this)
				evmtEnvironmentsDataForm::FormInstance = nullptr;

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
	private: System::Windows::Forms::Button ^  buttonDeleteEnvironment;
	private: System::Windows::Forms::Button ^  buttonCreateEnvironment;
	private: System::Windows::Forms::TreeView ^  treeViewSets;
	private: System::Windows::Forms::Button ^  buttonAssignObjects;
	private: System::Windows::Forms::TabControl ^  tabControl_environment;
	private: System::Windows::Forms::TabPage ^  tabPage_environment;
	private: System::Windows::Forms::Button ^  buttonEdit;

	private: tmaDialogMemory^ m_pMemory;
	private: bool m_bDisableNotify;
	private: bool m_bSelfEdit;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->buttonDeleteEnvironment = gcnew System::Windows::Forms::Button();
			this->buttonCreateEnvironment = gcnew System::Windows::Forms::Button();
			this->treeViewSets = gcnew System::Windows::Forms::TreeView();
			this->buttonAssignObjects = gcnew System::Windows::Forms::Button();
			this->buttonEdit = gcnew System::Windows::Forms::Button();
			this->tabControl_environment = gcnew System::Windows::Forms::TabControl();
			this->tabPage_environment = gcnew System::Windows::Forms::TabPage();
			this->tabControl_environment->SuspendLayout();
			this->tabPage_environment->SuspendLayout();
			this->SuspendLayout();
			// 
			// buttonDeleteEnvironment
			// 
			this->buttonDeleteEnvironment->Location = System::Drawing::Point(128, 8);
			this->buttonDeleteEnvironment->Name = "buttonDeleteEnvironment";
			this->buttonDeleteEnvironment->Size = System::Drawing::Size(104, 24);
			this->buttonDeleteEnvironment->TabIndex = 6;
			this->buttonDeleteEnvironment->Text = "Delete";
			this->buttonDeleteEnvironment->Click += gcnew System::EventHandler(this, &evmtEnvironmentsDataForm::buttonDeleteEnvironment_Click);
			// 
			// buttonCreateEnvironment
			// 
			this->buttonCreateEnvironment->Location = System::Drawing::Point(8, 8);
			this->buttonCreateEnvironment->Name = "buttonCreateEnvironment";
			this->buttonCreateEnvironment->Size = System::Drawing::Size(104, 24);
			this->buttonCreateEnvironment->TabIndex = 5;
			this->buttonCreateEnvironment->Text = "Create";
			this->buttonCreateEnvironment->Click += gcnew System::EventHandler(this, &evmtEnvironmentsDataForm::buttonCreateEnvironment_Click);
			// 
			// treeViewSets
			// 
			this->treeViewSets->AllowDrop = true;
			this->treeViewSets->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->treeViewSets->HotTracking = true;
			this->treeViewSets->ImageIndex = -1;
			this->treeViewSets->Location = System::Drawing::Point(8, 72);
			this->treeViewSets->Name = "treeViewSets";
			this->treeViewSets->SelectedImageIndex = -1;
			this->treeViewSets->Size = System::Drawing::Size(336, 192);
			this->treeViewSets->TabIndex = 4;
			this->treeViewSets->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &evmtEnvironmentsDataForm::treeViewSets_KeyDown);
			this->treeViewSets->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &evmtEnvironmentsDataForm::treeViewSets_DragOver);
			this->treeViewSets->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &evmtEnvironmentsDataForm::treeViewSets_AfterSelect);
			this->treeViewSets->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &evmtEnvironmentsDataForm::treeViewSets_DragEnter);
			this->treeViewSets->ItemDrag += gcnew System::Windows::Forms::ItemDragEventHandler(this, &evmtEnvironmentsDataForm::treeViewSets_ItemDrag);
			this->treeViewSets->DragLeave += gcnew System::EventHandler(this, &evmtEnvironmentsDataForm::treeViewSets_DragLeave);
			this->treeViewSets->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &evmtEnvironmentsDataForm::treeViewSets_DragDrop);
			// 
			// buttonAssignObjects
			// 
			this->buttonAssignObjects->Location = System::Drawing::Point(8, 40);
			this->buttonAssignObjects->Name = "buttonAssignObjects";
			this->buttonAssignObjects->Size = System::Drawing::Size(104, 24);
			this->buttonAssignObjects->TabIndex = 7;
			this->buttonAssignObjects->Text = "Assign Objects...";
			this->buttonAssignObjects->Click += gcnew System::EventHandler(this, &evmtEnvironmentsDataForm::buttonAssignObjects_Click);
			// 
			// buttonEdit
			// 
			this->buttonEdit->Location = System::Drawing::Point(128, 40);
			this->buttonEdit->Name = "buttonEdit";
			this->buttonEdit->Size = System::Drawing::Size(104, 24);
			this->buttonEdit->TabIndex = 8;
			this->buttonEdit->Text = "Edit";
			this->buttonEdit->Click += gcnew System::EventHandler(this, &evmtEnvironmentsDataForm::buttonEdit_Click);
			// 
			// tabControl_environment
			// 
			this->tabControl_environment->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_environment->Controls->Add(this->tabPage_environment);
			this->tabControl_environment->Location = System::Drawing::Point(8, 8);
			this->tabControl_environment->Name = "tabControl_environment";
			this->tabControl_environment->SelectedIndex = 0;
			this->tabControl_environment->Size = System::Drawing::Size(360, 296);
			this->tabControl_environment->TabIndex = 9;
			// 
			// tabPage_environment
			// 
			this->tabPage_environment->Controls->Add(this->buttonCreateEnvironment);
			this->tabPage_environment->Controls->Add(this->treeViewSets);
			this->tabPage_environment->Controls->Add(this->buttonDeleteEnvironment);
			this->tabPage_environment->Controls->Add(this->buttonAssignObjects);
			this->tabPage_environment->Controls->Add(this->buttonEdit);
			this->tabPage_environment->Location = System::Drawing::Point(4, 22);
			this->tabPage_environment->Name = "tabPage_environment";
			this->tabPage_environment->Size = System::Drawing::Size(352, 270);
			this->tabPage_environment->TabIndex = 0;
			this->tabPage_environment->Text = "Environment";
			this->tabPage_environment->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &evmtEnvironmentsDataForm::tabPage_environment_KeyDown);
			// 
			// evmtEnvironmentsDataForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(376, 310);
			this->Controls->Add(this->tabControl_environment);
			this->Name = "evmtEnvironmentsDataForm";
			this->ShowInTaskbar = false;
			this->Text = "Environments";
			this->tabControl_environment->ResumeLayout(false);
			this->tabPage_environment->ResumeLayout(false);
			this->ResumeLayout(false);

		}

		//

private: System::Void buttonCreateEnvironment_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			cmmEnterNameForm ^dialog = gcnew cmmEnterNameForm(gcnew System::String("Environment Name"), gcnew System::String(""));
			dialog->SetNameCheckFunction(&evmtEnvironmentMgr::IsValidEnvironmentName);
			if (dialog->ShowDialog() == ::DialogResult::OK)
			{
				std::string str;
				tmaManagedStringUtils::ManagedStringToStdString(dialog->GetName(), str);
				nameString name(str);
//				envtOperations::CreateEnvironment(name);
			}
			delete dialog;
		}

private: System::Void buttonDeleteEnvironment_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			delete_environment_if_possible();
		 }

private: System::Void buttonAssignObjects_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 // See if root node is selected
			 if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
			 {
				 // Can't assign to "<unassigned>" environment (index==0)
//				 if (treeViewSets->SelectedNode->Index > 0)
				 {
					//nameString name = get_name_from_node(treeViewSets->SelectedNode);
					envtEnvironmentObjectsForm ^dialog = gcnew envtEnvironmentObjectsForm();//name);
					dialog->ShowDialog();
					delete dialog;
			 	 }
			 }
		 }

private: System::Void buttonEdit_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
             // See if root node is selected
			 if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
			 {
				 // use empty name for global ambient light
				 nameString name;
//				 if (treeViewSets->SelectedNode->Index > 0)
				 {
					 name = get_name_from_node(treeViewSets->SelectedNode);
			 	 }
				 envtEnvironmentPrtyForm ^dialog = gcnew envtEnvironmentPrtyForm(name);
				 dialog->ShowDialog();
				 delete dialog;
			 }
		 }

private: System::Void treeViewSets_AfterSelect(System::Object ^  sender, System::Windows::Forms::TreeViewEventArgs ^  e)
		 {
			 if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
			 {

				 // Selected a root node.
				 bool enabled = (treeViewSets->SelectedNode->Index > 0);
				 buttonDeleteEnvironment->Enabled = enabled;
				 buttonAssignObjects->Enabled = true;//enabled;
				 buttonEdit->Enabled = true;//enabled;
			 }
			 else
			 {
				 buttonEdit->Enabled = false;
				 buttonDeleteEnvironment->Enabled = false;
				 buttonAssignObjects->Enabled = false;

				 if (treeViewSets->SelectedNode)
				 {
					// Select the light object in the 3d scene also
					nameString light_name = get_name_from_node(treeViewSets->SelectedNode);
//					evmtEnvironmentMgr::SelectObject(light_name);
				 }
			 }
		 }

private: System::Void treeViewSets_ItemDrag(System::Object ^  sender, System::Windows::Forms::ItemDragEventArgs ^  e)
		 {
			 TreeNode ^drag_node = safe_cast<TreeNode^>( e->Item );
			 if (drag_node)
			 {
				// Make sure dragged node is a object (not root node)
				if (drag_node->Parent != nullptr)
				{
					//DBG_LOG0("Item Drag");
					this->treeViewSets->DoDragDrop( drag_node, DragDropEffects::Move );
				}
			}
		 }

private: System::Void treeViewSets_DragDrop(System::Object ^  sender, System::Windows::Forms::DragEventArgs ^  e)
		 {
			 TreeNode ^drag_node = safe_cast<TreeNode^>( e->Data->GetData(System::Windows::Forms::TreeNode::typeid) );
			 if (drag_node != nullptr)
			 {
				 TreeNode ^drop_node = treeViewSets->GetNodeAt(
					 treeViewSets->PointToClient(Point(e->X, e->Y)));
				 if (drop_node)
				 {
					nameString object_name = get_name_from_node(drag_node);

					TreeNode ^set_node = (drop_node->Parent == nullptr) ? drop_node : drop_node->Parent;
/*					if (set_node->Index == 0)
					{
						// Dragging object into "<unassigned>" object set
						// means remove from old object set.
						TreeNode ^old_parent = drag_node->Parent;
						nameString set_name = get_name_from_node(old_parent);
						envtOperations::RemoveObjectFromEnvironment(set_name, object_name);
					}
					else
*/					{
						nameString set_name = get_name_from_node(set_node);
//						envtOperations::AddObjectToEnvironment(set_name, object_name);
					}
				 }
			 }
		 }

private: System::Void treeViewSets_DragOver(System::Object ^  sender, System::Windows::Forms::DragEventArgs ^  e)
		 {
			 if (e->Data->GetDataPresent(System::Windows::Forms::TreeNode::typeid))
			 {	 
				//DBG_LOG0("Have tree node");
				 
				 TreeNode ^drop_node = treeViewSets->GetNodeAt(
					 treeViewSets->PointToClient(Point(e->X, e->Y)));
				 if (drop_node)
				 {
					e->Effect = DragDropEffects::Move;
					treeViewSets->SelectedNode = drop_node;
				 }
				 else
					e->Effect = DragDropEffects::None;
			 }
			 else
			 {
				 e->Effect = DragDropEffects::None;
			 }
		 }

private: System::Void treeViewSets_DragEnter(System::Object ^  sender, System::Windows::Forms::DragEventArgs ^  e)
		 {
		 }

private: System::Void treeViewSets_DragLeave(System::Object ^  sender, System::EventArgs ^  e)
		 {
		 }


private: System::Void tabPage_environment_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
		 {
		 }

private: System::Void treeViewSets_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
		 {
			 //	if delete key is pressed and it is okay for the user to delete this object
			 //
			 if ((e->KeyCode == Keys::Delete) && (buttonDeleteEnvironment->Enabled))
			 {
				delete_environment_if_possible();
			 }
		 }

private: void delete_environment_if_possible()
		 {
			// See if root node is selected
			if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
			{
				// Can't delete "<unassigned>" environment (index==0)
				if (treeViewSets->SelectedNode->Index > 0)
				{
					nameString name = get_name_from_node(treeViewSets->SelectedNode);
//					envtOperations::DeleteEnvironment(name);
				}
			}
		 }

};
}
#endif // _MANAGED
