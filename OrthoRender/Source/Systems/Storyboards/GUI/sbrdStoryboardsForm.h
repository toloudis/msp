/********************************************************************************************\
**  sbrdStoryboardsForm.h
**
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifndef SBRD_OPERATIONS_HPP
#include "Systems/Storyboards/Undo/sbrdOperations.hpp"
#endif
#ifndef SBRD_LISTDATA_HPP
#include "Systems/Storyboards/Data/sbrdListData.hpp"
#endif

#ifndef DBG_LOG_HPP
#include "Core/dbg/dbgLog.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef GF_PATHS_HPP
#include "Core/gf/gfPaths.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/it/itStringUtil.hpp"
#endif
#ifndef MNM_PATHS_HPP
#include "Support/mnm/mnmPaths.hpp"
#endif
#ifndef GUI_FILEDIALOGUTILS_HPP
#include "Tool/gui/guiFileDialogUtils.hpp"
#endif
#ifndef GUI_MESSAGEBOX_HPP
#include "Tool/gui/guiMessageBox.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif
#ifndef TMA_IMAGELIST_HPP
#include "ToolUIManaged/tma/tmaImageList.hpp"
#endif
#ifndef TMA_MANAGEDCONTROLUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedControlUtil.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
#endif

#include "Systems/Storyboards/GUI/sbrdStoryboardViewer.h"

#ifdef _MANAGED


//============================================================================
//============================================================================
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

//============================================================================
//============================================================================
namespace
{
const char *c_ImageFileFilter = "images files (*.bmp;*.png;*.dds))|*.bmp;*.png; *.dds|All files (*.*)|*.*";
}

//============================================================================
//============================================================================
namespace SystemStoryboards
{
	/// <summary>
	/// Summary for sbrdStoryboardsForm
	///
	/// WARNING: If you change the name of this class, yada yada...
	/// </summary>
	public ref class sbrdStoryboardsForm : public System::Windows::Forms::Form
	{
	public:
		static sbrdStoryboardsForm^ FormInstance = nullptr;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		sbrdStoryboardsForm()
			: m_bSelfEdit(false)
		{
			m_bDisableNotify = true;

			InitializeComponent();

			// Dialog memory remembers size, location, visiblity of dialog 
			m_pMemory = gcnew tmaDialogMemory( this );
			m_pImageList = gcnew tmaImageList();

			m_bDisableNotify = false;
		}

		//--------------------------------------------------------------------
		//	Update Form from the data
		//--------------------------------------------------------------------
		void UpdateForm( sbrdListData& i_ListData )
		{
			if (m_bDisableNotify)
				return;
			m_bDisableNotify = true;

			//	if the list contains no data, then clear the lists.  The Object Manager
			//	has already been called so the images are disposed of.
			//
			if (i_ListData.m_Filenames.size() == 0)
			{
				clear_storyboards();
				m_pImageList->RemoveAll();
				m_bDisableNotify = false;
				return;
			}

			//	reload all the images
			//
			fsLocator file_path( gfPaths::GetPath(mnmPaths::e_DataScene) );
			file_path.Push( "Storyboards");

			//	see what things are already in the list
			//
			const int count = m_pImageList->Count();
			ArrayList^ indices = gcnew ArrayList;
			indices->Capacity  = 0;//count);

			//	loop through and see which storyboards are loaded and which are not.
			//	The order is not guaranteed to match, so check them all.
			//
			for (int i = 0 ; i < i_ListData.m_Filenames.size() ; ++i)
			{
				std::string fstr;
				fstr = itStringUtil::GetStdString(i_ListData.m_Filenames[i].GetValue());
				DBG_LOG2("%d. %s", i, fstr.c_str());

				indices->Add(-1);

				for (int j = 0; j < m_pImageList->Count(); ++j)
				{
					//	if found, mark it otherwise load it
					if (i_ListData.m_Filenames[i].GetValue() == m_pImageList->GetImageData(j)->m_pImageFilename->GetLastName())
					{
						indices[i] = j;
					}
				}
			}

			//	now go through the list and see which ones are flagged as not loaded.
			//
			//	FIX - if there is already items, the filename won't match
			int k = 0;
			System::Collections::IEnumerator^ it = indices->GetEnumerator();
			while (it->MoveNext())
			{
				if ((it)->Current->Equals(-1))
				{
					file_path.Push( i_ListData.m_Filenames[k++].GetValue() );
					bool loaded = load_and_add_image( file_path );
					file_path.Pop();
				}
			}

			set_state_storyboard_buttons();
			m_bDisableNotify = false;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		TabPage^ GetTabPage()
		{
			return this->tabPage_storyboards;
		}

	protected:
		~sbrdStoryboardsForm()
		{
			// clear instance
			if (sbrdStoryboardsForm::FormInstance == this)
				sbrdStoryboardsForm::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}
private: System::ComponentModel::IContainer ^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


		// Holds reference to data, changing the data
		// within the caller's structure

private: System::Windows::Forms::TabControl ^  tabControl_storyboards;
private: System::Windows::Forms::TabPage ^  tabPage_storyboards;
private: System::Windows::Forms::Button ^  button_view;
private: System::Windows::Forms::Button ^  button_moveup;
private: System::Windows::Forms::Button ^  button_movedown;
private: System::Windows::Forms::ListBox ^  listBox_storyboards;
private: System::Windows::Forms::Button ^  button_add;
private: System::Windows::Forms::Button ^  button_delete;
private: System::Windows::Forms::PictureBox ^  pictureBox_storyboard;
private: System::Windows::Forms::CheckBox ^  checkBox_addto3dworld;

	private: bool m_bSelfEdit;
	private: bool m_bDisableNotify;
	private: tmaDialogMemory^ m_pMemory;
	private: tmaImageList^ m_pImageList;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->button_delete = gcnew System::Windows::Forms::Button();
			this->button_add = gcnew System::Windows::Forms::Button();
			this->tabControl_storyboards = gcnew System::Windows::Forms::TabControl();
			this->tabPage_storyboards = gcnew System::Windows::Forms::TabPage();
			this->pictureBox_storyboard = gcnew System::Windows::Forms::PictureBox();
			this->listBox_storyboards = gcnew System::Windows::Forms::ListBox();
			this->button_movedown = gcnew System::Windows::Forms::Button();
			this->button_moveup = gcnew System::Windows::Forms::Button();
			this->button_view = gcnew System::Windows::Forms::Button();
			this->checkBox_addto3dworld = gcnew System::Windows::Forms::CheckBox();
			this->tabControl_storyboards->SuspendLayout();
			this->tabPage_storyboards->SuspendLayout();
			this->SuspendLayout();
			// 
			// button_delete
			// 
			this->button_delete->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->button_delete->Location = System::Drawing::Point(312, 40);
			this->button_delete->Name = "button_delete";
			this->button_delete->TabIndex = 6;
			this->button_delete->Text = "Delete";
			this->button_delete->Click += gcnew System::EventHandler(this, &sbrdStoryboardsForm::button_delete_Click);
			// 
			// button_add
			// 
			this->button_add->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->button_add->Location = System::Drawing::Point(312, 8);
			this->button_add->Name = "button_add";
			this->button_add->TabIndex = 5;
			this->button_add->Text = "Add";
			this->button_add->Click += gcnew System::EventHandler(this, &sbrdStoryboardsForm::button_add_Click);
			// 
			// tabControl_storyboards
			// 
			this->tabControl_storyboards->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_storyboards->Controls->Add(this->tabPage_storyboards);
			this->tabControl_storyboards->Location = System::Drawing::Point(8, 8);
			this->tabControl_storyboards->Name = "tabControl_storyboards";
			this->tabControl_storyboards->SelectedIndex = 0;
			this->tabControl_storyboards->Size = System::Drawing::Size(408, 320);
			this->tabControl_storyboards->TabIndex = 8;
			// 
			// tabPage_storyboards
			// 
			this->tabPage_storyboards->Controls->Add(this->checkBox_addto3dworld);
			this->tabPage_storyboards->Controls->Add(this->pictureBox_storyboard);
			this->tabPage_storyboards->Controls->Add(this->listBox_storyboards);
			this->tabPage_storyboards->Controls->Add(this->button_movedown);
			this->tabPage_storyboards->Controls->Add(this->button_moveup);
			this->tabPage_storyboards->Controls->Add(this->button_view);
			this->tabPage_storyboards->Controls->Add(this->button_add);
			this->tabPage_storyboards->Controls->Add(this->button_delete);
			this->tabPage_storyboards->Location = System::Drawing::Point(4, 22);
			this->tabPage_storyboards->Name = "tabPage_storyboards";
			this->tabPage_storyboards->Size = System::Drawing::Size(400, 294);
			this->tabPage_storyboards->TabIndex = 0;
			this->tabPage_storyboards->Text = "Storyboards";
			// 
			// pictureBox_storyboard
			// 
			this->pictureBox_storyboard->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->pictureBox_storyboard->BackColor = System::Drawing::SystemColors::ControlLight;
			this->pictureBox_storyboard->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBox_storyboard->Location = System::Drawing::Point(205, 153);
			this->pictureBox_storyboard->Name = "pictureBox_storyboard";
			this->pictureBox_storyboard->Size = System::Drawing::Size(184, 130);
			this->pictureBox_storyboard->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBox_storyboard->TabIndex = 12;
			this->pictureBox_storyboard->TabStop = false;
			// 
			// listBox_storyboards
			// 
			this->listBox_storyboards->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->listBox_storyboards->Location = System::Drawing::Point(10, 8);
			this->listBox_storyboards->Name = "listBox_storyboards";
			this->listBox_storyboards->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->listBox_storyboards->Size = System::Drawing::Size(184, 277);
			this->listBox_storyboards->TabIndex = 11;
			this->listBox_storyboards->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &sbrdStoryboardsForm::listBox_storyboards_KeyDown);
			this->listBox_storyboards->SelectedIndexChanged += gcnew System::EventHandler(this, &sbrdStoryboardsForm::listBox_storyboards_SelectedIndexChanged);
			// 
			// button_movedown
			// 
			this->button_movedown->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->button_movedown->Location = System::Drawing::Point(204, 40);
			this->button_movedown->Name = "button_movedown";
			this->button_movedown->TabIndex = 10;
			this->button_movedown->Text = "Move Down";
			this->button_movedown->Click += gcnew System::EventHandler(this, &sbrdStoryboardsForm::button_movedown_Click);
			// 
			// button_moveup
			// 
			this->button_moveup->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->button_moveup->Location = System::Drawing::Point(204, 8);
			this->button_moveup->Name = "button_moveup";
			this->button_moveup->TabIndex = 9;
			this->button_moveup->Text = "Move Up";
			this->button_moveup->Click += gcnew System::EventHandler(this, &sbrdStoryboardsForm::button_moveup_Click);
			// 
			// button_view
			// 
			this->button_view->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->button_view->Location = System::Drawing::Point(204, 80);
			this->button_view->Name = "button_view";
			this->button_view->TabIndex = 8;
			this->button_view->Text = "Full View";
			this->button_view->Click += gcnew System::EventHandler(this, &sbrdStoryboardsForm::button_view_Click);
			// 
			// checkBox_addto3dworld
			// 
			this->checkBox_addto3dworld->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->checkBox_addto3dworld->Checked = true;
			this->checkBox_addto3dworld->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_addto3dworld->Location = System::Drawing::Point(204, 115);
			this->checkBox_addto3dworld->Name = "checkBox_addto3dworld";
			this->checkBox_addto3dworld->Size = System::Drawing::Size(120, 24);
			this->checkBox_addto3dworld->TabIndex = 13;
			this->checkBox_addto3dworld->Text = "Add To 3-D World";
			this->checkBox_addto3dworld->CheckedChanged += gcnew System::EventHandler(this, &sbrdStoryboardsForm::checkBox_addto3dworld_CheckedChanged);
			// 
			// sbrdStoryboardsForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(424, 334);
			this->Controls->Add(this->tabControl_storyboards);
			this->Name = "sbrdStoryboardsForm";
			this->ShowInTaskbar = false;
			this->Text = "Storyboards";
			this->tabControl_storyboards->ResumeLayout(false);
			this->tabPage_storyboards->ResumeLayout(false);
			this->ResumeLayout(false);

		}

		//

private: System::Void button_view_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 // TODO - if the user clicks view over and over new instances of the view
			 //	dialog will keep popping up.
			 //
			sbrdStoryboardViewer^ dialog = gcnew sbrdStoryboardViewer(m_pImageList, listBox_storyboards->SelectedIndex);
			dialog->Show();
			//dialog->ShowDialog();
			//delete dialog;
		 }

private: System::Void button_moveup_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (listBox_storyboards->SelectedItem != nullptr)
			{
				if (listBox_storyboards->SelectedIndex > 0)
				{
					swap_listbox_items(listBox_storyboards->SelectedIndex, listBox_storyboards->SelectedIndex-1);
				}
			}
		 }

private: System::Void button_movedown_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (listBox_storyboards->SelectedItem != nullptr)
			{
				if (listBox_storyboards->SelectedIndex < (listBox_storyboards->Items->Count-1))
				{
					swap_listbox_items(listBox_storyboards->SelectedIndex, listBox_storyboards->SelectedIndex+1);
				}
			}
		 }

private: System::Void listBox_storyboards_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			int index = this->listBox_storyboards->SelectedIndex;
			display_thumbnail(index);
		 }

private: System::Void listBox_storyboards_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
		 {
			 //	if delete key is pressed and it is okay for the user to delete this object
			 //
			 if ((e->KeyCode == Keys::Delete) && (button_delete->Enabled))
			 {
				delete_storyboard_if_possible();
			 }
		 }

private: System::Void button_add_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			// add to listBox_storyboards
			std::string filter = c_ImageFileFilter;
			fsLocator file_loc;
			fsLocator initial_dir;

			if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
			{
				if ( file_loc.GetNumNames() > 0 )
				{
					//std::string name;
					//name = itStringUtil::GetStdString(file_loc.GetLastName());

					//	loop and try to load a series of images based on the one selected
					//
					int initnum, numdigits;
					itString fname, basename, ext;
					fname = file_loc.GetLastName();
					numdigits = itStringUtil::Breakup_Filename(fname, basename, ext, initnum);

					int number = initnum;

					while (true)
					{
						//	if digits then build the filename
						if (numdigits > 0)
						{
							file_loc.Pop();
							itStringUtil::Build_Filename(basename, ext, number, numdigits, fname);
							file_loc.Push(fname);
						}

						bool loaded = load_and_add_image( file_loc );
						if (!loaded)
							break;

						sbrdOperations::AddStoryboard(fname);

						++number;

						set_state_storyboard_buttons();

						//	if no digits, then jump out since this is a single file being loaded
						if (numdigits == 0)
						{
							break;
						}
					}
 				}
			}
		 }

private: System::Void checkBox_addto3dworld_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
		 }

private: System::Void delete_storyboard_if_possible()
		 {
			// del to listBox_storyboards
			if (listBox_storyboards->SelectedItem != nullptr)
			{
				int index = listBox_storyboards->SelectedIndex;
				this->listBox_storyboards->Items->RemoveAt( index );
				this->m_pImageList->RemoveAt( index );

				sbrdOperations::RemoveStoryboard(index);

				//if (this->m_pImageList->Count == 0)
				//{
				//	m_UVA_image_max_width = 0;
				//}
			}

			set_state_storyboard_buttons();
		 }

private: System::Void button_delete_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
 			 delete_storyboard_if_possible();
		 }


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void set_state_storyboard_buttons()
{
	if (this->listBox_storyboards->Items->Count > 0)
	{
		this->button_delete->Enabled = true;
		this->button_movedown->Enabled = true;
		this->button_moveup->Enabled = true;
		this->button_view->Enabled = true;
	}
	else
	{
		this->button_delete->Enabled = false;
		this->button_movedown->Enabled = false;
		this->button_moveup->Enabled = false;
		this->button_view->Enabled = false;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void swap_listbox_items(int i_Index1, int i_Index2)
{
	//	switch both the listbox and the imagelist so they match
	//
	System::Object^ pObject = this->listBox_storyboards->Items[i_Index2];
	this->listBox_storyboards->Items->RemoveAt(i_Index2);
	this->listBox_storyboards->Items->Insert(i_Index1, pObject);

	System::Drawing::Image^ pImage1 = this->m_pImageList->Get(i_Index1);
	System::Drawing::Image^ pImage2 = this->m_pImageList->Get(i_Index2);
	this->m_pImageList->Set(i_Index1, pImage2);
	this->m_pImageList->Set(i_Index2, pImage1);

	sbrdOperations::SwapStoryboards(i_Index1, i_Index2);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void display_thumbnail(int i_Index)
{
	if (i_Index != -1)
	{
		this->pictureBox_storyboard->Image = this->m_pImageList->Get(i_Index);
		//DBG_LOG2("Storyboard THUMBNAIL image size (%d, %d)", this->pictureBox_storyboard->Image->Width, this->pictureBox_storyboard->Image->Height);
		this->pictureBox_storyboard->Invalidate();

		DBG_LOG1("displaying thumbnail %d", i_Index);
	}
	else
	{
		this->pictureBox_storyboard->Image = nullptr;
		this->pictureBox_storyboard->Invalidate();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
Image^ load_image(String^ i_pFilename)
{
	Image^ pImage;

	try
	{
		pImage = System::Drawing::Image::FromFile(i_pFilename);

		//DBG_LOG2("Storyboard LOAD image size (%d, %d)", pImage->Width, pImage->Height);

		int bpp = pImage->GetPixelFormatSize(pImage->PixelFormat);
		if (bpp < 24)
		{
			// TODO create our own exception for this case
			throw gcnew System::IO::FileLoadException;
		}
	}
	catch (System::IO::FileNotFoundException^)
	{
		String^ errormsg = String::Format("File {0} not found", i_pFilename);
		std::string stderrormsg;
		tmaManagedStringUtils::ManagedStringToStdString(errormsg, stderrormsg);
		guiMessageBox::Show(stderrormsg.c_str(), "File Not Found", guiMessageBox::e_OKOnly);
		return nullptr;
	}
	catch (System::IO::FileLoadException^)
	{
		String^ errormsg = String::Format("File {0} has an unsupported pixel format", i_pFilename);
		std::string stderrormsg;
		tmaManagedStringUtils::ManagedStringToStdString(errormsg, stderrormsg);
		guiMessageBox::Show(stderrormsg.c_str(), "Invalid File Format", guiMessageBox::e_OKOnly);
	}
	
	return pImage;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool load_and_add_image( fsLocator& i_FilePath )
{
	String^ pFullPath = tmaManagedStringUtils::LocatorToManagedString(i_FilePath);

	//	if the file isn't found then jump out
	if (!System::IO::File::Exists(pFullPath))
	{
		DBG_ERROR1("Image file doesn't exist (%s)", pFullPath->ToCharArray());
		return false;
	}

	System::Drawing::Image^ pImage = load_image(pFullPath);
	if (pImage == nullptr)
	{
		DBG_ERROR1("Cannot load image (%s)", pFullPath->ToCharArray());
		return false;
	}

	//if (m_UVA_image_max_width < pImage->Width)
	//{
	//	m_UVA_image_max_width = pImage->Width;
	//
	//	set_valid_resolutions();
	//}

	//if (this->m_pImageList->Count == 0)
	//	this->m_pImageListize = pImage->Size;

	int il_index = this->m_pImageList->Add( pImage, i_FilePath );
	//DBG_LOG2("Storyboard AFTER load image size (%d, %d)", pImage->Width, pImage->Height);

	this->listBox_storyboards->BeginUpdate();
	//int index = this->listBox_storyboards->Items->Add( gcnew System::String(name.c_str()) );
	int index = this->listBox_storyboards->Items->Add( pFullPath );
	this->listBox_storyboards->EndUpdate();

	DBG_LOG3("Image #%d-%d file added (%s)", index, il_index, pFullPath->ToCharArray());

	// set the new image as selected
	//int index = this->m_pImageList->Count-1;
	this->listBox_storyboards->SelectedIndex = index;
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void clear_storyboards()
{
	this->listBox_storyboards->Items->Clear();
	display_thumbnail(-1);

	set_state_storyboard_buttons();
}

};
}
#endif // _MANAGED
