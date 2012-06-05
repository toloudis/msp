#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyControlMgr.hpp
//**
//**		Control and Control Factory Manager
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_CONTROLMGR_HPP
//#error prtyControlMgr.hpp multiply included
//#endif
//#define PRTY_CONTROLMGR_HPP
//
//#ifndef PRTY_CONTROL_HPP
//#include "ToolUIManaged/prtym/prtyControl.hpp"
//#endif
//#ifndef PRTY_CONTROLBUFFER_HPP
//#include "ToolUIManaged/prtym/prtyControlBuffer.hpp"
//#endif
//#ifndef PRTY_CONTROLFACTORY_HPP
//#include "ToolUIManaged/prtym/prtyControlFactory.hpp"
//#endif
//#ifndef PRTY_PROPERTY_HPP
//#include "Core/prty/prtyProperty.hpp"
//#endif
//#ifndef PRTY_PROPERTYUIINFO_HPP
//#include "Core/prty/prtyPropertyUIInfo.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//#ifndef DBG_LOG_HPP
//#include "Core/dbg/dbgLog.hpp"
//#endif
//#ifndef DBG_MSG_HPP
//#include "Core/dbg/dbgMsg.hpp"
//#endif
//#ifndef TMA_MANAGEDSTRINGUTILS_HPP
//#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
//#endif
//
//#include <vector>
//
//
////============================================================================
////============================================================================
//class nameString;
//
//
////============================================================================
////============================================================================
//#ifdef _MANAGED
//
//using namespace System::Collections;
//
////============================================================================
////============================================================================
//public ref class prtyControlMgr
//{
//	static ArrayList^ m_pFactories = nullptr;
//	static ArrayList^ m_pControls = nullptr;
//
//public:
//	//---------------------------------------------------------------------------
//	//	Initialize()
//	//---------------------------------------------------------------------------
//	static void Initialize()
//	{
//		m_pFactories = gcnew ArrayList;
//		m_pFactories->Clear();
//		m_pControls = gcnew ArrayList;
//		m_pControls->Clear();
//	};
//
//	//---------------------------------------------------------------------------
//	//	DeInitialize()
//	//---------------------------------------------------------------------------
//	static void DeInitialize()
//	{
//		m_pFactories->Clear();
//		m_pControls->Clear();
//
//		// function in prtyControlBuffer.hpp:
//		ClearAllControlBuffers();
//	};
//
//	//----------------------------------------------------------------------------
//	//----------------------------------------------------------------------------
//	static void AddControlFactory(prtyControlFactory^ i_pControlFactory)
//	{
//		DBG_ASSERT0(i_pControlFactory != nullptr, "Cannot add a NULL factory");
//
//		m_pFactories->Add( i_pControlFactory );
//	};
//
//	//----------------------------------------------------------------------------
//	//----------------------------------------------------------------------------
//	static void RemoveControlFactory(prtyControlFactory^ i_pControlFactory)
//	{
//		DBG_ASSERT0(i_pControlFactory != nullptr, "Cannot remove a NULL factory");
//
//		m_pFactories->Remove( i_pControlFactory );
//	};
//
//	//----------------------------------------------------------------------------
//	//----------------------------------------------------------------------------
//	static int GetNumControls()
//	{
//		return m_pControls->Count;
//	}
//
//	//----------------------------------------------------------------------------
//	//----------------------------------------------------------------------------
//	static prtyControl^ GetControl(const std::string& i_ControlName)
//	{
//		prtyControl^ pControl;
//
//		System::Collections::IEnumerator^ myEnumerator = m_pControls->GetEnumerator();
//		while ( myEnumerator->MoveNext() )
//		{
//			pControl = dynamic_cast<prtyControl^>(myEnumerator->Current);
//			if (pControl != nullptr)
//			{
//				if (strcmp(pControl->GetName()->c_str(), i_ControlName.c_str()) == 0)
//				{
//					return pControl;
//				}
//			}
//		}
//
//		return nullptr;
//	};
//
//	//----------------------------------------------------------------------------
//	//----------------------------------------------------------------------------
//	static prtyControl^ CreateControl(prtyPropertyUIInfo* i_pUIInfo)
//	{
//		DBG_ASSERT0(i_pUIInfo != 0, "Cannot create a control with a NULL property UI Info");
//
//		prtyControlFactory^ pCF;
//		prtyControl^ pControl;
//
//		System::Collections::IEnumerator^ myEnumerator = m_pFactories->GetEnumerator();
//		while ( myEnumerator->MoveNext() )
//		{
//			pCF = dynamic_cast<prtyControlFactory^>(myEnumerator->Current);
//			pControl = pCF->CreateControl(i_pUIInfo);
//
//			if (pControl != nullptr)
//			{
//				//std::string ctxt2;
//				//tmaManagedStringUtils::ManagedStringToStdString(pControl->GetControl()->Text, ctxt2);
//				//DBG_LOG3("ADD [%23s] %20s %8x", pControl->GetName()->c_str(), ctxt2.c_str(), pControl->GetControl() );
//		
//				//DBG_LOG1("prtyControlMgr, Creating control named: %s", i_pUIInfo->GetControlName().c_str());
//
//				m_pControls->Add(pControl);
//				return pControl;
//			}
//		}
//		return nullptr;
//	};
//
//	//----------------------------------------------------------------------------
//	// Returns true if the control was found and deleted
//	//----------------------------------------------------------------------------
//	static bool DeleteControl(System::Windows::Forms::Control^ i_pControl)
//	{
//		//std::string ctxt1;
//		//tmaManagedStringUtils::ManagedStringToStdString(i_pControl->Text, ctxt1);
//		//DBG_LOG2("DELETING Control [%20s] %x", ctxt1.c_str(), i_pControl);
//
//		prtyControl^ pControl;
//		System::Collections::IEnumerator^ myEnumerator = m_pControls->GetEnumerator();
//		while ( myEnumerator->MoveNext() )
//		{
//			pControl = safe_cast<prtyControl^>(myEnumerator->Current);
//
//			//std::string ctxt2;
//			//tmaManagedStringUtils::ManagedStringToStdString(pControl->GetControl()->Text, ctxt2);
//			//DBG_LOG6("%s [%23s] %20s %8x vs %8x %20s", (pControl->GetControl() == i_pControl)?"---":"DEL", pControl->GetName()->c_str(), ctxt2.c_str(), pControl->GetControl(), i_pControl, ctxt1.c_str() );
//			if (pControl->GetControl() == i_pControl)
//			{
//				//DBG_LOG("prtyControlMgr, Removing control.");
//
//				m_pControls->Remove(pControl);
//				delete pControl;
//				return true;
//			}
//		}
//		return false;
//	}
//
//	//----------------------------------------------------------------------------
//	// Double-check our controls and make sure none are attached to controls
//	// that have been disposed.
//	//----------------------------------------------------------------------------
//	static void RemoveDisposed()
//	{
//		prtyControl^ pControl;
//		const int num_controls = m_pControls->Count;
//		for (int i = num_controls-1; i >= 0; i--)
//		{
//			pControl = safe_cast<prtyControl^>(m_pControls[i]);
//			if (pControl->GetControl()->IsDisposed)
//			{
//				m_pControls->RemoveAt(i);
//			}
//		}
//
//	}
//};
//
//#endif // _MANAGED
