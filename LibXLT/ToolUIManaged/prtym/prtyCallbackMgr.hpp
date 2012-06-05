#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyCallbackMgr.hpp
//**
//**		Property callback manager.  It handles to routing of callbacks from
//**	an unmanaged property with a managed control.
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_CALLBACKMGR_HPP
//#error prtyCallbackMgr.hpp multiply included
//#endif
//#define PRTY_CALLBACKMGR_HPP
//
//#ifndef PRTY_INTCALLBACKMGR_HPP
//#include "Core/prty/prtyIntCallbackMgr.hpp"
//#endif
//
//#ifndef PRTY_CONTROL_HPP
//#include "ToolUIManaged/prtym/prtyControl.hpp"
//#endif
//
//#ifndef PRTY_PROPERTY_HPP
//#include "Core/prty/prtyProperty.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//
//
////============================================================================
////============================================================================
//class nameString;
//
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//using namespace System::Collections;
//
////============================================================================
////============================================================================
//class prtymIntCallbackMgr : public prtyIntCallbackMgrImpl
//{
//public:
//	//----------------------------------------------------------------------------
//	//----------------------------------------------------------------------------
//	virtual void PropertyChanged(prtyProperty* i_pProperty, int i_Index);
//
//	//----------------------------------------------------------------------------
//	//----------------------------------------------------------------------------
//	virtual void UpdateControl( prtyPropertyUIInfo* i_pUII );
//};
//
////============================================================================
////============================================================================
//public ref class prtyCallbackMgr
//{
//	static ArrayList^ m_pControls = nullptr;
//
//public:
//	//---------------------------------------------------------------------------
//	//	Initialize()
//	//---------------------------------------------------------------------------
//	static void Initialize()
//	{
//		m_pControls = gcnew ArrayList;
//		m_pControls->Clear();
//
//		prtyIntCallbackMgr::SetImplementation(new prtymIntCallbackMgr);
//	};
//
//	//---------------------------------------------------------------------------
//	//	DeInitialize()
//	//---------------------------------------------------------------------------
//	static void DeInitialize()
//	{
//		m_pControls->Clear();
//	};
//
//	//----------------------------------------------------------------------------
//	//----------------------------------------------------------------------------
//	static void AddCallback(prtyProperty* i_pProperty, prtyControl^ i_pControl)
//	{
//		DBG_ASSERT0(i_pProperty != 0, "Cannot add a NULL callback");
//		if (i_pProperty == nullptr)
//			return;
//
//		for (int i = 0; i < m_pControls->Count; ++i)
//		{
//			if ( m_pControls[i] == nullptr )
//			{
//				m_pControls[i] = i_pControl;
//				i_pProperty->AddIntCallback( i );
//				return;
//			}
//		}
//
//		int index = m_pControls->Add( i_pControl );
//		i_pProperty->AddIntCallback( index );
//	};
//
//	//----------------------------------------------------------------------------
//	//----------------------------------------------------------------------------
//	static void RemoveCallback(prtyProperty* i_pProperty, prtyControl^ i_pControl)
//	{
//		DBG_ASSERT0(i_pProperty != 0, "Cannot add a NULL callback");
//		if (i_pProperty == nullptr)
//			return;
//
//		int index;
//		for (index = 0; index < m_pControls->Count; ++index)
//		{
//			if ( m_pControls[index] == i_pControl )
//			{
//				i_pProperty->RemoveIntCallback(index);
//
//				m_pControls[index] = nullptr;
//				return;
//			}
//		}
//	};
//
//	//----------------------------------------------------------------------------
//	//----------------------------------------------------------------------------
//	static void PropertyChanged(prtyProperty* i_pProperty, int i_Index)
//	{
//		(dynamic_cast<prtyControl^>(m_pControls[i_Index]))->PropertyChanged(i_pProperty);
//	};
//
//	//----------------------------------------------------------------------------
//	// When a property UI Info has changed, call this function to update
//	// the control that is attached to it.
//	//----------------------------------------------------------------------------
//	static void UpdateControl( prtyPropertyUIInfo* i_pUII ) 
//	{
//		// loop through the control array and find the control that
//		// has this UIInfo. 
//		for (int i = 0; i < m_pControls->Count; ++i)
//		{
//			prtyControl^ pControl = static_cast<prtyControl^>(m_pControls[i]);
//			// found the matching control, so update it with the new UII data.
//			//
//			if (pControl != nullptr &&
//				pControl->HasUIInfo( i_pUII ))
//			{
//				pControl->UpdateControl( i_pUII );
//
//				// have to keep looking for others because the
//				// controls aren't cleaning up correctly...
//				//return;
//			}
//		}
//	}
//
//};
//
//
//#endif // _MANAGED
