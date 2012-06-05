#pragma once

#ifndef RCD_DRIVERFLOAT_HPP
#include "Record/Float/rcdDriverFloat.hpp"
#endif
#ifndef RCD_DRIVERFLOATINFO_HPP
#include "Record/Float/rcdDriverFloatInfo.hpp"
#endif
#ifndef RCD_RECORDUTIL_HPP
#include "Record/rcdRecordUtil.hpp"
#endif
#ifndef TMLN_CHANNELRANGEDFLOAT_HPP
#include "Support/tmln/tmlnChannelRangedFloat.hpp"
#endif
#ifndef TMLN_TIMELINE_HPP
#include "Support/tmln/tmlnTimeLine.hpp"
#endif

#ifdef _MANAGED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace StudioFramework
{
	/// <summary> 
	/// Summary for rcdDriverFloatForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class rcdDriverFloatForm : public System::Windows::Forms::Form
	{
	public: 
		static rcdDriverFloatForm^ FormInstance = nullptr;

		rcdDriverFloatForm(rcdDriverFloat &i_Driver)
			: m_Driver(i_Driver), m_SliderScale(1), m_bRecording(false)
		{
			m_bDisableNotify = true;
			InitializeComponent();
	
			const tmlnChannelRangedFloat &channel = m_Driver.GetChannel();
			set_range(channel.GetMinValue(), channel.GetMaxValue());
			
			float time = tmlnTimeLine::GetValue();
			Update(time);

			m_bDisableNotify = false;
		}

		void Update(float i_Time)
		{
			m_bDisableNotify = true;

			if (!m_bRecording)
			{
				float value = m_Driver.GetValue(i_Time);
				this->trackBarValue->Value = (int)(value * m_SliderScale);
				this->textValue->Text = System::String::Format("{0}", (value));
			}
			else
			{
				float value = this->trackBarValue->Value / m_SliderScale;
				m_Driver.SetValue(i_Time, value);
				this->textValue->Text = System::String::Format("{0}", (value));
			}

			m_bDisableNotify = false;
		}	
        
    	//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage()
		{
			return this->tabPage_rcd;
		}

	protected: 
		~rcdDriverFloatForm()
		{
			if (FormInstance == this)
				FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::TrackBar ^  trackBarValue;
	private: System::Windows::Forms::TextBox ^  textValue;
	private: System::Windows::Forms::TextBox ^  textMin;
	private: System::Windows::Forms::TextBox ^  textMax;
	private: System::Windows::Forms::CheckBox ^  checkRecord;
	private: System::Windows::Forms::Label ^  label_value;
	private: System::Windows::Forms::Label ^  label_min;
	private: System::Windows::Forms::Label ^  label_max;
	private: System::Windows::Forms::TabControl ^  tabControl_rcd;
	private: System::Windows::Forms::TabPage ^  tabPage_rcd;

	private: rcdDriverFloat &m_Driver;
	private: bool m_bDisableNotify;
	private: float m_SliderScale;
	private: bool m_bRecording;

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
			this->label_value = gcnew System::Windows::Forms::Label();
			this->trackBarValue = gcnew System::Windows::Forms::TrackBar();
			this->textValue = gcnew System::Windows::Forms::TextBox();
			this->textMin = gcnew System::Windows::Forms::TextBox();
			this->textMax = gcnew System::Windows::Forms::TextBox();
			this->label_min = gcnew System::Windows::Forms::Label();
			this->label_max = gcnew System::Windows::Forms::Label();
			this->checkRecord = gcnew System::Windows::Forms::CheckBox();
			this->tabControl_rcd = gcnew System::Windows::Forms::TabControl();
			this->tabPage_rcd = gcnew System::Windows::Forms::TabPage();
			(safe_cast<System::ComponentModel::ISupportInitialize ^  >(this->trackBarValue))->BeginInit();
			this->tabControl_rcd->SuspendLayout();
			this->tabPage_rcd->SuspendLayout();
			this->SuspendLayout();
			// 
			// label_value
			// 
			this->label_value->Location = System::Drawing::Point(16, 24);
			this->label_value->Name = "label_value";
			this->label_value->Size = System::Drawing::Size(60, 28);
			this->label_value->TabIndex = 2;
			this->label_value->Text = "Value:";
			// 
			// trackBarValue
			// 
			this->trackBarValue->Location = System::Drawing::Point(80, 16);
			this->trackBarValue->Maximum = 100;
			this->trackBarValue->Name = "trackBarValue";
			this->trackBarValue->Size = System::Drawing::Size(193, 45);
			this->trackBarValue->TabIndex = 3;
			this->trackBarValue->TickFrequency = 5;
			this->trackBarValue->ValueChanged += gcnew System::EventHandler(this, &rcdDriverFloatForm::trackBarValue_ValueChanged);
			// 
			// textValue
			// 
			this->textValue->Location = System::Drawing::Point(288, 24);
			this->textValue->Name = "textValue";
			this->textValue->Size = System::Drawing::Size(46, 20);
			this->textValue->TabIndex = 4;
			this->textValue->Text = "";
			this->textValue->TextChanged += gcnew System::EventHandler(this, &rcdDriverFloatForm::textValue_TextChanged);
			// 
			// textMin
			// 
			this->textMin->Location = System::Drawing::Point(72, 64);
			this->textMin->Name = "textMin";
			this->textMin->Size = System::Drawing::Size(54, 20);
			this->textMin->TabIndex = 5;
			this->textMin->Text = "";
			this->textMin->Leave += gcnew System::EventHandler(this, &rcdDriverFloatForm::textMin_Leave);
			// 
			// textMax
			// 
			this->textMax->Location = System::Drawing::Point(248, 64);
			this->textMax->Name = "textMax";
			this->textMax->Size = System::Drawing::Size(46, 20);
			this->textMax->TabIndex = 6;
			this->textMax->Text = "";
			this->textMax->TextChanged += gcnew System::EventHandler(this, &rcdDriverFloatForm::textMax_TextChanged);
			// 
			// label_min
			// 
			this->label_min->Location = System::Drawing::Point(40, 64);
			this->label_min->Name = "label_min";
			this->label_min->Size = System::Drawing::Size(33, 21);
			this->label_min->TabIndex = 7;
			this->label_min->Text = "Min:";
			// 
			// label_max
			// 
			this->label_max->Location = System::Drawing::Point(216, 64);
			this->label_max->Name = "label_max";
			this->label_max->Size = System::Drawing::Size(34, 21);
			this->label_max->TabIndex = 8;
			this->label_max->Text = "Max:";
			// 
			// checkRecord
			// 
			this->checkRecord->Location = System::Drawing::Point(16, 96);
			this->checkRecord->Name = "checkRecord";
			this->checkRecord->Size = System::Drawing::Size(80, 21);
			this->checkRecord->TabIndex = 9;
			this->checkRecord->Text = "Record";
			this->checkRecord->CheckedChanged += gcnew System::EventHandler(this, &rcdDriverFloatForm::checkRecord_CheckedChanged);
			// 
			// tabControl_rcd
			// 
			this->tabControl_rcd->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_rcd->Controls->Add(this->tabPage_rcd);
			this->tabControl_rcd->Location = System::Drawing::Point(0, 0);
			this->tabControl_rcd->Name = "tabControl_rcd";
			this->tabControl_rcd->SelectedIndex = 0;
			this->tabControl_rcd->Size = System::Drawing::Size(360, 152);
			this->tabControl_rcd->TabIndex = 10;
			// 
			// tabPage_rcd
			// 
			this->tabPage_rcd->Controls->Add(this->textMax);
			this->tabPage_rcd->Controls->Add(this->label_min);
			this->tabPage_rcd->Controls->Add(this->label_max);
			this->tabPage_rcd->Controls->Add(this->checkRecord);
			this->tabPage_rcd->Controls->Add(this->label_value);
			this->tabPage_rcd->Controls->Add(this->textValue);
			this->tabPage_rcd->Controls->Add(this->trackBarValue);
			this->tabPage_rcd->Controls->Add(this->textMin);
			this->tabPage_rcd->Location = System::Drawing::Point(4, 22);
			this->tabPage_rcd->Name = "tabPage_rcd";
			this->tabPage_rcd->Size = System::Drawing::Size(352, 126);
			this->tabPage_rcd->TabIndex = 0;
			this->tabPage_rcd->Text = "Record";
			// 
			// rcdDriverFloatForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(360, 150);
			this->Controls->Add(this->tabControl_rcd);
			this->Name = "rcdDriverFloatForm";
			this->Text = "Slider Recording";
			(safe_cast<System::ComponentModel::ISupportInitialize ^  >(this->trackBarValue))->EndInit();
			this->tabControl_rcd->ResumeLayout(false);
			this->tabPage_rcd->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

		void set_range( float i_MinValue, float i_MaxValue )
		{
			m_bDisableNotify = true;
			this->textMin->Text = System::String::Format("{0}", (i_MinValue));
			this->textMax->Text = System::String::Format("{0}", (i_MaxValue));

			DBG_ASSERT0(i_MaxValue > i_MinValue, "Min and Max values not valid")
		
			m_SliderScale = 100.0f / (i_MaxValue - i_MinValue);
			this->trackBarValue->Minimum = (int)(i_MinValue * m_SliderScale);
			this->trackBarValue->Maximum = (int)(i_MaxValue * m_SliderScale);
			m_bDisableNotify = false;
		}

		void set_value( float i_Value )
		{
			float time = tmlnTimeLine::GetValue();
			m_Driver.SetValue(time, i_Value);
			this->Update(time);
		}

	private: System::Void textValue_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 if (!m_bDisableNotify)
				 {
					 m_bDisableNotify = true;
					try
					{
						if (this->textValue->Text->Length > 0)
						{
							float value = (float) System::Double::Parse(this->textValue->Text);
							set_value( value );
						}
					}
					catch (System::FormatException^)
					{
					}
					m_bDisableNotify = false;
				 }
			 }

private: System::Void trackBarValue_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				float value = this->trackBarValue->Value / m_SliderScale;
				set_value( value );
				m_bDisableNotify = false;
			}
		 }

private: System::Void checkRecord_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				this->m_bRecording = this->checkRecord->Checked;
				if (this->m_bRecording)
					rcdRecordUtil::StartRecording(m_Driver.GetBeginTime(), m_Driver.GetEndTime());
				else
					rcdRecordUtil::StopRecording();
				m_bDisableNotify = false;
			}
		 }

		// Get min and max range values and update trackbar
		 void range_changed()
		 {
			try
			{
				if (this->textMin->Text->Length > 0 && this->textMax->Text->Length > 0)
				{
					float min = (float) System::Double::Parse(this->textMin->Text);
					float max = (float) System::Double::Parse(this->textMax->Text);
					if (max > min)
					{
						m_Driver.Channel().SetMinValue(min);
						m_Driver.Channel().SetMaxValue(max);
						set_range( min, max );
					}
					else
						MessageBox::Show("Max value must be greater than min value");
				}
			}
			catch (System::FormatException^)
			{
			}
				 
		 }

private: System::Void textMin_Leave(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				range_changed();
				m_bDisableNotify = false;
			}
		 }

private: System::Void textMax_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				range_changed();
				m_bDisableNotify = false;
			}
		 }

};
}
#endif // _MANAGED
