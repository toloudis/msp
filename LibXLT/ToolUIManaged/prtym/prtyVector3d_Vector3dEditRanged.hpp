#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyVector3d_Vector3dEditRanged.hpp
//**
//**		Intermediate class between the property (prtyVector3d) and 
//**	the control (Vector3dEditRanged).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_VECTOR3D_VECTOR3DEDITRANGED_HPP
//#error prtyVector3d_Vector3dEditRanged.hpp multiply included
//#endif
//#define PRTY_VECTOR3D_VECTOR3DEDITRANGED_HPP
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
//#ifndef PRTY_VECTOR3DEDITRANGEDUIINFO_HPP
//#include "Core/prty/prtyVector3dEditRangedUIInfo.hpp"
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
//public ref class prtyVector3d_Vector3dEditRanged : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyVector3d_Vector3dEditRanged(prtyPropertyUIInfo* i_pUIInfo)
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
//			m_pActualControl = prtyVector3RangedControlBuffer::CreateControl();
//			//m_pActualControl = gcnew TerawattManagedControls::Vector3EditRanged();
//			SetControl(m_pActualControl);
//
//			//	set the control values
//			this->UpdateControl(i_pUIInfo);
//
//			m_pOriginalValue = new maVector3d();
//			if (bSameValue)
//			{
//				m_pActualControl->ValueX = newvalue.GetX();
//				m_pActualControl->ValueY = newvalue.GetY();
//				m_pActualControl->ValueZ = newvalue.GetZ();
//				(*m_pOriginalValue) = newvalue;
//			}
//
//			//	hook up events
//			//
//			//m_pActualControl->LeaveChild	+= gcnew System::EventHandler(this, &prtyVector3d_Vector3dEditRanged::vector3Edit_ValueChanged);
//			//m_pActualControl->KeyPressChild	+= gcnew System::Windows::Forms::KeyPressEventHandler(this:, &prtyVector3d_Vector3dEditRanged::vector3Edit_KeyPress);
//
//			m_pValueChangedHandler = gcnew System::EventHandler(this, &prtyVector3d_Vector3dEditRanged::vector3Edit_ValueChanged);
//			m_pActualControl->ValueChanged	+= m_pValueChangedHandler;
//		}
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyVector3d_Vector3dEditRanged()
//		{
//			m_pActualControl->ValueChanged -= m_pValueChangedHandler;
//			delete m_pOriginalValue;
//			prtyVector3RangedControlBuffer::ReleaseControl(m_pActualControl);
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
//			m_bLocalChangeNoUpdate = true;
//			if (m_pActualControl->ValueX != newvalue.GetX())
//				m_pActualControl->ValueX = newvalue.GetX();
//			if (m_pActualControl->ValueY != newvalue.GetY())
//				m_pActualControl->ValueY = newvalue.GetY();
//			if (m_pActualControl->ValueZ != newvalue.GetZ())
//				m_pActualControl->ValueZ = newvalue.GetZ();
//			(*m_pOriginalValue) = newvalue;
//			m_bLocalChangeNoUpdate = false;
//		}
//
//		//----------------------------------------------------------------------------
//		// Update control to match the changes in the UI Info
//		//----------------------------------------------------------------------------
//		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo) override
//		{
//			prtyVector3dEditRangedUIInfo* pUIInfo = static_cast<prtyVector3dEditRangedUIInfo*>(m_pUIInfo);
//			m_pActualControl->Precision = pUIInfo->GetDecimalPlaces();	// set before setting values so precision is correct
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//
//			m_pActualControl->Precision = pUIInfo->GetNumTicks();
//			m_pActualControl->Minimum = pUIInfo->GetMinimum();
//			m_pActualControl->Maximum = pUIInfo->GetMaximum();
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
//	TerawattManagedControls::Vector3EditRanged^ m_pActualControl;
//	maVector3d* m_pOriginalValue;
//	System::EventHandler^ m_pValueChangedHandler;
//};
//
//#endif // _MANAGED
