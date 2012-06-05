#pragma once

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif

#include <set>

#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace Features {

	/// <summary>
	/// Summary for LoadPrefsDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class LoadPrefsDialog : public System::Windows::Forms::Form
	{
	public:
		LoadPrefsDialog(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}
		System::Windows::Forms::Control^ GetTabPageMain()
		{
			return this->tabPage_loadPrefs;
		}

		// Fill in list box with filenames from the textures that were skipped:
		void SetMissingTextureList(const std::set<fsLocator>& i_Set)
		{
			std::set<fsLocator>::const_iterator it;
			for (it = i_Set.begin(); it != i_Set.end(); ++it)
			{
				this->listBox_missingTextures->Items->Add( tmaManagedStringUtils::LocatorToManagedString( *it ) );
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~LoadPrefsDialog()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TabControl^  tabControl_LoadPrefs;
	private: System::Windows::Forms::TabPage^  tabPage_loadPrefs;
	private: System::Windows::Forms::TabPage^  tabPage_missingTextures;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::ListBox^  listBox_missingTextures;

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
			this->tabControl_LoadPrefs = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_loadPrefs = (gcnew System::Windows::Forms::TabPage());
			this->tabPage_missingTextures = (gcnew System::Windows::Forms::TabPage());
			this->listBox_missingTextures = (gcnew System::Windows::Forms::ListBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tabControl_LoadPrefs->SuspendLayout();
			this->tabPage_missingTextures->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_LoadPrefs
			// 
			this->tabControl_LoadPrefs->Controls->Add(this->tabPage_loadPrefs);
			this->tabControl_LoadPrefs->Controls->Add(this->tabPage_missingTextures);
			this->tabControl_LoadPrefs->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl_LoadPrefs->Location = System::Drawing::Point(0, 0);
			this->tabControl_LoadPrefs->Name = L"tabControl_LoadPrefs";
			this->tabControl_LoadPrefs->SelectedIndex = 0;
			this->tabControl_LoadPrefs->Size = System::Drawing::Size(403, 268);
			this->tabControl_LoadPrefs->TabIndex = 0;
			// 
			// tabPage_loadPrefs
			// 
			this->tabPage_loadPrefs->AutoScroll = true;
			this->tabPage_loadPrefs->Location = System::Drawing::Point(4, 22);
			this->tabPage_loadPrefs->Name = L"tabPage_loadPrefs";
			this->tabPage_loadPrefs->Size = System::Drawing::Size(395, 242);
			this->tabPage_loadPrefs->TabIndex = 0;
			this->tabPage_loadPrefs->Text = L"Load Prefs";
			this->tabPage_loadPrefs->UseVisualStyleBackColor = true;
			// 
			// tabPage_missingTextures
			// 
			this->tabPage_missingTextures->Controls->Add(this->label1);
			this->tabPage_missingTextures->Controls->Add(this->listBox_missingTextures);
			this->tabPage_missingTextures->Location = System::Drawing::Point(4, 22);
			this->tabPage_missingTextures->Name = L"tabPage_missingTextures";
			this->tabPage_missingTextures->Size = System::Drawing::Size(395, 242);
			this->tabPage_missingTextures->TabIndex = 1;
			this->tabPage_missingTextures->Text = L"Missing Textures";
			this->tabPage_missingTextures->UseVisualStyleBackColor = true;
			// 
			// listBox_missingTextures
			// 
			this->listBox_missingTextures->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->listBox_missingTextures->FormattingEnabled = true;
			this->listBox_missingTextures->Location = System::Drawing::Point(7, 35);
			this->listBox_missingTextures->Name = L"listBox_missingTextures";
			this->listBox_missingTextures->Size = System::Drawing::Size(375, 186);
			this->listBox_missingTextures->TabIndex = 0;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(8, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(157, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Textures skipped when loading:";
			// 
			// LoadPrefsDialog
			// 
			this->ClientSize = System::Drawing::Size(403, 268);
			this->Controls->Add(this->tabControl_LoadPrefs);
			this->Name = L"LoadPrefsDialog";
			this->Text = L"Load Preferences";
			this->tabControl_LoadPrefs->ResumeLayout(false);
			this->tabPage_missingTextures->ResumeLayout(false);
			this->tabPage_missingTextures->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}

#endif // _MANAGED
