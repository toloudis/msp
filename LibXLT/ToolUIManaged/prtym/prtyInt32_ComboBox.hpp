#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyInt32_ComboBox.hpp
//**
//**		Intermediate class between the property (prtyInt32) and 
//**	the control (ComboBox).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_INT32_COMBOBOX_HPP
//#error prtyInt32_ComboBox.hpp multiply included
//#endif
//#define PRTY_INT32_COMBOBOX_HPP
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
//#ifndef PRTY_INT32_HPP
//#include "Core/prty/prtyInt32.hpp"
//#endif
//#ifndef PRTY_COMBOBOXUIINFO_HPP
//#include "Core/prty/prtyComboBoxUIInfo.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//#ifndef DBG_LOG_HPP
//#include "Core/dbg/dbgLog.hpp"
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
//public ref class prtyInt32_ComboBox : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyInt32_ComboBox(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != 0, "UI Info cannot be NULL");
//
//			prtyComboBoxUIInfo* pUII = static_cast<prtyComboBoxUIInfo*>(i_pUIInfo);
//
//			bool bSameValue = true;
//			std::vector<std::string> newvalues;
//			int newvalue = -1;
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyInt32* pActualProperty = static_cast<prtyInt32*>(i_pUIInfo->GetProperty(i));
//				AddProperty(pActualProperty);
//				SetName(pActualProperty->GetPropertyName());
//
//				if (i==0)
//				{
//					newvalue = pActualProperty->GetValue();
//
//					for (int j = 0; j < pUII->m_List.size(); ++j)
//					{
//						newvalues.push_back( pUII->m_List[j] );
//					}
//				}
//				else
//				{
//					bool bDataMatches = (	(newvalue == pActualProperty->GetValue())
//										 && (pUII->m_List.size() == newvalues.size()));
//					if (bDataMatches)
//					{
//						for (int j = 0; j < pUII->m_List.size(); ++j)
//						{
//							if (strcmp(newvalues[j].c_str(), pUII->m_List[j].c_str()) != 0) 
//							{
//								bDataMatches = false;
//								break;
//							}
//						}
//					}
//
//					if (!bDataMatches)
//						bSameValue = false;
//				}
//
//				//	register the callback
//				prtyCallbackMgr::AddCallback( pActualProperty, this );
//
//				// create the actual control
//				m_pActualControl = prtyComboBoxControlBuffer::CreateControl();
//				//m_pActualControl = gcnew System::Windows::Forms::ComboBox();
//				prtyComboBoxUIInfo* pCBUII = dynamic_cast<prtyComboBoxUIInfo*>(i_pUIInfo);
//				m_pActualControl->Width = pCBUII->GetWidth();
//				SetControl(m_pActualControl);
//				m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//
//				m_OriginalValue = 1;
//				m_pActualControl->Items->Clear();
//				if (bSameValue)
//				{
//					for (int j = 0; j < newvalues.size(); ++j)
//					{
//						m_pActualControl->Items->Add( gcnew System::String(newvalues[j].c_str()) );
//					}
//
//					if ((newvalue >= -1) && (newvalue < m_pActualControl->Items->Count))
//					{
//						m_pActualControl->SelectedIndex = newvalue;
//						m_OriginalValue = newvalue;
//					}
//				}
//
//				m_pActualControl->Width = 64;
//				int newvalue = pActualProperty->GetValue();
//				
//				int index = -1;
//				index = find_index( System::Convert::ToString(newvalue) );
//				DBG_ASSERT1( index > -1, "Could not find (%d) in combobox list", newvalue);
//				m_pActualControl->SelectedIndex = index;
//				m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//
//				//	hook up the events
//				//
//				m_pValueChangedHandler = gcnew System::EventHandler(this, &prtyInt32_ComboBox::control_IndexChanged);
//				m_pActualControl->SelectedIndexChanged	+= m_pValueChangedHandler;
//			}
//		}
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyInt32_ComboBox()
//		{
//			m_pActualControl->SelectedIndexChanged -= m_pValueChangedHandler;
//			prtyComboBoxControlBuffer::ReleaseControl(m_pActualControl);
//			//m_pActualControl = nullptr;	//m_pActualControl->Dispose(true) is protected
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
//			const int newvalue = (static_cast<prtyInt32*>(i_pProperty))->GetValue();
//
//			m_bLocalChangeNoUpdate = true;
//			int index = -1;
//			index = find_index( System::Convert::ToString(newvalue) );
//			DBG_ASSERT1( index > -1, "Could not find (%d) in combobox list", newvalue);
//			if (m_pActualControl->SelectedIndex != index)
//			{
//				//m_OriginalValue = index;
//				m_pActualControl->SelectedIndex = index;
//			}
//			m_bLocalChangeNoUpdate = false;
//		}
//
//		//----------------------------------------------------------------------------
//		// Update control to match the changes in the UI Info
//		//----------------------------------------------------------------------------
//		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo) override
//		{
//			prtyComboBoxUIInfo* pUII = static_cast<prtyComboBoxUIInfo*>(i_pUIInfo);
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//
//			m_pActualControl->Items->Clear();
//			std::vector<std::string> newvalues = pUII->m_List;
//			for (int j = 0; j < newvalues.size(); ++j)
//			{
//				m_pActualControl->Items->Add( gcnew System::String( newvalues[j].c_str() ) );
//			}
//		}
//
//	private:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		System::Void control_IndexChanged(System::Object ^  sender, System::EventArgs ^  e)
//		{
//			if (m_bLocalChangeNoUpdate) return;
//
//			m_bLocalChangeNoUpdate = true;
//			if (m_pActualControl->Text->Length > 0)
//			{
//				int value = System::Convert::ToInt32(m_pActualControl->Text);
//
//				//causes missed updates--if (m_OriginalValue != value)
//				{
//					const int num_properties = this->GetNumberOfProperties();
//					if (num_properties > 1)
//						undoUndoMgr::BeginMultipleOperationBlock();
//
//					for (int i=0; i < num_properties; ++i)
//					{
//						prtyInt32* pActualProperty = static_cast<prtyInt32*>(GetProperty(i));
//						pActualProperty->SetValue( value, prtyProperty::eNewUndo );	// store UNDO also
//					}
//
//					if (num_properties > 1)
//						undoUndoMgr::EndMultipleOperationBlock();
//				}
//			}
//			m_bLocalChangeNoUpdate = false;
//		};
//
//		//----------------------------------------------------------------------------
//		// This event occurs after the KeyDown event and can be used to prevent
//		// characters from entering the control.
//		//----------------------------------------------------------------------------
//		void control_KeyPress(System::Object ^sender, System::Windows::Forms::KeyPressEventArgs^ e)
//		{
//			if (e->KeyChar == (char)13)
//			{
//				control_IndexChanged( sender, e );
//			}
//		}
//
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		int find_index(System::String^ i_pText)
//		{
//			int index = -1;
//			for (int i=0; i < m_pActualControl->Items->Count; ++i)
//			{
//				std::string one, two;
//				tmaManagedStringUtils::ManagedStringToStdString(m_pActualControl->Items[i]->ToString(), one);
//				tmaManagedStringUtils::ManagedStringToStdString(i_pText, two);
//				//DBG_LOG3("%02d) (%s) vs (%s)", i, one.c_str(), two.c_str() );
//
//				if (m_pActualControl->Items[i]->ToString()->Equals(i_pText))
//				{
//					index = i;
//					break;
//				}
//			}
//			return index;
//		}
//
//public:
//	System::Windows::Forms::ComboBox^ m_pActualControl;
//	int m_OriginalValue;
//	System::EventHandler^ m_pValueChangedHandler;
//};
//
//#endif // _MANAGED
