#pragma once

//#ifndef UNDOHISTORYDATA_HPP
//#include "Features/UndoHistory/UndoHistoryData.hpp"
//#endif
#ifndef UNDOHISTORYUTIL_HPP
#include "Features/UndoHistory/UndoHistoryUtil.hpp"
#endif

#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif
#ifndef UNDO_UNDOMGR_HPP
#include "Core/undo/undoUndoMgr.hpp"
#endif

#ifdef _MANAGED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Features
{
	/// <summary> 
	/// Summary for UndoHistoryForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class UndoHistoryForm : public System::Windows::Forms::Form
	{
	public: 
		static UndoHistoryForm^ FormInstance = nullptr;

		UndoHistoryForm(UndoHistoryData& i_Data)
		:	m_Data(i_Data)
		{
			InitializeComponent();

			UndoHistoryForm::FormInstance = this;

			UpdateList();
		}

		void UpdateList()
		{
			UndoHistoryUtil::BuildDataList( m_Data );

			listBox_history->Items->Clear();
			for (int i = m_Data.m_HistoryList.size() - 1; i >= 0; --i)
			{
				//String^ pItem = String::Format( "{0}{1}", ((m_Data.m_NextUndoIndex == i)? S"->":S"  "), gcnew String(m_Data.m_HistoryList[i].m_Name.c_str()) );
				//listBox_history->Items->Add( pItem );
				listBox_history->Items->Add( gcnew String(m_Data.m_HistoryList[i].m_Name.c_str()) );

				//	if there is a match in index then everything previous in the list can
				//	be undone and everything after can be redone
				if (m_Data.m_NextUndoIndex == i)
				{
					listBox_history->Items->Add( "-----------------------------" );
				}
			}

			float curr_mem = undoUndoMgr::GetCurrentMemoryUsage();
			float max_mem = undoUndoMgr::GetMaximumMemoryUsage();
			if ( max_mem > 0 )
			{
				statusBar_history->Text = System::String::Format( "memory {0:F2}/{1:F2} Kb", curr_mem, max_mem );
			}
			else
			{
				statusBar_history->Text = System::String::Format( "count {0}  memory {1:F2} Kb", m_Data.m_HistoryList.size(), curr_mem );
			}
		}

	protected: 
		~UndoHistoryForm()
		{
			if (UndoHistoryForm::FormInstance == this)
				UndoHistoryForm::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TabControl ^  tabControl_undo;
	private: System::Windows::Forms::TabPage ^  tabPage_history;
	private: System::Windows::Forms::StatusBar ^  statusBar_history;
	private: System::Windows::Forms::ListBox ^  listBox_history;
	private: System::Windows::Forms::Button ^  button_undo;
	private: System::Windows::Forms::Button ^  button_redo;

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
			this->tabControl_undo = gcnew System::Windows::Forms::TabControl();
			this->tabPage_history = gcnew System::Windows::Forms::TabPage();
			this->listBox_history = gcnew System::Windows::Forms::ListBox();
			this->statusBar_history = gcnew System::Windows::Forms::StatusBar();
			this->button_undo = gcnew System::Windows::Forms::Button();
			this->button_redo = gcnew System::Windows::Forms::Button();
			this->tabControl_undo->SuspendLayout();
			this->tabPage_history->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_undo
			// 
			this->tabControl_undo->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_undo->Controls->Add(this->tabPage_history);
			this->tabControl_undo->Location = System::Drawing::Point(6, 6);
			this->tabControl_undo->Name = "tabControl_undo";
			this->tabControl_undo->SelectedIndex = 0;
			this->tabControl_undo->Size = System::Drawing::Size(164, 247);
			this->tabControl_undo->TabIndex = 0;
			// 
			// tabPage_history
			// 
			this->tabPage_history->AutoScroll = true;
			this->tabPage_history->Controls->Add(this->listBox_history);
			this->tabPage_history->Location = System::Drawing::Point(4, 22);
			this->tabPage_history->Name = "tabPage_history";
			this->tabPage_history->Size = System::Drawing::Size(156, 221);
			this->tabPage_history->TabIndex = 0;
			this->tabPage_history->Text = "History";
			// 
			// listBox_history
			// 
			this->listBox_history->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->listBox_history->Location = System::Drawing::Point(0, 0);
			this->listBox_history->Name = "listBox_history";
			this->listBox_history->SelectionMode = System::Windows::Forms::SelectionMode::None;
			this->listBox_history->Size = System::Drawing::Size(156, 212);
			this->listBox_history->TabIndex = 0;
			// 
			// statusBar_history
			// 
			this->statusBar_history->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->statusBar_history->Dock = System::Windows::Forms::DockStyle::None;
			this->statusBar_history->Location = System::Drawing::Point(0, 288);
			this->statusBar_history->Name = "statusBar_history";
			this->statusBar_history->Size = System::Drawing::Size(176, 16);
			this->statusBar_history->TabIndex = 1;
			// 
			// button_undo
			// 
			this->button_undo->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_undo->Location = System::Drawing::Point(8, 256);
			this->button_undo->Name = "button_undo";
			this->button_undo->TabIndex = 2;
			this->button_undo->Text = "Undo";
			this->button_undo->Click += gcnew System::EventHandler(this, &UndoHistoryForm::button_undo_Click);
			// 
			// button_redo
			// 
			this->button_redo->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_redo->Location = System::Drawing::Point(88, 256);
			this->button_redo->Name = "button_redo";
			this->button_redo->TabIndex = 3;
			this->button_redo->Text = "Redo";
			this->button_redo->Click += gcnew System::EventHandler(this, &UndoHistoryForm::button_redo_Click);
			// 
			// UndoHistoryForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(176, 310);
			this->Controls->Add(this->button_redo);
			this->Controls->Add(this->button_undo);
			this->Controls->Add(this->statusBar_history);
			this->Controls->Add(this->tabControl_undo);
			this->Name = "UndoHistoryForm";
			this->ShowInTaskbar = false;
			this->Text = "Action History";
			this->Load += gcnew System::EventHandler(this, &UndoHistoryForm::UndoHistoryForm_Load);
			this->tabControl_undo->ResumeLayout(false);
			this->tabPage_history->ResumeLayout(false);
			this->ResumeLayout(false);

		}		

private:
	UndoHistoryData&	m_Data;
	tmaDialogMemory^	m_pMemory;

	private: System::Void UndoHistoryForm_Load(System::Object ^  sender, System::EventArgs ^  e)
			 {
				m_pMemory = gcnew tmaDialogMemory( this );

				UpdateList();
			 }

	private: System::Void button_redo_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 undoUndoMgr::Redo();
			 }

	private: System::Void button_undo_Click(System::Object ^  sender, System::EventArgs ^  e)
			{
				 undoUndoMgr::Undo();
			}
};
}

#endif // _MANAGED
