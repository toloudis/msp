#pragma once


namespace TreeControl
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary> 
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class Form1 : public System::Windows::Forms::Form
	{	
	public:
		Form1(void)
		{
			InitializeComponent();

			fillintree(treeView_checked);
			fillintree(treeView_unchecked);
			fillintree_fancy(treeView_fancy);
			fillintree(treeView_color);

			//
			m_pSelectedTreeNode = 0;
			create_context_menu();
		}
  
	protected:
		void Dispose(Boolean disposing)
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private: System::Windows::Forms::TabControl *  tabControl_treeviews;
	private: System::Windows::Forms::TabPage *  tabPage_unchecked;
	private: System::Windows::Forms::TreeView *  treeView_unchecked;
	private: System::Windows::Forms::Label *  label_tree_unchecked;
	private: System::Windows::Forms::TabPage *  tabPage_checked;
	private: System::Windows::Forms::TreeView *  treeView_checked;
	private: System::Windows::Forms::Label *  label_tree_checked;
	private: System::Windows::Forms::TabPage *  tabPage_fancy;
	private: System::Windows::Forms::Label *  label_fancy;
	private: System::Windows::Forms::TreeView *  treeView_fancy;
	private: System::Windows::Forms::Label *  label_desc;

	private: System::Windows::Forms::Button *  button1;
	private: System::Windows::Forms::RadioButton *  radioButton1;
	private: System::Windows::Forms::TreeView *  treeView_color;
	private: System::Windows::Forms::TabPage *  tabPage_colortree;
	private: System::Windows::Forms::TabPage *  tabPage_misc;
	private: System::Windows::Forms::Label *  label_colordesc;

	private: TreeNode* m_pSelectedTreeNode;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container * components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->treeView_unchecked = new System::Windows::Forms::TreeView();
			this->treeView_checked = new System::Windows::Forms::TreeView();
			this->label_tree_unchecked = new System::Windows::Forms::Label();
			this->label_tree_checked = new System::Windows::Forms::Label();
			this->tabControl_treeviews = new System::Windows::Forms::TabControl();
			this->tabPage_unchecked = new System::Windows::Forms::TabPage();
			this->tabPage_checked = new System::Windows::Forms::TabPage();
			this->tabPage_fancy = new System::Windows::Forms::TabPage();
			this->label_desc = new System::Windows::Forms::Label();
			this->treeView_fancy = new System::Windows::Forms::TreeView();
			this->label_fancy = new System::Windows::Forms::Label();
			this->tabPage_colortree = new System::Windows::Forms::TabPage();
			this->treeView_color = new System::Windows::Forms::TreeView();
			this->radioButton1 = new System::Windows::Forms::RadioButton();
			this->button1 = new System::Windows::Forms::Button();
			this->tabPage_misc = new System::Windows::Forms::TabPage();
			this->label_colordesc = new System::Windows::Forms::Label();
			this->tabControl_treeviews->SuspendLayout();
			this->tabPage_unchecked->SuspendLayout();
			this->tabPage_checked->SuspendLayout();
			this->tabPage_fancy->SuspendLayout();
			this->tabPage_colortree->SuspendLayout();
			this->tabPage_misc->SuspendLayout();
			this->SuspendLayout();
			// 
			// treeView_unchecked
			// 
			this->treeView_unchecked->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->treeView_unchecked->BackColor = System::Drawing::SystemColors::Window;
			this->treeView_unchecked->ImageIndex = -1;
			this->treeView_unchecked->Location = System::Drawing::Point(8, 32);
			this->treeView_unchecked->Name = S"treeView_unchecked";
			this->treeView_unchecked->SelectedImageIndex = -1;
			this->treeView_unchecked->Size = System::Drawing::Size(392, 376);
			this->treeView_unchecked->TabIndex = 0;
			// 
			// treeView_checked
			// 
			this->treeView_checked->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->treeView_checked->CheckBoxes = true;
			this->treeView_checked->ImageIndex = -1;
			this->treeView_checked->Location = System::Drawing::Point(8, 32);
			this->treeView_checked->Name = S"treeView_checked";
			this->treeView_checked->SelectedImageIndex = -1;
			this->treeView_checked->Size = System::Drawing::Size(392, 376);
			this->treeView_checked->TabIndex = 1;
			// 
			// label_tree_unchecked
			// 
			this->label_tree_unchecked->Location = System::Drawing::Point(8, 8);
			this->label_tree_unchecked->Name = S"label_tree_unchecked";
			this->label_tree_unchecked->Size = System::Drawing::Size(272, 24);
			this->label_tree_unchecked->TabIndex = 2;
			this->label_tree_unchecked->Text = S"Tree (unchecked)";
			// 
			// label_tree_checked
			// 
			this->label_tree_checked->Location = System::Drawing::Point(8, 8);
			this->label_tree_checked->Name = S"label_tree_checked";
			this->label_tree_checked->TabIndex = 3;
			this->label_tree_checked->Text = S"Tree (checked)";
			// 
			// tabControl_treeviews
			// 
			this->tabControl_treeviews->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_treeviews->Controls->Add(this->tabPage_unchecked);
			this->tabControl_treeviews->Controls->Add(this->tabPage_fancy);
			this->tabControl_treeviews->Controls->Add(this->tabPage_checked);
			this->tabControl_treeviews->Controls->Add(this->tabPage_colortree);
			this->tabControl_treeviews->Controls->Add(this->tabPage_misc);
			this->tabControl_treeviews->Location = System::Drawing::Point(8, 8);
			this->tabControl_treeviews->Name = S"tabControl_treeviews";
			this->tabControl_treeviews->SelectedIndex = 0;
			this->tabControl_treeviews->Size = System::Drawing::Size(416, 440);
			this->tabControl_treeviews->TabIndex = 4;
			// 
			// tabPage_unchecked
			// 
			this->tabPage_unchecked->Controls->Add(this->treeView_unchecked);
			this->tabPage_unchecked->Controls->Add(this->label_tree_unchecked);
			this->tabPage_unchecked->Location = System::Drawing::Point(4, 22);
			this->tabPage_unchecked->Name = S"tabPage_unchecked";
			this->tabPage_unchecked->Size = System::Drawing::Size(408, 414);
			this->tabPage_unchecked->TabIndex = 0;
			this->tabPage_unchecked->Text = S"Unchecked";
			// 
			// tabPage_checked
			// 
			this->tabPage_checked->Controls->Add(this->label_tree_checked);
			this->tabPage_checked->Controls->Add(this->treeView_checked);
			this->tabPage_checked->Location = System::Drawing::Point(4, 22);
			this->tabPage_checked->Name = S"tabPage_checked";
			this->tabPage_checked->Size = System::Drawing::Size(408, 414);
			this->tabPage_checked->TabIndex = 1;
			this->tabPage_checked->Text = S"Checked";
			// 
			// tabPage_fancy
			// 
			this->tabPage_fancy->Controls->Add(this->label_desc);
			this->tabPage_fancy->Controls->Add(this->treeView_fancy);
			this->tabPage_fancy->Controls->Add(this->label_fancy);
			this->tabPage_fancy->Location = System::Drawing::Point(4, 22);
			this->tabPage_fancy->Name = S"tabPage_fancy";
			this->tabPage_fancy->Size = System::Drawing::Size(408, 414);
			this->tabPage_fancy->TabIndex = 2;
			this->tabPage_fancy->Text = S"Fancy";
			// 
			// label_desc
			// 
			this->label_desc->Location = System::Drawing::Point(8, 395);
			this->label_desc->Name = S"label_desc";
			this->label_desc->Size = System::Drawing::Size(408, 23);
			this->label_desc->TabIndex = 2;
			this->label_desc->Text = S"Description";
			// 
			// treeView_fancy
			// 
			this->treeView_fancy->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->treeView_fancy->ImageIndex = -1;
			this->treeView_fancy->Location = System::Drawing::Point(8, 32);
			this->treeView_fancy->Name = S"treeView_fancy";
			this->treeView_fancy->SelectedImageIndex = -1;
			this->treeView_fancy->Size = System::Drawing::Size(392, 352);
			this->treeView_fancy->TabIndex = 1;
			this->treeView_fancy->AfterSelect += new System::Windows::Forms::TreeViewEventHandler(this, &TreeControl::Form1::treeView_fancy_AfterSelect);
			// 
			// label_fancy
			// 
			this->label_fancy->Location = System::Drawing::Point(8, 8);
			this->label_fancy->Name = S"label_fancy";
			this->label_fancy->TabIndex = 0;
			this->label_fancy->Text = S"Fancy TreeView";
			// 
			// tabPage_colortree
			// 
			this->tabPage_colortree->Controls->Add(this->label_colordesc);
			this->tabPage_colortree->Controls->Add(this->treeView_color);
			this->tabPage_colortree->Location = System::Drawing::Point(4, 22);
			this->tabPage_colortree->Name = S"tabPage_colortree";
			this->tabPage_colortree->Size = System::Drawing::Size(408, 414);
			this->tabPage_colortree->TabIndex = 3;
			this->tabPage_colortree->Text = S"Color TreeNodes";
			// 
			// treeView_color
			// 
			this->treeView_color->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->treeView_color->ImageIndex = -1;
			this->treeView_color->Location = System::Drawing::Point(8, 40);
			this->treeView_color->Name = S"treeView_color";
			this->treeView_color->SelectedImageIndex = -1;
			this->treeView_color->Size = System::Drawing::Size(392, 360);
			this->treeView_color->TabIndex = 2;
			this->treeView_color->MouseDown += new System::Windows::Forms::MouseEventHandler(this, &TreeControl::Form1::treeView_color_MouseDown);
			// 
			// radioButton1
			// 
			this->radioButton1->Location = System::Drawing::Point(16, 48);
			this->radioButton1->Name = S"radioButton1";
			this->radioButton1->TabIndex = 1;
			this->radioButton1->Text = S"radioButton1";
			this->radioButton1->Enter += new System::EventHandler(this, &TreeControl::Form1::radioButton1_Enter);
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(16, 16);
			this->button1->Name = S"button1";
			this->button1->TabIndex = 0;
			this->button1->Text = S"button1";
			this->button1->Enter += new System::EventHandler(this, &TreeControl::Form1::button1_Enter);
			// 
			// tabPage_misc
			// 
			this->tabPage_misc->Controls->Add(this->radioButton1);
			this->tabPage_misc->Controls->Add(this->button1);
			this->tabPage_misc->Location = System::Drawing::Point(4, 22);
			this->tabPage_misc->Name = S"tabPage_misc";
			this->tabPage_misc->Size = System::Drawing::Size(360, 390);
			this->tabPage_misc->TabIndex = 4;
			this->tabPage_misc->Text = S"Misc";
			// 
			// label_colordesc
			// 
			this->label_colordesc->Location = System::Drawing::Point(8, 8);
			this->label_colordesc->Name = S"label_colordesc";
			this->label_colordesc->Size = System::Drawing::Size(208, 23);
			this->label_colordesc->TabIndex = 3;
			this->label_colordesc->Text = S"Right-Click on Tree Node to color it";
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(432, 454);
			this->Controls->Add(this->tabControl_treeviews);
			this->Name = S"Form1";
			this->Text = S"Test Form - TreeView";
			this->tabControl_treeviews->ResumeLayout(false);
			this->tabPage_unchecked->ResumeLayout(false);
			this->tabPage_checked->ResumeLayout(false);
			this->tabPage_fancy->ResumeLayout(false);
			this->tabPage_colortree->ResumeLayout(false);
			this->tabPage_misc->ResumeLayout(false);
			this->ResumeLayout(false);

		}	
		
