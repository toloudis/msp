#pragma once

#ifndef PRTCL_ADAPTEREMIT_HPP
#include "Systems/Particles/Timeline/prtclAdapterEmit.hpp"
#endif
#ifndef PRTCL_DRIVEREMIT_HPP
#include "Systems/Particles/Timeline/prtclDriverEmit.hpp"
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


namespace SystemParticle
{
	/// <summary>
	/// Summary for prtclDriverEmitForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' prtclerty for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact prtclerly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class prtclDriverEmitForm : public System::Windows::Forms::Form
	{
	public: 
		static prtclDriverEmitForm ^FormInstance = nullptr;
	public:
		prtclDriverEmitForm( prtclDriverEmit& i_Driver )
			: m_Driver(i_Driver)
		{
			InitializeComponent();

			FormInstance = this;

			SetUpComponents();
		}

		void UpdateForm()
		{
			SetUpComponents();
			this->Invalidate();
		}

	protected:
		~prtclDriverEmitForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: prtclDriverEmit	&m_Driver;
	private: System::Windows::Forms::Label ^  label_noproperties;
	private: System::Windows::Forms::TabControl ^  tabControl_emit;
	private: System::Windows::Forms::TabPage ^  tabPage_emit;

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
			this->label_noproperties = gcnew System::Windows::Forms::Label();
			this->tabControl_emit = gcnew System::Windows::Forms::TabControl();
			this->tabPage_emit = gcnew System::Windows::Forms::TabPage();
			this->tabControl_emit->SuspendLayout();
			this->tabPage_emit->SuspendLayout();
			this->SuspendLayout();
			// 
			// label_noproperties
			// 
			this->label_noproperties->Location = System::Drawing::Point(52, 28);
			this->label_noproperties->Name = "label_noproperties";
			this->label_noproperties->Size = System::Drawing::Size(160, 23);
			this->label_noproperties->TabIndex = 0;
			this->label_noproperties->Text = "No properties for this channel";
			// 
			// tabControl_emit
			// 
			this->tabControl_emit->Controls->Add(this->tabPage_emit);
			this->tabControl_emit->Location = System::Drawing::Point(8, 8);
			this->tabControl_emit->Name = "tabControl_emit";
			this->tabControl_emit->SelectedIndex = 0;
			this->tabControl_emit->Size = System::Drawing::Size(272, 104);
			this->tabControl_emit->TabIndex = 1;
			// 
			// tabPage_emit
			// 
			this->tabPage_emit->Controls->Add(this->label_noproperties);
			this->tabPage_emit->Location = System::Drawing::Point(4, 22);
			this->tabPage_emit->Name = "tabPage_emit";
			this->tabPage_emit->Size = System::Drawing::Size(264, 78);
			this->tabPage_emit->TabIndex = 0;
			this->tabPage_emit->Text = "Emit";
			// 
			// prtclDriverEmitForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(292, 125);
			this->Controls->Add(this->tabControl_emit);
			this->Name = "prtclDriverEmitForm";
			this->Text = "Particle Driver Emit Properties";
			this->TopMost = true;
			this->tabControl_emit->ResumeLayout(false);
			this->tabPage_emit->ResumeLayout(false);
			this->ResumeLayout(false);

		}

	private: void SetUpComponents()
			 {
				set_checkbox();
				fill_combo();
			 }
	private: System::Void checkBox_DriverToAnimLength_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 ////DBG_LOG1("checked %s", (checkBox_DriverToAnimLength->Checked ? "true":"false") );

				 //m_Driver.SetDriverToAnimLengthFlag( checkBox_DriverToAnimLength->Checked );

				 ////	check if the driver length needs to change.
				 //update_driver_length_from_anim_length();
			 }

	private: System::Void comboBox_AnimList_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
			 {
				////DBG_LOG2("selected %d %s", comboBox_AnimList->SelectedIndex, comboBox_AnimList->get_SelectedText() );

				//itString target;
				//tmaManagedStringUtils::ManagedStringToItString( comboBox_AnimList->get_SelectedText(), target );
				//m_Driver.SetAnimName( target );

				//DBG_LOG1( "selected index %d", comboBox_AnimList->SelectedIndex );

				//m_Driver.SetAnimIndex( comboBox_AnimList->SelectedIndex );

				// //	check if the driver length needs to change.
				// update_driver_length_from_anim_length();
			 }

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_driver_length_from_anim_length()
		{
			//// if this checkbox is checked, resize the driver length.
			////
			//if (   ( m_Driver.IsDriverToAnimLength() )
			//	&& ( m_Driver.GetAnimIndex() >= 0 ) )
			//{
			//	float anim_duration;
			//	anim_duration = m_Driver.Adapter().GetAnimationLength( m_Driver.GetAnimIndex() );

			//	m_Driver.SetEndTime( m_Driver.GetBeginTime() + anim_duration );

			//	//DBG_LOG1( "driver animfull duration  %6.2f", m_Driver.GetDuration() );
			//}
		};
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void set_checkbox()
		{
//			checkBox_DriverToAnimLength->set_Checked(true);
		};
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void fill_combo()
		{
			//prtclAdapterEmit::AnimMap anim_map;
			//m_Driver.Adapter().GetAnimationList( anim_map );

			//prtclAdapterEmit::AnimMap::iterator it	= anim_map.begin();
			//prtclAdapterEmit::AnimMap::iterator end	= anim_map.end();

			//while( it != end )
			//{
			//	DBG_LOG2( "combo %02d %s", it->first, it->second.c_str() );

			//	comboBox_AnimList->Items->Insert( it->first, gcnew System::String( it->second.c_str() ) );
			//	++it;
			//}
		};

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_emit;
		}
	};
}
#endif // _MANAGED
