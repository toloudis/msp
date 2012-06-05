#pragma once

#ifndef CMRA_DRIVERFRAMEDCLOSEUP_HPP
#include "Systems/Cameras/Drivers/cmraDriverFramedCloseUp.hpp"
#endif
#ifndef CMRA_DRIVERFRAMEDBASEINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverFramedBaseInfo.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "Core/Dbg/dbgLog.hpp"
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
	/// Summary for cmraDriverFramedCloseUpForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmraDriverFramedCloseUpForm : public System::Windows::Forms::Form
	{
	public: 
		static cmraDriverFramedCloseUpForm ^FormInstance = nullptr;
	public:
		cmraDriverFramedCloseUpForm(cmraDriverFramedCloseUp &i_Driver)
			: m_Driver(i_Driver)
		{
			m_bDisableNotify = true;

			InitializeComponent();

			FormInstance = this;

			SetComponentInitialValues();

			m_bDisableNotify = false;
		}

		void UpdateForm()
		{
			SetComponentInitialValues();
			this->Invalidate();
		}

	protected:
		~cmraDriverFramedCloseUpForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: cmraDriverFramedCloseUp &m_Driver;
	private: bool m_bDisableNotify;
	private: System::Windows::Forms::TabControl ^  tabControl_Framed;
	private: System::Windows::Forms::TabPage ^  tabPage_Framed;

	private: System::Windows::Forms::GroupBox ^  groupBox_restrictposition;
	private: System::Windows::Forms::CheckBox ^  checkBox_restrictposition;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_restrictposition_tolerance;
	private: System::Windows::Forms::Label ^  label_restrictposition_tolerance;

	private: System::Windows::Forms::GroupBox ^  groupBox_restrictangle;
	private: System::Windows::Forms::Label ^  label_restrictangle_tolerance;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_restrictangle_tolerance;
	private: System::Windows::Forms::CheckBox ^  checkBox_restrictangle;

	private: System::Windows::Forms::GroupBox ^  groupBox_restrictdistance;
	private: System::Windows::Forms::Label ^  label_restrictdistance_tolerance;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_restrictdistance_tolerance;
	private: System::Windows::Forms::CheckBox ^  checkBox_restrictdistance;

	private: System::Windows::Forms::GroupBox ^  groupBox_restrictdirection;
	private: System::Windows::Forms::Label ^  label_restrictdirection_tolerance;
	private: System::Windows::Forms::CheckBox ^  checkBox_restrictdirection;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_restrictdirection_tolerance;

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
			this->tabControl_Framed = gcnew System::Windows::Forms::TabControl();
			this->tabPage_Framed = gcnew System::Windows::Forms::TabPage();
			this->groupBox_restrictdistance = gcnew System::Windows::Forms::GroupBox();
			this->label_restrictdistance_tolerance = gcnew System::Windows::Forms::Label();
			this->vector3Edit_restrictdistance_tolerance = gcnew TerawattManagedControls::Vector3Edit();
			this->checkBox_restrictdistance = gcnew System::Windows::Forms::CheckBox();
			this->groupBox_restrictposition = gcnew System::Windows::Forms::GroupBox();
			this->label_restrictposition_tolerance = gcnew System::Windows::Forms::Label();
			this->vector3Edit_restrictposition_tolerance = gcnew TerawattManagedControls::Vector3Edit();
			this->checkBox_restrictposition = gcnew System::Windows::Forms::CheckBox();
			this->groupBox_restrictangle = gcnew System::Windows::Forms::GroupBox();
			this->label_restrictangle_tolerance = gcnew System::Windows::Forms::Label();
			this->vector3Edit_restrictangle_tolerance = gcnew TerawattManagedControls::Vector3Edit();
			this->checkBox_restrictangle = gcnew System::Windows::Forms::CheckBox();
			this->groupBox_restrictdirection = gcnew System::Windows::Forms::GroupBox();
			this->floatEdit_restrictdirection_tolerance = gcnew TerawattManagedControls::FloatEdit();
			this->label_restrictdirection_tolerance = gcnew System::Windows::Forms::Label();
			this->checkBox_restrictdirection = gcnew System::Windows::Forms::CheckBox();
			this->tabControl_Framed->SuspendLayout();
			this->tabPage_Framed->SuspendLayout();
			this->groupBox_restrictdistance->SuspendLayout();
			this->groupBox_restrictposition->SuspendLayout();
			this->groupBox_restrictangle->SuspendLayout();
			this->groupBox_restrictdirection->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_Framed
			// 
			this->tabControl_Framed->Controls->Add(this->tabPage_Framed);
			this->tabControl_Framed->Location = System::Drawing::Point(8, 8);
			this->tabControl_Framed->Name = "tabControl_Framed";
			this->tabControl_Framed->SelectedIndex = 0;
			this->tabControl_Framed->Size = System::Drawing::Size(424, 392);
			this->tabControl_Framed->TabIndex = 7;
			// 
			// tabPage_Framed
			// 
			this->tabPage_Framed->Controls->Add(this->groupBox_restrictdistance);
			this->tabPage_Framed->Controls->Add(this->groupBox_restrictposition);
			this->tabPage_Framed->Controls->Add(this->groupBox_restrictangle);
			this->tabPage_Framed->Controls->Add(this->groupBox_restrictdirection);
			this->tabPage_Framed->Location = System::Drawing::Point(4, 22);
			this->tabPage_Framed->Name = "tabPage_Framed";
			this->tabPage_Framed->Size = System::Drawing::Size(416, 366);
			this->tabPage_Framed->TabIndex = 0;
			this->tabPage_Framed->Text = "Framed";
			// 
			// groupBox_restrictdistance
			// 
			this->groupBox_restrictdistance->Controls->Add(this->label_restrictdistance_tolerance);
			this->groupBox_restrictdistance->Controls->Add(this->vector3Edit_restrictdistance_tolerance);
			this->groupBox_restrictdistance->Controls->Add(this->checkBox_restrictdistance);
			this->groupBox_restrictdistance->Location = System::Drawing::Point(8, 184);
			this->groupBox_restrictdistance->Name = "groupBox_restrictdistance";
			this->groupBox_restrictdistance->Size = System::Drawing::Size(392, 80);
			this->groupBox_restrictdistance->TabIndex = 7;
			this->groupBox_restrictdistance->TabStop = false;
			this->groupBox_restrictdistance->Text = "Camera Distance";
			// 
			// label_restrictdistance_tolerance
			// 
			this->label_restrictdistance_tolerance->Location = System::Drawing::Point(8, 52);
			this->label_restrictdistance_tolerance->Name = "label_restrictdistance_tolerance";
			this->label_restrictdistance_tolerance->Size = System::Drawing::Size(100, 16);
			this->label_restrictdistance_tolerance->TabIndex = 5;
			this->label_restrictdistance_tolerance->Text = "Tolerance (units)";
			// 
			// vector3Edit_restrictdistance_tolerance
			// 
			this->vector3Edit_restrictdistance_tolerance->Location = System::Drawing::Point(120, 48);
			this->vector3Edit_restrictdistance_tolerance->Name = "vector3Edit_restrictdistance_tolerance";
			this->vector3Edit_restrictdistance_tolerance->Precision = (System::Int16)2;
			this->vector3Edit_restrictdistance_tolerance->Size = System::Drawing::Size(264, 24);
			this->vector3Edit_restrictdistance_tolerance->TabIndex = 4;
			this->vector3Edit_restrictdistance_tolerance->ValueChanged += gcnew System::EventHandler(this, &cmraDriverFramedCloseUpForm::vector3Edit_restrictdistance_tolerance_ValueChanged);
			// 
			// checkBox_restrictdistance
			// 
			this->checkBox_restrictdistance->Location = System::Drawing::Point(140, 16);
			this->checkBox_restrictdistance->Name = "checkBox_restrictdistance";
			this->checkBox_restrictdistance->Size = System::Drawing::Size(112, 24);
			this->checkBox_restrictdistance->TabIndex = 3;
			this->checkBox_restrictdistance->Text = "Restrict Distance";
			this->checkBox_restrictdistance->CheckedChanged += gcnew System::EventHandler(this, &cmraDriverFramedCloseUpForm::checkBox_restrictdistance_CheckedChanged);
			// 
			// groupBox_restrictposition
			// 
			this->groupBox_restrictposition->Controls->Add(this->label_restrictposition_tolerance);
			this->groupBox_restrictposition->Controls->Add(this->vector3Edit_restrictposition_tolerance);
			this->groupBox_restrictposition->Controls->Add(this->checkBox_restrictposition);
			this->groupBox_restrictposition->Location = System::Drawing::Point(8, 8);
			this->groupBox_restrictposition->Name = "groupBox_restrictposition";
			this->groupBox_restrictposition->Size = System::Drawing::Size(392, 80);
			this->groupBox_restrictposition->TabIndex = 4;
			this->groupBox_restrictposition->TabStop = false;
			this->groupBox_restrictposition->Text = "Camera Position";
			// 
			// label_restrictposition_tolerance
			// 
			this->label_restrictposition_tolerance->Location = System::Drawing::Point(8, 52);
			this->label_restrictposition_tolerance->Name = "label_restrictposition_tolerance";
			this->label_restrictposition_tolerance->Size = System::Drawing::Size(100, 16);
			this->label_restrictposition_tolerance->TabIndex = 5;
			this->label_restrictposition_tolerance->Text = "Tolerance (units)";
			// 
			// vector3Edit_restrictposition_tolerance
			// 
			this->vector3Edit_restrictposition_tolerance->Location = System::Drawing::Point(120, 48);
			this->vector3Edit_restrictposition_tolerance->Name = "vector3Edit_restrictposition_tolerance";
			this->vector3Edit_restrictposition_tolerance->Precision = (System::Int16)2;
			this->vector3Edit_restrictposition_tolerance->Size = System::Drawing::Size(264, 24);
			this->vector3Edit_restrictposition_tolerance->TabIndex = 4;
			this->vector3Edit_restrictposition_tolerance->ValueChanged += gcnew System::EventHandler(this, &cmraDriverFramedCloseUpForm::vector3Edit_restrictposition_tolerance_ValueChanged);
			// 
			// checkBox_restrictposition
			// 
			this->checkBox_restrictposition->Location = System::Drawing::Point(140, 16);
			this->checkBox_restrictposition->Name = "checkBox_restrictposition";
			this->checkBox_restrictposition->Size = System::Drawing::Size(112, 24);
			this->checkBox_restrictposition->TabIndex = 3;
			this->checkBox_restrictposition->Text = "Restrict Position";
			this->checkBox_restrictposition->CheckedChanged += gcnew System::EventHandler(this, &cmraDriverFramedCloseUpForm::checkBox_restrictposition_CheckedChanged);
			// 
			// groupBox_restrictangle
			// 
			this->groupBox_restrictangle->Controls->Add(this->label_restrictangle_tolerance);
			this->groupBox_restrictangle->Controls->Add(this->vector3Edit_restrictangle_tolerance);
			this->groupBox_restrictangle->Controls->Add(this->checkBox_restrictangle);
			this->groupBox_restrictangle->Location = System::Drawing::Point(8, 96);
			this->groupBox_restrictangle->Name = "groupBox_restrictangle";
			this->groupBox_restrictangle->Size = System::Drawing::Size(392, 80);
			this->groupBox_restrictangle->TabIndex = 6;
			this->groupBox_restrictangle->TabStop = false;
			this->groupBox_restrictangle->Text = "Camera Angle";
			// 
			// label_restrictangle_tolerance
			// 
			this->label_restrictangle_tolerance->Location = System::Drawing::Point(8, 52);
			this->label_restrictangle_tolerance->Name = "label_restrictangle_tolerance";
			this->label_restrictangle_tolerance->Size = System::Drawing::Size(100, 16);
			this->label_restrictangle_tolerance->TabIndex = 5;
			this->label_restrictangle_tolerance->Text = "Tolerance (degs)";
			// 
			// vector3Edit_restrictangle_tolerance
			// 
			this->vector3Edit_restrictangle_tolerance->Location = System::Drawing::Point(120, 48);
			this->vector3Edit_restrictangle_tolerance->Name = "vector3Edit_restrictangle_tolerance";
			this->vector3Edit_restrictangle_tolerance->Precision = (System::Int16)2;
			this->vector3Edit_restrictangle_tolerance->Size = System::Drawing::Size(264, 24);
			this->vector3Edit_restrictangle_tolerance->TabIndex = 4;
			this->vector3Edit_restrictangle_tolerance->ValueChanged += gcnew System::EventHandler(this, &cmraDriverFramedCloseUpForm::vector3Edit_restrictangle_tolerance_ValueChanged);
			// 
			// checkBox_restrictangle
			// 
			this->checkBox_restrictangle->Location = System::Drawing::Point(140, 16);
			this->checkBox_restrictangle->Name = "checkBox_restrictangle";
			this->checkBox_restrictangle->Size = System::Drawing::Size(112, 24);
			this->checkBox_restrictangle->TabIndex = 3;
			this->checkBox_restrictangle->Text = "Restrict Angle";
			this->checkBox_restrictangle->CheckedChanged += gcnew System::EventHandler(this, &cmraDriverFramedCloseUpForm::checkBox_restrictangle_CheckedChanged);
			// 
			// groupBox_restrictdirection
			// 
			this->groupBox_restrictdirection->Controls->Add(this->floatEdit_restrictdirection_tolerance);
			this->groupBox_restrictdirection->Controls->Add(this->label_restrictdirection_tolerance);
			this->groupBox_restrictdirection->Controls->Add(this->checkBox_restrictdirection);
			this->groupBox_restrictdirection->Location = System::Drawing::Point(8, 272);
			this->groupBox_restrictdirection->Name = "groupBox_restrictdirection";
			this->groupBox_restrictdirection->Size = System::Drawing::Size(392, 80);
			this->groupBox_restrictdirection->TabIndex = 8;
			this->groupBox_restrictdirection->TabStop = false;
			this->groupBox_restrictdirection->Text = "Camera Direction";
			// 
			// floatEdit_restrictdirection_tolerance
			// 
			this->floatEdit_restrictdirection_tolerance->Location = System::Drawing::Point(120, 48);
			this->floatEdit_restrictdirection_tolerance->Name = "floatEdit_restrictdirection_tolerance";
			this->floatEdit_restrictdirection_tolerance->Precision = (System::Int16)2;
			this->floatEdit_restrictdirection_tolerance->Size = System::Drawing::Size(88, 24);
			this->floatEdit_restrictdirection_tolerance->TabIndex = 6;
			this->floatEdit_restrictdirection_tolerance->ValueChanged += gcnew System::EventHandler(this, &cmraDriverFramedCloseUpForm::floatEdit_restrictdirection_tolerance_ValueChanged);
			// 
			// label_restrictdirection_tolerance
			// 
			this->label_restrictdirection_tolerance->Location = System::Drawing::Point(8, 52);
			this->label_restrictdirection_tolerance->Name = "label_restrictdirection_tolerance";
			this->label_restrictdirection_tolerance->Size = System::Drawing::Size(100, 16);
			this->label_restrictdirection_tolerance->TabIndex = 5;
			this->label_restrictdirection_tolerance->Text = "Tolerance (degs)";
			// 
			// checkBox_restrictdirection
			// 
			this->checkBox_restrictdirection->Location = System::Drawing::Point(140, 16);
			this->checkBox_restrictdirection->Name = "checkBox_restrictdirection";
			this->checkBox_restrictdirection->Size = System::Drawing::Size(112, 24);
			this->checkBox_restrictdirection->TabIndex = 3;
			this->checkBox_restrictdirection->Text = "Restrict Direction";
			this->checkBox_restrictdirection->CheckedChanged += gcnew System::EventHandler(this, &cmraDriverFramedCloseUpForm::checkBox_restrictdirection_CheckedChanged);
			// 
			// cmraDriverFramedCloseUpForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(440, 405);
			this->Controls->Add(this->tabControl_Framed);
			this->Name = "cmraDriverFramedCloseUpForm";
			this->Text = "Framed Shot Properties";
			this->TopMost = true;
			this->tabControl_Framed->ResumeLayout(false);
			this->tabPage_Framed->ResumeLayout(false);
			this->groupBox_restrictdistance->ResumeLayout(false);
			this->groupBox_restrictposition->ResumeLayout(false);
			this->groupBox_restrictangle->ResumeLayout(false);
			this->groupBox_restrictdirection->ResumeLayout(false);
			this->ResumeLayout(false);

		}
		//


