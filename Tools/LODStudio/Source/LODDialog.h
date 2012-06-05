#pragma once

#ifndef LOD_OPERATIONS_HPP
#include "lodOperations.hpp"
#endif
#ifndef LOD3D_DATA_HPP
#include "lod3dData.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "tmaManagedConversionUtil.hpp"
#endif


//============================================================================
//============================================================================
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


//============================================================================
//	forward references
//============================================================================
__gc class tmaDialogMemory;


//============================================================================
//
//============================================================================
namespace LODStudio
{
	/// <summary>
	/// Summary for LODDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class LODDialog : public System::Windows::Forms::Form
	{
	public:
		static LODDialog *FormInstance = 0;

		LODDialog(lod3dData& i_Data);

		void UpdateData()
		{
		}

	protected:
		void Dispose(Boolean disposing)
		{
			// clear instance
			if (disposing && LODDialog::FormInstance == this)
				LODDialog::FormInstance = 0;

			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}

	private: lod3dData&	m_Data;
	private: tmaDialogMemory* m_pMemory;// Dialog memory remembers size, location, visiblity of dialog
	private: bool m_bDisableNotify;		// Turns off notify callbacks when setting up form
	private: bool m_bDataChanged;		// Set this when an individual component changes so the apply button can be active.

	//	main menu
	private: System::Windows::Forms::Button *  button_save;
	private: System::Windows::Forms::Button *  button_saveas;
	private: System::Windows::Forms::Button *  button_open;
	private: System::Windows::Forms::TabPage *  tabPage_LODlevels;
	private: System::Windows::Forms::TabControl *  tabControl_LODdata;
	private: System::Windows::Forms::GroupBox *  groupBox_LODs;

	private: System::Windows::Forms::RadioButton *  radioButton_LOD0default;
	private: TerawattManagedControls::FileChooser *  fileChooser_LOD0;
	private: TerawattManagedControls::FileChooser *  fileChooser_LOD1;
	private: TerawattManagedControls::FileChooser *  fileChooser_LOD2;
	private: TerawattManagedControls::FloatEdit *  floatEdit_LOD2;
	private: TerawattManagedControls::FloatEdit *  floatEdit_LOD1;
	private: System::Windows::Forms::RadioButton *  radioButton_LOD1default;
	private: System::Windows::Forms::RadioButton *  radioButton_LOD2default;
	private: System::Windows::Forms::Label *  label_editordefault;
	private: System::Windows::Forms::Button *  button_LOD1setcurrent;
	private: System::Windows::Forms::Button *  button_LOD2setcurrent;
	private: System::Windows::Forms::Label *  label_distancetoswitch;


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
			this->button_save = new System::Windows::Forms::Button();
			this->button_saveas = new System::Windows::Forms::Button();
			this->button_open = new System::Windows::Forms::Button();
			this->tabPage_LODlevels = new System::Windows::Forms::TabPage();
			this->groupBox_LODs = new System::Windows::Forms::GroupBox();
			this->button_LOD2setcurrent = new System::Windows::Forms::Button();
			this->button_LOD1setcurrent = new System::Windows::Forms::Button();
			this->label_editordefault = new System::Windows::Forms::Label();
			this->label_distancetoswitch = new System::Windows::Forms::Label();
			this->floatEdit_LOD1 = new TerawattManagedControls::FloatEdit();
			this->floatEdit_LOD2 = new TerawattManagedControls::FloatEdit();
			this->fileChooser_LOD2 = new TerawattManagedControls::FileChooser();
			this->fileChooser_LOD1 = new TerawattManagedControls::FileChooser();
			this->fileChooser_LOD0 = new TerawattManagedControls::FileChooser();
			this->radioButton_LOD1default = new System::Windows::Forms::RadioButton();
			this->radioButton_LOD0default = new System::Windows::Forms::RadioButton();
			this->radioButton_LOD2default = new System::Windows::Forms::RadioButton();
			this->tabControl_LODdata = new System::Windows::Forms::TabControl();
			this->tabPage_LODlevels->SuspendLayout();
			this->groupBox_LODs->SuspendLayout();
			this->tabControl_LODdata->SuspendLayout();
			this->SuspendLayout();
			// 
			// button_save
			// 
			this->button_save->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->button_save->Location = System::Drawing::Point(168, 264);
			this->button_save->Name = S"button_save";
			this->button_save->TabIndex = 1;
			this->button_save->Text = S"Save";
			this->button_save->Click += new System::EventHandler(this, &LODDialog::button_save_Click);
			// 
			// button_saveas
			// 
			this->button_saveas->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->button_saveas->Location = System::Drawing::Point(288, 264);
			this->button_saveas->Name = S"button_saveas";
			this->button_saveas->TabIndex = 2;
			this->button_saveas->Text = S"Save As";
			this->button_saveas->Click += new System::EventHandler(this, &LODDialog::button_saveas_Click);
			// 
			// button_open
			// 
			this->button_open->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->button_open->Location = System::Drawing::Point(408, 264);
			this->button_open->Name = S"button_open";
			this->button_open->TabIndex = 3;
			this->button_open->Text = S"Open";
			this->button_open->Click += new System::EventHandler(this, &LODDialog::button_open_Click);
			// 
			// tabPage_LODlevels
			// 
			this->tabPage_LODlevels->Controls->Add(this->groupBox_LODs);
			this->tabPage_LODlevels->Location = System::Drawing::Point(4, 22);
			this->tabPage_LODlevels->Name = S"tabPage_LODlevels";
			this->tabPage_LODlevels->Size = System::Drawing::Size(624, 206);
			this->tabPage_LODlevels->TabIndex = 6;
			this->tabPage_LODlevels->Text = S"LOD Levels";
			// 
			// groupBox_LODs
			// 
			this->groupBox_LODs->Controls->Add(this->button_LOD2setcurrent);
			this->groupBox_LODs->Controls->Add(this->button_LOD1setcurrent);
			this->groupBox_LODs->Controls->Add(this->label_editordefault);
			this->groupBox_LODs->Controls->Add(this->label_distancetoswitch);
			this->groupBox_LODs->Controls->Add(this->floatEdit_LOD1);
			this->groupBox_LODs->Controls->Add(this->floatEdit_LOD2);
			this->groupBox_LODs->Controls->Add(this->fileChooser_LOD2);
			this->groupBox_LODs->Controls->Add(this->fileChooser_LOD1);
			this->groupBox_LODs->Controls->Add(this->fileChooser_LOD0);
			this->groupBox_LODs->Controls->Add(this->radioButton_LOD1default);
			this->groupBox_LODs->Controls->Add(this->radioButton_LOD0default);
			this->groupBox_LODs->Controls->Add(this->radioButton_LOD2default);
			this->groupBox_LODs->Location = System::Drawing::Point(8, 8);
			this->groupBox_LODs->Name = S"groupBox_LODs";
			this->groupBox_LODs->Size = System::Drawing::Size(600, 184);
			this->groupBox_LODs->TabIndex = 0;
			this->groupBox_LODs->TabStop = false;
			this->groupBox_LODs->Text = S"Levels of Detail";
			// 
			// button_LOD2setcurrent
			// 
			this->button_LOD2setcurrent->Location = System::Drawing::Point(504, 137);
			this->button_LOD2setcurrent->Name = S"button_LOD2setcurrent";
			this->button_LOD2setcurrent->TabIndex = 12;
			this->button_LOD2setcurrent->Text = S"Set Current";
			this->button_LOD2setcurrent->Click += new System::EventHandler(this, &LODDialog::button_LOD2setcurrent_Click);
			// 
			// button_LOD1setcurrent
			// 
			this->button_LOD1setcurrent->Location = System::Drawing::Point(504, 97);
			this->button_LOD1setcurrent->Name = S"button_LOD1setcurrent";
			this->button_LOD1setcurrent->TabIndex = 11;
			this->button_LOD1setcurrent->Text = S"Set Current";
			this->button_LOD1setcurrent->Click += new System::EventHandler(this, &LODDialog::button_LOD1setcurrent_Click);
			// 
			// label_editordefault
			// 
			this->label_editordefault->Location = System::Drawing::Point(16, 24);
			this->label_editordefault->Name = S"label_editordefault";
			this->label_editordefault->Size = System::Drawing::Size(48, 24);
			this->label_editordefault->TabIndex = 10;
			this->label_editordefault->Text = S"Editor Default";
			// 
			// label_distancetoswitch
			// 
			this->label_distancetoswitch->Location = System::Drawing::Point(440, 24);
			this->label_distancetoswitch->Name = S"label_distancetoswitch";
			this->label_distancetoswitch->Size = System::Drawing::Size(56, 24);
			this->label_distancetoswitch->TabIndex = 9;
			this->label_distancetoswitch->Text = S"Distance to Switch";
			this->label_distancetoswitch->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_LOD1
			// 
			this->floatEdit_LOD1->Location = System::Drawing::Point(440, 96);
			this->floatEdit_LOD1->Name = S"floatEdit_LOD1";
			this->floatEdit_LOD1->Precision = (System::Int16)2;
			this->floatEdit_LOD1->Size = System::Drawing::Size(48, 24);
			this->floatEdit_LOD1->TabIndex = 8;
			this->floatEdit_LOD1->ValueChanged += new System::EventHandler(this, &LODDialog::floatEdit_LOD1_ValueChanged);
			// 
			// floatEdit_LOD2
			// 
			this->floatEdit_LOD2->Location = System::Drawing::Point(440, 136);
			this->floatEdit_LOD2->Name = S"floatEdit_LOD2";
			this->floatEdit_LOD2->Precision = (System::Int16)2;
			this->floatEdit_LOD2->Size = System::Drawing::Size(48, 24);
			this->floatEdit_LOD2->TabIndex = 7;
			this->floatEdit_LOD2->ValueChanged += new System::EventHandler(this, &LODDialog::floatEdit_LOD2_ValueChanged);
			// 
			// fileChooser_LOD2
			// 
			this->fileChooser_LOD2->Location = System::Drawing::Point(152, 136);
			this->fileChooser_LOD2->Name = S"fileChooser_LOD2";
			this->fileChooser_LOD2->Size = System::Drawing::Size(264, 24);
			this->fileChooser_LOD2->TabIndex = 2;
			this->fileChooser_LOD2->ValueChanged += new System::EventHandler(this, &LODDialog::fileChooser_LOD2_ValueChanged);
			// 
			// fileChooser_LOD1
			// 
			this->fileChooser_LOD1->Location = System::Drawing::Point(152, 96);
			this->fileChooser_LOD1->Name = S"fileChooser_LOD1";
			this->fileChooser_LOD1->Size = System::Drawing::Size(264, 24);
			this->fileChooser_LOD1->TabIndex = 1;
			this->fileChooser_LOD1->ValueChanged += new System::EventHandler(this, &LODDialog::fileChooser_LOD1_ValueChanged);
			// 
			// fileChooser_LOD0
			// 
			this->fileChooser_LOD0->Location = System::Drawing::Point(152, 56);
			this->fileChooser_LOD0->Name = S"fileChooser_LOD0";
			this->fileChooser_LOD0->Size = System::Drawing::Size(264, 24);
			this->fileChooser_LOD0->TabIndex = 0;
			this->fileChooser_LOD0->ValueChanged += new System::EventHandler(this, &LODDialog::fileChooser_LOD0_ValueChanged);
			// 
			// radioButton_LOD1default
			// 
			this->radioButton_LOD1default->Location = System::Drawing::Point(32, 96);
			this->radioButton_LOD1default->Name = S"radioButton_LOD1default";
			this->radioButton_LOD1default->Size = System::Drawing::Size(112, 16);
			this->radioButton_LOD1default->TabIndex = 1;
			this->radioButton_LOD1default->Text = S"   LOD 1";
			this->radioButton_LOD1default->CheckedChanged += new System::EventHandler(this, &LODDialog::radioButton_LOD1default_CheckedChanged);
			// 
			// radioButton_LOD0default
			// 
			this->radioButton_LOD0default->Location = System::Drawing::Point(32, 56);
			this->radioButton_LOD0default->Name = S"radioButton_LOD0default";
			this->radioButton_LOD0default->Size = System::Drawing::Size(112, 16);
			this->radioButton_LOD0default->TabIndex = 0;
			this->radioButton_LOD0default->Text = S"   LOD 0 (highest)";
			this->radioButton_LOD0default->CheckedChanged += new System::EventHandler(this, &LODDialog::radioButton_LOD0default_CheckedChanged);
			// 
			// radioButton_LOD2default
			// 
			this->radioButton_LOD2default->Checked = true;
			this->radioButton_LOD2default->Location = System::Drawing::Point(32, 136);
			this->radioButton_LOD2default->Name = S"radioButton_LOD2default";
			this->radioButton_LOD2default->Size = System::Drawing::Size(112, 16);
			this->radioButton_LOD2default->TabIndex = 2;
			this->radioButton_LOD2default->TabStop = true;
			this->radioButton_LOD2default->Text = S"   LOD 2";
			this->radioButton_LOD2default->CheckedChanged += new System::EventHandler(this, &LODDialog::radioButton_LOD2default_CheckedChanged);
			// 
			// tabControl_LODdata
			// 
			this->tabControl_LODdata->Controls->Add(this->tabPage_LODlevels);
			this->tabControl_LODdata->ItemSize = System::Drawing::Size(42, 18);
			this->tabControl_LODdata->Location = System::Drawing::Point(8, 8);
			this->tabControl_LODdata->Name = S"tabControl_LODdata";
			this->tabControl_LODdata->SelectedIndex = 0;
			this->tabControl_LODdata->Size = System::Drawing::Size(632, 232);
			this->tabControl_LODdata->TabIndex = 0;
			// 
			// LODDialog
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(648, 301);
			this->ControlBox = false;
			this->Controls->Add(this->button_open);
			this->Controls->Add(this->button_saveas);
			this->Controls->Add(this->button_save);
			this->Controls->Add(this->tabControl_LODdata);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = S"LODDialog";
			this->Text = S"Level of Detail";
			this->tabPage_LODlevels->ResumeLayout(false);
			this->groupBox_LODs->ResumeLayout(false);
			this->tabControl_LODdata->ResumeLayout(false);
			this->ResumeLayout(false);

		}




//
private: System::Void button_save_Click(System::Object *  sender, System::EventArgs *  e);
private: System::Void button_saveas_Click(System::Object *  sender, System::EventArgs *  e);
private: System::Void button_open_Click(System::Object *  sender, System::EventArgs *  e);

private: System::Void button_LOD1setcurrent_Click(System::Object *  sender, System::EventArgs *  e);
private: System::Void button_LOD2setcurrent_Click(System::Object *  sender, System::EventArgs *  e);
private: System::Void radioButton_LOD0default_CheckedChanged(System::Object *  sender, System::EventArgs *  e);
private: System::Void radioButton_LOD1default_CheckedChanged(System::Object *  sender, System::EventArgs *  e);
private: System::Void radioButton_LOD2default_CheckedChanged(System::Object *  sender, System::EventArgs *  e);
private: System::Void fileChooser_LOD0_ValueChanged(System::Object *  sender, System::EventArgs *  e);
private: System::Void fileChooser_LOD1_ValueChanged(System::Object *  sender, System::EventArgs *  e);
private: System::Void fileChooser_LOD2_ValueChanged(System::Object *  sender, System::EventArgs *  e);
private: System::Void floatEdit_LOD1_ValueChanged(System::Object *  sender, System::EventArgs *  e);
private: System::Void floatEdit_LOD2_ValueChanged(System::Object *  sender, System::EventArgs *  e);

private:
		//----------------------------------------------------------------------------
		//	set that a control component changed
		//----------------------------------------------------------------------------
		void SetControlDataChanged();

		//----------------------------------------------------------------------------
		//	reset that a control component changed
		//----------------------------------------------------------------------------
		void ResetControlDataChanged();

		//----------------------------------------------------------------------------
		//	set that a control component changed
		//----------------------------------------------------------------------------
		void SetControlFilenameChanged();

		//----------------------------------------------------------------------------
		//	reset that a control component changed
		//----------------------------------------------------------------------------
		void ResetControlFilenameChanged();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void SetupControls();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void SetupData();
};
}
