#pragma once

#ifndef CMRA_DRIVERKEYTARGET_HPP
#include "Systems/Cameras/Drivers/cmraDriverKeyTarget.hpp"
#endif
#ifndef CMRA_DRIVERKEYTARGETINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverKeyTargetInfo.hpp"
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
	/// Summary for cmraDriverKeyTargetForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmraDriverKeyTargetForm : public System::Windows::Forms::Form
	{
	public: 
		static cmraDriverKeyTargetForm ^FormInstance = nullptr;
	public:
		cmraDriverKeyTargetForm(cmraDriverKeyTarget &i_Driver)
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
		~cmraDriverKeyTargetForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: cmraDriverKeyTarget &m_Driver;
	private: bool m_bDisableNotify;

	private: System::Windows::Forms::Label ^  label_target;
	private: TerawattManagedControls::Vector3EditUpDown ^  vector3Edit_target;
	private: System::Windows::Forms::TabControl ^  tabControl_camkeyTar;
	private: System::Windows::Forms::TabPage ^  tabPage_camkeyTar;

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
			this->tabControl_camkeyTar = gcnew System::Windows::Forms::TabControl();
			this->tabPage_camkeyTar = gcnew System::Windows::Forms::TabPage();
			this->vector3Edit_target = gcnew TerawattManagedControls::Vector3EditUpDown();
			this->label_target = gcnew System::Windows::Forms::Label();
			this->tabControl_camkeyTar->SuspendLayout();
			this->tabPage_camkeyTar->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_camkeyTar
			// 
			this->tabControl_camkeyTar->Controls->Add(this->tabPage_camkeyTar);
			this->tabControl_camkeyTar->Location = System::Drawing::Point(8, 8);
			this->tabControl_camkeyTar->Name = "tabControl_camkeyTar";
			this->tabControl_camkeyTar->SelectedIndex = 0;
			this->tabControl_camkeyTar->Size = System::Drawing::Size(424, 152);
			this->tabControl_camkeyTar->TabIndex = 7;
			// 
			// tabPage_camkeyTar
			// 
			this->tabPage_camkeyTar->Controls->Add(this->vector3Edit_target);
			this->tabPage_camkeyTar->Controls->Add(this->label_target);
			this->tabPage_camkeyTar->Location = System::Drawing::Point(4, 22);
			this->tabPage_camkeyTar->Name = "tabPage_camkeyTar";
			this->tabPage_camkeyTar->Size = System::Drawing::Size(416, 126);
			this->tabPage_camkeyTar->TabIndex = 0;
			this->tabPage_camkeyTar->Text = "Camera Key Target";
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
			this->vector3Edit_target->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &cmraDriverKeyTargetForm::vector3Edit_target_KeyPressChild);
			this->vector3Edit_target->LeaveChild += gcnew System::EventHandler(this, &cmraDriverKeyTargetForm::vector3Edit_target_LeaveChild);
			// 
			// label_target
			// 
			this->label_target->Location = System::Drawing::Point(40, 54);
			this->label_target->Name = "label_target";
			this->label_target->Size = System::Drawing::Size(56, 16);
			this->label_target->TabIndex = 6;
			this->label_target->Text = "Target:";
			// 
			// cmraDriverKeyTargetForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(440, 165);
			this->Controls->Add(this->tabControl_camkeyTar);
			this->Name = "cmraDriverKeyTargetForm";
			this->Text = "Camera Key Properties";
			this->TopMost = true;
			this->tabControl_camkeyTar->ResumeLayout(false);
			this->tabPage_camkeyTar->ResumeLayout(false);
			this->ResumeLayout(false);

		}
		//

private: System::Void vector3Edit_target_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
				if (!m_bDisableNotify)
				{
					m_bDisableNotify = true;
					cmraDriverKeyTargetInfo *pInfo = dynamic_cast<cmraDriverKeyTargetInfo*>(m_Driver.GetDriverInfo());
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
			cmraDriverKeyTargetInfo *pInfo = dynamic_cast<cmraDriverKeyTargetInfo*>( m_Driver.GetDriverInfo() );

			tmaManagedConversionUtil::SetPoint3(pInfo->m_CameraKeyTarget, this->vector3Edit_target);

			delete pInfo;
		}

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_camkeyTar;
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
