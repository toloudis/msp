#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyVector3d_Vector3dEdit.hpp
//**
//**		Intermediate class between the property (prtyVector3d) and 
//**	the control (Vector3dEdit).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_VECTOR3D_VECTOR3DEDIT_HPP
//#error prtyVector3d_Vector3dEdit.hpp multiply included
//#endif
//#define PRTY_VECTOR3D_VECTOR3DEDIT_HPP
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
//#ifndef PRTY_VECTOR3D_HPP
//#include "Core/prty/prtyVector3d.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
////#ifndef DBG_LOG_HPP
////#include "Core/dbg/dbgLog.hpp"
////#endif
//#ifndef UNDO_UNDOMGR_HPP
//#include "Core/undo/undoUndoMgr.hpp"
//#endif
//
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//public ref class prtyVector3d_Vector3dEdit : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyVector3d_Vector3dEdit(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != 0, "UI Info cannot be NULL");
//
//			bool bSameValue = true;
//			maVector3d newvalue;
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyVector3d* pActualProperty = static_cast<prtyVector3d*>(i_pUIInfo->GetProperty(i));
//				AddProperty(pActualProperty);
//				SetName(pActualProperty->GetPropertyName());
//
//				//DBG_LOG3("V3d-V3E properties #%d [%x] (%s)", i, (&pActualProperty), pActualProperty->GetPropertyName().c_str() );
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
//			m_pActualControl = prtyVector3EditControlBuffer::CreateControl();
//			//m_pActualControl = gcnew TerawattManagedControls::Vector3Edit();
//			SetControl(m_pActualControl);
//
//			//	set the control values
//			this->UpdateControl(i_pUIInfo);
//
//			m_pOriginalValue = new maVector3d();
//			if (bSameValue)
//			{
//				//DBG_LOG4("Created control newvalue (%s) [%6.2f,%6.2f,%6.2f]", this->GetName()->c_str(), newvalue.GetX(), newvalue.GetY(), newvalue.GetZ());
//
//				m_pActualControl->ValueX = newvalue.GetX();
//				m_pActualControl->ValueY = newvalue.GetY();
//				m_pActualControl->ValueZ = newvalue.GetZ();
//				(*m_pOriginalValue) = newvalue;
//			}
//			//else
//			//{
//				//DBG_LOG4("Created control (%s) [%6.2f,%6.2f,%6.2f]", this->GetName()->c_str(), m_pActualControl->ValueX, m_pActualControl->ValueY, m_pActualControl->ValueZ);
//				//DBG_LOG4("                (%s) [%6.2f,%6.2f,%6.2f]", this->GetName()->c_str(), newvalue.GetX(), newvalue.GetY(), newvalue.GetZ());
//			//}
//
//			//	hook up the events
//			//
//			m_pLeaveChildHandler = gcnew System::EventHandler(this, &prtyVector3d_Vector3dEdit::vector3Edit_ValueChanged);
//			m_pActualControl->LeaveChild	+= m_pLeaveChildHandler;
//			m_pKeyPressHandler = gcnew System::Windows::Forms::KeyPressEventHandler(this, &prtyVector3d_Vector3dEdit::vector3Edit_KeyPress);
//			m_pActualControl->KeyPressChild	+= m_pKeyPressHandler;
//		}
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyVector3d_Vector3dEdit()
//		{
//			m_pActualControl->LeaveChild -= m_pLeaveChildHandler;
//			m_pActualControl->KeyPressChild -= m_pKeyPressHandler;
//			delete m_pOriginalValue;
//			prtyVector3EditControlBuffer::ReleaseControl(m_pActualControl);
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
//			const maVector3d& newvalue = (static_cast<prtyVector3d*>(i_pProperty))->GetValue();
//
//			//DBG_LOG4("property changed (%s) [%6.2f,%6.2f,%6.2f]", this->GetName()->c_str(), newvalue.GetX(), newvalue.GetY(), newvalue.GetZ());
//
//			m_bLocalChangeNoUpdate = true;
//			if (m_pActualControl->ValueX != newvalue.GetX())
//				m_pActualControl->ValueX = newvalue.GetX();
//			if (m_pActualControl->ValueY != newvalue.GetY())
//				m_pActualControl->ValueY = newvalue.GetY();
//			if (m_pActualControl->ValueZ != newvalue.GetZ())
//				m_pActualControl->ValueZ = newvalue.GetZ();
//			(*m_pOriginalValue) = newvalue;
//			m_bLocalChangeNoUpdate = false;
//		
//			//DBG_LOG1("V3d-V3E property changed (%s)", i_pProperty->GetPropertyName().c_str() );
//		}
//
//		//----------------------------------------------------------------------------
//		// Update control to match the changes in the UI Info
//		//----------------------------------------------------------------------------
//		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo) override
//		{
//			prtyVector3dEditUIInfo* pUIInfo = static_cast<prtyVector3dEditUIInfo*>(m_pUIInfo);
//			m_pActualControl->Precision = pUIInfo->GetDecimalPlaces();	// set before setting values so precision is correct
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//		}
//
//	private:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		System::Void vector3Edit_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
//		{
//			if (m_bLocalChangeNoUpdate) return;
//
//			m_bLocalChangeNoUpdate = true;
//			maVector3d newvalue( (float)m_pActualControl->ValueX, (float)m_pActualControl->ValueY, (float)m_pActualControl->ValueZ );
//
//			//DBG_LOG4("control value changed (%s) [%6.2f,%6.2f,%6.2f]", this->GetName()->c_str(), newvalue.GetX(), newvalue.GetY(), newvalue.GetZ());
//
//			//causes missed updates--if ((*m_pOriginalValue) != newvalue)
//			{
//				const int num_properties = this->GetNumberOfProperties();
//				if (num_properties > 1)
//					undoUndoMgr::BeginMultipleOperationBlock();
//
//				for (int i=0; i < num_properties; ++i)
//				{
//					prtyVector3d* pActualProperty = static_cast<prtyVector3d*>(GetProperty(i));
//					pActualProperty->SetValue( newvalue, prtyProperty::eNewUndo );	// store UNDO also
//				}
//
//				if (num_properties > 1)
//					undoUndoMgr::EndMultipleOperationBlock();
//
//				//DBG_LOG2("---%02d V3d-V3E control value changed (%s)", i, pActualProperty->GetPropertyName().c_str() );
//			}
//			m_bLocalChangeNoUpdate = false;
//		};
//
//		//----------------------------------------------------------------------------
//		// This event occurs after the KeyDown event and can be used to prevent
//		// characters from entering the control.
//		//----------------------------------------------------------------------------
//		void vector3Edit_KeyPress(System::Object ^sender, System::Windows::Forms::KeyPressEventArgs^ e)
//		{
//			if (e->KeyChar == (char)13)
//			{
//				vector3Edit_ValueChanged( sender, e );
//			}
//		}
//
//public:
//	TerawattManagedControls::Vector3Edit^ m_pActualControl;
//	maVector3d* m_pOriginalValue;
//	System::EventHandler^ m_pLeaveChildHandler;
//	System::Windows::Forms::KeyPressEventHandler^ m_pKeyPressHandler;
//};
//
//#endif // _MANAGED
