#pragma once

#ifndef SBRD_DRIVERANIMATEDTEXTURE_HPP
#include "Systems/Storyboards/Timeline/sbrdDriverAnimatedTexture.hpp"
#endif
#ifndef SBRD_LISTDATA_HPP
#include "Systems/Storyboards/Data/sbrdListData.hpp"
#endif
#ifndef SBRD_OBJECTMGR_HPP
#include "Systems/Storyboards/Object/sbrdObjectMgr.hpp"
#endif

#ifndef CHNL_DIALOGUTIL_HPP
#include "Features/Channels/chnlDialogUtil.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "Core/Dbg/dbgLog.hpp"
#endif
#ifndef FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif

#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace SystemStoryboards
{
	/// <summary> 
	/// Summary for sbrdDriverAnimatedTextureForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class sbrdDriverAnimatedTextureForm : public System::Windows::Forms::Form
	{
	public: 
		static sbrdDriverAnimatedTextureForm ^FormInstance = nullptr;
	public: 
		sbrdDriverAnimatedTextureForm( sbrdDriverAnimatedTexture& i_Driver )
			: m_Driver(i_Driver)
		{
			m_bDisableNotify = true;
			InitializeComponent();
			
			FormInstance = this;

			SetupData(i_Driver);
			m_bDisableNotify = false;
		}

		void UpdateForm()
		{
			SetupData(m_Driver);
			this->Invalidate();
		}
        
	protected: 
		~sbrdDriverAnimatedTextureForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::ComboBox ^  comboBox_AnimList;
	private: System::Windows::Forms::CheckBox ^  checkBox_DriverToAnimLength;
	private: System::Windows::Forms::TabControl ^  tabControl_animfull;
	private: System::Windows::Forms::TabPage ^  tabPage_animfull;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_FrameRate;
	private: System::Windows::Forms::CheckBox ^  checkBox_Looping;
	private: TerawattManagedControls::FloatEdit ^  editNumFrames;
	private: System::Windows::Forms::Label ^  label_numframes;
	private: System::Windows::Forms::Label ^  label_filename;
	private: System::Windows::Forms::Label ^  label_framerate;

	private: sbrdDriverAnimatedTexture	&m_Driver;
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
			this->comboBox_AnimList = (gcnew System::Windows::Forms::ComboBox());
			this->checkBox_DriverToAnimLength = (gcnew System::Windows::Forms::CheckBox());
			this->tabControl_animfull = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_animfull = (gcnew System::Windows::Forms::TabPage());
			this->label_numframes = (gcnew System::Windows::Forms::Label());
			this->editNumFrames = (gcnew TerawattManagedControls::FloatEdit());
			this->label_filename = (gcnew System::Windows::Forms::Label());
			this->label_framerate = (gcnew System::Windows::Forms::Label());
			this->checkBox_Looping = (gcnew System::Windows::Forms::CheckBox());
			this->floatEdit_FrameRate = (gcnew TerawattManagedControls::FloatEdit());
			this->tabControl_animfull->SuspendLayout();
			this->tabPage_animfull->SuspendLayout();
			this->SuspendLayout();
			// 
			// comboBox_AnimList
			// 
			this->comboBox_AnimList->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->comboBox_AnimList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox_AnimList->Location = System::Drawing::Point(88, 24);
			this->comboBox_AnimList->MaxDropDownItems = 12;
			this->comboBox_AnimList->Name = "comboBox_AnimList";
			this->comboBox_AnimList->Size = System::Drawing::Size(253, 21);
			this->comboBox_AnimList->TabIndex = 0;
			this->comboBox_AnimList->SelectedIndexChanged += gcnew System::EventHandler(this, &sbrdDriverAnimatedTextureForm::comboBox_AnimList_SelectedIndexChanged);
			// 
			// checkBox_DriverToAnimLength
			// 
			this->checkBox_DriverToAnimLength->Location = System::Drawing::Point(56, 112);
			this->checkBox_DriverToAnimLength->Name = "checkBox_DriverToAnimLength";
			this->checkBox_DriverToAnimLength->Size = System::Drawing::Size(160, 24);
			this->checkBox_DriverToAnimLength->TabIndex = 1;
			this->checkBox_DriverToAnimLength->Text = "Set driver to length of anim";
			this->checkBox_DriverToAnimLength->CheckedChanged += gcnew System::EventHandler(this, &sbrdDriverAnimatedTextureForm::checkBox_DriverToAnimLength_CheckedChanged);
			// 
			// tabControl_animfull
			// 
			this->tabControl_animfull->Controls->Add(this->tabPage_animfull);
			this->tabControl_animfull->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl_animfull->Location = System::Drawing::Point(0, 0);
			this->tabControl_animfull->Name = "tabControl_animfull";
			this->tabControl_animfull->SelectedIndex = 0;
			this->tabControl_animfull->Size = System::Drawing::Size(360, 331);
			this->tabControl_animfull->TabIndex = 2;
			// 
			// tabPage_animfull
			// 
			this->tabPage_animfull->Controls->Add(this->label_numframes);
			this->tabPage_animfull->Controls->Add(this->editNumFrames);
			this->tabPage_animfull->Controls->Add(this->label_filename);
			this->tabPage_animfull->Controls->Add(this->label_framerate);
			this->tabPage_animfull->Controls->Add(this->checkBox_Looping);
			this->tabPage_animfull->Controls->Add(this->floatEdit_FrameRate);
			this->tabPage_animfull->Controls->Add(this->comboBox_AnimList);
			this->tabPage_animfull->Controls->Add(this->checkBox_DriverToAnimLength);
			this->tabPage_animfull->Location = System::Drawing::Point(4, 22);
			this->tabPage_animfull->Name = "tabPage_animfull";
			this->tabPage_animfull->Size = System::Drawing::Size(352, 305);
			this->tabPage_animfull->TabIndex = 0;
			this->tabPage_animfull->Text = "Texture Animation";
			// 
			// label_numframes
			// 
			this->label_numframes->Location = System::Drawing::Point(88, 56);
			this->label_numframes->Name = "label_numframes";
			this->label_numframes->Size = System::Drawing::Size(112, 21);
			this->label_numframes->TabIndex = 11;
			this->label_numframes->Text = "Number of Frames:";
			// 
			// editNumFrames
			// 
			this->editNumFrames->Location = System::Drawing::Point(216, 56);
			this->editNumFrames->Name = "editNumFrames";
			this->editNumFrames->Precision = (System::Int16)0;
			this->editNumFrames->Size = System::Drawing::Size(73, 28);
			this->editNumFrames->TabIndex = 10;
			this->editNumFrames->ValueChanged += gcnew System::EventHandler(this, &sbrdDriverAnimatedTextureForm::editNumFrames_ValueChanged);
			// 
			// label_filename
			// 
			this->label_filename->Location = System::Drawing::Point(13, 28);
			this->label_filename->Name = "label_filename";
			this->label_filename->Size = System::Drawing::Size(74, 14);
			this->label_filename->TabIndex = 9;
			this->label_filename->Text = "Filename:";
			// 
			// label_framerate
			// 
			this->label_framerate->Location = System::Drawing::Point(16, 160);
			this->label_framerate->Name = "label_framerate";
			this->label_framerate->Size = System::Drawing::Size(67, 21);
			this->label_framerate->TabIndex = 6;
			this->label_framerate->Text = "Frame Rate:";
			// 
			// checkBox_Looping
			// 
			this->checkBox_Looping->Location = System::Drawing::Point(56, 88);
			this->checkBox_Looping->Name = "checkBox_Looping";
			this->checkBox_Looping->Size = System::Drawing::Size(153, 28);
			this->checkBox_Looping->TabIndex = 5;
			this->checkBox_Looping->Text = "Looping";
			this->checkBox_Looping->CheckedChanged += gcnew System::EventHandler(this, &sbrdDriverAnimatedTextureForm::checkBox_Looping_CheckedChanged);
			// 
			// floatEdit_FrameRate
			// 
			this->floatEdit_FrameRate->Location = System::Drawing::Point(96, 160);
			this->floatEdit_FrameRate->Name = "floatEdit_FrameRate";
			this->floatEdit_FrameRate->Precision = (System::Int16)2;
			this->floatEdit_FrameRate->Size = System::Drawing::Size(73, 28);
			this->floatEdit_FrameRate->TabIndex = 2;
			this->floatEdit_FrameRate->ValueChanged += gcnew System::EventHandler(this, &sbrdDriverAnimatedTextureForm::floatEdit_FrameRate_ValueChanged);
			// 
			// sbrdDriverAnimatedTextureForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(360, 331);
			this->Controls->Add(this->tabControl_animfull);
			this->Name = "sbrdDriverAnimatedTextureForm";
			this->Text = "Texture Animation Properties";
			this->TopMost = true;
			this->tabControl_animfull->ResumeLayout(false);
			this->tabPage_animfull->ResumeLayout(false);
			this->ResumeLayout(false);

		}		

		//

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_driver_length_from_anim_length()
		{
			// if this checkbox is checked, resize the driver length.
			//
			if (   ( m_Driver.IsDriverToAnimLength() )
				&& ( m_Driver.GetFirstTextureFilename().GetLength() > 0 ) )
			{
				float anim_duration = m_Driver.GetAnimationLength();
			
				m_Driver.SetEndTime( m_Driver.GetBeginTime() + anim_duration );
			
				// update gui
				//chnlDialogUtil::UpdateChannels();
				chnlDialogUtil::UpdateDriver(&m_Driver);
			}
		};

		
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void fill_combo()
		{
			sbrdListData& data = sbrdObjectMgr::GetListData();

			const int num_objects = data.m_Filenames.size();
			for (int i=0; i<num_objects; i++)
			{
				comboBox_AnimList->Items->Add( tmaManagedStringUtils::ItStringToManagedString(data.m_Filenames[i].GetValue()) );
			}
		};

		//
		//
		void SetupData(const sbrdDriverAnimatedTexture& i_Driver)
		{
			m_bDisableNotify = true;

			fill_combo();

			this->comboBox_AnimList->Text = tmaManagedStringUtils::ItStringToManagedString(i_Driver.GetFirstTextureFilename());
			this->checkBox_DriverToAnimLength->Checked = i_Driver.IsDriverToAnimLength();
			this->checkBox_Looping->Checked = i_Driver.GetLooping();

			this->floatEdit_FrameRate->Value = i_Driver.GetFrameRate();
			this->editNumFrames->Value = i_Driver.GetAnimationNumFrames();

			m_bDisableNotify = false;
		}

		

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_animfull;
		}

