#error THIS_FILE_IS_OBSOLETE

//*****************************************************************************
//**  tmaDialogTabbed.hpp
//**
//**      A dialog that had a tab control on it for holding tabpages.
//**
//**	StudioGPU
//**	Copyright(C) 2004 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_DIALOGTABBED_HPP
//#error tmaDialogTabbed.hpp multiply included
//#endif
//#define TMA_DIALOGTABBED_HPP
//
//#ifndef TMA_DIALOGTABBEDMEMORY_HPP
//#include "ToolUIManaged/tma/tmaDialogTabbedMemory.hpp"
//#endif
//#ifndef TMA_MANAGEDCONTROLUTIL_HPP
//#include "ToolUIManaged/tma/tmaManagedControlUtil.hpp"
//#endif
//#ifndef TMA_SYSTEM_HPP
//#include "ToolUIManaged/tma/tmaSystem.hpp"
//#endif
//
//#ifdef _MANAGED
//
//#ifndef PRTY_CONTROLMGR_HPP
//#include "ToolUIManaged/prtym/prtyControlMgr.hpp"
//#endif
//
////#ifndef TMA_MESSAGING_HPP
////#include "ToolUIManaged/tma/tmaMessaging.hpp"
////#endif
//#ifndef DBG_LOG_HPP
//#include "Core/dbg/dbgLog.hpp"
//#endif
//
//
////============================================================================
////============================================================================
//using namespace System;
//using namespace System::Windows::Forms;
//
//
////============================================================================
////	Create a form that can override keys to allow for command execution
////============================================================================
//public ref class tmaForm : public System::Windows::Forms::Form
//{
//public:
//	//--------------------------------------------------------------------
//	//--------------------------------------------------------------------
//	tmaForm() 
//	{
//		tmaInitializeComponents();
//	};
//	//--------------------------------------------------------------------
//	//--------------------------------------------------------------------
//	~tmaForm() {};
//
//	//----------------------------------------------------------------------------
//	//----------------------------------------------------------------------------
//	System::Void Form_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
//	{
//		//	Check for universal hot keys
//		//
//		int x = 0;
//		//if (e->Control)
//		//{
//		//	if (e->KeyCode == Keys::Z)
//		//	{
//		//		undoUndoMgr::Undo();
//		//		e->Handled = true;
//		//	}
//		//	else if (e->KeyCode == Keys::Y)
//		//	{
//		//		undoUndoMgr::Redo();
//		//		e->Handled = true;
//		//	}
//		//}
//	}
//
//	//--------------------------------------------------------------------
//	//	TO DO - why doesn't this function ever get called?  
//	//	It works for MainForm and chnlTraxEditor.
//	//--------------------------------------------------------------------
//protected: virtual bool ProcessCmdKey(Message% msg, Keys keyData) override
//	{
//		//bool bProcessed = tmaMessaging::ProcessCmdKey(msg, keyData);
//
//		return (/*bProcessed ||*/ (__super::ProcessCmdKey(msg,keyData)));
//	}
//
//	//----------------------------------------------------------------------------
//	//----------------------------------------------------------------------------
//	System::Void Form_MouseDown(System::Object ^  sender, System::Windows::Forms::MouseEventArgs ^  e)
//	{
//		int x = 0;
//	}
//
//	//----------------------------------------------------------------------------
//	//----------------------------------------------------------------------------
//	System::Void Form_Activated(System::Object ^  sender, System::EventArgs ^  e)
//	{
//		int x = 0;
//	}
//
//private:
//	//----------------------------------------------------------------------------
//	//----------------------------------------------------------------------------
//	void tmaInitializeComponents()
//	{
//		this->SuspendLayout();
//
//		this->ShowInTaskbar = false;
//		this->KeyPreview = true;
//
//		this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &tmaForm::Form_KeyDown);
//		this->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &tmaForm::Form_MouseDown);
//		this->Activated += gcnew System::EventHandler(this, &tmaForm::Form_Activated);
//
//		this->ResumeLayout(false);
//	}
//
//};
//
////============================================================================
////============================================================================
//public ref class tmaDialogTabbed
//{
//public:
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	tmaDialogTabbed(System::String^ i_pName)
//	:	m_pName(i_pName),
//		m_bOnAddSelect( false ),
//		m_pDialog(nullptr)
//	{
//		// Create tab control once and reuse it
//		m_pTabControl	= gcnew TabControl;
//
//		m_pTabControl->Visible = true;
//		m_pTabControl->Multiline = false;
//		m_pTabControl->Dock = System::Windows::Forms::DockStyle::Fill;
//		//m_pTabControl->Location = System::Drawing::Point(8, 16);
//		//m_pTabControl->Size = System::Drawing::Size(432, 272);
//
//		create_dialog();
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	~tmaDialogTabbed()
//	{
//		if (m_pDialog != nullptr)
//		{
//			// remove tab control from dialog before it closes
//			m_pDialog->Controls->Remove( m_pTabControl );
//		
//			delete m_pDialog;
//		}
//
//		delete m_pTabControl;
//	}
//
//	//---------------------------------------------------------------------------
//	//	On add of the tab page, select it.
//	//---------------------------------------------------------------------------
//	void SetOnAddSelect( const bool i_bOnAddSelect )
//	{
//		m_bOnAddSelect = i_bOnAddSelect;
//	}
//	
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void AddTabPage( TabPage^ i_pTabPage )
//	{
//		// Adds the tab pages to the TabControl
//		m_pTabControl->Controls->Add( i_pTabPage );
//
//		//
//		if ( m_bOnAddSelect )
//		{
//			m_pTabControl->SelectedIndex = (m_pTabControl->TabCount - 1);
//		}
//		i_pTabPage->Dock = System::Windows::Forms::DockStyle::Fill;
//	}
//	
//	//---------------------------------------------------------------------------
//	//	Remove the tab page (by pointer)
//	//---------------------------------------------------------------------------
//	void RemoveTabPage( TabPage^ i_pTabPage )
//	{
//		m_pTabControl->Controls->Remove( i_pTabPage );
//	}
//
//	//---------------------------------------------------------------------------
//	//	Remove the tab page (by name)
//	//---------------------------------------------------------------------------
//	void RemoveTabPage( System::String^ i_TabPageName )
//	{
//		TabPage^ pTP = GetTabPage( i_TabPageName );
//
//		if (pTP != nullptr)
//		{
//			m_pTabControl->Controls->Remove( pTP );
//		}
//	}
//
//	//---------------------------------------------------------------------------
//	//	Remove the tab pages
//	//---------------------------------------------------------------------------
//	void RemoveTabPages()
//	{
//		//	remove all the dynamic controls (prty) and dispose of the rest
//		//
//		if ((m_pTabControl) && (m_pTabControl->Controls))
//		{	
//			//DBG_LOG1("Before RemoveTabPages, prtyControlMgr::GetNumControls: %d", prtyControlMgr::GetNumControls());
//
//			for (int i = m_pTabControl->Controls->Count-1; i>=0; --i)
//			{
//				//	remove the control from the form first 
//				//
//				System::Windows::Forms::Control^ pTabPage = m_pTabControl->Controls[i];
//				m_pTabControl->Controls->Remove(pTabPage);
//
//				//	and then delete the control
//				for (int j=pTabPage->Controls->Count-1; j>=0; --j)
//				{
//					System::Windows::Forms::Control^ pCtrl = pTabPage->Controls[j];
//
//					// If the property control manager could delete the control,
//					// then remove it from the list. Otherwise, keep the control
//					// in the tab page so that it will be disposed when the tab page is
//					// deleted below. This is for the labels for the properties and 
//					// categories.
//					if (prtyControlMgr::DeleteControl( pCtrl ))
//					{
//						pTabPage->Controls->Remove(pCtrl);
//					}
//				}
//				delete pTabPage;
//			}
//			
//			//DBG_LOG1("After RemoveTabPages, prtyControlMgr::GetNumControls: %d", prtyControlMgr::GetNumControls());
//		}
//	}
//
//	//---------------------------------------------------------------------------
//	//	show dialog
//	//---------------------------------------------------------------------------
//	void Show()
//	{
//		create_dialog();
//		m_pDialog->Show();
//		if (m_pDialog->WindowState == FormWindowState::Minimized)
//			m_pDialog->WindowState = FormWindowState::Normal;
//		m_pDialog->BringToFront();
//	}
//
//	//---------------------------------------------------------------------------
//	//	show as a modal dialog
//	//---------------------------------------------------------------------------
//	void ShowDialog()
//	{
//		create_dialog();
//		m_pDialog->ShowDialog();
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void SetTitle(System::String ^i_Title)
//	{
//		if (m_pDialog != nullptr)
//			m_pDialog->Text = i_Title;
//		m_pTitle = i_Title;
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void SetMultiline( bool i_bMultiline )
//	{
//		m_pTabControl->Multiline = i_bMultiline;
//	}
//
//	//---------------------------------------------------------------------------
//	//	reluctantly give access to the tab page.
//	//---------------------------------------------------------------------------
//	TabPage^ GetTabPage( String^ i_pTabPageName )
//	{
//		System::Windows::Forms::TabPage^ pTabPage = nullptr;
//		int count = m_pTabControl->Controls->Count;
//
//		int i;
//		for ( i=0; i < count ; i++ )
//		{
//			pTabPage = dynamic_cast<System::Windows::Forms::TabPage^>(m_pTabControl->Controls[i]);
//			if (	( pTabPage != nullptr )
//				&&	( String::CompareOrdinal( pTabPage->Text, i_pTabPageName ) == 0 ) )
//			{
//				break;
//			}
//		}
//
//		return pTabPage;
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	bool IsVisible()
//	{
//		if (m_pDialog == nullptr)
//			return false;
//
//		return m_pDialog->Visible;
//	}
//
//private:
//	System::Windows::Forms::Form^		m_pDialog;
//	System::Windows::Forms::TabControl^	m_pTabControl;
//	tmaDialogTabbedMemory^				m_pMemory;
//	System::String^						m_pName;
//	System::String^						m_pTitle;
//
//	bool	m_bOnAddSelect;
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	System::Void dialog_Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e)
//	{
//		// remove tab control from dialog before it closes
//		m_pDialog->Controls->Remove( m_pTabControl );
//
//		m_pDialog = nullptr;
//		m_pMemory = nullptr;
//	}
//
//	//---------------------------------------------------------------------------
//	//	create the dialog if it hasn't been created yet.
//	//---------------------------------------------------------------------------
//	void create_dialog()
//	{
//		if ( m_pDialog == nullptr )
//		{
//			m_pDialog		= gcnew tmaForm; //Form;
//			
//			m_pDialog->SuspendLayout();
//
//			m_pDialog->Name = m_pName;
//			if (m_pTitle) m_pDialog->Text = m_pTitle;
//			m_pDialog->Size = System::Drawing::Size(450, 300);
//			m_pDialog->ShowInTaskbar = false;
//			m_pDialog->KeyPreview = true;
//			tmaSystem::g_pMainForm->AddOwnedForm(m_pDialog);
//
//			m_pDialog->Closing += gcnew System::ComponentModel::CancelEventHandler( this, &tmaDialogTabbed::dialog_Closing );
//
//			if ( m_pTabControl != nullptr )
//			{
//				//resize_dialog();
//				m_pDialog->Controls->Add( m_pTabControl );
//			}
//
//			m_pDialog->ResumeLayout(false);
//
//			if ( m_pMemory == nullptr )
//			{
//				m_pMemory = gcnew tmaDialogTabbedMemory( m_pDialog );
//			}
//		}
//	}
//
//
//	//---------------------------------------------------------------------------
//	//	set size of dialog based on tab control's size
//	//---------------------------------------------------------------------------
//	void resize_dialog()
//	{
//		System::Drawing::Size formSize			= m_pDialog->Size;
//		System::Drawing::Size tabctrlSize		= m_pTabControl->Size;
//		//DBG_LOG2( "tabctrl h=%d w=%d", tabctrlSize.get_Height(), tabctrlSize.get_Width() );
//		//DBG_LOG2( "dialog  h=%d w=%d", formSize.get_Height(), formSize.get_Width() );
//		resize_dialog(tabctrlSize);
//	}
//	void resize_dialog(System::Drawing::Size tabctrlSize)
//	{
//		System::Drawing::Size formSize;
//		formSize.Width = tabctrlSize.Width + 8;
//		formSize.Height = tabctrlSize.Height + 35;
//		m_pDialog->Size = formSize;
//	}
//};
//
//#endif // _MANAGED
