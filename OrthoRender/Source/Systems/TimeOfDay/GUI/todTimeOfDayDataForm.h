/********************************************************************************************\
**  todTimeOfDayDataForm.h
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#pragma once

#ifndef TOD_TIMEOFDAYDATA_HPP
#include "todTimeOfDayData.hpp"
#endif
#ifndef TOD_OPERATIONS_HPP
#include "todOperations.hpp"
#endif

#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "tmaManagedConversionUtil.hpp"
#endif


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Globalization;	// for Convert
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace GeneratedForms
{
	/// <summary>
	/// Summary for todTimeOfDayDataForm
	///
	/// WARNING: If you change the name of this class, yada yada...
	/// </summary>
	public __gc class todTimeOfDayDataForm : public System::Windows::Forms::Form
	{
	public:
		static todTimeOfDayDataForm* FormInstance = 0;

		todTimeOfDayDataForm()
		{
			m_bDisableNotify = true;
			InitializeComponent();
			m_bDisableNotify = false;
		}

		// Call this to update dialog to new data
		void Update(const todTimeOfDayData &i_Data)
		{
			SetupData(i_Data);
		}

	protected:
		void Dispose(Boolean disposing)
		{
			// clear instance
			if (disposing && todTimeOfDayDataForm::FormInstance == this)
				todTimeOfDayDataForm::FormInstance = 0;

			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container* components;
	private: System::Windows::Forms::TabControl *  tabControl_tod;
	private: System::Windows::Forms::TabPage *  tabPage_tod;
	private: System::Windows::Forms::Label *  label_time;
	private: System::Windows::Forms::ComboBox *  comboBox_tod;
	private: System::Windows::Forms::RadioButton *  radioButton_auto;
	private: System::Windows::Forms::RadioButton *  radioButton_fixed;
	private: System::Windows::Forms::GroupBox *  groupBox_autoorfixed;
	private: System::Windows::Forms::Label *  label_lengthofday;

	private: System::Windows::Forms::TextBox *  textBox_LengthOfDay;
	private: System::Windows::Forms::CheckBox *  checkBox_enable;


	private: System::Windows::Forms::GroupBox *  groupBox_daycycle;













		// Holds reference to data, changing the data
		// within the caller's structure
	private: bool m_bDisableNotify;



		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl_tod = new System::Windows::Forms::TabControl();
			this->tabPage_tod = new System::Windows::Forms::TabPage();
			this->radioButton_fixed = new System::Windows::Forms::RadioButton();
			this->radioButton_auto = new System::Windows::Forms::RadioButton();
			this->groupBox_autoorfixed = new System::Windows::Forms::GroupBox();
			this->groupBox_daycycle = new System::Windows::Forms::GroupBox();
			this->textBox_LengthOfDay = new System::Windows::Forms::TextBox();
			this->label_lengthofday = new System::Windows::Forms::Label();
			this->label_time = new System::Windows::Forms::Label();
			this->comboBox_tod = new System::Windows::Forms::ComboBox();
			this->checkBox_enable = new System::Windows::Forms::CheckBox();
			this->tabControl_tod->SuspendLayout();
			this->tabPage_tod->SuspendLayout();
			this->groupBox_daycycle->SuspendLayout();
			this->SuspendLayout();
			//
			// tabControl_tod
			//
			this->tabControl_tod->Controls->Add(this->tabPage_tod);
			this->tabControl_tod->Location = System::Drawing::Point(8, 8);
			this->tabControl_tod->Name = S"tabControl_tod";
			this->tabControl_tod->SelectedIndex = 0;
			this->tabControl_tod->Size = System::Drawing::Size(352, 264);
			this->tabControl_tod->TabIndex = 3;
			//
			// tabPage_tod
			//
			this->tabPage_tod->Controls->Add(this->radioButton_fixed);
			this->tabPage_tod->Controls->Add(this->radioButton_auto);
			this->tabPage_tod->Controls->Add(this->groupBox_autoorfixed);
			this->tabPage_tod->Controls->Add(this->groupBox_daycycle);
			this->tabPage_tod->Location = System::Drawing::Point(4, 22);
			this->tabPage_tod->Name = S"tabPage_tod";
			this->tabPage_tod->Size = System::Drawing::Size(344, 238);
			this->tabPage_tod->TabIndex = 0;
			this->tabPage_tod->Text = S"TimeOfDay";
			//
			// radioButton_fixed
			//
			this->radioButton_fixed->Location = System::Drawing::Point(200, 24);
			this->radioButton_fixed->Name = S"radioButton_fixed";
			this->radioButton_fixed->Size = System::Drawing::Size(112, 24);
			this->radioButton_fixed->TabIndex = 21;
			this->radioButton_fixed->Text = S"fixed (no rotation)";
			this->radioButton_fixed->CheckedChanged += new System::EventHandler(this, radioButton_fixed_CheckedChanged);
			//
			// radioButton_auto
			//
			this->radioButton_auto->Location = System::Drawing::Point(40, 24);
			this->radioButton_auto->Name = S"radioButton_auto";
			this->radioButton_auto->Size = System::Drawing::Size(120, 24);
			this->radioButton_auto->TabIndex = 20;
			this->radioButton_auto->Text = S"automatic rotation";
			this->radioButton_auto->CheckedChanged += new System::EventHandler(this, radioButton_auto_CheckedChanged);
			//
			// groupBox_autoorfixed
			//
			this->groupBox_autoorfixed->Location = System::Drawing::Point(8, 8);
			this->groupBox_autoorfixed->Name = S"groupBox_autoorfixed";
			this->groupBox_autoorfixed->Size = System::Drawing::Size(328, 48);
			this->groupBox_autoorfixed->TabIndex = 22;
			this->groupBox_autoorfixed->TabStop = false;
			this->groupBox_autoorfixed->Text = S"Light Source Rotation";
			//
			// groupBox_daycycle
			//
			this->groupBox_daycycle->Controls->Add(this->textBox_LengthOfDay);
			this->groupBox_daycycle->Controls->Add(this->label_lengthofday);
			this->groupBox_daycycle->Controls->Add(this->label_time);
			this->groupBox_daycycle->Controls->Add(this->comboBox_tod);
			this->groupBox_daycycle->Controls->Add(this->checkBox_enable);
			this->groupBox_daycycle->Location = System::Drawing::Point(8, 64);
			this->groupBox_daycycle->Name = S"groupBox_daycycle";
			this->groupBox_daycycle->Size = System::Drawing::Size(328, 128);
			this->groupBox_daycycle->TabIndex = 29;
			this->groupBox_daycycle->TabStop = false;
			this->groupBox_daycycle->Text = S"Day Cycle";
			//
			// textBox_LengthOfDay
			//
			this->textBox_LengthOfDay->Location = System::Drawing::Point(184, 96);
			this->textBox_LengthOfDay->Name = S"textBox_LengthOfDay";
			this->textBox_LengthOfDay->Size = System::Drawing::Size(64, 20);
			this->textBox_LengthOfDay->TabIndex = 25;
			this->textBox_LengthOfDay->Text = S"";
			this->textBox_LengthOfDay->TextChanged += new System::EventHandler(this, textBox_LengthOfDay_TextChanged);
			//
			// label_lengthofday
			//
			this->label_lengthofday->Location = System::Drawing::Point(72, 96);
			this->label_lengthofday->Name = S"label_lengthofday";
			this->label_lengthofday->Size = System::Drawing::Size(104, 16);
			this->label_lengthofday->TabIndex = 23;
			this->label_lengthofday->Text = S"Length of Day (min)";
			//
			// label_time
			//
			this->label_time->Location = System::Drawing::Point(56, 24);
			this->label_time->Name = S"label_time";
			this->label_time->Size = System::Drawing::Size(73, 15);
			this->label_time->TabIndex = 10;
			this->label_time->Text = S"Time of Day";
			//
			// comboBox_tod
			//
			this->comboBox_tod->AllowDrop = true;
			this->comboBox_tod->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			System::Object* __mcTemp__1[] = new System::Object*[24];
			__mcTemp__1[0] = S"12:00 AM (midnight)";
			__mcTemp__1[1] = S"01:00 AM";
			__mcTemp__1[2] = S"02:00 AM";
			__mcTemp__1[3] = S"03:00 AM";
			__mcTemp__1[4] = S"04:00 AM";
			__mcTemp__1[5] = S"05:00 AM";
			__mcTemp__1[6] = S"06:00 AM";
			__mcTemp__1[7] = S"07:00 AM";
			__mcTemp__1[8] = S"08:00 AM";
			__mcTemp__1[9] = S"09:00 AM";
			__mcTemp__1[10] = S"10:00 AM";
			__mcTemp__1[11] = S"11:00 AM";
			__mcTemp__1[12] = S"12:00 PM (noon)";
			__mcTemp__1[13] = S"01:00 PM";
			__mcTemp__1[14] = S"02:00 PM";
			__mcTemp__1[15] = S"03:00 PM";
			__mcTemp__1[16] = S"04:00 PM";
			__mcTemp__1[17] = S"05:00 PM";
			__mcTemp__1[18] = S"06:00 PM";
			__mcTemp__1[19] = S"07:00 PM";
			__mcTemp__1[20] = S"08:00 PM";
			__mcTemp__1[21] = S"09:00 PM";
			__mcTemp__1[22] = S"10:00 PM";
			__mcTemp__1[23] = S"11:00 PM";
			this->comboBox_tod->Items->AddRange(__mcTemp__1);
			this->comboBox_tod->Location = System::Drawing::Point(136, 24);
			this->comboBox_tod->Name = S"comboBox_tod";
			this->comboBox_tod->Size = System::Drawing::Size(144, 21);
			this->comboBox_tod->TabIndex = 11;
			this->comboBox_tod->SelectedIndexChanged += new System::EventHandler(this, comboBox_tod_SelectedIndexChanged);
			//
			// checkBox_enable
			//
			this->checkBox_enable->Location = System::Drawing::Point(112, 56);
			this->checkBox_enable->Name = S"checkBox_enable";
			this->checkBox_enable->TabIndex = 26;
			this->checkBox_enable->Text = S"Enable Rotation";
			this->checkBox_enable->CheckedChanged += new System::EventHandler(this, checkBox_enable_CheckedChanged);
			//
			// todTimeOfDayDataForm
			//
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(368, 277);
			this->Controls->Add(this->tabControl_tod);
			this->Name = S"todTimeOfDayDataForm";
			this->Text = S"TimeOfDay Data Form";
			this->tabControl_tod->ResumeLayout(false);
			this->tabPage_tod->ResumeLayout(false);
			this->groupBox_daycycle->ResumeLayout(false);
			this->ResumeLayout(false);

		}

		//

		void SetupData(const todTimeOfDayData &i_Data)
		{
			m_bDisableNotify = true;
			checkBox_enable->set_Checked( i_Data.m_bEnableAutoRotation.GetValue() );
			textBox_LengthOfDay->set_Text( Convert::ToString( (int)i_Data.m_fLengthOfDay.GetValue() ) );
			radioButton_fixed->set_Checked( !i_Data.m_bEnableAutoRotation.GetValue() );
			radioButton_auto->set_Checked( i_Data.m_bEnableAutoRotation.GetValue() );
			comboBox_tod->set_SelectedIndex( (int)i_Data.m_fStartTimeOfDay.GetValue() );
			m_bDisableNotify = false;
		}

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage * GetTabPage( int i_Index )
		{
			return tabPage_tod;
		}

private: System::Void radioButton_auto_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
		 {
 			if (!m_bDisableNotify)
			{
				//radioButton_fixed->set_Checked( !(radioButton_auto->get_Checked()) );

				todOperations::ChangeTimeOfDayAutoOrFixed(radioButton_auto->get_Checked());
			}
		 }

private: System::Void radioButton_fixed_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
		 {
 			if (!m_bDisableNotify)
			{
				//radioButton_auto->set_Checked( !(radioButton_fixed->get_Checked()) );

				//NOTE: we shouldn't need this because with each change of one changes the other.
				todOperations::ChangeTimeOfDayAutoOrFixed( !radioButton_fixed->get_Checked() );
			}
		 }

private: System::Void textBox_LengthOfDay_TextChanged(System::Object *  sender, System::EventArgs *  e)
		 {
 			if (!m_bDisableNotify)
			{
				float length = 0.0f;

				if ( textBox_LengthOfDay->get_Text()->get_Length() > 0 )
				{
					try
					{
						length = Convert::ToSingle( textBox_LengthOfDay->get_Text() );
					}
					catch (System::OverflowException*)
					{
						length = 0.0f;
					}
					catch (System::FormatException*)
					{
						length = 0.0f;
					}
					catch (System::ArgumentNullException*)
					{
						length = 0.0f;
					}
				}
				todOperations::ChangeTimeOfDayLength( length );
			}
		 }

private: System::Void comboBox_tod_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			if (!m_bDisableNotify)
			{
				todOperations::ChangeTimeOfDayStartTimeOfDay( (float)comboBox_tod->get_SelectedIndex() );
			}
		 }

private: System::Void checkBox_enable_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
		 {
 			if (!m_bDisableNotify)
			{
				todOperations::ChangeTimeOfDayEnabled( checkBox_enable->get_Checked() );
			}
		 }
};
}
