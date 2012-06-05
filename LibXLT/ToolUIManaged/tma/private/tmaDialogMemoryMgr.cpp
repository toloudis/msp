///****************************************************************************\
//**	tmaDialogMemoryMgr.cpp
//**
//**		see .hpp
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#include "ToolUIManaged/tma/tmaDialogMemoryMgr.hpp"
//
//#include "ToolUIManaged/tma/tmaDialogMemoryInterest.hpp"
//
////	library
//#include "Core/dbg/dbgAssert.hpp"
//#include "Core/dbg/dbgLog.hpp"
//#include "Core/env/envSTLHelpers.hpp"
//
//#ifdef _MANAGED
//
////#using <mscorlib.dll>
//using namespace System;
//using namespace System::Collections;
//
//
////============================================================================
////============================================================================
//public ref class dialog_memory_mgr
//{
//public: static ArrayList^ l_InterestList;
//};
//
////--------------------------------------------------------------------
////--------------------------------------------------------------------
//void tmaDialogMemoryMgr::Initialize()
//{
//	dialog_memory_mgr::l_InterestList = gcnew ArrayList();
//}
//void tmaDialogMemoryMgr::DeInitialize()
//{
//	dialog_memory_mgr::l_InterestList = nullptr;
//}
//
//
////--------------------------------------------------------------------
////	RegisterInterest() - add a interest to the system
////--------------------------------------------------------------------
//void tmaDialogMemoryMgr::RegisterInterest( tmaDialogMemoryInterest^ i_pInterest )
//{
//	DBG_ASSERT0( i_pInterest != nullptr, "Cannot register a NULL Interest" );
//
//	dialog_memory_mgr::l_InterestList->Add( i_pInterest );
//}
//
////--------------------------------------------------------------------
////	UnRegisterInterest() - remove a interest from the system.
////
////	Note: this will NOT delete the interest.  It is up to the
////	registerer.
////--------------------------------------------------------------------
//void tmaDialogMemoryMgr::UnRegisterInterest( tmaDialogMemoryInterest^ i_pInterest )
//{
//	dialog_memory_mgr::l_InterestList->Remove( i_pInterest );
//}
//
////----------------------------------------------------------------------------
////	Clear() - clear the list
////----------------------------------------------------------------------------
//void tmaDialogMemoryMgr::Clear()
//{
//	dialog_memory_mgr::l_InterestList->Clear();
//}
//
////--------------------------------------------------------------------
//// Read - Read the coordinates of each dialog
////--------------------------------------------------------------------
//void tmaDialogMemoryMgr::Read()
//{
//	for (int i = 0; i < dialog_memory_mgr::l_InterestList->Count; ++i)
//	{
//		(dynamic_cast<tmaDialogMemoryInterest^>(dialog_memory_mgr::l_InterestList[i]))->Read();
//	}
//}
//
////--------------------------------------------------------------------
//// Write - write the coordinates of each dialog
////--------------------------------------------------------------------
//void tmaDialogMemoryMgr::Write()
//{
//	for (int i = 0; i < dialog_memory_mgr::l_InterestList->Count; ++i)
//	{
//		(dynamic_cast<tmaDialogMemoryInterest^>(dialog_memory_mgr::l_InterestList[i]))->Write();
//	}
//}
//
//void tmaDialogMemoryMgr::ChangeWriteFlag()
//{
//	for (int i = 0; i < dialog_memory_mgr::l_InterestList->Count; ++i)
//	{
//		(dynamic_cast<tmaDialogMemoryInterest^>(dialog_memory_mgr::l_InterestList[i]))->setWriteVisibleFlag(false);
//	}
//
//}
//
//#endif // _MANAGED
