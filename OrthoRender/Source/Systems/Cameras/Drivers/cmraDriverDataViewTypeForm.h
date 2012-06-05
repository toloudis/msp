#pragma once

#ifndef CMRA_DRIVERDATAVIEWTYPEINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataViewTypeInfo.hpp"
#endif
#ifndef CMRA_OBJECTMGR_HPP
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#endif

#ifndef DBG_LOG_HPP
#include "Core/Dbg/dbgLog.hpp"
#endif
#ifndef MA_CONSTANTS_HPP
#include "Core/Ma/maConstants.hpp"
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
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif

#include "Systems/Cameras/Timeline/cmraRefNameForm.h"
#include "Systems/Cameras/Drivers/cmraDriverDataPositionForm.h"

#ifdef _MANAGED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace StudioFramework
{
	enum eCameraViewTypes
	{
		e_ExtremeCloseUp	= 0x0001,
		e_CloseUp			= 0x0002,
		e_Medium			= 0x0004,
		e_MedLong			= 0x0008,
		e_Long				= 0x0010,
		e_Overhead			= 0x0020,
		e_Aerial			= 0x0040,
		e_Arc				= 0x0080,
		e_AllViewTypes		= 0x8888
	};

	/// <summary> 
	/// Summary for cmraDriverDataViewTypeForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmraDriverDataViewTypeForm : public System::Windows::Forms::Form
	{
	public: 
		static cmraDriverDataViewTypeForm ^FormInstance = nullptr;
	public: 
		cmraDriverDataViewTypeForm(cmraDriverDataViewTypeInfo &i_Data)
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
		~cmraDriverDataViewTypeForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: cmraDriverDataViewTypeInfo &m_Data;
	private: bool m_bDisableNotify;
	private: StudioFramework::cmraDriverDataPositionForm ^ m_pPForm;
	private: System::Windows::Forms::TabPage ^  tabPage_camerasubject;
	private: System::Windows::Forms::TabControl ^  tabControl_target;

	private: System::Windows::Forms::RadioButton ^  radioButton_shot_extremecloseup;
	private: System::Windows::Forms::RadioButton ^  radioButton_shot_closeup;
	private: System::Windows::Forms::RadioButton ^  radioButton_shot_medium;
	private: System::Windows::Forms::RadioButton ^  radioButton_shot_mediumlong;
	private: System::Windows::Forms::RadioButton ^  radioButton_shot_long;
	private: System::Windows::Forms::RadioButton ^  radioButton_shot_overhead;
	private: System::Windows::Forms::RadioButton ^  radioButton_shot_aerial;
	private: System::Windows::Forms::GroupBox ^  groupBox_shottype;

	private: System::Windows::Forms::Label ^  label_tolerance;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_viewtolerance;
	private: System::Windows::Forms::Label ^  label_viewtolerance_desc;

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
			this->tabPage_camerasubject = gcnew System::Windows::Forms::TabPage();
			this->label_viewtolerance_desc = gcnew System::Windows::Forms::Label();
			this->groupBox_shottype = gcnew System::Windows::Forms::GroupBox();
			this->radioButton_shot_aerial = gcnew System::Windows::Forms::RadioButton();
			this->radioButton_shot_overhead = gcnew System::Windows::Forms::RadioButton();
			this->radioButton_shot_long = gcnew System::Windows::Forms::RadioButton();
			this->radioButton_shot_mediumlong = gcnew System::Windows::Forms::RadioButton();
			this->radioButton_shot_medium = gcnew System::Windows::Forms::RadioButton();
			this->radioButton_shot_closeup = gcnew System::Windows::Forms::RadioButton();
			this->radioButton_shot_extremecloseup = gcnew System::Windows::Forms::RadioButton();
			this->label_tolerance = gcnew System::Windows::Forms::Label();
			this->rangedFloat_viewtolerance = gcnew TerawattManagedControls::RangedFloat();
			this->tabControl_target = gcnew System::Windows::Forms::TabControl();
			this->tabPage_camerasubject->SuspendLayout();
			this->groupBox_shottype->SuspendLayout();
			this->tabControl_target->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabPage_camerasubject
			// 
			this->tabPage_camerasubject->Controls->Add(this->label_viewtolerance_desc);
			this->tabPage_camerasubject->Controls->Add(this->groupBox_shottype);
			this->tabPage_camerasubject->Controls->Add(this->label_tolerance);
			this->tabPage_camerasubject->Controls->Add(this->rangedFloat_viewtolerance);
			this->tabPage_camerasubject->Location = System::Drawing::Point(4, 22);
			this->tabPage_camerasubject->Name = "tabPage_camerasubject";
			this->tabPage_camerasubject->Size = System::Drawing::Size(416, 238);
			this->tabPage_camerasubject->TabIndex = 0;
			this->tabPage_camerasubject->Text = "ViewType";
			this->tabPage_camerasubject->ToolTipText = "Camera Focus ViewType";
			// 
			// label_viewtolerance_desc
			// 
			this->label_viewtolerance_desc->Location = System::Drawing::Point(128, 200);
			this->label_viewtolerance_desc->Name = "label_viewtolerance_desc";
			this->label_viewtolerance_desc->Size = System::Drawing::Size(200, 32);
			this->label_viewtolerance_desc->TabIndex = 9;
			this->label_viewtolerance_desc->Text = "0.0 = keep subject(s) in center,          1.0 = let subject(s) go to edges of vie" 
				"w";
			// 
			// groupBox_shottype
			// 
			this->groupBox_shottype->Controls->Add(this->radioButton_shot_aerial);
			this->groupBox_shottype->Controls->Add(this->radioButton_shot_overhead);
			this->groupBox_shottype->Controls->Add(this->radioButton_shot_long);
			this->groupBox_shottype->Controls->Add(this->radioButton_shot_mediumlong);
			this->groupBox_shottype->Controls->Add(this->radioButton_shot_medium);
			this->groupBox_shottype->Controls->Add(this->radioButton_shot_closeup);
			this->groupBox_shottype->Controls->Add(this->radioButton_shot_extremecloseup);
			this->groupBox_shottype->Location = System::Drawing::Point(8, 8);
			this->groupBox_shottype->Name = "groupBox_shottype";
			this->groupBox_shottype->Size = System::Drawing::Size(400, 152);
			this->groupBox_shottype->TabIndex = 1;
			this->groupBox_shottype->TabStop = false;
			this->groupBox_shottype->Text = "Shot Type";
			// 
			// radioButton_shot_aerial
			// 
			this->radioButton_shot_aerial->Location = System::Drawing::Point(224, 88);
			this->radioButton_shot_aerial->Name = "radioButton_shot_aerial";
			this->radioButton_shot_aerial->Size = System::Drawing::Size(120, 24);
			this->radioButton_shot_aerial->TabIndex = 6;
			this->radioButton_shot_aerial->Text = "Aerial";
			this->radioButton_shot_aerial->CheckedChanged += gcnew System::EventHandler(this, &cmraDriverDataViewTypeForm::radioButton_shot_aerial_CheckedChanged);
			// 
			// radioButton_shot_overhead
			// 
			this->radioButton_shot_overhead->Location = System::Drawing::Point(224, 56);
			this->radioButton_shot_overhead->Name = "radioButton_shot_overhead";
			this->radioButton_shot_overhead->Size = System::Drawing::Size(120, 24);
			this->radioButton_shot_overhead->TabIndex = 5;
			this->radioButton_shot_overhead->Text = "Overhead";
			this->radioButton_shot_overhead->CheckedChanged += gcnew System::EventHandler(this, &cmraDriverDataViewTypeForm::radioButton_shot_overhead_CheckedChanged);
			// 
			// radioButton_shot_long
			// 
			this->radioButton_shot_long->Location = System::Drawing::Point(224, 24);
			this->radioButton_shot_long->Name = "radioButton_shot_long";
			this->radioButton_shot_long->Size = System::Drawing::Size(120, 24);
			this->radioButton_shot_long->TabIndex = 4;
			this->radioButton_shot_long->Text = "Long";
			this->radioButton_shot_long->CheckedChanged += gcnew System::EventHandler(this, &cmraDriverDataViewTypeForm::radioButton_shot_long_CheckedChanged);
			// 
			// radioButton_shot_mediumlong
			// 
			this->radioButton_shot_mediumlong->Location = System::Drawing::Point(16, 120);
			this->radioButton_shot_mediumlong->Name = "radioButton_shot_mediumlong";
			this->radioButton_shot_mediumlong->Size = System::Drawing::Size(120, 24);
			this->radioButton_shot_mediumlong->TabIndex = 3;
			this->radioButton_shot_mediumlong->Text = "Medium-Long";
			this->radioButton_shot_mediumlong->CheckedChanged += gcnew System::EventHandler(this, &cmraDriverDataViewTypeForm::radioButton_shot_mediumlong_CheckedChanged);
			// 
			// radioButton_shot_medium
			// 
			this->radioButton_shot_medium->Location = System::Drawing::Point(16, 88);
			this->radioButton_shot_medium->Name = "radioButton_shot_medium";
			this->radioButton_shot_medium->Size = System::Drawing::Size(120, 24);
			this->radioButton_shot_medium->TabIndex = 2;
			this->radioButton_shot_medium->Text = "Medium";
			this->radioButton_shot_medium->CheckedChanged += gcnew System::EventHandler(this, &cmraDriverDataViewTypeForm::radioButton_shot_medium_CheckedChanged);
			// 
			// radioButton_shot_closeup
			// 
			this->radioButton_shot_closeup->Location = System::Drawing::Point(16, 56);
			this->radioButton_shot_closeup->Name = "radioButton_shot_closeup";
			this->radioButton_shot_closeup->Size = System::Drawing::Size(120, 24);
			this->radioButton_shot_closeup->TabIndex = 1;
			this->radioButton_shot_closeup->Text = "Close-Up";
			this->radioButton_shot_closeup->CheckedChanged += gcnew System::EventHandler(this, &cmraDriverDataViewTypeForm::radioButton_shot_closeup_CheckedChanged);
			// 
			// radioButton_shot_extremecloseup
			// 
			this->radioButton_shot_extremecloseup->Location = System::Drawing::Point(16, 24);
			this->radioButton_shot_extremecloseup->Name = "radioButton_shot_extremecloseup";
			this->radioButton_shot_extremecloseup->Size = System::Drawing::Size(120, 24);
			this->radioButton_shot_extremecloseup->TabIndex = 0;
			this->radioButton_shot_extremecloseup->Text = "Extreme Close-Up";
			this->radioButton_shot_extremecloseup->CheckedChanged += gcnew System::EventHandler(this, &cmraDriverDataViewTypeForm::radioButton_shot_extremecloseup_CheckedChanged);
			// 
			// label_tolerance
			// 
			this->label_tolerance->Location = System::Drawing::Point(8, 176);
			this->label_tolerance->Name = "label_tolerance";
			this->label_tolerance->Size = System::Drawing::Size(104, 23);
			this->label_tolerance->TabIndex = 8;
			this->label_tolerance->Text = "View Tolerance";
			// 
			// rangedFloat_viewtolerance
			// 
			this->rangedFloat_viewtolerance->Exponent = (System::Int16)1;
			this->rangedFloat_viewtolerance->Location = System::Drawing::Point(112, 168);
			this->rangedFloat_viewtolerance->Name = "rangedFloat_viewtolerance";
			this->rangedFloat_viewtolerance->NumTicks = (System::Int16)100;
			this->rangedFloat_viewtolerance->Precision = (System::Int16)2;
			this->rangedFloat_viewtolerance->Size = System::Drawing::Size(280, 32);
			this->rangedFloat_viewtolerance->TabIndex = 7;
			this->rangedFloat_viewtolerance->ValueChanged += gcnew System::EventHandler(this, &cmraDriverDataViewTypeForm::rangedFloat_viewtolerance_ValueChanged);
			// 
			// tabControl_target
			// 
			this->tabControl_target->Controls->Add(this->tabPage_camerasubject);
			this->tabControl_target->ItemSize = System::Drawing::Size(59, 18);
			this->tabControl_target->Location = System::Drawing::Point(8, 8);
			this->tabControl_target->Name = "tabControl_target";
			this->tabControl_target->SelectedIndex = 0;
			this->tabControl_target->Size = System::Drawing::Size(424, 264);
			this->tabControl_target->TabIndex = 7;
			// 
			// cmraDriverDataViewTypeForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(440, 293);
			this->Controls->Add(this->tabControl_target);
			this->Name = "cmraDriverDataViewTypeForm";
			this->Text = "Camera ViewType Properties";
			this->TopMost = true;
			this->tabPage_camerasubject->ResumeLayout(false);
			this->groupBox_shottype->ResumeLayout(false);
			this->tabControl_target->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

private: System::Void Set_Data()
		 {
			 // set the data view type
			 //
			if ( radioButton_shot_extremecloseup->Checked )
			{
				m_Data.m_ViewType = cmraDriverDataViewTypeInfo::e_DataView_ExtremeCloseUp;
			}
			else if ( radioButton_shot_closeup->Checked )
			{
				m_Data.m_ViewType = cmraDriverDataViewTypeInfo::e_DataView_CloseUp;
			}
			else if ( radioButton_shot_medium->Checked )
			{
				m_Data.m_ViewType = cmraDriverDataViewTypeInfo::e_DataView_Medium;
			}
			else if ( radioButton_shot_mediumlong->Checked )
			{
				m_Data.m_ViewType = cmraDriverDataViewTypeInfo::e_DataView_MediumLong;
			}
			else if ( radioButton_shot_long->Checked )
			{
				m_Data.m_ViewType = cmraDriverDataViewTypeInfo::e_DataView_Long;
			}
			else if ( radioButton_shot_overhead->Checked )
			{
				m_Data.m_ViewType = cmraDriverDataViewTypeInfo::e_DataView_Overhead;
			}
			else if ( radioButton_shot_aerial->Checked )
			{
				m_Data.m_ViewType = cmraDriverDataViewTypeInfo::e_DataView_Aerial;
			}

			//	view tolerance
			//
			m_Data.m_fViewTolerance = (float)rangedFloat_viewtolerance->Value;

			//	based on the view type set the parameters of the position tab page (is set).
			//
			if ( m_pPForm != nullptr )
			{
				switch ( m_Data.m_ViewType )
				{
					case cmraDriverDataViewTypeInfo::e_DataView_ExtremeCloseUp:
					{
						m_pPForm->SetDistanceConstraints( 0.01f, 2.0f );
						m_pPForm->SetPitchConstraints( 0.01f, 80.0f );
						m_pPForm->SetYawConstraints( -180.0f, 180.0f );

						m_pPForm->SetDistanceValue( 1.0f );
						m_pPForm->SetPitchValue( 45.0f );
						m_pPForm->SetYawValue( 0.0f );
						break;
					}
					case cmraDriverDataViewTypeInfo::e_DataView_CloseUp:
					{
						m_pPForm->SetDistanceConstraints( 2.01f, 4.0f );
						m_pPForm->SetPitchConstraints( 0.01f, 80.0f );
						m_pPForm->SetYawConstraints( -180.0f, 180.0f );

						m_pPForm->SetDistanceValue( 3.0f );
						m_pPForm->SetPitchValue( 45.0f );
						m_pPForm->SetYawValue( 0.0f );
						break;
					}
					default:
					case cmraDriverDataViewTypeInfo::e_DataView_Medium:
					{
						m_pPForm->SetDistanceConstraints( 4.01f, 10.0f );
						m_pPForm->SetPitchConstraints( 0.01f, 80.0f );
						m_pPForm->SetYawConstraints( -180.0f, 180.0f );

						m_pPForm->SetDistanceValue( 6.0f );
						m_pPForm->SetPitchValue( 45.0f );
						m_pPForm->SetYawValue( 0.0f );
						break;
					}
					case cmraDriverDataViewTypeInfo::e_DataView_MediumLong:
					{
						m_pPForm->SetDistanceConstraints( 10.01f, 40.0f );
						m_pPForm->SetPitchConstraints( 0.01f, 80.0f );
						m_pPForm->SetYawConstraints( -180.0f, 180.0f );

						m_pPForm->SetDistanceValue( 25.0f );
						m_pPForm->SetPitchValue( 45.0f );
						m_pPForm->SetYawValue( 0.0f );
						break;
					}
					case cmraDriverDataViewTypeInfo::e_DataView_Long:
					{
						m_pPForm->SetDistanceConstraints( 40.01f, 100.0f );
						m_pPForm->SetPitchConstraints( 0.01f, 80.0f );
						m_pPForm->SetYawConstraints( -180.0f, 180.0f );

						m_pPForm->SetDistanceValue( 50.0f );
						m_pPForm->SetPitchValue( 45.0f );
						m_pPForm->SetYawValue( 0.0f );
						break;
					}
					case cmraDriverDataViewTypeInfo::e_DataView_Overhead:
					{
						m_pPForm->SetDistanceConstraints( 2.01f, 10.0f );
						m_pPForm->SetPitchConstraints( 70.01f, 89.0f );
						m_pPForm->SetYawConstraints( -180.0f, 180.0f );

						m_pPForm->SetDistanceValue( 6.0f );
						m_pPForm->SetPitchValue( 80.0f );
						m_pPForm->SetYawValue( 0.0f );
						break;
					}
					case cmraDriverDataViewTypeInfo::e_DataView_Aerial:
					{
						m_pPForm->SetDistanceConstraints( 10.01f, 100.0f );
						m_pPForm->SetPitchConstraints( 70.01f, 89.0f );
						m_pPForm->SetYawConstraints( -180.0f, 180.0f );

						m_pPForm->SetDistanceValue( 50.0f );
						m_pPForm->SetPitchValue( 85.0f );
						m_pPForm->SetYawValue( 0.0f );
						break;
					}
				}
			}
		 }

private: System::Void comboBox_targetobject_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				Set_Data();
				m_bDisableNotify = false;
			}
		 }

