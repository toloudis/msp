#pragma once

#ifndef TMLN_DRIVERSOUND_HPP
#include "Drivers/Sound/tmlnDriverSound.hpp"
#endif
#ifndef TMLN_DRIVERSOUNDINFO_HPP
#include "Drivers/Sound/tmlnDriverSoundInfo.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif

#ifndef CHNL_DIALOGUTIL_HPP
#include "Features/Channels/chnlDialogUtil.hpp"
#endif
#ifndef MNM_PATHS_HPP
#include "Support/mnm/mnmPaths.hpp"
#endif

#ifndef DBG_LOG_HPP
#include "Core/Dbg/dbgLog.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/Fs/fsFileUtil.hpp"
#endif
#ifndef GF_PATHS_HPP
#include "Core/Gf/gfPaths.hpp"
#endif

#ifdef _MANAGED

//============================================================================
//============================================================================
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


//============================================================================
//============================================================================
namespace StudioFramework
{
	/// <summary> 
	/// Summary for tmlnDriverSoundForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class tmlnDriverSoundForm : public System::Windows::Forms::Form
	{
	public: 
		static tmlnDriverSoundForm ^FormInstance = nullptr;
	public:
		tmlnDriverSoundForm(tmlnDriverSound &i_Driver)
			: m_Driver(i_Driver)
		{
			InitializeComponent();

			m_bDisableNotify = true;

			FormInstance = this;

			SetUpComponents();

			m_bDisableNotify = false;
		}

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_sound;
		}

		void UpdateForm()
		{
			SetUpComponents();
			this->Invalidate();
		}

