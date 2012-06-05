#pragma once

#ifndef MTR_LIGHTMGR_HPP
#include "mcpLightMgr.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "tmaManagedConversionUtil.hpp"
#endif

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace MotionAnalysisStudio
{
	/// <summary> 
	/// Summary for LightsDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class LightsDialog : public System::Windows::Forms::Form
	{
	public: 
		static LightsDialog *FormInstance = 0;

		LightsDialog(void)
		{
			InitializeComponent();
			SetupData();
		}
        
	protected: 
		void Dispose(Boolean disposing)
		{
			// clear instance
			if (disposing && LightsDialog::FormInstance == this)
				LightsDialog::FormInstance = 0;

			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}

	private: bool m_bDisableNotify;
	private: System::Windows::Forms::GroupBox *  groupDirLight;
	private: System::Windows::Forms::Label *  label1;

	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::Label *  label3;
	private: TerawattManagedControls::ColorRGBEdit *  colorDir1;
	private: TerawattManagedControls::RangedFloat *  rangeDir1Yaw;
	private: TerawattManagedControls::RangedFloat *  rangeDir1Pitch;
	private: System::Windows::Forms::CheckBox *  checkDir1Shadow;
	private: System::Windows::Forms::GroupBox *  groupPointLight1;
	private: TerawattManagedControls::RangedFloat *  rangePoint1Yaw;
	private: TerawattManagedControls::RangedFloat *  rangePoint1Pitch;
	private: System::Windows::Forms::Label *  label4;
	private: System::Windows::Forms::Label *  label5;
	private: System::Windows::Forms::CheckBox *  checkPoint1Shadow;
	private: System::Windows::Forms::Label *  label6;
	private: TerawattManagedControls::ColorRGBEdit *  colorPoint1;
	private: System::Windows::Forms::Label *  label7;
	private: TerawattManagedControls::RangedFloat *  rangePoint1Radius;
	private: System::Windows::Forms::CheckBox *  checkDir1Enabled;
	private: System::Windows::Forms::CheckBox *  checkPoint1Enabled;
	private: System::Windows::Forms::GroupBox *  groupBox1;
	private: System::Windows::Forms::CheckBox *  checkProject1Enabled;
	private: TerawattManagedControls::RangedFloat *  rangeProject1Radius;
	private: System::Windows::Forms::Label *  label8;
	private: TerawattManagedControls::RangedFloat *  rangeProject1Yaw;
	private: TerawattManagedControls::RangedFloat *  rangeProject1Pitch;
	private: System::Windows::Forms::Label *  label9;
	private: System::Windows::Forms::Label *  label10;
	private: System::Windows::Forms::CheckBox *  checkProject1Shadow;
	private: System::Windows::Forms::Label *  label11;
	private: TerawattManagedControls::ColorRGBEdit *  colorProject1;



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container* components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->colorDir1 = new TerawattManagedControls::ColorRGBEdit();
			this->groupDirLight = new System::Windows::Forms::GroupBox();
			this->checkDir1Enabled = new System::Windows::Forms::CheckBox();
			this->rangeDir1Yaw = new TerawattManagedControls::RangedFloat();
			this->rangeDir1Pitch = new TerawattManagedControls::RangedFloat();
			this->label3 = new System::Windows::Forms::Label();
			this->label2 = new System::Windows::Forms::Label();
			this->checkDir1Shadow = new System::Windows::Forms::CheckBox();
			this->label1 = new System::Windows::Forms::Label();
			this->groupPointLight1 = new System::Windows::Forms::GroupBox();
			this->checkPoint1Enabled = new System::Windows::Forms::CheckBox();
			this->rangePoint1Radius = new TerawattManagedControls::RangedFloat();
			this->label7 = new System::Windows::Forms::Label();
			this->rangePoint1Yaw = new TerawattManagedControls::RangedFloat();
			this->rangePoint1Pitch = new TerawattManagedControls::RangedFloat();
			this->label4 = new System::Windows::Forms::Label();
			this->label5 = new System::Windows::Forms::Label();
			this->checkPoint1Shadow = new System::Windows::Forms::CheckBox();
			this->label6 = new System::Windows::Forms::Label();
			this->colorPoint1 = new TerawattManagedControls::ColorRGBEdit();
			this->groupBox1 = new System::Windows::Forms::GroupBox();
			this->checkProject1Enabled = new System::Windows::Forms::CheckBox();
			this->rangeProject1Radius = new TerawattManagedControls::RangedFloat();
			this->label8 = new System::Windows::Forms::Label();
			this->rangeProject1Yaw = new TerawattManagedControls::RangedFloat();
			this->rangeProject1Pitch = new TerawattManagedControls::RangedFloat();
			this->label9 = new System::Windows::Forms::Label();
			this->label10 = new System::Windows::Forms::Label();
			this->checkProject1Shadow = new System::Windows::Forms::CheckBox();
			this->label11 = new System::Windows::Forms::Label();
			this->colorProject1 = new TerawattManagedControls::ColorRGBEdit();
			this->groupDirLight->SuspendLayout();
			this->groupPointLight1->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// colorDir1
			// 
			this->colorDir1->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->colorDir1->Color = System::Drawing::Color::FromArgb((System::Byte)128, (System::Byte)128, (System::Byte)128);
			this->colorDir1->Location = System::Drawing::Point(80, 24);
			this->colorDir1->Name = S"colorDir1";
			this->colorDir1->Size = System::Drawing::Size(328, 24);
			this->colorDir1->TabIndex = 0;
			this->colorDir1->ValueChanged += new System::EventHandler(this, &LightsDialog::colorDir1_ValueChanged);
			// 
			// groupDirLight
			// 
			this->groupDirLight->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupDirLight->Controls->Add(this->checkDir1Enabled);
			this->groupDirLight->Controls->Add(this->rangeDir1Yaw);
			this->groupDirLight->Controls->Add(this->rangeDir1Pitch);
			this->groupDirLight->Controls->Add(this->label3);
			this->groupDirLight->Controls->Add(this->label2);
			this->groupDirLight->Controls->Add(this->checkDir1Shadow);
			this->groupDirLight->Controls->Add(this->label1);
			this->groupDirLight->Controls->Add(this->colorDir1);
			this->groupDirLight->Location = System::Drawing::Point(8, 8);
			this->groupDirLight->Name = S"groupDirLight";
			this->groupDirLight->Size = System::Drawing::Size(424, 168);
			this->groupDirLight->TabIndex = 1;
			this->groupDirLight->TabStop = false;
			this->groupDirLight->Text = S"Directional Light";
			// 
			// checkDir1Enabled
			// 
			this->checkDir1Enabled->Location = System::Drawing::Point(240, 136);
			this->checkDir1Enabled->Name = S"checkDir1Enabled";
			this->checkDir1Enabled->Size = System::Drawing::Size(136, 24);
			this->checkDir1Enabled->TabIndex = 7;
			this->checkDir1Enabled->Text = S"Enabled";
			this->checkDir1Enabled->CheckedChanged += new System::EventHandler(this, &LightsDialog::checkDir1Enabled_CheckedChanged);
			// 
			// rangeDir1Yaw
			// 
			this->rangeDir1Yaw->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->rangeDir1Yaw->Exponent = (System::Int16)1;
			this->rangeDir1Yaw->Location = System::Drawing::Point(72, 96);
			this->rangeDir1Yaw->Maximum = 180;
			this->rangeDir1Yaw->Minimum = -180;
			this->rangeDir1Yaw->Name = S"rangeDir1Yaw";
			this->rangeDir1Yaw->Precision = (System::Int16)2;
			this->rangeDir1Yaw->Size = System::Drawing::Size(336, 40);
			this->rangeDir1Yaw->TabIndex = 6;
			this->rangeDir1Yaw->ValueChanged += new System::EventHandler(this, &LightsDialog::rangeDir1Yaw_ValueChanged);
			// 
			// rangeDir1Pitch
			// 
			this->rangeDir1Pitch->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->rangeDir1Pitch->Exponent = (System::Int16)1;
			this->rangeDir1Pitch->Location = System::Drawing::Point(72, 56);
			this->rangeDir1Pitch->Maximum = 90;
			this->rangeDir1Pitch->Minimum = -90;
			this->rangeDir1Pitch->Name = S"rangeDir1Pitch";
			this->rangeDir1Pitch->Precision = (System::Int16)2;
			this->rangeDir1Pitch->Size = System::Drawing::Size(336, 40);
			this->rangeDir1Pitch->TabIndex = 5;
			this->rangeDir1Pitch->ValueChanged += new System::EventHandler(this, &LightsDialog::rangeDir1Pitch_ValueChanged);
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 104);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(56, 24);
			this->label3->TabIndex = 4;
			this->label3->Text = S"Yaw";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 64);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(56, 24);
			this->label2->TabIndex = 3;
			this->label2->Text = S"Pitch";
			// 
			// checkDir1Shadow
			// 
			this->checkDir1Shadow->Checked = true;
			this->checkDir1Shadow->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkDir1Shadow->Location = System::Drawing::Point(88, 136);
			this->checkDir1Shadow->Name = S"checkDir1Shadow";
			this->checkDir1Shadow->Size = System::Drawing::Size(144, 24);
			this->checkDir1Shadow->TabIndex = 2;
			this->checkDir1Shadow->Text = S"Casts Shadow";
			this->checkDir1Shadow->CheckedChanged += new System::EventHandler(this, &LightsDialog::checkDir1Shadow_CheckedChanged);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 24);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(56, 24);
			this->label1->TabIndex = 1;
			this->label1->Text = S"Color";
			// 
			// groupPointLight1
			// 
			this->groupPointLight1->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupPointLight1->Controls->Add(this->checkPoint1Enabled);
			this->groupPointLight1->Controls->Add(this->rangePoint1Radius);
			this->groupPointLight1->Controls->Add(this->label7);
			this->groupPointLight1->Controls->Add(this->rangePoint1Yaw);
			this->groupPointLight1->Controls->Add(this->rangePoint1Pitch);
			this->groupPointLight1->Controls->Add(this->label4);
			this->groupPointLight1->Controls->Add(this->label5);
			this->groupPointLight1->Controls->Add(this->checkPoint1Shadow);
			this->groupPointLight1->Controls->Add(this->label6);
			this->groupPointLight1->Controls->Add(this->colorPoint1);
			this->groupPointLight1->Location = System::Drawing::Point(8, 184);
			this->groupPointLight1->Name = S"groupPointLight1";
			this->groupPointLight1->Size = System::Drawing::Size(424, 216);
			this->groupPointLight1->TabIndex = 2;
			this->groupPointLight1->TabStop = false;
			this->groupPointLight1->Text = S"Point Light #1";
			// 
			// checkPoint1Enabled
			// 
			this->checkPoint1Enabled->Location = System::Drawing::Point(240, 184);
			this->checkPoint1Enabled->Name = S"checkPoint1Enabled";
			this->checkPoint1Enabled->Size = System::Drawing::Size(136, 24);
			this->checkPoint1Enabled->TabIndex = 9;
			this->checkPoint1Enabled->Text = S"Enabled";
			this->checkPoint1Enabled->CheckedChanged += new System::EventHandler(this, &LightsDialog::checkPoint1Enabled_CheckedChanged);
			// 
			// rangePoint1Radius
			// 
			this->rangePoint1Radius->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->rangePoint1Radius->Exponent = (System::Int16)2;
			this->rangePoint1Radius->Location = System::Drawing::Point(72, 136);
			this->rangePoint1Radius->Maximum = 100;
			this->rangePoint1Radius->Name = S"rangePoint1Radius";
			this->rangePoint1Radius->Precision = (System::Int16)2;
			this->rangePoint1Radius->Size = System::Drawing::Size(336, 40);
			this->rangePoint1Radius->TabIndex = 8;
			this->rangePoint1Radius->ValueChanged += new System::EventHandler(this, &LightsDialog::rangePoint1Radius_ValueChanged);
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(8, 144);
			this->label7->Name = S"label7";
			this->label7->Size = System::Drawing::Size(56, 24);
			this->label7->TabIndex = 7;
			this->label7->Text = S"Radius";
			// 
			// rangePoint1Yaw
			// 
			this->rangePoint1Yaw->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->rangePoint1Yaw->Exponent = (System::Int16)1;
			this->rangePoint1Yaw->Location = System::Drawing::Point(72, 96);
			this->rangePoint1Yaw->Maximum = 180;
			this->rangePoint1Yaw->Minimum = -180;
			this->rangePoint1Yaw->Name = S"rangePoint1Yaw";
			this->rangePoint1Yaw->Precision = (System::Int16)2;
			this->rangePoint1Yaw->Size = System::Drawing::Size(336, 40);
			this->rangePoint1Yaw->TabIndex = 6;
			this->rangePoint1Yaw->ValueChanged += new System::EventHandler(this, &LightsDialog::rangePoint1Yaw_ValueChanged);
			// 
			// rangePoint1Pitch
			// 
			this->rangePoint1Pitch->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->rangePoint1Pitch->Exponent = (System::Int16)1;
			this->rangePoint1Pitch->Location = System::Drawing::Point(72, 56);
			this->rangePoint1Pitch->Maximum = 90;
			this->rangePoint1Pitch->Minimum = -90;
			this->rangePoint1Pitch->Name = S"rangePoint1Pitch";
			this->rangePoint1Pitch->Precision = (System::Int16)2;
			this->rangePoint1Pitch->Size = System::Drawing::Size(336, 40);
			this->rangePoint1Pitch->TabIndex = 5;
			this->rangePoint1Pitch->ValueChanged += new System::EventHandler(this, &LightsDialog::rangePoint1Pitch_ValueChanged);
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(8, 104);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(56, 24);
			this->label4->TabIndex = 4;
			this->label4->Text = S"Yaw";
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(8, 64);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(56, 24);
			this->label5->TabIndex = 3;
			this->label5->Text = S"Pitch";
			// 
			// checkPoint1Shadow
			// 
			this->checkPoint1Shadow->Checked = true;
			this->checkPoint1Shadow->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkPoint1Shadow->Location = System::Drawing::Point(88, 184);
			this->checkPoint1Shadow->Name = S"checkPoint1Shadow";
			this->checkPoint1Shadow->Size = System::Drawing::Size(144, 24);
			this->checkPoint1Shadow->TabIndex = 2;
			this->checkPoint1Shadow->Text = S"Casts Shadow";
			this->checkPoint1Shadow->CheckedChanged += new System::EventHandler(this, &LightsDialog::checkPoint1Shadow_CheckedChanged);
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(8, 24);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(56, 24);
			this->label6->TabIndex = 1;
			this->label6->Text = S"Color";
			// 
			// colorPoint1
			// 
			this->colorPoint1->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->colorPoint1->Color = System::Drawing::Color::FromArgb((System::Byte)128, (System::Byte)128, (System::Byte)128);
			this->colorPoint1->Location = System::Drawing::Point(80, 24);
			this->colorPoint1->Name = S"colorPoint1";
			this->colorPoint1->Size = System::Drawing::Size(328, 24);
			this->colorPoint1->TabIndex = 0;
			this->colorPoint1->ValueChanged += new System::EventHandler(this, &LightsDialog::colorPoint1_ValueChanged);
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupBox1->Controls->Add(this->checkProject1Enabled);
			this->groupBox1->Controls->Add(this->rangeProject1Radius);
			this->groupBox1->Controls->Add(this->label8);
			this->groupBox1->Controls->Add(this->rangeProject1Yaw);
			this->groupBox1->Controls->Add(this->rangeProject1Pitch);
			this->groupBox1->Controls->Add(this->label9);
			this->groupBox1->Controls->Add(this->label10);
			this->groupBox1->Controls->Add(this->checkProject1Shadow);
			this->groupBox1->Controls->Add(this->label11);
			this->groupBox1->Controls->Add(this->colorProject1);
			this->groupBox1->Location = System::Drawing::Point(8, 408);
			this->groupBox1->Name = S"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(424, 216);
			this->groupBox1->TabIndex = 3;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = S"Projected Light";
			// 
			// checkProject1Enabled
			// 
			this->checkProject1Enabled->Location = System::Drawing::Point(240, 184);
			this->checkProject1Enabled->Name = S"checkProject1Enabled";
			this->checkProject1Enabled->Size = System::Drawing::Size(136, 24);
			this->checkProject1Enabled->TabIndex = 9;
			this->checkProject1Enabled->Text = S"Enabled";
			this->checkProject1Enabled->CheckedChanged += new System::EventHandler(this, &LightsDialog::checkProject1Enabled_CheckedChanged);
			// 
			// rangeProject1Radius
			// 
			this->rangeProject1Radius->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->rangeProject1Radius->Exponent = (System::Int16)2;
			this->rangeProject1Radius->Location = System::Drawing::Point(72, 136);
			this->rangeProject1Radius->Maximum = 100;
			this->rangeProject1Radius->Name = S"rangeProject1Radius";
			this->rangeProject1Radius->Precision = (System::Int16)2;
			this->rangeProject1Radius->Size = System::Drawing::Size(336, 40);
			this->rangeProject1Radius->TabIndex = 8;
			this->rangeProject1Radius->ValueChanged += new System::EventHandler(this, &LightsDialog::rangeProject1Radius_ValueChanged);
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(8, 144);
			this->label8->Name = S"label8";
			this->label8->Size = System::Drawing::Size(56, 24);
			this->label8->TabIndex = 7;
			this->label8->Text = S"Radius";
			// 
			// rangeProject1Yaw
			// 
			this->rangeProject1Yaw->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->rangeProject1Yaw->Exponent = (System::Int16)1;
			this->rangeProject1Yaw->Location = System::Drawing::Point(72, 96);
			this->rangeProject1Yaw->Maximum = 180;
			this->rangeProject1Yaw->Minimum = -180;
			this->rangeProject1Yaw->Name = S"rangeProject1Yaw";
			this->rangeProject1Yaw->Precision = (System::Int16)2;
			this->rangeProject1Yaw->Size = System::Drawing::Size(336, 40);
			this->rangeProject1Yaw->TabIndex = 6;
			this->rangeProject1Yaw->ValueChanged += new System::EventHandler(this, &LightsDialog::rangeProject1Yaw_ValueChanged);
			// 
			// rangeProject1Pitch
			// 
			this->rangeProject1Pitch->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->rangeProject1Pitch->Exponent = (System::Int16)1;
			this->rangeProject1Pitch->Location = System::Drawing::Point(72, 56);
			this->rangeProject1Pitch->Maximum = 90;
			this->rangeProject1Pitch->Minimum = -90;
			this->rangeProject1Pitch->Name = S"rangeProject1Pitch";
			this->rangeProject1Pitch->Precision = (System::Int16)2;
			this->rangeProject1Pitch->Size = System::Drawing::Size(336, 40);
			this->rangeProject1Pitch->TabIndex = 5;
			this->rangeProject1Pitch->ValueChanged += new System::EventHandler(this, &LightsDialog::rangeProject1Pitch_ValueChanged);
			// 
			// label9
			// 
			this->label9->Location = System::Drawing::Point(8, 104);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(56, 24);
			this->label9->TabIndex = 4;
			this->label9->Text = S"Yaw";
			// 
			// label10
			// 
			this->label10->Location = System::Drawing::Point(8, 64);
			this->label10->Name = S"label10";
			this->label10->Size = System::Drawing::Size(56, 24);
			this->label10->TabIndex = 3;
			this->label10->Text = S"Pitch";
			// 
			// checkProject1Shadow
			// 
			this->checkProject1Shadow->Checked = true;
			this->checkProject1Shadow->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkProject1Shadow->Enabled = false;
			this->checkProject1Shadow->Location = System::Drawing::Point(88, 184);
			this->checkProject1Shadow->Name = S"checkProject1Shadow";
			this->checkProject1Shadow->Size = System::Drawing::Size(144, 24);
			this->checkProject1Shadow->TabIndex = 2;
			this->checkProject1Shadow->Text = S"Casts Shadow";
			this->checkProject1Shadow->CheckedChanged += new System::EventHandler(this, &LightsDialog::checkProject1Shadow_CheckedChanged);
			// 
			// label11
			// 
			this->label11->Location = System::Drawing::Point(8, 24);
			this->label11->Name = S"label11";
			this->label11->Size = System::Drawing::Size(56, 24);
			this->label11->TabIndex = 1;
			this->label11->Text = S"Color";
			// 
			// colorProject1
			// 
			this->colorProject1->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->colorProject1->Color = System::Drawing::Color::FromArgb((System::Byte)128, (System::Byte)128, (System::Byte)128);
			this->colorProject1->Location = System::Drawing::Point(80, 24);
			this->colorProject1->Name = S"colorProject1";
			this->colorProject1->Size = System::Drawing::Size(328, 24);
			this->colorProject1->TabIndex = 0;
			this->colorProject1->ValueChanged += new System::EventHandler(this, &LightsDialog::colorProject1_ValueChanged);
			// 
			// LightsDialog
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(6, 15);
			this->ClientSize = System::Drawing::Size(440, 647);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->groupPointLight1);
			this->Controls->Add(this->groupDirLight);
			this->Name = S"LightsDialog";
			this->Text = S"Lights Dialog";
			this->groupDirLight->ResumeLayout(false);
			this->groupPointLight1->ResumeLayout(false);
			this->groupBox1->ResumeLayout(false);
			this->ResumeLayout(false);

		}		

		//

		//============================================================================
		//============================================================================
		void SetupData()
		{
			//if (m_bOurChange) return;

			m_bDisableNotify = true;

			colorDir1->Color = tmaManagedConversionUtil::SetColorRGB(mcpLightMgr::GetDirLightColor());
			float pitch = 0, yaw = 0;
			mcpLightMgr::GetDirLightDirection(pitch, yaw);
			rangeDir1Pitch->Value = pitch;
			rangeDir1Yaw->Value = yaw;
			checkDir1Shadow->Checked = mcpLightMgr::GetDirLightCastShadow();
			checkDir1Enabled->Checked = mcpLightMgr::IsEnabledDirLight();

			colorPoint1->Color = tmaManagedConversionUtil::SetColorRGB(mcpLightMgr::GetPointLight1Color());
			float radius = 0;
			mcpLightMgr::GetPointLight1Position(pitch, yaw, radius);
			rangePoint1Pitch->Value = pitch;
			rangePoint1Yaw->Value = yaw;
			rangePoint1Radius->Value = radius;
			checkPoint1Shadow->Checked = mcpLightMgr::GetPointLight1CastShadow();
			checkPoint1Enabled->Checked = mcpLightMgr::IsEnabledPointLight1();

			colorProject1->Color = tmaManagedConversionUtil::SetColorRGB(mcpLightMgr::GetProjectedLightColor());
			mcpLightMgr::GetProjectedLightPosition(pitch, yaw, radius);
			rangeProject1Pitch->Value = pitch;
			rangeProject1Yaw->Value = yaw;
			rangeProject1Radius->Value = radius;
			//checkProject1Shadow->Checked = mcpLightMgr::GetProjectedLightCastShadow();
			checkProject1Enabled->Checked = mcpLightMgr::IsEnabledProjectedLight();

			m_bDisableNotify = false;
		}

	private: System::Void colorDir1_ValueChanged(System::Object *  sender, System::EventArgs *  e)
			 {
				 if (!m_bDisableNotify)
				 {
					maFloatRGBA color = tmaManagedConversionUtil::ConvertColorRGB(this->colorDir1->Color);
					mcpLightMgr::SetDirLightColor(color);
				 }
			 }

	private: System::Void rangeDir1Pitch_ValueChanged(System::Object *  sender, System::EventArgs *  e)
			{
				if (!m_bDisableNotify)
				{
					 mcpLightMgr::SetDirLightDirection((float)this->rangeDir1Pitch->Value, (float)this->rangeDir1Yaw->Value);
				}
			}

	private: System::Void rangeDir1Yaw_ValueChanged(System::Object *  sender, System::EventArgs *  e)
			{
				if (!m_bDisableNotify)
				{
					mcpLightMgr::SetDirLightDirection((float)this->rangeDir1Pitch->Value, (float)this->rangeDir1Yaw->Value);
				}
			}

	private: System::Void checkDir1Shadow_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
			{
				if (!m_bDisableNotify)
				{
					mcpLightMgr::SetDirLightCastShadow(this->checkDir1Shadow->Checked);
				}
			}

	private: System::Void checkDir1Enabled_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
			{
				if (!m_bDisableNotify)
				{
					mcpLightMgr::EnableDirLight(this->checkDir1Enabled->Checked);
				}
			}

	private: System::Void colorPoint1_ValueChanged(System::Object *  sender, System::EventArgs *  e)
			 {
				 if (!m_bDisableNotify)
				 {
					maFloatRGBA color = tmaManagedConversionUtil::ConvertColorRGB(this->colorPoint1->Color);
					mcpLightMgr::SetPointLight1Color(color);
				 }
			 }

	private: System::Void rangePoint1Pitch_ValueChanged(System::Object *  sender, System::EventArgs *  e)
			{
				if (!m_bDisableNotify)
				{
					 mcpLightMgr::SetPointLight1Position((float)this->rangePoint1Pitch->Value, 
						 (float)this->rangePoint1Yaw->Value, 
						 (float)this->rangePoint1Radius->Value);
				}
			}

	private: System::Void rangePoint1Yaw_ValueChanged(System::Object *  sender, System::EventArgs *  e)
			{
				if (!m_bDisableNotify)
				{
					 mcpLightMgr::SetPointLight1Position((float)this->rangePoint1Pitch->Value, 
						 (float)this->rangePoint1Yaw->Value, 
						 (float)this->rangePoint1Radius->Value);
				}
			}

	private: System::Void rangePoint1Radius_ValueChanged(System::Object *  sender, System::EventArgs *  e)
			{
				if (!m_bDisableNotify)
				{
					 mcpLightMgr::SetPointLight1Position((float)this->rangePoint1Pitch->Value, 
						 (float)this->rangePoint1Yaw->Value, 
						 (float)this->rangePoint1Radius->Value);
				}
			}

	private: System::Void checkPoint1Shadow_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
			{
				if (!m_bDisableNotify)
				{
					mcpLightMgr::SetPointLight1CastShadow(this->checkPoint1Shadow->Checked);
				}
			}


	private: System::Void checkPoint1Enabled_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
			{
					if (!m_bDisableNotify)
					{
						mcpLightMgr::EnablePointLight1(this->checkPoint1Enabled->Checked);
					}
			}

	private: System::Void colorProject1_ValueChanged(System::Object *  sender, System::EventArgs *  e)
			 {
				 if (!m_bDisableNotify)
				 {
					maFloatRGBA color = tmaManagedConversionUtil::ConvertColorRGB(this->colorProject1->Color);
					mcpLightMgr::SetProjectedLightColor(color);
				 }
			 }

	private: System::Void rangeProject1Pitch_ValueChanged(System::Object *  sender, System::EventArgs *  e)
			{
				if (!m_bDisableNotify)
				{
					 mcpLightMgr::SetProjectedLightPosition((float)this->rangeProject1Pitch->Value, 
						 (float)this->rangeProject1Yaw->Value, 
						 (float)this->rangeProject1Radius->Value);
				}
			}

	private: System::Void rangeProject1Yaw_ValueChanged(System::Object *  sender, System::EventArgs *  e)
			{
				if (!m_bDisableNotify)
				{
					 mcpLightMgr::SetProjectedLightPosition((float)this->rangeProject1Pitch->Value, 
						 (float)this->rangeProject1Yaw->Value, 
						 (float)this->rangeProject1Radius->Value);
				}
			}

	private: System::Void rangeProject1Radius_ValueChanged(System::Object *  sender, System::EventArgs *  e)
			{
				if (!m_bDisableNotify)
				{
					 mcpLightMgr::SetProjectedLightPosition((float)this->rangeProject1Pitch->Value, 
						 (float)this->rangeProject1Yaw->Value, 
						 (float)this->rangeProject1Radius->Value);
				}
			}

	private: System::Void checkProject1Shadow_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
			{
				//if (!m_bDisableNotify)
				//{
				//	mcpLightMgr::SetProjectedLightCastShadow(this->checkProject1Shadow->Checked);
				//}
			}


	private: System::Void checkProject1Enabled_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
			{
					if (!m_bDisableNotify)
					{
						mcpLightMgr::EnableProjectedLight(this->checkProject1Enabled->Checked);
					}
			}

};
}