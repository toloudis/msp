#pragma once


namespace ListBoxes
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

			fill_checked_listbox(checkedListBox_basic);
			fill_checked_listbox(checkedListBox_onecheck);
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
	private: System::Windows::Forms::TabControl *  tabControl_listboxes;
	private: System::Windows::Forms::TabPage *  tabPage_listbox;
	private: System::Windows::Forms::ListBox *  listBox_basic;
	private: System::Windows::Forms::TabPage *  tabPage_checked;
	private: System::Windows::Forms::CheckedListBox *  checkedListBox_basic;
	private: System::Windows::Forms::CheckedListBox *  checkedListBox_onecheck;
	private: System::Windows::Forms::Label *  label_onecheck;

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
			this->tabControl_listboxes = new System::Windows::Forms::TabControl();
			this->tabPage_listbox = new System::Windows::Forms::TabPage();
			this->listBox_basic = new System::Windows::Forms::ListBox();
			this->tabPage_checked = new System::Windows::Forms::TabPage();
			this->label_onecheck = new System::Windows::Forms::Label();
			this->checkedListBox_onecheck = new System::Windows::Forms::CheckedListBox();
			this->checkedListBox_basic = new System::Windows::Forms::CheckedListBox();
			this->tabControl_listboxes->SuspendLayout();
			this->tabPage_listbox->SuspendLayout();
			this->tabPage_checked->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_listboxes
			// 
			this->tabControl_listboxes->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_listboxes->Controls->Add(this->tabPage_listbox);
			this->tabControl_listboxes->Controls->Add(this->tabPage_checked);
			this->tabControl_listboxes->Location = System::Drawing::Point(8, 8);
			this->tabControl_listboxes->Name = S"tabControl_listboxes";
			this->tabControl_listboxes->SelectedIndex = 0;
			this->tabControl_listboxes->Size = System::Drawing::Size(504, 456);
			this->tabControl_listboxes->TabIndex = 0;
			// 
			// tabPage_listbox
			// 
			this->tabPage_listbox->Controls->Add(this->listBox_basic);
			this->tabPage_listbox->Location = System::Drawing::Point(4, 22);
			this->tabPage_listbox->Name = S"tabPage_listbox";
			this->tabPage_listbox->Size = System::Drawing::Size(496, 430);
			this->tabPage_listbox->TabIndex = 0;
			this->tabPage_listbox->Text = S"listbox";
			// 
			// listBox_basic
			// 
			this->listBox_basic->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->listBox_basic->Location = System::Drawing::Point(8, 16);
			this->listBox_basic->Name = S"listBox_basic";
			this->listBox_basic->Size = System::Drawing::Size(472, 134);
			this->listBox_basic->TabIndex = 0;
			// 
			// tabPage_checked
			// 
			this->tabPage_checked->Controls->Add(this->label_onecheck);
			this->tabPage_checked->Controls->Add(this->checkedListBox_onecheck);
			this->tabPage_checked->Controls->Add(this->checkedListBox_basic);
			this->tabPage_checked->Location = System::Drawing::Point(4, 22);
			this->tabPage_checked->Name = S"tabPage_checked";
			this->tabPage_checked->Size = System::Drawing::Size(496, 430);
			this->tabPage_checked->TabIndex = 1;
			this->tabPage_checked->Text = S"checked listbox";
			// 
			// label_onecheck
			// 
			this->label_onecheck->Location = System::Drawing::Point(16, 192);
			this->label_onecheck->Name = S"label_onecheck";
			this->label_onecheck->TabIndex = 2;
			this->label_onecheck->Text = S"One Check";
			// 
			// checkedListBox_onecheck
			// 
			this->checkedListBox_onecheck->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->checkedListBox_onecheck->CheckOnClick = true;
			this->checkedListBox_onecheck->Location = System::Drawing::Point(8, 224);
			this->checkedListBox_onecheck->Name = S"checkedListBox_onecheck";
			this->checkedListBox_onecheck->Size = System::Drawing::Size(472, 169);
			this->checkedListBox_onecheck->TabIndex = 1;
			this->checkedListBox_onecheck->SelectedIndexChanged += new System::EventHandler(this, checkedListBox_onecheck_SelectedIndexChanged);
			// 
			// checkedListBox_basic
			// 
			this->checkedListBox_basic->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->checkedListBox_basic->CheckOnClick = true;
			this->checkedListBox_basic->Location = System::Drawing::Point(8, 16);
			this->checkedListBox_basic->Name = S"checkedListBox_basic";
			this->checkedListBox_basic->Size = System::Drawing::Size(472, 169);
			this->checkedListBox_basic->TabIndex = 0;
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(520, 478);
			this->Controls->Add(this->tabControl_listboxes);
			this->Name = S"Form1";
			this->Text = S"ListBoxes";
			this->tabControl_listboxes->ResumeLayout(false);
			this->tabPage_listbox->ResumeLayout(false);
			this->tabPage_checked->ResumeLayout(false);
			this->ResumeLayout(false);

		}	


		void fill_checked_listbox(CheckedListBox* i_pLB)
		{
			i_pLB->Items->Add(new System::String("One"), CheckState::Unchecked);
			i_pLB->Items->Add(new System::String("Two"), CheckState::Unchecked);
			i_pLB->Items->Add(new System::String("Three"), CheckState::Unchecked);
			i_pLB->Items->Add(new System::String("Four"), CheckState::Unchecked);
			i_pLB->Items->Add(new System::String("Five"), CheckState::Checked);
		}
private: System::Void checkedListBox_onecheck_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			//	First unselect all and then select only the current one
			//
			IEnumerator* myEnum1 = checkedListBox_onecheck->CheckedIndices->GetEnumerator();
			while (myEnum1->MoveNext()) 
			{
				Int32 indexChecked =  *__try_cast<__box Int32*>(myEnum1->Current);
				checkedListBox_onecheck->SetItemCheckState(indexChecked,CheckState::Unchecked);
			}

			//	check just the one
			checkedListBox_onecheck->SetItemCheckState( checkedListBox_onecheck->SelectedIndex, CheckState::Checked );
		 }

};
}


