#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyListChecked_ListBox.hpp
//**
//**		Intermediate class between the property (prtyListChecked) and 
//**	the control (ListBox).
//**
//**	StudioGPU
//**	Copyright(C) 2007 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_LISTCHECKED_LISTBOX_HPP
//#error prtyListChecked_ListBox.hpp multiply included
//#endif
//#define PRTY_LISTCHECKED_LISTBOX_HPP
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
//#ifndef PRTY_LISTCHECKED_HPP
//#include "Core/prty/prtyListChecked.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//#ifndef DBG_MSG_HPP
//#include "Core/dbg/dbgMsg.hpp"
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
//public ref class prtyListChecked_ListBox : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyListChecked_ListBox(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != nullptr, "UI Info cannot be NULL");
//
//			m_pOriginalValue = new checked_list_type();
//
//			//	get the ListBox list
//			//
//			prtyListBoxUIInfo* pLBUII = dynamic_cast<prtyListBoxUIInfo*>(i_pUIInfo);
//			checked_list_type newvalues;
//			newvalues.resize(pLBUII->m_List.size());
//			for (int i=0; i < pLBUII->m_List.size(); ++i)
//			{
//				newvalues[i].m_Text = pLBUII->m_List[i];
//				newvalues[i].m_bChecked = pLBUII->m_Checked[i];
//			}
//
//			//	get the selected value based on the properties
//			//
//			bool bSameValue = true;
//			std::string newvalue;
//			
//			// TODO implement?
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyListChecked* pActualProperty = static_cast<prtyListChecked*>(i_pUIInfo->GetProperty(i));
//				AddProperty(pActualProperty);
//				//SetName(pActualProperty->GetName());
//			
//				//	for the first value, just set the value
//			//	if (i==0)
//			//	{
//			//		newvalue = pActualProperty->GetValueText(0);
//			//	}
//			//	else
//				{
//			//		//	check if other properties match
//			//		//
//			//		bool bDataMatches = (newvalue == pActualProperty->GetValueText(0));
//			//		if (!bDataMatches)
//			//			bSameValue = false;
//				}
//			
//				//	register the callback
//				prtyCallbackMgr::AddCallback( pActualProperty, this );
//			}
//
//			// create the actual control
//			//
//			//if (pLBUII->m_bChecked)
//			//{
//				m_pActualControl = prtyCheckedListBoxControlBuffer::CreateControl();
//				//m_pActualControl = gcnew System::Windows::Forms::CheckedListBox();
//				dynamic_cast<CheckedListBox^>(m_pActualControl)->CheckOnClick = true;
//			//}
//			//else
//			//{
//			//	m_pActualControl = gcnew System::Windows::Forms::ListBox();
//			//}
//			SetControl(m_pActualControl);
//
//			//	set the control values
//			//
//			const int lc_LINEHEIGHT = 15;
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//			m_pActualControl->Width = 300;
//			m_pActualControl->Height = ((pLBUII->m_List.size()+1) * lc_LINEHEIGHT);
//			m_pActualControl->Anchor = (System::Windows::Forms::AnchorStyles)
//											(((	System::Windows::Forms::AnchorStyles::Top 
//											/*|	System::Windows::Forms::AnchorStyles::Bottom*/) 
//											|	System::Windows::Forms::AnchorStyles::Left) 
//											|	System::Windows::Forms::AnchorStyles::Right);
//			m_pActualControl->ScrollAlwaysVisible = false;
//			m_pActualControl->Sorted = true;
//			m_pActualControl->Name = "ListBox_ListChecked";
//			//m_pActualControl->Size = System::Drawing::Size(304, 229);
//
//			if (bSameValue)
//			{
//				UpdateControlFromList( newvalues );
//				
//				//// find the correct index
//				//int index = tmaManagedControlUtil::GetMatchingListBoxIndex( m_pActualControl,
//				//	newvalue.c_str() );
//				//if (index >= 0)
//				{
//				//	m_pActualControl->SelectedIndex = index;
//					(*m_pOriginalValue) = newvalues;
//				}
//			}
//
//			//DBG_LOG2("prtyListChecked_ListBox constructor nv(%s) ov(%s)", newvalue.GetString().c_str(), (*m_pOriginalValue).GetString().c_str() );
//
//			//	hook up the events
//			//
//			if (pLBUII->m_bOnlyOneSelected)
//			{
//				m_pValueChangedHandler = gcnew System::EventHandler(this, &prtyListChecked_ListBox::ListBox_SelectedIndexChanged_OneCheck);
//			}
//			else
//			{
//				m_pValueChangedHandler = gcnew System::EventHandler(this, &prtyListChecked_ListBox::ListBox_SelectedIndexChanged);
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
//
//			m_pActualControl->SuspendLayout();
//			m_pActualControl->Items->Clear();
//			for (int j = 0; j < pLBUII->m_List.size(); ++j)
//			{
//				bool bChecked = pLBUII->m_Checked[j];
//				dynamic_cast<CheckedListBox^>(m_pActualControl)->Items->Add( gcnew System::String(pLBUII->m_List[j].c_str()), 
//						bChecked );
//			}
//
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//			m_pActualControl->ResumeLayout();
//		}
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyListChecked_ListBox()
//		{
//			delete m_pOriginalValue;
//			m_pActualControl->SelectedIndexChanged -= m_pValueChangedHandler;
//			prtyCheckedListBoxControlBuffer::ReleaseControl(m_pActualControl);
//			//delete m_pActualControl;
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
//			//const std::string newvalue = (static_cast<prtyListChecked*>(i_pProperty))->GetValueText();
//
//			m_bLocalChangeNoUpdate = true;
//
//			//	update the control with the new property data
//			//
//			prtyListChecked* pActualProperty = static_cast<prtyListChecked*>(i_pProperty);
//			if (pActualProperty != nullptr)
//			{
//				checked_list_type itemlist = pActualProperty->GetValue();
//				UpdateControlFromList( itemlist );
//			}
//
//			m_bLocalChangeNoUpdate = false;
//		};
//
//	private:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		void UpdateControlFromList( checked_list_type& i_List )
//		{
//			//	TODO need a better way than clearing + redoing the list...
//			//
//			this->m_pActualControl->SuspendLayout();
//			dynamic_cast<CheckedListBox^>(m_pActualControl)->Items->Clear();
//
//			for (int j = 0; j < i_List.size(); ++j)
//			{
//				bool bChecked = i_List[j].m_bChecked;
//				dynamic_cast<CheckedListBox^>(m_pActualControl)->Items->Add( gcnew System::String(i_List[j].m_Text.c_str()), 
//						bChecked );
//			}
//			this->m_pActualControl->ResumeLayout();
//		}
//
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
//			prtyListBoxUIInfo* pLCUII = dynamic_cast<prtyListBoxUIInfo*>(this->m_pUIInfo);
//
//			//	build the list
//			//
//			//DBG_LOG("-------------------ListBox_ValueChanged ");
//			checked_list_type itemlist;
//			itemlist.resize( m_pActualControl->Items->Count );
//			for ( int i = 0; i < m_pActualControl->Items->Count; i++ )
//			{
//				std::string acitem;
//				System::Object^ pObj = m_pActualControl->Items[i];
//				tmaManagedStringUtils::ManagedStringToStdString( pObj->ToString(), acitem );
//				bool acchecked = m_pActualControl->GetItemChecked(i);
//				//DBG_LOG3("FIND  %03d %s %s", i, acitem.c_str(), acchecked?"true":"false");
//
//				//	find the matching item
//				for (int j=0; j < itemlist.size(); ++j)
//				{
//					//DBG_LOG4("FIND %03d %s vs %03d %s", i, acitem.c_str(), j, itemlist[j].m_Text.c_str());
//					if (pLCUII->GetItemText(j) == acitem)
//					{
//						itemlist[j].m_bChecked = acchecked;
//						itemlist[j].m_Text = pLCUII->m_List[j];
//						//DBG_LOG4("MATCH %03d %s vs %03d %s", i, acitem.c_str(), j, itemlist[j].m_Text.c_str());
//
//						//	update the UIInfo checked state also
//						pLCUII->SetItem( j, acchecked );
//						break;
//					}
//				}
//			}
//
//			//	update the properties with the new value
//			//
//			const int num_properties = this->GetNumberOfProperties();
//			if (num_properties > 1)
//				undoUndoMgr::BeginMultipleOperationBlock();
//
//			for (int i=0; i < num_properties; ++i)
//			{
//				prtyListChecked* pActualProperty = static_cast<prtyListChecked*>(GetProperty(i));
//				pActualProperty->SetValue( itemlist, prtyProperty::eNewUndo );	// store UNDO also
//			}
//
//			if (num_properties > 1)
//				undoUndoMgr::EndMultipleOperationBlock();
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
//	System::Windows::Forms::CheckedListBox^ m_pActualControl;
//	checked_list_type* m_pOriginalValue;
//	System::EventHandler^ m_pValueChangedHandler;
//};
//
//#endif // _MANAGED