private:
		// Updates all child tree nodes recursively.
	void CheckAllChildNodes(TreeNode* treeNode, bool nodeChecked) 
	{
		IEnumerator* myEnum = treeNode->Nodes->GetEnumerator();
		while (myEnum->MoveNext()) 
		{
			TreeNode* node = __try_cast<TreeNode*>(myEnum->Current);

			node->Checked = nodeChecked;
			if (node->Nodes->Count > 0) 
			{
				// If the current node has child nodes, call the CheckAllChildsNodes method recursively.
				this->CheckAllChildNodes(node, nodeChecked);
			}
		}
	}

	// NOTE   This code can be added to the BeforeCheck event handler instead of the AfterCheck event.
	// After a tree node's Checked property is changed, all its child nodes are updated to the same value.
	void node_AfterCheck(Object* /*sender*/, TreeViewEventArgs* e) 
	{
		// The code only executes if the user caused the checked state to change.
		if (e->Action != TreeViewAction::Unknown) 
		{
			if (e->Node->Nodes->Count > 0) 
			{
				// Calls the CheckAllChildNodes method, passing in the current
				// Checked value of the TreeNode whose checked state changed.
				this->CheckAllChildNodes(e->Node, e->Node->Checked);
			}
		}
	}

	void fillintree( TreeView* i_pTV )
	{
		TreeNode* rootnode;
		TreeNode* node;
		int n;

		//	set up the tree
		//
		i_pTV->AfterCheck += new System::Windows::Forms::TreeViewEventHandler( this, &TreeControl::Form1::node_AfterCheck );

		//	fill it with crap data
		//
		// Add nodes to treeView_unchecked.
		n=0;
		rootnode = i_pTV->Nodes->Add(String::Format(S"Level BLAH", __box(n++)));

		node = rootnode->Nodes->Add(String::Format(S"Actors", __box(n++)));
		node->Nodes->Add(String::Format(S"Bob", __box(n++)));
		node->Nodes->Add(String::Format(S"Steve", __box(n++)));
		node->Nodes->Add(String::Format(S"Joe", __box(n++)));
		node->Nodes->Add(String::Format(S"Mike", __box(n++)));

		node = rootnode->Nodes->Add(String::Format(S"Lights", __box(n++)));
		node->Nodes->Add(String::Format(S"spot", __box(n++)));
		node->Nodes->Add(String::Format(S"directional", __box(n++)));
		node->Nodes->Add(String::Format(S"point-001", __box(n++)));

		node = rootnode->Nodes->Add(String::Format(S"Props", __box(n++)));
		node->Nodes->Add(String::Format(S"Chair1", __box(n++)));
		node->Nodes->Add(String::Format(S"Chair2", __box(n++)));
		node->Nodes->Add(String::Format(S"table", __box(n++)));
		node->Nodes->Add(String::Format(S"cup", __box(n++)));
	}

	void fillintree_fancy( TreeView* i_pTV )
	{
		TreeNode* rootnode;
		TreeNode* node;
		TreeNode* subnode;
		int n;

		//	set up the tree
		//
		i_pTV->FullRowSelect = true;
		i_pTV->ShowLines = false;
		i_pTV->Scrollable = true;
		i_pTV->AfterCheck += new System::Windows::Forms::TreeViewEventHandler( this, &TreeControl::Form1::node_AfterCheck );
		//i_pTV->BackColor = System::Drawing::SystemColors::ControlDark;

		//	fill it with crap data
		//
		// Add nodes to treeView_unchecked.
		n=0;
		node = i_pTV->Nodes->Add(String::Format(S"Actors", __box(n++)));
		node->BackColor = System::Drawing::SystemColors::ControlDark;
		node->ForeColor = System::Drawing::SystemColors::ControlLightLight;
		subnode = node->Nodes->Add(String::Format(S"Bob", __box(n++)));
		subnode->BackColor = System::Drawing::SystemColors::ControlLightLight;
		subnode = node->Nodes->Add(String::Format(S"Steve", __box(n++)));
		subnode->BackColor = System::Drawing::SystemColors::ControlLightLight;
		subnode = node->Nodes->Add(String::Format(S"Joe", __box(n++)));
		subnode->BackColor = System::Drawing::SystemColors::ControlLightLight;
		subnode = node->Nodes->Add(String::Format(S"Mike", __box(n++)));
		subnode->BackColor = System::Drawing::SystemColors::ControlLightLight;

		node = i_pTV->Nodes->Add(String::Format(S"Lights", __box(n++)));
		node->BackColor = System::Drawing::Color::LightGray;
		node->ForeColor = System::Drawing::Color::Black;
		subnode = node->Nodes->Add(String::Format(S"spot", __box(n++)));
		subnode->BackColor = System::Drawing::SystemColors::ControlLightLight;
		subnode = node->Nodes->Add(String::Format(S"directional", __box(n++)));
		subnode->BackColor = System::Drawing::Color::Yellow;
		subnode = node->Nodes->Add(String::Format(S"point-001", __box(n++)));
		subnode->BackColor = System::Drawing::SystemColors::ControlLightLight;

		node = i_pTV->Nodes->Add(String::Format(S"Props", __box(n++)));
		node->BackColor = System::Drawing::SystemColors::ControlDark;
		node->ForeColor = System::Drawing::SystemColors::ControlLightLight;
		subnode = node->Nodes->Add(String::Format(S"Chair1", __box(n++)));
		subnode->BackColor = System::Drawing::SystemColors::ControlLightLight;
		subnode = node->Nodes->Add(String::Format(S"Chair2", __box(n++)));
		subnode->BackColor = System::Drawing::SystemColors::ControlLightLight;
		subnode = node->Nodes->Add(String::Format(S"table", __box(n++)));
		subnode->BackColor = System::Drawing::SystemColors::ControlLightLight;
		subnode = node->Nodes->Add(String::Format(S"cup", __box(n++)));
		subnode->BackColor = System::Drawing::SystemColors::ControlLightLight;
	}
	private: System::Void treeView_fancy_AfterSelect(System::Object *  sender, System::Windows::Forms::TreeViewEventArgs *  e)
			 {
				 TreeNode* pTN = e->get_Node();
				 if (pTN != 0)
				 {
					 label_desc->Text = String::Concat("Description - ",pTN->Text);
				 }
				 else
				 {
					 label_desc->Text = "Description";
				 }
			 }

