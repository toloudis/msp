#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyFloat_NumericUpDown.hpp
//**
//**		Intermediate class between the property (prtyFloat) and 
//**	the control (NumericUpDown).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_FLOAT_NUMERICUPDOWN_HPP
//#error prtyFloat_NumericUpDown.hpp multiply included
//#endif
//#define PRTY_FLOAT_NUMERICUPDOWN_HPP
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
//#ifndef PRTY_FLOAT_HPP
//#include "Core/prty/prtyFloat.hpp"
//#endif
//#ifndef PRTY_NUMERICUPDOWNUIINFO_HPP
//#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
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
//public ref class prtyFloat_NumericUpDown : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyFloat_NumericUpDown(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != 0, "UI Info cannot be NULL");
//
//			bool bSameValue = true;
//			float newvalue;
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyFloat* pActualProperty = static_cast<prtyFloat*>(i_pUIInfo->GetProperty(i));
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
//			m_pActualControl = prtyNumericUpDownControlBuffer::CreateControl();
//			//m_pActualControl = gcnew System::Windows::Forms::NumericUpDown();
//			SetControl(m_pActualControl);
//
//			this->UpdateControl(i_pUIInfo);
//
//			m_OriginalValue = 0.0f;
//			if (bSameValue)
//			{
//				m_pActualControl->Value = (System::Decimal) newvalue;
//				m_OriginalValue = newvalue;
//			}
//
//			//	hook up the events
//			//
//			m_pValueChangedHandler = gcnew System::EventHandler(this, &prtyFloat_NumericUpDown::control_ValueChanged);
//			m_pActualControl->ValueChanged	+= m_pValueChangedHandler;
//			m_pKeyPressHandler = gcnew System::Windows::Forms::KeyPressEventHandler(this, &prtyFloat_NumericUpDown::control_KeyPress);
//			m_pActualControl->KeyPress	+= m_pKeyPressHandler;
//		};
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyFloat_NumericUpDown()
//		{
//			m_pActualControl->ValueChanged -= m_pValueChangedHandler;
//			m_pActualControl->KeyPress -= m_pKeyPressHandler;
//			prtyNumericUpDownControlBuffer::ReleaseControl(m_pActualControl);
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
//			System::Decimal newvalue = (System::Decimal)(static_cast<prtyFloat*>(i_pProperty))->GetValue();
//
//			m_bLocalChangeNoUpdate = true;
//			if ( m_pActualControl->Value != newvalue )
//			{
//				m_pActualControl->Value = newvalue;
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
//			prtyNumericUpDownUIInfo* pUII = static_cast<prtyNumericUpDownUIInfo*>(i_pUIInfo);
//			m_pActualControl->Width = 64;
//			m_pActualControl->DecimalPlaces	= pUII->GetDecimalPlaces();	// set the places before the value to precision is correct
//			m_pActualControl->Increment		= (System::Decimal) pUII->GetIncrement();
//			if ( pUII->GetMinimum() == pUII->GetMaximum() )
//			{
//				m_pActualControl->Minimum = std::numeric_limits<__int32>::min();
//				m_pActualControl->Maximum = std::numeric_limits<__int32>::max();
//			}
//			else
//			{
//				m_pActualControl->Minimum = (System::Decimal) pUII->GetMinimum();
//				m_pActualControl->Maximum = (System::Decimal) pUII->GetMaximum();
//			}
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
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
//			//causes missed updates--if (m_OriginalValue != (float)m_pActualControl->Value)
//			{
//				const int num_properties = this->GetNumberOfProperties();
//				if (num_properties > 1)
//					undoUndoMgr::BeginMultipleOperationBlock();
//
//				for (int i=0; i < num_properties; ++i)
//				{
//					prtyFloat* pActualProperty = static_cast<prtyFloat*>(GetProperty(i));
//					pActualProperty->SetValue( (float)m_pActualControl->Value, prtyProperty::eNewUndo );	// store UNDO also
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
//				//control_ValueChanged( sender, e );
//				m_pActualControl->Select(0, m_pActualControl->Text->Length);
//			}
//		}
//
//public:
//	System::Windows::Forms::NumericUpDown^ m_pActualControl;
//	float m_OriginalValue;
//	System::EventHandler^ m_pValueChangedHandler;
//	System::Windows::Forms::KeyPressEventHandler^ m_pKeyPressHandler;
//};
//
//#endif // _MANAGED
