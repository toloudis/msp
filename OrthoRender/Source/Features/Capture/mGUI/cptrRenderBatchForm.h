#pragma once

#ifndef CPTR_RENDERBATCHDIALOGUTIL_HPP
#include "Features/Capture/cptrRenderBatchDialogUtil.hpp"
#endif
#ifndef CPTR_RENDERBATCHDATA_HPP
#include "Features/Capture/cptrRenderBatchData.hpp"
#endif
#ifndef CPTR_RENDERBATCHDATAPARSER_HPP
#include "Features/Capture/cptrRenderBatchDataParser.hpp"
#endif
#ifndef CPTR_RENDERBATCHDATAUTIL_HPP
#include "Features/Capture/cptrRenderBatchDataUtil.hpp"
#endif
#ifndef CH_Reader_HPP
#include "Core/ch/chReader.hpp"
#endif
#ifndef CH_Writer_HPP
#include "Core/ch/chWriter.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "Core/dbg/dbgLog.hpp"
#endif
#ifndef DOC_SINGLEDOCUMENTMGR_HPP
#include "Tool/doc/docSingleDocumentMgr.hpp"
#endif
#ifndef FS_FILESTREAM_HPP
#include "Core/fs/fsFileStream.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_FILEX_HPP
#include "Core/fs/fsFileX.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef GF_FILEBIN_HPP
#include "Core/gf/gfFileBin.hpp"
#endif
#ifndef GF_PATHS_HPP
#include "Core/gf/gfPaths.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/it/itStringUtil.hpp"
#endif
#ifndef GUI_FILEDIALOGUTILS_HPP
#include "Tool/gui/guiFileDialogUtils.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifdef _MANAGED

//
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