private: System::Void checkBox_restrictposition_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				cmraDriverFramedBaseInfo *pInfo = dynamic_cast<cmraDriverFramedBaseInfo*>(m_Driver.GetDriverInfo());

				pInfo->m_bPositionRestrict = checkBox_restrictposition->Checked;

				m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);
				delete pInfo;
				m_bDisableNotify = false;
			}
		 }

private: System::Void vector3Edit_restrictposition_tolerance_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				cmraDriverFramedBaseInfo *pInfo = dynamic_cast<cmraDriverFramedBaseInfo*>(m_Driver.GetDriverInfo());

				pInfo->m_PositionTolerance.SetX( (float)vector3Edit_restrictposition_tolerance->ValueX );
				pInfo->m_PositionTolerance.SetY( (float)vector3Edit_restrictposition_tolerance->ValueY );
				pInfo->m_PositionTolerance.SetZ( (float)vector3Edit_restrictposition_tolerance->ValueZ );

				m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);
				delete pInfo;
				m_bDisableNotify = false;
			}
		 }

private: System::Void checkBox_restrictangle_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				cmraDriverFramedBaseInfo *pInfo = dynamic_cast<cmraDriverFramedBaseInfo*>(m_Driver.GetDriverInfo());

				pInfo->m_bAngleRestrict = checkBox_restrictangle->Checked;

				m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);
				delete pInfo;
				m_bDisableNotify = false;
			}
		 }

