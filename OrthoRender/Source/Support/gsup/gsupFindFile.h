#pragma once

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
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


namespace Support
{
	/// <summary> 
	/// Summary for gsupFindFile
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class gsupFindFile : public System::Windows::Forms::Form
	{
	public: 
		gsupFindFile(void)
		{
			InitializeComponent();
		}
        
	public: void Configure(	const fsLocator& i_StartFolder, 
							const char* i_FileFilter )
		{
			fileChooser_findfile->Filter = gcnew System::String(i_FileFilter);
			fileChooser_findfile->Fullpath = tmaManagedStringUtils::LocatorToManagedString( i_StartFolder );
		}

	public: String^ GetFullpath()
			{
				return this->fileChooser_findfile->Fullpath;
			}
	public: 
		~gsupFindFile()
		{
			if (components)
			{
				delete components;
			}
		}
	private: TerawattManagedControls::FileChooser ^  fileChooser_findfile;
	private: System::Windows::Forms::Label ^  label_findfile;
	private: System::Windows::Forms::Button ^  button_ok;

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
			this->fileChooser_findfile = gcnew TerawattManagedControls::FileChooser();
			this->label_findfile = gcnew System::Windows::Forms::Label();
			this->button_ok = gcnew System::Windows::Forms::Button();
			this->SuspendLayout();
			// 
			// fileChooser_findfile
			// 
			this->fileChooser_findfile->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->fileChooser_findfile->Location = System::Drawing::Point(8, 39);
			this->fileChooser_findfile->Name = "fileChooser_findfile";
			this->fileChooser_findfile->Size = System::Drawing::Size(352, 24);
			this->fileChooser_findfile->TabIndex = 0;
			// 
			// label_findfile
			// 
			this->label_findfile->Location = System::Drawing::Point(16, 15);
			this->label_findfile->Name = "label_findfile";
			this->label_findfile->Size = System::Drawing::Size(128, 16);
			this->label_findfile->TabIndex = 1;
			this->label_findfile->Text = "Browse for the File";
			// 
			// button_ok
			// 
			this->button_ok->Location = System::Drawing::Point(148, 72);
			this->button_ok->Name = "button_ok";
			this->button_ok->Size = System::Drawing::Size(72, 24);
			this->button_ok->TabIndex = 2;
			this->button_ok->Text = "OK";
			this->button_ok->Click += gcnew System::EventHandler(this, &gsupFindFile::button_ok_Click);
			// 
			// gsupFindFile
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(368, 110);
			this->Controls->Add(this->button_ok);
			this->Controls->Add(this->label_findfile);
			this->Controls->Add(this->fileChooser_findfile);
			this->Name = "gsupFindFile";
			this->Text = "Find File";
			this->ResumeLayout(false);

		}		

	private: System::Void button_ok_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
			 }

	};
}
#endif // _MANAGED
