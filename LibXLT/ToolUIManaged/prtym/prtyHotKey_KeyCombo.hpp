#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyHotKey_KeyCombo.hpp
//**
//**		Intermediate class between the property (prtyHotKey) and 
//**	the control (KeyCombo).
//**
//**	StudioGPU
//**	Copyright(C) 2007 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_HOTKEY_KEYCOMBO_HPP
//#error prtyHotKey_KeyCombo.hpp multiply included
//#endif
//#define PRTY_HOTKEY_KEYCOMBO_HPP
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
//#ifndef PRTY_HOTKEY_HPP
//#include "Core/prty/prtyHotKey.hpp"
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
//
//#include <string>
//
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//public ref class prtyHotKey_KeyCombo : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyHotKey_KeyCombo(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != 0, "UI Info cannot be NULL");
//
//			bool bSameValue = true;
//			std::string newvalue;
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyHotKey* pActualProperty = static_cast<prtyHotKey*>(i_pUIInfo->GetProperty(i));
//				DBG_ASSERT0(pActualProperty != 0, "Invalid property type");
//				AddProperty(pActualProperty);
//				SetName(pActualProperty->GetPropertyName());
//
//				if (i==0)
//				{
//					newvalue = pActualProperty->GetValue();
//				}
//				else
//				{
//					if (!(newvalue == pActualProperty->GetValue()))
//						bSameValue = false;
//				}
//
//				//	register the callback
//				prtyCallbackMgr::AddCallback( pActualProperty, this );
//			}
//
//			// create the actual control
//			m_pActualControl = prtyKeyComboControlBuffer::CreateControl();
//			//m_pActualControl = gcnew TerawattManagedControls::KeyCombo();
//			SetControl(m_pActualControl);
//
//			//	set the control values
//			//
//			this->UpdateControl(i_pUIInfo);
//
//			m_pOriginalValue = new std::string();
//			if (bSameValue)
//			{
//				m_pActualControl->KeyComboString = gcnew System::String(newvalue.c_str());
//				(*m_pOriginalValue) = newvalue;
//			}
//
//			//DBG_LOG2("prtyHotKey_KeyCombo constructor nv(%s) ov(%s)", newvalue.GetString().c_str(), (*m_pOriginalValue).GetString().c_str() );
//
//			//	hook up the events
//			//
//			m_pKeyHandler = gcnew System::Windows::Forms::KeyEventHandler(this, &prtyHotKey_KeyCombo::KeyCombo_ValueChanged);
//			m_pActualControl->KeyChanged += m_pKeyHandler;
//		}
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyHotKey_KeyCombo()
//		{
//			delete m_pOriginalValue;
//
//			m_pActualControl->KeyChanged -= m_pKeyHandler;
//			prtyKeyComboControlBuffer::ReleaseControl(m_pActualControl);
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
//			const std::string& newvalue = (static_cast<prtyHotKey*>(i_pProperty))->GetValue();
//
//			m_bLocalChangeNoUpdate = true;
//			std::string currvalue;
//			tmaManagedStringUtils::ManagedStringToStdString(m_pActualControl->KeyComboString, currvalue);
//
//			if ( strcmp(currvalue.c_str(), newvalue.c_str()) != 0 )
//			{
//				m_pActualControl->KeyComboString = gcnew System::String(newvalue.c_str());
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
//			m_pActualControl->Width = 140;
//		}
//
//	private:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		System::Void KeyCombo_ValueChanged(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
//		{
//			if (m_bLocalChangeNoUpdate) return;
//
//			m_bLocalChangeNoUpdate = true;
//			std::string newvalue;
//			tmaManagedStringUtils::ManagedStringToStdString(m_pActualControl->KeyComboString, newvalue);
//
//			//	if the value has changed then update the properties
//			//
//			if (strcmp((*m_pOriginalValue).c_str(), newvalue.c_str()) != 0)
//			{
//				const int num_properties = this->GetNumberOfProperties();
//				if (num_properties > 1)
//					undoUndoMgr::BeginMultipleOperationBlock();
//
//				for (int i=0; i < num_properties; ++i)
//				{
//					prtyHotKey* pActualProperty = static_cast<prtyHotKey*>(GetProperty(i));
//					pActualProperty->SetValue( newvalue, prtyProperty::eNewUndo );	// store UNDO also
//				}
//
//				if (num_properties > 1)
//					undoUndoMgr::EndMultipleOperationBlock();
//			}
//			m_bLocalChangeNoUpdate = false;
//		};
//
//public:
//	TerawattManagedControls::KeyCombo^ m_pActualControl;
//	std::string* m_pOriginalValue;
//	System::Windows::Forms::KeyEventHandler^ m_pKeyHandler;
//};
//
//#endif // _MANAGED
