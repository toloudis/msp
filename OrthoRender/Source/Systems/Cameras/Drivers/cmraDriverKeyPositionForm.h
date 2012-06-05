#pragma once

#ifndef CMRA_DRIVERKEYPOSITION_HPP
#include "Systems/Cameras/Drivers/cmraDriverKeyPosition.hpp"
#endif
#ifndef CMRA_DRIVERKEYPOSITIONINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverKeyPositionInfo.hpp"
#endif
#ifndef CMRA_OBJECTMGR_HPP
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "Core/Dbg/dbgLog.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
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
	/// Summary for cmraDriverKeyPositionForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmraDriverKeyPositionForm : public System::Windows::Forms::Form
	{
	public: 
		static cmraDriverKeyPositionForm ^FormInstance = nullptr;
	public:
		cmraDriverKeyPositionForm(cmraDriverKeyPosition &i_Driver)
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
		~cmraDriverKeyPositionForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: cmraDriverKeyPosition &m_Driver;
	private: bool m_bDisableNotify;

	private: TerawattManagedControls::Vector3EditUpDown ^  vector3Edit_position;
	private: System::Windows::Forms::Label ^  label_position;
	private: System::Windows::Forms::TabControl ^  tabControl_camkeyPos;
	private: System::Windows::Forms::TabPage ^  tabPage_camkeyPos;

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
			this->vector3Edit_position = gcnew TerawattManagedControls::Vector3EditUpDown();
			this->label_position = gcnew System::Windows::Forms::Label();
			this->tabControl_camkeyPos = gcnew System::Windows::Forms::TabControl();
			this->tabPage_camkeyPos = gcnew System::Windows::Forms::TabPage();
			this->tabControl_camkeyPos->SuspendLayout();
			this->tabPage_camkeyPos->SuspendLayout();
			this->SuspendLayout();
			// 
			// vector3Edit_position
			// 
			this->vector3Edit_position->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->vector3Edit_position->Increment = System::Decimal(0.1f);
			this->vector3Edit_position->Location = System::Drawing::Point(104, 12);
			this->vector3Edit_position->Name = "vector3Edit_position";
			this->vector3Edit_position->Precision = (System::Int16)2;
			this->vector3Edit_position->Size = System::Drawing::Size(288, 24);
			this->vector3Edit_position->TabIndex = 4;
			this->vector3Edit_position->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &cmraDriverKeyPositionForm::vector3Edit_position_KeyPressChild);
			this->vector3Edit_position->LeaveChild += gcnew System::EventHandler(this, &cmraDriverKeyPositionForm::vector3Edit_position_LeaveChild);
			// 
			// label_position
			// 
			this->label_position->Location = System::Drawing::Point(40, 16);
			this->label_position->Name = "label_position";
			this->label_position->Size = System::Drawing::Size(56, 16);
			this->label_position->TabIndex = 5;
			this->label_position->Text = "Position:";
			// 
			// tabControl_camkeyPos
			// 
			this->tabControl_camkeyPos->Controls->Add(this->tabPage_camkeyPos);
			this->tabControl_camkeyPos->Location = System::Drawing::Point(8, 8);
			this->tabControl_camkeyPos->Name = "tabControl_camkeyPos";
			this->tabControl_camkeyPos->SelectedIndex = 0;
			this->tabControl_camkeyPos->Size = System::Drawing::Size(424, 152);
			this->tabControl_camkeyPos->TabIndex = 7;
			// 
			// tabPage_camkeyPos
			// 
			this->tabPage_camkeyPos->Controls->Add(this->vector3Edit_position);
			this->tabPage_camkeyPos->Controls->Add(this->label_position);
			this->tabPage_camkeyPos->Location = System::Drawing::Point(4, 22);
			this->tabPage_camkeyPos->Name = "tabPage_camkeyPos";
			this->tabPage_camkeyPos->Size = System::Drawing::Size(416, 126);
			this->tabPage_camkeyPos->TabIndex = 0;
			this->tabPage_camkeyPos->Text = "Camera Key Pos";
			// 
			// cmraDriverKeyPositionForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(440, 165);
			this->Controls->Add(this->tabControl_camkeyPos);
			this->Name = "cmraDriverKeyPositionForm";
			this->Text = "Camera Key Position Properties";
			this->TopMost = true;
			this->tabControl_camkeyPos->ResumeLayout(false);
			this->tabPage_camkeyPos->ResumeLayout(false);
			this->ResumeLayout(false);

		}
		//

private: System::Void vector3Edit_position_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
				if (!m_bDisableNotify)
				{
					m_bDisableNotify = true;
					cmraDriverKeyPositionInfo *pInfo = dynamic_cast<cmraDriverKeyPositionInfo*>(m_Driver.GetDriverInfo());
					pInfo->m_CameraKeyPosition.Set( (float)this->vector3Edit_position->ValueX, (float)this->vector3Edit_position->ValueY, (float)this->vector3Edit_position->ValueZ );
					m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);
					delete pInfo;
					m_bDisableNotify = false;
				}
		 }

private:
		//
		void SetUpComponents()
		{
			//
			cmraDriverKeyPositionInfo *pInfo = dynamic_cast<cmraDriverKeyPositionInfo*>( m_Driver.GetDriverInfo() );

			tmaManagedConversionUtil::SetPoint3(pInfo->m_CameraKeyPosition, this->vector3Edit_position);

			delete pInfo;
		}

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_camkeyPos;
		}
private: System::Void vector3Edit_position_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
 			if (e->KeyChar != (char)13)
				return;
			vector3Edit_position_ValueChanged(sender,e);
		 }

private: System::Void vector3Edit_position_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			vector3Edit_position_ValueChanged(sender,e);
		 }

};
}
#endif // _MANAGED
