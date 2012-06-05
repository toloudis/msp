#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaMenuObjectsMgr.hpp
//**
//**      The manager for the tmaMenuObjects class
//**
//**	StudioGPU
//**	Copyright(C) 2003 - All Rights Reserved
//\****************************************************************************/
//
//#ifdef TMA_MENUOBJECTSMGR_HPP
//#error tmaMenuObjectsMgr.hpp multiply included
//#endif
//#define TMA_MENUOBJECTSMGR_HPP
//
//#ifndef TMA_MENUOBJECTS_HPP
//#include "ToolUIManaged/tma/tmaMenuObjects.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//
////============================================================================
////============================================================================
//#ifdef _MANAGED
//
//using namespace System::Collections;
//
//
////============================================================================
////============================================================================
//public ref class tmaMenuObjectsMgr
//{
//public:
//	static tmaMenuObjectsMgr^ g_pMgr = nullptr;
//
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//tmaMenuObjectsMgr()
//{
//}
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//~tmaMenuObjectsMgr()
//{
//}
//
//
////
////	local (private) functions
////
//
////---------------------------------------------------------------------------
////	generate a menu item
////---------------------------------------------------------------------------
//tmaMenuObjects^ generate_menuitem()
//{
//	tmaMenuObjects^ pMI = gcnew tmaMenuObjects;
//	l_pMenuObjects->Add( pMI );
//
//	pMI->SetID(l_pMenuObjects->Count - 1);
//
//	return pMI;
//}
//
////---------------------------------------------------------------------------
////	get a menu item
////---------------------------------------------------------------------------
//tmaMenuObjects^ get_menuitem( int i_ObjectID )
//{
//	tmaMenuObjects^ pMO;
//
//    System::Collections::IEnumerator^ myEnumerator = l_pMenuObjects->GetEnumerator();
//    while ( myEnumerator->MoveNext() )
//	{
//		pMO = dynamic_cast<tmaMenuObjects^>(myEnumerator->Current);
//		if ( pMO->GetID() == i_ObjectID )
//		{
//			return pMO;
//		}
//	}
//	return nullptr;
//}
//
////---------------------------------------------------------------------------
////	get a menu item from a menu item
////---------------------------------------------------------------------------
//tmaMenuObjects^ get_menuitem( ToolStripMenuItem^ i_pMenuItem )
//{
//	tmaMenuObjects^ pMO;
//
//    System::Collections::IEnumerator^ myEnumerator = l_pMenuObjects->GetEnumerator();
//    while ( myEnumerator->MoveNext() )
//	{
//		pMO = dynamic_cast<tmaMenuObjects^>(myEnumerator->Current);
//		if ( pMO->GetMenuItem() == i_pMenuItem )
//		{
//			return pMO;
//		}
//	}
//	return nullptr;
//}
//
//
////---------------------------------------------------------------------------
////	get a menu item from a ToolStrip button
////---------------------------------------------------------------------------
//tmaMenuObjects^ get_menuitem( ToolStripButton^ i_pToolStripButton )
//{
//	tmaMenuObjects^ pMO;
//
//    System::Collections::IEnumerator^ myEnumerator = l_pMenuObjects->GetEnumerator();
//    while ( myEnumerator->MoveNext() )
//	{
//		pMO = dynamic_cast<tmaMenuObjects^>(myEnumerator->Current);
//		if ( pMO->GetToolStripButton() == i_pToolStripButton )
//		{
//			return pMO;
//		}
//	}
//	return nullptr;
//}
//
//
////===========================================================================
////	tmaMenuObjectsMgr functions
////===========================================================================
//
////---------------------------------------------------------------------------
////	Initialize()
////---------------------------------------------------------------------------
//void Initialize()
//{
//	l_pMenuObjects = gcnew ArrayList;
//	l_pMenuObjects->Clear();
//}
//
////---------------------------------------------------------------------------
////	DeInitialize()
////---------------------------------------------------------------------------
//void DeInitialize()
//{
//	l_pMenuObjects->Clear();
//	//delete l_pMenuObjects;
//}
//
////---------------------------------------------------------------------------
////	AddDesignerMenu()
////---------------------------------------------------------------------------
//void AddDesignerMenu(System::Windows::Forms::ToolStripMenuItem ^pMenu)
//{
//	tmaMenuObjects^ pMI = generate_menuitem();
//	pMI->SetMenuItem( pMenu );
//}
//
////---------------------------------------------------------------------------
////	GetMenuObjectsList()
////---------------------------------------------------------------------------
//ArrayList^ GetMenuObjectsList()
//{
//	return l_pMenuObjects;
//}
//
//private:
//	ArrayList^ l_pMenuObjects;	// tmaMenuObjects
//};
//#endif // _MANAGED
