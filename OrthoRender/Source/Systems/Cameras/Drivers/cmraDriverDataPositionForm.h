#pragma once

#ifndef CMRA_DRIVERDATAPOSITIONINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataPositionInfo.hpp"
#endif
#ifndef CMRA_OBJECTMGR_HPP
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#endif

#ifndef DBG_LOG_HPP
#include "Core/Dbg/dbgLog.hpp"
#endif
#ifndef NAME_MGR_HPP
#include "Core/name/nameMgr.hpp"
#endif
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef NAME_TYPES_HPP
#include "Core/name/nameTypes.hpp"
#endif
#ifndef TMLN_TIMELINE_HPP
#include "Support/tmln/tmlnTimeLine.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif

#include "Systems/Cameras/Timeline/cmraRefNameForm.h"
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
	/// Summary for cmraDriverDataPositionForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmraDriverDataPositionForm : public System::Windows::Forms::Form
	{
	public: 
		static cmraDriverDataPositionForm ^FormInstance = nullptr;
	public: 
		cmraDriverDataPositionForm(cmraDriverDataPositionInfo &i_Data)
			: m_Data(i_Data)
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
		~cmraDriverDataPositionForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: cmraDriverDataPositionInfo &m_Data;
	private: bool m_bDisableNotify;

	private: System::Windows::Forms::TabPage ^  tabPage_cameraposition;
	private: System::Windows::Forms::TabControl ^  tabControl_cameraposition;

	private: System::Windows::Forms::Label ^  label_distance;
	private: System::Windows::Forms::Label ^  label_direction;
	private: System::Windows::Forms::Label ^  label_angle;
	private: System::Windows::Forms::Label ^  label_timedelta;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_pitch;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_yaw;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_distance;
	private: System::Windows::Forms::TextBox ^  textBox_deltatime;

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
			this->tabPage_cameraposition = gcnew System::Windows::Forms::TabPage();
			this->label_timedelta = gcnew System::Windows::Forms::Label();
			this->textBox_deltatime = gcnew System::Windows::Forms::TextBox();
			this->label_angle = gcnew System::Windows::Forms::Label();
			this->rangedFloat_pitch = gcnew TerawattManagedControls::RangedFloat();
			this->label_distance = gcnew System::Windows::Forms::Label();
			this->rangedFloat_distance = gcnew TerawattManagedControls::RangedFloat();
			this->label_direction = gcnew System::Windows::Forms::Label();
			this->rangedFloat_yaw = gcnew TerawattManagedControls::RangedFloat();
			this->tabControl_cameraposition = gcnew System::Windows::Forms::TabControl();
			this->tabPage_cameraposition->SuspendLayout();
			this->tabControl_cameraposition->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabPage_cameraposition
			// 
			this->tabPage_cameraposition->Controls->Add(this->label_timedelta);
			this->tabPage_cameraposition->Controls->Add(this->textBox_deltatime);
			this->tabPage_cameraposition->Controls->Add(this->label_angle);
			this->tabPage_cameraposition->Controls->Add(this->rangedFloat_pitch);
			this->tabPage_cameraposition->Controls->Add(this->label_distance);
			this->tabPage_cameraposition->Controls->Add(this->rangedFloat_distance);
			this->tabPage_cameraposition->Controls->Add(this->label_direction);
			this->tabPage_cameraposition->Controls->Add(this->rangedFloat_yaw);
			this->tabPage_cameraposition->Location = System::Drawing::Point(4, 22);
			this->tabPage_cameraposition->Name = "tabPage_cameraposition";
			this->tabPage_cameraposition->Size = System::Drawing::Size(416, 238);
			this->tabPage_cameraposition->TabIndex = 0;
			this->tabPage_cameraposition->Text = "Position";
			this->tabPage_cameraposition->ToolTipText = "Camera Focus Position";
			// 
			// label_timedelta
			// 
			this->label_timedelta->Location = System::Drawing::Point(56, 146);
			this->label_timedelta->Name = "label_timedelta";
			this->label_timedelta->Size = System::Drawing::Size(184, 16);
			this->label_timedelta->TabIndex = 15;
			this->label_timedelta->Text = "Time Offset From Driver Start Time";
			// 
			// textBox_deltatime
			// 
			this->textBox_deltatime->Enabled = false;
			this->textBox_deltatime->Location = System::Drawing::Point(248, 144);
			this->textBox_deltatime->Name = "textBox_deltatime";
			this->textBox_deltatime->TabIndex = 14;
			this->textBox_deltatime->Text = "";
			// 
			// label_angle
			// 
			this->label_angle->Location = System::Drawing::Point(8, 96);
			this->label_angle->Name = "label_angle";
			this->label_angle->Size = System::Drawing::Size(96, 19);
			this->label_angle->TabIndex = 13;
			this->label_angle->Text = "Angle (pitch)";
			// 
			// rangedFloat_pitch
			// 
			this->rangedFloat_pitch->Exponent = (System::Int16)1;
			this->rangedFloat_pitch->Location = System::Drawing::Point(112, 88);
			this->rangedFloat_pitch->Maximum = 90;
			this->rangedFloat_pitch->Name = "rangedFloat_pitch";
			this->rangedFloat_pitch->NumTicks = (System::Int16)100;
			this->rangedFloat_pitch->Precision = (System::Int16)2;
			this->rangedFloat_pitch->Size = System::Drawing::Size(280, 24);
			this->rangedFloat_pitch->TabIndex = 12;
			this->rangedFloat_pitch->Value = 45;
			this->rangedFloat_pitch->ValueChanged += gcnew System::EventHandler(this, &cmraDriverDataPositionForm::rangedFloat_pitch_ValueChanged);
			// 
			// label_distance
			// 
			this->label_distance->Location = System::Drawing::Point(8, 56);
			this->label_distance->Name = "label_distance";
			this->label_distance->Size = System::Drawing::Size(96, 19);
			this->label_distance->TabIndex = 11;
			this->label_distance->Text = "Distance (radius)";
			// 
			// rangedFloat_distance
			// 
			this->rangedFloat_distance->Exponent = (System::Int16)1;
			this->rangedFloat_distance->Location = System::Drawing::Point(112, 48);
			this->rangedFloat_distance->Maximum = 100;
			this->rangedFloat_distance->Name = "rangedFloat_distance";
			this->rangedFloat_distance->NumTicks = (System::Int16)100;
			this->rangedFloat_distance->Precision = (System::Int16)2;
			this->rangedFloat_distance->Size = System::Drawing::Size(280, 24);
			this->rangedFloat_distance->TabIndex = 10;
			this->rangedFloat_distance->Value = 20;
			this->rangedFloat_distance->ValueChanged += gcnew System::EventHandler(this, &cmraDriverDataPositionForm::rangedFloat_distance_ValueChanged);
			// 
			// label_direction
			// 
			this->label_direction->Location = System::Drawing::Point(8, 16);
			this->label_direction->Name = "label_direction";
			this->label_direction->Size = System::Drawing::Size(96, 19);
			this->label_direction->TabIndex = 8;
			this->label_direction->Text = "Direction (yaw)";
			// 
			// rangedFloat_yaw
			// 
			this->rangedFloat_yaw->Exponent = (System::Int16)1;
			this->rangedFloat_yaw->Location = System::Drawing::Point(112, 8);
			this->rangedFloat_yaw->Maximum = 180;
			this->rangedFloat_yaw->Minimum = 180;
			this->rangedFloat_yaw->Name = "rangedFloat_yaw";
			this->rangedFloat_yaw->NumTicks = (System::Int16)100;
			this->rangedFloat_yaw->Precision = (System::Int16)2;
			this->rangedFloat_yaw->Size = System::Drawing::Size(280, 24);
			this->rangedFloat_yaw->TabIndex = 7;
			this->rangedFloat_yaw->ValueChanged += gcnew System::EventHandler(this, &cmraDriverDataPositionForm::rangedFloat_yaw_ValueChanged);
			// 
			// tabControl_cameraposition
			// 
			this->tabControl_cameraposition->Controls->Add(this->tabPage_cameraposition);
			this->tabControl_cameraposition->ItemSize = System::Drawing::Size(59, 18);
			this->tabControl_cameraposition->Location = System::Drawing::Point(8, 8);
			this->tabControl_cameraposition->Name = "tabControl_cameraposition";
			this->tabControl_cameraposition->SelectedIndex = 0;
			this->tabControl_cameraposition->Size = System::Drawing::Size(424, 264);
			this->tabControl_cameraposition->TabIndex = 7;
			// 
			// cmraDriverDataPositionForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(440, 285);
			this->Controls->Add(this->tabControl_cameraposition);
			this->Name = "cmraDriverDataPositionForm";
			this->Text = "Camera Position Properties";
			this->TopMost = true;
			this->tabPage_cameraposition->ResumeLayout(false);
			this->tabControl_cameraposition->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

private: System::Void rangedFloat_yaw_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				Set_Data();
				m_bDisableNotify = false;
			}
		 }

