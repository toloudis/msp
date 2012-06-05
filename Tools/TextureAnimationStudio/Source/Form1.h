#pragma once

#ifndef TAS_DOCUMENT_HPP
#include "tasDocument.hpp"
#endif
#ifndef TMA_IMAGELIST_HPP
#include "tmaImageList.hpp"
#endif


//============================================================================
//============================================================================
namespace TextureAnimationStudio
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Globalization;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace TerawattManagedControls;

	/// <summary> 
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public Form
	{	
	public:
		Form1(void);

	public:
		System::Windows::Forms::Panel ^ GetTUVRenderWindow()
		{
			return this->panel_TUV_render;
		}

		System::Windows::Forms::Panel ^ GetTUVRenderInitWindow()
		{
			return this->panel_TUV_init;
		}

	protected:
		~Form1()
		{
			m_pImageList->RemoveAll();

			delete m_pDocument;

			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::MainMenu ^  mainMenu1;
	private: System::Windows::Forms::MenuItem ^  menuItem_file;
	private: System::Windows::Forms::MenuItem ^  menuItem_exit;
	private: System::Windows::Forms::MenuItem ^  menuItem_about;
	private: System::Windows::Forms::MenuItem ^  menuItem_help;

	private: System::Windows::Forms::TabControl ^  tabControl_tools;
	private: System::Windows::Forms::TabPage ^  tabPage_instructions;
	private: System::Windows::Forms::TextBox ^  textBox_instructions;

	private: System::Windows::Forms::TabPage ^  tabPage_tuv;
	private: System::Windows::Forms::Button ^  button_TUV_new;
	private: System::Windows::Forms::Button ^  button_TUV_open;
	private: System::Windows::Forms::Button ^  button_TUV_save;
	private: System::Windows::Forms::Button ^  button_TUV_saveas;
	private: System::Windows::Forms::CheckBox ^  checkBox_TUV_looping;
	private: System::Windows::Forms::CheckBox ^  checkBox_TUV_reversing;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_TUV_rate;
	private: System::Windows::Forms::NumericUpDown ^  numericUpDown_TUV_xdiv;
	private: System::Windows::Forms::NumericUpDown ^  numericUpDown_TUV_ydiv;
	private: System::Windows::Forms::Label ^  label_TUV_xdiv;
	private: System::Windows::Forms::Label ^  label_TUV_ydiv;
	private: System::Windows::Forms::Label ^  label_TUV_rate;
	private: System::Windows::Forms::Button ^  button_TUV_reset;
	private: System::Windows::Forms::Panel ^  panel_TUV_render;
	private: System::Windows::Forms::Panel ^  panel_TUV_init;
	private: System::Windows::Forms::Label ^  label_TUV_animtexture_filename;
	private: TerawattManagedControls::FileChooser ^  fileChooser_TUV_animtexture;

	private: System::Windows::Forms::TabPage ^  tabPage_texture;
	private: System::Windows::Forms::GroupBox ^  groupBox_individual_textures;
	private: System::Windows::Forms::PictureBox ^  pictureBox_UVA_anim;
	private: System::Windows::Forms::ListBox ^  listBox_UVA_textures;
	private: System::Windows::Forms::Button ^  button_UVA_add;
	private: System::Windows::Forms::Button ^  button_UVA_del;
	private: System::Windows::Forms::Button ^  button_UVA_up;
	private: System::Windows::Forms::Button ^  button_UVA_down;
	private: System::Windows::Forms::Label ^  label_UVA_texture_size;
	private: System::Windows::Forms::Button ^  button_UVA_send;
	private: System::Windows::Forms::Button ^  button_UVA_saveUVA;
	private: System::Windows::Forms::CheckedListBox ^  checkedListBox_UVA_resolution;
	private: System::Windows::Forms::PictureBox ^  pictureBox_UVA_composite;
	private: System::Windows::Forms::PictureBox ^  pictureBox_TUV_tmap;
	private: System::Windows::Forms::ComboBox^  comboBox_format;

	private: System::ComponentModel::IContainer ^  components;

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
			this->components = (gcnew System::ComponentModel::Container());
			this->pictureBox_UVA_anim = (gcnew System::Windows::Forms::PictureBox());
			this->mainMenu1 = (gcnew System::Windows::Forms::MainMenu(this->components));
			this->menuItem_file = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem_exit = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem_help = (gcnew System::Windows::Forms::MenuItem());
			this->menuItem_about = (gcnew System::Windows::Forms::MenuItem());
			this->checkBox_TUV_looping = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_TUV_reversing = (gcnew System::Windows::Forms::CheckBox());
			this->rangedFloat_TUV_rate = (gcnew TerawattManagedControls::RangedFloat());
			this->numericUpDown_TUV_xdiv = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown_TUV_ydiv = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_TUV_xdiv = (gcnew System::Windows::Forms::Label());
			this->label_TUV_ydiv = (gcnew System::Windows::Forms::Label());
			this->label_TUV_rate = (gcnew System::Windows::Forms::Label());
			this->listBox_UVA_textures = (gcnew System::Windows::Forms::ListBox());
			this->button_UVA_add = (gcnew System::Windows::Forms::Button());
			this->button_UVA_del = (gcnew System::Windows::Forms::Button());
			this->button_UVA_up = (gcnew System::Windows::Forms::Button());
			this->button_UVA_down = (gcnew System::Windows::Forms::Button());
			this->button_TUV_reset = (gcnew System::Windows::Forms::Button());
			this->panel_TUV_render = (gcnew System::Windows::Forms::Panel());
			this->tabControl_tools = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_instructions = (gcnew System::Windows::Forms::TabPage());
			this->textBox_instructions = (gcnew System::Windows::Forms::TextBox());
			this->tabPage_texture = (gcnew System::Windows::Forms::TabPage());
			this->label_UVA_texture_size = (gcnew System::Windows::Forms::Label());
			this->button_UVA_send = (gcnew System::Windows::Forms::Button());
			this->button_UVA_saveUVA = (gcnew System::Windows::Forms::Button());
			this->checkedListBox_UVA_resolution = (gcnew System::Windows::Forms::CheckedListBox());
			this->pictureBox_UVA_composite = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox_individual_textures = (gcnew System::Windows::Forms::GroupBox());
			this->tabPage_tuv = (gcnew System::Windows::Forms::TabPage());
			this->pictureBox_TUV_tmap = (gcnew System::Windows::Forms::PictureBox());
			this->button_TUV_saveas = (gcnew System::Windows::Forms::Button());
			this->button_TUV_save = (gcnew System::Windows::Forms::Button());
			this->button_TUV_open = (gcnew System::Windows::Forms::Button());
			this->button_TUV_new = (gcnew System::Windows::Forms::Button());
			this->label_TUV_animtexture_filename = (gcnew System::Windows::Forms::Label());
			this->fileChooser_TUV_animtexture = (gcnew TerawattManagedControls::FileChooser());
			this->panel_TUV_init = (gcnew System::Windows::Forms::Panel());
			this->comboBox_format = (gcnew System::Windows::Forms::ComboBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox_UVA_anim))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_TUV_xdiv))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_TUV_ydiv))->BeginInit();
			this->tabControl_tools->SuspendLayout();
			this->tabPage_instructions->SuspendLayout();
			this->tabPage_texture->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox_UVA_composite))->BeginInit();
			this->groupBox_individual_textures->SuspendLayout();
			this->tabPage_tuv->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox_TUV_tmap))->BeginInit();
			this->SuspendLayout();
			// 
			// pictureBox_UVA_anim
			// 
			this->pictureBox_UVA_anim->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->pictureBox_UVA_anim->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			this->pictureBox_UVA_anim->Location = System::Drawing::Point(16, 24);
			this->pictureBox_UVA_anim->Name = L"pictureBox_UVA_anim";
			this->pictureBox_UVA_anim->Size = System::Drawing::Size(128, 128);
			this->pictureBox_UVA_anim->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBox_UVA_anim->TabIndex = 0;
			this->pictureBox_UVA_anim->TabStop = false;
			// 
			// mainMenu1
			// 
			this->mainMenu1->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(2) {this->menuItem_file, this->menuItem_help});
			// 
			// menuItem_file
			// 
			this->menuItem_file->Index = 0;
			this->menuItem_file->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(1) {this->menuItem_exit});
			this->menuItem_file->Text = L"File";
			// 
			// menuItem_exit
			// 
			this->menuItem_exit->Index = 0;
			this->menuItem_exit->Text = L"Exit";
			this->menuItem_exit->Click += gcnew System::EventHandler(this, &Form1::menuItem_exit_Click);
			// 
			// menuItem_help
			// 
			this->menuItem_help->Index = 1;
			this->menuItem_help->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(1) {this->menuItem_about});
			this->menuItem_help->Text = L"Help";
			// 
			// menuItem_about
			// 
			this->menuItem_about->Index = 0;
			this->menuItem_about->Text = L"About";
			this->menuItem_about->Click += gcnew System::EventHandler(this, &Form1::menuItem_about_Click);
			// 
			// checkBox_TUV_looping
			// 
			this->checkBox_TUV_looping->Checked = true;
			this->checkBox_TUV_looping->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_TUV_looping->Location = System::Drawing::Point(280, 24);
			this->checkBox_TUV_looping->Name = L"checkBox_TUV_looping";
			this->checkBox_TUV_looping->Size = System::Drawing::Size(80, 24);
			this->checkBox_TUV_looping->TabIndex = 1;
			this->checkBox_TUV_looping->Text = L"Looping";
			this->checkBox_TUV_looping->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox_TUV_looping_CheckedChanged);
			// 
			// checkBox_TUV_reversing
			// 
			this->checkBox_TUV_reversing->Location = System::Drawing::Point(280, 48);
			this->checkBox_TUV_reversing->Name = L"checkBox_TUV_reversing";
			this->checkBox_TUV_reversing->Size = System::Drawing::Size(80, 24);
			this->checkBox_TUV_reversing->TabIndex = 2;
			this->checkBox_TUV_reversing->Text = L"Reversing";
			this->checkBox_TUV_reversing->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox_TUV_reversing_CheckedChanged);
			// 
			// rangedFloat_TUV_rate
			// 
			this->rangedFloat_TUV_rate->Exponent = static_cast<System::Int16>(1);
			this->rangedFloat_TUV_rate->Location = System::Drawing::Point(48, 280);
			this->rangedFloat_TUV_rate->Maximum = 60;
			this->rangedFloat_TUV_rate->Name = L"rangedFloat_TUV_rate";
			this->rangedFloat_TUV_rate->NumTicks = static_cast<System::Int16>(100);
			this->rangedFloat_TUV_rate->Precision = static_cast<System::Int16>(2);
			this->rangedFloat_TUV_rate->Size = System::Drawing::Size(152, 32);
			this->rangedFloat_TUV_rate->TabIndex = 3;
			this->rangedFloat_TUV_rate->Value = 15;
			this->rangedFloat_TUV_rate->ValueChanged += gcnew System::EventHandler(this, &Form1::rangedFloat_TUV_rate_ValueChanged);
			// 
			// numericUpDown_TUV_xdiv
			// 
			this->numericUpDown_TUV_xdiv->Location = System::Drawing::Point(344, 80);
			this->numericUpDown_TUV_xdiv->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {256, 0, 0, 0});
			this->numericUpDown_TUV_xdiv->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown_TUV_xdiv->Name = L"numericUpDown_TUV_xdiv";
			this->numericUpDown_TUV_xdiv->Size = System::Drawing::Size(56, 20);
			this->numericUpDown_TUV_xdiv->TabIndex = 4;
			this->numericUpDown_TUV_xdiv->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDown_TUV_xdiv->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown_TUV_xdiv->ValueChanged += gcnew System::EventHandler(this, &Form1::numericUpDown_TUV_xdiv_ValueChanged);
			// 
			// numericUpDown_TUV_ydiv
			// 
			this->numericUpDown_TUV_ydiv->Location = System::Drawing::Point(344, 104);
			this->numericUpDown_TUV_ydiv->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {256, 0, 0, 0});
			this->numericUpDown_TUV_ydiv->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown_TUV_ydiv->Name = L"numericUpDown_TUV_ydiv";
			this->numericUpDown_TUV_ydiv->Size = System::Drawing::Size(56, 20);
			this->numericUpDown_TUV_ydiv->TabIndex = 5;
			this->numericUpDown_TUV_ydiv->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDown_TUV_ydiv->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown_TUV_ydiv->ValueChanged += gcnew System::EventHandler(this, &Form1::numericUpDown_TUV_ydiv_ValueChanged);
			// 
			// label_TUV_xdiv
			// 
			this->label_TUV_xdiv->Location = System::Drawing::Point(280, 80);
			this->label_TUV_xdiv->Name = L"label_TUV_xdiv";
			this->label_TUV_xdiv->Size = System::Drawing::Size(56, 23);
			this->label_TUV_xdiv->TabIndex = 6;
			this->label_TUV_xdiv->Text = L"X Division";
			this->label_TUV_xdiv->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_TUV_ydiv
			// 
			this->label_TUV_ydiv->Location = System::Drawing::Point(280, 104);
			this->label_TUV_ydiv->Name = L"label_TUV_ydiv";
			this->label_TUV_ydiv->Size = System::Drawing::Size(56, 23);
			this->label_TUV_ydiv->TabIndex = 7;
			this->label_TUV_ydiv->Text = L"Y Division";
			this->label_TUV_ydiv->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_TUV_rate
			// 
			this->label_TUV_rate->Location = System::Drawing::Point(16, 280);
			this->label_TUV_rate->Name = L"label_TUV_rate";
			this->label_TUV_rate->Size = System::Drawing::Size(32, 32);
			this->label_TUV_rate->TabIndex = 8;
			this->label_TUV_rate->Text = L"Rate";
			this->label_TUV_rate->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// listBox_UVA_textures
			// 
			this->listBox_UVA_textures->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->listBox_UVA_textures->Location = System::Drawing::Point(16, 160);
			this->listBox_UVA_textures->Name = L"listBox_UVA_textures";
			this->listBox_UVA_textures->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->listBox_UVA_textures->Size = System::Drawing::Size(128, 186);
			this->listBox_UVA_textures->TabIndex = 9;
			this->listBox_UVA_textures->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::listBox_UVA_textures_SelectedIndexChanged);
			// 
			// button_UVA_add
			// 
			this->button_UVA_add->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_UVA_add->Location = System::Drawing::Point(32, 352);
			this->button_UVA_add->Name = L"button_UVA_add";
			this->button_UVA_add->Size = System::Drawing::Size(40, 23);
			this->button_UVA_add->TabIndex = 10;
			this->button_UVA_add->Text = L"Add";
			this->button_UVA_add->Click += gcnew System::EventHandler(this, &Form1::button_UVA_add_Click);
			// 
			// button_UVA_del
			// 
			this->button_UVA_del->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_UVA_del->Location = System::Drawing::Point(80, 352);
			this->button_UVA_del->Name = L"button_UVA_del";
			this->button_UVA_del->Size = System::Drawing::Size(40, 23);
			this->button_UVA_del->TabIndex = 11;
			this->button_UVA_del->Text = L"Del";
			this->button_UVA_del->Click += gcnew System::EventHandler(this, &Form1::button_UVA_del_Click);
			// 
			// button_UVA_up
			// 
			this->button_UVA_up->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->button_UVA_up->Location = System::Drawing::Point(152, 216);
			this->button_UVA_up->Name = L"button_UVA_up";
			this->button_UVA_up->Size = System::Drawing::Size(40, 23);
			this->button_UVA_up->TabIndex = 12;
			this->button_UVA_up->Text = L"Up";
			this->button_UVA_up->Click += gcnew System::EventHandler(this, &Form1::button_UVA_up_Click);
			// 
			// button_UVA_down
			// 
			this->button_UVA_down->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->button_UVA_down->Location = System::Drawing::Point(152, 248);
			this->button_UVA_down->Name = L"button_UVA_down";
			this->button_UVA_down->Size = System::Drawing::Size(40, 23);
			this->button_UVA_down->TabIndex = 13;
			this->button_UVA_down->Text = L"Dwn";
			this->button_UVA_down->Click += gcnew System::EventHandler(this, &Form1::button_UVA_down_Click);
			// 
			// button_TUV_reset
			// 
			this->button_TUV_reset->Location = System::Drawing::Point(216, 280);
			this->button_TUV_reset->Name = L"button_TUV_reset";
			this->button_TUV_reset->Size = System::Drawing::Size(48, 32);
			this->button_TUV_reset->TabIndex = 14;
			this->button_TUV_reset->Text = L"Reset Play";
			this->button_TUV_reset->Click += gcnew System::EventHandler(this, &Form1::button_TUV_reset_Click);
			// 
			// panel_TUV_render
			// 
			this->panel_TUV_render->BackColor = System::Drawing::SystemColors::WindowFrame;
			this->panel_TUV_render->Location = System::Drawing::Point(8, 16);
			this->panel_TUV_render->Name = L"panel_TUV_render";
			this->panel_TUV_render->Size = System::Drawing::Size(256, 256);
			this->panel_TUV_render->TabIndex = 15;
			// 
			// tabControl_tools
			// 
			this->tabControl_tools->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl_tools->Controls->Add(this->tabPage_instructions);
			this->tabControl_tools->Controls->Add(this->tabPage_texture);
			this->tabControl_tools->Controls->Add(this->tabPage_tuv);
			this->tabControl_tools->Location = System::Drawing::Point(8, 8);
			this->tabControl_tools->Name = L"tabControl_tools";
			this->tabControl_tools->SelectedIndex = 0;
			this->tabControl_tools->Size = System::Drawing::Size(496, 435);
			this->tabControl_tools->TabIndex = 16;
			// 
			// tabPage_instructions
			// 
			this->tabPage_instructions->Controls->Add(this->textBox_instructions);
			this->tabPage_instructions->Location = System::Drawing::Point(4, 22);
			this->tabPage_instructions->Name = L"tabPage_instructions";
			this->tabPage_instructions->Size = System::Drawing::Size(488, 409);
			this->tabPage_instructions->TabIndex = 2;
			this->tabPage_instructions->Text = L"Instruction";
			// 
			// textBox_instructions
			// 
			this->textBox_instructions->AcceptsReturn = true;
			this->textBox_instructions->AcceptsTab = true;
			this->textBox_instructions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_instructions->BackColor = System::Drawing::SystemColors::Control;
			this->textBox_instructions->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->textBox_instructions->Location = System::Drawing::Point(8, 8);
			this->textBox_instructions->Multiline = true;
			this->textBox_instructions->Name = L"textBox_instructions";
			this->textBox_instructions->Size = System::Drawing::Size(464, 387);
			this->textBox_instructions->TabIndex = 0;
			this->textBox_instructions->Text = L"Instruction";
			// 
			// tabPage_texture
			// 
			this->tabPage_texture->Controls->Add(this->comboBox_format);
			this->tabPage_texture->Controls->Add(this->label_UVA_texture_size);
			this->tabPage_texture->Controls->Add(this->button_UVA_send);
			this->tabPage_texture->Controls->Add(this->button_UVA_saveUVA);
			this->tabPage_texture->Controls->Add(this->checkedListBox_UVA_resolution);
			this->tabPage_texture->Controls->Add(this->pictureBox_UVA_composite);
			this->tabPage_texture->Controls->Add(this->groupBox_individual_textures);
			this->tabPage_texture->Location = System::Drawing::Point(4, 22);
			this->tabPage_texture->Name = L"tabPage_texture";
			this->tabPage_texture->Size = System::Drawing::Size(488, 409);
			this->tabPage_texture->TabIndex = 1;
			this->tabPage_texture->Text = L"Texture";
			// 
			// label_UVA_texture_size
			// 
			this->label_UVA_texture_size->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label_UVA_texture_size->Location = System::Drawing::Point(280, 280);
			this->label_UVA_texture_size->Name = L"label_UVA_texture_size";
			this->label_UVA_texture_size->Size = System::Drawing::Size(192, 23);
			this->label_UVA_texture_size->TabIndex = 19;
			this->label_UVA_texture_size->Text = L"Texture Size = ";
			this->label_UVA_texture_size->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// button_UVA_send
			// 
			this->button_UVA_send->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->button_UVA_send->Location = System::Drawing::Point(384, 376);
			this->button_UVA_send->Name = L"button_UVA_send";
			this->button_UVA_send->Size = System::Drawing::Size(96, 23);
			this->button_UVA_send->TabIndex = 17;
			this->button_UVA_send->Text = L"To Animation-->";
			this->button_UVA_send->Click += gcnew System::EventHandler(this, &Form1::button_UVA_send_Click);
			// 
			// button_UVA_saveUVA
			// 
			this->button_UVA_saveUVA->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->button_UVA_saveUVA->Location = System::Drawing::Point(280, 376);
			this->button_UVA_saveUVA->Name = L"button_UVA_saveUVA";
			this->button_UVA_saveUVA->Size = System::Drawing::Size(96, 23);
			this->button_UVA_saveUVA->TabIndex = 16;
			this->button_UVA_saveUVA->Text = L"Save Texture";
			this->button_UVA_saveUVA->Click += gcnew System::EventHandler(this, &Form1::button_UVA_saveUVA_Click);
			// 
			// checkedListBox_UVA_resolution
			// 
			this->checkedListBox_UVA_resolution->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->checkedListBox_UVA_resolution->CheckOnClick = true;
			this->checkedListBox_UVA_resolution->Items->AddRange(gcnew cli::array< System::Object^  >(7) {L"4096", L"2048", L"1024", 
				L"512", L"256", L"128", L"64"});
			this->checkedListBox_UVA_resolution->Location = System::Drawing::Point(216, 290);
			this->checkedListBox_UVA_resolution->Name = L"checkedListBox_UVA_resolution";
			this->checkedListBox_UVA_resolution->Size = System::Drawing::Size(56, 109);
			this->checkedListBox_UVA_resolution->TabIndex = 15;
			this->checkedListBox_UVA_resolution->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::checkedListBox_UVA_resolution_SelectedIndexChanged);
			// 
			// pictureBox_UVA_composite
			// 
			this->pictureBox_UVA_composite->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->pictureBox_UVA_composite->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			this->pictureBox_UVA_composite->Location = System::Drawing::Point(216, 16);
			this->pictureBox_UVA_composite->Name = L"pictureBox_UVA_composite";
			this->pictureBox_UVA_composite->Size = System::Drawing::Size(256, 256);
			this->pictureBox_UVA_composite->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBox_UVA_composite->TabIndex = 14;
			this->pictureBox_UVA_composite->TabStop = false;
			// 
			// groupBox_individual_textures
			// 
			this->groupBox_individual_textures->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_individual_textures->Controls->Add(this->listBox_UVA_textures);
			this->groupBox_individual_textures->Controls->Add(this->button_UVA_down);
			this->groupBox_individual_textures->Controls->Add(this->button_UVA_add);
			this->groupBox_individual_textures->Controls->Add(this->button_UVA_del);
			this->groupBox_individual_textures->Controls->Add(this->pictureBox_UVA_anim);
			this->groupBox_individual_textures->Controls->Add(this->button_UVA_up);
			this->groupBox_individual_textures->Location = System::Drawing::Point(8, 8);
			this->groupBox_individual_textures->Name = L"groupBox_individual_textures";
			this->groupBox_individual_textures->Size = System::Drawing::Size(200, 392);
			this->groupBox_individual_textures->TabIndex = 20;
			this->groupBox_individual_textures->TabStop = false;
			this->groupBox_individual_textures->Text = L"Texture";
			// 
			// tabPage_tuv
			// 
			this->tabPage_tuv->Controls->Add(this->pictureBox_TUV_tmap);
			this->tabPage_tuv->Controls->Add(this->button_TUV_saveas);
			this->tabPage_tuv->Controls->Add(this->button_TUV_save);
			this->tabPage_tuv->Controls->Add(this->button_TUV_open);
			this->tabPage_tuv->Controls->Add(this->button_TUV_new);
			this->tabPage_tuv->Controls->Add(this->label_TUV_animtexture_filename);
			this->tabPage_tuv->Controls->Add(this->fileChooser_TUV_animtexture);
			this->tabPage_tuv->Controls->Add(this->checkBox_TUV_reversing);
			this->tabPage_tuv->Controls->Add(this->rangedFloat_TUV_rate);
			this->tabPage_tuv->Controls->Add(this->numericUpDown_TUV_xdiv);
			this->tabPage_tuv->Controls->Add(this->numericUpDown_TUV_ydiv);
			this->tabPage_tuv->Controls->Add(this->label_TUV_xdiv);
			this->tabPage_tuv->Controls->Add(this->label_TUV_ydiv);
			this->tabPage_tuv->Controls->Add(this->label_TUV_rate);
			this->tabPage_tuv->Controls->Add(this->checkBox_TUV_looping);
			this->tabPage_tuv->Controls->Add(this->button_TUV_reset);
			this->tabPage_tuv->Controls->Add(this->panel_TUV_render);
			this->tabPage_tuv->Controls->Add(this->panel_TUV_init);
			this->tabPage_tuv->Location = System::Drawing::Point(4, 22);
			this->tabPage_tuv->Name = L"tabPage_tuv";
			this->tabPage_tuv->Size = System::Drawing::Size(488, 409);
			this->tabPage_tuv->TabIndex = 0;
			this->tabPage_tuv->Text = L"Animation";
			// 
			// pictureBox_TUV_tmap
			// 
			this->pictureBox_TUV_tmap->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->pictureBox_TUV_tmap->Location = System::Drawing::Point(288, 144);
			this->pictureBox_TUV_tmap->Name = L"pictureBox_TUV_tmap";
			this->pictureBox_TUV_tmap->Size = System::Drawing::Size(128, 128);
			this->pictureBox_TUV_tmap->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBox_TUV_tmap->TabIndex = 23;
			this->pictureBox_TUV_tmap->TabStop = false;
			// 
			// button_TUV_saveas
			// 
			this->button_TUV_saveas->Location = System::Drawing::Point(312, 368);
			this->button_TUV_saveas->Name = L"button_TUV_saveas";
			this->button_TUV_saveas->Size = System::Drawing::Size(75, 23);
			this->button_TUV_saveas->TabIndex = 21;
			this->button_TUV_saveas->Text = L"Save As";
			this->button_TUV_saveas->Click += gcnew System::EventHandler(this, &Form1::button_TUV_saveas_Click);
			// 
			// button_TUV_save
			// 
			this->button_TUV_save->Location = System::Drawing::Point(216, 368);
			this->button_TUV_save->Name = L"button_TUV_save";
			this->button_TUV_save->Size = System::Drawing::Size(75, 23);
			this->button_TUV_save->TabIndex = 20;
			this->button_TUV_save->Text = L"Save";
			this->button_TUV_save->Click += gcnew System::EventHandler(this, &Form1::button_TUV_save_Click);
			// 
			// button_TUV_open
			// 
			this->button_TUV_open->Location = System::Drawing::Point(120, 368);
			this->button_TUV_open->Name = L"button_TUV_open";
			this->button_TUV_open->Size = System::Drawing::Size(75, 23);
			this->button_TUV_open->TabIndex = 19;
			this->button_TUV_open->Text = L"Open";
			this->button_TUV_open->Click += gcnew System::EventHandler(this, &Form1::button_TUV_open_Click);
			// 
			// button_TUV_new
			// 
			this->button_TUV_new->Location = System::Drawing::Point(24, 368);
			this->button_TUV_new->Name = L"button_TUV_new";
			this->button_TUV_new->Size = System::Drawing::Size(75, 23);
			this->button_TUV_new->TabIndex = 18;
			this->button_TUV_new->Text = L"New";
			this->button_TUV_new->Click += gcnew System::EventHandler(this, &Form1::button_TUV_new_Click);
			// 
			// label_TUV_animtexture_filename
			// 
			this->label_TUV_animtexture_filename->Location = System::Drawing::Point(16, 328);
			this->label_TUV_animtexture_filename->Name = L"label_TUV_animtexture_filename";
			this->label_TUV_animtexture_filename->Size = System::Drawing::Size(100, 23);
			this->label_TUV_animtexture_filename->TabIndex = 17;
			this->label_TUV_animtexture_filename->Text = L"Animation Texture";
			this->label_TUV_animtexture_filename->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// fileChooser_TUV_animtexture
			// 
			this->fileChooser_TUV_animtexture->Location = System::Drawing::Point(120, 328);
			this->fileChooser_TUV_animtexture->Name = L"fileChooser_TUV_animtexture";
			this->fileChooser_TUV_animtexture->Size = System::Drawing::Size(344, 24);
			this->fileChooser_TUV_animtexture->TabIndex = 16;
			this->fileChooser_TUV_animtexture->ValueChanged += gcnew System::EventHandler(this, &Form1::fileChooser_TUV_animtexture_ValueChanged);
			// 
			// panel_TUV_init
			// 
			this->panel_TUV_init->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->panel_TUV_init->Location = System::Drawing::Point(0, 0);
			this->panel_TUV_init->Name = L"panel_TUV_init";
			this->panel_TUV_init->Size = System::Drawing::Size(24, 24);
			this->panel_TUV_init->TabIndex = 22;
			this->panel_TUV_init->Visible = false;
			// 
			// comboBox_format
			// 
			this->comboBox_format->FormattingEnabled = true;
			this->comboBox_format->Items->AddRange(gcnew cli::array< System::Object^  >(5) {L"BMP", L"JPG", L"TGA", L"PNG", L"DDS"});
			this->comboBox_format->Location = System::Drawing::Point(282, 348);
			this->comboBox_format->Name = L"comboBox_format";
			this->comboBox_format->Size = System::Drawing::Size(94, 21);
			this->comboBox_format->TabIndex = 21;
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(512, 473);
			this->Controls->Add(this->tabControl_tools);
			this->Menu = this->mainMenu1;
			this->MinimumSize = System::Drawing::Size(512, 496);
			this->Name = L"Form1";
			this->Text = L"Texture Animation Studio";
			this->Closing += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::Form1_Closing);
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox_UVA_anim))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_TUV_xdiv))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_TUV_ydiv))->EndInit();
			this->tabControl_tools->ResumeLayout(false);
			this->tabPage_instructions->ResumeLayout(false);
			this->tabPage_instructions->PerformLayout();
			this->tabPage_texture->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox_UVA_composite))->EndInit();
			this->groupBox_individual_textures->ResumeLayout(false);
			this->tabPage_tuv->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox_TUV_tmap))->EndInit();
			this->ResumeLayout(false);

		}	
