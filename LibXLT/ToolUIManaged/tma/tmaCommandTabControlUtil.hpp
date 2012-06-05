#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaCommandTabControlUtil.hpp
//**
//**	Helper functions to easily add commands to the system properties
//**	dialog.
//**
//**	There are 2 ways to set up a system tab page:
//**
//**	1) with the GUI designer (see Camera system)
//**		a) add the tab page
//**			tmaDialogTabbedMgr::AddTabPage( "System", cmraSystemForm::FormInstance->GetTabPage(0) );
//**
//**		b) add commands to the tab page group box (drivers or shortcuts)
//**			tmaCommandTabControlUtil::AddShortcutButton( StudioFramework::cmraSystemForm::FormInstance->GetTabPage(0)->Name, pCmd );
//**			tmaCommandTabControlUtil::AddDriverButton( StudioFramework::cmraSystemForm::FormInstance->GetTabPage(0)->Name, pCmd );
//**
//**		c) remove the tab page
//**			tmaDialogTabbedMgr::RemoveTabPage( "System", cmraSystemForm::FormInstance->GetTabPage(0) );
//**
//**  2) by hand (see Point Light system)
//**		a) create and add the tab page
//**			System::Windows::Forms::TabPage^ pTP = tmaCommandTabControlUtil::CreateSystemTabPage( "Point Light" );
//**
//**		b) add commands to the tab page group box (drivers or shortcuts)
//**			tmaCommandTabControlUtil::AddShortcutButton( pTP->Name, pCmd );
//**
//**			Note: for drivers you need to create the command first and assign it a unique ID.
//**			the name must match a driver for this system.
//**
//**			Fix needed: deal with the hard-coded ID and driver name.
//**
//**			chnlCommandCreateDriver* pCmdCD = new chnlCommandCreateDriver();
//**			pCmdCD->SetDriverName("Color");
//**			cmaCommandMgr::Add( pCmdCD, 1000 );	// FIX: - need to generate this number somehow
//**			tmaCommandTabControlUtil::AddDriverButton( pTP->Name, pCmdCD, gcnew String(pCmdCD->GetDriverName().c_str()) );
//**
//**		c) remove the tab page
//**			tmaDialogTabbedMgr::RemoveTabPage( "System", "Point Light" );
//**
//**	StudioGPU
//**	Copyright(C) 2005 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_COMMANDTABCONTROLUTIL_HPP
//#error tmaCommandTabControlUtil.hpp multiply included
//#endif
//#define TMA_COMMANDTABCONTROLUTIL_HPP
//
//#ifndef TMA_CONTROL_MGR
//#include "ToolUIManaged/tma/tmaControlMgr.hpp"
//#endif
//#ifndef TMA_DIALOGTABBED_HPP
//#include "ToolUIManaged/tma/tmaDialogTabbed.hpp"
//#endif
//#ifndef TMA_DIALOGTABBEDMGR_HPP
//#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"
//#endif
//#ifndef TMA_MANAGEDCONTROLUTIL_HPP
//#include "ToolUIManaged/tma/tmaManagedControlUtil.hpp"
//#endif
//#ifndef TMA_SYSTEM_HPP
//#include "ToolUIManaged/tma/tmaSystem.hpp"
//#endif
//#ifndef CMA_COMMAND_HPP
//#include "Tool/cma/cmaCommand.hpp"
//#endif
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//#ifndef DBG_LOG_HPP
//#include "Core/dbg/dbgLog.hpp"
//#endif
//
////============================================================================
////============================================================================
//#ifdef _MANAGED
//
//using namespace System;
//using namespace System::Collections;
//using namespace System::Drawing;
//
//
////============================================================================
////	forward references
////============================================================================
//
//
////============================================================================
////============================================================================
//public ref class tmaCommandTabControlUtil
//{
//private:
//	const static int c_BUTTON_SIZE_X		= 80;
//	const static int c_BUTTON_SIZE_Y		= 30;
//	const static int c_BUTTON_OFFSET_X		= c_BUTTON_SIZE_X + 2;
//	const static int c_BUTTON_OFFSET_Y		= c_BUTTON_SIZE_Y + 2;
//	const static int c_GROUPBOX_OFFSET_X	= 4;
//	const static int c_GROUPBOX_OFFSET_Y	= 12;
//
//public: 
//	//----------------------------------------------------------------------------
//	//	add a shortcut button and return the object ID
//	//----------------------------------------------------------------------------
//	static int AddShortcutButton( String^ i_pTabPageName, cmaCommand* i_pCmd )
//	{
//		return AddShortcutButton( i_pTabPageName, i_pCmd, i_pCmd->GetTag().c_str() );
//	}
//
//	//----------------------------------------------------------------------------
//	//	Add a shortcut button and return the object ID.
//	//	This version allows you to set the Text to be displayed in the button.
//	//----------------------------------------------------------------------------
//	static int AddShortcutButton( String^ i_pTabPageName, 
//								  cmaCommand* i_pCmd,
//								  const char * i_pText )
//	{
//		tmaDialogTabbed^ dialog = tmaDialogTabbedMgr::GetDialogFromName("System");
//		TabPage^ pTP = dialog->GetTabPage(i_pTabPageName);
//
//		if ( pTP != nullptr )
//		{
//			//	get the drivers groupBox
//			System::Windows::Forms::GroupBox^ pGB = nullptr;
//			int count = pTP->Controls->Count;
//
//			int i;
//			for ( i=0; i < count ; i++ )
//			{
//				pGB = dynamic_cast<System::Windows::Forms::GroupBox^>(pTP->Controls[i]);
//
//				// TODO: - is there a way to fix this without hardcoding this?
//				if (	( pGB != nullptr )
//					&&	( String::CompareOrdinal( pGB->Text, "Shortcuts" ) == 0 ) )
//				{
//					break;
//				}
//			}
//
//			if ( pGB != nullptr )
//			{
//				//	create the button
//				System::Windows::Forms::Button^ pBtn;
//				pBtn = gcnew System::Windows::Forms::Button;
//				pBtn->Name = gcnew System::String(i_pCmd->GetTag().c_str());
//				pBtn->Text = gcnew System::String(i_pText);
//
//				//	set the button size and position
//				int num = pGB->Controls->Count;
//				pBtn->Location	= System::Drawing::Point( (num % 3)*c_BUTTON_OFFSET_X + c_GROUPBOX_OFFSET_X,
//														  (num / 3)*c_BUTTON_OFFSET_Y + c_GROUPBOX_OFFSET_Y);
//				pBtn->Size		= System::Drawing::Size(c_BUTTON_SIZE_X,c_BUTTON_SIZE_Y);
//
//				int objectID;
//				objectID = tmaControlMgr::Add( pBtn, i_pCmd );
//
//				//	add the button
//				pGB->Controls->Add( pBtn );
//
//				return objectID;
//			}
//		}
//
//		return -1;
//	}
//
//	//----------------------------------------------------------------------------
//	//	add a driver button and return the object ID
//	//----------------------------------------------------------------------------
//	static int AddDriverButton( String^ i_pTabPageName, cmaCommand* i_pCmd, String^ i_pDriverName )
//	{
//		tmaDialogTabbed^ dialog = tmaDialogTabbedMgr::GetDialogFromName("System");
//		TabPage^ pTP = dialog->GetTabPage(i_pTabPageName);
//
//		if ( pTP != nullptr )
//		{
//			//	get the drivers groupBox
//			System::Windows::Forms::GroupBox^ pGB = nullptr;
//			int count = pTP->Controls->Count;
//
//			int i;
//			for ( i=0; i < count ; i++ )
//			{
//				pGB = dynamic_cast<System::Windows::Forms::GroupBox^>(pTP->Controls[i]);
//
//				// TODO: - is there a way to fix this without hardcoding this?
//				if (	( pGB != nullptr )
//					&&	( String::CompareOrdinal( pGB->Text, "Drivers" ) == 0 ) )
//				{
//					break;
//				}
//			}
//
//			if ( pGB != nullptr )
//			{
//				//	create the button
//				System::Windows::Forms::Button^ pBtn;
//				pBtn = gcnew System::Windows::Forms::Button;
//				pBtn->Name = i_pDriverName;
//				pBtn->Text = i_pDriverName;
//
//				//	set the button size and position
//				int num = pGB->Controls->Count;
//				pBtn->Location	= System::Drawing::Point( (num % 3)*c_BUTTON_OFFSET_X + c_GROUPBOX_OFFSET_X,
//														  (num / 3)*c_BUTTON_OFFSET_Y + c_GROUPBOX_OFFSET_Y);
//				pBtn->Size		= System::Drawing::Size(c_BUTTON_SIZE_X,c_BUTTON_SIZE_Y);
//
//				//	add the control+ set the click event
//				int objectID;
//				objectID = tmaControlMgr::Add( pBtn, i_pCmd );
//
//				//	add the button
//				pGB->Controls->Add( pBtn );
//
//				return objectID;
//			}
//		}
//
//		return -1;
//	}
//
//	//----------------------------------------------------------------------------
//	//	EnableDriverButtons
//	//----------------------------------------------------------------------------
//	static void EnableDriverButtons( String^ i_pTabPageName, bool i_bEnable )
//	{
//		tmaDialogTabbed^ dialog = tmaDialogTabbedMgr::GetDialogFromName("System");
//		TabPage^ pTP = dialog->GetTabPage(i_pTabPageName);
//
//		if ( pTP != nullptr )
//		{
//			//	get the drivers groupBox
//			System::Windows::Forms::GroupBox^ pGB;
//			//pGB = pTP->Controls->("groupBox_drivers");
//
//			pGB->Enabled = i_bEnable;
//		}
//	}
//
//	//----------------------------------------------------------------------------
//	//	CreateSystemTabPage
//	//----------------------------------------------------------------------------
//	static System::Windows::Forms::TabPage^ CreateSystemTabPage( String^ i_pTabPageName )
//	{
//		tmaDialogTabbed^ dialog = tmaDialogTabbedMgr::GetDialogFromName("System");
//
//		//	create the tab page
//		System::Windows::Forms::TabPage^ pTP = gcnew System::Windows::Forms::TabPage;
//		pTP->Name = i_pTabPageName;
//		pTP->Text = i_pTabPageName;
//		dialog->AddTabPage( pTP );
//
//		// add group boxes to it
//		System::Windows::Forms::GroupBox^ pGB;
//		pGB = gcnew System::Windows::Forms::GroupBox;
//		pGB->Location = System::Drawing::Point(8, 8);
//		pGB->Name = "groupBox_drivers";
//		pGB->Size = System::Drawing::Size(256, c_BUTTON_OFFSET_Y*3 + c_GROUPBOX_OFFSET_Y);
//		pGB->TabIndex = 0;
//		pGB->TabStop = false;
//		pGB->Text = "Drivers";
//		pTP->Controls->Add( pGB );
//
//		System::Windows::Forms::GroupBox^ pGBs;
//		pGBs = gcnew System::Windows::Forms::GroupBox;
//		pGBs->Location = System::Drawing::Point(8, pGB->Height + 8 + 2);
//		pGBs->Name = "groupBox_shortcuts";
//		pGBs->Size = System::Drawing::Size(256, c_BUTTON_OFFSET_Y*3 + c_GROUPBOX_OFFSET_Y);
//		pGBs->TabIndex = 7;
//		pGBs->TabStop = false;
//		pGBs->Text = "Shortcuts";
//		pTP->Controls->Add( pGBs );
//
//		return pTP;
//	}
//};
//#endif // _MANAGED