private: System::Void checkBox_DriverToAnimLength_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
			{
				//DBG_LOG1("checked %s", (checkBox_DriverToAnimLength->Checked ? "true":"false") );

				if (!m_bDisableNotify)
				{
					m_bDisableNotify = true;
					m_Driver.SetDriverToAnimLengthFlag( checkBox_DriverToAnimLength->Checked );
				
					//	check if the driver length needs to change.
					update_driver_length_from_anim_length();
					m_bDisableNotify = false;
				}
			}

private: System::Void comboBox_AnimList_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
			{
				if (!m_bDisableNotify)
				{
					m_bDisableNotify = true;
					itString fname;
					tmaManagedStringUtils::ManagedStringToItString( comboBox_AnimList->Text, fname );
					m_Driver.SetTextures( fname, 1 );

					//	check if the driver length needs to change.
					update_driver_length_from_anim_length();

					// just set number of frames to 1
					this->editNumFrames->Value = 1;
					m_bDisableNotify = false;
				}
			}

private: System::Void checkBox_Looping_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
			{
				if (!m_bDisableNotify)
				{
					m_bDisableNotify = true;
					m_Driver.SetLooping( this->checkBox_Looping->Checked );
					m_bDisableNotify = false;
				}
			}

private: System::Void floatEdit_FrameRate_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				m_Driver.SetFrameRate( (float)this->floatEdit_FrameRate->Value );
				update_driver_length_from_anim_length();
				m_bDisableNotify = false;
			}
		 }

private: System::Void editNumFrames_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				m_Driver.SetTextures( m_Driver.GetFirstTextureFilename(),
					(int)this->editNumFrames->Value );
				update_driver_length_from_anim_length();
				m_bDisableNotify = false;
			}
		 }


};
}
#endif // _MANAGED
