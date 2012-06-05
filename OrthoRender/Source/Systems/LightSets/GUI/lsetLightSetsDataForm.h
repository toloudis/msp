/********************************************************************************************\
**  ltstLightSetsDataForm.h
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifndef LTST_LIGHTSETMGR_HPP
#include "Support/ltst/ltstLightSetMgr.hpp"
#endif
#ifndef LSET_OPERATIONS_HPP
#include "Systems/LightSets/Undo/lsetOperations.hpp"
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
#ifndef DBG_LOG_HPP
#include "Core/Dbg/dbgLog.hpp"
#endif

#include "Systems/Common/GUI/cmmEnterNameForm.h"
#include "Systems/LightSets/GUI/lsetLightSetAmbientForm.h"
#include "Systems/LightSets/GUI/lsetLightSetObjectsForm.h"

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
namespace SystemLightSets
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
	}

	/// <summary>
	/// Summary for ltstLightSetsDataForm
	///
	/// WARNING: If you change the name of this class, yada yada...
	/// </summary>
	public ref class ltstLightSetsDataForm : public System::Windows::Forms::Form
	{
	public:
		static ltstLightSetsDataForm^ FormInstance = nullptr;

		ltstLightSetsDataForm()
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
			// of old light sets
			ArrayList^ compressed = gcnew ArrayList();
			for (int i=0; i<treeViewSets->Nodes->Count; i++)
			{
				TreeNode ^node = treeViewSets->Nodes[i];
				if ((node->Nodes->Count > 0) && (!node->IsExpanded))
					compressed->Add(node->Text);
			}

			//treeViewSets->Controls->Clear();
			tmaManagedControlUtil::DisposeOfControls(treeViewSets->Controls);

			populate_tree_view();
			buttonDeleteLightSet->Enabled = false;
			buttonRenameLightSet->Enabled = false;

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

		void populate_tree_view()
		{
			treeViewSets->Nodes->Clear();

			// Unassigned lights
			TreeNode ^unassigned = treeViewSets->Nodes->Add(gcnew System::String("<unassigned>"));
			populate_set_node(unassigned, nameString());
			unassigned->Expand();

			// Light sets
			std::vector<nameString> set_names;
			ltstLightSetMgr::GetLightSetNames(set_names);
			const int num_set_names = set_names.size();
			for (int i=0; i<num_set_names; ++i)
			{
				nameString &name = set_names[i];
				TreeNode ^node = treeViewSets->Nodes->Add(gcnew System::String(name.GetString().c_str()));
				node->Tag = (name.GetUID());
				//DBG_LOG1("Name uid: %d", name.GetUID());
				populate_set_node(node, set_names[i]);
				node->Expand();
			}
		}

		void populate_set_node(TreeNode ^i_Node, const nameString &i_SetName)
		{
			std::vector<nameString> light_names;
			ltstLightSetMgr::GetLightsInSet(i_SetName, light_names);
			const int num_light_names = light_names.size();
			for (int i=0; i<num_light_names; ++i)
			{
				nameString &name = light_names[i];
				TreeNode ^node = i_Node->Nodes->Add(gcnew System::String(name.GetString().c_str()));
				node->Tag = (name.GetUID());
			}
		}

		TabPage^ GetTabPage()
		{
			return tabPage_lightset;
		}

	protected:
		~ltstLightSetsDataForm()
		{
			// clear instance
			if (ltstLightSetsDataForm::FormInstance == this)
				ltstLightSetsDataForm::FormInstance = nullptr;

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
	private: System::Windows::Forms::Button ^  buttonDeleteLightSet;
	private: System::Windows::Forms::Button ^  buttonCreateLightSet;
	private: System::Windows::Forms::TreeView ^  treeViewSets;
	private: System::Windows::Forms::Button ^  buttonAssignObjects;
	private: System::Windows::Forms::Button ^  buttonAmbient;

	private: tmaDialogMemory^ m_pMemory;
private: System::Windows::Forms::TabControl ^  tabControl_lightset;
private: System::Windows::Forms::TabPage ^  tabPage_lightset;
private: System::Windows::Forms::Button ^  buttonRenameLightSet;
	private: bool m_bSelfEdit;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->buttonDeleteLightSet = gcnew System::Windows::Forms::Button();
			this->buttonCreateLightSet = gcnew System::Windows::Forms::Button();
			this->treeViewSets = gcnew System::Windows::Forms::TreeView();
			this->buttonAssignObjects = gcnew System::Windows::Forms::Button();
			this->buttonAmbient = gcnew System::Windows::Forms::Button();
			this->tabControl_lightset = gcnew System::Windows::Forms::TabControl();
			this->tabPage_lightset = gcnew System::Windows::Forms::TabPage();
			this->buttonRenameLightSet = gcnew System::Windows::Forms::Button();
			this->tabControl_lightset->SuspendLayout();
			this->tabPage_lightset->SuspendLayout();
			this->SuspendLayout();
			// 
			// buttonDeleteLightSet
			// 
			this->buttonDeleteLightSet->Location = System::Drawing::Point(128, 8);
			this->buttonDeleteLightSet->Name = "buttonDeleteLightSet";
			this->buttonDeleteLightSet->Size = System::Drawing::Size(96, 24);
			this->buttonDeleteLightSet->TabIndex = 6;
			this->buttonDeleteLightSet->Text = "Delete Light Set";
			this->buttonDeleteLightSet->Click += gcnew System::EventHandler(this, &ltstLightSetsDataForm::buttonDeleteLightSet_Click);
			// 
			// buttonCreateLightSet
			// 
			this->buttonCreateLightSet->Location = System::Drawing::Point(8, 8);
			this->buttonCreateLightSet->Name = "buttonCreateLightSet";
			this->buttonCreateLightSet->Size = System::Drawing::Size(104, 24);
			this->buttonCreateLightSet->TabIndex = 5;
			this->buttonCreateLightSet->Text = "Create Light Set";
			this->buttonCreateLightSet->Click += gcnew System::EventHandler(this, &ltstLightSetsDataForm::buttonCreateLightSet_Click);
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
			this->treeViewSets->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &ltstLightSetsDataForm::treeViewSets_KeyDown);
			this->treeViewSets->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &ltstLightSetsDataForm::treeViewSets_DragOver);
			this->treeViewSets->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &ltstLightSetsDataForm::treeViewSets_AfterSelect);
			this->treeViewSets->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &ltstLightSetsDataForm::treeViewSets_DragEnter);
			this->treeViewSets->ItemDrag += gcnew System::Windows::Forms::ItemDragEventHandler(this, &ltstLightSetsDataForm::treeViewSets_ItemDrag);
			this->treeViewSets->DragLeave += gcnew System::EventHandler(this, &ltstLightSetsDataForm::treeViewSets_DragLeave);
			this->treeViewSets->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &ltstLightSetsDataForm::treeViewSets_DragDrop);
			// 
			// buttonAssignObjects
			// 
			this->buttonAssignObjects->Location = System::Drawing::Point(8, 40);
			this->buttonAssignObjects->Name = "buttonAssignObjects";
			this->buttonAssignObjects->Size = System::Drawing::Size(104, 24);
			this->buttonAssignObjects->TabIndex = 7;
			this->buttonAssignObjects->Text = "Assign Objects...";
			this->buttonAssignObjects->Click += gcnew System::EventHandler(this, &ltstLightSetsDataForm::buttonAssignObjects_Click);
			// 
			// buttonAmbient
			// 
			this->buttonAmbient->Location = System::Drawing::Point(128, 40);
			this->buttonAmbient->Name = "buttonAmbient";
			this->buttonAmbient->Size = System::Drawing::Size(88, 24);
			this->buttonAmbient->TabIndex = 8;
			this->buttonAmbient->Text = "Ambient Light";
			this->buttonAmbient->Click += gcnew System::EventHandler(this, &ltstLightSetsDataForm::buttonAmbient_Click);
			// 
			// tabControl_lightset
			// 
			this->tabControl_lightset->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_lightset->Controls->Add(this->tabPage_lightset);
			this->tabControl_lightset->Location = System::Drawing::Point(8, 8);
			this->tabControl_lightset->Name = "tabControl_lightset";
			this->tabControl_lightset->SelectedIndex = 0;
			this->tabControl_lightset->Size = System::Drawing::Size(360, 296);
			this->tabControl_lightset->TabIndex = 9;
			// 
			// tabPage_lightset
			// 
			this->tabPage_lightset->Controls->Add(this->buttonRenameLightSet);
			this->tabPage_lightset->Controls->Add(this->buttonCreateLightSet);
			this->tabPage_lightset->Controls->Add(this->treeViewSets);
			this->tabPage_lightset->Controls->Add(this->buttonDeleteLightSet);
			this->tabPage_lightset->Controls->Add(this->buttonAssignObjects);
			this->tabPage_lightset->Controls->Add(this->buttonAmbient);
			this->tabPage_lightset->Location = System::Drawing::Point(4, 22);
			this->tabPage_lightset->Name = "tabPage_lightset";
			this->tabPage_lightset->Size = System::Drawing::Size(352, 270);
			this->tabPage_lightset->TabIndex = 0;
			this->tabPage_lightset->Text = "Light Set";
			this->tabPage_lightset->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &ltstLightSetsDataForm::tabPage_lightset_KeyDown);
			// 
			// buttonRenameLightSet
			// 
			this->buttonRenameLightSet->Location = System::Drawing::Point(240, 8);
			this->buttonRenameLightSet->Name = "buttonRenameLightSet";
			this->buttonRenameLightSet->Size = System::Drawing::Size(104, 24);
			this->buttonRenameLightSet->TabIndex = 9;
			this->buttonRenameLightSet->Text = "Rename Light Set";
			this->buttonRenameLightSet->Click += gcnew System::EventHandler(this, &ltstLightSetsDataForm::buttonRenameLightSet_Click);
			// 
			// ltstLightSetsDataForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(376, 310);
			this->Controls->Add(this->tabControl_lightset);
			this->Name = "ltstLightSetsDataForm";
			this->ShowInTaskbar = false;
			this->Text = "Light Sets";
			this->tabControl_lightset->ResumeLayout(false);
			this->tabPage_lightset->ResumeLayout(false);
			this->ResumeLayout(false);

		}

		//

private: System::Void buttonCreateLightSet_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			cmmEnterNameForm ^dialog = gcnew cmmEnterNameForm( gcnew System::String("Light Set Name"), gcnew System::String("") );
			dialog->SetNameCheckFunction( &ltstLightSetMgr::IsValidLightSetName );
			if (dialog->ShowDialog() == ::DialogResult::OK)
			{
				std::string str;
				tmaManagedStringUtils::ManagedStringToStdString(dialog->GetName(), str);
				nameString name(str);
				lsetOperations::CreateLightSet(name);
			}
			delete dialog;
		}

private: System::Void buttonDeleteLightSet_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			delete_lightset_if_possible();
		 }

private: System::Void buttonRenameLightSet_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			// See if root node is selected
			if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
			{
				// Can't rename "<unassigned>" light set (index==0)
				if (treeViewSets->SelectedNode->Index > 0)
				{
					nameString name = get_name_from_node(treeViewSets->SelectedNode);

					cmmEnterNameForm ^dialog = gcnew cmmEnterNameForm( gcnew System::String("Light Set Name"), 
																		gcnew System::String(name.GetString().c_str()) );
					dialog->SetNameCheckFunction( &ltstLightSetMgr::IsValidLightSetName );
					if (dialog->ShowDialog() == ::DialogResult::OK)
					{
						std::string str;
						tmaManagedStringUtils::ManagedStringToStdString(dialog->GetName(), str);
						lsetOperations::RenameLightSet(name, str);

						this->Update();
					}
				}
			}
		 }

private: System::Void buttonAssignObjects_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 // See if root node is selected
			 if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
			 {
				 // Can't assign to "<unassigned>" light set (index==0)
				 if (treeViewSets->SelectedNode->Index > 0)
				 {
					nameString name = get_name_from_node(treeViewSets->SelectedNode);
					lsetLightSetObjectsForm ^dialog = gcnew lsetLightSetObjectsForm(name);
					dialog->ShowDialog();
					delete dialog;
			 	 }
			 }
		 }

private: System::Void buttonAmbient_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 // See if root node is selected
			 if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
			 {
				 // use empty name for global ambient light
				 nameString name;
				 if (treeViewSets->SelectedNode->Index > 0)
				 {
					 name = get_name_from_node(treeViewSets->SelectedNode);
			 	 }
				 lsetLightSetAmbientForm ^dialog = gcnew lsetLightSetAmbientForm(name);
				 dialog->ShowDialog();
				delete dialog;
			 }
		 }

private: System::Void treeViewSets_AfterSelect(System::Object ^  sender, System::Windows::Forms::TreeViewEventArgs ^  e)
		 {
			 if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
			 {
				 buttonAmbient->Enabled = true;

				 // Selected a root node.
				 bool enabled = (treeViewSets->SelectedNode->Index > 0);
				 buttonDeleteLightSet->Enabled = enabled;
				 buttonRenameLightSet->Enabled = enabled;
				 buttonAssignObjects->Enabled = enabled;
			 }
			 else
			 {
				 buttonAmbient->Enabled = false;
				 buttonDeleteLightSet->Enabled = false;
				 buttonRenameLightSet->Enabled = false;
				 buttonAssignObjects->Enabled = false;

				 if (treeViewSets->SelectedNode)
				 {
					// Select the light object in the 3d scene also
					nameString light_name = get_name_from_node(treeViewSets->SelectedNode);
					ltstLightSetMgr::SelectLight(light_name);
				 }
			 }
		 }

private: System::Void treeViewSets_ItemDrag(System::Object ^  sender, System::Windows::Forms::ItemDragEventArgs ^  e)
		 {
			 TreeNode ^drag_node = safe_cast<TreeNode^>( e->Item );
			 if (drag_node)
			 {
				// Make sure dragged node is a light (not root node)
				if (drag_node->Parent != nullptr)
				{
					//DBG_LOG0("Item Drag");
					treeViewSets->DoDragDrop( drag_node, DragDropEffects::Move );
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
					nameString light_name = get_name_from_node(drag_node);

					TreeNode ^set_node = (drop_node->Parent == nullptr) ? drop_node : drop_node->Parent;
					if (set_node->Index == 0)
					{
						// Dragging light into "<unassigned>" light set
						// means remove from old light set.
						TreeNode ^old_parent = drag_node->Parent;
						nameString set_name = get_name_from_node(old_parent);
						lsetOperations::RemoveLightFromSet(set_name, light_name);
					}
					else
					{
						nameString set_name = get_name_from_node(set_node);
						lsetOperations::AddLightToSet(set_name, light_name);
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


private: System::Void tabPage_lightset_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
		 {
		 }

private: System::Void treeViewSets_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
		 {
			 //	if delete key is pressed and it is okay for the user to delete this object
			 //
			 if ((e->KeyCode == Keys::Delete) && (buttonDeleteLightSet->Enabled))
			 {
				delete_lightset_if_possible();
			 }
		 }

private: void delete_lightset_if_possible()
		 {
			// See if root node is selected
			if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
			{
				// Can't delete "<unassigned>" light set (index==0)
				if (treeViewSets->SelectedNode->Index > 0)
				{
					nameString name = get_name_from_node(treeViewSets->SelectedNode);
					lsetOperations::DeleteLightSet(name);
				}
			}
		 }

};
}

#endif // _MANAGED
