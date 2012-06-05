#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyFileName_TextBox.hpp
//**
//**		Intermediate class between the property (prtyFileName) and 
//**	the control (TextBox).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_FILENAME_TEXTBOX_HPP
//#error prtyFileName_TextBox.hpp multiply included
//#endif
//#define PRTY_FILENAME_TEXTBOX_HPP
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
//#ifndef PRTY_TEXTBOXUIINFO_HPP
//#include "Core/prty/prtyTextBoxUIInfo.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//#ifndef IT_STRING_HPP
//#include "Core/it/itString.hpp"
//#endif
//#ifndef IT_STRINGUTIL_HPP
//#include "Core/it/itStringUtil.hpp"
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
//public ref class prtyFileName_TextBox : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyFileName_TextBox(prtyPropertyUIInfo* i_pUIInfo)
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
//					newvalue = pActualProperty->GetString();
//				else
//					if (newvalue != pActualProperty->GetString())
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
//			//	set the control values
//			this->UpdateControl(i_pUIInfo);
//
//			m_pOriginalValue = new itString("");
//			if (bSameValue)
//			{
//				m_pActualControl->Text = gcnew System::String(newvalue.c_str());
//				(*m_pOriginalValue) = itString(newvalue.c_str());
//			}
//
//			//	hook up events
//			m_pLeaveHandler = gcnew System::EventHandler(this, &prtyFileName_TextBox::textBox_ValueChanged);
//			m_pActualControl->Leave	+= m_pLeaveHandler;
//			m_pKeyPressHandler = gcnew System::Windows::Forms::KeyPressEventHandler(this, &prtyFileName_TextBox::textBox_KeyPress);
//			m_pActualControl->KeyPress	+= m_pKeyPressHandler;
//		}
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyFileName_TextBox()
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
//			const itString& newvalue = (static_cast<prtyFileName*>(i_pProperty))->GetValue();
//
//			m_bLocalChangeNoUpdate = true;
//			std::string newstring;
//			std::string currvalue;
//			tmaManagedStringUtils::ManagedStringToStdString(m_pActualControl->Text, currvalue);
//			newstring = itStringUtil::GetStdString(newvalue);
//
//			if ( strcmp(currvalue.c_str(), newstring.c_str()) != 0 )
//			{
//				m_pActualControl->Text = gcnew System::String( itStringUtil::GetStdString(newvalue).c_str() );
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
//			if ( strcmp(itStringUtil::GetStdString((*m_pOriginalValue)).c_str(), newvalue.c_str()) != 0)
//			{
//				const int num_properties = this->GetNumberOfProperties();
//				if (num_properties > 1)
//					undoUndoMgr::BeginMultipleOperationBlock();
//
//				for (int i=0; i < num_properties; ++i)
//				{
//					prtyFileName* pActualProperty = static_cast<prtyFileName*>(GetProperty(i));
//					pActualProperty->SetValue( itString(newvalue.c_str()), prtyProperty::eNewUndo );	// store UNDO also
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
//	itString* m_pOriginalValue;
//	System::EventHandler^ m_pLeaveHandler;
//	System::Windows::Forms::KeyPressEventHandler^ m_pKeyPressHandler;
//};
//
//#endif // _MANAGED
