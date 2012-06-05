#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyEnum_ComboBox.hpp
//**
//**		Intermediate class between the property (prtyEnum) and 
//**	the control (ComboBox).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_ENUM_COMBOBOX_HPP
//#error prtyEnum_ComboBox.hpp multiply included
//#endif
//#define PRTY_ENUM_COMBOBOX_HPP
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
//#ifndef PRTY_ENUM_HPP
//#include "Core/prty/prtyEnum.hpp"
//#endif
//#ifndef PRTY_COMBOBOXUIINFO_HPP
//#include "Core/prty/prtyComboBoxUIInfo.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//#ifndef UNDO_UNDOMGR_HPP
//#include "Core/undo/undoUndoMgr.hpp"
//#endif
//
//#include <limits>
//
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//public ref class prtyEnum_ComboBox : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyEnum_ComboBox(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != 0, "UI Info cannot be NULL");
//
//			bool bSameValue = true;
//			short newvalue;
//			std::vector<std::string> newvalues;
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyEnum* pActualProperty = static_cast<prtyEnum*>(i_pUIInfo->GetProperty(i));
//				AddProperty(pActualProperty);
//				SetName(pActualProperty->GetPropertyName());
//
//				if (i==0)
//				{
//					newvalue = pActualProperty->GetValue();
//
//					for (int j = 0; j < pActualProperty->m_EnumTags.size(); ++j)
//					{
//						newvalues.push_back( pActualProperty->m_EnumTags[j] );
//					}
//				}
//				else
//				{
//					bool bDataMatches = (	(newvalue == pActualProperty->GetValue())
//										 && (pActualProperty->m_EnumTags.size() == newvalues.size()));
//					if (bDataMatches)
//					{
//						for (int j = 0; j < pActualProperty->m_EnumTags.size(); ++j)
//						{
//							if (strcmp(newvalues[j].c_str(), pActualProperty->m_EnumTags[j].c_str()) != 0)
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
//			}
//
//			// create the actual control
//			m_pActualControl = prtyComboBoxControlBuffer::CreateControl();
//			//m_pActualControl = gcnew System::Windows::Forms::ComboBox();
//			prtyComboBoxUIInfo* pCBUII = dynamic_cast<prtyComboBoxUIInfo*>(i_pUIInfo);
//			m_pActualControl->Width = pCBUII->GetWidth();
//			SetControl(m_pActualControl);
//
//			//	set the control values
//			m_OriginalValue = 0;
//			m_pActualControl->Items->Clear();
//			m_pActualControl->Text = "";
//			if (bSameValue)
//			{
//				for (int j = 0; j < newvalues.size(); ++j)
//				{
//					m_pActualControl->Items->Add( gcnew System::String(newvalues[j].c_str()) );
//				}
//
//				if ((newvalue >= -1) && (newvalue < m_pActualControl->Items->Count))
//				{
//					m_pActualControl->SelectedIndex = newvalue;
//					m_OriginalValue = newvalue;
//				}
//			}
//
//			//	hook up the events
//			//
//			m_pValueChangedHandler = gcnew System::EventHandler(this, &prtyEnum_ComboBox::control_ValueChanged);
//			m_pActualControl->SelectedIndexChanged	+= m_pValueChangedHandler;
//		};
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyEnum_ComboBox()
//		{
//			m_pActualControl->SelectedIndexChanged -= m_pValueChangedHandler;
//			prtyComboBoxControlBuffer::ReleaseControl(m_pActualControl);
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
//			const short newvalue = (static_cast<prtyEnum*>(i_pProperty))->GetValue();
//
//			m_bLocalChangeNoUpdate = true;
//			if ( m_pActualControl->SelectedIndex != newvalue )
//			{
//				m_pActualControl->SelectedIndex = newvalue;
//				//m_OriginalValue = newvalue;
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
//			if (i_pUIInfo->GetNumberOfProperties() > 0)
//			{
//				prtyEnum* pActualProperty = static_cast<prtyEnum*>(i_pUIInfo->GetProperty(0));
//
//				m_pActualControl->Items->Clear();
//				for (int j = 0; j < pActualProperty->m_EnumTags.size(); ++j)
//				{
//					m_pActualControl->Items->Add( gcnew System::String( pActualProperty->m_EnumTags[j].c_str() ) );
//				}
//			}
//		}
//
//	private:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		System::Void control_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
//		{
//			if (m_bLocalChangeNoUpdate) return;
//
//			m_bLocalChangeNoUpdate = true;
//			//causes missed updates--if (m_OriginalValue != (envType::Int8)m_pActualControl->SelectedIndex)
//			{
//				const int num_properties = this->GetNumberOfProperties();
//				if (num_properties > 1)
//					undoUndoMgr::BeginMultipleOperationBlock();
//
//				for (int i=0; i < num_properties; ++i)
//				{
//					prtyEnum* pActualProperty = static_cast<prtyEnum*>(GetProperty(i));
//					pActualProperty->SetValue( (envType::Int8)m_pActualControl->SelectedIndex, prtyProperty::eNewUndo );	// store UNDO also
//				}
//
//				if (num_properties > 1)
//					undoUndoMgr::EndMultipleOperationBlock();
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
//				control_ValueChanged( sender, e );
//			}
//		}
//
//public:
//	System::Windows::Forms::ComboBox^ m_pActualControl;
//	short m_OriginalValue;
//	System::EventHandler^ m_pValueChangedHandler;
//};
//
//#endif // _MANAGED