private: System::Void vector3Edit_restrictangle_tolerance_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				cmraDriverFramedBaseInfo *pInfo = dynamic_cast<cmraDriverFramedBaseInfo*>(m_Driver.GetDriverInfo());

				pInfo->m_AngleTolerance.SetX( (float)vector3Edit_restrictangle_tolerance->ValueX );
				pInfo->m_AngleTolerance.SetY( (float)vector3Edit_restrictangle_tolerance->ValueY );
				pInfo->m_AngleTolerance.SetZ( (float)vector3Edit_restrictangle_tolerance->ValueZ );

				m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);
				delete pInfo;
				m_bDisableNotify = false;
			}
		 }

private: System::Void checkBox_restrictdistance_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				cmraDriverFramedBaseInfo *pInfo = dynamic_cast<cmraDriverFramedBaseInfo*>(m_Driver.GetDriverInfo());

				pInfo->m_bDistanceRestrict = checkBox_restrictdistance->Checked;

				m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);
				delete pInfo;
				m_bDisableNotify = false;
			}
		 }

private: System::Void vector3Edit_restrictdistance_tolerance_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				cmraDriverFramedBaseInfo *pInfo = dynamic_cast<cmraDriverFramedBaseInfo*>(m_Driver.GetDriverInfo());

				pInfo->m_DistanceTolerance.SetX( (float)vector3Edit_restrictdistance_tolerance->ValueX );
				pInfo->m_DistanceTolerance.SetY( (float)vector3Edit_restrictdistance_tolerance->ValueY );
				pInfo->m_DistanceTolerance.SetZ( (float)vector3Edit_restrictdistance_tolerance->ValueZ );

				m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);
				delete pInfo;
				m_bDisableNotify = false;
			}
		 }