private: System::Void OnApplicationIdle(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void Form1_Load(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void Form1_Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e);
private: System::Void checkBox_TUV_looping_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void checkBox_TUV_reversing_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void numericUpDown_TUV_xdiv_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void numericUpDown_TUV_ydiv_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void rangedFloat_TUV_rate_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void button_TUV_reset_Click(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void button_UVA_add_Click(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void button_UVA_del_Click(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void button_UVA_up_Click(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void button_UVA_down_Click(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void listBox_UVA_textures_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void menuItem_exit_Click(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void menuItem_about_Click(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void fileChooser_TUV_animtexture_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void checkedListBox_UVA_resolution_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void button_UVA_saveUVA_Click(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void button_UVA_send_Click(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void button_TUV_new_Click(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void button_TUV_open_Click(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void button_TUV_save_Click(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void button_TUV_saveas_Click(System::Object ^  sender, System::EventArgs ^  e);

private: String^ GetExecutableVersion();
private: void display_UVA_composite();
private: void display_thumbnail(int i_Index);
private: void swap_listbox_items(int i_Index1, int i_Index2);
private: void set_UVAdata_from_controls();
private: void set_TUVcontrols_from_data();
private: void set_TUVdata_from_controls();
private: void layout_images();
private: Image^ load_image(String^ i_pFilename);
private: void set_valid_resolutions();
private: void set_state_UVA_buttons();
private: void set_state_TUV_buttons();
private: void load_animation_tab();

// member variables
//
private: tasDocument* m_pDocument;

private: bool m_bDisableNotify;
		 bool m_bInIdleCallback;
private: System::Int32 m_UVA_image_max_width;
private: tmaImageList^ m_pImageList;
};
}