	protected: 
		~tmlnDriverSoundForm()
		{
			if (this == FormInstance)
				FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::TabControl ^  tabControl_sound;
	private: System::Windows::Forms::TabPage ^  tabPage_sound;
	private: System::Windows::Forms::Label ^  label_filename;
	private: TerawattManagedControls::FileChooser ^  fileChooser_sound;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_starttime;
	private: System::Windows::Forms::Label ^  label_starttime;
	private: System::Windows::Forms::CheckBox ^  checkBox_settolength;
	private: System::Windows::Forms::Label ^  label_starttimeseconds;
	private: System::Windows::Forms::Label ^  label_endtimeseconds;
	private: System::Windows::Forms::Label ^  label_endtime;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_endtime;

	private: tmlnDriverSound &m_Driver;
	private: bool m_bDisableNotify;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->label_filename = gcnew System::Windows::Forms::Label();
			this->tabControl_sound = gcnew System::Windows::Forms::TabControl();
			this->tabPage_sound = gcnew System::Windows::Forms::TabPage();
			this->label_endtimeseconds = gcnew System::Windows::Forms::Label();
			this->label_endtime = gcnew System::Windows::Forms::Label();
			this->floatEdit_endtime = gcnew TerawattManagedControls::FloatEdit();
			this->checkBox_settolength = gcnew System::Windows::Forms::CheckBox();
			this->label_starttimeseconds = gcnew System::Windows::Forms::Label();
			this->label_starttime = gcnew System::Windows::Forms::Label();
			this->floatEdit_starttime = gcnew TerawattManagedControls::FloatEdit();
			this->fileChooser_sound = gcnew TerawattManagedControls::FileChooser();
			this->tabControl_sound->SuspendLayout();
			this->tabPage_sound->SuspendLayout();
			this->SuspendLayout();
			// 
			// label_filename
			// 
			this->label_filename->Location = System::Drawing::Point(8, 8);
			this->label_filename->Name = "label_filename";
			this->label_filename->Size = System::Drawing::Size(96, 28);
			this->label_filename->TabIndex = 0;
			this->label_filename->Text = "Sound Filename";
			// 
			// tabControl_sound
			// 
			this->tabControl_sound->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_sound->Controls->Add(this->tabPage_sound);
			this->tabControl_sound->Location = System::Drawing::Point(8, 8);
			this->tabControl_sound->Name = "tabControl_sound";
			this->tabControl_sound->SelectedIndex = 0;
			this->tabControl_sound->Size = System::Drawing::Size(483, 218);
			this->tabControl_sound->TabIndex = 3;
			// 
			// tabPage_sound
			// 
			this->tabPage_sound->Controls->Add(this->label_endtimeseconds);
			this->tabPage_sound->Controls->Add(this->label_endtime);
			this->tabPage_sound->Controls->Add(this->floatEdit_endtime);
			this->tabPage_sound->Controls->Add(this->checkBox_settolength);
			this->tabPage_sound->Controls->Add(this->label_starttimeseconds);
			this->tabPage_sound->Controls->Add(this->label_starttime);
			this->tabPage_sound->Controls->Add(this->floatEdit_starttime);
			this->tabPage_sound->Controls->Add(this->fileChooser_sound);
			this->tabPage_sound->Controls->Add(this->label_filename);
			this->tabPage_sound->Location = System::Drawing::Point(4, 22);
			this->tabPage_sound->Name = "tabPage_sound";
			this->tabPage_sound->Size = System::Drawing::Size(475, 192);
			this->tabPage_sound->TabIndex = 0;
			this->tabPage_sound->Text = "Sound";
			// 
			// label_endtimeseconds
			// 
			this->label_endtimeseconds->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->label_endtimeseconds->Location = System::Drawing::Point(136, 120);
			this->label_endtimeseconds->Name = "label_endtimeseconds";
			this->label_endtimeseconds->Size = System::Drawing::Size(152, 24);
			this->label_endtimeseconds->TabIndex = 10;
			this->label_endtimeseconds->Text = "seconds at end to skip";
			// 
			// label_endtime
			// 
			this->label_endtime->Location = System::Drawing::Point(8, 120);
			this->label_endtime->Name = "label_endtime";
			this->label_endtime->Size = System::Drawing::Size(56, 23);
			this->label_endtime->TabIndex = 9;
			this->label_endtime->Text = "End Time";
			// 
			// floatEdit_endtime
			// 
			this->floatEdit_endtime->Location = System::Drawing::Point(80, 120);
			this->floatEdit_endtime->Name = "floatEdit_endtime";
			this->floatEdit_endtime->Precision = (System::Int16)2;
			this->floatEdit_endtime->Size = System::Drawing::Size(48, 24);
			this->floatEdit_endtime->TabIndex = 8;
			this->floatEdit_endtime->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &tmlnDriverSoundForm::floatEdit_endtime_KeyPressChild);
			this->floatEdit_endtime->LeaveChild += gcnew System::EventHandler(this, &tmlnDriverSoundForm::floatEdit_endtime_LeaveChild);
			// 
			// checkBox_settolength
			// 
			this->checkBox_settolength->Checked = true;
			this->checkBox_settolength->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_settolength->Location = System::Drawing::Point(8, 64);
			this->checkBox_settolength->Name = "checkBox_settolength";
			this->checkBox_settolength->Size = System::Drawing::Size(192, 24);
			this->checkBox_settolength->TabIndex = 7;
			this->checkBox_settolength->Text = "Set Driver To Sound Length";
			this->checkBox_settolength->CheckedChanged += gcnew System::EventHandler(this, &tmlnDriverSoundForm::checkBox_settolength_CheckedChanged);
			// 
			// label_starttimeseconds
			// 
			this->label_starttimeseconds->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->label_starttimeseconds->Location = System::Drawing::Point(136, 96);
			this->label_starttimeseconds->Name = "label_starttimeseconds";
			this->label_starttimeseconds->Size = System::Drawing::Size(192, 24);
			this->label_starttimeseconds->TabIndex = 6;
			this->label_starttimeseconds->Text = "seconds at beginning to skip";
			// 
			// label_starttime
			// 
			this->label_starttime->Location = System::Drawing::Point(8, 96);
			this->label_starttime->Name = "label_starttime";
			this->label_starttime->Size = System::Drawing::Size(56, 23);
			this->label_starttime->TabIndex = 5;
			this->label_starttime->Text = "Start Time";
			// 
			// floatEdit_starttime
			// 
			this->floatEdit_starttime->Location = System::Drawing::Point(80, 96);
			this->floatEdit_starttime->Name = "floatEdit_starttime";
			this->floatEdit_starttime->Precision = (System::Int16)2;
			this->floatEdit_starttime->Size = System::Drawing::Size(48, 24);
			this->floatEdit_starttime->TabIndex = 4;
			this->floatEdit_starttime->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &tmlnDriverSoundForm::floatEdit_starttime_KeyPressChild);
			this->floatEdit_starttime->LeaveChild += gcnew System::EventHandler(this, &tmlnDriverSoundForm::floatEdit_starttime_LeaveChild);
			// 
			// fileChooser_sound
			// 
			this->fileChooser_sound->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->fileChooser_sound->Filter = "Sound files (*.wav)|*.wav|All files (*.*)|*.*";
			this->fileChooser_sound->Location = System::Drawing::Point(8, 32);
			this->fileChooser_sound->Name = "fileChooser_sound";
			this->fileChooser_sound->Size = System::Drawing::Size(463, 24);
			this->fileChooser_sound->TabIndex = 3;
			this->fileChooser_sound->ValueChanged += gcnew System::EventHandler(this, &tmlnDriverSoundForm::fileChooser_sound_ValueChanged);
			// 
			// tmlnDriverSoundForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(496, 238);
			this->Controls->Add(this->tabControl_sound);
			this->ImeMode = System::Windows::Forms::ImeMode::Off;
			this->Name = "tmlnDriverSoundForm";
			this->Text = "Sound Properties";
			this->TopMost = true;
			this->tabControl_sound->ResumeLayout(false);
			this->tabPage_sound->ResumeLayout(false);
			this->ResumeLayout(false);

		}		

