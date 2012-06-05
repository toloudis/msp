#pragma once

#ifndef LTST_LIGHTSETMGR_HPP
#include "Support/ltst/ltstLightSetMgr.hpp"
#endif
#ifndef LSET_OPERATIONS_HPP
#include "Systems/LightSets/Undo/lsetOperations.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
#endif

#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace SystemLightSets
{
	/// <summary> 
	/// Summary for lsetLightSetAmbientForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class lsetLightSetAmbientForm : public System::Windows::Forms::Form
	{
	public: 
		lsetLightSetAmbientForm(const nameString &i_LightSetName)
			: m_LightSetName(i_LightSetName)
		{
			InitializeComponent();

			m_bDisableNotify = true;
			labelLightSetName->Text = gcnew System::String(i_LightSetName.GetString().c_str());
			maFloatRGBA color = 
				(i_LightSetName.IsEmpty()) ? ltstLightSetMgr::GetSceneAmbientLight() : ltstLightSetMgr::GetLightSetAmbientLight(i_LightSetName);
			colorRGBEditAmbient->Color = tmaManagedConversionUtil::SetColorRGB(color);
			m_bDisableNotify = false;
		}
        
	public: 
		~lsetLightSetAmbientForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label ^  labelLightSetName;
	private: TerawattManagedControls::ColorRGBEdit ^  colorRGBEditAmbient;
	private: System::Windows::Forms::Label ^  label1;


		// Turns off notify callbacks when setting up form
	private: bool m_bDisableNotify;
	private: const nameString &m_LightSetName;

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
			this->labelLightSetName = gcnew System::Windows::Forms::Label();
			this->colorRGBEditAmbient = gcnew TerawattManagedControls::ColorRGBEdit();
			this->label1 = gcnew System::Windows::Forms::Label();
			this->SuspendLayout();
			// 
			// labelLightSetName
			// 
			this->labelLightSetName->Location = System::Drawing::Point(8, 8);
			this->labelLightSetName->Name = "labelLightSetName";
			this->labelLightSetName->Size = System::Drawing::Size(264, 16);
			this->labelLightSetName->TabIndex = 2;
			// 
			// colorRGBEditAmbient
			// 
			this->colorRGBEditAmbient->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->colorRGBEditAmbient->Color = System::Drawing::Color::FromArgb((System::Byte)128, (System::Byte)128, (System::Byte)128);
			this->colorRGBEditAmbient->Location = System::Drawing::Point(8, 56);
			this->colorRGBEditAmbient->Name = "colorRGBEditAmbient";
			this->colorRGBEditAmbient->Size = System::Drawing::Size(292, 32);
			this->colorRGBEditAmbient->TabIndex = 3;
			this->colorRGBEditAmbient->ValueChanged += gcnew System::EventHandler(this, &lsetLightSetAmbientForm::colorRGBEditAmbient_ValueChanged);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(16, 32);
			this->label1->Name = "label1";
			this->label1->Size = System::Drawing::Size(192, 16);
			this->label1->TabIndex = 4;
			this->label1->Text = "Ambient Color";
			// 
			// lsetLightSetAmbientForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(312, 118);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->colorRGBEditAmbient);
			this->Controls->Add(this->labelLightSetName);
			this->Name = "lsetLightSetAmbientForm";
			this->Text = "Light Set Ambient Color";
			this->ResumeLayout(false);

		}		
		//

	private: System::Void colorRGBEditAmbient_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
			 {
				if (!m_bDisableNotify) {
					maFloatRGBA color = tmaManagedConversionUtil::ConvertColorRGB(colorRGBEditAmbient->Color);
					lsetOperations::ChangeAmbientColor(m_LightSetName, color);
				}
			 }

	};
}
#endif // _MANAGED
