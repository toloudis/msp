#pragma once

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef GF_FILETRANSLATION_MGR
#include "Core/gf/gfFileTranslationMgr.hpp"
#endif
#ifndef	TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifndef MTRL_OPERATIONS_HPP
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#endif
#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::IO;


namespace StudioFramework {

	/// <summary>
	/// Summary for mtrlCopyTextures
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class mtrlCopyTextures : public System::Windows::Forms::Form
	{
	public:
		mtrlCopyTextures(const fsLocator& i_DestDir,
						 const std::vector<fsLocator>& i_Filenames)
		 : m_DestDir (i_DestDir),
		   m_Filenames(i_Filenames)
		{
			InitializeComponent();

			this->label_destinationDir->Text = tmaManagedStringUtils::LocatorToManagedString(i_DestDir);

			const int num_textures = i_Filenames.size();
			for (int i=0; i<num_textures; ++i)
			{
				this->checkedListBox1->Items->Add(get_string_for_file(i_Filenames[i]), true);
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~mtrlCopyTextures()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::CheckedListBox^  checkedListBox1;
	private: System::Windows::Forms::Button^  button_copyTextures;
	private: System::Windows::Forms::Button^  button_moveTextures;
	private: System::Windows::Forms::Button^  button_doNothing;

	private: 
		const fsLocator& m_DestDir;
	private: System::Windows::Forms::Button^  button_refresh;
			 const std::vector<fsLocator>& m_Filenames;

		System::String^ get_string_for_file(const fsLocator &i_Locator)
		{
			 fsLocator src_file(i_Locator);
			 gfFileTranslationMgr::ExpandLocator(src_file);
			 System::String^ fullpath = tmaManagedStringUtils::LocatorToManagedString(src_file);

			 System::String^ line;
			 if (this->checkBox_FullPath->Checked)
				line = fullpath;
			 else
				line = tmaManagedStringUtils::ItStringToManagedString(i_Locator.GetLastName());

			 fsLocator dest_file = m_DestDir;
			 dest_file.Push(i_Locator.GetLastName());
			 gfFileTranslationMgr::ExpandLocator(dest_file);
			 System::String^ destpath = tmaManagedStringUtils::LocatorToManagedString(dest_file);

			 FileInfo^ dest_info = gcnew FileInfo(destpath);
			 FileInfo^ src_info = gcnew FileInfo(fullpath);

			 if (dest_info->Exists)
			 {
				 if (dest_info->IsReadOnly)
					 line = String::Concat(line, " - dest is readonly");

				 if (src_info->Exists &&
					 src_info->LastWriteTime < dest_info->LastWriteTime)
				 {
					 line = String::Concat(line, " - dest is newer");
				 }
			 }
			 
			 if (!src_info->Exists)
				 line = String::Concat(line, " - source does not exist");
			 else if (src_info->IsReadOnly)
			 {
				 line = String::Concat(line, " - source is readonly");
			 }
			 
			 return line;
		}

		 void refresh_list()
		 {
			 this->checkedListBox1->BeginUpdate();
			 this->checkedListBox1->Items->Clear();
			 const int num_textures = m_Filenames.size();
			 for (int i=0; i<num_textures; ++i)
			 {
				this->checkedListBox1->Items->Add(get_string_for_file(m_Filenames[i]), true);
			 }
			 this->checkedListBox1->EndUpdate();
		 }

		 void do_copy(bool i_bDeleteSource)
		 {
			 const int num_textures = m_Filenames.size();
			 for (int i=0; i<num_textures; ++i)
			 {
				if (this->checkedListBox1->GetItemChecked(i))
				{
					fsLocator src_file = m_Filenames[i];
					fsLocator dest_file = m_DestDir;
					dest_file.Push(src_file.GetLastName());

					gfFileTranslationMgr::ExpandLocator(src_file);
					gfFileTranslationMgr::ExpandLocator(dest_file);
						
					mtrlOperations::CopyTextureFile(src_file, dest_file, i_bDeleteSource);
				}
			 }
		 }

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::CheckBox^  checkBox_FullPath;
	private: System::Windows::Forms::Label^  label_destinationDir;
	private: System::Windows::Forms::Label^  label3;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->checkedListBox1 = (gcnew System::Windows::Forms::CheckedListBox());
			this->button_copyTextures = (gcnew System::Windows::Forms::Button());
			this->button_moveTextures = (gcnew System::Windows::Forms::Button());
			this->button_doNothing = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->checkBox_FullPath = (gcnew System::Windows::Forms::CheckBox());
			this->label_destinationDir = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->button_refresh = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// checkedListBox1
			// 
			this->checkedListBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->checkedListBox1->FormattingEnabled = true;
			this->checkedListBox1->HorizontalScrollbar = true;
			this->checkedListBox1->Location = System::Drawing::Point(12, 58);
			this->checkedListBox1->Name = L"checkedListBox1";
			this->checkedListBox1->Size = System::Drawing::Size(376, 259);
			this->checkedListBox1->TabIndex = 0;
			// 
			// button_copyTextures
			// 
			this->button_copyTextures->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_copyTextures->Location = System::Drawing::Point(55, 352);
			this->button_copyTextures->Name = L"button_copyTextures";
			this->button_copyTextures->Size = System::Drawing::Size(96, 26);
			this->button_copyTextures->TabIndex = 1;
			this->button_copyTextures->Text = L"Copy Textures";
			this->button_copyTextures->UseVisualStyleBackColor = true;
			this->button_copyTextures->Click += gcnew System::EventHandler(this, &mtrlCopyTextures::button_copyTextures_Click);
			// 
			// button_moveTextures
			// 
			this->button_moveTextures->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_moveTextures->Location = System::Drawing::Point(164, 352);
			this->button_moveTextures->Name = L"button_moveTextures";
			this->button_moveTextures->Size = System::Drawing::Size(96, 26);
			this->button_moveTextures->TabIndex = 2;
			this->button_moveTextures->Text = L"Move Textures";
			this->button_moveTextures->UseVisualStyleBackColor = true;
			this->button_moveTextures->Click += gcnew System::EventHandler(this, &mtrlCopyTextures::button_moveTextures_Click);
			// 
			// button_doNothing
			// 
			this->button_doNothing->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_doNothing->Location = System::Drawing::Point(273, 352);
			this->button_doNothing->Name = L"button_doNothing";
			this->button_doNothing->Size = System::Drawing::Size(96, 26);
			this->button_doNothing->TabIndex = 3;
			this->button_doNothing->Text = L"Do Nothing";
			this->button_doNothing->UseVisualStyleBackColor = true;
			this->button_doNothing->Click += gcnew System::EventHandler(this, &mtrlCopyTextures::button_doNothing_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(9, 34);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(130, 13);
			this->label1->TabIndex = 4;
			this->label1->Text = L"Textures used by material:";
			// 
			// checkBox_FullPath
			// 
			this->checkBox_FullPath->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->checkBox_FullPath->AutoSize = true;
			this->checkBox_FullPath->Location = System::Drawing::Point(282, 34);
			this->checkBox_FullPath->Name = L"checkBox_FullPath";
			this->checkBox_FullPath->Size = System::Drawing::Size(102, 17);
			this->checkBox_FullPath->TabIndex = 5;
			this->checkBox_FullPath->Text = L"Show Full Paths";
			this->checkBox_FullPath->UseVisualStyleBackColor = true;
			this->checkBox_FullPath->CheckedChanged += gcnew System::EventHandler(this, &mtrlCopyTextures::checkBox_FullPath_CheckedChanged);
			// 
			// label_destinationDir
			// 
			this->label_destinationDir->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->label_destinationDir->AutoSize = true;
			this->label_destinationDir->Location = System::Drawing::Point(77, 10);
			this->label_destinationDir->Name = L"label_destinationDir";
			this->label_destinationDir->Size = System::Drawing::Size(0, 13);
			this->label_destinationDir->TabIndex = 6;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(12, 9);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(63, 13);
			this->label3->TabIndex = 7;
			this->label3->Text = L"Destination:";
			// 
			// button_refresh
			// 
			this->button_refresh->Location = System::Drawing::Point(158, 29);
			this->button_refresh->Name = L"button_refresh";
			this->button_refresh->Size = System::Drawing::Size(88, 23);
			this->button_refresh->TabIndex = 8;
			this->button_refresh->Text = L"Refresh";
			this->button_refresh->UseVisualStyleBackColor = true;
			this->button_refresh->Click += gcnew System::EventHandler(this, &mtrlCopyTextures::button_refresh_Click);
			// 
			// mtrlCopyTextures
			// 
			this->ClientSize = System::Drawing::Size(400, 392);
			this->Controls->Add(this->button_refresh);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label_destinationDir);
			this->Controls->Add(this->checkBox_FullPath);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button_doNothing);
			this->Controls->Add(this->button_moveTextures);
			this->Controls->Add(this->button_copyTextures);
			this->Controls->Add(this->checkedListBox1);
			this->Name = L"mtrlCopyTextures";
			this->Text = L"Copy or Move Textures";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

private: System::Void button_copyTextures_Click(System::Object^  sender, System::EventArgs^  e) {
			 const bool c_bDeleteSource = false;
			 this->do_copy(c_bDeleteSource);
			 this->Close();
		 }
private: System::Void button_moveTextures_Click(System::Object^  sender, System::EventArgs^  e) {
			 const bool c_bDeleteSource = true;
			 this->do_copy(c_bDeleteSource);
			 this->Close();
		 }
private: System::Void button_doNothing_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->Close();
		 }
private: System::Void checkBox_FullPath_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 this->refresh_list();
		 }
private: System::Void button_refresh_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->refresh_list();
		 }
};
}

#endif // _MANAGED
