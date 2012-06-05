#pragma once

#ifndef CMM_SPLINEOPERATIONS_HPP
#include "Systems/Common/Spline/cmmSplineOperations.hpp"
#endif
#ifndef SEL3D_MGR_HPP
#include "Tool/sel3d/sel3dMgr.hpp"
#endif
#ifndef SPLN_CURVEPOINTOBJECT_HPP
#include "Support/spln/splnCurvePointObject.hpp"
#endif
#ifndef SPLN_CURVEMGR_HPP
#include "Support/spln/splnCurveMgr.hpp"
#endif
#ifndef SPLN_CURVESELECT_HPP
#include "Support/spln/splnCurveSelect.hpp"
#endif
#ifndef SPLN_SPLINE_HPP
#include "Support/spln/splnSpline.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef CAM3D_MGR_HPP
#include "Tool/cam3d/cam3dMgr.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
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
	/// Summary for cmmSplinePointForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmmSplinePointForm : public System::Windows::Forms::Form
	{
	public: 
		static cmmSplinePointForm^ FormInstance = nullptr;

		cmmSplinePointForm() : m_pObject(NULL)
		{
			m_bOurChange = false;
			m_bDisableNotify = true;
			InitializeComponent();
			SetupData(m_pObject);
			m_bDisableNotify = false;
		}

		// Call this to update form data
		void Update()
		{
			if (m_pObject != NULL)
				SetupData(m_pObject);
		}
		void Update(splnCurvePointObject *i_pObject)
		{
			SetupData(i_pObject);
			m_pObject = i_pObject;
		}

	protected: 
		~cmmSplinePointForm()
		{
			// clear instance
			if (cmmSplinePointForm::FormInstance == this)
				cmmSplinePointForm::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}

	private: splnCurvePointObject* m_pObject;
		// Turns off notify callbacks when setting up form
	private: bool m_bDisableNotify;
		// Turns off setting of data fields when we make the change
	private: bool m_bOurChange;


	private: System::Windows::Forms::Button ^  buttonInsert;
	private: System::Windows::Forms::Button ^  buttonSelectPrev;
	private: System::Windows::Forms::Button ^  buttonSelectNext;
	private: System::Windows::Forms::Button ^  button_seteditcam;
	private: System::Windows::Forms::Button ^  button_setpoint;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_pos;
	private: System::Windows::Forms::TabControl ^  tabControl_splinepoint;
	private: System::Windows::Forms::TabPage ^  tabPage_splinepoint;
	private: System::Windows::Forms::Label ^  label_pos;
	private: System::Windows::Forms::GroupBox ^  groupBox_splinepoint;
	private: System::Windows::Forms::Label ^  label_pointnum;
	private: System::Windows::Forms::GroupBox ^  groupBox_spline;
	private: System::Windows::Forms::Label ^  label_splinename;
	private: TerawattManagedControls::Vector3EditUpDown ^  vector3EditUpDown_spline;
	private: System::Windows::Forms::Label ^  label1;
	private: System::Windows::Forms::Button ^  button_deleteCP;


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
			this->tabControl_splinepoint = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_splinepoint = (gcnew System::Windows::Forms::TabPage());
			this->button_deleteCP = (gcnew System::Windows::Forms::Button());
			this->groupBox_spline = (gcnew System::Windows::Forms::GroupBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->vector3EditUpDown_spline = (gcnew TerawattManagedControls::Vector3EditUpDown());
			this->label_splinename = (gcnew System::Windows::Forms::Label());
			this->groupBox_splinepoint = (gcnew System::Windows::Forms::GroupBox());
			this->label_pointnum = (gcnew System::Windows::Forms::Label());
			this->button_setpoint = (gcnew System::Windows::Forms::Button());
			this->button_seteditcam = (gcnew System::Windows::Forms::Button());
			this->vector3Edit_pos = (gcnew TerawattManagedControls::Vector3Edit());
			this->label_pos = (gcnew System::Windows::Forms::Label());
			this->buttonInsert = (gcnew System::Windows::Forms::Button());
			this->buttonSelectPrev = (gcnew System::Windows::Forms::Button());
			this->buttonSelectNext = (gcnew System::Windows::Forms::Button());
			this->tabControl_splinepoint->SuspendLayout();
			this->tabPage_splinepoint->SuspendLayout();
			this->groupBox_spline->SuspendLayout();
			this->groupBox_splinepoint->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_splinepoint
			// 
			this->tabControl_splinepoint->Controls->Add(this->tabPage_splinepoint);
			this->tabControl_splinepoint->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl_splinepoint->Location = System::Drawing::Point(0, 0);
			this->tabControl_splinepoint->Name = L"tabControl_splinepoint";
			this->tabControl_splinepoint->SelectedIndex = 0;
			this->tabControl_splinepoint->Size = System::Drawing::Size(411, 277);
			this->tabControl_splinepoint->TabIndex = 0;
			// 
			// tabPage_splinepoint
			// 
			this->tabPage_splinepoint->Controls->Add(this->button_deleteCP);
			this->tabPage_splinepoint->Controls->Add(this->groupBox_spline);
			this->tabPage_splinepoint->Controls->Add(this->groupBox_splinepoint);
			this->tabPage_splinepoint->Controls->Add(this->buttonInsert);
			this->tabPage_splinepoint->Controls->Add(this->buttonSelectPrev);
			this->tabPage_splinepoint->Controls->Add(this->buttonSelectNext);
			this->tabPage_splinepoint->Location = System::Drawing::Point(4, 22);
			this->tabPage_splinepoint->Name = L"tabPage_splinepoint";
			this->tabPage_splinepoint->Size = System::Drawing::Size(403, 251);
			this->tabPage_splinepoint->TabIndex = 0;
			this->tabPage_splinepoint->Text = L"Spline Point";
			// 
			// button_deleteCP
			// 
			this->button_deleteCP->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_deleteCP->Location = System::Drawing::Point(317, 215);
			this->button_deleteCP->Name = L"button_deleteCP";
			this->button_deleteCP->Size = System::Drawing::Size(80, 28);
			this->button_deleteCP->TabIndex = 11;
			this->button_deleteCP->Text = L"Delete Point";
			this->button_deleteCP->Click += gcnew System::EventHandler(this, &cmmSplinePointForm::button_deleteCP_Click);
			// 
			// groupBox_spline
			// 
			this->groupBox_spline->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_spline->Controls->Add(this->label1);
			this->groupBox_spline->Controls->Add(this->vector3EditUpDown_spline);
			this->groupBox_spline->Controls->Add(this->label_splinename);
			this->groupBox_spline->Location = System::Drawing::Point(5, 136);
			this->groupBox_spline->Name = L"groupBox_spline";
			this->groupBox_spline->Size = System::Drawing::Size(395, 72);
			this->groupBox_spline->TabIndex = 10;
			this->groupBox_spline->TabStop = false;
			this->groupBox_spline->Text = L"Spline Data";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(16, 40);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(100, 23);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Move Entire Spline";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// vector3EditUpDown_spline
			// 
			this->vector3EditUpDown_spline->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->vector3EditUpDown_spline->IncrementX = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->vector3EditUpDown_spline->IncrementY = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->vector3EditUpDown_spline->IncrementZ = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->vector3EditUpDown_spline->Location = System::Drawing::Point(120, 40);
			this->vector3EditUpDown_spline->MaximumX = System::Decimal(gcnew cli::array< System::Int32 >(4) {-1, -1, -1, 0});
			this->vector3EditUpDown_spline->MaximumY = System::Decimal(gcnew cli::array< System::Int32 >(4) {-1, -1, -1, 0});
			this->vector3EditUpDown_spline->MaximumZ = System::Decimal(gcnew cli::array< System::Int32 >(4) {-1, -1, -1, 0});
			this->vector3EditUpDown_spline->MinimumX = System::Decimal(gcnew cli::array< System::Int32 >(4) {-1, -1, -1, System::Int32::MinValue});
			this->vector3EditUpDown_spline->MinimumY = System::Decimal(gcnew cli::array< System::Int32 >(4) {-1, -1, -1, System::Int32::MinValue});
			this->vector3EditUpDown_spline->MinimumZ = System::Decimal(gcnew cli::array< System::Int32 >(4) {-1, -1, -1, System::Int32::MinValue});
			this->vector3EditUpDown_spline->Name = L"vector3EditUpDown_spline";
			this->vector3EditUpDown_spline->Precision = static_cast<System::Int16>(2);
			this->vector3EditUpDown_spline->Size = System::Drawing::Size(256, 20);
			this->vector3EditUpDown_spline->TabIndex = 1;
			this->vector3EditUpDown_spline->ValueX = System::Decimal(gcnew cli::array< System::Int32 >(4) {0, 0, 0, 0});
			this->vector3EditUpDown_spline->ValueY = System::Decimal(gcnew cli::array< System::Int32 >(4) {0, 0, 0, 0});
			this->vector3EditUpDown_spline->ValueZ = System::Decimal(gcnew cli::array< System::Int32 >(4) {0, 0, 0, 0});
			this->vector3EditUpDown_spline->ValueChanged += gcnew System::EventHandler(this, &cmmSplinePointForm::vector3EditUpDown_spline_ValueChanged);
			// 
			// label_splinename
			// 
			this->label_splinename->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->label_splinename->Location = System::Drawing::Point(16, 12);
			this->label_splinename->Name = L"label_splinename";
			this->label_splinename->Size = System::Drawing::Size(371, 23);
			this->label_splinename->TabIndex = 0;
			this->label_splinename->Text = L"Spline Name";
			this->label_splinename->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// groupBox_splinepoint
			// 
			this->groupBox_splinepoint->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_splinepoint->Controls->Add(this->label_pointnum);
			this->groupBox_splinepoint->Controls->Add(this->button_setpoint);
			this->groupBox_splinepoint->Controls->Add(this->button_seteditcam);
			this->groupBox_splinepoint->Controls->Add(this->vector3Edit_pos);
			this->groupBox_splinepoint->Controls->Add(this->label_pos);
			this->groupBox_splinepoint->Location = System::Drawing::Point(5, 8);
			this->groupBox_splinepoint->Name = L"groupBox_splinepoint";
			this->groupBox_splinepoint->Size = System::Drawing::Size(395, 120);
			this->groupBox_splinepoint->TabIndex = 9;
			this->groupBox_splinepoint->TabStop = false;
			this->groupBox_splinepoint->Text = L"Point Data";
			// 
			// label_pointnum
			// 
			this->label_pointnum->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->label_pointnum->Location = System::Drawing::Point(16, 24);
			this->label_pointnum->Name = L"label_pointnum";
			this->label_pointnum->Size = System::Drawing::Size(371, 23);
			this->label_pointnum->TabIndex = 9;
			this->label_pointnum->Text = L"Point Number X of X";
			// 
			// button_setpoint
			// 
			this->button_setpoint->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_setpoint->Location = System::Drawing::Point(16, 88);
			this->button_setpoint->Name = L"button_setpoint";
			this->button_setpoint->Size = System::Drawing::Size(176, 23);
			this->button_setpoint->TabIndex = 8;
			this->button_setpoint->Text = L"Set Selected Point To Edit Cam";
			this->button_setpoint->Click += gcnew System::EventHandler(this, &cmmSplinePointForm::button_setpoint_Click);
			// 
			// button_seteditcam
			// 
			this->button_seteditcam->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_seteditcam->Location = System::Drawing::Point(200, 88);
			this->button_seteditcam->Name = L"button_seteditcam";
			this->button_seteditcam->Size = System::Drawing::Size(176, 23);
			this->button_seteditcam->TabIndex = 7;
			this->button_seteditcam->Text = L"Set Edit Cam To Selected Point";
			this->button_seteditcam->Click += gcnew System::EventHandler(this, &cmmSplinePointForm::button_seteditcam_Click);
			// 
			// vector3Edit_pos
			// 
			this->vector3Edit_pos->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->vector3Edit_pos->Enabled = false;
			this->vector3Edit_pos->Location = System::Drawing::Point(80, 56);
			this->vector3Edit_pos->Name = L"vector3Edit_pos";
			this->vector3Edit_pos->Precision = static_cast<System::Int16>(2);
			this->vector3Edit_pos->Size = System::Drawing::Size(299, 28);
			this->vector3Edit_pos->TabIndex = 0;
			this->vector3Edit_pos->ValueChanged += gcnew System::EventHandler(this, &cmmSplinePointForm::vector3Edit_pos_ValueChanged);
			// 
			// label_pos
			// 
			this->label_pos->Location = System::Drawing::Point(16, 56);
			this->label_pos->Name = L"label_pos";
			this->label_pos->Size = System::Drawing::Size(53, 21);
			this->label_pos->TabIndex = 1;
			this->label_pos->Text = L"Position:";
			// 
			// buttonInsert
			// 
			this->buttonInsert->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->buttonInsert->Location = System::Drawing::Point(218, 215);
			this->buttonInsert->Name = L"buttonInsert";
			this->buttonInsert->Size = System::Drawing::Size(80, 28);
			this->buttonInsert->TabIndex = 6;
			this->buttonInsert->Text = L"Insert Point";
			this->buttonInsert->Click += gcnew System::EventHandler(this, &cmmSplinePointForm::buttonInsert_Click);
			// 
			// buttonSelectPrev
			// 
			this->buttonSelectPrev->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->buttonSelectPrev->Location = System::Drawing::Point(8, 215);
			this->buttonSelectPrev->Name = L"buttonSelectPrev";
			this->buttonSelectPrev->Size = System::Drawing::Size(96, 28);
			this->buttonSelectPrev->TabIndex = 5;
			this->buttonSelectPrev->Text = L"Select Previous";
			this->buttonSelectPrev->Click += gcnew System::EventHandler(this, &cmmSplinePointForm::buttonSelectPrev_Click);
			// 
			// buttonSelectNext
			// 
			this->buttonSelectNext->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->buttonSelectNext->Location = System::Drawing::Point(104, 215);
			this->buttonSelectNext->Name = L"buttonSelectNext";
			this->buttonSelectNext->Size = System::Drawing::Size(96, 28);
			this->buttonSelectNext->TabIndex = 4;
			this->buttonSelectNext->Text = L"Select Next";
			this->buttonSelectNext->Click += gcnew System::EventHandler(this, &cmmSplinePointForm::buttonSelectNext_Click);
			// 
			// cmmSplinePointForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(411, 277);
			this->Controls->Add(this->tabControl_splinepoint);
			this->Name = L"cmmSplinePointForm";
			this->Text = L"cmmSplinePointForm";
			this->tabControl_splinepoint->ResumeLayout(false);
			this->tabPage_splinepoint->ResumeLayout(false);
			this->groupBox_spline->ResumeLayout(false);
			this->groupBox_splinepoint->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

		void SetupData(splnCurvePointObject *i_pObject)
		{
			if (m_bOurChange) return;

			if (!i_pObject)
			{
				vector3Edit_pos->Enabled = false;
				vector3EditUpDown_spline->Enabled = false;

				label_pointnum->Text = gcnew System::String("Point  0 of  0");
				label_splinename->Text = String::Format("Spline Name: {0}", "none");
			}
			else
			{
				vector3Edit_pos->Enabled = true;
				vector3EditUpDown_spline->Enabled = true;

				m_bDisableNotify = true;
				tmaManagedConversionUtil::SetPoint3(i_pObject->GetPosition(), vector3Edit_pos);
				tmaManagedConversionUtil::SetPoint3(i_pObject->GetPosition(), vector3EditUpDown_spline);

				splnSpline* pSpline = i_pObject->GetCurve();
				label_pointnum->Text = String::Format("Point #{0:00} of {1:00}", i_pObject->GetPointIndex()+1, (pSpline->GetNumPoints()) );
				label_splinename->Text = String::Format("Spline Name: {0}", gcnew String(pSpline->GetName().c_str()));
	
				m_bDisableNotify = false;
			}
		}

	private: System::Void vector3Edit_pos_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			if (!m_bDisableNotify) 
			{
				m_bDisableNotify = true;
				m_bOurChange = true;
				m_pObject->UpdatePosition(maPoint3d((float)vector3Edit_pos->ValueX, (float)vector3Edit_pos->ValueY, (float)vector3Edit_pos->ValueZ));
				vector3EditUpDown_spline->ValueX = (System::Decimal)vector3Edit_pos->ValueX;
				vector3EditUpDown_spline->ValueY = (System::Decimal)vector3Edit_pos->ValueY;
				vector3EditUpDown_spline->ValueZ = (System::Decimal)vector3Edit_pos->ValueZ;
				m_bOurChange = false;
				m_bDisableNotify = false;
			}
		}

	private: System::Void buttonSelectNext_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			 //NOTE: If any changes are needed, use the cmmSplineOperations functions instead

			splnSpline* pSpline = m_pObject->GetCurve();
			int num_points = pSpline->GetNumPoints();
			int sel_ind = get_selected_index();
			sel_ind = (sel_ind + 1) % num_points;
			sel3dMgr::CreateUndoOperation();
			sel3dMgr::Select(splnCurveSelect::GetControlPointObject(pSpline, sel_ind));
		}

	private: System::Void buttonSelectPrev_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			 //NOTE: If any changes are needed, use the cmmSplineOperations functions instead

			splnSpline* pSpline = m_pObject->GetCurve();
			int num_points = pSpline->GetNumPoints();
			int sel_ind = get_selected_index();
			sel_ind = (sel_ind + num_points - 1) % num_points;
			sel3dMgr::CreateUndoOperation();
			sel3dMgr::Select(splnCurveSelect::GetControlPointObject(pSpline, sel_ind));
		}

	private: System::Void buttonInsert_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			 //NOTE: If any changes are needed, use the cmmSplineOperations functions instead

			splnSpline* pSpline = m_pObject->GetCurve();
			int num_points = pSpline->GetNumPoints();
			int sel_ind = get_selected_index();
			splnCurveMgr::InsertCtrlPoint(pSpline, sel_ind);
		}

