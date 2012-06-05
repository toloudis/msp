/********************************************************************************************\
**  fogFogDataForm.h
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#pragma once
#ifndef FOG_FOGDATA_HPP
#include "Systems/Fog/Data/fogFogData.hpp"
#endif
#ifndef FOG_OPERATIONS_HPP
#include "Systems/Fog/Undo/fogOperations.hpp"
#endif

#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
#endif

#ifdef _MANAGED

//============================================================================
//============================================================================
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace GeneratedForms
{
	/// <summary>
	/// Summary for fogFogDataForm
	///
	/// WARNING: If you change the name of this class, yada yada...
	/// </summary>
	public ref class fogFogDataForm : public System::Windows::Forms::Form
	{
	public:
		static fogFogDataForm^ FormInstance = nullptr;

		fogFogDataForm()
			: m_bSelfEdit(false)
		{
			m_bDisableNotify = true;
			InitializeComponent();
			m_bDisableNotify = false;
		}

		// Call this to update dialog to new data
		void Update(const fogFogData &i_Data)
		{
			SetupData(i_Data);
		}

	protected:
		~fogFogDataForm()
		{
			// clear instance
			if (fogFogDataForm::FormInstance == this)
				fogFogDataForm::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;
	private: System::Windows::Forms::TabControl ^  tabControl_fog;
	private: System::Windows::Forms::TabPage ^  tabPage_fog;

	private: TerawattManagedControls::ColorRGBEdit ^  colorRGBEdit1;
	private: System::Windows::Forms::GroupBox ^  groupBox_fogparams;
	private: System::Windows::Forms::GroupBox ^  groupBox_fogcolor;
	private: System::Windows::Forms::GroupBox ^  groupBox_fogmode;
	private: System::Windows::Forms::Label ^  label_mode;
	private: System::Windows::Forms::Label ^  label_color;
	private: System::Windows::Forms::Label ^  label_density;
	private: System::Windows::Forms::Label ^  label_start;
	private: System::Windows::Forms::Label ^  label_end;
	private: System::Windows::Forms::Label ^  label_densitymax;
	private: System::Windows::Forms::Label ^  label_startmax;
	private: System::Windows::Forms::Label ^  label_endmax;
	private: System::Windows::Forms::ComboBox ^  comboBox_mode;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_density;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_start;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_end;

		// Holds reference to data, changing the data
		// within the caller's structure
	private: bool m_bDisableNotify;
	private: bool m_bSelfEdit;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl_fog = gcnew System::Windows::Forms::TabControl();
			this->tabPage_fog = gcnew System::Windows::Forms::TabPage();
			this->label_mode = gcnew System::Windows::Forms::Label();
			this->comboBox_mode = gcnew System::Windows::Forms::ComboBox();
			this->label_color = gcnew System::Windows::Forms::Label();
			this->colorRGBEdit1 = gcnew TerawattManagedControls::ColorRGBEdit();
			this->label_density = gcnew System::Windows::Forms::Label();
			this->rangedFloat_density = gcnew TerawattManagedControls::RangedFloat();
			this->label_start = gcnew System::Windows::Forms::Label();
			this->rangedFloat_start = gcnew TerawattManagedControls::RangedFloat();
			this->label_end = gcnew System::Windows::Forms::Label();
			this->rangedFloat_end = gcnew TerawattManagedControls::RangedFloat();
			this->groupBox_fogparams = gcnew System::Windows::Forms::GroupBox();
			this->groupBox_fogcolor = gcnew System::Windows::Forms::GroupBox();
			this->groupBox_fogmode = gcnew System::Windows::Forms::GroupBox();
			this->label_densitymax = gcnew System::Windows::Forms::Label();
			this->label_startmax = gcnew System::Windows::Forms::Label();
			this->label_endmax = gcnew System::Windows::Forms::Label();
			this->tabControl_fog->SuspendLayout();
			this->tabPage_fog->SuspendLayout();
			this->groupBox_fogparams->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_fog
			// 
			this->tabControl_fog->Controls->Add(this->tabPage_fog);
			this->tabControl_fog->Location = System::Drawing::Point(8, 8);
			this->tabControl_fog->Name = "tabControl_fog";
			this->tabControl_fog->SelectedIndex = 0;
			this->tabControl_fog->Size = System::Drawing::Size(352, 296);
			this->tabControl_fog->TabIndex = 3;
			// 
			// tabPage_fog
			// 
			this->tabPage_fog->Controls->Add(this->label_mode);
			this->tabPage_fog->Controls->Add(this->comboBox_mode);
			this->tabPage_fog->Controls->Add(this->label_color);
			this->tabPage_fog->Controls->Add(this->colorRGBEdit1);
			this->tabPage_fog->Controls->Add(this->label_density);
			this->tabPage_fog->Controls->Add(this->rangedFloat_density);
			this->tabPage_fog->Controls->Add(this->label_start);
			this->tabPage_fog->Controls->Add(this->label_end);
			this->tabPage_fog->Controls->Add(this->rangedFloat_end);
			this->tabPage_fog->Controls->Add(this->groupBox_fogcolor);
			this->tabPage_fog->Controls->Add(this->groupBox_fogmode);
			this->tabPage_fog->Controls->Add(this->rangedFloat_start);
			this->tabPage_fog->Controls->Add(this->groupBox_fogparams);
			this->tabPage_fog->Location = System::Drawing::Point(4, 22);
			this->tabPage_fog->Name = "tabPage_fog";
			this->tabPage_fog->Size = System::Drawing::Size(344, 270);
			this->tabPage_fog->TabIndex = 0;
			this->tabPage_fog->Text = "Fog";
			// 
			// label_mode
			// 
			this->label_mode->Location = System::Drawing::Point(80, 32);
			this->label_mode->Name = "label_mode";
			this->label_mode->Size = System::Drawing::Size(48, 20);
			this->label_mode->TabIndex = 10;
			this->label_mode->Text = "Mode:";
			// 
			// comboBox_mode
			// 
			this->comboBox_mode->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox_mode->Items->AddRange(gcnew cli::array< System::Object^ >(4)
				{ "None", "Linear", "Exponential", "ExponentialSq" });
			this->comboBox_mode->Location = System::Drawing::Point(136, 32);
			this->comboBox_mode->Name = "comboBox_mode";
			this->comboBox_mode->Size = System::Drawing::Size(127, 21);
			this->comboBox_mode->TabIndex = 11;
			this->comboBox_mode->SelectedIndexChanged += gcnew System::EventHandler(this, &fogFogDataForm::comboBox_mode_SelectedIndexChanged);
			// 
			// label_color
			// 
			this->label_color->Location = System::Drawing::Point(48, 96);
			this->label_color->Name = "label_color";
			this->label_color->Size = System::Drawing::Size(32, 21);
			this->label_color->TabIndex = 12;
			this->label_color->Text = "Color:";
			// 
			// colorRGBEdit1
			// 
			this->colorRGBEdit1->Color = System::Drawing::Color::FromArgb((System::Byte)128, (System::Byte)128, (System::Byte)128);
			this->colorRGBEdit1->Location = System::Drawing::Point(88, 96);
			this->colorRGBEdit1->Name = "colorRGBEdit1";
			this->colorRGBEdit1->Size = System::Drawing::Size(224, 24);
			this->colorRGBEdit1->TabIndex = 13;
			this->colorRGBEdit1->ValueChanged += gcnew System::EventHandler(this, &fogFogDataForm::colorRGBEdit1_ValueChanged);
			// 
			// label_density
			// 
			this->label_density->Location = System::Drawing::Point(16, 160);
			this->label_density->Name = "label_density";
			this->label_density->Size = System::Drawing::Size(73, 21);
			this->label_density->TabIndex = 14;
			this->label_density->Text = "Density:";
			// 
			// rangedFloat_density
			// 
			this->rangedFloat_density->Exponent = (System::Int16)1;
			this->rangedFloat_density->Location = System::Drawing::Point(96, 160);
			this->rangedFloat_density->Maximum = 0.25;
			this->rangedFloat_density->Name = "rangedFloat_density";
			this->rangedFloat_density->NumTicks = (System::Int16)100;
			this->rangedFloat_density->Precision = (System::Int16)2;
			this->rangedFloat_density->Size = System::Drawing::Size(187, 21);
			this->rangedFloat_density->TabIndex = 15;
			this->rangedFloat_density->ValueChanged += gcnew System::EventHandler(this, &fogFogDataForm::rangedFloat_density_ValueChanged);
			// 
			// label_start
			// 
			this->label_start->Location = System::Drawing::Point(16, 192);
			this->label_start->Name = "label_start";
			this->label_start->Size = System::Drawing::Size(73, 20);
			this->label_start->TabIndex = 16;
			this->label_start->Text = "Start:";
			// 
			// rangedFloat_start
			// 
			this->rangedFloat_start->Exponent = (System::Int16)1;
			this->rangedFloat_start->Location = System::Drawing::Point(96, 192);
			this->rangedFloat_start->Maximum = 2000;
			this->rangedFloat_start->Minimum = 1;
			this->rangedFloat_start->Name = "rangedFloat_start";
			this->rangedFloat_start->NumTicks = (System::Int16)100;
			this->rangedFloat_start->Precision = (System::Int16)2;
			this->rangedFloat_start->Size = System::Drawing::Size(187, 20);
			this->rangedFloat_start->TabIndex = 17;
			this->rangedFloat_start->ValueChanged += gcnew System::EventHandler(this, &fogFogDataForm::rangedFloat_start_ValueChanged);
			// 
			// label_end
			// 
			this->label_end->Location = System::Drawing::Point(16, 224);
			this->label_end->Name = "label_end";
			this->label_end->Size = System::Drawing::Size(73, 21);
			this->label_end->TabIndex = 18;
			this->label_end->Text = "End:";
			// 
			// rangedFloat_end
			// 
			this->rangedFloat_end->Exponent = (System::Int16)1;
			this->rangedFloat_end->Location = System::Drawing::Point(96, 224);
			this->rangedFloat_end->Maximum = 2000;
			this->rangedFloat_end->Minimum = 1;
			this->rangedFloat_end->Name = "rangedFloat_end";
			this->rangedFloat_end->NumTicks = (System::Int16)100;
			this->rangedFloat_end->Precision = (System::Int16)2;
			this->rangedFloat_end->Size = System::Drawing::Size(187, 21);
			this->rangedFloat_end->TabIndex = 19;
			this->rangedFloat_end->ValueChanged += gcnew System::EventHandler(this, &fogFogDataForm::rangedFloat_end_ValueChanged);
			// 
			// groupBox_fogparams
			// 
			this->groupBox_fogparams->Controls->Add(this->label_endmax);
			this->groupBox_fogparams->Controls->Add(this->label_startmax);
			this->groupBox_fogparams->Controls->Add(this->label_densitymax);
			this->groupBox_fogparams->Location = System::Drawing::Point(8, 144);
			this->groupBox_fogparams->Name = "groupBox_fogparams";
			this->groupBox_fogparams->Size = System::Drawing::Size(328, 112);
			this->groupBox_fogparams->TabIndex = 20;
			this->groupBox_fogparams->TabStop = false;
			this->groupBox_fogparams->Text = "Parameters";
			// 
			// groupBox_fogcolor
			// 
			this->groupBox_fogcolor->Location = System::Drawing::Point(8, 80);
			this->groupBox_fogcolor->Name = "groupBox_fogcolor";
			this->groupBox_fogcolor->Size = System::Drawing::Size(328, 48);
			this->groupBox_fogcolor->TabIndex = 21;
			this->groupBox_fogcolor->TabStop = false;
			this->groupBox_fogcolor->Text = "Color";
			// 
			// groupBox_fogmode
			// 
			this->groupBox_fogmode->Location = System::Drawing::Point(8, 8);
			this->groupBox_fogmode->Name = "groupBox_fogmode";
			this->groupBox_fogmode->Size = System::Drawing::Size(328, 56);
			this->groupBox_fogmode->TabIndex = 22;
			this->groupBox_fogmode->TabStop = false;
			this->groupBox_fogmode->Text = "Type";
			// 
			// label_densitymax
			// 
			this->label_densitymax->Location = System::Drawing::Point(280, 16);
			this->label_densitymax->Name = "label_densitymax";
			this->label_densitymax->Size = System::Drawing::Size(40, 24);
			this->label_densitymax->TabIndex = 0;
			this->label_densitymax->Text = "max";
			// 
			// label_startmax
			// 
			this->label_startmax->Location = System::Drawing::Point(280, 48);
			this->label_startmax->Name = "label_startmax";
			this->label_startmax->Size = System::Drawing::Size(40, 24);
			this->label_startmax->TabIndex = 1;
			this->label_startmax->Text = "max";
			// 
			// label_endmax
			// 
			this->label_endmax->Location = System::Drawing::Point(280, 80);
			this->label_endmax->Name = "label_endmax";
			this->label_endmax->Size = System::Drawing::Size(40, 24);
			this->label_endmax->TabIndex = 2;
			this->label_endmax->Text = "max";
			// 
			// fogFogDataForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(368, 317);
			this->Controls->Add(this->tabControl_fog);
			this->Name = "fogFogDataForm";
			this->Text = "Fog Data Form";
			this->tabControl_fog->ResumeLayout(false);
			this->tabPage_fog->ResumeLayout(false);
			this->groupBox_fogparams->ResumeLayout(false);
			this->ResumeLayout(false);

		}

		//

		void SetupData(const fogFogData &i_Data)
		{
			if (m_bSelfEdit) return;

			m_bDisableNotify = true;
			comboBox_mode->SelectedIndex = i_Data.m_Mode.GetValue();
			colorRGBEdit1->Color		= tmaManagedConversionUtil::SetColorRGB(i_Data.m_Color.GetValue());
			rangedFloat_density->Value	= i_Data.m_Density.GetValue();
			rangedFloat_start->Value	= i_Data.m_Start.GetValue();
			rangedFloat_end->Value		= i_Data.m_End.GetValue();

			label_densitymax->Text	= rangedFloat_density->Maximum.ToString();
			label_startmax->Text	= rangedFloat_start->Maximum.ToString();
			label_endmax->Text		= rangedFloat_end->Maximum.ToString();
			m_bDisableNotify = false;
		}

		System::Void comboBox_mode_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			if (!m_bDisableNotify)
				fogOperations::ChangeFogMode(fogFogData::FogMode(comboBox_mode->SelectedIndex));
		}

		System::Void colorRGBEdit1_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			if (!m_bDisableNotify)
				fogOperations::ChangeFogColor(tmaManagedConversionUtil::ConvertColorRGB(colorRGBEdit1->Color));
		}

		System::Void rangedFloat_density_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			if (!m_bDisableNotify)
			{
				m_bSelfEdit = true;
				fogOperations::ChangeFogDensity((float)rangedFloat_density->Value);
				m_bSelfEdit = false;
			}
		}

		System::Void rangedFloat_start_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			if (!m_bDisableNotify)
			{
				m_bSelfEdit = true;
				fogOperations::ChangeFogStart((float)rangedFloat_start->Value);
				m_bSelfEdit = false;
			}
		}

		System::Void rangedFloat_end_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			if (!m_bDisableNotify)
			{
				m_bSelfEdit = true;
				fogOperations::ChangeFogEnd((float)rangedFloat_end->Value);
				m_bSelfEdit = false;
			}
		}

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_fog;
		}
	};
}
#endif // _MANAGED