private: void SetUpComponents()
		 {
			fsLocator fullpath( gfPaths::GetPath(mnmPaths::e_DataScene) );

			std::string dir;
			fsFileUtil::LocatorToANSIFilename( fullpath, dir );
			DBG_LOG1("directory %s", dir.c_str());

			itString scene = fullpath.GetLastName();

			// HACK: [rjk] push these names on stack to put sounds in general folder
			//	for whatever system is using this driver.

			if (m_Driver.GetSoundDir().GetNumNames() > 2)
			{
				fullpath.Push( m_Driver.GetSoundDir().GetName(2) );	// this should be the system dir name
				fullpath.Push( itString("General") );
				fullpath.Push( gfPaths::GetSubPath( gfPaths::e_Sounds ) );
				
				fsFileUtil::LocatorToANSIFilename( fullpath, dir );
				DBG_LOG1("= dir (%s)", dir.c_str());
			}

			fileChooser_sound->Fullpath = tmaManagedStringUtils::LocatorToManagedString(fullpath);


			tmlnDriverSoundInfo *pInfo = dynamic_cast<tmlnDriverSoundInfo*>(m_Driver.GetDriverInfo());

			//	set the sound name
			itString fname = fullpath.GetLastName();
			if ( fname != pInfo->m_SoundName )
			{
				std::string dir;
				fsFileUtil::LocatorToANSIFilename( fullpath, dir );
				DBG_LOG1("directory %s", dir.c_str());

				if ( fullpath.GetNumNames() > 0 )
				{
					if ( pInfo->m_SoundName.GetLength() > 0 )
					{
						itString cfName = fullpath.GetLastName();
						if ( cfName.HasSubString(itString(".wav"))
							|| cfName.HasSubString(itString(".WAV")) )
						{
							fullpath.Pop();
						}
						fullpath.Push( pInfo->m_SoundName );

						fsFileUtil::LocatorToANSIFilename( fullpath, dir );
						DBG_LOG1("modded directory %s", dir.c_str());

						System::String^ pFullPath = tmaManagedStringUtils::LocatorToManagedString(fullpath);
			
						try
						{
							this->fileChooser_sound->Fullpath = pFullPath;
						}
						catch ( Exception^ e ) 
						{
							Console::WriteLine( "Error: {0}", e->ToString() );
						}
					}
				}

				m_bDisableNotify = false;
			}

			//	 Set the sound start time
			this->floatEdit_starttime->Value = pInfo->m_fSoundStartTime;
			this->floatEdit_endtime->Value = pInfo->m_fSoundEndTime;
			this->checkBox_settolength->Checked = pInfo->m_bSetToSoundLength;

			//
			delete pInfo;
		 }

		//
