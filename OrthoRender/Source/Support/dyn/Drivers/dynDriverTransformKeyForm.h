#pragma once

#ifndef DYN_DRIVERTRANSFORMKEY_HPP
#include "Support/dyn/Drivers/dynDriverTransformKey.hpp"
#endif
#ifndef DYN_DRIVERTRANSFORMKEYINFO_HPP
#include "Support/dyn/Drivers/dynDriverTransformKeyInfo.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
#endif
#ifndef MA_CONSTANTS_HPP
#include "Core/ma/maConstants.hpp"
#endif

#ifdef _MANAGED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Support
{
	/// <summary>
	/// Summary for dynDriverTransformKeyForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class dynDriverTransformKeyForm : public System::Windows::Forms::Form
	{
	public:
		dynDriverTransformKeyForm(dynDriverTransformKey &i_Driver)
			: m_Driver(i_Driver)
		{
			m_bDisableNotify = true;
			InitializeComponent();

			tmaManagedConversionUtil::SetPoint3( m_Driver.GetRotation(), vector3EditRanged_rotation );
			tmaManagedConversionUtil::SetPoint3( m_Driver.GetTranslation(), vector3Edit_Translation );
			m_bDisableNotify = false;
		}

	protected:
		~dynDriverTransformKeyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: dynDriverTransformKey &m_Driver;
	private: bool m_bDisableNotify;


	private: System::Windows::Forms::TabControl ^  tabControl_ori;
	private: System::Windows::Forms::TabPage ^  tabPage_ori;

	private: TerawattManagedControls::Vector3EditUpDown ^  vector3Edit_Translation;
	private: TerawattManagedControls::Vector3EditRanged ^  vector3EditRanged_rotation;
	private: System::Windows::Forms::Label ^  label_rotation;
	private: System::Windows::Forms::Label ^  label_translation;

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
			this->label_rotation = gcnew System::Windows::Forms::Label();
			this->tabControl_ori = gcnew System::Windows::Forms::TabControl();
			this->tabPage_ori = gcnew System::Windows::Forms::TabPage();
			this->label_translation = gcnew System::Windows::Forms::Label();
			this->vector3Edit_Translation = gcnew TerawattManagedControls::Vector3EditUpDown();
			this->vector3EditRanged_rotation = gcnew TerawattManagedControls::Vector3EditRanged();
			this->tabControl_ori->SuspendLayout();
			this->tabPage_ori->SuspendLayout();
			this->SuspendLayout();
			// 
			// label_rotation
			// 
			this->label_rotation->Location = System::Drawing::Point(16, 56);
			this->label_rotation->Name = "label_rotation";
			this->label_rotation->Size = System::Drawing::Size(64, 24);
			this->label_rotation->TabIndex = 2;
			this->label_rotation->Text = "Rotation:";
			// 
			// tabControl_ori
			// 
			this->tabControl_ori->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_ori->Controls->Add(this->tabPage_ori);
			this->tabControl_ori->Location = System::Drawing::Point(8, 8);
			this->tabControl_ori->Name = "tabControl_ori";
			this->tabControl_ori->SelectedIndex = 0;
			this->tabControl_ori->Size = System::Drawing::Size(383, 227);
			this->tabControl_ori->TabIndex = 4;
			// 
			// tabPage_ori
			// 
			this->tabPage_ori->Controls->Add(this->label_translation);
			this->tabPage_ori->Controls->Add(this->vector3Edit_Translation);
			this->tabPage_ori->Controls->Add(this->label_rotation);
			this->tabPage_ori->Controls->Add(this->vector3EditRanged_rotation);
			this->tabPage_ori->Location = System::Drawing::Point(4, 22);
			this->tabPage_ori->Name = "tabPage_ori";
			this->tabPage_ori->Size = System::Drawing::Size(375, 201);
			this->tabPage_ori->TabIndex = 0;
			this->tabPage_ori->Text = "Transform Control Key";
			// 
			// label_translation
			// 
			this->label_translation->Location = System::Drawing::Point(16, 16);
			this->label_translation->Name = "label_translation";
			this->label_translation->Size = System::Drawing::Size(64, 24);
			this->label_translation->TabIndex = 4;
			this->label_translation->Text = "Translation:";
			// 
			// vector3Edit_Translation
			// 
			this->vector3Edit_Translation->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->vector3Edit_Translation->Location = System::Drawing::Point(88, 16);
			
			this->vector3Edit_Translation->Name = "vector3Edit_Translation";
			this->vector3Edit_Translation->Precision = (System::Int16)2;
			this->vector3Edit_Translation->Size = System::Drawing::Size(265, 27);
			this->vector3Edit_Translation->TabIndex = 5;
			this->vector3Edit_Translation->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &dynDriverTransformKeyForm::vector3Edit_Translation_KeyPressChild);
			this->vector3Edit_Translation->LeaveChild += gcnew System::EventHandler(this, &dynDriverTransformKeyForm::vector3Edit_Translation_LeaveChild);
			// 
			// vector3EditRanged_rotation
			// 
			this->vector3EditRanged_rotation->Exponent = (System::Int16)1;
			this->vector3EditRanged_rotation->Location = System::Drawing::Point(88, 56);
			this->vector3EditRanged_rotation->Maximum = 180;
			this->vector3EditRanged_rotation->Minimum = -180;
			this->vector3EditRanged_rotation->Name = "vector3EditRanged_rotation";
			this->vector3EditRanged_rotation->NumTicks = (System::Int16)100;
			this->vector3EditRanged_rotation->Precision = (System::Int16)2;
			this->vector3EditRanged_rotation->Size = System::Drawing::Size(264, 128);
			this->vector3EditRanged_rotation->TabIndex = 6;
			this->vector3EditRanged_rotation->KeyPressChild += gcnew System::Windows::Forms::KeyPressEventHandler(this, &dynDriverTransformKeyForm::vector3EditRanged_rotation_KeyPressChild);
			this->vector3EditRanged_rotation->LeaveChild += gcnew System::EventHandler(this, &dynDriverTransformKeyForm::vector3EditRanged_rotation_LeaveChild);
			// 
			// dynDriverTransformKeyForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(392, 238);
			this->Controls->Add(this->tabControl_ori);
			this->Name = "dynDriverTransformKeyForm";
			this->Text = "Transform Control Key Properties";
			this->tabControl_ori->ResumeLayout(false);
			this->tabPage_ori->ResumeLayout(false);
			this->ResumeLayout(false);

		}
		//

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_ori;
		}


