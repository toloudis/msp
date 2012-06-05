#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyRotation_Vector3dEditRanged.hpp
//**
//**		Intermediate class between the property (prtyRotation) and 
//**	the control (RotationEditRanged).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_ROTATION_VECTOR3DEDITRANGED_HPP
//#error prtyRotation_Vector3dEditRanged.hpp multiply included
//#endif
//#define PRTY_ROTATION_VECTOR3DEDITRANGED_HPP
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
//public ref class prtyRotation_Vector3dEditRanged : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyRotation_Vector3dEditRanged(prtyPropertyUIInfo* i_pUIInfo)
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
//			m_pActualControl = prtyVector3RangedControlBuffer::CreateControl();
//			//m_pActualControl = gcnew TerawattManagedControls::Vector3EditRanged();
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
//				if (m_pActualControl->ValueX != nx)
//					m_pActualControl->ValueX = nx;
//				if (m_pActualControl->ValueY != ny)
//					m_pActualControl->ValueY = ny;
//				if (m_pActualControl->ValueZ != nz)
//					m_pActualControl->ValueZ = nz;
//			
//				m_pOriginalValue->Set(nx,ny,nz);
//
//				m_bLocalChangeNoUpdate = false;
//			}
//
//			//	hook up events
//			//
//			//m_pActualControl->LeaveChild	+= gcnew System::EventHandler(this, &prtyRotation_Vector3dEditRanged::vector3Edit_ValueChanged);
//			//m_pActualControl->KeyPressChild	+= gcnew System::Windows::Forms::KeyPressEventHandler(this, &prtyRotation_Vector3dEditRanged::vector3Edit_KeyPress);
//
//			m_pValueChangedHandler = gcnew System::EventHandler(this, &prtyRotation_Vector3dEditRanged::vector3Edit_ValueChanged);
//			m_pActualControl->ValueChanged	+= m_pValueChangedHandler;
//
//		}
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyRotation_Vector3dEditRanged()
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
//			if (m_pActualControl->ValueX != x)
//				m_pActualControl->ValueX = x;
//			if (m_pActualControl->ValueY != y)
//				m_pActualControl->ValueY = y;
//			if (m_pActualControl->ValueZ != z)
//				m_pActualControl->ValueZ = z;
//			//(*m_pOriginalValue) = newvalue;
//			m_bLocalChangeNoUpdate = false;
//		}
//
//		//----------------------------------------------------------------------------
//		// Update control to match the changes in the UI Info
//		//----------------------------------------------------------------------------
//		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo) override
//		{
//			prtyVector3dEditRangedUIInfo* pUIInfo = static_cast<prtyVector3dEditRangedUIInfo*>(m_pUIInfo);
//			m_pActualControl->Precision = pUIInfo->GetDecimalPlaces();	// set before value so precision is correct
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
//	TerawattManagedControls::Vector3EditRanged^ m_pActualControl;
//	maVector3d* m_pOriginalValue;
//	System::EventHandler^ m_pValueChangedHandler;
//};
//
//#endif // _MANAGED