//
namespace StudioFramework
{
	/// <summary>
	/// Summary for cptrRenderBatchForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cptrRenderBatchForm : public System::Windows::Forms::Form
	{
	public:
		cptrRenderBatchForm(cptrRenderBatchData& i_Data) : m_Data(i_Data)
		{
			InitializeComponent();

			SetComponentInitialValues();

			set_capture_button();
		}

	protected:
		~cptrRenderBatchForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

	private: System::Windows::Forms::CheckedListBox ^  checkedListBox_scenes;
	private: System::Windows::Forms::Button ^  button_batchsave;
	private: System::Windows::Forms::Button ^  button_capturesetup;
	private: System::Windows::Forms::TextBox ^  textBox_batchfilename;
	private: System::Windows::Forms::Button ^  button_addscene;
	private: System::Windows::Forms::GroupBox ^  groupBox_batchfilename;
	private: System::Windows::Forms::Button ^  button_deletescene;
	private: System::Windows::Forms::Button ^  button_batchload;
	private: System::Windows::Forms::GroupBox^  groupBox_scenes;
	private: System::Windows::Forms::Label^  label_instructions;
	private: System::Windows::Forms::Button^  button_addcurrentscene;


	private: cptrRenderBatchData& m_Data;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->button_capturesetup = (gcnew System::Windows::Forms::Button());
			this->textBox_batchfilename = (gcnew System::Windows::Forms::TextBox());
			this->button_batchload = (gcnew System::Windows::Forms::Button());
			this->checkedListBox_scenes = (gcnew System::Windows::Forms::CheckedListBox());
			this->button_addscene = (gcnew System::Windows::Forms::Button());
			this->groupBox_batchfilename = (gcnew System::Windows::Forms::GroupBox());
			this->button_batchsave = (gcnew System::Windows::Forms::Button());
			this->button_deletescene = (gcnew System::Windows::Forms::Button());
			this->groupBox_scenes = (gcnew System::Windows::Forms::GroupBox());
			this->label_instructions = (gcnew System::Windows::Forms::Label());
			this->button_addcurrentscene = (gcnew System::Windows::Forms::Button());
			this->groupBox_batchfilename->SuspendLayout();
			this->groupBox_scenes->SuspendLayout();
			this->SuspendLayout();
			// 
			// button_capturesetup
			// 
			this->button_capturesetup->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->button_capturesetup->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->button_capturesetup->Location = System::Drawing::Point(268, 351);
			this->button_capturesetup->Name = L"button_capturesetup";
			this->button_capturesetup->Size = System::Drawing::Size(96, 23);
			this->button_capturesetup->TabIndex = 0;
			this->button_capturesetup->Text = L"Continue";
			this->button_capturesetup->Click += gcnew System::EventHandler(this, &cptrRenderBatchForm::button_capturesetup_Click);
			// 
			// textBox_batchfilename
			// 
			this->textBox_batchfilename->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_batchfilename->Location = System::Drawing::Point(16, 25);
			this->textBox_batchfilename->Name = L"textBox_batchfilename";
			this->textBox_batchfilename->Size = System::Drawing::Size(326, 20);
			this->textBox_batchfilename->TabIndex = 5;
			// 
			// button_batchload
			// 
			this->button_batchload->Location = System::Drawing::Point(16, 56);
			this->button_batchload->Name = L"button_batchload";
			this->button_batchload->Size = System::Drawing::Size(104, 22);
			this->button_batchload->TabIndex = 16;
			this->button_batchload->Text = L"Load Batch File";
			this->button_batchload->Click += gcnew System::EventHandler(this, &cptrRenderBatchForm::button_batchload_Click);
			// 
			// checkedListBox_scenes
			// 
			this->checkedListBox_scenes->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->checkedListBox_scenes->Location = System::Drawing::Point(16, 19);
			this->checkedListBox_scenes->Name = L"checkedListBox_scenes";
			this->checkedListBox_scenes->Size = System::Drawing::Size(326, 139);
			this->checkedListBox_scenes->TabIndex = 17;
			// 
			// button_addscene
			// 
			this->button_addscene->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_addscene->Location = System::Drawing::Point(15, 166);
			this->button_addscene->Name = L"button_addscene";
			this->button_addscene->Size = System::Drawing::Size(75, 23);
			this->button_addscene->TabIndex = 18;
			this->button_addscene->Text = L"Add Scene";
			this->button_addscene->Click += gcnew System::EventHandler(this, &cptrRenderBatchForm::button_addscene_Click);
			// 
			// groupBox_batchfilename
			// 
			this->groupBox_batchfilename->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_batchfilename->Controls->Add(this->button_batchsave);
			this->groupBox_batchfilename->Controls->Add(this->textBox_batchfilename);
			this->groupBox_batchfilename->Controls->Add(this->button_batchload);
			this->groupBox_batchfilename->Location = System::Drawing::Point(8, 248);
			this->groupBox_batchfilename->Name = L"groupBox_batchfilename";
			this->groupBox_batchfilename->Size = System::Drawing::Size(358, 96);
			this->groupBox_batchfilename->TabIndex = 19;
			this->groupBox_batchfilename->TabStop = false;
			this->groupBox_batchfilename->Text = L"Batch Filename";
			// 
			// button_batchsave
			// 
			this->button_batchsave->Location = System::Drawing::Point(131, 56);
			this->button_batchsave->Name = L"button_batchsave";
			this->button_batchsave->Size = System::Drawing::Size(104, 22);
			this->button_batchsave->TabIndex = 17;
			this->button_batchsave->Text = L"Save Batch File";
			this->button_batchsave->Click += gcnew System::EventHandler(this, &cptrRenderBatchForm::button_batchsave_Click);
			// 
			// button_deletescene
			// 
			this->button_deletescene->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_deletescene->Location = System::Drawing::Point(177, 166);
			this->button_deletescene->Name = L"button_deletescene";
			this->button_deletescene->Size = System::Drawing::Size(75, 23);
			this->button_deletescene->TabIndex = 20;
			this->button_deletescene->Text = L"Del Scene";
			this->button_deletescene->Click += gcnew System::EventHandler(this, &cptrRenderBatchForm::button_deletescene_Click);
			// 
			// groupBox_scenes
			// 
			this->groupBox_scenes->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_scenes->Controls->Add(this->button_addcurrentscene);
			this->groupBox_scenes->Controls->Add(this->checkedListBox_scenes);
			this->groupBox_scenes->Controls->Add(this->button_deletescene);
			this->groupBox_scenes->Controls->Add(this->button_addscene);
			this->groupBox_scenes->Location = System::Drawing::Point(8, 44);
			this->groupBox_scenes->Name = L"groupBox_scenes";
			this->groupBox_scenes->Size = System::Drawing::Size(356, 199);
			this->groupBox_scenes->TabIndex = 21;
			this->groupBox_scenes->TabStop = false;
			this->groupBox_scenes->Text = L"Scenes";
			// 
			// label_instructions
			// 
			this->label_instructions->AutoSize = true;
			this->label_instructions->Location = System::Drawing::Point(9, 19);
			this->label_instructions->Name = L"label_instructions";
			this->label_instructions->Size = System::Drawing::Size(341, 13);
			this->label_instructions->TabIndex = 22;
			this->label_instructions->Text = L"Add scenes and save as a batch.  Saved batches can be loaded later.";
			// 
			// button_addcurrentscene
			// 
			this->button_addcurrentscene->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_addcurrentscene->Location = System::Drawing::Point(96, 166);
			this->button_addcurrentscene->Name = L"button_addcurrentscene";
			this->button_addcurrentscene->Size = System::Drawing::Size(75, 23);
			this->button_addcurrentscene->TabIndex = 21;
			this->button_addcurrentscene->Text = L"Add Current";
			this->button_addcurrentscene->Click += gcnew System::EventHandler(this, &cptrRenderBatchForm::button_addcurrentscene_Click);
			// 
			// cptrRenderBatchForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(376, 386);
			this->Controls->Add(this->label_instructions);
			this->Controls->Add(this->groupBox_scenes);
			this->Controls->Add(this->button_capturesetup);
			this->Controls->Add(this->groupBox_batchfilename);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MinimizeBox = false;
			this->MinimumSize = System::Drawing::Size(304, 272);
			this->Name = L"cptrRenderBatchForm";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Batch Render";
			this->TopMost = true;
			this->Closing += gcnew System::ComponentModel::CancelEventHandler(this, &cptrRenderBatchForm::cptrRenderBatchForm_Closing);
			this->groupBox_batchfilename->ResumeLayout(false);
			this->groupBox_batchfilename->PerformLayout();
			this->groupBox_scenes->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
private: System::Void button_capturesetup_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			SetDataValues();

			cptrRenderBatchDialogUtil::Hide();
			cptrRenderBatchDialogUtil::SetDialogClosing( true );

			this->Close();
		 }

private: System::Void button_addscene_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			std::string filter = "scene files (*.mab)|*.mab|All files (*.*)|*.*";
			fsLocator file_loc;
			fsLocator initial_dir;