private: System::Void vector3EditRanged_rotation_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
			{
				if (!m_bDisableNotify)
				{
					m_bDisableNotify = true;
					maVector3d rot( (float)vector3EditRanged_rotation->ValueX,
									(float)vector3EditRanged_rotation->ValueY,
									(float)vector3EditRanged_rotation->ValueZ );
					m_Driver.SetRotation( rot );
					m_bDisableNotify = false;
				}
			}

private: System::Void vector3EditRanged_rotation_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
 			if (e->KeyChar != (char)13)
				return;
			vector3EditRanged_rotation_ValueChanged(sender,e);
		 }

private: System::Void vector3EditRanged_rotation_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			vector3EditRanged_rotation_ValueChanged(sender,e);
		 }

private: System::Void vector3Edit_Translation_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
			{
				if (!m_bDisableNotify)
				{
					m_bDisableNotify = true;
				maVector3d vec( (float) vector3Edit_Translation->ValueX, 
								(float) vector3Edit_Translation->ValueY, 
								(float) vector3Edit_Translation->ValueZ);
					m_Driver.SetTranslation( vec );
					m_bDisableNotify = false;
				}
			}

private: System::Void vector3Edit_Translation_KeyPressChild(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
 			if (e->KeyChar != (char)13)
				return;
			vector3Edit_Translation_ValueChanged(sender,e);
		 }

private: System::Void vector3Edit_Translation_LeaveChild(System::Object ^  sender, System::EventArgs ^  e)
		 {
			vector3Edit_Translation_ValueChanged(sender,e);
		 }

};
}
#endif // _MANAGED
