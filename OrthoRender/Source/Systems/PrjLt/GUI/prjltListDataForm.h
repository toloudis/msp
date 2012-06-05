/********************************************************************************************\
**  prjltListDataForm.h
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#pragma once

#ifndef PRJLT_SCRIPTDATA_HPP
#include "prjltScriptData.hpp"
#endif

#ifndef PRJLT_OPERATIONS_HPP
#include "prjltOperations.hpp"
#endif
#ifndef MUI_MAINWINDOW_HPP
#include "muiMainWindow.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "tmaManagedConversionUtil.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "dbgLog.hpp"
#endif


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace SystemProjectedLights
{
	/// <summary>
	/// Summary for prjltListDataForm
	///
	/// WARNING: If you change the name of this class, yada yada...
	/// </summary>
	public ref class prjltListDataForm : public System::Windows::Forms::Form
	{
	public:
		static prjltListDataForm^ FormInstance = nullptr;
	private: System::Windows::Forms::ListBox ^  listBox_lights;
	private: System::Windows::Forms::Button ^  buttonDuplicate;

	public:
		// EventCallback for when a dialog to edit data needs to be opened
		typedef void (*EditLightDataCallback)();
		static EditLightDataCallback DoEditCallback = 0;


		prjltListDataForm()
		{
			m_bDisableNotify = true;
			InitializeComponent();
			m_bDisableNotify = false;
		}

		// Call this to update form data
		void Update(const prjltProjectLightsData &i_Data)
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
		~prjltListDataForm()
		{
			// clear instance
			if (prjltListDataForm::FormInstance == this)
				prjltListDataForm::FormInstance = nullptr;

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

		// Turns off notify callbacks when setting up form
	private: bool m_bDisableNotify;

	private: System::Windows::Forms::TabControl ^  tabControl1;
	private: System::Windows::Forms::TabPage ^  tabPage1;

	private: System::Windows::Forms::Button ^  buttonNew;
	private: System::Windows::Forms::Button ^  buttonEdit;
	private: System::Windows::Forms::Button ^  buttonDelete;



		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->buttonDelete = gcnew System::Windows::Forms::Button();
			this->buttonEdit = gcnew System::Windows::Forms::Button();
			this->buttonNew = gcnew System::Windows::Forms::Button();
			this->listBox_lights = gcnew System::Windows::Forms::ListBox();
			this->tabControl1 = gcnew System::Windows::Forms::TabControl();
			this->tabPage1 = gcnew System::Windows::Forms::TabPage();
			this->buttonDuplicate = gcnew System::Windows::Forms::Button();
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->SuspendLayout();
			// 
			// buttonDelete
			// 
			this->buttonDelete->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonDelete->Location = System::Drawing::Point(144, 232);
			this->buttonDelete->Name = "buttonDelete";
			this->buttonDelete->Size = System::Drawing::Size(56, 27);
			this->buttonDelete->TabIndex = 4;
			this->buttonDelete->Text = "Delete";
			this->buttonDelete->Click += gcnew System::EventHandler(this, &prjltListDataForm::buttonDelete_Click);
			// 
			// buttonEdit
			// 
			this->buttonEdit->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonEdit->Location = System::Drawing::Point(80, 232);
			this->buttonEdit->Name = "buttonEdit";
			this->buttonEdit->Size = System::Drawing::Size(56, 27);
			this->buttonEdit->TabIndex = 3;
			this->buttonEdit->Text = "Edit...";
			this->buttonEdit->Click += gcnew System::EventHandler(this, &prjltListDataForm::buttonEdit_Click);
			// 
			// buttonNew
			// 
			this->buttonNew->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonNew->Location = System::Drawing::Point(16, 232);
			this->buttonNew->Name = "buttonNew";
			this->buttonNew->Size = System::Drawing::Size(56, 27);
			this->buttonNew->TabIndex = 2;
			this->buttonNew->Text = "New";
			this->buttonNew->Click += gcnew System::EventHandler(this, &prjltListDataForm::buttonNew_Click);
			// 
			// listBox_lights
			// 
			this->listBox_lights->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->listBox_lights->Location = System::Drawing::Point(13, 14);
			this->listBox_lights->Name = "listBox_lights";
			this->listBox_lights->Size = System::Drawing::Size(283, 186);
			this->listBox_lights->TabIndex = 1;
			this->listBox_lights->DoubleClick += gcnew System::EventHandler(this, &prjltListDataForm::listBox_lights_DoubleClick);
			this->listBox_lights->SelectedIndexChanged += gcnew System::EventHandler(this, &prjltListDataForm::listBox_lights_SelectedIndexChanged);
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl1->Location = System::Drawing::Point(0, 0);
			this->tabControl1->Name = "tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(320, 302);
			this->tabControl1->TabIndex = 1;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->buttonDuplicate);
			this->tabPage1->Controls->Add(this->buttonEdit);
			this->tabPage1->Controls->Add(this->buttonDelete);
			this->tabPage1->Controls->Add(this->buttonNew);
			this->tabPage1->Controls->Add(this->listBox_lights);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = "tabPage1";
			this->tabPage1->Size = System::Drawing::Size(312, 276);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = "Projected Lights";
			// 
			// buttonDuplicate
			// 
			this->buttonDuplicate->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonDuplicate->Location = System::Drawing::Point(208, 232);
			this->buttonDuplicate->Name = "buttonDuplicate";
			this->buttonDuplicate->Size = System::Drawing::Size(64, 27);
			this->buttonDuplicate->TabIndex = 5;
			this->buttonDuplicate->Text = "Duplicate";
			this->buttonDuplicate->Click += gcnew System::EventHandler(this, &prjltListDataForm::buttonDuplicate_Click);
			// 
			// prjltListDataForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(320, 302);
			this->Controls->Add(this->tabControl1);
			this->Name = "prjltListDataForm";
			this->Text = "Projected Lights";
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->ResumeLayout(false);

		}

		//

		System::String^ GetObjectName(const prjltData &i_Data)
		{
			// Add code here to give useful string name for this object
			return gcnew System::String( i_Data.m_Name.GetString().c_str() );
		}

		void SetupData(const prjltProjectLightsData &i_Data)
		{
			m_bDisableNotify = true;

			System::String^ item_text;
			this->listBox_lights->Items->Clear();
			for ( int i=0; i < i_Data.m_Items.size(); i++ )
			{
				//DBG_LOG2( "%02d. (%s)", i, i_Data.m_Items[i].m_Name.c_str() );

				item_text = String::Copy( this->GetObjectName( i_Data.m_Items[i].m_BaseData ) );

				this->listBox_lights->Items->Add( item_text );
			}
			//DBG_LOG0( "------------------- end of lights" );

			m_bDisableNotify = false;
		}

	private: System::Void buttonNew_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				prjltOperations::AddObject();

				// change focus to the main app
				muiMainWindow::Focus();
			 }

	private: System::Void buttonEdit_Click(System::Object ^  sender, System::EventArgs ^  e)
			{
				if (DoEditCallback)
				{
					int sel_index = this->listBox_lights->SelectedIndex;
					if (sel_index >= 0)
					{
						prjltOperations::SelectObject(sel_index);
						(*DoEditCallback)();
					}
				}
			}

	private: System::Void buttonDelete_Click(System::Object ^  sender, System::EventArgs ^  e)
			{
				int sel_index = this->listBox_lights->SelectedIndex;
				if (sel_index >= 0)
				{
					prjltOperations::DeleteObject(sel_index);
				}
			}

	private: System::Void buttonDuplicate_Click(System::Object ^  sender, System::EventArgs ^  e)
			{
				int sel_index = this->listBox_lights->SelectedIndex;
				if (sel_index >= 0)
				{
					prjltOperations::DuplicateObject(sel_index);
				}
			}

	private: System::Void listBox_lights_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
			{
				if (!m_bDisableNotify)
				{
					int sel_index = this->listBox_lights->SelectedIndex;
					if (sel_index >= 0)
					{
						prjltOperations::SelectObject(sel_index);
					}
				}
			}

private: System::Void listBox_lights_DoubleClick(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				int sel_index = this->listBox_lights->SelectedIndex;
				if (sel_index >= 0)
				{
					prjltOperations::SelectObject(sel_index);
				}
			}

			 // change focus to the main app
			 muiMainWindow::Focus();
		 }

	public:
    	//	get a projecteder to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage1;
		}


};
}
