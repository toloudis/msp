#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaDialogTabbedMgr.hpp
//**
//**      The tabbed dialog manager
//**
//**	StudioGPU
//**	Copyright(C) 2004 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_DIALOGTABBEDMGR_HPP
//#error tmaDialogTabbedMgr.hpp multiply included
//#endif
//#define TMA_DIALOGTABBEDMGR_HPP
//
//#ifndef TMA_DIALOGTABBED_HPP
//#include "ToolUIManaged/tma/tmaDialogTabbed.hpp"
//#endif
//
//#ifdef _MANAGED
//
////============================================================================
////	forward references
////============================================================================
//
////NOTE: this version uses a single dialog.  the next version should probably
////	have multiple dialogs
//
////============================================================================
////============================================================================
//public ref class tmaDialogTabbedMgr
//{
//public:
//	static System::Collections::Hashtable^ l_pDialogHash = nullptr;
//
//	//------------------------------------------------------------------------
//	//------------------------------------------------------------------------
//	static void Initialize()
//	{
//		if (!l_pDialogHash)
//			l_pDialogHash = gcnew System::Collections::Hashtable();
//	};
//
//	//------------------------------------------------------------------------
//	//------------------------------------------------------------------------
//	static void DeInitialize()
//	{
//		l_pDialogHash = nullptr;
//	};
//
//
//	//---------------------------------------------------------------------------
//	// Remove all Tab Pages from the dialog
//	//---------------------------------------------------------------------------
//	static void RemoveTabPages( System::String^ i_DialogName )
//	{
//		if (!l_pDialogHash) return;
//
//		tmaDialogTabbed^ dialog = GetDialogFromName(i_DialogName);
//
//		DBG_ASSERT0( dialog != nullptr, "Dialog pointer is null" );
//
//		dialog->RemoveTabPages();
//	}
//
//	//---------------------------------------------------------------------------
//	// Add Tab Page to the dialog with the given name
//	//---------------------------------------------------------------------------
//	static void AddTabPage( System::String^ i_DialogName, TabPage^ i_pTabPage )
//	{
//		if (!l_pDialogHash) return;
//
//		tmaDialogTabbed^ dialog = GetDialogFromName(i_DialogName);
//
//		DBG_ASSERT0( dialog != nullptr, "Dialog pointer is null" );
//
//		dialog->AddTabPage(i_pTabPage);
//	}
//
//	//---------------------------------------------------------------------------
//	// Add Tab Page to the dialog with the given name
//	//---------------------------------------------------------------------------
//	static void AddTabPage( System::String^ i_DialogName, System::String^ i_pTabPageName )
//	{
//		if (!l_pDialogHash) return;
//
//		tmaDialogTabbed^ dialog = GetDialogFromName(i_DialogName);
//
//		DBG_ASSERT0( dialog != nullptr, "Dialog pointer is null" );
//
//		TabPage^ pTabPage;
//		pTabPage = gcnew TabPage(i_pTabPageName);
//		pTabPage->AutoScroll = true;
//
//		dialog->AddTabPage(pTabPage);
//	}
//
//	//---------------------------------------------------------------------------
//	// Add Tab Page to the dialog with the given name
//	//---------------------------------------------------------------------------
//	static TabPage^ GetTabPage( System::String^ i_DialogName, System::String^ i_pTabPageName )
//	{
//		if (!l_pDialogHash) return nullptr;
//
//		tmaDialogTabbed^ dialog = GetDialogFromName(i_DialogName);
//
//		DBG_ASSERT0( dialog != nullptr, "Dialog pointer is null" );
//
//		return dialog->GetTabPage(i_pTabPageName);
//	}
//
//	//---------------------------------------------------------------------------
//	// Remove Tab Page from the dialog with the given name
//	//---------------------------------------------------------------------------
//	static void RemoveTabPage( System::String^ i_DialogName, TabPage^ i_pTabPage )
//	{
//		if (!l_pDialogHash) return;
//
//		tmaDialogTabbed^ dialog = GetDialogFromName(i_DialogName);
//
//		DBG_ASSERT0( dialog != nullptr, "Dialog pointer is null" );
//
//		dialog->RemoveTabPage(i_pTabPage);
//	}
//
//	//---------------------------------------------------------------------------
//	// Remove Tab Page from the dialog with the given name
//	//---------------------------------------------------------------------------
//	static void RemoveTabPage( System::String^ i_DialogName, System::String^ i_TabPageName )
//	{
//		if (!l_pDialogHash) return;
//
//		tmaDialogTabbed^ dialog = GetDialogFromName(i_DialogName);
//
//		DBG_ASSERT0( dialog != nullptr, "Dialog pointer is null" );
//
//		dialog->RemoveTabPage(i_TabPageName);
//	}
//
//	//---------------------------------------------------------------------------
//	// Create the dialog with the given name, assigning the given title.
//	//	Will not directly show the dialog.
//	//---------------------------------------------------------------------------
//	static void Create(System::String^ i_DialogName, System::String^ i_Title, bool i_bMultiline, bool i_bSelectTabOnAdd )
//	{
//		if (!l_pDialogHash) return;
//
//		tmaDialogTabbed^ dialog = GetDialogFromName(i_DialogName);
//
//		DBG_ASSERT0( dialog != nullptr, "Dialog pointer is null" );
//
//		dialog->SetTitle(i_Title);
//		dialog->SetMultiline( i_bMultiline );
//		dialog->SetOnAddSelect( i_bSelectTabOnAdd );
//	}
//
//	//---------------------------------------------------------------------------
//	//	Close the dialog allowing it to be freed
//	//---------------------------------------------------------------------------
//	static void Close(System::String^ i_DialogName )
//	{
//		if (!l_pDialogHash) return;
//
//		if (l_pDialogHash->Contains(i_DialogName))
//		{
//			tmaDialogTabbed^ pDialog = safe_cast<tmaDialogTabbed^>(l_pDialogHash[i_DialogName]);
//
//			l_pDialogHash->Remove(i_DialogName);
//
//			if (pDialog != nullptr)
//				delete pDialog;
//		}
//	}
//
//	//------------------------%---------------------------------------------------
//	// Show the dialog with the given name
//	//---------------------------------------------------------------------------
//	static void Show(System::String^ i_DialogName)
//	{
//		if (!l_pDialogHash) return;
//
//		tmaDialogTabbed^ dialog = GetDialogFromName(i_DialogName);
//
//		DBG_ASSERT0( dialog != nullptr, "Dialog pointer is null" );
//
//		dialog->Show();
//	}
//
//	//---------------------------------------------------------------------------
//	// Show modal dialog with the given name
//	//---------------------------------------------------------------------------
//	static void ShowDialog(System::String^ i_DialogName)
//	{
//		if (!l_pDialogHash) return;
//
//		tmaDialogTabbed^ dialog = GetDialogFromName(i_DialogName);
//
//		DBG_ASSERT0( dialog != nullptr, "Dialog pointer is null" );
//
//		dialog->ShowDialog();
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	static bool IsVisible(System::String^ i_DialogName)
//	{
//		tmaDialogTabbed^ dialog = GetDialogFromName(i_DialogName);
//
//		if (dialog == nullptr)
//			return false;
//
//		return dialog->IsVisible();
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	static tmaDialogTabbed^ GetDialogFromName(System::String^ i_DialogName)
//	{
//		// If the dialog has is null, then we are shutting down or have not
//		// initialized yet. So, don't take any action.
//		if (l_pDialogHash)
//		{
//			if (l_pDialogHash->Contains(i_DialogName))
//			{
//				return safe_cast<tmaDialogTabbed^>(l_pDialogHash[i_DialogName]);
//			}
//			else
//			{
//				tmaDialogTabbed^ dialog = gcnew tmaDialogTabbed(i_DialogName);
//				l_pDialogHash->Add(i_DialogName, dialog);
//				return dialog;
//			}
//		}
//		return nullptr;
//	}
//};
//#endif // _MANAGED
