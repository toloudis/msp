#error THIS_FILE_IS_OBSOLETE

//*****************************************************************************
//**  tmaDialogTabbedMemory.hpp
//**
//**      extends the memory dialog base class adding remembering of the
//**	current tab.
//**
//**	StudioGPU
//**	Copyright(C) 2004 - All Rights Reserved
//\****************************************************************************/
//
//#ifdef TMA_DIALOGTABBEDMEMORY_HPP
//#error tmaDialogTabbedMemory.hpp multiply included
//#endif
//#define TMA_DIALOGTABBEDMEMORY_HPP
//
//#ifndef TMA_DIALOGMEMORY_HPP
//#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
//#endif
//#ifndef TMA_REGISTRYUTIL_HPP
//#include "ToolUIManaged/tma/tmaRegistryUtil.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//#ifndef DBG_LOG_HPP
//#include "Core/dbg/dbgLog.hpp"
//#endif
//
//#ifdef _MANAGED
//
////using namespace System;
//using namespace System::Windows::Forms;
//
//
////============================================================================
////============================================================================
//public ref class tmaDialogTabbedMemory : public tmaDialogMemory
//{
//public:
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	tmaDialogTabbedMemory(System::Windows::Forms::Form ^i_pForm)
//		: tmaDialogMemory( i_pForm )
//	{
//		//	find the tab control
//		Control::ControlCollection^ pColl = m_pDialog->Controls;
//		int count = pColl->Count;
//
//		for ( int i = 0 ; i < count ; i++ )
//		{
//			Control^ pControl = pColl[i];
//
//			this->m_pTabControl = dynamic_cast<TabControl^>(pControl);
//			if ( m_pTabControl != nullptr )
//			{
//				// FIX: (?) - this memory class only supports ONE tab control in the dialog
//				break;
//			}
//		}
//
//		DBG_ASSERT0( m_pTabControl != nullptr, "Must have a tab control to use tmaDialogTabbedMemory" );
//
//		ReadFromRegistryEx();
//
//		//	add event for tab control changing
//		//
//		m_pTabControl->SelectedIndexChanged += gcnew System::EventHandler(this, &tmaDialogTabbedMemory::TabChanged);
//	}
//
//public:
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	virtual void Read() override
//	{
//		tmaDialogMemory::Read();
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	virtual void ReadFromRegistryEx()
//	{
//		//	add new value(s)
//		std::string name;
//		BuildRegFolderName( name );
//
//		std::string strindex = tmaRegistryUtil::GetValue(tmaRegistryUtil::e_CurrentUser,
//			name.c_str(), "SelTab");
//
//		if (strindex.length() > 0)
//		{
//			int index = 0;
//			::sscanf(strindex.c_str(), "%d", &index );
//			if (index >= 0)
//			{
//				m_pTabControl->SelectedIndex = index;
//			}
//		}
//	}
//
//	//---------------------------------------------------------------------------
//	// write size and location 
//	//---------------------------------------------------------------------------
//	virtual void Write() override
//	{
//		if (m_pDialog->Visible)
//		{
//			tmaDialogMemory::Write();
//
//			std::string name;
//			BuildRegFolderName( name );
//
//			char buffer[128];
//			::sprintf(buffer, "%d", m_pTabControl->SelectedIndex );
//			tmaRegistryUtil::SetValue(tmaRegistryUtil::e_CurrentUser, name.c_str(), "SelTab", buffer);
//		}
//	}
//
//protected:
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void TabChanged(System::Object ^  sender, System::EventArgs ^  e)
//	{
//		Write();
//	}
//
//private:
//	System::Windows::Forms::TabControl^	m_pTabControl;
//};
//
//#endif // _MANAGED
