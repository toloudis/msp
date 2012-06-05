#pragma once

#ifndef CHTR_CHANNELANIMATIONSUB_HPP
#include "Systems/Character/Timeline/chtrChannelAnimationSub.hpp"
#endif
#ifndef CHTR_DRIVERANIMATIONSUB_HPP
#include "Systems/Character/Drivers/chtrDriverAnimationSub.hpp"
#endif
#ifndef CHTR_ANIMLIST_HPP
#include "Systems/Character/GUI/chtrAnimList.hpp"
#endif
#ifndef CHNL_DIALOGUTIL_HPP
#include "Features/Channels/chnlDialogUtil.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "Core/dbg/dbgLog.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifndef FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif

#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace SystemChtr
{
	/// <summary> 
	/// Summary for chtrDriverAnimationSubForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class chtrDriverAnimationSubForm : public System::Windows::Forms::Form
	{
	public: 
		static chtrDriverAnimationSubForm ^FormInstance = nullptr;
	public: 
		chtrDriverAnimationSubForm( chtrDriverAnimationSub& i_Driver )
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
		~chtrDriverAnimationSubForm()
		{
			if (components)
			{
				delete components;
			}
		}



	private: chtrDriverAnimationSub	&m_Driver;
	private: bool m_bDisableNotify;
	private: System::Windows::Forms::TabControl ^  tabControl_animsub;
	private: System::Windows::Forms::TabPage ^  tabPage_animsub;
	private: System::Windows::Forms::TextBox ^  textBox_AnimName;





	private: System::Windows::Forms::CheckBox ^  checkBox_Looping;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_EndFrame;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_StartFrame;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_FrameRate;
	private: System::Windows::Forms::ComboBox ^  comboBox_AnimList;
	private: System::Windows::Forms::CheckBox ^  checkBox_DriverToAnimLength;
	private: System::Windows::Forms::Label ^  label_animname;
	private: System::Windows::Forms::Label ^  label_filename;
	private: System::Windows::Forms::Label ^  label_endframe;
	private: System::Windows::Forms::Label ^  label_startframe;
	private: System::Windows::Forms::Label ^  label_framerate;

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
			this->tabControl_animsub = gcnew System::Windows::Forms::TabControl();
			this->tabPage_animsub = gcnew System::Windows::Forms::TabPage();
			this->textBox_AnimName = gcnew System::Windows::Forms::TextBox();
			this->label_animname = gcnew System::Windows::Forms::Label();
			this->label_filename = gcnew System::Windows::Forms::Label();
			this->label_endframe = gcnew System::Windows::Forms::Label();
			this->label_startframe = gcnew System::Windows::Forms::Label();
			this->label_framerate = gcnew System::Windows::Forms::Label();
			this->checkBox_Looping = gcnew System::Windows::Forms::CheckBox();
			this->rangedFloat_EndFrame = gcnew TerawattManagedControls::RangedFloat();
			this->rangedFloat_StartFrame = gcnew TerawattManagedControls::RangedFloat();
			this->floatEdit_FrameRate = gcnew TerawattManagedControls::FloatEdit();
			this->comboBox_AnimList = gcnew System::Windows::Forms::ComboBox();
			this->checkBox_DriverToAnimLength = gcnew System::Windows::Forms::CheckBox();
			this->tabControl_animsub->SuspendLayout();
			this->tabPage_animsub->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_animsub
			// 
			this->tabControl_animsub->Controls->Add(this->tabPage_animsub);
			this->tabControl_animsub->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl_animsub->Location = System::Drawing::Point(0, 0);
			this->tabControl_animsub->Name = "tabControl_animsub";
			this->tabControl_animsub->SelectedIndex = 0;
			this->tabControl_animsub->Size = System::Drawing::Size(360, 326);
			this->tabControl_animsub->TabIndex = 2;
			// 
			// tabPage_animsub
			// 
			this->tabPage_animsub->Controls->Add(this->textBox_AnimName);
			this->tabPage_animsub->Controls->Add(this->label_animname);
			this->tabPage_animsub->Controls->Add(this->label_filename);
			this->tabPage_animsub->Controls->Add(this->label_endframe);
			this->tabPage_animsub->Controls->Add(this->label_startframe);
			this->tabPage_animsub->Controls->Add(this->label_framerate);
			this->tabPage_animsub->Controls->Add(this->checkBox_Looping);
			this->tabPage_animsub->Controls->Add(this->rangedFloat_EndFrame);
			this->tabPage_animsub->Controls->Add(this->rangedFloat_StartFrame);
			this->tabPage_animsub->Controls->Add(this->floatEdit_FrameRate);
			this->tabPage_animsub->Controls->Add(this->comboBox_AnimList);
			this->tabPage_animsub->Controls->Add(this->checkBox_DriverToAnimLength);
			this->tabPage_animsub->Location = System::Drawing::Point(4, 22);
			this->tabPage_animsub->Name = "tabPage_animsub";
			this->tabPage_animsub->Size = System::Drawing::Size(352, 300);
			this->tabPage_animsub->TabIndex = 0;
			this->tabPage_animsub->Text = "SubAnim";
			// 
			// textBox_AnimName
			// 
			this->textBox_AnimName->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textBox_AnimName->Location = System::Drawing::Point(88, 64);
			this->textBox_AnimName->Name = "textBox_AnimName";
			this->textBox_AnimName->Size = System::Drawing::Size(248, 20);
			this->textBox_AnimName->TabIndex = 23;
			this->textBox_AnimName->Text = "";
			this->textBox_AnimName->TextChanged += gcnew System::EventHandler(this, &chtrDriverAnimationSubForm::textBox_AnimName_TextChanged);
			// 
			// label_animname
			// 
			this->label_animname->Location = System::Drawing::Point(16, 64);
			this->label_animname->Name = "label_animname";
			this->label_animname->Size = System::Drawing::Size(74, 14);
			this->label_animname->TabIndex = 22;
			this->label_animname->Text = "Anim Name:";
			// 
			// label_filename
			// 
			this->label_filename->Location = System::Drawing::Point(16, 24);
			this->label_filename->Name = "label_filename";
			this->label_filename->Size = System::Drawing::Size(56, 14);
			this->label_filename->TabIndex = 21;
			this->label_filename->Text = "Filename:";
			// 
			// label_endframe
			// 
			this->label_endframe->Location = System::Drawing::Point(24, 240);
			this->label_endframe->Name = "label_endframe";
			this->label_endframe->Size = System::Drawing::Size(67, 27);
			this->label_endframe->TabIndex = 20;
			this->label_endframe->Text = "End Frame:";
			// 
			// label_startframe
			// 
			this->label_startframe->Location = System::Drawing::Point(24, 204);
			this->label_startframe->Name = "label_startframe";
			this->label_startframe->Size = System::Drawing::Size(73, 21);
			this->label_startframe->TabIndex = 19;
			this->label_startframe->Text = "Start Frame:";
			// 
			// label_framerate
			// 
			this->label_framerate->Location = System::Drawing::Point(24, 160);
			this->label_framerate->Name = "label_framerate";
			this->label_framerate->Size = System::Drawing::Size(67, 21);
			this->label_framerate->TabIndex = 18;
			this->label_framerate->Text = "Frame Rate:";
			// 
			// checkBox_Looping
			// 
			this->checkBox_Looping->Location = System::Drawing::Point(64, 88);
			this->checkBox_Looping->Name = "checkBox_Looping";
			this->checkBox_Looping->Size = System::Drawing::Size(153, 28);
			this->checkBox_Looping->TabIndex = 17;
			this->checkBox_Looping->Text = "Looping";
			this->checkBox_Looping->CheckedChanged += gcnew System::EventHandler(this, &chtrDriverAnimationSubForm::checkBox_Looping_CheckedChanged);
			// 
			// rangedFloat_EndFrame
			// 
			this->rangedFloat_EndFrame->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->rangedFloat_EndFrame->Exponent = (System::Int16)1;
			this->rangedFloat_EndFrame->Location = System::Drawing::Point(96, 236);
			this->rangedFloat_EndFrame->Minimum = 1;
			this->rangedFloat_EndFrame->Name = "rangedFloat_EndFrame";
			this->rangedFloat_EndFrame->NumTicks = (System::Int16)100;
			this->rangedFloat_EndFrame->Precision = (System::Int16)2;
			this->rangedFloat_EndFrame->Size = System::Drawing::Size(224, 34);
			this->rangedFloat_EndFrame->TabIndex = 16;
			this->rangedFloat_EndFrame->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &chtrDriverAnimationSubForm::rangedFloat_EndFrame_KeyPressChild);
			this->rangedFloat_EndFrame->LeaveChild += gcnew System::EventHandler(this, &chtrDriverAnimationSubForm::rangedFloat_EndFrame_LeaveChild);
			this->rangedFloat_EndFrame->ValueChanged += gcnew System::EventHandler(this, &chtrDriverAnimationSubForm::rangedFloat_EndFrame_ValueChanged);
			// 
			// rangedFloat_StartFrame
			// 
			this->rangedFloat_StartFrame->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->rangedFloat_StartFrame->Exponent = (System::Int16)1;
			this->rangedFloat_StartFrame->Location = System::Drawing::Point(96, 200);
			this->rangedFloat_StartFrame->Minimum = 1;
			this->rangedFloat_StartFrame->Name = "rangedFloat_StartFrame";
			this->rangedFloat_StartFrame->NumTicks = (System::Int16)100;
			this->rangedFloat_StartFrame->Precision = (System::Int16)2;
			this->rangedFloat_StartFrame->Size = System::Drawing::Size(224, 28);
			this->rangedFloat_StartFrame->TabIndex = 15;
			this->rangedFloat_StartFrame->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &chtrDriverAnimationSubForm::rangedFloat_StartFrame_KeyPressChild);
			this->rangedFloat_StartFrame->LeaveChild += gcnew System::EventHandler(this, &chtrDriverAnimationSubForm::rangedFloat_StartFrame_LeaveChild);
			this->rangedFloat_StartFrame->ValueChanged += gcnew System::EventHandler(this, &chtrDriverAnimationSubForm::rangedFloat_StartFrame_ValueChanged);
			// 
			// floatEdit_FrameRate
			// 
			this->floatEdit_FrameRate->Location = System::Drawing::Point(104, 160);
			this->floatEdit_FrameRate->Name = "floatEdit_FrameRate";
			this->floatEdit_FrameRate->Precision = (System::Int16)2;
			this->floatEdit_FrameRate->Size = System::Drawing::Size(73, 28);
			this->floatEdit_FrameRate->TabIndex = 14;
			this->floatEdit_FrameRate->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &chtrDriverAnimationSubForm::floatEdit_FrameRate_KeyPressChild);
			this->floatEdit_FrameRate->LeaveChild += gcnew System::EventHandler(this, &chtrDriverAnimationSubForm::floatEdit_FrameRate_LeaveChild);
			this->floatEdit_FrameRate->ValueChanged += gcnew System::EventHandler(this, &chtrDriverAnimationSubForm::floatEdit_FrameRate_ValueChanged);
			// 
			// comboBox_AnimList
			// 
			this->comboBox_AnimList->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->comboBox_AnimList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox_AnimList->Location = System::Drawing::Point(88, 24);
			this->comboBox_AnimList->MaxDropDownItems = 12;
			this->comboBox_AnimList->Name = "comboBox_AnimList";
			this->comboBox_AnimList->Size = System::Drawing::Size(248, 21);
			this->comboBox_AnimList->TabIndex = 12;
			this->comboBox_AnimList->SelectedIndexChanged += gcnew System::EventHandler(this, &chtrDriverAnimationSubForm::comboBox_AnimList_SelectedIndexChanged);
			// 
			// checkBox_DriverToAnimLength
			// 
			this->checkBox_DriverToAnimLength->Location = System::Drawing::Point(64, 120);
			this->checkBox_DriverToAnimLength->Name = "checkBox_DriverToAnimLength";
			this->checkBox_DriverToAnimLength->Size = System::Drawing::Size(160, 24);
			this->checkBox_DriverToAnimLength->TabIndex = 13;
			this->checkBox_DriverToAnimLength->Text = "Set driver to length of anim";
			this->checkBox_DriverToAnimLength->CheckedChanged += gcnew System::EventHandler(this, &chtrDriverAnimationSubForm::checkBox_DriverToAnimLength_CheckedChanged);
			// 
			// chtrDriverAnimationSubForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(360, 326);
			this->Controls->Add(this->tabControl_animsub);
			this->MinimumSize = System::Drawing::Size(360, 360);
			this->Name = "chtrDriverAnimationSubForm";
			this->Text = "Character SubAnim Properties";
			this->TopMost = true;
			this->tabControl_animsub->ResumeLayout(false);
			this->tabPage_animsub->ResumeLayout(false);
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
				&& ( m_Driver.GetAnimFilename().GetLength() > 0 ) )
			{
				float anim_duration = m_Driver.GetAnimationLength();

				m_Driver.SetEndTime( m_Driver.GetBeginTime() + anim_duration );

				// update gui
				//chnlDialogUtil::UpdateChannels();
				chnlDialogUtil::UpdateDriver(&m_Driver);

				//DBG_LOG1( "driver animfull duration  %6.2f", m_Driver.GetDuration() );
			}
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void fill_combo()
		{
			fsysFileList file_list;
			chtrAnimList::BuildFileList(file_list, m_Driver.GetCharacterDir());

			comboBox_AnimList->Items->Clear();
			int num_anims = file_list.Size();
			for (int i=0; i<num_anims; i++)
			{
				itString filename = file_list.GetFilename(i);
				comboBox_AnimList->Items->Add( tmaManagedStringUtils::ItStringToManagedString(filename) );

			}
		};

		//
		//
		void SetupData(const chtrDriverAnimationSub& i_Driver)
		{
			m_bDisableNotify = true;

			fill_combo();

			this->textBox_AnimName->Text = gcnew String( i_Driver.GetAnimName().c_str() );
			this->comboBox_AnimList->Text = tmaManagedStringUtils::ItStringToManagedString(i_Driver.GetAnimFilename());
			this->checkBox_DriverToAnimLength->Checked = i_Driver.IsDriverToAnimLength();
			this->checkBox_Looping->Checked = i_Driver.GetLooping();

			this->floatEdit_FrameRate->Value = i_Driver.GetFrameRate();

			float num_frames = i_Driver.GetAnimationNumFrames();
			if (num_frames > 0)
			{
				this->rangedFloat_StartFrame->Minimum = -1;
				this->rangedFloat_StartFrame->Maximum = num_frames;
				this->rangedFloat_StartFrame->NumTicks = (short)(num_frames + 1);
				this->rangedFloat_EndFrame->Minimum = -1;
				this->rangedFloat_EndFrame->Maximum = num_frames;
				this->rangedFloat_EndFrame->NumTicks = (short)(num_frames + 1);

				if (i_Driver.GetStartFrame() < num_frames)
					this->rangedFloat_StartFrame->Value = i_Driver.GetStartFrame();
				else 
					this->rangedFloat_StartFrame->Value = -1;

				if (i_Driver.GetEndFrame() < num_frames)
					this->rangedFloat_EndFrame->Value = i_Driver.GetEndFrame();
				else 
					this->rangedFloat_EndFrame->Value = -1;
			}
			else
			{	
				this->rangedFloat_StartFrame->Enabled = false;
				this->rangedFloat_EndFrame->Enabled = false;
			}

			m_bDisableNotify = false;
		}


	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_animsub;
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
					m_Driver.SetAnimFilename( fname );

					//	check if the driver length needs to change.
					update_driver_length_from_anim_length();
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

