#pragma once

#ifndef PROP_ADAPTERANIMATIONSUB_HPP
#include "Systems/Props/Timeline/propChannelAnimationSub.hpp"
#endif
#ifndef PROP_DRIVERANIMATIONSUB_HPP
#include "Systems/Props/Drivers/propDriverAnimationSub.hpp"
#endif
#ifndef CHNL_DIALOGUTIL_HPP
#include "Features/Channels/chnlDialogUtil.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "Core/Dbg/dbgLog.hpp"
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


namespace SystemProp
{
	/// <summary> 
	/// Summary for propDriverAnimationSubForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class propDriverAnimationSubForm : public System::Windows::Forms::Form
	{
	public: 
		static propDriverAnimationSubForm ^FormInstance = nullptr;
	public: 
		propDriverAnimationSubForm( propDriverAnimationSub& i_Driver )
			: m_Driver(i_Driver)
		{
			m_bDisableNotify = true;
			InitializeComponent();

			FormInstance = this;

			SetUpComponents();
			m_bDisableNotify = false;
		}
        
		void UpdateForm()
		{
			SetUpComponents();
			this->Invalidate();
		}


	protected: 
		~propDriverAnimationSubForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ComboBox ^  comboBox_AnimList;
	private: System::Windows::Forms::CheckBox ^  checkBox_DriverToAnimLength;

	private: propDriverAnimationSub	&m_Driver;
	private: bool m_bDisableNotify;
	private: System::Windows::Forms::TabControl ^  tabControl_animsub;
	private: System::Windows::Forms::TabPage ^  tabPage_animsub;

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
			this->comboBox_AnimList = gcnew System::Windows::Forms::ComboBox();
			this->checkBox_DriverToAnimLength = gcnew System::Windows::Forms::CheckBox();
			this->tabControl_animsub = gcnew System::Windows::Forms::TabControl();
			this->tabPage_animsub = gcnew System::Windows::Forms::TabPage();
			this->tabControl_animsub->SuspendLayout();
			this->tabPage_animsub->SuspendLayout();
			this->SuspendLayout();
			// 
			// comboBox_AnimList
			// 
			this->comboBox_AnimList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox_AnimList->Location = System::Drawing::Point(8, 32);
			this->comboBox_AnimList->Name = "comboBox_AnimList";
			this->comboBox_AnimList->Size = System::Drawing::Size(256, 21);
			this->comboBox_AnimList->TabIndex = 0;
			this->comboBox_AnimList->SelectedIndexChanged += gcnew System::EventHandler(this, &propDriverAnimationSubForm::comboBox_AnimList_SelectedIndexChanged);
			// 
			// checkBox_DriverToAnimLength
			// 
			this->checkBox_DriverToAnimLength->Location = System::Drawing::Point(54, 80);
			this->checkBox_DriverToAnimLength->Name = "checkBox_DriverToAnimLength";
			this->checkBox_DriverToAnimLength->Size = System::Drawing::Size(164, 24);
			this->checkBox_DriverToAnimLength->TabIndex = 1;
			this->checkBox_DriverToAnimLength->Text = "Set driver to length of anim";
			this->checkBox_DriverToAnimLength->CheckedChanged += gcnew System::EventHandler(this, &propDriverAnimationSubForm::checkBox_DriverToAnimLength_CheckedChanged);
			// 
			// tabControl_animsub
			// 
			this->tabControl_animsub->Controls->Add(this->tabPage_animsub);
			this->tabControl_animsub->Location = System::Drawing::Point(8, 8);
			this->tabControl_animsub->Name = "tabControl_animsub";
			this->tabControl_animsub->SelectedIndex = 0;
			this->tabControl_animsub->Size = System::Drawing::Size(280, 160);
			this->tabControl_animsub->TabIndex = 2;
			// 
			// tabPage_animsub
			// 
			this->tabPage_animsub->Controls->Add(this->comboBox_AnimList);
			this->tabPage_animsub->Controls->Add(this->checkBox_DriverToAnimLength);
			this->tabPage_animsub->Location = System::Drawing::Point(4, 22);
			this->tabPage_animsub->Name = "tabPage_animsub";
			this->tabPage_animsub->Size = System::Drawing::Size(272, 134);
			this->tabPage_animsub->TabIndex = 0;
			this->tabPage_animsub->Text = "sub-anim";
			// 
			// propDriverAnimationSubForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(292, 173);
			this->Controls->Add(this->tabControl_animsub);
			this->Name = "propDriverAnimationSubForm";
			this->Text = "Prop Driver Anim-Full Properties";
			this->TopMost = true;
			this->tabControl_animsub->ResumeLayout(false);
			this->tabPage_animsub->ResumeLayout(false);
			this->ResumeLayout(false);

		}		

		//
	private: void SetUpComponents()
			 {
				set_checkbox();
				fill_combo();
				comboBox_AnimList->SelectedIndex = m_Driver.GetAnimIndex();
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
				//DBG_LOG2("selected %d %s", comboBox_AnimList->SelectedIndex, comboBox_AnimList->get_SelectedText() );

				 if (!m_bDisableNotify)
				 {
					m_bDisableNotify = true;
					itString target;
					tmaManagedStringUtils::ManagedStringToItString( comboBox_AnimList->Text, target );
					m_Driver.SetAnimName( target );

					DBG_LOG1( "selected index %d", comboBox_AnimList->SelectedIndex );

					m_Driver.SetAnimIndex( comboBox_AnimList->SelectedIndex );

					//	check if the driver length needs to change.
					update_driver_length_from_anim_length();
					m_bDisableNotify = false;
				 }
			 }

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_driver_length_from_anim_length()
		{
			// if this checkbox is checked, resize the driver length.
			//
			if (   ( m_Driver.IsDriverToAnimLength() )
				&& ( m_Driver.GetAnimIndex() >= 0 ) )
			{
				float anim_duration;
				anim_duration = m_Driver.Adapter().GetAnimationLength( m_Driver.GetAnimIndex() );

				m_Driver.SetEndTime( m_Driver.GetBeginTime() + anim_duration );

				// update gui
				//chnlDialogUtil::UpdateChannels();
				chnlDialogUtil::UpdateDriver(&m_Driver);

				//DBG_LOG1( "driver animfull duration  %6.2f", m_Driver.GetDuration() );
			}
		};
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void set_checkbox()
		{
			checkBox_DriverToAnimLength->Checked = m_Driver.IsDriverToAnimLength();
		};
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void fill_combo()
		{
			propChannelAnimationSub::AnimMap anim_map;
			m_Driver.Adapter().GetAnimationList( anim_map );

			propChannelAnimationSub::AnimMap::iterator it	= anim_map.begin();
			propChannelAnimationSub::AnimMap::iterator end	= anim_map.end();
			
			while( it != end )
			{
				DBG_LOG2( "combo %02d %s", it->first, it->second.c_str() );

				comboBox_AnimList->Items->Insert( it->first, gcnew System::String( it->second.c_str() ) );
				++it;
			}
		};

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_animsub;
		}
	};
}
#endif // _MANAGED
