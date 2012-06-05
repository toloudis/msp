#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyFileName_FileChooser.hpp
//**
//**		Intermediate class between the property (prtyFileName) and 
//**	the control (FileChooser).
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_FILENAME_FILECHOOSER_HPP
//#error prtyFileName_FileChooser.hpp multiply included
//#endif
//#define PRTY_FILENAME_FILECHOOSER_HPP
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
//#ifndef PRTY_FILECHOOSERUIINFO_HPP
//#include "Core/prty/prtyFileChooserUIInfo.hpp"
//#endif
//#ifndef PRTY_FILENAME_HPP
//#include "Core/prty/prtyFileName.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//#ifndef DBG_LOG_HPP
//#include "Core/dbg/dbgLog.hpp"
//#endif
//#ifndef FS_FILEUTIL_HPP
//#include "Core/fs/fsFileUtil.hpp"
//#endif
//#ifndef GF_FILETRANSLATIONMGR_HPP
//#include "Core/gf/gfFileTranslationMgr.hpp"
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
//public ref class prtyFileName_FileChooser : public prtyControl
//{
//	public:
//		//----------------------------------------------------------------------------
//		//----------------------------------------------------------------------------
//		prtyFileName_FileChooser(prtyPropertyUIInfo* i_pUIInfo)
//		:	prtyControl(i_pUIInfo)
//		{
//			DBG_ASSERT0( i_pUIInfo != 0, "UI Info cannot be NULL");
//
//			bool bSameValue = true;
//			itString newvalue;
//			for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
//			{
//				prtyFileName* pActualProperty = static_cast<prtyFileName*>(i_pUIInfo->GetProperty(i));
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
//			m_pActualControl = prtyFileChooserControlBuffer::CreateControl();
//			//m_pActualControl = gcnew TerawattManagedControls::FileChooser();
//			SetControl(m_pActualControl);
//
//			//	set the control values
//			//
//			this->UpdateControl(i_pUIInfo);
//
//			m_pOriginalValue = new itString("");
//			if (bSameValue)
//			{
//				prtyFileChooserUIInfo* pUII = static_cast<prtyFileChooserUIInfo*>(i_pUIInfo);
//				fsLocator dirvalue = pUII->GetInitialDirectory();
//				fsLocator filepath(dirvalue);
//				gfFileTranslationMgr::ExpandLocator(filepath);		// get the full path
//				if (newvalue.GetLength() > 0)
//				{
//					filepath.Push( newvalue );
//				}
//				//std::string oldpath;
//				//tmaManagedStringUtils::ManagedStringToStdString( m_pActualControl->Fullpath, oldpath );
//				//DBG_LOG1("OLD control actual path (%s)", oldpath.c_str());
//				System::String^ pStr = tmaManagedStringUtils::LocatorToManagedString(filepath);
//				m_pActualControl->Fullpath = System::String::Copy(pStr);
//				//m_pActualControl->Fullpath = "C:\\Windows";
//
//				(*m_pOriginalValue) = newvalue;
//
//				// DEBUG ONLY
//				//std::string path, path2;
//				//fsFileUtil::LocatorToANSIFilename(filepath, path);
//				//DBG_LOG1("creating property FileName-FileChooser (%s)", path.c_str());
//				//tmaManagedStringUtils::ManagedStringToStdString( m_pActualControl->Fullpath, path2 );
//				//DBG_LOG1("control actual path (%s)", path2.c_str());
//			}
//
//			//	hook up the events
//			//
//			m_pLeaveChildHandler = gcnew System::EventHandler(this, &prtyFileName_FileChooser::Control_ValueChanged);
//			m_pActualControl->LeaveChild	+= m_pLeaveChildHandler;
//			m_pKeyPressHandler = gcnew System::Windows::Forms::KeyPressEventHandler(this, &prtyFileName_FileChooser::Control_KeyPress);
//			m_pActualControl->KeyPressChild	+= m_pKeyPressHandler;
//		};
//
//		//----------------------------------------------------------------------------
//		//	Destructor - dispose of controls
//		//----------------------------------------------------------------------------
//		~prtyFileName_FileChooser()
//		{
//			m_pActualControl->LeaveChild -= m_pLeaveChildHandler;
//			m_pActualControl->KeyPressChild -= m_pKeyPressHandler;
//			delete m_pOriginalValue;
//			prtyFileChooserControlBuffer::ReleaseControl(m_pActualControl);
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
//			const fsLocator& newvalue = (static_cast<prtyFileName*>(i_pProperty))->GetValue();
//
//			m_bLocalChangeNoUpdate = true;
//			fsLocator controlvalue;
//			tmaManagedStringUtils::ManagedStringToLocator(m_pActualControl->Fullpath, controlvalue);
//			if (controlvalue != newvalue)
//			{
//				//(*m_pOriginalValue) = newvalue.GetLastName();
//				m_pActualControl->Fullpath = tmaManagedStringUtils::LocatorToManagedString(newvalue);
//			}
//			m_bLocalChangeNoUpdate = false;
//		}
//
//		//----------------------------------------------------------------------------
//		// Update control to match the changes in the UI Info
//		//----------------------------------------------------------------------------
//		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo) override
//		{
//			prtyFileChooserUIInfo* pUII = static_cast<prtyFileChooserUIInfo*>(i_pUIInfo);
//			m_pActualControl->Filter = gcnew System::String( pUII->GetFileFilter().c_str() );
//			m_pActualControl->Enabled = !(i_pUIInfo->GetReadOnly());
//			m_pActualControl->FileNameOnly = pUII->GetShowFileNameOnly();
//			
//			// Update full path to filename
//			if (i_pUIInfo->GetNumberOfProperties() == 1)
//			{
//				prtyFileName* pActualProperty = static_cast<prtyFileName*>(i_pUIInfo->GetProperty(0));
//				itString filename = pActualProperty->GetValue();
//
//				fsLocator filepath(pUII->GetInitialDirectory());
//				gfFileTranslationMgr::ExpandLocator(filepath);		// get the full path
//				if (filename.GetLength() > 0)
//				{
//					filepath.Push( filename );
//				}
//				System::String^ pStr = tmaManagedStringUtils::LocatorToManagedString(filepath);
//				m_pActualControl->Fullpath = System::String::Copy(pStr);
//			}
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
//
//			// Should use Filename, not Fullpath because Filename represents when the
//			// text field is empty better. Or, when the fullpath represents a directory, not
//			// just a full path to a filename.
//
//			//fsLocator newvalue;
//			//tmaManagedStringUtils::ManagedStringToLocator(m_pActualControl->Fullpath,newvalue);
//			itString lastname;
//			//if (newvalue.GetNumNames() != 0)
//			//	lastname = newvalue.GetLastName();
//			tmaManagedStringUtils::ManagedStringToItString(m_pActualControl->Filename, lastname);
//
//			//causes missed updates--if ((*m_pOriginalValue) != lastname)
//			{
//				const int num_properties = this->GetNumberOfProperties();
//				if (num_properties > 1)
//					undoUndoMgr::BeginMultipleOperationBlock();
//
//				for (int i=0; i < num_properties; ++i)
//				{
//					prtyFileName* pActualProperty = static_cast<prtyFileName*>(GetProperty(i));
//					pActualProperty->SetValue( lastname, prtyProperty::eNewUndo );	// store UNDO also
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
//	TerawattManagedControls::FileChooser^ m_pActualControl;
//	itString* m_pOriginalValue;
//	System::EventHandler^ m_pLeaveChildHandler;
//	System::Windows::Forms::KeyPressEventHandler^ m_pKeyPressHandler;
//};
//
//#endif // _MANAGED
