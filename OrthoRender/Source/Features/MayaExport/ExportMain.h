#pragma once

#ifndef MEXP_EXPORTDATA_HPP
#include "Support/mexp/mexpExportData.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif

#ifdef _MANAGED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace Features
{
	/// <summary> 
	/// Summary for ExportMain
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ExportMain : public System::Windows::Forms::Form
	{
	public: 
		ExportMain( mexpExportData& i_Data )
			: m_Data( i_Data )
		{
			InitializeComponent();

			SetupControls();
		}

		void SetupControls()
		{
			if (m_Data.m_bExportAnimation)
			{
				this->radioButton_Animation->Checked = true;
				this->radioButton_Pose->Checked = false;
				this->floatEdit_FPS->Enabled = true;
			}
			else
			{
				this->radioButton_Animation->Checked = false;
				this->radioButton_Pose->Checked = true;
				this->floatEdit_FPS->Enabled = false;
			}

			this->floatEdit_FPS->Value = m_Data.m_SimulationFrameRate;
			
			this->checkBox_Joints->Checked = m_Data.m_bExportJoints;

			fill_in_tree();
		}
        
	public: 
		~ExportMain()
		{
			if (components)
			{
				delete components;
			}
		}
		mexpExportData& m_Data;

	private: System::Windows::Forms::Label ^  label_tree;
	private: System::Windows::Forms::TreeView ^  treeView_inventory;
	private: System::Windows::Forms::Button ^  button_export;
	private: System::Windows::Forms::RadioButton ^  radioButton_Animation;
	private: System::Windows::Forms::RadioButton ^  radioButton_Pose;
	private: System::Windows::Forms::Label ^  label1;

	private: System::Windows::Forms::GroupBox ^  groupBox1;
	private: System::Windows::Forms::CheckBox ^  checkBox_Joints;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_FPS;


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
			this->button_export = gcnew System::Windows::Forms::Button();
			this->label_tree = gcnew System::Windows::Forms::Label();
			this->treeView_inventory = gcnew System::Windows::Forms::TreeView();
			this->radioButton_Animation = gcnew System::Windows::Forms::RadioButton();
			this->radioButton_Pose = gcnew System::Windows::Forms::RadioButton();
			this->label1 = gcnew System::Windows::Forms::Label();
			this->groupBox1 = gcnew System::Windows::Forms::GroupBox();
			this->floatEdit_FPS = gcnew TerawattManagedControls::FloatEdit();
			this->checkBox_Joints = gcnew System::Windows::Forms::CheckBox();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// button_export
			// 
			this->button_export->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_export->Location = System::Drawing::Point(160, 400);
			this->button_export->Name = "button_export";
			this->button_export->Size = System::Drawing::Size(128, 23);
			this->button_export->TabIndex = 7;
			this->button_export->Text = "Export";
			this->button_export->Click += gcnew System::EventHandler(this, &ExportMain::button_export_Click);
			// 
			// label_tree
			// 
			this->label_tree->Location = System::Drawing::Point(16, 176);
			this->label_tree->Name = "label_tree";
			this->label_tree->Size = System::Drawing::Size(168, 16);
			this->label_tree->TabIndex = 4;
			this->label_tree->Text = "Inventory List";
			// 
			// treeView_inventory
			// 
			this->treeView_inventory->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->treeView_inventory->CheckBoxes = true;
			this->treeView_inventory->ImageIndex = -1;
			this->treeView_inventory->Location = System::Drawing::Point(8, 200);
			this->treeView_inventory->Name = "treeView_inventory";
			this->treeView_inventory->SelectedImageIndex = -1;
			this->treeView_inventory->Size = System::Drawing::Size(440, 184);
			this->treeView_inventory->TabIndex = 6;
			this->treeView_inventory->AfterCheck += gcnew System::Windows::Forms::TreeViewEventHandler(this, &ExportMain::treeView_inventory_AfterCheck);
			// 
			// radioButton_Animation
			// 
			this->radioButton_Animation->Location = System::Drawing::Point(16, 16);
			this->radioButton_Animation->Name = "radioButton_Animation";
			this->radioButton_Animation->Size = System::Drawing::Size(128, 24);
			this->radioButton_Animation->TabIndex = 8;
			this->radioButton_Animation->Text = "Export Animation";
			this->radioButton_Animation->CheckedChanged += gcnew System::EventHandler(this, &ExportMain::radioButton_Animation_CheckedChanged);
			// 
			// radioButton_Pose
			// 
			this->radioButton_Pose->Location = System::Drawing::Point(16, 72);
			this->radioButton_Pose->Name = "radioButton_Pose";
			this->radioButton_Pose->Size = System::Drawing::Size(160, 24);
			this->radioButton_Pose->TabIndex = 9;
			this->radioButton_Pose->Text = "Export Current Positions";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(40, 48);
			this->label1->Name = "label1";
			this->label1->Size = System::Drawing::Size(168, 24);
			this->label1->TabIndex = 10;
			this->label1->Text = "Frames per Second to Simulate";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->floatEdit_FPS);
			this->groupBox1->Controls->Add(this->radioButton_Animation);
			this->groupBox1->Controls->Add(this->radioButton_Pose);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Location = System::Drawing::Point(8, 8);
			this->groupBox1->Name = "groupBox1";
			this->groupBox1->Size = System::Drawing::Size(440, 104);
			this->groupBox1->TabIndex = 12;
			this->groupBox1->TabStop = false;
			// 
			// floatEdit_FPS
			// 
			this->floatEdit_FPS->Location = System::Drawing::Point(216, 40);
			this->floatEdit_FPS->Name = "floatEdit_FPS";
			this->floatEdit_FPS->Precision = (System::Int16)0;
			this->floatEdit_FPS->Size = System::Drawing::Size(48, 24);
			this->floatEdit_FPS->TabIndex = 11;
			// 
			// checkBox_Joints
			// 
			this->checkBox_Joints->Location = System::Drawing::Point(16, 128);
			this->checkBox_Joints->Name = "checkBox_Joints";
			this->checkBox_Joints->Size = System::Drawing::Size(248, 16);
			this->checkBox_Joints->TabIndex = 13;
			this->checkBox_Joints->Text = "Export Joints also (no Blend Shapes yet)";
			// 
			// ExportMain
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(464, 438);
			this->Controls->Add(this->checkBox_Joints);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->button_export);
			this->Controls->Add(this->label_tree);
			this->Controls->Add(this->treeView_inventory);
			this->Name = "ExportMain";
			this->Text = "Export to Maya ascii";
			this->groupBox1->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void fill_in_tree()
		{
			treeView_inventory->Nodes->Clear();

			// Add nodes to treeView_inventory.
			//
			int n=0;
			const int num_chunks = m_Data.m_Chunks.size();
			for (int ci = 0 ; ci < num_chunks ; ci++ )
			{
				const mexpExportChunkListData& chunk_data = m_Data.m_Chunks[ci];
				const int num_items = chunk_data.m_Items.size();
				if ( num_items > 0 )
				{
					TreeNode^ node = treeView_inventory->Nodes->Add(String::Format( gcnew System::String(chunk_data.m_Desc.c_str()) , n++));
					node->Expand();
					node->Checked = true;

					for (int ii = 0 ; ii < num_items ; ii++ )
					{
						node->Nodes->Add(String::Format( gcnew System::String(chunk_data.m_Items[ii].c_str()), n++ ));
					}

					this->check_all_child_nodes(node, true);
				}
			}
		}

		//------------------------------------------------------------------------
		// Updates all child tree nodes recursively.
		//------------------------------------------------------------------------
		void check_all_child_nodes(TreeNode^ treeNode, bool nodeChecked) 
		{
			IEnumerator^ myEnum = treeNode->Nodes->GetEnumerator();
			while (myEnum->MoveNext()) 
			{
				TreeNode^ node = safe_cast<TreeNode^>(myEnum->Current);

				node->Checked = nodeChecked;
				if (node->Nodes->Count > 0) 
				{
					// If the current node has child nodes, call the CheckAllChildsNodes method recursively.
					this->check_all_child_nodes(node, nodeChecked);
				}
			}
		}

		//------------------------------------------------------------------------
		//	Update the data list with ONLY the items checked.  These will
		//	then be exported to the Maya ascii file.
		//------------------------------------------------------------------------
		System::Void update_ExportDataList()
		{
			const int num_nodes = treeView_inventory->Nodes->Count;
			for (int ci=0; ci<num_nodes; ci++)
			{
				TreeNode^ chunkNode = treeView_inventory->Nodes[ci];

				// find the correct chunk index
				//
				std::string chunkdesc;
				tmaManagedStringUtils::ManagedStringToStdString( chunkNode->Text, chunkdesc );

				const int num_chunks = m_Data.m_Chunks.size();
				int chunk_index;
				for ( chunk_index = 0 ; chunk_index < num_chunks ; chunk_index++ )
				{
					if ( m_Data.m_Chunks[ chunk_index ].m_Desc == chunkdesc )
					{
						break;
					}
				}

				// If we found the chunk index...
				if (chunk_index < num_chunks)
				{
					// Clear out old data list and add in only the names that are checked
					m_Data.m_Chunks[chunk_index].m_Items.clear();

					const int num_items = chunkNode->Nodes->Count;
					for (int ii=0; ii<num_items; ii++)
					{
						TreeNode^ itemNode = chunkNode->Nodes[ii];
						if (itemNode->Checked)
						{						
							std::string itemName;
							tmaManagedStringUtils::ManagedStringToStdString( itemNode->Text, itemName );

							m_Data.m_Chunks[chunk_index].m_Items.push_back( itemName );
						}
					}
				}
			}
		}

private: System::Void button_export_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			m_Data.m_bExportAnimation = this->radioButton_Animation->Checked;
			m_Data.m_SimulationFrameRate = (float) this->floatEdit_FPS->Value;
			m_Data.m_bExportJoints = this->checkBox_Joints->Checked;

			this->update_ExportDataList();
			this->DialogResult = ::DialogResult::OK;
			this->Close();
		}
		
private: System::Void treeView_inventory_AfterCheck(System::Object ^  sender, System::Windows::Forms::TreeViewEventArgs ^  e)
		 {
			// The code only executes if the user caused the checked state to change.
			if (e->Action != TreeViewAction::Unknown) 
			{
				if (e->Node->Nodes->Count > 0) 
				{
					// Calls the check_all_child_nodes method, passing in the current
					// Checked value of the TreeNode whose checked state changed.
					this->check_all_child_nodes(e->Node, e->Node->Checked);
				}
			}
		 }

private: System::Void radioButton_Animation_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
				this->floatEdit_FPS->Enabled = this->radioButton_Animation->Checked;
		 }

};
}
#endif // _MANAGED
