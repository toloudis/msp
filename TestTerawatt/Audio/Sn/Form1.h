#pragma once

#include "fsLocator.hpp"
#include "fsFileUtil.hpp"
#include "snSoundManager.hpp"
#include "snSoundJob2D.hpp"


namespace Sn
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
	private: System::Windows::Forms::TabControl *  tabControl_sn;
	private: System::Windows::Forms::TabPage *  tabPage_basic;
	private: System::Windows::Forms::Button *  button_play;
	private: System::Windows::Forms::TextBox *  textBox_sound;
	private: System::Windows::Forms::Button *  button_browse;
	private: System::Timers::Timer *  timer1;
	private: System::Windows::Forms::Label *  label_time;
	private: System::Windows::Forms::TabPage *  tabPage_stream;


	private: System::ComponentModel::IContainer *  components;

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
			this->tabControl_sn = new System::Windows::Forms::TabControl();
			this->tabPage_basic = new System::Windows::Forms::TabPage();
			this->label_time = new System::Windows::Forms::Label();
			this->button_browse = new System::Windows::Forms::Button();
			this->textBox_sound = new System::Windows::Forms::TextBox();
			this->button_play = new System::Windows::Forms::Button();
			this->tabPage_stream = new System::Windows::Forms::TabPage();
			this->timer1 = new System::Timers::Timer();
			this->tabControl_sn->SuspendLayout();
			this->tabPage_basic->SuspendLayout();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->timer1))->BeginInit();
			this->SuspendLayout();
			// 
			// tabControl_sn
			// 
			this->tabControl_sn->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_sn->Controls->Add(this->tabPage_basic);
			this->tabControl_sn->Controls->Add(this->tabPage_stream);
			this->tabControl_sn->Location = System::Drawing::Point(8, 8);
			this->tabControl_sn->Name = S"tabControl_sn";
			this->tabControl_sn->SelectedIndex = 0;
			this->tabControl_sn->Size = System::Drawing::Size(424, 424);
			this->tabControl_sn->TabIndex = 0;
			// 
			// tabPage_basic
			// 
			this->tabPage_basic->Controls->Add(this->label_time);
			this->tabPage_basic->Controls->Add(this->button_browse);
			this->tabPage_basic->Controls->Add(this->textBox_sound);
			this->tabPage_basic->Controls->Add(this->button_play);
			this->tabPage_basic->Location = System::Drawing::Point(4, 22);
			this->tabPage_basic->Name = S"tabPage_basic";
			this->tabPage_basic->Size = System::Drawing::Size(416, 398);
			this->tabPage_basic->TabIndex = 0;
			this->tabPage_basic->Text = S"Basic";
			// 
			// label_time
			// 
			this->label_time->Location = System::Drawing::Point(104, 72);
			this->label_time->Name = S"label_time";
			this->label_time->Size = System::Drawing::Size(144, 23);
			this->label_time->TabIndex = 3;
			this->label_time->Text = S"00:00:00";
			// 
			// button_browse
			// 
			this->button_browse->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->button_browse->Location = System::Drawing::Point(384, 32);
			this->button_browse->Name = S"button_browse";
			this->button_browse->Size = System::Drawing::Size(24, 16);
			this->button_browse->TabIndex = 2;
			this->button_browse->Text = S"...";
			this->button_browse->Click += new System::EventHandler(this, button_browse_Click);
			// 
			// textBox_sound
			// 
			this->textBox_sound->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textBox_sound->Enabled = false;
			this->textBox_sound->Location = System::Drawing::Point(8, 32);
			this->textBox_sound->Name = S"textBox_sound";
			this->textBox_sound->Size = System::Drawing::Size(368, 20);
			this->textBox_sound->TabIndex = 1;
			this->textBox_sound->Text = S"";
			// 
			// button_play
			// 
			this->button_play->Location = System::Drawing::Point(8, 72);
			this->button_play->Name = S"button_play";
			this->button_play->TabIndex = 0;
			this->button_play->Text = S"play";
			this->button_play->Click += new System::EventHandler(this, button_play_Click);
			// 
			// tabPage_stream
			// 
			this->tabPage_stream->Location = System::Drawing::Point(4, 22);
			this->tabPage_stream->Name = S"tabPage_stream";
			this->tabPage_stream->Size = System::Drawing::Size(416, 398);
			this->tabPage_stream->TabIndex = 1;
			this->tabPage_stream->Text = S"Stream";
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->SynchronizingObject = this;
			this->timer1->Elapsed += new System::Timers::ElapsedEventHandler(this, timer1_Elapsed);
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(440, 438);
			this->Controls->Add(this->tabControl_sn);
			this->Name = S"Form1";
			this->Text = S"Sn Sound Test";
			this->tabControl_sn->ResumeLayout(false);
			this->tabPage_basic->ResumeLayout(false);
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->timer1))->EndInit();
			this->ResumeLayout(false);

		}	
	private: System::Void button_play_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void button_browse_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void timer1_Elapsed(System::Object *  sender, System::Timers::ElapsedEventArgs *  e);
};
}


