#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyText_TextBox.hpp
//**
//**		Intermediate class between the property (prtyText) and 
//**	the control (TextBox).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_TEXT_TEXTBOX_HPP
//#error prtyText_TextBox.hpp multiply included
//#endif
//#define PRTY_TEXT_TEXTBOX_HPP
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
//#ifndef PRTY_TEXTBOXUIINFO_HPP
//#include "Core/prty/prtyTextBoxUIInfo.hpp"
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
//public ref class prtyText_TextBox : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyText_TextBox(prtyPropertyUIInfo* i_pUIInfo)
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
//					newvalue = pActualProperty->GetValue();
//				else
//					if (newvalue != pActualProperty->GetValue())
//						bSameValue = false;
//
//				//	register the callback
//				prtyCallbackMgr::AddCallback( pActualProperty, this );
//			}
//
//			// create the actual control
//			m_pActualControl = prtyTextBoxControlBuffer::CreateControl();
//			//m_pActualControl = gcnew System::Windows::Forms::TextBox();
//			SetControl(m_pActualControl);
//
//			//	Set the control values
//			//
//			this->UpdateControl(i_pUIInfo);
//
//			m_pOriginalValue = new std::string();
//			if (bSameValue)
//			{
//				m_pActualControl->Text = gcnew System::String(newvalue.c_str());
//				(*m_pOriginalValue) = newvalue;
//			}
//
//			//	hook up the events
//			//
//			m_pLeaveHandler = gcnew System::EventHandler(this, &prtyText_TextBox::textBox_ValueChanged);
//			m_pActualControl->Leave	+= m_pLeaveHandler;
//			m_pKeyPressHandler = gcnew System::Windows::Forms::KeyPressEventHandler(this, &prtyText_TextBox::textBox_KeyPress);
//			m_pActualControl->KeyPress	+= m_pKeyPressHandler;
//		}
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyText_TextBox()
//		{
//			m_pActualControl->Leave -= m_pLeaveHandler;
//			m_pActualControl->KeyPress -= m_pKeyPressHandler;
//			prtyTextBoxControlBuffer::ReleaseControl(m_pActualControl);
//			//delete m_pActualControl;
//			delete m_pOriginalValue;
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
//			const std::string& newvalue = (static_cast<prtyText*>(i_pProperty))->GetValue();
//
//			m_bLocalChangeNoUpdate = true;
//			std::string currvalue;
//			tmaManagedStringUtils::ManagedStringToStdString(m_pActualControl->Text, currvalue);
//
//			if ( strcmp(currvalue.c_str(), newvalue.c_str()) != 0 )
//			{
//				m_pActualControl->Text = gcnew System::String(newvalue.c_str());
//				//(*m_pOriginalValue) = newvalue;
//			}
//			m_bLocalChangeNoUpdate = false;
//		}
//
//		//----------------------------------------------------------------------------
//		// Update control to match the changes in the UI Info
//		//----------------------------------------------------------------------------
//		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo) override
//		{
//			prtyTextBoxUIInfo* pTBUII = static_cast<prtyTextBoxUIInfo*>(i_pUIInfo);
//			m_pActualControl->MaxLength = pTBUII->GetMaxChars();
//			float fontsize = m_pActualControl->Font->Size;
//			m_pActualControl->Width = (pTBUII->GetMaxWidth() * (int)fontsize);
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//		}
//
//	private:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		System::Void textBox_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
//		{
//			if (m_bLocalChangeNoUpdate) return;
//
//			m_bLocalChangeNoUpdate = true;
//			std::string newvalue;
//			tmaManagedStringUtils::ManagedStringToStdString(m_pActualControl->Text, newvalue);
//
//			if (strcmp((*m_pOriginalValue).c_str(), newvalue.c_str()) != 0)
//			{
//				const int num_properties = this->GetNumberOfProperties();
//				if (num_properties > 1)
//					undoUndoMgr::BeginMultipleOperationBlock();
//
//				for (int i=0; i < num_properties; ++i)
//				{
//					prtyText* pActualProperty = static_cast<prtyText*>(GetProperty(i));
//					pActualProperty->SetValue( newvalue, prtyProperty::eNewUndo );	// store UNDO also
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
//		void textBox_KeyPress(System::Object ^sender, System::Windows::Forms::KeyPressEventArgs^ e)
//		{
//			if (e->KeyChar == (char)13)
//			{
//				textBox_ValueChanged( sender, e );
//				m_pActualControl->SelectAll();
//			}
//		}
//
//public:
//	System::Windows::Forms::TextBox^ m_pActualControl;
//	std::string* m_pOriginalValue;
//	System::EventHandler^ m_pLeaveHandler;
//	System::Windows::Forms::KeyPressEventHandler^ m_pKeyPressHandler;
//};
//
//#endif // _MANAGED
