#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyTrigger_Button.hpp
//**
//**		Intermediate class between the property (prtyTrigger) and 
//**	the control (Button).
//**
//**	StudioGPU
//**	Copyright(C) 2007 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_TRIGGER_BUTTON_HPP
//#error prtyTrigger_Button.hpp multiply included
//#endif
//#define PRTY_TRIGGER_BUTTON_HPP
//
//#ifndef PRTY_CALLBACKMGR_HPP
//#include "ToolUIManaged/prtym/prtyCallbackMgr.hpp"
//#endif
//#ifndef PRTY_CONTROL_HPP
//#include "ToolUIManaged/prtym/prtyControl.hpp"
//#endif
//#ifndef PRTY_TRIGGER_HPP
//#include "Core/prty/prtyTrigger.hpp"
//#endif
//#ifndef PRTY_BUTTONUIINFO_HPP
//#include "Core/prty/prtyButtonUIInfo.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
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
//public ref class prtyTrigger_Button : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyTrigger_Button(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != 0, "UI Info cannot be NULL");
//
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyTrigger* pActualProperty = static_cast<prtyTrigger*>(i_pUIInfo->GetProperty(i));
//				AddProperty(pActualProperty);
//				SetName(pActualProperty->GetPropertyName());
//
//				//	register the callback
//				prtyCallbackMgr::AddCallback( pActualProperty, this );
//			}
//
//			// create the actual control
//			m_pActualControl = prtyButtonControlBuffer::CreateControl();
//			//m_pActualControl = gcnew System::Windows::Forms::Button();
//			SetControl(m_pActualControl);
//
//			//	set the control values
//			this->UpdateControl(i_pUIInfo);
//
//			//	hook up events
//			//
//			m_pValueChangedHandler = gcnew System::EventHandler(this, &prtyTrigger_Button::button_ValueChanged);
//			m_pActualControl->Click	+= m_pValueChangedHandler;
//		};
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyTrigger_Button()
//		{
//			m_pActualControl->Click -= m_pValueChangedHandler;
//			prtyButtonControlBuffer::ReleaseControl(m_pActualControl);
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
//			m_bLocalChangeNoUpdate = true;
//			//if ( m_pActualControl->Checked != newvalue )
//			//{
//			//	m_pActualControl->Checked = newvalue;
//			//	//m_OriginalValue = newvalue;
//			//}
//			m_bLocalChangeNoUpdate = false;
//		}
//
//		//----------------------------------------------------------------------------
//		// Update control to match the changes in the UI Info
//		//----------------------------------------------------------------------------
//		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo) override
//		{
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//
//			prtyButtonUIInfo* pBUII = static_cast<prtyButtonUIInfo*>(i_pUIInfo);
//			m_pActualControl->Text = gcnew System::String(pBUII->m_ButtonText.c_str());
//		}
//
//	private:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		System::Void button_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
//		{
//			if (m_bLocalChangeNoUpdate) return;
//
//			m_bLocalChangeNoUpdate = true;
//			//if (m_OriginalValue != m_pActualControl->Checked)
//			{
//				const int num_properties = this->GetNumberOfProperties();
//				if (num_properties > 1)
//					undoUndoMgr::BeginMultipleOperationBlock();
//
//				for (int i=0; i < num_properties; ++i)
//				{
//					prtyTrigger* pActualProperty = static_cast<prtyTrigger*>(GetProperty(i));
//					pActualProperty->TriggerCallbacks();
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
//		void checkBox_KeyPress(System::Object ^sender, System::Windows::Forms::KeyPressEventArgs^ e)
//		{
//			if (e->KeyChar == (char)13)
//			{
//				button_ValueChanged( sender, e );
//			}
//		}
//
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		const char* GetControlName()
//		{
//			return "Button";
//		}
//
//public:
//	System::Windows::Forms::Button^ m_pActualControl;
//	System::EventHandler^ m_pValueChangedHandler;
//};
//
//#endif // _MANAGED
