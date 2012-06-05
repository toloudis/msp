#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyColor_ColorRGBAEdit.hpp
//**
//**		Intermediate class between the property (prtyColor) and 
//**	the control (ColorRGBAEdit).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_COLOR_COLORRGBAEDIT_HPP
//#error prtyColor_ColorRGBAEdit.hpp multiply included
//#endif
//#define PRTY_COLOR_COLORRGBAEDIT_HPP
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
//#ifndef PRTY_COLOR_HPP
//#include "Core/prty/prtyColor.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//#ifndef MA_FLOATRGBA_HPP
//#include "Core/ma/maFloatRGBA.hpp"
//#endif
//#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
//#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
//#endif
//#ifndef UNDO_UNDOMGR_HPP
//#include "Core/undo/undoUndoMgr.hpp"
//#endif
//
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//public ref class prtyColor_ColorRGBAEdit : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyColor_ColorRGBAEdit(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != 0, "UI Info cannot be NULL");
//
//			bool bSameValue = true;
//			maFloatRGBA newvalue;
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyColor* pActualProperty = static_cast<prtyColor*>(i_pUIInfo->GetProperty(i));
//				AddProperty(pActualProperty);
//				SetName(pActualProperty->GetPropertyName());
//
//				if (i==0)
//					newvalue = pActualProperty->GetValue();
//				else
//					if (!(newvalue == pActualProperty->GetValue()))
//						bSameValue = false;
//
//				//	register the callback
//				prtyCallbackMgr::AddCallback( pActualProperty, this );
//			}
//
//			// create the actual control
//			m_pActualControl = prtyColorRGBAControlBuffer::CreateControl();
//			//m_pActualControl = gcnew TerawattManagedControls::ColorRGBAEdit();
//			SetControl(m_pActualControl);
//
//			//	set the control values
//			this->UpdateControl(i_pUIInfo);
//			if (i_pUIInfo->GetNumberOfProperties() > 1)
//				m_pActualControl->DialogText = gcnew System::String("Multiple Objects");
//			else
//			{
//				// use the first property in the list to set the dialog text for color picker.
//				prtyColor* pActualProperty = static_cast<prtyColor*>(i_pUIInfo->GetProperty(0));
//				m_pActualControl->DialogText = gcnew System::String(pActualProperty->GetPropertyName().c_str());
//			}
//
//			m_pOriginalValue = new maFloatRGBA();
//			if (bSameValue)
//			{
//				m_pActualControl->Color = tmaManagedConversionUtil::SetColorRGBA(newvalue);
//				(*m_pOriginalValue) = newvalue;
//			}
//
//			//	hook up events
//			//
//			m_pLeaveChildHandler = gcnew System::EventHandler(this, &prtyColor_ColorRGBAEdit::control_ValueChanged);
//			m_pActualControl->LeaveChild	+= m_pLeaveChildHandler;
//			m_pKeyPressHandler = gcnew System::Windows::Forms::KeyPressEventHandler(this, &prtyColor_ColorRGBAEdit::control_KeyPress);
//			m_pActualControl->KeyPressChild	+= m_pKeyPressHandler;
//		}
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyColor_ColorRGBAEdit()
//		{
//			m_pActualControl->LeaveChild -= m_pLeaveChildHandler;
//			m_pActualControl->KeyPressChild -= m_pKeyPressHandler;
//			delete m_pOriginalValue;
//			prtyColorRGBAControlBuffer::ReleaseControl(m_pActualControl);
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
//			const maFloatRGBA newvalue = (static_cast<prtyColor*>(i_pProperty))->GetValue();
//
//			m_bLocalChangeNoUpdate = true;
//			if ( !(tmaManagedConversionUtil::ConvertColorRGBA(m_pActualControl->Color) == newvalue))
//			{
//				//(*m_pOriginalValue) = newvalue;
//				m_pActualControl->Color = tmaManagedConversionUtil::SetColorRGBA(newvalue);
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
//private:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		System::Void control_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
//		{
//			if (m_bLocalChangeNoUpdate) return;
//
//			m_bLocalChangeNoUpdate = true;
//
//			maFloatRGBA newvalue = tmaManagedConversionUtil::ConvertColorRGBA(m_pActualControl->Color);
//			//causes missed updates--if (!((*m_pOriginalValue) == newvalue))
//			{
//				const int num_properties = this->GetNumberOfProperties();
//				if (num_properties > 1)
//					undoUndoMgr::BeginMultipleOperationBlock();
//
//				for (int i=0; i < num_properties; ++i)
//				{
//					prtyColor* pActualProperty = static_cast<prtyColor*>(GetProperty(i));
//					pActualProperty->SetValue( newvalue, prtyProperty::eNewUndo );
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
//	TerawattManagedControls::ColorRGBAEdit^ m_pActualControl;
//	maFloatRGBA* m_pOriginalValue;
//	System::EventHandler^ m_pLeaveChildHandler;
//	System::Windows::Forms::KeyPressEventHandler^ m_pKeyPressHandler;
//};
//
//#endif // _MANAGED
