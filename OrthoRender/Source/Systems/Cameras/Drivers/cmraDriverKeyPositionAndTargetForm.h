#pragma once

#ifndef CMRA_DRIVERKEYPOSITIONANDTARGET_HPP
#include "Systems/Cameras/Drivers/cmraDriverKeyPositionAndTarget.hpp"
#endif
#ifndef CMRA_DRIVERKEYPOSITIONANDTARGETINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverKeyPositionAndTargetInfo.hpp"
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
	/// Summary for cmraDriverKeyPositionAndTargetForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmraDriverKeyPositionAndTargetForm : public System::Windows::Forms::Form
	{
	public: 
		static cmraDriverKeyPositionAndTargetForm ^FormInstance = nullptr;
	public:
		cmraDriverKeyPositionAndTargetForm(cmraDriverKeyPositionAndTarget &i_Driver)
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
		~cmraDriverKeyPositionAndTargetForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: cmraDriverKeyPositionAndTarget &m_Driver;
	private: bool m_bDisableNotify;

	private: TerawattManagedControls::Vector3EditUpDown ^  vector3Edit_position;
	private: System::Windows::Forms::Label ^  label_position;
	private: System::Windows::Forms::Label ^  label_target;
	private: TerawattManagedControls::Vector3EditUpDown ^  vector3Edit_target;
	private: System::Windows::Forms::TabControl ^  tabControl_camkey;
	private: System::Windows::Forms::TabPage ^  tabPage_camkey;

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
			this->tabControl_camkey = gcnew System::Windows::Forms::TabControl();
			this->tabPage_camkey = gcnew System::Windows::Forms::TabPage();
			this->vector3Edit_target = gcnew TerawattManagedControls::Vector3EditUpDown();
			this->label_target = gcnew System::Windows::Forms::Label();
			this->tabControl_camkey->SuspendLayout();
			this->tabPage_camkey->SuspendLayout();
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
			this->vector3Edit_position->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &cmraDriverKeyPositionAndTargetForm::vector3Edit_position_KeyPressChild);
			this->vector3Edit_position->LeaveChild += gcnew System::EventHandler(this, &cmraDriverKeyPositionAndTargetForm::vector3Edit_position_LeaveChild);
			// 
			// label_position
			// 
			this->label_position->Location = System::Drawing::Point(40, 16);
			this->label_position->Name = "label_position";
			this->label_position->Size = System::Drawing::Size(56, 16);
			this->label_position->TabIndex = 5;
			this->label_position->Text = "Position:";
			// 
			// tabControl_camkey
			// 
			this->tabControl_camkey->Controls->Add(this->tabPage_camkey);
			this->tabControl_camkey->Location = System::Drawing::Point(8, 8);
			this->tabControl_camkey->Name = "tabControl_camkey";
			this->tabControl_camkey->SelectedIndex = 0;
			this->tabControl_camkey->Size = System::Drawing::Size(424, 152);
			this->tabControl_camkey->TabIndex = 7;
			// 
			// tabPage_camkey
			// 
			this->tabPage_camkey->Controls->Add(this->vector3Edit_target);
			this->tabPage_camkey->Controls->Add(this->label_target);
			this->tabPage_camkey->Controls->Add(this->vector3Edit_position);
			this->tabPage_camkey->Controls->Add(this->label_position);
			this->tabPage_camkey->Location = System::Drawing::Point(4, 22);
			this->tabPage_camkey->Name = "tabPage_camkey";
			this->tabPage_camkey->Size = System::Drawing::Size(416, 126);
			this->tabPage_camkey->TabIndex = 0;
			this->tabPage_camkey->Text = "Camera Key";
			// 
			// vector3Edit_target
			// 
			this->vector3Edit_target->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->vector3Edit_target->Increment = System::Decimal(0.1f);
			this->vector3Edit_target->Location = System::Drawing::Point(104, 48);
			this->vector3Edit_target->Name = "vector3Edit_target";
			this->vector3Edit_target->Precision = (System::Int16)2;
			this->vector3Edit_target->Size = System::Drawing::Size(288, 28);
			this->vector3Edit_target->TabIndex = 7;
			this->vector3Edit_target->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &cmraDriverKeyPositionAndTargetForm::vector3Edit_target_KeyPressChild);
			this->vector3Edit_target->LeaveChild += gcnew System::EventHandler(this, &cmraDriverKeyPositionAndTargetForm::vector3Edit_target_LeaveChild);
			// 
			// label_target
			// 
			this->label_target->Location = System::Drawing::Point(40, 54);
			this->label_target->Name = "label_target";
			this->label_target->Size = System::Drawing::Size(56, 16);
			this->label_target->TabIndex = 6;
			this->label_target->Text = "Target:";
			// 
			// cmraDriverKeyPositionAndTargetForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(440, 165);
			this->Controls->Add(this->tabControl_camkey);
			this->Name = "cmraDriverKeyPositionAndTargetForm";
			this->Text = "Camera Key Properties";
			this->TopMost = true;
			this->tabControl_camkey->ResumeLayout(false);
			this->tabPage_camkey->ResumeLayout(false);
			this->ResumeLayout(false);

		}
		//

private: System::Void vector3Edit_position_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
				if (!m_bDisableNotify)
				{
					m_bDisableNotify = true;
					cmraDriverKeyPositionAndTargetInfo *pInfo = dynamic_cast<cmraDriverKeyPositionAndTargetInfo*>(m_Driver.GetDriverInfo());
					pInfo->m_CameraKeyPosition.Set( (float)this->vector3Edit_position->ValueX, (float)this->vector3Edit_position->ValueY, (float)this->vector3Edit_position->ValueZ );
					m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);
					delete pInfo;
					m_bDisableNotify = false;
				}
		 }

private: System::Void vector3Edit_target_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
				if (!m_bDisableNotify)
				{
					m_bDisableNotify = true;
					cmraDriverKeyPositionAndTargetInfo *pInfo = dynamic_cast<cmraDriverKeyPositionAndTargetInfo*>(m_Driver.GetDriverInfo());
					pInfo->m_CameraKeyTarget.Set( (float)this->vector3Edit_target->ValueX, (float)this->vector3Edit_target->ValueY, (float)this->vector3Edit_target->ValueZ );
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
			cmraDriverKeyPositionAndTargetInfo *pInfo = dynamic_cast<cmraDriverKeyPositionAndTargetInfo*>( m_Driver.GetDriverInfo() );

			tmaManagedConversionUtil::SetPoint3(pInfo->m_CameraKeyPosition, this->vector3Edit_position);
			tmaManagedConversionUtil::SetPoint3(pInfo->m_CameraKeyTarget, this->vector3Edit_target);

			delete pInfo;
		}

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_camkey;
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

private: System::Void vector3Edit_target_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
 			if (e->KeyChar != (char)13)
				return;
			vector3Edit_target_ValueChanged(sender,e);
		 }

private: System::Void vector3Edit_target_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			vector3Edit_target_ValueChanged(sender,e);
		 }

};
}
#endif // _MANAGED
