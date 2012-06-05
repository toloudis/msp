#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	tmaDialogMemoryInterest.hpp
//**
//**		A DialogMemory Interest is related to dialogs that remember their
//**	location + size.
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_DIALOGMEMORYINTEREST_HPP
//#error tmaDialogMemoryInterest.hpp multiply included
//#endif
//#define TMA_DIALOGMEMORYINTEREST_HPP
//
//#ifndef TMA_DIALOGMEMORYMGR_HPP
//#include "ToolUIManaged/tma/tmaDialogMemoryMgr.hpp"
//#endif
//
//
////============================================================================
////============================================================================
//#ifdef _MANAGED
//
//public ref class tmaDialogMemoryInterest abstract
//{
//	public:
//		//---------------------------------------------------------------------------
//		//---------------------------------------------------------------------------
//		tmaDialogMemoryInterest()
//		{
//			tmaDialogMemoryMgr::RegisterInterest( this );	// TODO [rjk] remove this circular dependency
//		}
//
//		//---------------------------------------------------------------------------
//		//---------------------------------------------------------------------------
//		virtual ~tmaDialogMemoryInterest()
//		{
//			tmaDialogMemoryMgr::UnRegisterInterest( this );	// TODO [rjk] remove this circular dependency
//		}
//
//		//---------------------------------------------------------------------------
//		// write size and location
//		//---------------------------------------------------------------------------
//		virtual void Write() = 0;
//
//		//---------------------------------------------------------------------------
//		// read size and location
//		//---------------------------------------------------------------------------
//		virtual void Read() = 0;
//
//		virtual bool getWriteVisibleFlag() = 0;
//		virtual void setWriteVisibleFlag(bool new_writeFlag) = 0;
//};
//#endif // _MANAGED
