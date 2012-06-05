#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyName_ListBox.hpp
//**
//**		Intermediate class between the property (prtyName) and 
//**	the control (ListBox).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_NAME_LISTBOX_HPP
//#error prtyName_ListBox.hpp multiply included
//#endif
//#define PRTY_NAME_LISTBOX_HPP
//
//#ifndef PRTY_CALLBACKMGR_HPP
//#include "ToolUIManaged/prtym/prtyCallbackMgr.hpp"
//#endif
//#ifndef PRTY_CONTROL_HPP
//#include "ToolUIManaged/prtym/prtyControl.hpp"
//#endif
//#ifndef PRTY_CONTROLBUFFER_HPP
//#include "ToolUIManaged/prtym/prtyControlBuffer.hpp"
//#endif
//#ifndef PRTY_LISTBOXUIINFO_HPP
//#include "Core/prty/prtyListBoxUIInfo.hpp"
//#endif
//#ifndef PRTY_NAME_HPP
//#include "Core/prty/prtyName.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//#ifndef NAME_MGR_HPP
//#include "Core/name/nameMgr.hpp"
//#endif
//#ifndef NAME_STRING_HPP
//#include "Core/name/nameString.hpp"
//#endif
//#ifndef TMA_MANAGEDCONTROLUTIL_HPP
//#include "ToolUIManaged/tma/tmaManagedControlUtil.hpp"
//#endif
//#ifndef TMA_MANAGEDSTRINGUTILS_HPP
//#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
//#endif
//#ifndef UNDO_UNDOMGR_HPP
//#include "Core/undo/undoUndoMgr.hpp"
//#endif
//
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//using namespace System;
//using namespace System::Windows::Forms;
//
//
////============================================================================
////============================================================================
//public ref class prtyName_ListBox : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyName_ListBox(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != nullptr, "UI Info cannot be NULL");
//
//			m_pOriginalValue = new nameString();
//
//			//	get the ListBox list
//			//
//			prtyListBoxUIInfo* pLBUII = dynamic_cast<prtyListBoxUIInfo*>(i_pUIInfo);
//			std::vector<std::string> newvalues;
//			for (int i=0; i < pLBUII->m_List.size(); ++i)
//			{
//				newvalues.push_back( pLBUII->m_List[i] );
//			}
//
//			//	get the selected value based on the properties
//			//
//			bool bSameValue = true;
//			nameString newvalue;
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyName* pActualProperty = static_cast<prtyName*>(i_pUIInfo->GetProperty(i));
//				AddProperty(pActualProperty);
//				SetName(pActualProperty->GetPropertyName());
//
//				//	for the first value, just set the value
//				if (i==0)
//				{
//					newvalue = pActualProperty->GetValue();
//
//					//	Check the name to see if the string has changed, if so, update it.
//					//
//					std::string name_string;
//					nameMgr::GetNameString(newvalue.GetUID(), name_string);
//					if (newvalue != name_string)
//					{
//						newvalue.SetString(name_string);
//					}
//				}
//				else
//				{
//					//	check if other properties match
//					//
//					bool bDataMatches = (newvalue == pActualProperty->GetValue());
//					if (!bDataMatches)
//						bSameValue = false;
//				}
//
//				//	register the callback
//				prtyCallbackMgr::AddCallback( pActualProperty, this );
//			}
//
//			// create the actual control
//			//
//			if (pLBUII->m_bChecked)
//			{
//				//m_pActualControl = prtyCheckedListBoxControlBuffer::CreateControl();
//				m_pActualControl = gcnew System::Windows::Forms::CheckedListBox();
//				dynamic_cast<CheckedListBox^>(m_pActualControl)->CheckOnClick = true;
//			}
//			else
//			{
//				//m_pActualControl = prtyListBoxControlBuffer::CreateControl();
//				m_pActualControl = gcnew System::Windows::Forms::ListBox();
//			}
//			SetControl(m_pActualControl);
//
//			//	set the control values
//			//
//			const int lc_LINEHEIGHT = 14;
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//			m_pActualControl->Width = 100;
//			m_pActualControl->Height = pLBUII->m_List.size() * lc_LINEHEIGHT;
//			m_pActualControl->Anchor = (System::Windows::Forms::AnchorStyles)
//											(((	System::Windows::Forms::AnchorStyles::Top 
//											/*|	System::Windows::Forms::AnchorStyles::Bottom*/) 
//											|	System::Windows::Forms::AnchorStyles::Left) 
//											|	System::Windows::Forms::AnchorStyles::Right);
//			m_pActualControl->Name = "ListBox_Name";
//			//m_pActualControl->Size = System::Drawing::Size(304, 229);
//
//			m_pActualControl->Items->Clear();
//			if (bSameValue)
//			{
//				for (int j = 0; j < newvalues.size(); ++j)
//				{
//					if (pLBUII->m_bChecked)
//					{
//						bool bChecked = (strcmp(newvalues[j].c_str(),newvalue.GetString().c_str()) == 0);
//						dynamic_cast<CheckedListBox^>(m_pActualControl)->Items->Add( gcnew System::String(newvalues[j].c_str()), 
//							(bChecked? CheckState::Checked : CheckState::Unchecked) );
//					}
//					else
//					{
//						m_pActualControl->Items->Add( gcnew System::String(newvalues[j].c_str()) );
//					}
//				}
//
//				// find the correct index
//				int index = tmaManagedControlUtil::GetMatchingListBoxIndex( m_pActualControl,
//					tmaManagedStringUtils::NameStringToManagedString( newvalue ) );
//				if (index >= 0)
//				{
//					m_pActualControl->SelectedIndex = index;
//					(*m_pOriginalValue) = newvalue;
//				}
//			}
//
//			//DBG_LOG2("prtyName_ListBox constructor nv(%s) ov(%s)", newvalue.GetString().c_str(), (*m_pOriginalValue).GetString().c_str() );
//
//			//	hook up the events
//			//
//			if (pLBUII->m_bOnlyOneSelected)
//			{
//				m_pValueChangedHandler = gcnew System::EventHandler(this, &prtyName_ListBox::ListBox_SelectedIndexChanged_OneCheck);
//			}
//			else
//			{
//				m_pValueChangedHandler = gcnew System::EventHandler(this, &prtyName_ListBox::ListBox_SelectedIndexChanged);
//			}
//			m_pActualControl->SelectedIndexChanged	+= m_pValueChangedHandler;
//		}
//
//		//----------------------------------------------------------------------------
//		// Update control to match the changes in the UI Info
//		//----------------------------------------------------------------------------
//		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo) override
//		{
//			prtyListBoxUIInfo* pLBUII = dynamic_cast<prtyListBoxUIInfo*>(i_pUIInfo);
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//
//			m_pActualControl->Items->Clear();
//			for (int j = 0; j < pLBUII->m_List.size(); ++j)
//			{
//				/*if (pLBUII->m_bChecked)
//				{
//					bool bChecked = (strcmp(pLBUII->m_List[j].c_str(),newvalue.GetString().c_str()) == 0);
//					dynamic_cast<CheckedListBox^>(m_pActualControl)->Items->Add( gcnew System::String(pLBUII->m_List[j].c_str()), 
//						(bChecked? CheckState::Checked : CheckState::Unchecked) );
//				}
//				else*/
//				{
//					m_pActualControl->Items->Add( gcnew System::String(pLBUII->m_List[j].c_str()) );
//				}
//			}
//		}
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyName_ListBox()
//		{
//			delete m_pOriginalValue;
//
//			m_pActualControl->SelectedIndexChanged -= m_pValueChangedHandler;
//			//prtyListBoxControlBuffer::ReleaseControl(m_pActualControl);
//			delete m_pActualControl;
//
//			for (int i=0; i < this->GetNumberOfProperties(); ++i)
//			{
//				//	de-register the callback
//				prtyCallbackMgr::RemoveCallback(GetProperty(i), this);
//			}
//		}
//
//		//----------------------------------------------------------------------------
//		//	Note: right now, PropertyChanged will not create a circular update because
//		//	of the m_bLocalChangeNoUpdate flag.  This IS NOT true the other way
//		//	around.  If a control calls its "ValueChanged" then the property will
//		//	call all of its callback controls and one of them could have been the one
//		//	that originally updated the property value(s).  By checking the diff of 
//		//	the values we can avoid a repetitive setting of the control's values.
//		//----------------------------------------------------------------------------
//		virtual void PropertyChanged(prtyProperty* i_pProperty) override
//		{
//			if (m_bLocalChangeNoUpdate) return;
//
//			const nameString newvalue = (static_cast<prtyName*>(i_pProperty))->GetValue();
//
//			m_bLocalChangeNoUpdate = true;
//
//			if (   (m_pActualControl->SelectedItem == nullptr)
//				|| (!(tmaManagedStringUtils::NameStringEqualsManagedString(newvalue, m_pActualControl->SelectedItem->ToString()))))
//			{
//				int index = tmaManagedControlUtil::GetMatchingListBoxIndex( m_pActualControl,
//					tmaManagedStringUtils::NameStringToManagedString( newvalue ) );
//				if (index >= 0)
//					m_pActualControl->SelectedIndex = index;
//			}
//			m_bLocalChangeNoUpdate = false;
//		};
//
//	private:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		void listbox_onecheck()
//		{
//			if (m_bLocalChangeNoUpdate) return;
//			m_bLocalChangeNoUpdate = true;
//
//			//	First unselect all and then select only the current one
//			//
//			CheckedListBox^ pCLBControl = dynamic_cast<CheckedListBox^>(m_pActualControl);
//			IEnumerator^ myEnum1 = pCLBControl->CheckedIndices->GetEnumerator();
//			while (myEnum1->MoveNext()) 
//			{
//				Int32 indexChecked =  *safe_cast<Int32^>(myEnum1->Current);
//				pCLBControl->SetItemCheckState(indexChecked,CheckState::Unchecked);
//			}
//
//			//	check just the one
//			pCLBControl->SetItemCheckState( m_pActualControl->SelectedIndex, CheckState::Checked );
//
//			m_bLocalChangeNoUpdate = false;
//		}
//
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		System::Void ListBox_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
//		{
//			if (m_bLocalChangeNoUpdate) return;
//			m_bLocalChangeNoUpdate = true;
//
//			int index = tmaManagedControlUtil::GetMatchingListBoxIndex( m_pActualControl,
//				tmaManagedStringUtils::NameStringToManagedString( (*m_pOriginalValue) ) );
//			//causes missed updates--if (index != m_pActualControl->SelectedIndex)
//			{
//				const int num_properties = this->GetNumberOfProperties();
//				if (num_properties > 1)
//					undoUndoMgr::BeginMultipleOperationBlock();
//
//				for (int i=0; i < num_properties; ++i)
//				{
//					prtyName* pActualProperty = static_cast<prtyName*>(GetProperty(i));
//					nameString newvalue;
//					tmaManagedStringUtils::ManagedStringToNameString( m_pActualControl->SelectedItem->ToString(), newvalue );
//					pActualProperty->SetValue( newvalue, prtyProperty::eNewUndo );	// store UNDO also
//				}
//
//				if (num_properties > 1)
//					undoUndoMgr::EndMultipleOperationBlock();
//			}
//
//			m_bLocalChangeNoUpdate = false;
//		};
//
//		//----------------------------------------------------------------------------
//		// This event occurs after the user selects a new item in the list.
//		//----------------------------------------------------------------------------
//		void ListBox_SelectedIndexChanged(System::Object ^sender, System::EventArgs^ e)
//		{
//			ListBox_ValueChanged( sender, e );
//		}
//
//		//----------------------------------------------------------------------------
//		// This event occurs after the user selects a new item in the list.
//		//----------------------------------------------------------------------------
//		void ListBox_SelectedIndexChanged_OneCheck(System::Object ^sender, System::EventArgs^ e)
//		{
//			listbox_onecheck();
//			ListBox_ValueChanged( sender, e );
//		}
//
//public:
//	System::Windows::Forms::ListBox^ m_pActualControl;
//	nameString* m_pOriginalValue;
//	System::EventHandler^ m_pValueChangedHandler;
//};
//
//#endif // _MANAGED