private: System::Void button_deleteCP_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 //NOTE: If any changes are needed, use the cmmSplineOperations functions instead

			 //	delete the spline point
			splnSpline* pSpline = m_pObject->GetCurve();
			int num_points = pSpline->GetNumPoints();
			int sel_ind = get_selected_index();
			splnCurveMgr::DeleteCtrlPoint(pSpline, sel_ind);

			// select the next point (if there is one)
			num_points = pSpline->GetNumPoints();
			if (num_points == 0)
				return;

			sel_ind++;
			if (sel_ind > num_points)
			{
				sel_ind = num_points;
			}
			sel_ind = (sel_ind + num_points - 1) % num_points;
			sel3dMgr::Select(splnCurveSelect::GetControlPointObject(pSpline, sel_ind));
		 }

private: System::Void button_seteditcam_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 //NOTE: If any changes are needed, use the cmmSplineOperations functions instead

			camCamera& cam = cam3dMgr::GetEditorCamera();
			cam3dMgr::SetManipPosition( m_pObject->GetPosition() );
			m_pObject->UpdatePosition( cam.GetPosition() );
		 }

private: System::Void button_setpoint_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 //NOTE: If any changes are needed, use the cmmSplineOperations functions instead

			if (!m_bDisableNotify) 
			{
				m_bOurChange = true;

				camCamera& cam = cam3dMgr::GetEditorCamera();
				m_pObject->UpdatePosition( cam.GetPosition() );
				//splnCurveMgr::AlterCtrlPoint(pSpline, sel_ind, cam.GetPosition());

				m_bOurChange = false;
			}
		 }

