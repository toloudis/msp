#pragma once

#ifndef CMPS_COMPASSMGR_HPP
#include "Support/cmps/cmpsCompassMgr.hpp"
#endif
#ifndef MNM_COMPASSUTIL_HPP
#include "Support/mnm/mnmCompassUtil.hpp"
#endif
#ifndef SEL3D_MGR_HPP
#include "Tool/sel3d/sel3dMgr.hpp"
#endif
#ifndef SPLN_CURVEMGR_HPP
#include "Support/spln/splnCurveMgr.hpp"
#endif
#ifndef SPLN_CURVEPOINTOBJECT_HPP
#include "Support/spln/splnCurvePointObject.hpp"
#endif
#ifndef SPLN_CURVESELECT_HPP
#include "Support/spln/splnCurveSelect.hpp"
#endif
#ifndef SPLN_SPLINE_HPP
#include "Support/spln/splnSpline.hpp"
#endif
#ifndef TMLN_DRIVERSPLINE_HPP
#include "Drivers/Spline/tmlnDriverSpline.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/Cam/camCamera.hpp"
#endif
#ifndef CAM3D_MGR_HPP
#include "Tool/cam3d/cam3dMgr.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "Core/Dbg/dbgLog.hpp"
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
	/// Summary for tmlnDriverSplineForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class tmlnDriverSplineForm : public System::Windows::Forms::Form
	{
	public: 
		tmlnDriverSplineForm(tmlnDriverSpline &i_Driver)
			: m_Driver(i_Driver)
		{
			InitializeComponent();
		}
        
	protected: 
		~tmlnDriverSplineForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: tmlnDriverSpline &m_Driver;
	//private: bool m_bDisableNotify;
	private: System::Windows::Forms::Button ^  buttonSelectNext;
	private: System::Windows::Forms::Button ^  buttonSelectPrev;
	private: System::Windows::Forms::Button ^  buttonInsert;
	private: System::Windows::Forms::TabControl ^  tabControl_spline;
	private: System::Windows::Forms::TabPage ^  tabPage_spline;
	private: System::Windows::Forms::Label ^  label_pos;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_pos;
	private: System::Windows::Forms::Button ^  button_setpoint;
	private: System::Windows::Forms::Button ^  button_seteditcam;
	private: System::Windows::Forms::GroupBox ^  groupBox_pointmanip;
	private: System::Windows::Forms::GroupBox ^  groupBox_data;
	private: System::Windows::Forms::GroupBox ^  groupBox_select;
	private: System::Windows::Forms::GroupBox ^  groupBox_addpoint;
	private: System::Windows::Forms::Button ^  buttonAppend;



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
			this->buttonSelectNext = gcnew System::Windows::Forms::Button();
			this->buttonSelectPrev = gcnew System::Windows::Forms::Button();
			this->buttonInsert = gcnew System::Windows::Forms::Button();
			this->tabControl_spline = gcnew System::Windows::Forms::TabControl();
			this->tabPage_spline = gcnew System::Windows::Forms::TabPage();
			this->vector3Edit_pos = gcnew TerawattManagedControls::Vector3Edit();
			this->label_pos = gcnew System::Windows::Forms::Label();
			this->groupBox_data = gcnew System::Windows::Forms::GroupBox();
			this->button_setpoint = gcnew System::Windows::Forms::Button();
			this->groupBox_select = gcnew System::Windows::Forms::GroupBox();
			this->groupBox_addpoint = gcnew System::Windows::Forms::GroupBox();
			this->buttonAppend = gcnew System::Windows::Forms::Button();
			this->groupBox_pointmanip = gcnew System::Windows::Forms::GroupBox();
			this->button_seteditcam = gcnew System::Windows::Forms::Button();
			this->tabControl_spline->SuspendLayout();
			this->tabPage_spline->SuspendLayout();
			this->groupBox_data->SuspendLayout();
			this->groupBox_select->SuspendLayout();
			this->groupBox_addpoint->SuspendLayout();
			this->groupBox_pointmanip->SuspendLayout();
			this->SuspendLayout();
			// 
			// buttonSelectNext
			// 
			this->buttonSelectNext->Location = System::Drawing::Point(168, 16);
			this->buttonSelectNext->Name = "buttonSelectNext";
			this->buttonSelectNext->Size = System::Drawing::Size(96, 32);
			this->buttonSelectNext->TabIndex = 0;
			this->buttonSelectNext->Text = "Select Next";
			this->buttonSelectNext->Click += gcnew System::EventHandler(this, &tmlnDriverSplineForm::buttonSelectNext_Click);
			// 
			// buttonSelectPrev
			// 
			this->buttonSelectPrev->Location = System::Drawing::Point(56, 16);
			this->buttonSelectPrev->Name = "buttonSelectPrev";
			this->buttonSelectPrev->Size = System::Drawing::Size(96, 32);
			this->buttonSelectPrev->TabIndex = 1;
			this->buttonSelectPrev->Text = "Select Previous";
			this->buttonSelectPrev->Click += gcnew System::EventHandler(this, &tmlnDriverSplineForm::buttonSelectPrev_Click);
			// 
			// buttonInsert
			// 
			this->buttonInsert->Location = System::Drawing::Point(32, 16);
			this->buttonInsert->Name = "buttonInsert";
			this->buttonInsert->Size = System::Drawing::Size(122, 32);
			this->buttonInsert->TabIndex = 3;
			this->buttonInsert->Text = "Add Control Point After Selected";
			this->buttonInsert->Click += gcnew System::EventHandler(this, &tmlnDriverSplineForm::buttonInsert_Click);
			// 
			// tabControl_spline
			// 
			this->tabControl_spline->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_spline->Controls->Add(this->tabPage_spline);
			this->tabControl_spline->Location = System::Drawing::Point(8, 8);
			this->tabControl_spline->Name = "tabControl_spline";
			this->tabControl_spline->SelectedIndex = 0;
			this->tabControl_spline->Size = System::Drawing::Size(329, 324);
			this->tabControl_spline->TabIndex = 4;
			// 
			// tabPage_spline
			// 
			this->tabPage_spline->Controls->Add(this->vector3Edit_pos);
			this->tabPage_spline->Controls->Add(this->label_pos);
			this->tabPage_spline->Controls->Add(this->groupBox_data);
			this->tabPage_spline->Controls->Add(this->groupBox_select);
			this->tabPage_spline->Controls->Add(this->groupBox_addpoint);
			this->tabPage_spline->Controls->Add(this->groupBox_pointmanip);
			this->tabPage_spline->Location = System::Drawing::Point(4, 22);
			this->tabPage_spline->Name = "tabPage_spline";
			this->tabPage_spline->Size = System::Drawing::Size(321, 298);
			this->tabPage_spline->TabIndex = 0;
			this->tabPage_spline->Text = "Spline Point";
			// 
			// vector3Edit_pos
			// 
			this->vector3Edit_pos->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->vector3Edit_pos->Enabled = false;
			this->vector3Edit_pos->Location = System::Drawing::Point(72, 24);
			this->vector3Edit_pos->Name = "vector3Edit_pos";
			this->vector3Edit_pos->Precision = (System::Int16)2;
			this->vector3Edit_pos->Size = System::Drawing::Size(233, 28);
			this->vector3Edit_pos->TabIndex = 5;
			// 
			// label_pos
			// 
			this->label_pos->Location = System::Drawing::Point(16, 24);
			this->label_pos->Name = "label_pos";
			this->label_pos->Size = System::Drawing::Size(48, 21);
			this->label_pos->TabIndex = 6;
			this->label_pos->Text = "Position:";
			// 
			// groupBox_data
			// 
			this->groupBox_data->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupBox_data->Controls->Add(this->button_setpoint);
			this->groupBox_data->Location = System::Drawing::Point(8, 8);
			this->groupBox_data->Name = "groupBox_data";
			this->groupBox_data->Size = System::Drawing::Size(304, 80);
			this->groupBox_data->TabIndex = 12;
			this->groupBox_data->TabStop = false;
			this->groupBox_data->Text = "Point Data";
			// 
			// button_setpoint
			// 
			this->button_setpoint->Location = System::Drawing::Point(52, 48);
			this->button_setpoint->Name = "button_setpoint";
			this->button_setpoint->Size = System::Drawing::Size(216, 23);
			this->button_setpoint->TabIndex = 10;
			this->button_setpoint->Text = "Set Selected Point To Edit Cam";
			this->button_setpoint->Click += gcnew System::EventHandler(this, &tmlnDriverSplineForm::button_setpoint_Click);
			// 
			// groupBox_select
			// 
			this->groupBox_select->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupBox_select->Controls->Add(this->buttonSelectPrev);
			this->groupBox_select->Controls->Add(this->buttonSelectNext);
			this->groupBox_select->Location = System::Drawing::Point(8, 96);
			this->groupBox_select->Name = "groupBox_select";
			this->groupBox_select->Size = System::Drawing::Size(304, 56);
			this->groupBox_select->TabIndex = 13;
			this->groupBox_select->TabStop = false;
			this->groupBox_select->Text = "Point Selection";
			// 
			// groupBox_addpoint
			// 
			this->groupBox_addpoint->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupBox_addpoint->Controls->Add(this->buttonAppend);
			this->groupBox_addpoint->Controls->Add(this->buttonInsert);
			this->groupBox_addpoint->Location = System::Drawing::Point(8, 160);
			this->groupBox_addpoint->Name = "groupBox_addpoint";
			this->groupBox_addpoint->Size = System::Drawing::Size(304, 56);
			this->groupBox_addpoint->TabIndex = 14;
			this->groupBox_addpoint->TabStop = false;
			this->groupBox_addpoint->Text = "Insert Spline Point";
			// 
			// buttonAppend
			// 
			this->buttonAppend->Location = System::Drawing::Point(168, 16);
			this->buttonAppend->Name = "buttonAppend";
			this->buttonAppend->Size = System::Drawing::Size(122, 32);
			this->buttonAppend->TabIndex = 4;
			this->buttonAppend->Text = "Append Control Point at Object Position";
			this->buttonAppend->Click += gcnew System::EventHandler(this, &tmlnDriverSplineForm::buttonAppend_Click);
			// 
			// groupBox_pointmanip
			// 
			this->groupBox_pointmanip->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupBox_pointmanip->Controls->Add(this->button_seteditcam);
			this->groupBox_pointmanip->Location = System::Drawing::Point(8, 232);
			this->groupBox_pointmanip->Name = "groupBox_pointmanip";
			this->groupBox_pointmanip->Size = System::Drawing::Size(304, 56);
			this->groupBox_pointmanip->TabIndex = 11;
			this->groupBox_pointmanip->TabStop = false;
			this->groupBox_pointmanip->Text = "Point Manip Helpers";
			// 
			// button_seteditcam
			// 
			this->button_seteditcam->Location = System::Drawing::Point(52, 24);
			this->button_seteditcam->Name = "button_seteditcam";
			this->button_seteditcam->Size = System::Drawing::Size(216, 23);
			this->button_seteditcam->TabIndex = 9;
			this->button_seteditcam->Text = "Set Edit Cam To Selected Point";
			this->button_seteditcam->Click += gcnew System::EventHandler(this, &tmlnDriverSplineForm::button_seteditcam_Click);
			// 
			// tmlnDriverSplineForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(344, 342);
			this->Controls->Add(this->tabControl_spline);
			this->Name = "tmlnDriverSplineForm";
			this->Text = "Spline Properties";
			this->TopMost = true;
			this->tabControl_spline->ResumeLayout(false);
			this->tabPage_spline->ResumeLayout(false);
			this->groupBox_data->ResumeLayout(false);
			this->groupBox_select->ResumeLayout(false);
			this->groupBox_addpoint->ResumeLayout(false);
			this->groupBox_pointmanip->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

	private: System::Void vector3Edit_pos_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			 if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

			//if (!m_bDisableNotify) 
			{
				//m_bDisableNotify = true;
				//m_bOurChange = true;	

				splnSpline* pSpline = &m_Driver.Spline();
				int sel_ind = get_selected_index();
				pSpline->SetPointPos( sel_ind, maPoint3d((float)vector3Edit_pos->ValueX, (float)vector3Edit_pos->ValueY, (float)vector3Edit_pos->ValueZ));

				//m_bOurChange = false;
				//m_bDisableNotify = false;
			}
		}

	private: System::Void buttonSelectNext_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

				//if (!m_bDisableNotify) 
				{
					//m_bDisableNotify = true;
					splnSpline* pSpline = &m_Driver.Spline();
					int num_points = pSpline->GetNumPoints();
					int sel_ind = get_selected_index();
					sel_ind = (sel_ind + 1) % num_points;
					sel3dMgr::CreateUndoOperation();
					sel3dMgr::Select(splnCurveSelect::GetControlPointObject(pSpline, sel_ind));

					//mnmCompassUtil::SetUpCompass(  cmpsCompassMgr::e_Select, 
					//							splnCurveSelect::GetControlPointObject(pSpline, sel_ind) );
					//m_bDisableNotify = false;
				}
			 }

	private: System::Void buttonSelectPrev_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

				//if (!m_bDisableNotify) 
				{
					//m_bDisableNotify = true;
					splnSpline* pSpline = &m_Driver.Spline();
					int num_points = pSpline->GetNumPoints();
					int sel_ind = get_selected_index();
					sel_ind = (sel_ind + num_points - 1) % num_points;
					sel3dMgr::CreateUndoOperation();
					sel3dMgr::Select(splnCurveSelect::GetControlPointObject(pSpline, sel_ind));

					//mnmCompassUtil::SetUpCompass(  cmpsCompassMgr::e_Select, 
					//							splnCurveSelect::GetControlPointObject(pSpline, sel_ind) );
					//m_bDisableNotify = false;
				}
			 }

	private: System::Void buttonInsert_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

				//if (!m_bDisableNotify) 
				{
					//m_bDisableNotify = true;
					splnSpline* pSpline = &m_Driver.Spline();
					int num_points = pSpline->GetNumPoints();
					int sel_ind = get_selected_index();
					splnCurveMgr::InsertCtrlPoint(pSpline, sel_ind);
					//m_bDisableNotify = false;
				}
			 }

	private: System::Void buttonAppend_Click(System::Object ^  sender, System::EventArgs ^  e)
			{
				 if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

				//if (!m_bDisableNotify) 
				{
					//m_bDisableNotify = true;
					splnSpline* pSpline = &m_Driver.Spline();
					splnCurveMgr::AppendCtrlPoint(pSpline, m_Driver.GetChannelCurrentPosition());
					//m_bDisableNotify = false;
				}
			}

	private: System::Void button_seteditcam_Click(System::Object ^  sender, System::EventArgs ^  e)
			{
				if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

				//if (!m_bDisableNotify) 
				{
					//m_bDisableNotify = true;
					camCamera& cam = cam3dMgr::GetEditorCamera();

					splnSpline* pSpline = &m_Driver.Spline();
					int sel_ind = get_selected_index();
					
					if ( sel_ind >= 0 )
					{
						cam3dMgr::SetManipPosition( pSpline->GetPointPos( sel_ind ) );
					}
					else
					{
						DBG_WARNING0( "spline point not the selected item" );
					}
					//m_bDisableNotify = false;
				}
			}

	private: System::Void button_setpoint_Click(System::Object ^  sender, System::EventArgs ^  e)
			{
				if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

				//if (!m_bDisableNotify) 
				{
					//m_bDisableNotify = true;
					//m_bOurChange = true;

					camCamera& cam = cam3dMgr::GetEditorCamera();

					splnSpline* pSpline = &m_Driver.Spline();
					int sel_ind = get_selected_index();
			
					if ( sel_ind >= 0 )
					{
						splnCurveMgr::AlterCtrlPoint(pSpline, sel_ind, cam.GetPosition());
					}
					else
					{
						DBG_WARNING0( "spline point not the selected item" );
					}

					//m_bOurChange = false;
					//m_bDisableNotify = false;
				}
			}

	private: int get_selected_index()
			 {
				 int sel_ind = -1;
				 if (&m_Driver == 0) return sel_ind;	// if the user selected something else while this was open, don't let them crash

				 splnSpline* pSpline = &m_Driver.Spline();
				 int num_points = pSpline->GetNumPoints();
				 
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
			return tabPage_spline;
		}

};
}
#endif // _MANAGED
