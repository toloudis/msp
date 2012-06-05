#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyFileName_ComboBox.hpp
//**
//**		Intermediate class between the property (prtyFileName) and 
//**	the control (ComboBox) 
//**
//**	StudioGPU
//**	Copyright(C) 2007 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_FILENAME_COMBOBOX_HPP
//#error prtyFileName_ComboBox.hpp multiply included
//#endif
//#define PRTY_FILENAME_COMBOBOX_HPP
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
//#ifndef PRTY_FILENAME_HPP
//#include "Core/prty/prtyFileName.hpp"
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
//public ref class prtyFileName_ComboBox : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyFileName_ComboBox(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != 0, "UI Info cannot be NULL");
//
//			bool bSameValue = true;
//			std::string newvalue;
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyFileName* pActualProperty = static_cast<prtyFileName*>(i_pUIInfo->GetProperty(i));
//				AddProperty(pActualProperty);
//				SetName(pActualProperty->GetPropertyName());
//
//				if (i==0)
//				{
//					newvalue = pActualProperty->GetString();
//				}
//				else
//				{
//					if (newvalue != pActualProperty->GetString())
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
//			m_pValueChangedHandler = gcnew System::EventHandler(this, &prtyFileName_ComboBox::control_ValueChanged);
//			m_pActualControl->SelectedIndexChanged	+= m_pValueChangedHandler;
//		};
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyFileName_ComboBox()
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
//			const std::string str = (static_cast<prtyFileName*>(i_pProperty))->GetString();
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
//					prtyFileName* pActualProperty = static_cast<prtyFileName*>(GetProperty(i));
//					itString file_name;
//					tmaManagedStringUtils::ManagedStringToItString(m_pActualControl->Text, file_name);
//					pActualProperty->SetValue( file_name, prtyProperty::eNewUndo );	// store UNDO also
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