private: System::Void rangedFloat_StartFrame_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				m_Driver.SetStartFrame( (float)this->rangedFloat_StartFrame->Value );
				update_driver_length_from_anim_length();
				m_bDisableNotify = false;
			}
		 }

private: System::Void rangedFloat_EndFrame_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				m_Driver.SetEndFrame( (float)this->rangedFloat_EndFrame->Value );
				update_driver_length_from_anim_length();
				m_bDisableNotify = false;
			}
		 }

private: System::Void textBox_AnimName_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				std::string anim_name;
				tmaManagedStringUtils::ManagedStringToStdString(this->textBox_AnimName->Text, anim_name);
				m_Driver.SetAnimName(anim_name);

				// update gui
				//chnlDialogUtil::UpdateChannels();
				chnlDialogUtil::UpdateDriver(&m_Driver);

				m_bDisableNotify = false;
			}
		 }

private: System::Void floatEdit_FrameRate_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (m_bDisableNotify) return;
			
			m_bDisableNotify = true;
			m_Driver.SetFrameRate( (float)this->floatEdit_FrameRate->Value );
			update_driver_length_from_anim_length();
			m_bDisableNotify = false;
		 }

private: System::Void floatEdit_FrameRate_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
			if (m_bDisableNotify) return;

			if (e->KeyChar == (char)13)
			{
				m_bDisableNotify = true;
				m_Driver.SetFrameRate( (float)this->floatEdit_FrameRate->Value );
				update_driver_length_from_anim_length();
				m_bDisableNotify = false;
			}
		 }

