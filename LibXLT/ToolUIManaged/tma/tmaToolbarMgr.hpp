#error THIS_FILE_IS_OBSOLETE

//*****************************************************************************
//**  tmaToolBarMgr.hpp
//**
//**      The main application toolstrip manager
//**
//**	StudioGPU
//**	Copyright(C) 2003-7 - All Rights Reserved
//\****************************************************************************/
//
//#ifdef TMA_TOOLBARMGR_HPP
//#error tmaToolBarMgr.hpp multiply included
//#endif
//#define TMA_TOOLBARMGR_HPP
//
//#ifndef TMA_MANAGEDCONTROLUTIL_HPP
//#include "ToolUIManaged/tma/tmaManagedControlUtil.hpp"
//#endif
//#ifndef TMA_MENUOBJECTSMGR_HPP
//#include "ToolUIManaged/tma/tmaMenuObjectsMgr.hpp"
//#endif
//#ifndef TMA_MENUOBJECTS_HPP
//#include "ToolUIManaged/tma/tmaMenuObjects.hpp"
//#endif
//#ifndef TMA_SYSTEM_HPP
//#include "ToolUIManaged/tma/tmaSystem.hpp"
//#endif
//
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
//using namespace System::Collections;
//using namespace System::Drawing;
//
//
////============================================================================
////	forward references
////============================================================================
////ref class tmaMenuObjects;
//
//
////============================================================================
////============================================================================
//public ref class tmaToolBarMgr
//{
//public:
//	static tmaToolBarMgr^ g_pMgr = nullptr;
//
//public:
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//tmaToolBarMgr()
//{
//}
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//~tmaToolBarMgr()
//{
//}
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//void Initialize()
//{
//}
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//void DeInitialize()
//{
//}
//
////---------------------------------------------------------------------------
////	create a toolstrip and add it to the mgr.
////
////	NOTE: this manager DOES NOT own the toolstrip, usually the form it is
////	added to is the owner.
////---------------------------------------------------------------------------
//System::Windows::Forms::ToolStrip ^ AddToolBar( const char * i_ToolStripName )
//{
//	System::Windows::Forms::ToolStrip ^  newtoolstrip;
//	newtoolstrip = gcnew System::Windows::Forms::ToolStrip();
//	newtoolstrip->BackColor = System::Drawing::SystemColors::Control;
//	newtoolstrip->Dock = System::Windows::Forms::DockStyle::None;
//	newtoolstrip->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 1, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
//		static_cast<System::Byte>(0)));
//	newtoolstrip->RenderMode = System::Windows::Forms::ToolStripRenderMode::Professional;
//	newtoolstrip->Name = gcnew System::String(i_ToolStripName);
//
//	newtoolstrip->Location = System::Drawing::Point(0,0); /*(7, 24);*/	// some default location
//	newtoolstrip->Size = System::Drawing::Size(111, 25);				// some default size
//
//	m_ToolStrips.Add( newtoolstrip );
//	return newtoolstrip;
//}
//
//
////
////	local (private) functions
////
//
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//System::Windows::Forms::ToolStrip^ get_toolstrip(System::Windows::Forms::Control::ControlCollection^ i_pCollection, 
//												 System::String^ i_toolstrip_name )
//{
//	int ccount = i_pCollection->Count;
//	for (int j=0; j < ccount; ++j)
//	{
//		System::Windows::Forms::ToolStrip^ ptoolstrip = dynamic_cast<System::Windows::Forms::ToolStrip^>(i_pCollection[j]);
//		if (ptoolstrip != nullptr)
//		{
//			if (String::CompareOrdinal( ptoolstrip->Name, i_toolstrip_name ) == 0)
//			{
//				return ptoolstrip;
//			}
//		}
//	}
//	return nullptr;
//}
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//System::Windows::Forms::ToolStrip^ get_toolstrip( System::String^ i_toolstrip_name )
//{
//	System::Windows::Forms::ToolStrip^ ptoolstrip = nullptr;
//	System::Windows::Forms::ToolStripContainer^ ptoolstripcontainer = nullptr;
//	int count = tmaSystem::g_pMainForm->Controls->Count;
//
//	//	check the main controls
//	//
//	ptoolstrip = get_toolstrip( tmaSystem::g_pMainForm->Controls, i_toolstrip_name );
//
//	//	check for toolstripcontainers and look through their controls
//	int ccount = tmaSystem::g_pMainForm->Controls->Count;
//	for (int i=0; i < ccount; ++i)
//	{
//		ptoolstripcontainer = dynamic_cast<System::Windows::Forms::ToolStripContainer^>(tmaSystem::g_pMainForm->Controls[i]);
//		if (ptoolstripcontainer != nullptr)
//		{
//			//this->toolStripContainer1->TopToolStripPanel->Controls->Add(this->toolBar_modes);
//			//ptoolstripcontainer->
//
//			//	if a container, search for panels...these contain the controls
//			int jcount = ptoolstripcontainer->Controls->Count;
//			for (int j=0; j < jcount; ++j)
//			{
//				System::Windows::Forms::ToolStripPanel^ ptspanel = dynamic_cast<System::Windows::Forms::ToolStripPanel^>(ptoolstripcontainer->Controls[j]);
//				if ( ptspanel != nullptr )
//				{
//					int kcount;
//					kcount = ptspanel->Controls->Count;
//					for (int k=0; k < kcount; ++k)
//					{
//						ptoolstrip = dynamic_cast<System::Windows::Forms::ToolStrip^>(ptspanel->Controls[k]);
//						if ( (ptoolstrip != nullptr) && (String::CompareOrdinal( ptoolstrip->Name, i_toolstrip_name ) == 0) )
//						{
//							return ptoolstrip;
//						}
//					}
//				}
//			}
//		}
//	}
//
//	return ptoolstrip;
//}
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//System::Windows::Forms::ToolStrip^ get_toolstrip( const char * i_toolstrip_name )
//{
//	return get_toolstrip( gcnew System::String( i_toolstrip_name ) );
//}
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//void add_toolstrip_button( tmaMenuObjects^ pMI, const char * i_ToolBarName, const char * i_ToolStripButton_ImageDir )
//{
//	DBG_ASSERT0( pMI != nullptr, "Invalid Menu Item in toolstrip button add" );
//
//	System::Windows::Forms::ToolStrip^ pTB = get_toolstrip( gcnew System::String(i_ToolBarName) );
//	if ( pTB == nullptr )
//	{
//		DBG_WARNING1( "Cannot find the menu item toolstrip (%s)", pMI->GetMenuItem()->Text );
//		return;
//	}
//
//	ToolStripButton ^ pTBB = gcnew ToolStripButton();
//
//	pTBB->Text = nullptr;
//	pTBB->ToolTipText = pMI->GetMenuItem()->Text;
//	//pTBB->set_Style(ToolStripButtonStyle::PushButton);
//	//pTBB->set_Visible( true );
//	//pTBB->set_Enabled( true );
//
//	//	if the image filename + dir has been passed in,
//	//	create the icon for the tbb
//	//
//	if ( i_ToolStripButton_ImageDir != nullptr )
//	{
//		tmaManagedControlUtil::Create_ToolStripButton_Image( pTBB, gcnew System::String(i_ToolStripButton_ImageDir), pTB );
//	}
//
//	pTBB->ImageScaling = ToolStripItemImageScaling::SizeToFit;
//	pTBB->ImageTransparentColor = System::Drawing::Color::FromArgb(255,0,255);
//
//	//	set the tbb
//	pMI->SetToolStripButton( pTBB );
//
//	// Add the ToolStripButton controls to the ToolStrip.
//	//
//	pTB->Items->Add( pTBB );
//
//	// hook-up one and only one callback
//	//
//	//if ( pTB->Buttons->Count == 1 )
//	//{
//	//	pTBB->Parent->ButtonClick += new ToolStripButtonClickEventHandler( (pMI), tmaMenuObjects::CallbackTBB );
//	//}
//}
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//void remove_toolstrip_button( const char * i_ToolBarName, tmaMenuObjects^ i_pMI )
//{
//	DBG_ASSERT0( i_pMI != nullptr, "cannot remove toolstrip from null menu item" );
//
//	if ( i_pMI->GetToolStripButton() == nullptr )
//		return;
//
//	System::Windows::Forms::ToolStrip^ pTB = get_toolstrip( gcnew System::String(i_ToolBarName) );
//
//	//	find out if the button exists in either toolstrip + remove it.
//	//
//	if ( (pTB != nullptr) || !pTB->Items->Contains( i_pMI->GetToolStripButton() ) )
//	{
//		DBG_ASSERT1( pTB != nullptr, "This app has no toolstrip (%s)", i_ToolBarName );
//	}
//
//	pTB->Items->Remove( i_pMI->GetToolStripButton() );
//}
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//void enable_toolstrip_button( tmaMenuObjects^ i_pMI, bool i_bEnable )
//{
//	DBG_ASSERT0( i_pMI != nullptr, "cannot remove toolstrip from null menu item" );
//
//	if ( i_pMI->GetToolStripButton() == nullptr )
//		return;
//
//	i_pMI->GetToolStripButton()->Enabled = i_bEnable;
//}
//
//
////===========================================================================
////	tmatoolstripMgr functions
////===========================================================================
//
////--------------------------------------------------------------------
//// Execution event handler
////--------------------------------------------------------------------
//void toolstrip_ButtonClick( System::Object^ Sender, System::EventArgs^ e )
//{
//	ToolStripButton^	pTSB	= dynamic_cast<ToolStripButton^>(Sender);
//
//	tmaMenuObjects^ pMO = nullptr;
//	if (pTSB != nullptr)
//		pMO = tmaMenuObjectsMgr::g_pMgr->get_menuitem( pTSB );
//
//	if ( pMO && (pMO->GetCallback()) )
//	{
//		(*(pMO->GetCallback()))( pMO->GetID() );
//	}
//}
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//void AttachHandler( tmaMenuObjects^ i_pMO )
//{
//	ToolStripButton^ tbb = i_pMO->GetToolStripButton();
//	if (tbb != nullptr)
//	{
//		System::EventHandler^ handler = gcnew System::EventHandler( this, &tmaToolBarMgr::toolstrip_ButtonClick );
//		tbb->Click -= handler;
//		tbb->Click += handler;
//	}
//}
//private:
//	ArrayList m_ToolStrips;
//};
//#endif // _MANAGED
//
