#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyText_ComboBox.hpp
//**
//**		Intermediate class between the property (prtyText) and 
//**	the control (ComboBox) 
//**
//**	StudioGPU
//**	Copyright(C) 2007 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_TEXT_COMBOBOX_HPP
//#error prtyText_ComboBox.hpp multiply included
//#endif
//#define PRTY_TEXT_COMBOBOX_HPP
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
//#ifndef PRTY_TEXT_HPP
//#include "Core/prty/prtyText.hpp"
//#endif
//#ifndef PRTY_COMBOBOXUIINFO_HPP
//#include "Core/prty/prtyComboBoxUIInfo.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
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
//public ref class prtyText_ComboBox : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyText_ComboBox(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != 0, "UI Info cannot be NULL");
//
//			bool bSameValue = true;
//			std::string newvalue;
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyText* pActualProperty = static_cast<prtyText*>(i_pUIInfo->GetProperty(i));
//				AddProperty(pActualProperty);
//				SetName(pActualProperty->GetPropertyName());
//
//				if (i==0)
//				{
//					newvalue = pActualProperty->GetValue();
//				}
//				else
//				{
//					if (newvalue != pActualProperty->GetValue())
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
//			prtyComboBoxUIInfo* pUII = static_cast<prtyComboBoxUIInfo*>(i_pUIInfo);
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//
//			std::vector<std::string> newvalues = pUII->m_List;
//			//pUII->GetListOfChoices( newvalues );
//
//			m_OriginalValue = -1;
//			m_pActualControl->Items->Clear();
//			m_pActualControl->Text = "";
//			if (bSameValue)
//			{
//				m_pActualControl->Text = gcnew System::String(newvalue.c_str());
//
//				for (int j = 0; j < newvalues.size(); ++j)
//				{
//					m_pActualControl->Items->Add( gcnew System::String(newvalues[j].c_str()) );
//
//					//if (strncmp( newvalue.c_str, newvalues[j].c_str(), newvalue.size() ) == 0)
//					if (newvalue == newvalues[j])
//					{
//						m_pActualControl->SelectedIndex = j;
//						m_OriginalValue = j;
//					}
//				}
//			}
//
//			//	hook up the events
//			//
//			m_pValueChangedHandler = gcnew System::EventHandler(this, &prtyText_ComboBox::control_ValueChanged);
//			m_pActualControl->SelectedIndexChanged	+= m_pValueChangedHandler;
//		};
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyText_ComboBox()
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
//			const std::string str = (static_cast<prtyText*>(i_pProperty))->GetValue();
//			System::String ^newvalue = gcnew System::String(str.c_str());
//
//			m_bLocalChangeNoUpdate = true;
//			if ( m_pActualControl->Text != newvalue )
//			{
//				m_pActualControl->Text = newvalue;
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
//
//			//	store the current selected text and reselect it, if it is added
//			//
//			System::String ^seltxt = gcnew System::String("");
//			if (m_pActualControl->SelectedItem != nullptr)
//				seltxt = m_pActualControl->SelectedItem->ToString();
//
//			m_pActualControl->SuspendLayout();
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//			m_pActualControl->Text = "";
//			m_pActualControl->Items->Clear();
//
//			//	loop through the list and add all the elements
//			int selindex = -1;
//			for (int j = 0; j < pUII->m_List.size(); ++j)
//			{
//				System::String^ addtxt = gcnew System::String( pUII->m_List[j].c_str() );
//				if (seltxt == addtxt)
//				{
//					selindex = j;
//				}
//				m_pActualControl->Items->Add( addtxt );
//			}
//
//			m_pActualControl->SelectedIndex = (m_pActualControl->Items->Count > 0) ? selindex : -1;
//			//m_pActualControl->SelectedIndex = j;
//			//m_pActualControl->Text = addtxt;
//			m_pActualControl->Invalidate();
//			m_pActualControl->Update();
//			m_pActualControl->ResumeLayout();
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
//					prtyText* pActualProperty = static_cast<prtyText*>(GetProperty(i));
//					std::string str;
//					tmaManagedStringUtils::ManagedStringToStdString(m_pActualControl->Text, str);
//					pActualProperty->SetValue( str, prtyProperty::eNewUndo );	// store UNDO also
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