private: System::Void checkBox_restrictdirection_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				cmraDriverFramedBaseInfo *pInfo = dynamic_cast<cmraDriverFramedBaseInfo*>(m_Driver.GetDriverInfo());

				pInfo->m_bDirectionRestrict = checkBox_restrictdirection->Checked;

				m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);
				delete pInfo;
				m_bDisableNotify = false;
			}
		 }

private: System::Void floatEdit_restrictdirection_tolerance_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				cmraDriverFramedBaseInfo *pInfo = dynamic_cast<cmraDriverFramedBaseInfo*>(m_Driver.GetDriverInfo());

				pInfo->m_fDirectionTolerance = (float)floatEdit_restrictdirection_tolerance->Value;

				m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);
				delete pInfo;
				m_bDisableNotify = false;
			}
		 }

private:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void SetComponentInitialValues()
	{
		cmraDriverFramedBaseInfo *pInfo = dynamic_cast<cmraDriverFramedBaseInfo*>(m_Driver.GetDriverInfo());

		checkBox_restrictposition->Checked				= pInfo->m_bPositionRestrict;
		vector3Edit_restrictposition_tolerance->ValueX	= pInfo->m_PositionTolerance.GetX();
		vector3Edit_restrictposition_tolerance->ValueY	= pInfo->m_PositionTolerance.GetY();
		vector3Edit_restrictposition_tolerance->ValueZ	= pInfo->m_PositionTolerance.GetZ();

		checkBox_restrictangle->Checked					= pInfo->m_bAngleRestrict;
		vector3Edit_restrictangle_tolerance->ValueX		= pInfo->m_AngleTolerance.GetX();
		vector3Edit_restrictangle_tolerance->ValueY		= pInfo->m_AngleTolerance.GetY();
		vector3Edit_restrictangle_tolerance->ValueZ		= pInfo->m_AngleTolerance.GetZ();

		checkBox_restrictdistance->Checked				= pInfo->m_bDistanceRestrict;
		vector3Edit_restrictdistance_tolerance->ValueX	= pInfo->m_DistanceTolerance.GetX();
		vector3Edit_restrictdistance_tolerance->ValueY	= pInfo->m_DistanceTolerance.GetY();
		vector3Edit_restrictdistance_tolerance->ValueZ	= pInfo->m_DistanceTolerance.GetZ();

		checkBox_restrictdirection->Checked				= pInfo->m_bDirectionRestrict;
		floatEdit_restrictdirection_tolerance->Value	= pInfo->m_fDirectionTolerance;

		delete pInfo;
	}

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_Framed;
		}

		//	add a tab page
		System::Void AddTabPage( System::Windows::Forms::TabPage^ i_pPage )
		{
			tabControl_Framed->Controls->Add( i_pPage );
		}

};
}
#endif // _MANAGED