			//	set initial dir to the scene dir or just "Shots"
			//
			initial_dir = gfPaths::GetAppPath();
			initial_dir.Push("Shots");

			if ( docSingleDocumentMgr::GetFilename().GetNumNames() > 0 )
			{
				initial_dir = docSingleDocumentMgr::GetFilename();
				initial_dir.Pop();
			}

			//	let the user choose
			if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
			{
				checkedListBox_scenes->Items->Add( tmaManagedStringUtils::LocatorToManagedString(file_loc), true );	// true = checked initially
			}

			set_capture_button();
		 }

private: System::Void button_deletescene_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 checkedListBox_scenes->Items->Remove( checkedListBox_scenes->SelectedItem );

			 set_capture_button();
		 }

private: System::Void button_addcurrentscene_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			fsLocator file_loc;
			if ( docSingleDocumentMgr::GetFilename().GetNumNames() > 0 )
			{
				file_loc = docSingleDocumentMgr::GetFilename();
				checkedListBox_scenes->Items->Add( tmaManagedStringUtils::LocatorToManagedString(file_loc), true );	// true = checked initially
			}

			set_capture_button();
		 }

private: System::Void button_batchsave_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			std::string filter = "scene batch files (*.scb)|*.scb|All files (*.*)|*.*";
			fsLocator file_loc;
			file_loc = gfPaths::GetAppPath();
			file_loc.Push("Footage");
			if (guiFileDialogUtils::GetSaveFileName(filter, file_loc))
			{
				std::string name;
				name = itStringUtil::GetStdString(file_loc.GetLastName());
				DBG_LOG1( "Saving batch file (%s)", name.c_str() );

				SaveFile(file_loc);

				UpdateFileDirectory( file_loc );
			}
		 }

private: System::Void button_batchload_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			std::string filter = "scene batch files (*.scb)|*.scb|All files (*.*)|*.*";
			fsLocator file_loc;
			fsLocator initial_dir;
			initial_dir = gfPaths::GetAppPath();
			initial_dir.Push("Footage");
			//fsFileUtil::UnicodeStringToLocator( itString( ".\\Data\\Levels" ), initial_dir );

			if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
			{
				std::string name;
				name = itStringUtil::GetStdString(file_loc.GetLastName());
				//DBG_LOG1( "Loading batch file (%s)", name.c_str() );

				ReadFile(file_loc);

				SetComponentValues();

				UpdateFileDirectory( file_loc );

				set_capture_button();
			}
		 }

private:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void set_capture_button()
	{
		if (   (m_Data.m_BatchFilename.GetValue().GetNumNames() > 0)
			|| (checkedListBox_scenes->Items->Count > 0))
		{
			button_capturesetup->Enabled = true;
		}
		else
		{
			button_capturesetup->Enabled = false;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadFile(const fsLocator &i_Loc)
	{
		try
		{
			cptrRenderBatchData& data = cptrRenderBatchDataUtil::Data();
			cptrRenderBatchDataUtil::ReadData( i_Loc, data );
			m_Data = data;
			//DBG_LOG1( "Read %d items", m_Data.m_Scenes.size() );
		}
		catch( const fsFileDoesntExistX& i_Ex )
		{		
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			DBG_LOG1("fsFileDoesntExistX: %s", filename.c_str());
			std::string msg = "File does not exist: " + filename;
			MessageBox::Show(gcnew System::String(msg.c_str()), "Error");
		}
		catch( const fsDirectoryDoesntExistX& i_Ex )
		{		
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			DBG_LOG1("fsDirectoryDoesntExistX: %s", filename.c_str());
			std::string msg = "Directory does not exist: " + filename;
			MessageBox::Show(gcnew System::String(msg.c_str()), "Error");
		}
		catch( ... )
		{		
			MessageBox::Show("General exception error", "Error");
			throw;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SaveFile(const fsLocator &i_Loc)
	{
		SetDataValues();

		// Create document file
		//
		if (fsFileUtil::FileExists(i_Loc))
		{
			fsFileUtil::DeleteFile(i_Loc);
		}

		fsFileUtil::CreateFile(i_Loc);

		try
		{
			cptrRenderBatchDataUtil::WriteData(i_Loc,m_Data);
		}
		catch( const fsReadOnlyX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			std::string msg = "File is read only: " + filename;
			MessageBox::Show(gcnew System::String(msg.c_str()), "Error");
		}
		catch( ... )
		{
			MessageBox::Show("General exception error", "Error");
			throw;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void UpdateFileDirectory( fsLocator& i_dir )
	{
		System::String^ dir = tmaManagedStringUtils::LocatorToManagedString( i_dir );
		textBox_batchfilename->Text = dir;

		cptrRenderBatchData& data = cptrRenderBatchDataUtil::Data();
		data.m_BatchFilename.SetValue(i_dir);
		m_Data.m_BatchFilename.SetValue(i_dir);

		std::string fname;
		fsFileUtil::LocatorToANSIFilename( data.m_BatchFilename.GetValue(), fname );
		DBG_LOG1("cap batch Batch (%s)", fname.c_str());
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void SetComponentInitialValues()
	{
		m_Data = cptrRenderBatchDataUtil::Data();
		m_Data.m_Scenes.clear();

		std::string sceneFilename;
		if ( docSingleDocumentMgr::GetFilename().GetNumNames() > 0 )
		{
			this->button_addcurrentscene->Enabled = true;
		}
		else
		{
			this->button_addcurrentscene->Enabled = false;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void SetDataValues()
	{
		int numitems = checkedListBox_scenes->Items->Count;

		m_Data.m_Scenes.clear();
		m_Data.m_Scenes.resize( numitems );

		//DBG_LOG1( "items in list box = %d", numitems );
		int i;
		for ( i = 0 ; i < numitems ; ++i )
		{
			fsLocator dir;
			tmaManagedStringUtils::ManagedStringToLocator( checkedListBox_scenes->Items[i]->ToString(), dir );
			m_Data.m_Scenes[i].m_Filename.SetValue(dir);
			m_Data.m_Scenes[i].m_bChecked.SetValue( checkedListBox_scenes->GetItemChecked(i) );

			std::string sceneFilename;
			sceneFilename = itStringUtil::GetStdString( m_Data.m_Scenes[i].m_Filename.GetValue().GetLastName() );

			//DBG_LOG3( "%02d - %s (%s)", i, 
			//	sceneFilename.c_str(), 
			//	(m_Data.m_Scenes[i].m_bChecked ? "true":"false") );
		}

		cptrRenderBatchDataUtil::UpdateData(m_Data);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void SetComponentValues()
	{
		textBox_batchfilename->Clear();
		checkedListBox_scenes->Items->Clear();

		//DBG_LOG1( "setting %d items", m_Data.m_Scenes.size() );

		int i;
		for ( i = 0 ; i < m_Data.m_Scenes.size() ; ++i )
		{
			System::String ^ filename;
			filename = tmaManagedStringUtils::LocatorToManagedString( m_Data.m_Scenes[i].m_Filename.GetValue() );
			checkedListBox_scenes->Items->Add(	filename,
												m_Data.m_Scenes[i].m_bChecked.GetValue() );
		}
	}

private: System::Void cptrRenderBatchForm_Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e)
		 {
			 //	if the dialog is not closing because of hitting "capture" then the "X" must have been hit
			 //
			 if (!cptrRenderBatchDialogUtil::IsDialogClosing())
			 {
				 cptrRenderBatchDialogUtil::SetDialogExitting(true);
			 }
		 }
};
}

#endif // _MANAGED
