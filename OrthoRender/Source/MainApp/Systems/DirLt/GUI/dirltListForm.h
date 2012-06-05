/********************************************************************************************\
**  dirltListForm.h
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#pragma once

#ifndef DIRLT_SCRIPTDATA_HPP
#include "dirltScriptData.hpp"
#endif

#ifndef DIRLT_OPERATIONS_HPP
#include "dirltOperations.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "dbgLog.hpp"
#endif
#ifndef MUI_MAINWINDOW_HPP
#include "muiMainWindow.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "tmaManagedConversionUtil.hpp"
#endif


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace GeneratedForms
{
	/// <summary>
	/// Summary for dirltListForm
	///
	/// WARNING: If you change the name of this class, yada yada...
	/// </summary>
	public __gc class dirltListForm : public System::Windows::Forms::Form
	{
	public:
		static dirltListForm* FormInstance = 0;
	private: System::Windows::Forms::ListBox *  listBox_lights;
	private: System::Windows::Forms::TabControl *  tabControl_dirlights;
	private: System::Windows::Forms::TabPage *  tabPage_dirlights;
	private: System::Windows::Forms::Button *  buttonDuplicate;

	public:
		// EventCallback for when a dialog to edit data needs to be opened
		typedef void (*EditLightDataCallback)();
		static EditLightDataCallback DoEditCallback = 0;


		dirltListForm()
		{
			m_bDisableNotify = true;
			InitializeComponent();
			m_bDisableNotify = false;
		}

		// Call this to update form data
		void Update(const dirltDirLightsData &i_Data)
		{
			//for ( int i=0; i < i_Data.m_Items.size(); i++ )
			//{
			//	DBG_LOG2( "Update %d (%s)", i, i_Data.m_Items[i].m_Name.c_str() );
			//}
			SetupData(i_Data);
		}

		void Select(int i_Index)
		{
			if (this->listBox_lights->SelectedIndex != i_Index)
				this->listBox_lights->SelectedIndex = i_Index;
		}

	protected:
		void Dispose(Boolean disposing)
		{
			// clear instance
			if (disposing && dirltListForm::FormInstance == this)
				dirltListForm::FormInstance = 0;

			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container* components;

		// Turns off notify callbacks when setting up form
	private: bool m_bDisableNotify;




	private: System::Windows::Forms::Button *  buttonNew;
	private: System::Windows::Forms::Button *  buttonEdit;
	private: System::Windows::Forms::Button *  buttonDelete;



		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->buttonDelete = new System::Windows::Forms::Button();
			this->buttonEdit = new System::Windows::Forms::Button();
			this->buttonNew = new System::Windows::Forms::Button();
			this->listBox_lights = new System::Windows::Forms::ListBox();
			this->tabControl_dirlights = new System::Windows::Forms::TabControl();
			this->tabPage_dirlights = new System::Windows::Forms::TabPage();
			this->buttonDuplicate = new System::Windows::Forms::Button();
			this->tabControl_dirlights->SuspendLayout();
			this->tabPage_dirlights->SuspendLayout();
			this->SuspendLayout();
			// 
			// buttonDelete
			// 
			this->buttonDelete->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonDelete->Location = System::Drawing::Point(144, 232);
			this->buttonDelete->Name = S"buttonDelete";
			this->buttonDelete->Size = System::Drawing::Size(64, 27);
			this->buttonDelete->TabIndex = 4;
			this->buttonDelete->Text = S"Delete";
			this->buttonDelete->Click += new System::EventHandler(this, buttonDelete_Click);
			// 
			// buttonEdit
			// 
			this->buttonEdit->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonEdit->Location = System::Drawing::Point(80, 232);
			this->buttonEdit->Name = S"buttonEdit";
			this->buttonEdit->Size = System::Drawing::Size(56, 27);
			this->buttonEdit->TabIndex = 3;
			this->buttonEdit->Text = S"Edit...";
			this->buttonEdit->Click += new System::EventHandler(this, buttonEdit_Click);
			// 
			// buttonNew
			// 
			this->buttonNew->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonNew->Location = System::Drawing::Point(13, 232);
			this->buttonNew->Name = S"buttonNew";
			this->buttonNew->Size = System::Drawing::Size(59, 27);
			this->buttonNew->TabIndex = 2;
			this->buttonNew->Text = S"New";
			this->buttonNew->Click += new System::EventHandler(this, buttonNew_Click);
			// 
			// listBox_lights
			// 
			this->listBox_lights->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->listBox_lights->Location = System::Drawing::Point(13, 14);
			this->listBox_lights->Name = S"listBox_lights";
			this->listBox_lights->Size = System::Drawing::Size(331, 199);
			this->listBox_lights->TabIndex = 1;
			this->listBox_lights->DoubleClick += new System::EventHandler(this, listBox_lights_DoubleClick);
			this->listBox_lights->SelectedIndexChanged += new System::EventHandler(this, listBox_lights_SelectedIndexChanged);
			// 
			// tabControl_dirlights
			// 
			this->tabControl_dirlights->Controls->Add(this->tabPage_dirlights);
			this->tabControl_dirlights->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl_dirlights->Location = System::Drawing::Point(0, 0);
			this->tabControl_dirlights->Name = S"tabControl_dirlights";
			this->tabControl_dirlights->SelectedIndex = 0;
			this->tabControl_dirlights->Size = System::Drawing::Size(368, 302);
			this->tabControl_dirlights->TabIndex = 1;
			// 
			// tabPage_dirlights
			// 
			this->tabPage_dirlights->Controls->Add(this->buttonDuplicate);
			this->tabPage_dirlights->Controls->Add(this->buttonEdit);
			this->tabPage_dirlights->Controls->Add(this->buttonDelete);
			this->tabPage_dirlights->Controls->Add(this->buttonNew);
			this->tabPage_dirlights->Controls->Add(this->listBox_lights);
			this->tabPage_dirlights->Location = System::Drawing::Point(4, 22);
			this->tabPage_dirlights->Name = S"tabPage_dirlights";
			this->tabPage_dirlights->Size = System::Drawing::Size(360, 276);
			this->tabPage_dirlights->TabIndex = 0;
			this->tabPage_dirlights->Text = S"Dir Lights";
			// 
			// buttonDuplicate
			// 
			this->buttonDuplicate->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonDuplicate->Location = System::Drawing::Point(216, 232);
			this->buttonDuplicate->Name = S"buttonDuplicate";
			this->buttonDuplicate->Size = System::Drawing::Size(72, 27);
			this->buttonDuplicate->TabIndex = 5;
			this->buttonDuplicate->Text = S"Duplicate";
			this->buttonDuplicate->Click += new System::EventHandler(this, buttonDuplicate_Click);
			// 
			// dirltListForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(368, 302);
			this->Controls->Add(this->tabControl_dirlights);
			this->Name = S"dirltListForm";
			this->Text = S"Dir Lights";
			this->tabControl_dirlights->ResumeLayout(false);
			this->tabPage_dirlights->ResumeLayout(false);
			this->ResumeLayout(false);

		}

		//

		System::String* GetName(const dirltScriptData &i_Data)
		{
			return new System::String( i_Data.m_BaseData.m_Name.GetString().c_str() );
		}

		void SetupData(const dirltDirLightsData &i_Data)
		{
			m_bDisableNotify = true;

			System::String* item_text;
			this->listBox_lights->Items->Clear();
			for (int i=0; i<i_Data.m_Items.size(); i++)
			{
				//DBG_LOG2( "%02d. dir lights (%s)", i, i_Data.m_Items[i].m_Name.c_str() );

				item_text = String::Copy( this->GetName( i_Data.m_Items[i] ) );
				this->listBox_lights->Items->Add( item_text );
			}
			//DBG_LOG0( "------------------- end of dir lights" );

			m_bDisableNotify = false;
		}

	private: System::Void buttonNew_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				dirltOperations::AddObject();

				// change focus to the main app
				muiMainWindow::Focus();
			 }

	private: System::Void buttonEdit_Click(System::Object *  sender, System::EventArgs *  e)
			{
				if (DoEditCallback)
				{
					int sel_index = this->listBox_lights->SelectedIndex;
					if (sel_index >= 0)
					{
						dirltOperations::SelectObject(sel_index);
						(*DoEditCallback)();
					}
				}
			}

	private: System::Void buttonDelete_Click(System::Object *  sender, System::EventArgs *  e)
			{
				int sel_index = this->listBox_lights->SelectedIndex;
				if (sel_index >= 0)
				{
					dirltOperations::DeleteObject(sel_index);
				}
			}

	private: System::Void buttonDuplicate_Click(System::Object *  sender, System::EventArgs *  e)
			{
				int sel_index = this->listBox_lights->SelectedIndex;
				if (sel_index >= 0)
				{
					dirltOperations::DuplicateDirLight(sel_index);
				}
			}

	private: System::Void listBox_lights_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
			{
				if (!m_bDisableNotify)
				{
					int sel_index = this->listBox_lights->SelectedIndex;
					if (sel_index >= 0)
					{
						dirltOperations::SelectObject(sel_index);
					}
				}
			}

private: System::Void listBox_lights_DoubleClick(System::Object *  sender, System::EventArgs *  e)
		 {
			if (!m_bDisableNotify)
			{
				int sel_index = this->listBox_lights->SelectedIndex;
				if (sel_index >= 0)
				{
					dirltOperations::SelectObject(sel_index);
				}
			}

			 // change focus to the main app
			 muiMainWindow::Focus();
		 }

	public:
    	//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage * GetTabPage( int i_Index )
		{
			return tabPage_dirlights;
		}


};
}