private: System::Void rangedFloat_StartFrame_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
			if (m_bDisableNotify) return;

			if (e->KeyChar == (char)13)
			{
				m_bDisableNotify = true;
				m_Driver.SetStartFrame( (float)this->rangedFloat_StartFrame->Value );
				update_driver_length_from_anim_length();
				m_bDisableNotify = false;
			}
		 }

private: System::Void rangedFloat_StartFrame_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (m_bDisableNotify) return;
			
			m_bDisableNotify = true;
			m_Driver.SetStartFrame( (float)this->rangedFloat_StartFrame->Value );
			update_driver_length_from_anim_length();
			m_bDisableNotify = false;
		 }

private: System::Void rangedFloat_EndFrame_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
			if (m_bDisableNotify) return;

			if (e->KeyChar == (char)13)
			{
				m_bDisableNotify = true;
				m_Driver.SetEndFrame( (float)this->rangedFloat_EndFrame->Value );
				update_driver_length_from_anim_length();
				m_bDisableNotify = false;
			}
		 }

private: System::Void rangedFloat_EndFrame_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (m_bDisableNotify) return;
			
			m_bDisableNotify = true;
			m_Driver.SetEndFrame( (float)this->rangedFloat_EndFrame->Value );
			update_driver_length_from_anim_length();
			m_bDisableNotify = false;
		 }

};
}
#endif // _MANAGED
