#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaMenuMgr.hpp
//**
//**      The main application menu manager
//**
//**	StudioGPU
//**	Copyright(C) 2003 - All Rights Reserved
//\****************************************************************************/
//
//#ifdef TMA_MENUMGR_HPP
//#error tmaMenuMgr.hpp multiply included
//#endif
//#define TMA_MENUMGR_HPP
//
//#ifndef TMA_MANAGEDSTRINGUTILS_HPP
//#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
//#endif
//#ifndef TMA_MENUOBJECTSMGR_HPP
//#include "ToolUIManaged/tma/tmaMenuObjectsMgr.hpp"
//#endif
//#ifndef TMA_SYSTEM_HPP
//#include "ToolUIManaged/tma/tmaSystem.hpp"
//#endif
//#ifndef TMA_TOOLBARMGR_HPP
//#include "ToolUIManaged/tma/tmaToolbarMgr.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//
//
////============================================================================
////============================================================================
//#ifdef _MANAGED
//
//using namespace System::Collections;
//using namespace System::ComponentModel;
//using namespace System::Windows::Forms;
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
//public ref class tmaMenuMgr
//{
//public:
//	static tmaMenuMgr^ g_pMgr = nullptr;
//
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	tmaMenuMgr()
//	{
//		l_pMenuObjects = gcnew ArrayList();
//		m_IconDir = nullptr;
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	~tmaMenuMgr()
//	{
//		//delete l_pMenuObjects;
//	}
//
//
//	//
//	//	local (private) functions
//	//
//
//	//---------------------------------------------------------------------------
//	//	generate a menu item
//	//---------------------------------------------------------------------------
//	tmaMenuObjects^ generate_menuitem(ToolStripItem^ i_pTSI)
//	{
//		tmaMenuObjects^ pMI = tmaMenuObjectsMgr::g_pMgr->generate_menuitem();
//		
//		//	if a toolstripitem was passed in, then use it otherwise create a new one
//		if (i_pTSI == nullptr)
//		{
//			System::Windows::Forms::ToolStripMenuItem^ pSWFMI = gcnew System::Windows::Forms::ToolStripMenuItem;
//			pSWFMI->ShowShortcutKeys = true;
//			pMI->SetMenuItem( pSWFMI );
//		}
//		else
//		{
//			dynamic_cast<ToolStripMenuItem^>(i_pTSI)->ShowShortcutKeys = true;
//			pMI->SetMenuItem( dynamic_cast<ToolStripMenuItem^>(i_pTSI) );
//		}
//
//		return pMI;
//	}
//
//
//	//===========================================================================
//	//	tmaMenuMgr functions
//	//===========================================================================
//
//	//---------------------------------------------------------------------------
//	// Execution event handler
//	//---------------------------------------------------------------------------
//	void menuItem_Click( Object^ Sender, System::EventArgs^ e )
//	{
//		//System::Windows::Forms::MessageBox::Show( "menu item clicked!" );
//
//		tmaMenuObjects^ pmmaMI = tmaMenuObjectsMgr::g_pMgr->get_menuitem( dynamic_cast<ToolStripMenuItem^>(Sender) );
//		if ( pmmaMI->GetCallback() )
//		{
//			(*(pmmaMI->GetCallback()))( pmmaMI->GetID() );
//		}
//	}
//	//---------------------------------------------------------------------------
//	// Popup event handler
//	//---------------------------------------------------------------------------
//	void menuItem_Popup(System::Object ^  Sender, System::EventArgs ^  e)
//	{
//		// Popup is called on the parent, but Update is registered on the child items.
//		// Go through the 
//		ToolStripMenuItem^ parent = dynamic_cast<ToolStripMenuItem^>(Sender);
//		int num_kids = parent->DropDownItems->Count;
//		for (int k=0; k<num_kids; k++)
//		{
//			ToolStripMenuItem ^child = dynamic_cast<ToolStripMenuItem^>(parent->DropDownItems[k]);
//
//			tmaMenuObjects^ pItem = tmaMenuObjectsMgr::g_pMgr->get_menuitem( child );
//			if ( pItem && pItem->GetUpdateCallback() )
//			{
//				(*(pItem->GetUpdateCallback()))( pItem->GetID() );
//			}
//		}
//	}
//
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void Initialize()
//	{
//		tmaMenuObjectsMgr::g_pMgr = gcnew tmaMenuObjectsMgr();
//		tmaMenuObjectsMgr::g_pMgr->Initialize();
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void DeInitialize()
//	{
//		tmaMenuObjectsMgr::g_pMgr->DeInitialize();
//		delete tmaMenuObjectsMgr::g_pMgr;
//	}
//
//
//	//---------------------------------------------------------------------------
//	//	AttachEventToMenuObjects()
//	//---------------------------------------------------------------------------
//	void AttachEventToMenuObjects( int i_ObjectID, ControlCallback i_pFunction, ControlCallback i_pUpdate )
//	{
//		tmaMenuObjects^ pmmaMI = tmaMenuObjectsMgr::g_pMgr->get_menuitem( i_ObjectID );
//		DBG_ASSERT0( pmmaMI != nullptr, "MenuItem is 0" );
//
//		pmmaMI->SetCallback( i_pFunction );
//		if (i_pUpdate != nullptr)
//		{
//			pmmaMI->SetUpdateCallback( i_pUpdate );
//		}
//	}
//
//
//	//---------------------------------------------------------------------------
//	//	AddMenuItem()
//	//		add a menu item to the Menu Tree.
//	//		set the childname to "" is adding a parent item.
//	//		set the toolbar name to "" to not add a toolbar button
//	//	the function returns a unique ObjectID for the GUI object.
//	//	
//	//---------------------------------------------------------------------------
//	int AddMenuItem( const char * i_ParentName,
//					const char * i_ChildName,
//					const char * i_ToolbarName,
//					const char * i_ToolbarButton_ImageFilename )
//	{
//		DBG_ASSERT0( tmaSystem::g_pMainForm != nullptr, "tmaMenuMgr not initialized" );
//		DBG_ASSERT0( i_ParentName != 0, "Parent Name cannot be 0" );
//		DBG_ASSERT0( (*i_ParentName != 0), "Parent Name cannot be empty" );
//
//		tmaMenuObjects^ pMITop = nullptr;
//		tmaMenuObjects^ pMISub = nullptr;
//
//		// find if the parent name exists already
//		//
//		tmaMenuObjects^ pMO;
//
//		System::String^ parent_name = gcnew System::String(i_ParentName);
//		System::String^ child_name = gcnew System::String(i_ChildName);
//		System::Collections::IEnumerator^ myEnumerator = tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList()->GetEnumerator();
//		while ( myEnumerator->MoveNext() )
//		{
//			pMO = dynamic_cast<tmaMenuObjects^>(myEnumerator->Current);
//			if ( String::CompareOrdinal( pMO->GetMenuItem()->Text, parent_name ) == 0 )
//			{
//				pMITop = pMO;
//			}
//			if (	(String::CompareOrdinal( pMO->GetMenuItem()->Text, child_name ) == 0)
//				&&	(pMITop != nullptr))
//			{
//				pMISub = pMO;
//			}
//		}
//
//		tmaSystem::g_pMainForm->SuspendLayout();
//
//		// the top doesn't exist yet, so create it.
//		if ( pMITop == nullptr )
//		{
//			//	check if the menu item was created in the designer instead of programmatically
//			//	if so, create a tmaMenuObject and use the ToolStripItem that is already there.
//			//
//			ToolStripItem^ pTSI = nullptr;
//			if (tmaSystem::g_pMainForm->MainMenuStrip != nullptr)
//			{
//				int ndx = tmaSystem::g_pMainForm->MainMenuStrip->Items->Count;
//				System::String ^parent_name = gcnew System::String(i_ParentName);
//				for (int i = 0; i < ndx; ++i)
//				{
//					pTSI = tmaSystem::g_pMainForm->MainMenuStrip->Items[i];
//					if (pTSI->Text->Equals(parent_name))
//					{
//						break;
//					}
//					pTSI = nullptr;
//				}
//			}
//
//			// generate the menu item
//			pMITop = generate_menuitem(pTSI);
//			DBG_ASSERT0( pMITop != nullptr, "generation of menu item failed" );
//			pMITop->GetMenuItem()->Text = parent_name;
//
//			// attach popup callback in order to get callback for updates
//			pMITop->GetMenuItem()->DropDownOpening += gcnew System::EventHandler( this, &tmaMenuMgr::menuItem_Popup );
//
//			//	add the top level menu item to the list
//			if ((pTSI == nullptr) && (tmaSystem::g_pMainForm->MainMenuStrip != nullptr))
//				tmaSystem::g_pMainForm->MainMenuStrip->Items->Add( pMITop->GetMenuItem() );
//		}
//
//		//	sub menu item (if not an empty string passed in)
//		//
//		if ( *i_ChildName != 0 )
//		{
//			if ( pMISub == nullptr )
//			{
//				pMISub = generate_menuitem(nullptr);
//				DBG_ASSERT0( pMISub != nullptr, "generation of sub menu item failed" );
//				pMISub->GetMenuItem()->Text = gcnew System::String(i_ChildName);
//
//				//	hook up the sub menu items
//				/*System::Windows::Forms::ToolStripMenuItem^ __mcTemp__2[] = 
//					gcnew System::Windows::Forms::ToolStripMenuItem^[1];
//
//				__mcTemp__2[0] = pMISub->GetMenuItem();*/
//
//				pMISub->GetMenuItem()->MergeIndex = 0;
//				//pMITop->GetMenuItem()->DropDownItems->AddRange( __mcTemp__2 );
//				pMITop->GetMenuItem()->DropDownItems->Add( pMISub->GetMenuItem() );
//
//				// Attach popup callback in order to get callback for updates.
//				// This particular callback won't get called until other items
//				// are added to this as children, so this is a little earlier, 
//				// but atleast we'll be ready.
//				pMISub->GetMenuItem()->DropDownOpening += gcnew System::EventHandler( this, &tmaMenuMgr::menuItem_Popup );
//			}
//			else
//			{
//				tmaSystem::g_pMainForm->ResumeLayout();
//
//				//	the item already exists so don't drop below and try to
//				//	add another button
//				return pMISub->GetID();
//			}
//		}
//
//		//	set-up the event handling for the menu items
//		//
//		tmaMenuObjects ^pMIAdded;
//
//		if ( pMISub != nullptr )
//		{
//			pMIAdded = pMISub;
//		}
//		else
//		{
//			pMIAdded = pMITop;
//		}
//
//		//pMIAdded->GetMenuItem()->Click += new EventHandler( (pMIAdded), tmaMenuObjects::Callback );
//		//pMIAdded->GetMenuItem()->Select += new EventHandler( (pMIAdded), tmaMenuObjects::Callback );
//		//pSWFMI->PerformClick();
//		//pSWFMI->PerformSelect();
//
//		//
//		//	add buttons to the toolbar
//		//
//		if ( i_ToolbarName != 0 )
//		{
//			std::string icon_dir;
//			tmaManagedStringUtils::ManagedStringToStdString( m_IconDir, icon_dir );
//			icon_dir += i_ToolbarButton_ImageFilename;
//
//			tmaToolBarMgr::g_pMgr->add_toolstrip_button( pMIAdded, i_ToolbarName, icon_dir.c_str() );
//		}
//
//		AttachCallbacks( pMIAdded->GetID() );
//
//		tmaSystem::g_pMainForm->ResumeLayout();
//
//		return pMIAdded->GetID();
//	}
//
//	//---------------------------------------------------------------------------
//	//	RemoveMenuItem()
//	//		remove a menu item from the Menu Tree
//	//---------------------------------------------------------------------------
//	void RemoveMenuItem( const char * i_ParentName,
//						const char * i_ChildName  )
//	{
//		DBG_ASSERT0( tmaSystem::g_pMainForm != nullptr, "tmaMenuMgr not initialized" );
//
//		//	find the menu items
//		//
//		tmaMenuObjects^ pMI_top = nullptr;
//		tmaMenuObjects^ pMI = nullptr;
//		tmaMenuObjects^ pMO;
//
//		System::String ^parent_name = gcnew System::String( i_ParentName );
//		System::String ^child_name = gcnew System::String( i_ChildName );
//		System::Collections::IEnumerator^ myEnumerator = tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList()->GetEnumerator();
//		while ( myEnumerator->MoveNext() )
//		{
//			pMO = dynamic_cast<tmaMenuObjects^>(myEnumerator->Current);
//			if ( String::CompareOrdinal( pMO->GetMenuItem()->Text, parent_name ) == 0 )
//			{
//				pMI_top = pMO;
//			}
//			if (	(String::CompareOrdinal( pMO->GetMenuItem()->Text, child_name ) == 0)
//				&&	(pMI_top != nullptr))
//			{
//				pMI = pMO;
//			}
//		}
//
//		if ( !pMI_top )
//		{
//			return;
//		}
//
//		//	if the sub menu item is null that means we want to remove the root menu item
//		//
//		if ( pMI == nullptr )
//		{
//			//	remove the MenuItem + toolbarbutton
//			//
//			tmaSystem::g_pMainForm->MainMenuStrip->Items->Remove( pMI_top->GetMenuItem() );
//
////FINISH			tmaToolBarMgr::g_pMgr->remove_toolstrip_button( pMI_top );
//
//			tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList()->Remove( pMI_top );
//
//			delete pMI_top;
//		}
//		else
//		{
//			//	remove the sub MenuItem + toolbarbutton
//			//
//			pMI_top->GetMenuItem()->DropDownItems->Remove( pMI->GetMenuItem() );
//
////FINISH			tmaToolBarMgr::g_pMgr->remove_toolstrip_button( pMI );
//
//			tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList()->Remove( pMI );
//
//			delete pMI;
//		}
//	}
//
//	//---------------------------------------------------------------------------
//	//	EnableMenuItem()
//	//		enable or disable a menu item from the Menu Tree
//	//---------------------------------------------------------------------------
//	void EnableMenuItem( const char * i_ParentName,
//						const char * i_ChildName,
//						bool i_bEnable )
//	{
//		DBG_ASSERT0( tmaSystem::g_pMainForm != nullptr, "tmaMenuMgr not initialized" );
//
//		//	find the menu items
//		//
//		tmaMenuObjects^ pMI_top = nullptr;
//		tmaMenuObjects^ pMI = nullptr;
//		tmaMenuObjects^ pMO;
//
//		System::String ^parent_name = gcnew System::String( i_ParentName );
//		System::String ^child_name = gcnew System::String( i_ChildName );
//		System::Collections::IEnumerator^ myEnumerator = tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList()->GetEnumerator();
//		while ( myEnumerator->MoveNext() )
//		{
//			pMO = dynamic_cast<tmaMenuObjects^>(myEnumerator->Current);
//			if ( String::CompareOrdinal( pMO->GetMenuItem()->Text, parent_name ) == 0 )
//			{
//				pMI_top = pMO;
//			}
//			if (	(String::CompareOrdinal( pMO->GetMenuItem()->Text, child_name ) == 0)
//				&&	(pMI_top != nullptr))
//			{
//				pMI = pMO;
//			}
//		}
//
//		if ( !pMI_top )
//		{
//			return;
//		}
//
//		DBG_ASSERT0( pMI != nullptr, "Cannot enable/disable a menu item that is NULL" );
//
//		//	enable/disable the menu item + the toolbar
//		//
//		pMI->GetMenuItem()->Enabled = i_bEnable;
//
//		tmaToolBarMgr::g_pMgr->enable_toolstrip_button( pMI, i_bEnable );
//	}
//
//	//---------------------------------------------------------------------------
//	//	GetMenuItemID()
//	//		get the menu item ID
//	//---------------------------------------------------------------------------
//	int GetMenuItemID( const char * i_ParentName,
//					const char * i_ChildName )
//	{
//		DBG_ASSERT0( tmaSystem::g_pMainForm != nullptr, "tmaMenuMgr not initialized" );
//
//		//	find the menu items
//		//
//		tmaMenuObjects^ pMI_top = nullptr;
//		tmaMenuObjects^ pMI = nullptr;
//		tmaMenuObjects^ pMO;
//
//		System::String ^parent_name = gcnew System::String( i_ParentName );
//		System::String ^child_name = gcnew System::String( i_ChildName );
//		System::Collections::IEnumerator^ myEnumerator = tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList()->GetEnumerator();
//		while ( myEnumerator->MoveNext() )
//		{
//			pMO = dynamic_cast<tmaMenuObjects^>(myEnumerator->Current);
//			if ( String::CompareOrdinal( pMO->GetMenuItem()->Text, parent_name ) == 0 )
//			{
//				pMI_top = pMO;
//			}
//			if (	(String::CompareOrdinal( pMO->GetMenuItem()->Text, child_name ) == 0)
//				&&	(pMI_top != nullptr))
//			{
//				pMI = pMO;
//			}
//		}
//
//		if (pMI != nullptr)
//			return pMI->GetID();
//		else
//			return -1;
//		
//		//DBG_ASSERT0( pMI_top != nullptr, "Cannot get a menu item with that ID" );
//		//DBG_ASSERT0( pMI != nullptr, "Cannot get a menu item with that ID" );
//		//return pMI->GetID();
//	}
//
//	//---------------------------------------------------------------------------
//	//	MenuObjectsExist()
//	//		based on the menu item ID.
//	//
//	//		returns true if the ID refers to a valid tmaMenuObjects
//	//---------------------------------------------------------------------------
//	bool MenuObjectsExist( int i_ObjectID )
//	{
//		if ( GetMenuObjects( i_ObjectID ) != nullptr )
//		{
//			return true;
//		}
//
//		return false;
//	}
//
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	tmaMenuObjects^ GetMenuObjects( int i_ObjectID )
//	{
//		return tmaMenuObjectsMgr::g_pMgr->get_menuitem( i_ObjectID );
//	}
//
//
//	//---------------------------------------------------------------------------
//	//	MenuObjectsEnable()
//	//---------------------------------------------------------------------------
//	void MenuObjectsEnable( int i_ObjectID, bool i_bEnabled )
//	{
//		tmaMenuObjects^ pmmaMI = tmaMenuObjectsMgr::g_pMgr->get_menuitem( i_ObjectID );
//		DBG_ASSERT0( pmmaMI != nullptr, "Cannot attach an event to a 0 menu item" );
//
//		// menu item
//		ToolStripMenuItem^ mi = pmmaMI->GetMenuItem();
//		mi->Enabled =  i_bEnabled;
//
//		// toolbar button
//		ToolStripButton^ tbb = pmmaMI->GetToolStripButton();
//		if ( tbb )
//		{
//			tbb->Enabled =  i_bEnabled;
//		}
//	}
//
//	//---------------------------------------------------------------------------
//	//	MenuObjectsCheck()
//	//---------------------------------------------------------------------------
//	void MenuObjectsCheck( int i_ObjectID, bool i_bChecked )
//	{
//		tmaMenuObjects^ pmmaMI = tmaMenuObjectsMgr::g_pMgr->get_menuitem( i_ObjectID );
//		DBG_ASSERT0( pmmaMI != nullptr, "Cannot attach an event to a 0 menu item" );
//
//		// menu item
//		ToolStripMenuItem^ mi = pmmaMI->GetMenuItem();
//		mi->Checked =  i_bChecked;
//
//		// toolbar button
//		//	Not Applicable?
//		//ToolBarButton^ tbb = pmmaMI->GetToolbarButton();
//		//if ( tbb )
//		//{
//		//	tbb->set_Pushed( i_bChecked );
//		//}
//	}
//
//	//---------------------------------------------------------------------------
//	//	AttachCallbacks()
//	//---------------------------------------------------------------------------
//	void AttachCallbacks( int i_ObjectID )
//	{
//		tmaMenuObjects^ pmmaMI = tmaMenuObjectsMgr::g_pMgr->get_menuitem( i_ObjectID );
//		DBG_ASSERT0( pmmaMI != nullptr, "Cannot attach an event to a 0 menu item" );
//
//		// menu item
//		ToolStripMenuItem^ mi = pmmaMI->GetMenuItem();
//		mi->Click += gcnew System::EventHandler( this, &tmaMenuMgr::menuItem_Click );
//
//		// toolbar
//		tmaToolBarMgr::g_pMgr->AttachHandler( pmmaMI );
//	}
//
//	//---------------------------------------------------------------------------
//	//	GetMenuObjectsList()
//	//---------------------------------------------------------------------------
//	ArrayList^ GetMenuObjectsList()
//	{
//		return tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList();
//	}
//
//	//---------------------------------------------------------------------------
//	//	AddDesignerMenu()
//	//---------------------------------------------------------------------------
//	void AddDesignerMenu(System::Windows::Forms::ToolStripMenuItem ^pMenu)
//	{
//		tmaMenuObjectsMgr::g_pMgr->AddDesignerMenu(pMenu);
//
//		// attach popup callback in order to get callback for updates
//		pMenu->DropDownOpening += gcnew System::EventHandler( this, &tmaMenuMgr::menuItem_Popup );
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void SetIconDirectory( const char * i_IconDirectory )
//	{
//		if ( m_IconDir == nullptr )
//		{
//			m_IconDir = gcnew System::String( i_IconDirectory );
//		}
//		else
//		{
//			m_IconDir->Copy( gcnew System::String(i_IconDirectory) );
//		}
//	}
//
//	//---------------------------------------------------------------------------
//	//	Find a menu item.
//	//	NOTE: can return incorrect results if multiple menu items have the same
//	//	name.
//	//---------------------------------------------------------------------------
//	int FindMenuItem(String^ i_pMenuItemName)
//	{
//		// loop through the menus and try to find a matching item
//		DBG_ASSERT0( tmaSystem::g_pMainForm != nullptr, "tmaMenuMgr not initialized" );
//
//		//	find the menu items
//		//
//		tmaMenuObjects^ pMI = nullptr;
//		tmaMenuObjects^ pMO;
//
//		System::Collections::IEnumerator^ myEnumerator = tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList()->GetEnumerator();
//		while ( myEnumerator->MoveNext() )
//		{
//			pMO = dynamic_cast<tmaMenuObjects^>(myEnumerator->Current);
//			if (String::CompareOrdinal( pMO->GetMenuItem()->Text, i_pMenuItemName ) == 0)
//			{
//				pMI = pMO;
//			}
//		}
//
//		if (pMI == nullptr)
//			return -1;
//		else
//			return pMI->GetID();
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void SetMenuItemShortcut(int i_ObjectID, String^ i_pShortcutString)
//	{
//		tmaMenuObjects^ pmmaMI = tmaMenuObjectsMgr::g_pMgr->get_menuitem( i_ObjectID );
//		DBG_ASSERT0( pmmaMI != nullptr, "Cannot attach an event to a 0 menu item" );
//
//		// menu item
//		ToolStripMenuItem^ pMI = pmmaMI->GetMenuItem();
//
//		if (pMI != nullptr && i_pShortcutString != nullptr)
//		{
//			//	set-up a converter for shortcuts
//			//
//			TypeConverter^ conv = TypeDescriptor::GetConverter(Keys::typeid);
//			Object^ obj = conv->ConvertFromString(i_pShortcutString);
//			if (obj != nullptr)
//			{
//				Keys^ kys = dynamic_cast<Keys^>(conv->ConvertFromString( i_pShortcutString ));
//				if (kys != nullptr)
//				{
//					try
//					{
//						//pMI->set_ShortcutKeys(^(kys));
//						pMI->ShortcutKeys = *(kys);
//
//						int kysvalue = (int)(*kys);
//						std::string skstr;
//						tmaManagedStringUtils::ManagedStringToStdString( conv->ConvertToString(pMI->ShortcutKeys), skstr);
//						std::string menustr;
//						tmaManagedStringUtils::ManagedStringToStdString( pMI->Text, menustr );
//						//DBG_LOG4("    shortcut menu-id=%d-%s scut=%d-%s", i_ObjectID, menustr.c_str(), kysvalue, skstr.c_str());
//					}
//					catch(...)
//					{
//					}
//				}
//			}
//		}
//		else
//		{
//			pMI->ShortcutKeys = System::Windows::Forms::Keys::None;
//		}
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	bool IsValidShortcut( String^ i_ShortcutString )
//	{
//		if (i_ShortcutString == nullptr)
//			return false;
//		if (i_ShortcutString->Length == 0)
//			return false;
//
//		TypeConverter^ conv = TypeDescriptor::GetConverter(Keys::typeid);
//		return conv->IsValid(i_ShortcutString);
//	}
//
//private:
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void GetDropDownItemShortcut(MenuItem^ i_pMenuItem, String^ o_pShortcutString)
//	{
//		if (i_pMenuItem != nullptr && o_pShortcutString != nullptr)
//		{
//			//TypeConverter^ keyconv = TypeDescriptor::GetConverter(Keys::typeid);//new TypeConverter;
//			//Shortcut scut = i_pMenuItem->Shortcut;
//			//Keys k;
//			//k = static_cast<Keys>(i_pMenuItem->Shortcut);
//			//String^ st = gcnew String("");
//			//st->Concat(st, scut);
//			//o_pShortcutString = keyconv->ConvertToString(k);
//
////Shortcut sc = Shortcut.CtrlShiftF1;
////string s = TypeDescriptor.GetConverter(typeof(Keys)).ConvertToString((Keys) sc);
//		}
//		else
//		{
//			if (o_pShortcutString != nullptr)
//			{
//				o_pShortcutString = "";
//			}
//		}
//	}
//
//private:
//	ArrayList^ l_pMenuObjects;	// tmaMenuObjects
//
//	System::String^	m_IconDir;
//};
//#endif // _MANAGED
