#pragma once

#ifndef LTST_LIGHTSETMGR_HPP
#include "Support/ltst/ltstLightSetMgr.hpp"
#endif
#ifndef LSET_OPERATIONS_HPP
#include "Systems/LightSets/Undo/lsetOperations.hpp"
#endif
#ifndef ENV_STLHELPERS_HPP
#include "Core/env/envSTLHelpers.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif


#include <algorithm>

#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace SystemLightSets
{

	/// <summary> 
	/// Summary for lsetLightSetObjectsForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class lsetLightSetObjectsForm : public System::Windows::Forms::Form
	{
	public: 
		static lsetLightSetObjectsForm^ FormInstance = nullptr;

		lsetLightSetObjectsForm()
		: m_pLightSetName(NULL)
		{
			InitializeComponent();
		}
		
		void Update(const nameString &i_LightSetName)
		{
			if (m_pLightSetName != NULL)
				delete m_pLightSetName;
			m_pLightSetName = new nameString(i_LightSetName);

			m_bDisableNotify = true;
			labelLightSetName->Text = gcnew System::String(i_LightSetName.GetString().c_str());
			fill_checked_box();
			fill_tree_view();
			m_bDisableNotify = false;
		}

		//	get a pointer to a tab page
		//
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			if (i_Index == 0)
				return tabPage_Lights;
			else
				return tabPage_Objects;
		}

		void fill_checked_box()
		{
			this->checkedListBoxLights->Items->Clear();

			std::vector<nameString> all_lights;
			ltstLightSetMgr::GetAllLights(all_lights);

			std::vector<nameString> set_lights;
			ltstLightSetMgr::GetLightsInSet(*m_pLightSetName, set_lights);

			const int num_lights = all_lights.size();
			for (int i=0; i<num_lights; i++)
			{
				std::vector<nameString>::const_iterator it = std::find(set_lights.begin(), set_lights.end(), all_lights[i]);
				bool checked = (it != set_lights.end());

				this->checkedListBoxLights->Items->Add(
					gcnew System::String(all_lights[i].GetString().c_str()), checked );
			}
		}

		void fill_tree_view()
		{
			std::vector<nameString> all_objects;
			ltstLightSetMgr::GetAllObjects(all_objects);

			std::vector<nameString> set_objects;
			ltstLightSetMgr::GetObjectsInSet(*m_pLightSetName, set_objects);

			treeViewObjects->BeginUpdate();
			treeViewObjects->Nodes->Clear();
			const int num_objects = all_objects.size();
			for (int i=0; i<num_objects; i++)
			{
				std::vector<nameString>::const_iterator it = std::find(set_objects.begin(), set_objects.end(), all_objects[i]);
				bool checked = (it != set_objects.end());

				TreeNode^ pNode = this->treeViewObjects->Nodes->Add(
					gcnew System::String(all_objects[i].GetString().c_str()));
				pNode->Checked = checked;

				std::vector<std::string> node_names;
				ltstLightSetMgr::GetFragmentNodeNames(all_objects[i], node_names);

				std::vector<int> lit_fragment_indices;
				if (checked)
				{
					ltstLightSetMgr::GetLitFragmentIndices(*m_pLightSetName, 
								   all_objects[i],
								   lit_fragment_indices);
				}

				const int num_nodes = node_names.size();
				for (int n=0; n<num_nodes; n++)
				{
					TreeNode^ pChild = pNode->Nodes->Add( gcnew System::String( node_names[n].c_str() ) );
					if (!checked)
						pChild->Checked = false;
					else if (lit_fragment_indices.empty())
						pChild->Checked = true;
					else
						pChild->Checked = envSTLHelpers::Contains(lit_fragment_indices, n);
				}
			}
			treeViewObjects->EndUpdate();
		}
		
	public: 
		~lsetLightSetObjectsForm()
		{
			if (m_pLightSetName != NULL)
				delete m_pLightSetName;

			// clear instance
			if (lsetLightSetObjectsForm::FormInstance == this)
				lsetLightSetObjectsForm::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}


	private: System::Windows::Forms::Label ^  labelLightSetName;

	private: nameString * m_pLightSetName;
	private: bool m_bDisableNotify;
	private: System::Windows::Forms::TreeView^  treeViewObjects;
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage_Lights;
	private: System::Windows::Forms::TabPage^  tabPage_Objects;


	private: System::Windows::Forms::CheckedListBox^  checkedListBoxLights;
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
			this->labelLightSetName = (gcnew System::Windows::Forms::Label());
			this->treeViewObjects = (gcnew System::Windows::Forms::TreeView());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_Lights = (gcnew System::Windows::Forms::TabPage());
			this->checkedListBoxLights = (gcnew System::Windows::Forms::CheckedListBox());
			this->tabPage_Objects = (gcnew System::Windows::Forms::TabPage());
			this->tabControl1->SuspendLayout();
			this->tabPage_Lights->SuspendLayout();
			this->tabPage_Objects->SuspendLayout();
			this->SuspendLayout();
			// 
			// labelLightSetName
			// 
			this->labelLightSetName->Location = System::Drawing::Point(16, 8);
			this->labelLightSetName->Name = L"labelLightSetName";
			this->labelLightSetName->Size = System::Drawing::Size(264, 16);
			this->labelLightSetName->TabIndex = 1;
			// 
			// treeViewObjects
			// 
			this->treeViewObjects->CheckBoxes = true;
			this->treeViewObjects->Dock = System::Windows::Forms::DockStyle::Fill;
			this->treeViewObjects->Location = System::Drawing::Point(3, 3);
			this->treeViewObjects->Name = L"treeViewObjects";
			this->treeViewObjects->Size = System::Drawing::Size(296, 262);
			this->treeViewObjects->TabIndex = 2;
			this->treeViewObjects->AfterCheck += gcnew System::Windows::Forms::TreeViewEventHandler(this, &lsetLightSetObjectsForm::treeViewObjects_AfterCheck);
			// 
			// tabControl1
			// 
			this->tabControl1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl1->Controls->Add(this->tabPage_Lights);
			this->tabControl1->Controls->Add(this->tabPage_Objects);
			this->tabControl1->Location = System::Drawing::Point(11, 11);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(310, 294);
			this->tabControl1->TabIndex = 3;
			// 
			// tabPage_Lights
			// 
			this->tabPage_Lights->Controls->Add(this->checkedListBoxLights);
			this->tabPage_Lights->Location = System::Drawing::Point(4, 22);
			this->tabPage_Lights->Name = L"tabPage_Lights";
			this->tabPage_Lights->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Lights->Size = System::Drawing::Size(302, 268);
			this->tabPage_Lights->TabIndex = 0;
			this->tabPage_Lights->Text = L"Lights";
			this->tabPage_Lights->UseVisualStyleBackColor = true;
			// 
			// checkedListBoxLights
			// 
			this->checkedListBoxLights->CheckOnClick = true;
			this->checkedListBoxLights->Dock = System::Windows::Forms::DockStyle::Fill;
			this->checkedListBoxLights->FormattingEnabled = true;
			this->checkedListBoxLights->Location = System::Drawing::Point(3, 3);
			this->checkedListBoxLights->Name = L"checkedListBoxLights";
			this->checkedListBoxLights->Size = System::Drawing::Size(296, 259);
			this->checkedListBoxLights->TabIndex = 0;
			this->checkedListBoxLights->ItemCheck += gcnew System::Windows::Forms::ItemCheckEventHandler(this, &lsetLightSetObjectsForm::checkedListBoxLights_ItemCheck);
			// 
			// tabPage_Objects
			// 
			this->tabPage_Objects->Controls->Add(this->treeViewObjects);
			this->tabPage_Objects->Location = System::Drawing::Point(4, 22);
			this->tabPage_Objects->Name = L"tabPage_Objects";
			this->tabPage_Objects->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Objects->Size = System::Drawing::Size(302, 268);
			this->tabPage_Objects->TabIndex = 1;
			this->tabPage_Objects->Text = L"Objects";
			this->tabPage_Objects->UseVisualStyleBackColor = true;
			// 
			// lsetLightSetObjectsForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(325, 308);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->labelLightSetName);
			this->Name = L"lsetLightSetObjectsForm";
			this->Text = L"LightSet Objects";
			this->tabControl1->ResumeLayout(false);
			this->tabPage_Lights->ResumeLayout(false);
			this->tabPage_Objects->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

	private: System::Void checkedListBoxLights_ItemCheck(System::Object ^  sender, System::Windows::Forms::ItemCheckEventArgs ^  e)
		{
			if (m_bDisableNotify) return;

			if (e->NewValue != e->CurrentValue)
			{
				int index = e->Index;
				std::vector<nameString> all_lights;
				ltstLightSetMgr::GetAllLights(all_lights);

				if (e->NewValue == CheckState::Checked)
				{
					lsetOperations::AddLightToSet(*m_pLightSetName, all_lights[index]);
				}
				else
				{
					lsetOperations::RemoveLightFromSet(*m_pLightSetName, all_lights[index]);
				}
			}
		}

	private: System::Void treeViewObjects_AfterCheck(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) 
		 {
			if (m_bDisableNotify) return;
			
			TreeNode^ pNode = e->Node;
			if ( pNode != nullptr )
			{
				bool bCheckState = pNode->Checked;

				// Identify the object
				std::vector<nameString> all_objects;
				ltstLightSetMgr::GetAllObjects(all_objects);

				bool bRoot = (pNode->Parent == nullptr);
				TreeNode^ pRoot = (bRoot) ? pNode : pNode->Parent;
				int index = this->treeViewObjects->Nodes->IndexOf(pRoot);
				if (index >= 0)
				{
					if (bRoot)
					{
						// Root node, turn on/off whole object
						if (bCheckState)
							lsetOperations::AddObjectToLightSet(*m_pLightSetName, all_objects[index]);
						else
							lsetOperations::RemoveObjectFromLightSet(*m_pLightSetName, all_objects[index]);

						int num_childnodes = pNode->GetNodeCount(false);
						if (num_childnodes > 0)
						{
							// This is a parent node, so make all the children's  
							// check states match this one, but don't notify
							m_bDisableNotify = true;
							for (int i=0; i< num_childnodes; ++i)
							{
								pNode->Nodes[i]->Checked = bCheckState;
							}
							m_bDisableNotify = false;
						}
					}
					else
					{
						// Child node, turn on/off just the fragment
						int node_index = pRoot->Nodes->IndexOf(pNode);
						if (node_index >= 0)
						{
							if (bCheckState)
							{
								lsetOperations::AddNodeToLightSet(*m_pLightSetName, all_objects[index], node_index);
								
								m_bDisableNotify = true;
								// Make sure the parent node is checked also now
								pRoot->Checked = true;
								m_bDisableNotify = false;
							}
							else
							{
								lsetOperations::RemoveNodeFromLightSet(*m_pLightSetName, all_objects[index], node_index);

								// If this is the last child being turned off, then
								// turn off parent node also
								int num_childnodes = pRoot->GetNodeCount(false);
								bool bAllOff = true;
								for (int i=0; i< num_childnodes; ++i)
								{
									if (pRoot->Nodes[i]->Checked)
										bAllOff = false;
								}
								if (bAllOff)
								{
									m_bDisableNotify = true;
									pRoot->Checked = false;
									m_bDisableNotify = false;
								}
							}
						}
					}
				}
			}
		 }
};
}
#endif // _MANAGED