private: System::Void vector3EditUpDown_spline_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify) 
			{
				maPoint3d vec_spline, vec_pos;
				tmaManagedConversionUtil::GetPoint3(vector3EditUpDown_spline, vec_spline);
				tmaManagedConversionUtil::GetPoint3(vector3Edit_pos, vec_pos);
				maVector3d diff = vec_spline - vec_pos;

				splnSpline* pSpline = m_pObject->GetCurve();
				int count = pSpline->GetNumPoints();
				maPoint3d newpos;
				for (int i = 0; i < count; ++i)
				{
					newpos = pSpline->GetPointPos(i);
					newpos += diff;
					pSpline->SetPointPos(i, newpos);
				}

				m_bDisableNotify = true;
				vector3Edit_pos->ValueX = System::Decimal::ToDouble(vector3EditUpDown_spline->ValueX);
				vector3Edit_pos->ValueY = System::Decimal::ToDouble(vector3EditUpDown_spline->ValueY);
				vector3Edit_pos->ValueZ = System::Decimal::ToDouble(vector3EditUpDown_spline->ValueZ);
				m_bOurChange = true;
				m_pObject->UpdatePosition(maPoint3d((float)vector3Edit_pos->ValueX, (float)vector3Edit_pos->ValueY, (float)vector3Edit_pos->ValueZ));
				m_bOurChange = false;
				m_bDisableNotify = false;
			}
		 }

	private: int get_selected_index()
		{
			splnSpline* pSpline = m_pObject->GetCurve();
			int num_points = pSpline->GetNumPoints();
			int sel_ind = -1;
			
			if (sel3dMgr::GetSelected() != NULL)
			{
				splnCurvePointObject* obj = dynamic_cast<splnCurvePointObject*>(sel3dMgr::GetSelected());
				for (int i=0; i<num_points; i++)
				{
					if (obj == splnCurveSelect::GetControlPointObject(pSpline, i))
					{
						sel_ind = i;
						break;
					}
				}
			}
			return sel_ind;
		}

	public:
    	//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_splinepoint;
		}

};
}
#endif // _MANAGED