private: System::Void radioButton_shot_extremecloseup_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				Set_Data();
				m_bDisableNotify = false;
			}
		 }

private: System::Void radioButton_shot_closeup_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				Set_Data();
				m_bDisableNotify = false;
			}
		 }

private: System::Void radioButton_shot_medium_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				Set_Data();
				m_bDisableNotify = false;
			}
		 }

private: System::Void radioButton_shot_mediumlong_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				Set_Data();
				m_bDisableNotify = false;
			}
		 }

private: System::Void radioButton_shot_long_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				Set_Data();
				m_bDisableNotify = false;
			}
		 }

private: System::Void radioButton_shot_overhead_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				Set_Data();
				m_bDisableNotify = false;
			}
		 }

private: System::Void radioButton_shot_aerial_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				Set_Data();
				m_bDisableNotify = false;
			}
		 }

private: System::Void rangedFloat_viewtolerance_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				Set_Data();
				m_bDisableNotify = false;
			}
		 }

private:
		//
		void SetUpComponents()
		{
			switch ( m_Data.m_ViewType )
			{
				case cmraDriverDataViewTypeInfo::e_DataView_ExtremeCloseUp:
				{
					radioButton_shot_extremecloseup->Checked = true;
					break;
				}
				case cmraDriverDataViewTypeInfo::e_DataView_CloseUp:
				{
					radioButton_shot_closeup->Checked = true;
					break;
				}
				default:
				case cmraDriverDataViewTypeInfo::e_DataView_Medium:
				{
					radioButton_shot_medium->Checked = true;
					break;
				}
				case cmraDriverDataViewTypeInfo::e_DataView_MediumLong:
				{
					radioButton_shot_mediumlong->Checked = true;
					break;
				}
				case cmraDriverDataViewTypeInfo::e_DataView_Long:
				{
					radioButton_shot_long->Checked = true;
					break;
				}
				case cmraDriverDataViewTypeInfo::e_DataView_Overhead:
				{
					radioButton_shot_overhead->Checked = true;
					break;
				}
				case cmraDriverDataViewTypeInfo::e_DataView_Aerial:
				{
					radioButton_shot_aerial->Checked = true;
					break;
				}
			}

			rangedFloat_viewtolerance->Value = m_Data.m_fViewTolerance;

			m_pPForm = nullptr;
		}

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_camerasubject;
		}

		// 
		//
		System::Void SetPositionForm( StudioFramework::cmraDriverDataPositionForm ^ i_pPForm )
		{
			m_pPForm = i_pPForm;
		}

		//	enable the passed in view types and DISABLE all the rest.
		//
		System::Void EnableViewTypes( int i_ViewTypeMask )
		{
			radioButton_shot_extremecloseup->Enabled	= ((i_ViewTypeMask & e_ExtremeCloseUp) != 0x0);
			radioButton_shot_closeup->Enabled			= ((i_ViewTypeMask & e_CloseUp) != 0x0);
			radioButton_shot_medium->Enabled			= ((i_ViewTypeMask & e_Medium) != 0x0);
			radioButton_shot_mediumlong->Enabled		= ((i_ViewTypeMask & e_MedLong) != 0x0);
			radioButton_shot_long->Enabled				= ((i_ViewTypeMask & e_Long) != 0x0);
			radioButton_shot_overhead->Enabled			= ((i_ViewTypeMask & e_Overhead) != 0x0);
			radioButton_shot_aerial->Enabled			= ((i_ViewTypeMask & e_Aerial) != 0x0);
		}

		//	Set the radio button with the passed in mask.
		//
		System::Void SetViewType( int i_ViewTypeMask )
		{
			switch ( m_Data.m_ViewType )
			{
				case cmraDriverDataViewTypeInfo::e_DataView_ExtremeCloseUp:
				{
					if ( radioButton_shot_extremecloseup->Enabled )
						i_ViewTypeMask = e_ExtremeCloseUp;
					break;
				}
				case cmraDriverDataViewTypeInfo::e_DataView_CloseUp:
				{
					if ( radioButton_shot_closeup->Enabled )
						i_ViewTypeMask = e_CloseUp;
					break;
				}
				default:
				case cmraDriverDataViewTypeInfo::e_DataView_Medium:
				{
					if ( radioButton_shot_medium->Enabled )
						i_ViewTypeMask = e_Medium;
					break;
				}
				case cmraDriverDataViewTypeInfo::e_DataView_MediumLong:
				{
					if ( radioButton_shot_mediumlong->Enabled )
						i_ViewTypeMask = e_MedLong;
					break;
				}
				case cmraDriverDataViewTypeInfo::e_DataView_Long:
				{
					if ( radioButton_shot_long->Enabled )
						i_ViewTypeMask = e_Long;
					break;
				}
				case cmraDriverDataViewTypeInfo::e_DataView_Overhead:
				{
					if ( radioButton_shot_overhead->Enabled )
						i_ViewTypeMask = e_Overhead;
					break;
				}
				case cmraDriverDataViewTypeInfo::e_DataView_Aerial:
				{
					if ( radioButton_shot_aerial->Enabled )
						i_ViewTypeMask = e_Aerial;
					break;
				}
			}

			m_bDisableNotify = true;
			radioButton_shot_extremecloseup->Checked	= ((i_ViewTypeMask & e_ExtremeCloseUp) != 0x0);
			radioButton_shot_closeup->Checked			= ((i_ViewTypeMask & e_CloseUp) != 0x0);
			radioButton_shot_medium->Checked			= ((i_ViewTypeMask & e_Medium) != 0x0);
			radioButton_shot_mediumlong->Checked		= ((i_ViewTypeMask & e_MedLong) != 0x0);
			radioButton_shot_long->Checked				= ((i_ViewTypeMask & e_Long) != 0x0);
			radioButton_shot_overhead->Checked			= ((i_ViewTypeMask & e_Overhead) != 0x0);
			radioButton_shot_aerial->Checked			= ((i_ViewTypeMask & e_Aerial) != 0x0);
			m_bDisableNotify = false;
		}
};
}
#endif // _MANAGED
