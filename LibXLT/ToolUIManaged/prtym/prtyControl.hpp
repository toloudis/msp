#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyControl.hpp
//**
//**		Class that ties the property and the actual control together.
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_CONTROL_HPP
//#error prtyControl.hpp multiply included
//#endif
//#define PRTY_CONTROL_HPP
//
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
//
//#include <string>
//#include <vector>
//
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//public ref class prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyControl(prtyPropertyUIInfo* i_pUIInfo)
//		:	m_bLocalChangeNoUpdate(false),
//			m_pUIInfo(i_pUIInfo)
//		{
//			m_Name = new std::string();
//			m_Properties = new std::vector<prtyProperty*>;
//
//		};
//
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		~prtyControl()
//		{
//			delete m_Name;
//			delete m_Properties;
//		}
//
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		std::string* GetName()
//		{
//			return m_Name;
//		};
//
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		void SetName(const std::string& i_Name)
//		{
//			*m_Name = i_Name.c_str();
//		};
//
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		System::Windows::Forms::Control^ GetControl()
//		{
//			return m_pControl;
//		};
//
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyProperty* GetProperty(int i_Index)
//		{
//			DBG_ASSERT0( (i_Index >= 0) && (i_Index < m_Properties->size()), "Invalid property index" );
//			return (*m_Properties)[i_Index];
//		};
//
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		int GetNumberOfProperties()
//		{
//			return m_Properties->size();
//		};
//
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		void SetProperty(prtyProperty* i_pProperty, int i_Index)
//		{
//			DBG_ASSERT0(i_pProperty != 0, "Cannot add a NULL property");
//			DBG_ASSERT0( (i_Index >= 0) && (i_Index < m_Properties->size()), "Invalid property index" );
//
//			(*m_Properties)[i_Index] = i_pProperty;
//		};
//		void AddProperty(prtyProperty* i_pProperty)
//		{
//			DBG_ASSERT0(i_pProperty != 0, "Cannot add a NULL property");
//
//			m_Properties->push_back(i_pProperty);
//		};
//
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		virtual void PropertyChanged(prtyProperty* i_pProperty)
//		{
//		};
//
//		//----------------------------------------------------------------------------
//		// Return true if this control is attached to this UI Info
//		//----------------------------------------------------------------------------
//		bool HasUIInfo(prtyPropertyUIInfo* i_pUIInfo)
//		{
//			return (m_pUIInfo == i_pUIInfo);
//		};
//
//		//----------------------------------------------------------------------------
//		// Update control to match the changes in the UI Info
//		//----------------------------------------------------------------------------
//		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo)
//		{
//		};
//
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		void SetToolTip()
//		{
//			const bool lc_UseToolTips = true;
//			if (lc_UseToolTips && (m_pControl != nullptr) && (m_pUIInfo != nullptr))
//			{
//				System::Windows::Forms::ToolTip^ ToolTip1 = gcnew System::Windows::Forms::ToolTip();
//				ToolTip1->IsBalloon = true;
//				ToolTip1->SetToolTip(m_pControl, gcnew System::String(m_pUIInfo->GetDescription().c_str()));
//			}
//		};
//
//		//----------------------------------------------------------------------------
//		//	Set the control
//		//----------------------------------------------------------------------------
//		void SetControl( System::Windows::Forms::Control^ i_pControl )
//		{
//			m_pControl = i_pControl;
//
//			//	Set-up the tooltip
//			SetToolTip();
//		};
//
//	protected:
//		System::Windows::Forms::Control^ m_pControl;
//		prtyPropertyUIInfo* m_pUIInfo;
//		bool	m_bLocalChangeNoUpdate;
//
//	private:
//		std::string*	m_Name;
//		std::vector<prtyProperty*>*	m_Properties;
//};
//
//#endif // _MANAGED