private: System::Void fileChooser_sound_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			if (m_bDisableNotify)
				return;

			if (   ( fileChooser_sound->Fullpath->EndsWith(".wav") )
				|| ( fileChooser_sound->Fullpath->EndsWith(".WAV") ) )
			{
				m_bDisableNotify = true;

				fsLocator full_path;
				tmaManagedStringUtils::ManagedStringToLocator(fileChooser_sound->Fullpath, full_path);

				std::string dir;
				fsFileUtil::LocatorToANSIFilename( full_path, dir );
				DBG_LOG1("directory %s", dir.c_str());

				itString filename = full_path.GetLastName();

				tmlnDriverSoundInfo *pInfo = dynamic_cast<tmlnDriverSoundInfo*>(m_Driver.GetDriverInfo());
				if (filename != pInfo->m_SoundName)
				{
					full_path.Pop();	// remove the filename
					full_path.RemoveBefore(itString("Data"));
					m_Driver.SetSoundDir( full_path );

					pInfo->m_SoundName = filename;
					pInfo->m_bNeedsLoad = true;

					m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);
					delete pInfo;

					// update gui
					//chnlDialogUtil::UpdateChannels();
					chnlDialogUtil::UpdateDriver(&m_Driver);
				}

				m_bDisableNotify = false;
			}
		}

private: System::Void floatEdit_starttime_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (m_bDisableNotify)
				return;

			if (   ( fileChooser_sound->Fullpath->EndsWith(".wav") )
				|| ( fileChooser_sound->Fullpath->EndsWith(".WAV") ) )
			{
				m_bDisableNotify = true;
				tmlnDriverSoundInfo *pInfo = dynamic_cast<tmlnDriverSoundInfo*>(m_Driver.GetDriverInfo());

				if ( pInfo->m_fSoundStartTime != (float)floatEdit_starttime->Value)
				{
					pInfo->m_fSoundStartTime = (float)floatEdit_starttime->Value;
					m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);

					float duration = (m_Driver.GetEndTime() - m_Driver.GetBeginTime()) - pInfo->m_fSoundStartTime - pInfo->m_fSoundEndTime;
					
					m_Driver.SetEndTime( m_Driver.GetBeginTime() + duration );

					if (pInfo->m_bSetToSoundLength)
					{
						// update gui
						//chnlDialogUtil::UpdateChannels();
						chnlDialogUtil::UpdateDriver(&m_Driver);
					}
				}
				m_bDisableNotify = false;
			}
		 }

private: System::Void floatEdit_endtime_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (m_bDisableNotify)
				return;

			if (   ( fileChooser_sound->Fullpath->EndsWith(".wav") )
				|| ( fileChooser_sound->Fullpath->EndsWith(".WAV") ) )
			{
				m_bDisableNotify = true;
				tmlnDriverSoundInfo *pInfo = dynamic_cast<tmlnDriverSoundInfo*>(m_Driver.GetDriverInfo());
				
				if (pInfo->m_fSoundEndTime != (float)floatEdit_endtime->Value)
				{
					pInfo->m_fSoundEndTime = (float)floatEdit_endtime->Value;
					m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);

					float duration = (m_Driver.GetEndTime() - m_Driver.GetBeginTime()) - pInfo->m_fSoundStartTime;// - pInfo->m_fSoundEndTime;
					if (duration <= 0)
						duration = 0;
					
					m_Driver.SetEndTime( m_Driver.GetBeginTime() + duration );

					if (pInfo->m_bSetToSoundLength)
					{
						// update gui
						//chnlDialogUtil::UpdateChannels();
						chnlDialogUtil::UpdateDriver(&m_Driver);
					}
				}
				m_bDisableNotify = false;
			}
		 }

private: System::Void checkBox_settolength_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (m_bDisableNotify)
				return;

			tmlnDriverSoundInfo *pInfo = dynamic_cast<tmlnDriverSoundInfo*>(m_Driver.GetDriverInfo());

			if (pInfo->m_bSetToSoundLength != this->checkBox_settolength->Checked)
			{
				m_bDisableNotify = true;
				pInfo->m_bSetToSoundLength = this->checkBox_settolength->Checked;
				m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);

				if (pInfo->m_bSetToSoundLength)
				{
					// update gui
					//chnlDialogUtil::UpdateChannels();
					chnlDialogUtil::UpdateDriver(&m_Driver);
				}
				m_bDisableNotify = false;
			}
		 }

private: System::Void floatEdit_starttime_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
			if (e->KeyChar != (char)13)
				return;

			 floatEdit_starttime_ValueChanged(sender, e);
		 }

private: System::Void floatEdit_starttime_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 floatEdit_starttime_ValueChanged(sender, e);
		 }

private: System::Void floatEdit_endtime_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
			if (e->KeyChar != (char)13)
				return;

			 floatEdit_endtime_ValueChanged(sender, e);
		 }

private: System::Void floatEdit_endtime_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 floatEdit_endtime_ValueChanged(sender, e);
		 }

};
}


#endif // _MANAGED
