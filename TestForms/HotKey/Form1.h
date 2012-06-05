#pragma once


namespace HotKey
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

			m_bAlt		= false;
			m_bCtrl		= false;
			m_bShift	= false;
			m_KeyValue  = 0;
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

	private: bool m_bAlt;
	private: bool m_bCtrl;
	private: bool m_bShift;
	private: int  m_KeyValue;

	private: System::Windows::Forms::TextBox *  textBox_hotkey;
	private: System::Windows::Forms::Label *  label_hotkey;
	private: System::Windows::Forms::Label *  label_keycode;

	private: System::Windows::Forms::CheckBox *  checkBox_updatemenu;

	private: System::Windows::Forms::Label *  label_hotkeytostring;
	private: System::Windows::Forms::Label *  label_scstring;
	private: System::Windows::Forms::MenuStrip*  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem*  ToolStripMenuItem_menuItem1;
	private: System::Windows::Forms::ToolStripMenuItem*  ToolStripMenuItem_action1;
	private: System::ComponentModel::IContainer*  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (new System::ComponentModel::Container());
			this->textBox_hotkey = (new System::Windows::Forms::TextBox());
			this->label_hotkey = (new System::Windows::Forms::Label());
			this->label_keycode = (new System::Windows::Forms::Label());
			this->checkBox_updatemenu = (new System::Windows::Forms::CheckBox());
			this->label_hotkeytostring = (new System::Windows::Forms::Label());
			this->label_scstring = (new System::Windows::Forms::Label());
			this->menuStrip1 = (new System::Windows::Forms::MenuStrip());
			this->ToolStripMenuItem_menuItem1 = (new System::Windows::Forms::ToolStripMenuItem());
			this->ToolStripMenuItem_action1 = (new System::Windows::Forms::ToolStripMenuItem());
			this->menuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// textBox_hotkey
			// 
			this->textBox_hotkey->Location = System::Drawing::Point(8, 59);
			this->textBox_hotkey->MaxLength = 32;
			this->textBox_hotkey->Name = S"textBox_hotkey";
			this->textBox_hotkey->ReadOnly = true;
			this->textBox_hotkey->Size = System::Drawing::Size(224, 20);
			this->textBox_hotkey->TabIndex = 0;
			this->textBox_hotkey->KeyUp += new System::Windows::Forms::KeyEventHandler(this, &Form1::textBox_hotkey_KeyUp);
			this->textBox_hotkey->KeyPress += new System::Windows::Forms::KeyPressEventHandler(this, &Form1::textBox_hotkey_KeyPress);
			this->textBox_hotkey->KeyDown += new System::Windows::Forms::KeyEventHandler(this, &Form1::textBox_hotkey_KeyDown);
			// 
			// label_hotkey
			// 
			this->label_hotkey->Location = System::Drawing::Point(8, 35);
			this->label_hotkey->Name = S"label_hotkey";
			this->label_hotkey->Size = System::Drawing::Size(100, 23);
			this->label_hotkey->TabIndex = 1;
			this->label_hotkey->Text = S"Enter the hot key";
			// 
			// label_keycode
			// 
			this->label_keycode->Location = System::Drawing::Point(8, 83);
			this->label_keycode->Name = S"label_keycode";
			this->label_keycode->Size = System::Drawing::Size(240, 23);
			this->label_keycode->TabIndex = 2;
			this->label_keycode->Text = S"KeyCode:";
			// 
			// checkBox_updatemenu
			// 
			this->checkBox_updatemenu->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->checkBox_updatemenu->Location = System::Drawing::Point(8, 168);
			this->checkBox_updatemenu->Name = S"checkBox_updatemenu";
			this->checkBox_updatemenu->Size = System::Drawing::Size(168, 24);
			this->checkBox_updatemenu->TabIndex = 3;
			this->checkBox_updatemenu->Text = S"Update menu hotkey";
			// 
			// label_hotkeytostring
			// 
			this->label_hotkeytostring->Location = System::Drawing::Point(9, 101);
			this->label_hotkeytostring->Name = S"label_hotkeytostring";
			this->label_hotkeytostring->Size = System::Drawing::Size(232, 23);
			this->label_hotkeytostring->TabIndex = 4;
			// 
			// label_scstring
			// 
			this->label_scstring->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->label_scstring->Location = System::Drawing::Point(9, 133);
			this->label_scstring->Name = S"label_scstring";
			this->label_scstring->Size = System::Drawing::Size(240, 23);
			this->label_scstring->TabIndex = 5;
			// 
			// menuStrip1
			// 
			System::Windows::Forms::ToolStripItem* __mcTemp__1[] = new System::Windows::Forms::ToolStripItem*[1];
			__mcTemp__1[0] = this->ToolStripMenuItem_menuItem1;
			this->menuStrip1->Items->AddRange(__mcTemp__1);
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = S"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(256, 24);
			this->menuStrip1->TabIndex = 6;
			this->menuStrip1->Text = S"menuStrip1";
			// 
			// ToolStripMenuItem_menuItem1
			// 
			System::Windows::Forms::ToolStripItem* __mcTemp__2[] = new System::Windows::Forms::ToolStripItem*[1];
			__mcTemp__2[0] = this->ToolStripMenuItem_action1;
			this->ToolStripMenuItem_menuItem1->DropDownItems->AddRange(__mcTemp__2);
			this->ToolStripMenuItem_menuItem1->Name = S"ToolStripMenuItem_menuItem1";
			this->ToolStripMenuItem_menuItem1->Size = System::Drawing::Size(77, 20);
			this->ToolStripMenuItem_menuItem1->Text = S"MenuItem-1";
			// 
			// ToolStripMenuItem_action1
			// 
			this->ToolStripMenuItem_action1->Name = S"ToolStripMenuItem_action1";
			this->ToolStripMenuItem_action1->Size = System::Drawing::Size(152, 22);
			this->ToolStripMenuItem_action1->Text = S"Action-1";
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(256, 201);
			this->Controls->Add(this->label_scstring);
			this->Controls->Add(this->label_hotkeytostring);
			this->Controls->Add(this->checkBox_updatemenu);
			this->Controls->Add(this->label_keycode);
			this->Controls->Add(this->label_hotkey);
			this->Controls->Add(this->textBox_hotkey);
			this->Controls->Add(this->menuStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = S"Form1";
			this->Text = S"HotKey input";
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}	
	private: System::Void textBox_hotkey_KeyPress(System::Object *  sender, System::Windows::Forms::KeyPressEventArgs *  e)
			 {
			 }

	private: System::Void textBox_hotkey_KeyUp(System::Object *  sender, System::Windows::Forms::KeyEventArgs *  e)
			 {
			 }

	private: System::Void textBox_hotkey_KeyDown(System::Object *  sender, System::Windows::Forms::KeyEventArgs *  e)
			 {
				m_bAlt = e->Alt;
				m_bCtrl = e->Control;
				m_bShift = e->Shift;
				m_KeyValue  = 0;

				if (   (e->KeyCode != System::Windows::Forms::Keys::Menu) // better known as "ALT"
					 && (e->KeyCode != System::Windows::Forms::Keys::ControlKey)
					 && (e->KeyCode != System::Windows::Forms::Keys::ShiftKey))
				 {
					 //	lock in the key code
					 m_KeyValue = e->KeyValue;

					 Int32 num = e->KeyValue;
					 label_keycode->Text = String::Format("KeyCode: {0}", num.ToString());
				 }

				 //	build the text box
				 //
				 String* hotkey;
				 hotkey = String::Format("{0}{1}",
					 (m_bAlt? S"Alt + ":S""),
					 (m_bCtrl? S"Ctrl + ":S""));
				 hotkey = String::Concat(hotkey, String::Format("{0}{1}",
					 (m_bShift? S"Shift + ":S""),
					 ((m_KeyValue != 0)? Char::ToString(m_KeyValue):S"")));
				 textBox_hotkey->Text = hotkey;

				//
				if (checkBox_updatemenu->Checked)
				{
					String* menushortcut;
					menushortcut = String::Format("{0}{1}",(m_bCtrl? S"Ctrl + ":S""),(m_bAlt? S"Alt + ":S""));
					menushortcut = String::Concat(menushortcut, String::Format("{0}{1}",
													(m_bShift? S"Shift + ":S""),
													((m_KeyValue != 0)? Char::ToString(m_KeyValue):S"")));
					label_hotkeytostring->Text = menushortcut;

					//	Get the keys.
					try
					{
						// method #1 - CtrlA (No spaces or pluses)
						//
						//Object* obj = (Enum::Parse(__typeof(Shortcut), menushortcut));
						//menuItem_action->Shortcut = static_cast<Shortcut>(*(dynamic_cast<Int32*>(obj)));

						// method #2 - Ctrl + A
						//
						TypeConverter* keyconv = TypeDescriptor::GetConverter(__typeof(Keys));//new TypeConverter;
						Object* obj = keyconv->ConvertFromString(menushortcut);
						this->ToolStripMenuItem_action1->ShortcutKeys = static_cast<Keys>(*(dynamic_cast<Int32*>(obj)));

						//	set this based on the shortcut
						Shortcut scut = ToolStripMenuItem_action1->Shortcut;
						TypeConverter* scutconv = TypeDescriptor::GetConverter(__typeof(ShortcutKeys));//new TypeConverter;
//						Int32 snum = scut;
////int shortcut = Convert.ToInt32(Keys.Z)						
//						String* scuttext;
//						Keys k = new Keys();
//						scutconv->
////						scuttext = scutconv->ConvertToString(scut);
//						scuttext = Convert::ToString(scut);
//						label_scstring->Text = scuttext;
					}
					catch (...)
					{
						this->ToolStripMenuItem_action1 = Keys::None;
					}
				}
			 }
};
}


