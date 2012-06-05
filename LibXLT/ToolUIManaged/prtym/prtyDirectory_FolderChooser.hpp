#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyDirectory_FolderChooser.hpp
//**
//**		Intermediate class between the property (prtyDirectory) and 
//**	the control (FolderChooser).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_DIRECTORY_FOLDERCHOOSER_HPP
//#error prtyDirectory_FolderChooser.hpp multiply included
//#endif
//#define PRTY_DIRECTORY_FOLDERCHOOSER_HPP
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
//#ifndef PRTY_DIRECTORY_HPP
//#include "Core/prty/prtyDirectory.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//#ifndef TMA_MANAGEDSTRINGUTILS_HPP
//#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
//#endif
//#ifndef UNDO_UNDOMGR_HPP
//#include "Core/undo/undoUndoMgr.hpp"
//#endif
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//public ref class prtyDirectory_FolderChooser : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyDirectory_FolderChooser::prtyDirectory_FolderChooser(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != 0, "UI Info cannot be NULL");
//
//			bool bSameValue = true;
//			fsLocator newvalue;
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyDirectory* pActualProperty = static_cast<prtyDirectory*>(i_pUIInfo->GetProperty(i));
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
//			m_pActualControl = prtyFolderChooserControlBuffer::CreateControl();
//			//m_pActualControl = gcnew TerawattManagedControls::FolderChooser();
//			SetControl(m_pActualControl);
//
//			//	set the control values
//			this->UpdateControl(i_pUIInfo);
//
//			m_pOriginalValue = new fsLocator();
//			if (bSameValue)
//			{
//				m_pActualControl->Directory = tmaManagedStringUtils::LocatorToManagedString(newvalue);
//				(*m_pOriginalValue) = newvalue;
//			}
//
//			//	hook up events
//			//
//			m_pLeaveChildHandler = gcnew System::EventHandler(this, &prtyDirectory_FolderChooser::Control_ValueChanged);
//			m_pActualControl->LeaveChild	+= m_pLeaveChildHandler;
//			m_pKeyPressHandler = gcnew System::Windows::Forms::KeyPressEventHandler(this, &prtyDirectory_FolderChooser::Control_KeyPress);
//			m_pActualControl->KeyPressChild	+= m_pKeyPressHandler;
//		};
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyDirectory_FolderChooser()
//		{
//			m_pActualControl->LeaveChild -= m_pLeaveChildHandler;
//			m_pActualControl->KeyPressChild -= m_pKeyPressHandler;
//			delete m_pOriginalValue;
//			prtyFolderChooserControlBuffer::ReleaseControl(m_pActualControl);
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
//			const fsLocator& newvalue = (static_cast<prtyDirectory*>(i_pProperty))->GetValue();
//
//			m_bLocalChangeNoUpdate = true;
//			if (m_pActualControl->Directory != tmaManagedStringUtils::LocatorToManagedString(newvalue))
//			{
//				//(*m_pOriginalValue) = newvalue;
//				m_pActualControl->Directory = tmaManagedStringUtils::LocatorToManagedString(newvalue);
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
//		System::Void Control_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
//		{
//			if (m_bLocalChangeNoUpdate) return;
//
//			m_bLocalChangeNoUpdate = true;
//			fsLocator newvalue;
//			tmaManagedStringUtils::ManagedStringToLocator(m_pActualControl->Directory, newvalue);
//
//			//causes missed updates--if (*m_pOriginalValue != newvalue)
//			{
//				const int num_properties = this->GetNumberOfProperties();
//				if (num_properties > 1)
//					undoUndoMgr::BeginMultipleOperationBlock();
//
//				for (int i=0; i < num_properties; ++i)
//				{
//					prtyDirectory* pActualProperty = static_cast<prtyDirectory*>(GetProperty(i));
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
//		void Control_KeyPress(System::Object ^sender, System::Windows::Forms::KeyPressEventArgs^ e)
//		{
//			if (e->KeyChar == (char)13)
//			{
//				Control_ValueChanged( sender, e );
//			}
//		}
//
//public:
//	TerawattManagedControls::FolderChooser^ m_pActualControl;
//	fsLocator* m_pOriginalValue;
//	System::EventHandler^ m_pLeaveChildHandler;
//	System::Windows::Forms::KeyPressEventHandler^ m_pKeyPressHandler;
//};
//
//#endif // _MANAGED
