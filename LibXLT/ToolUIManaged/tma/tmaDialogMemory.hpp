// #error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaDialogMemory.hpp
//**
//**      Class that memorizes the size, placement and visibility
//**	of a dialog in the registry.
//**
//**	StudioGPU
//**	Copyright(C) 2004 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_DIALOGMEMORY_HPP
//#error tmaDialogMemory.hpp multiply included
//#endif
//#define TMA_DIALOGMEMORY_HPP
//
//#ifndef TMA_DIALOGMEMORYINTEREST_HPP
//#include "ToolUIManaged/tma/tmaDialogMemoryInterest.hpp"
//#endif
//#ifndef TMA_MANAGEDSTRINGUTILS_HPP
//#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
//#endif
//#ifndef TMA_REGISTRYUTIL_HPP
//#include "ToolUIManaged/tma/tmaRegistryUtil.hpp"
//#endif
//
//#include <string>
//
//
////============================================================================
////============================================================================
//#ifdef _MANAGED
//
////using namespace System;
//using namespace System::Windows::Forms;
//
//
////============================================================================
////============================================================================
//public ref class tmaDialogMemory : public tmaDialogMemoryInterest
//{
//public:
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	tmaDialogMemory(System::Windows::Forms::Form ^i_pForm)
//	:	m_pDialog(i_pForm), 
//		m_bVisible(false),
//		m_bWriteVisibilityFlag(true)
//	{
//		m_CurrentLayoutConfig = gcnew System::String(lc_Key_LayoutConfig_Default);
//
//		// set size of dialog based on values in registry
//		Read();
//
//		// Set values to allow us to control the starting position
//		m_pDialog->StartPosition = FormStartPosition::Manual;
//
//		// Show the dialog now if it was visible last time
//		if (this->m_bVisible)
//			m_pDialog->Visible = true;
//
//		// register callbacks to notice changes in the dialog
//		//m_pDialog->SizeChanged += gcnew System::EventHandler(this, &tmaDialogMemory::SizeChanged);
//		m_pDialog->Resize += gcnew System::EventHandler(this, &tmaDialogMemory::SizeChanged);
//		//m_pDialog->LocationChanged += gcnew System::EventHandler(this, &tmaDialogMemory::LocationChanged);
//		m_pDialog->Move += gcnew System::EventHandler(this, &tmaDialogMemory::LocationChanged);
//		m_pDialog->VisibleChanged += gcnew System::EventHandler(this, &tmaDialogMemory::VisibleChanged);
//		m_pDialog->Closing += gcnew System::ComponentModel::CancelEventHandler(this, &tmaDialogMemory::Closing);
//	}
//
//protected:
//	//---------------------------------------------------------------------------
//	//	build the name for the registry folder for this dialog
//	//---------------------------------------------------------------------------
//	virtual void BuildRegFolderName( std::string& o_Name )
//	{
//		tmaManagedStringUtils::ManagedStringToStdString(m_pDialog->Name, o_Name);
//		//o_Name = "Dialog " + o_Name;
//		std::string folders;
//		tmaManagedStringUtils::ManagedStringToStdString(m_CurrentLayoutConfig, folders);
//		o_Name = std::string(lc_Key_LayoutConfiguration) + "\\" + folders + "\\" + "Dialog " + o_Name;
//	}
//public:
//	//---------------------------------------------------------------------------
//	// TODO - Read from XML file instead of registry.
//	//---------------------------------------------------------------------------
//	 virtual void Read() override
//	{
//		std::string name;
//		BuildRegFolderName( name );
//
//		std::string size = tmaRegistryUtil::GetValue(tmaRegistryUtil::e_CurrentUser,
//			name.c_str(), std::string(lc_Value_Size));
//		std::string loc = tmaRegistryUtil::GetValue(tmaRegistryUtil::e_CurrentUser,
//			name.c_str(), std::string(lc_Value_Location));
//		std::string visible = tmaRegistryUtil::GetValue(tmaRegistryUtil::e_CurrentUser,
//			name.c_str(), std::string(lc_Value_Visible));
//
//		if (size.length() > 0)
//		{
//			//DBG_LOG1("Get Size: %s", size.c_str());
//			int width = 0, height = 0;
//			::sscanf(size.c_str(), "%d %d", &width, &height);
//			if (width > 0 && height > 0)
//			{
//				//	check if the remembered size is smaller than minimum, if
//				//	so, use minimum
//				//
//				System::Drawing::Size minsize = m_pDialog->MinimumSize;
//				if (   (minsize.Height > height)
//					|| (minsize.Width > width))
//				{
//					m_pDialog->Size = minsize;
//				}
//				else
//				{
//					m_pDialog->Size = System::Drawing::Size(width, height);
//				}
//			}
//		}
//		if (loc.length() > 0)
//		{
//			//DBG_LOG1("Get Location: %s", loc.c_str());
//			int x = 0, y = 0;
//			::sscanf(loc.c_str(), "%d %d", &x, &y);
//			if (x < 0) x = 0;
//			if (y < 0) y = 0;
//			m_pDialog->Location = System::Drawing::Point(x, y);
//		}
//		if (visible.length() > 0 )
//		{
//			m_bVisible = (visible == "1");
//			//m_pDialog->Visible = m_bVisible;
//		}
//		else m_bVisible = false;
//	}
//
//	//---------------------------------------------------------------------------
//	// write size and location
//	//
//	// TODO - Write to XML file instead of registry.
//	//---------------------------------------------------------------------------
//	 virtual void Write() override 
//	{
//		if (m_pDialog->Visible && (m_pDialog->WindowState == System::Windows::Forms::FormWindowState::Normal))
//		{
//			std::string name;
//			BuildRegFolderName( name );
//
//			char buffer[128];
//			::sprintf(buffer, "%d %d", m_pDialog->Size.Width, m_pDialog->Size.Height);
//			//DBG_LOG1("Set Size: %s", buffer);
//			tmaRegistryUtil::SetValue(tmaRegistryUtil::e_CurrentUser, name.c_str(), std::string(lc_Value_Size), buffer);
//
//			::sprintf(buffer, "%d %d", m_pDialog->Location.X, m_pDialog->Location.Y);
//			//DBG_LOG1("Set Location: %s", buffer);
//			tmaRegistryUtil::SetValue(tmaRegistryUtil::e_CurrentUser, name.c_str(), std::string(lc_Value_Location), buffer);
//			if( m_bWriteVisibilityFlag )
//				tmaRegistryUtil::SetValue(tmaRegistryUtil::e_CurrentUser, name.c_str(), std::string(lc_Value_Visible), "1");
//			else
//				tmaRegistryUtil::SetValue(tmaRegistryUtil::e_CurrentUser, name.c_str(), std::string(lc_Value_Visible), "-1");
//		}
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	bool GetVisibleFlag()
//	{
//		return m_bVisible;
//	}
//
//	virtual bool getWriteVisibleFlag() override
//	{
//		return m_bWriteVisibilityFlag;
//	}
//
//	virtual void setWriteVisibleFlag(bool new_writeFlag) override 
//	{
//		m_bWriteVisibilityFlag = new_writeFlag;
//	}
//
//
//protected:
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e)
//	{
//		std::string name;
//		BuildRegFolderName( name );
//
//		tmaRegistryUtil::SetValue(tmaRegistryUtil::e_CurrentUser, name.c_str(), "Visible", "0");
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void VisibleChanged(System::Object ^  sender, System::EventArgs ^  e)
//	{
//		Write();
//	}
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void LocationChanged(System::Object ^  sender, System::EventArgs ^  e)
//	{
//		Write();
//	}
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void SizeChanged(System::Object ^  sender, System::EventArgs ^  e)
//	{
//		Write();
//	}
//
//protected:
//	System::Windows::Forms::Form^ m_pDialog;
//
//	bool m_bVisible;
//	bool m_bWriteVisibilityFlag;
//
//public:
//	static System::String^	m_CurrentLayoutConfig;
//	static char* lc_Key_LayoutConfiguration		= "Layout Configuration";
//	static char* lc_Key_LayoutConfig_Default	= "Default";
//	static char* lc_Value_Size		= "Size";
//	static char* lc_Value_Location	= "Location";
//	static char* lc_Value_Visible	= "Visible";
//};
//
//#endif // _MANAGED
