/********************************************************************************************\
**  lyerLayersDataForm.h
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifndef LYER_LAYERMGR_HPP
#include "Support/lyer/lyerLayerMgr.hpp"
#endif
#ifndef LYRS_OPERATIONS_HPP
#include "Systems/Layers/Undo/lyrsOperations.hpp"
#endif

#ifndef CMM_DIALOGINTERESTMGR_HPP
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#endif
#ifndef CMM_OBJECTDIALOGUTIL_HPP
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
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
//#define MULTISELECT_TREEVIEW;


//============================================================================
//============================================================================
namespace SystemLayers
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
		nameString get_name_from_checkbox(CheckBox ^i_Radio)
		{
			if (i_Radio == nullptr) return nameString();

			nameUID uid = Convert::ToInt32(i_Radio->Tag);
			//DBG_LOG1("Name uid: %d", uid);
			return nameString("", uid);
		}
	}

	/// <summary>
	/// Summary for lyerLayersDataForm
	///
	/// WARNING: If you change the name of this class, yada yada...
	/// </summary>
	public ref class lyerLayersDataForm : public System::Windows::Forms::Form
	{
	public:
		static lyerLayersDataForm^ FormInstance = nullptr;

		lyerLayersDataForm()
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

			this->treeViewSets->Controls->Clear();
			tmaManagedControlUtil::DisposeOfControls(treeViewSets->Controls);

			populate_controls();

			this->buttonDeleteLayer->Enabled = false;
			this->button_renamelayer->Enabled = false;

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
			//DBG_LOG0("populate_control ---------------------------------");
			treeViewSets->Nodes->Clear();
			//panel1->Controls->Clear();
			tmaManagedControlUtil::DisposeOfControls(panel1->Controls);

			// Unassigned objects
			TreeNode ^unassigned = treeViewSets->Nodes->Add(gcnew System::String("<unassigned>"));
			unassigned->BackColor = System::Drawing::Color::LightGray;
			populate_set_node(unassigned, nameString());
			unassigned->Expand();

			// Light sets
			std::vector<nameString> set_names;
			lyerLayerMgr::GetLayerNames(set_names);
			const int num_set_names = set_names.size();
			for (int i=0; i<num_set_names; ++i)
			{
				nameString &name = set_names[i];
				TreeNode ^node = treeViewSets->Nodes->Add(gcnew System::String(name.GetString().c_str()));
				node->BackColor = System::Drawing::Color::LightGray;
				node->Tag = name.GetUID();
				//DBG_LOG3("%02d (%d) %s", i, name.GetUID(), name.GetString().c_str());
				populate_set_node(node, set_names[i]);
				node->Expand();

				create_property_control(i, set_names[i]);
			}
		}

		void populate_set_node(TreeNode ^i_Node, const nameString &i_SetName)
		{
			std::vector<nameString> object_names;
			lyerLayerMgr::GetObjectsInLayer(i_SetName, object_names);
			const int num_object_names = object_names.size();
			for (int i=0; i<num_object_names; ++i)
			{
				nameString &name = object_names[i];
				//DBG_LOG3("    %02d (%d) %s", i, name.GetUID(), name.GetString().c_str());
				TreeNode ^node = i_Node->Nodes->Add(gcnew System::String(name.GetString().c_str()));
				node->Tag = name.GetUID();
			}
		}

		 void create_property_control(int i_Index, const nameString &name)
		 {
			System::Windows::Forms::Label ^  labelLayerName = gcnew Label();
			System::Windows::Forms::CheckBox ^  checkVisible = gcnew CheckBox();
			System::Windows::Forms::CheckBox ^  checkPickable = gcnew CheckBox();
			System::Windows::Forms::CheckBox ^  checkWire = gcnew CheckBox();
			System::Windows::Forms::CheckBox ^  checkLowRes = gcnew CheckBox();
			System::Windows::Forms::GroupBox ^  groupBox = gcnew GroupBox();

			 int y_loc =  8 + 34*i_Index;
			// 
			// labelLayerName
			// 
			labelLayerName->Location = System::Drawing::Point(8, 8);
			labelLayerName->Size = System::Drawing::Size(96, 24);
			labelLayerName->Text = gcnew System::String(name.GetString().c_str());
			// 
			// checkVisible
			// 
			checkVisible->AutoSize = true;
			checkVisible->Location = System::Drawing::Point(111, 8);
			checkVisible->Size = System::Drawing::Size(56, 24);
			checkVisible->Text = "Visible";
			checkVisible->Tag = name.GetUID();
			checkVisible->Checked = lyerLayerMgr::GetLayerVisible(name);
			checkVisible->CheckedChanged += gcnew System::EventHandler(this, &lyerLayersDataForm::checkVisible_CheckedChanged);
			// 
			// checkPickable
			// 
			checkPickable->AutoSize = true;
			checkPickable->Location = System::Drawing::Point(173, 8);
			checkPickable->Size = System::Drawing::Size(67, 24);
			checkPickable->Text = "Pickable";
			checkPickable->Tag = name.GetUID();
			checkPickable->Checked = lyerLayerMgr::GetLayerPickable(name);
			checkPickable->CheckedChanged += gcnew System::EventHandler(this, &lyerLayersDataForm::checkPickable_CheckedChanged);
			// 
			// checkWire
			// 
			checkWire->AutoSize = true;
			checkWire->Location = System::Drawing::Point(246, 8);
			checkWire->Size = System::Drawing::Size(74, 24);
			checkWire->Text = "Wireframe";
			checkWire->Tag = name.GetUID();
			checkWire->Checked = lyerLayerMgr::GetLayerWireframe(name);
			checkWire->CheckedChanged += gcnew System::EventHandler(this, &lyerLayersDataForm::checkWire_CheckedChanged);
			// 
			// checkLowRes
			// 
			checkLowRes->AutoSize = true;
			checkLowRes->Location = System::Drawing::Point(328, 8);
			checkLowRes->Size = System::Drawing::Size(65, 24);
			checkLowRes->Text = "LowRes";
			checkLowRes->Tag = name.GetUID();
			checkLowRes->Checked = lyerLayerMgr::GetLayerLowRes(name);
			checkLowRes->CheckedChanged += gcnew System::EventHandler(this, &lyerLayersDataForm::checkLowRes_CheckedChanged);
			// 
			// groupBox
			// 
			groupBox->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			groupBox->Controls->Add(labelLayerName);
			groupBox->Controls->Add(checkVisible);
			groupBox->Controls->Add(checkPickable);
			groupBox->Controls->Add(checkWire);
			groupBox->Controls->Add(checkLowRes);
			groupBox->Location = System::Drawing::Point(8, y_loc);
			groupBox->Size = System::Drawing::Size(400, 34);
			groupBox->TabStop = false;

			this->panel1->Controls->Add(groupBox);
		 }

		TabPage^ GetTabPage()
		{
			return this->tabPage_layers;
		}

	protected:
		~lyerLayersDataForm()
		{
			// clear instance
			if (lyerLayersDataForm::FormInstance == this)
				lyerLayersDataForm::FormInstance = nullptr;

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
private: System::Windows::Forms::Button ^  buttonDeleteLayer;
private: System::Windows::Forms::Button ^  buttonCreateLayer;
#if (MULTISELECT_TREEVIEW)
private: TerawattManagedControls::TreeViewMS ^  treeViewSets;
#else
private: System::Windows::Forms::TreeView ^  treeViewSets;
#endif
private: System::Windows::Forms::TabControl ^  tabControl1;
private: System::Windows::Forms::TabPage ^  tabPage_Objects;
private: System::Windows::Forms::TabPage ^  tabPage_Properties;
private: System::Windows::Forms::Panel ^  panel1;
private: System::Windows::Forms::TabControl ^  tabControl_layers;
private: System::Windows::Forms::TabPage ^  tabPage_layers;
private: System::Windows::Forms::Button ^  button_renamelayer;

private: tmaDialogMemory^ m_pMemory;
private: bool m_bDisableNotify;
private: bool m_bSelfEdit;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(lyerLayersDataForm::typeid));
			this->buttonDeleteLayer = (gcnew System::Windows::Forms::Button());
			this->buttonCreateLayer = (gcnew System::Windows::Forms::Button());
			this->treeViewSets = (gcnew TerawattManagedControls::TreeViewMS());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_Objects = (gcnew System::Windows::Forms::TabPage());
			this->tabPage_Properties = (gcnew System::Windows::Forms::TabPage());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->tabControl_layers = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_layers = (gcnew System::Windows::Forms::TabPage());
			this->button_renamelayer = (gcnew System::Windows::Forms::Button());
			this->tabControl1->SuspendLayout();
			this->tabPage_Objects->SuspendLayout();
			this->tabPage_Properties->SuspendLayout();
			this->tabControl_layers->SuspendLayout();
			this->tabPage_layers->SuspendLayout();
			this->SuspendLayout();
			// 
			// buttonDeleteLayer
			// 
			this->buttonDeleteLayer->Location = System::Drawing::Point(128, 8);
			this->buttonDeleteLayer->Name = L"buttonDeleteLayer";
			this->buttonDeleteLayer->Size = System::Drawing::Size(96, 24);
			this->buttonDeleteLayer->TabIndex = 6;
			this->buttonDeleteLayer->Text = L"Delete Layer";
			this->buttonDeleteLayer->Click += gcnew System::EventHandler(this, &lyerLayersDataForm::buttonDeleteLayer_Click);
			// 
			// buttonCreateLayer
			// 
			this->buttonCreateLayer->Location = System::Drawing::Point(8, 8);
			this->buttonCreateLayer->Name = L"buttonCreateLayer";
			this->buttonCreateLayer->Size = System::Drawing::Size(104, 24);
			this->buttonCreateLayer->TabIndex = 5;
			this->buttonCreateLayer->Text = L"Create Layer";
			this->buttonCreateLayer->Click += gcnew System::EventHandler(this, &lyerLayersDataForm::buttonCreateLayer_Click);
			// 
			// treeViewSets
			// 
			this->treeViewSets->AllowDrop = true;
			this->treeViewSets->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->treeViewSets->HotTracking = true;
			this->treeViewSets->Location = System::Drawing::Point(8, 8);
			this->treeViewSets->Name = L"treeViewSets";
			this->treeViewSets->Size = System::Drawing::Size(331, 216);
			this->treeViewSets->TabIndex = 4;
			this->treeViewSets->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &lyerLayersDataForm::treeViewSets_DragDrop);
			this->treeViewSets->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &lyerLayersDataForm::treeViewSets_DragOver);
			this->treeViewSets->DoubleClick += gcnew System::EventHandler(this, &lyerLayersDataForm::treeViewSets_DoubleClick);
			this->treeViewSets->DragLeave += gcnew System::EventHandler(this, &lyerLayersDataForm::treeViewSets_DragLeave);
			this->treeViewSets->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &lyerLayersDataForm::treeViewSets_AfterSelect);
			this->treeViewSets->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &lyerLayersDataForm::treeViewSets_DragEnter);
			this->treeViewSets->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &lyerLayersDataForm::treeViewSets_KeyDown);
			this->treeViewSets->ItemDrag += gcnew System::Windows::Forms::ItemDragEventHandler(this, &lyerLayersDataForm::treeViewSets_ItemDrag);
			// 
			// tabControl1
			// 
			this->tabControl1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl1->Controls->Add(this->tabPage_Objects);
			this->tabControl1->Controls->Add(this->tabPage_Properties);
			this->tabControl1->Location = System::Drawing::Point(8, 48);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(355, 256);
			this->tabControl1->TabIndex = 7;
			// 
			// tabPage_Objects
			// 
			this->tabPage_Objects->Controls->Add(this->treeViewSets);
			this->tabPage_Objects->Location = System::Drawing::Point(4, 22);
			this->tabPage_Objects->Name = L"tabPage_Objects";
			this->tabPage_Objects->Size = System::Drawing::Size(347, 230);
			this->tabPage_Objects->TabIndex = 0;
			this->tabPage_Objects->Text = L"Objects";
			// 
			// tabPage_Properties
			// 
			this->tabPage_Properties->Controls->Add(this->panel1);
			this->tabPage_Properties->Location = System::Drawing::Point(4, 22);
			this->tabPage_Properties->Name = L"tabPage_Properties";
			this->tabPage_Properties->Size = System::Drawing::Size(347, 230);
			this->tabPage_Properties->TabIndex = 1;
			this->tabPage_Properties->Text = L"Properties";
			// 
			// panel1
			// 
			this->panel1->AutoScroll = true;
			this->panel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel1->Location = System::Drawing::Point(0, 0);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(347, 230);
			this->panel1->TabIndex = 5;
			// 
			// tabControl_layers
			// 
			this->tabControl_layers->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl_layers->Controls->Add(this->tabPage_layers);
			this->tabControl_layers->Location = System::Drawing::Point(8, 8);
			this->tabControl_layers->Name = L"tabControl_layers";
			this->tabControl_layers->SelectedIndex = 0;
			this->tabControl_layers->Size = System::Drawing::Size(379, 336);
			this->tabControl_layers->TabIndex = 8;
			this->tabControl_layers->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &lyerLayersDataForm::tabControl_layers_KeyDown);
			// 
			// tabPage_layers
			// 
			this->tabPage_layers->Controls->Add(this->button_renamelayer);
			this->tabPage_layers->Controls->Add(this->buttonCreateLayer);
			this->tabPage_layers->Controls->Add(this->tabControl1);
			this->tabPage_layers->Controls->Add(this->buttonDeleteLayer);
			this->tabPage_layers->Location = System::Drawing::Point(4, 22);
			this->tabPage_layers->Name = L"tabPage_layers";
			this->tabPage_layers->Size = System::Drawing::Size(371, 310);
			this->tabPage_layers->TabIndex = 0;
			this->tabPage_layers->Text = L"Layers";
			// 
			// button_renamelayer
			// 
			this->button_renamelayer->Location = System::Drawing::Point(240, 8);
			this->button_renamelayer->Name = L"button_renamelayer";
			this->button_renamelayer->Size = System::Drawing::Size(96, 24);
			this->button_renamelayer->TabIndex = 8;
			this->button_renamelayer->Text = L"Rename Layer";
			this->button_renamelayer->Click += gcnew System::EventHandler(this, &lyerLayersDataForm::button_renamelayer_Click);
			// 
			// lyerLayersDataForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(395, 350);
			this->Controls->Add(this->tabControl_layers);
			this->Name = L"lyerLayersDataForm";
			this->ShowInTaskbar = false;
			this->Text = L"Layers";
			this->tabControl1->ResumeLayout(false);
			this->tabPage_Objects->ResumeLayout(false);
			this->tabPage_Properties->ResumeLayout(false);
			this->tabControl_layers->ResumeLayout(false);
			this->tabPage_layers->ResumeLayout(false);
			this->ResumeLayout(false);

		}

		//
private: System::Void buttonCreateLayer_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			create_layer_name(gcnew System::String(""));
		}

private: System::Void buttonDeleteLayer_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 delete_layer_if_possible();
		 }

private: System::Void buttonAssignObjects_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
		 }

private: System::Void treeViewSets_AfterSelect(System::Object ^  sender, System::Windows::Forms::TreeViewEventArgs ^  e)
		 {
			 if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
			 {
				// Selected a root node.
				bool enabled = (treeViewSets->SelectedNode->Index > 0);
				this->buttonDeleteLayer->Enabled = enabled;
				this->button_renamelayer->Enabled = enabled;
			 }
			 else
			 {
				this->buttonDeleteLayer->Enabled = false;
				this->button_renamelayer->Enabled = false;
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
			TreeNode ^drop_node = treeViewSets->GetNodeAt(treeViewSets->PointToClient(Point(e->X, e->Y)));
			if (drop_node == nullptr)
				return;

#if (MULTISELECT_TREEVIEW)
			DBG_LOG1("Dropped objects with %d nodes", treeViewSets->SelectedNodes->Count);

			IEnumerator^ myEnum = treeViewSets->SelectedNodes->GetEnumerator();
            while (myEnum->MoveNext())
            {
                TreeNode ^drag_node = (TreeNode^)(myEnum->Current);
#else
			{
				TreeNode ^drag_node = safe_cast<TreeNode^>( e->Data->GetData(System::Windows::Forms::TreeNode::typeid) );
#endif
				if (drag_node != nullptr)
				{
					nameString object_name = get_name_from_node(drag_node);

					//	DEBUG only
					//
					std::string dragstr, dropstr;
					tmaManagedStringUtils::ManagedStringToStdString(drag_node->Text, dragstr);
					tmaManagedStringUtils::ManagedStringToStdString(drop_node->Text, dropstr);
					//DBG_LOG2("dropping node (%s) to (%s)", dragstr.c_str(), dropstr.c_str());

					//
					TreeNode ^set_node = (drop_node->Parent == nullptr) ? drop_node : drop_node->Parent;
					if (set_node->Index == 0)
					{
						//DBG_LOG0("Dragged to UNASSIGNED");

						// Dragging object into "<unassigned>" object set
						// means remove from old object set.
						TreeNode ^old_parent = drag_node->Parent;
						nameString set_name = get_name_from_node(old_parent);
						lyrsOperations::RemoveObjectFromLayer(set_name, object_name);
					}
					else
					{
						nameString set_name = get_name_from_node(set_node);

						//DBG_LOG1("Dragged to %s", set_name.GetString().c_str());

						lyrsOperations::AddObjectToLayer(set_name, object_name);
					}
				}
				else
				{
					//DBG_LOG0("drag node is a null pointer!");
				}
            }

			//DBG_LOG0("Dropped objects end");
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
					//treeViewSets->SelectedNode = drop_node;
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

private: System::Void checkVisible_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
				CheckBox ^radio = safe_cast<CheckBox^>(sender);
				if (radio)
				{
					nameString name = get_name_from_checkbox(radio);
					lyrsOperations::SetLayerVisible(name, radio->Checked);
				}
			}
		 }
private: System::Void checkPickable_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
				CheckBox ^radio = safe_cast<CheckBox^>(sender);
				if (radio)
				{
					nameString name = get_name_from_checkbox(radio);
					lyrsOperations::SetLayerPickable(name, radio->Checked);
				}
			}
		 }
private: System::Void checkWire_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
				CheckBox ^radio = safe_cast<CheckBox^>(sender);
				if (radio)
				{
					nameString name = get_name_from_checkbox(radio);
					lyrsOperations::SetLayerWireframe(name, radio->Checked);
				}
			}
		 }
private: System::Void checkLowRes_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
				CheckBox ^radio = safe_cast<CheckBox^>(sender);
				if (radio)
				{
					nameString name = get_name_from_checkbox(radio);
					lyrsOperations::SetLayerLowRes(name, radio->Checked);
				}
			}
		 }

private: System::Void tabControl_layers_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
		 {
		 }

private: System::Void treeViewSets_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
		 {
			 //	if delete key is pressed and it is okay for the user to delete this object
			 //
			 if ((e->KeyCode == Keys::Delete) && (buttonDeleteLayer->Enabled))
			 {
				delete_layer_if_possible();
			 }
		 }

private: System::Void delete_layer_if_possible()
		 {
			// See if root node is selected
			if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
			{
				// Can't delete "<unassigned>" object set (index==0)
				if (treeViewSets->SelectedNode->Index > 0)
				{
					nameString name = get_name_from_node(treeViewSets->SelectedNode);
					lyrsOperations::DeleteLayer(name);
				}
			}
		 }

private: System::Void button_renamelayer_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			// See if root node is selected
			if (treeViewSets->SelectedNode && (treeViewSets->SelectedNode->Parent == nullptr))
			{
				// Can't rename "<unassigned>" object set (index==0)
				if (treeViewSets->SelectedNode->Index > 0)
				{
					std::string str;
					tmaManagedStringUtils::ManagedStringToStdString(treeViewSets->SelectedNode->Text, str);
					nameUID uid = Convert::ToInt32(treeViewSets->SelectedNode->Tag);	// keep the same name UID
					nameString name(str, uid);
					edit_layer_name(name);
				}
			}
		 }

private: void create_layer_name(System::String^ i_pLayerName)
		 {
			cmmEnterNameForm ^dialog = gcnew cmmEnterNameForm(gcnew System::String("Layer Name"), i_pLayerName);
			dialog->SetNameCheckFunction( &lyerLayerMgr::IsValidLayerName );
			if (dialog->ShowDialog() == ::DialogResult::OK)
			{
				std::string str;
				tmaManagedStringUtils::ManagedStringToStdString(dialog->GetName(), str);
				nameString name(str);
				lyrsOperations::CreateLayer(name);
			}
			delete dialog;
		 }

private: void edit_layer_name(nameString& i_LayerName)
		 {
			cmmEnterNameForm ^dialog = gcnew cmmEnterNameForm(gcnew System::String("Layer Name"), gcnew System::String(i_LayerName.GetString().c_str()));
			dialog->SetNameCheckFunction( &lyerLayerMgr::IsValidLayerName );
			if (dialog->ShowDialog() == ::DialogResult::OK)
			{
				std::string str;
				tmaManagedStringUtils::ManagedStringToStdString(dialog->GetName(), str);
				nameString new_name(str,i_LayerName.GetUID());

				lyrsOperations::RenameLayer(i_LayerName, new_name);
			}
			delete dialog;
		 }
private: System::Void treeViewSets_DoubleClick(System::Object^  sender, System::EventArgs^  e) 
		 {
			TreeNode^ tree_node = treeViewSets->SelectedNode;
			if (tree_node == nullptr)
				return;

			//	select it!
			nameString object_name = get_name_from_node( tree_node );
			cmmDialogInterestMgr::SelectObject(object_name);
			cmmObjectDialogUtil::Show();

			//
			if (tree_node->Parent == nullptr)
				return;
		 }
};
}

#endif // _MANAGED