private: System::Void rangedFloat_distance_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				Set_Data();
				m_bDisableNotify = false;
			}
		 }

private: System::Void rangedFloat_pitch_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				Set_Data();
				m_bDisableNotify = false;
			}
		 }

private: System::Void Set_Data()
		 {
			//	set the timeline to the correct time before updating
			//
			tmlnTimeLine::SetValue( m_Data.m_fBeginTime + m_Data.m_fDeltaFromStartTime );

			//	set the data
			//
			m_Data.m_fPitchAngle	= (float) rangedFloat_pitch->Value;
			m_Data.m_fYawAngle		= (float) rangedFloat_yaw->Value;
			m_Data.m_fDistance		= (float) rangedFloat_distance->Value;
		 }

private: System::Void SetUpComponents()
		 {
			rangedFloat_pitch->Value	= m_Data.m_fPitchAngle;
			rangedFloat_yaw->Value		= m_Data.m_fYawAngle;
			rangedFloat_distance->Value	= m_Data.m_fDistance;
		 }

public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_cameraposition;
		}

public:
	System::Void SetPitchValue( float i_fValue )
	{
		rangedFloat_pitch->Value = i_fValue;
		Set_Data();
	}
	System::Void SetPitchConstraints( float i_fMin, float i_fMax )
	{
		rangedFloat_pitch->Minimum = i_fMin;
		rangedFloat_pitch->Maximum = i_fMax;
		Set_Data();
	}
	System::Void SetYawValue( float i_fValue )
	{
		rangedFloat_yaw->Value = i_fValue;
		Set_Data();
	}
	System::Void SetYawConstraints( float i_fMin, float i_fMax )
	{
		rangedFloat_yaw->Minimum = i_fMin;
		rangedFloat_yaw->Maximum = i_fMax;
		Set_Data();
	}
	System::Void SetDistanceValue( float i_fValue )
	{
		rangedFloat_distance->Value = i_fValue;
		Set_Data();
	}
	System::Void SetDistanceConstraints( float i_fMin, float i_fMax )
	{
		rangedFloat_distance->Minimum = i_fMin;
		rangedFloat_distance->Maximum = i_fMax;
		Set_Data();
	}
};
}
#endif // _MANAGED
