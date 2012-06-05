#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyRotation_Vector3dEditUpDown.hpp
//**
//**		Intermediate class between the property (prtyRotation) and 
//**	the control (RotationEditUpDown).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_ROTATION_VECTOR3DEDITUPDOWN_HPP
//#error prtyRotation_Vector3dEditUpDown.hpp multiply included
//#endif
//#define PRTY_ROTATION_VECTOR3DEDITUPDOWN_HPP
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
//#ifndef PRTY_ROTATION_HPP
//#include "Core/prty/prtyRotation.hpp"
//#endif
//#ifndef PRTY_VECTOR3DEDITUPDOWNUIINFO_HPP
//#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
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
//public ref class prtyRotation_Vector3dEditUpDown : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyRotation_Vector3dEditUpDown(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != 0, "UI Info cannot be NULL");
//
//			bool bSameValue = true;
//			float nx = 0, ny = 0, nz = 0;
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyRotation* pActualProperty = static_cast<prtyRotation*>(i_pUIInfo->GetProperty(i));
//				AddProperty(pActualProperty);
//				SetName(pActualProperty->GetPropertyName());
//
//				if (i==0)
//					pActualProperty->GetEuler(nx, ny, nz);
//				else
//				{
//					float x = 0, y = 0, z = 0;
//					pActualProperty->GetEuler(x, y, z);
//					if (nx != x || ny != y || nz != z)
//						bSameValue = false;
//				}
//
//				//	register the callback
//				prtyCallbackMgr::AddCallback( pActualProperty, this );
//			}
//
//			// create the actual control
//			m_pActualControl = prtyVector3UpDownControlBuffer::CreateControl();
//			//m_pActualControl = gcnew TerawattManagedControls::Vector3EditUpDown();
//			SetControl(m_pActualControl);
//
//			//	set the control values
//			this->UpdateControl(i_pUIInfo);
//
//			m_pOriginalValue = new maVector3d(0,0,0);
//			if (bSameValue)
//			{
//				// HACK [rjk] to handle when GetEuler returns a NaN
//				if (nx != nx) {nx = 0; DBG_ERROR0("x = NaN!!!");}
//				if (ny != ny) {ny = 0; DBG_ERROR0("y = NaN!!!");}
//				if (nz != nz) {nz = 0; DBG_ERROR0("z = NaN!!!");}
//
//				// Convert to degrees for display
//				nx *= maConstants::c_fRadToAngle;
//				ny *= maConstants::c_fRadToAngle;
//				nz *= maConstants::c_fRadToAngle;
//
//				m_bLocalChangeNoUpdate = true;
//
//				if (m_pActualControl->ValueX != (System::Decimal) nx)
//					m_pActualControl->ValueX = (System::Decimal) nx;
//				if (m_pActualControl->ValueY != (System::Decimal) ny)
//					m_pActualControl->ValueY = (System::Decimal) ny;
//				if (m_pActualControl->ValueZ != (System::Decimal) nz)
//					m_pActualControl->ValueZ = (System::Decimal) nz;
//
//				m_pOriginalValue->Set(nx,ny,nz);
//
//				m_bLocalChangeNoUpdate = false;
//			}
//
//			//	hook up events
//			//
//			//m_pActualControl->LeaveChild	+= gcnew System::EventHandler(this, &prtyRotation_Vector3dEditUpDown::vector3Edit_ValueChanged);
//			//m_pActualControl->KeyPressChild	+= gcnew System::Windows::Forms::KeyPressEventHandler(this, &prtyRotation_Vector3dEditUpDown::vector3Edit_KeyPress);
//			m_pValueChangedHandler = gcnew System::EventHandler(this, &prtyRotation_Vector3dEditUpDown::vector3Edit_ValueChanged);
//			m_pActualControl->ValueChanged	+= m_pValueChangedHandler;
//		}
//
//		//----------------------------------------------------------------------------
//		// Update control to match the changes in the UI Info
//		//----------------------------------------------------------------------------
//		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo) override
//		{
//			prtyVector3dEditUpDownUIInfo* pUIInfo = static_cast<prtyVector3dEditUpDownUIInfo*>(m_pUIInfo);
//			m_pActualControl->Precision = pUIInfo->GetDecimalPlaces();	// set before value so precision is correct
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//
//			m_pActualControl->IncrementX = (System::Decimal) pUIInfo->GetIncrement().GetX();
//			m_pActualControl->IncrementY = (System::Decimal) pUIInfo->GetIncrement().GetY();
//			m_pActualControl->IncrementZ = (System::Decimal) pUIInfo->GetIncrement().GetZ();
//
//			m_pActualControl->MinimumX = (System::Decimal) pUIInfo->GetMinimum().GetX();
//			m_pActualControl->MinimumY = (System::Decimal) pUIInfo->GetMinimum().GetY();
//			m_pActualControl->MinimumZ = (System::Decimal) pUIInfo->GetMinimum().GetZ();
//
//			m_pActualControl->MaximumX = (System::Decimal) pUIInfo->GetMaximum().GetX();
//			m_pActualControl->MaximumY = (System::Decimal) pUIInfo->GetMaximum().GetY();
//			m_pActualControl->MaximumZ = (System::Decimal) pUIInfo->GetMaximum().GetZ();
//
//		}
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyRotation_Vector3dEditUpDown()
//		{
//			m_pActualControl->ValueChanged -= m_pValueChangedHandler;
//			delete m_pOriginalValue;
//			prtyVector3UpDownControlBuffer::ReleaseControl(m_pActualControl);
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
//			float x = 0, y = 0, z = 0;
//			(static_cast<prtyRotation*>(i_pProperty))->GetEuler(x,y,z);
//
//			// HACK [rjk] to handle when GetEuler returns a NaN
//			if (x != x) {x = 0; DBG_ERROR0("x = NaN!!!");}
//			if (y != y) {y = 0; DBG_ERROR0("y = NaN!!!");}
//			if (z != z) {z = 0; DBG_ERROR0("z = NaN!!!");}
//
//			// Convert to degrees for display
//			x *= maConstants::c_fRadToAngle;
//			y *= maConstants::c_fRadToAngle;
//			z *= maConstants::c_fRadToAngle;
//
//			m_bLocalChangeNoUpdate = true;
//			if (m_pActualControl->ValueX != (System::Decimal) x)
//				m_pActualControl->ValueX = (System::Decimal) x;
//			if (m_pActualControl->ValueY != (System::Decimal) y)
//				m_pActualControl->ValueY = (System::Decimal) y;
//			if (m_pActualControl->ValueZ != (System::Decimal) z)
//				m_pActualControl->ValueZ = (System::Decimal) z;
//			//(*m_pOriginalValue) = newvalue;
//			m_bLocalChangeNoUpdate = false;
//		};
//
//	private:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		System::Void vector3Edit_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
//		{
//			if (m_bLocalChangeNoUpdate) return;
//
//			// Convert from euler angles in degrees back to radians
//			m_bLocalChangeNoUpdate = true;
//			float x = (float)m_pActualControl->ValueX * maConstants::c_fAngleToRad; 
//			float y = (float)m_pActualControl->ValueY * maConstants::c_fAngleToRad; 
//			float z = (float)m_pActualControl->ValueZ * maConstants::c_fAngleToRad;
//
//			//causes missed updates--if ( (*m_pOriginalValue) != newvalue)
//			{
//				const int num_properties = this->GetNumberOfProperties();
//				if (num_properties > 1)
//					undoUndoMgr::BeginMultipleOperationBlock();
//
//				for (int i=0; i < num_properties; ++i)
//				{
//					prtyRotation* pActualProperty = static_cast<prtyRotation*>(GetProperty(i));
//					pActualProperty->SetEuler( x, y, z, prtyProperty::eNewUndo );	// store UNDO also
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
//	TerawattManagedControls::Vector3EditUpDown^ m_pActualControl;
//	maVector3d* m_pOriginalValue;
//	System::EventHandler^ m_pValueChangedHandler;
//};
//
//#endif // _MANAGED
