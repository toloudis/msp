#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyAngle_RangedFloat.hpp
//**
//**		Intermediate class between the property (prtyAngle) and 
//**	the control (RangedFloat).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_ANGLE_RANGEDFLOAT_HPP
//#error prtyAngle_RangedFloat.hpp multiply included
//#endif
//#define PRTY_ANGLE_RANGEDFLOAT_HPP
//
//#ifndef PRTY_ANGLE_HPP
//#include "Core/prty/prtyAngle.hpp"
//#endif
//#ifndef PRTY_CALLBACKMGR_HPP
//#include "ToolUIManaged/prtym/prtyCallbackMgr.hpp"
//#endif
//#ifndef PRTY_CONTROL_HPP
//#include "ToolUIManaged/prtym/prtyControl.hpp"
//#endif
//#ifndef PRTY_CONTROLBUFFER_HPP
//#include "ToolUIManaged/prtym/prtyControlBuffer.hpp"
//#endif
//#ifndef PRTY_RANGEDFLOATUIINFO_HPP
//#include "Core/prty/prtyRangedFloatUIInfo.hpp"
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
//public ref class prtyAngle_RangedFloat : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyAngle_RangedFloat(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != 0, "UI Info cannot be NULL");
//
//			bool bSameValue = true;
//			float newvalue;
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyAngle* pActualProperty = static_cast<prtyAngle*>(i_pUIInfo->GetProperty(i));
//				AddProperty(pActualProperty);
//				SetName(pActualProperty->GetPropertyName());
//
//				if (i==0)
//					newvalue = pActualProperty->GetValue().GetDegrees();
//				else
//					if (newvalue != pActualProperty->GetValue().GetDegrees())
//						bSameValue = false;
//
//				//	register the callback
//				prtyCallbackMgr::AddCallback( pActualProperty, this );
//			}
//
//			// create the actual control
//			m_pActualControl = prtyRangedFloatControlBuffer::CreateControl();
//			//m_pActualControl = gcnew TerawattManagedControls::RangedFloat();
//			SetControl(m_pActualControl);
//
//			//	set the control parameters
//			this->UpdateControl(i_pUIInfo);
//
//			m_OriginalValue = 0.0f;
//			if (bSameValue)
//			{
//				m_OriginalValue = newvalue;
//				m_pActualControl->Value = newvalue;
//			}
//
//			//	hook up events
//			//
//			m_pLeaveChildHandler = gcnew System::EventHandler(this, &prtyAngle_RangedFloat::control_ValueChanged);
//			m_pActualControl->LeaveChild	+= m_pLeaveChildHandler;
//			m_pKeyPressHandler = gcnew System::Windows::Forms::KeyPressEventHandler(this, &prtyAngle_RangedFloat::control_KeyPress);
//			m_pActualControl->KeyPressChild	+= m_pKeyPressHandler;
//		}
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyAngle_RangedFloat()
//		{
//			m_pActualControl->LeaveChild -= m_pLeaveChildHandler;
//			m_pActualControl->KeyPressChild -= m_pKeyPressHandler;
//			prtyRangedFloatControlBuffer::ReleaseControl(m_pActualControl);
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
//			const float newvalue = (static_cast<prtyAngle*>(i_pProperty))->GetValue().GetDegrees();
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
//
//		//----------------------------------------------------------------------------
//		// Update control to match the changes in the UI Info
//		//----------------------------------------------------------------------------
//		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo) override
//		{
//			//	set the control parameters
//			prtyRangedFloatUIInfo* pRFUII = static_cast<prtyRangedFloatUIInfo*>(i_pUIInfo);
//			m_pActualControl->Precision = pRFUII->GetDecimalPlaces();
//			m_pActualControl->NumTicks = pRFUII->GetNumTicks();
//			m_pActualControl->Exponent = pRFUII->GetExponent();
//			if ( pRFUII->GetMinimum() == pRFUII->GetMaximum() )
//			{
//				m_pActualControl->Minimum = std::numeric_limits<short>::min();
//				m_pActualControl->Maximum = std::numeric_limits<short>::max();
//			}
//			else
//			{
//				m_pActualControl->Minimum = pRFUII->GetMinimum();
//				m_pActualControl->Maximum = pRFUII->GetMaximum();
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
//					prtyAngle* pActualProperty = static_cast<prtyAngle*>(GetProperty(i));
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
//				control_ValueChanged( sender, e );
//			}
//		}
//
//public:
//	TerawattManagedControls::RangedFloat^ m_pActualControl;
//	float m_OriginalValue;
//	System::EventHandler^ m_pLeaveChildHandler;
//	System::Windows::Forms::KeyPressEventHandler^ m_pKeyPressHandler;
//};
//
//#endif // _MANAGED
