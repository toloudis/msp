#pragma once

#ifndef CMRA_DRIVERFRAMEDARC_HPP
#include "Systems/Cameras/Drivers/cmraDriverFramedArc.hpp"
#endif
#ifndef CMRA_DRIVERFRAMEDARCINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverFramedArcInfo.hpp"
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
	/// Summary for cmraDriverFramedArcForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmraDriverFramedArcForm : public System::Windows::Forms::Form
	{
	public: 
		static cmraDriverFramedArcForm ^FormInstance = nullptr;
	public:
		cmraDriverFramedArcForm(cmraDriverFramedArc &i_Driver)
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
		~cmraDriverFramedArcForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: cmraDriverFramedArc &m_Driver;
	private: bool m_bDisableNotify;
	private: System::Windows::Forms::TabControl ^  tabControl_arc;
	private: System::Windows::Forms::TabPage ^  tabPage_arc;

	private: System::Windows::Forms::CheckBox ^  checkBox_clockwise;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_revolutions;
	private: System::Windows::Forms::Label ^  label_revolutions;


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
			this->tabControl_arc = gcnew System::Windows::Forms::TabControl();
			this->tabPage_arc = gcnew System::Windows::Forms::TabPage();
			this->floatEdit_revolutions = gcnew TerawattManagedControls::FloatEdit();
			this->checkBox_clockwise = gcnew System::Windows::Forms::CheckBox();
			this->label_revolutions = gcnew System::Windows::Forms::Label();
			this->tabControl_arc->SuspendLayout();
			this->tabPage_arc->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_arc
			// 
			this->tabControl_arc->Controls->Add(this->tabPage_arc);
			this->tabControl_arc->Location = System::Drawing::Point(8, 8);
			this->tabControl_arc->Name = "tabControl_arc";
			this->tabControl_arc->SelectedIndex = 0;
			this->tabControl_arc->Size = System::Drawing::Size(424, 256);
			this->tabControl_arc->TabIndex = 7;
			// 
			// tabPage_arc
			// 
			this->tabPage_arc->Controls->Add(this->floatEdit_revolutions);
			this->tabPage_arc->Controls->Add(this->checkBox_clockwise);
			this->tabPage_arc->Controls->Add(this->label_revolutions);
			this->tabPage_arc->Location = System::Drawing::Point(4, 22);
			this->tabPage_arc->Name = "tabPage_arc";
			this->tabPage_arc->Size = System::Drawing::Size(416, 230);
			this->tabPage_arc->TabIndex = 0;
			this->tabPage_arc->Text = "Arc";
			// 
			// floatEdit_revolutions
			// 
			this->floatEdit_revolutions->Location = System::Drawing::Point(104, 16);
			this->floatEdit_revolutions->Name = "floatEdit_revolutions";
			this->floatEdit_revolutions->Precision = (System::Int16)2;
			this->floatEdit_revolutions->Size = System::Drawing::Size(112, 24);
			this->floatEdit_revolutions->TabIndex = 4;
			this->floatEdit_revolutions->Value = 1.5;
			this->floatEdit_revolutions->ValueChanged += gcnew System::EventHandler(this, &cmraDriverFramedArcForm::floatEdit_revolutions_ValueChanged);
			// 
			// checkBox_clockwise
			// 
			this->checkBox_clockwise->Checked = true;
			this->checkBox_clockwise->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_clockwise->Location = System::Drawing::Point(24, 48);
			this->checkBox_clockwise->Name = "checkBox_clockwise";
			this->checkBox_clockwise->Size = System::Drawing::Size(80, 24);
			this->checkBox_clockwise->TabIndex = 3;
			this->checkBox_clockwise->Text = "Clockwise";
			this->checkBox_clockwise->CheckedChanged += gcnew System::EventHandler(this, &cmraDriverFramedArcForm::checkBox_clockwise_CheckedChanged);
			// 
			// label_revolutions
			// 
			this->label_revolutions->Location = System::Drawing::Point(16, 18);
			this->label_revolutions->Name = "label_revolutions";
			this->label_revolutions->Size = System::Drawing::Size(80, 16);
			this->label_revolutions->TabIndex = 0;
			this->label_revolutions->Text = "Revolutions";
			// 
			// cmraDriverFramedArcForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(440, 277);
			this->Controls->Add(this->tabControl_arc);
			this->Name = "cmraDriverFramedArcForm";
			this->Text = "Arc Shot Properties";
			this->TopMost = true;
			this->tabControl_arc->ResumeLayout(false);
			this->tabPage_arc->ResumeLayout(false);
			this->ResumeLayout(false);

		}
		//


private: System::Void checkBox_clockwise_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				cmraDriverFramedArcInfo *pInfo = dynamic_cast<cmraDriverFramedArcInfo*>(m_Driver.GetDriverInfo());

				pInfo->m_bClockwise = checkBox_clockwise->Checked;

				m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);
				delete pInfo;
				m_bDisableNotify = false;
			}
		 }

private: System::Void floatEdit_revolutions_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				cmraDriverFramedArcInfo *pInfo = dynamic_cast<cmraDriverFramedArcInfo*>(m_Driver.GetDriverInfo());

				pInfo->m_fRevolutions = (float) this->floatEdit_revolutions->Value;

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
		cmraDriverFramedArcInfo *pInfo = dynamic_cast<cmraDriverFramedArcInfo*>(m_Driver.GetDriverInfo());

		//DBG_LOG2( "revolutions %6.3f  %6.3f", pInfo->m_fRevolutions, floatEdit_revolutions->Value );

		floatEdit_revolutions->Value = pInfo->m_fRevolutions;

		delete pInfo;
	}

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_arc;
		}

		//	add a tab page
		System::Void AddTabPage( System::Windows::Forms::TabPage^ i_pPage )
		{
			tabControl_arc->Controls->Add( i_pPage );
		}

};
}
#endif // _MANAGED