private: System::Void button1_Enter(System::Object *  sender, System::EventArgs *  e)
		 {
		 }

private: System::Void radioButton1_Enter(System::Object *  sender, System::EventArgs *  e)
		 {
		 }

private: void menuItem1_Click(Object* /*sender*/, System::EventArgs* /*e*/) 
		{
			if (m_pSelectedTreeNode != 0)
			{
				System::Windows::Forms::ColorDialog* dialog = new System::Windows::Forms::ColorDialog();
				dialog->FullOpen = true;

				if (dialog->ShowDialog() == DialogResult::OK)
				{
					m_pSelectedTreeNode->BackColor = dialog->Color;
				}
			}
		}

private: System::Void treeView_color_MouseDown(System::Object *  sender, System::Windows::Forms::MouseEventArgs *  e)
		 {
			m_pSelectedTreeNode = treeView_color->GetNodeAt(e->X, e->Y);
			//switch (e->get_Button()) 
			//{
			//	case MouseButtons::Left:
			//		break;
			//	case MouseButtons::Right:
			//		break;
			//	case MouseButtons::Middle:
			//		break;
			//	case MouseButtons::XButton1:
			//		break;
			//	case MouseButtons::XButton2:
			//		break;
			//	case MouseButtons::None:
			//	default:
			//		break;
			//}
		 }

private: System::Void create_context_menu()
		{
			if (treeView_color->ContextMenu == 0)
			{
				System::Windows::Forms::ContextMenu *mnuContextMenu = new System::Windows::Forms::ContextMenu();
				MenuItem *mnuItemNew = new MenuItem();
				mnuItemNew->Text = S"Set Color";
				mnuContextMenu->MenuItems->Add(mnuItemNew);
			    mnuItemNew->Click += new System::EventHandler(this, &Form1::menuItem1_Click);
				treeView_color->ContextMenu = mnuContextMenu;
			}
		}
};
}


