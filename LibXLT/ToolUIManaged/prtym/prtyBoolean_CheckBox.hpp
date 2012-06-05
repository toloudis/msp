#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyBoolean_CheckBox.hpp
//**
//**		Intermediate class between the property (prtyBoolean) and 
//**	the control (CheckBox).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_BOOLEAN_CHECKBOX_HPP
//#error prtyBoolean_CheckBox.hpp multiply included
//#endif
//#define PRTY_BOOLEAN_CHECKBOX_HPP
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
//#ifndef PRTY_BOOLEAN_HPP
//#include "Core/prty/prtyBoolean.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//#ifndef UNDO_UNDOMGR_HPP
//#include "Core/undo/undoUndoMgr.hpp"
//#endif
//
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//public ref class prtyBoolean_CheckBox : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyBoolean_CheckBox(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != 0, "UI Info cannot be NULL");
//
//			bool bSameValue = true;
//			bool newvalue;
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyBoolean* pActualProperty = static_cast<prtyBoolean*>(i_pUIInfo->GetProperty(i));
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
//			m_pActualControl = prtyCheckBoxControlBuffer::CreateControl();
//			//m_pActualControl = gcnew System::Windows::Forms::CheckBox();
//			SetControl(m_pActualControl);
//
//			//	set the control values
//			this->UpdateControl(i_pUIInfo);
//
//			m_OriginalValue = false;
//			if (bSameValue)
//			{
//				m_pActualControl->Checked = newvalue;
//				m_OriginalValue = newvalue;
//			}
//
//			//	hook up events
//			//
//			m_pValueChangedHandler = gcnew System::EventHandler(this, &prtyBoolean_CheckBox::checkBox_ValueChanged);
//			m_pActualControl->CheckedChanged	+= m_pValueChangedHandler;
//		};
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyBoolean_CheckBox()
//		{
//			m_pActualControl->CheckedChanged -= m_pValueChangedHandler;
//			prtyCheckBoxControlBuffer::ReleaseControl(m_pActualControl);
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
//			const bool newvalue = (static_cast<prtyBoolean*>(i_pProperty))->GetValue();
//
//			m_bLocalChangeNoUpdate = true;
//			if ( m_pActualControl->Checked != newvalue )
//			{
//				m_pActualControl->Checked = newvalue;
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
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//		}
//
//	private:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		System::Void checkBox_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
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
//					prtyBoolean* pActualProperty = static_cast<prtyBoolean*>(GetProperty(i));
//					pActualProperty->SetValue( m_pActualControl->Checked, prtyProperty::eNewUndo );	// store UNDO also
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
//				checkBox_ValueChanged( sender, e );
//			}
//		}
//
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		const char* GetControlName()
//		{
//			return "CheckBox";
//		}
//
//public:
//	System::Windows::Forms::CheckBox^ m_pActualControl;
//	bool m_OriginalValue;
//	System::EventHandler^ m_pValueChangedHandler;
//};
//
//#endif // _MANAGED
