/********************************************************************************************\
**  skySkyDataForm.h
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#pragma once

#ifndef DAY_SKYDATA_HPP
#include "daySkyData.hpp"
#endif
#ifndef DAY_SKYMGR_HPP
#include "daySkyMgr.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "fsLocator.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "fsFileUtil.hpp"
#endif
#ifndef GF_PATHS_HPP
#include "gfPaths.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "itStringUtil.hpp"
#endif
#ifndef MNM_PATHS_HPP
#include "mnmPaths.hpp"
#endif
#ifndef SKY_DATAMGR_HPP
#include "skyDataMgr.hpp"
#endif
#ifndef SKY_OPERATIONS_HPP
#include "skyOperations.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "tmaManagedConversionUtil.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "tmaManagedStringUtils.hpp"
#endif

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Globalization;	// for Convert
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace GeneratedForms
{
	/// <summary>
	/// Summary for skySkyDataForm
	///
	/// WARNING: If you change the name of this class, yada yada...
	/// </summary>
	public __gc class skySkyDataForm : public System::Windows::Forms::Form
	{
	public:
		static skySkyDataForm* FormInstance = 0;

		skySkyDataForm()
		{
			m_bDisableNotify = true;
			InitializeComponent();
			disable_controls();
			m_bDisableNotify = false;
		}

		// Call this to update dialog to new data
		void Update(const daySkyLayerList &i_Data)
		{
			listBox_layermodels->Items->Clear();

			int size = i_Data.size();

			int i;
			for ( i=0; i < size ; ++i )
			{
				int index = listBox_layermodels->Items->Add( new String( i_Data[i].m_ModelFileName.c_str()) );
			}
			int index = listBox_layermodels->get_SelectedIndex();
			if ( index < 0 && size > 0 )
			{
				listBox_layermodels->set_SelectedIndex(0);
				index = 0;
			}

			if ( index >= 0 )
			{
				SetupData(i_Data[index]);
			}
		}

	protected:
		void Dispose(Boolean disposing)
		{
			// clear instance
			if (disposing && skySkyDataForm::FormInstance == this)
				skySkyDataForm::FormInstance = 0;

			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container* components;
	private: System::Windows::Forms::TabControl *  tabControl_sky;
	private: System::Windows::Forms::TabPage *  tabPage_sky;
	private: System::Windows::Forms::GroupBox *  groupBox_layers;
	private: System::Windows::Forms::ListBox *  listBox_layermodels;
	private: System::Windows::Forms::GroupBox *  groupBox_layerproperties;
	private: System::Windows::Forms::Button *  button_new;
	private: System::Windows::Forms::Button *  button_delete;
	private: System::Windows::Forms::Label *  label_rotationaxis;




	private: System::Windows::Forms::Label *  label3;

	private: System::Windows::Forms::Label *  label_day;
	private: System::Windows::Forms::Label *  label_sunrisesunset;
	private: System::Windows::Forms::GroupBox *  groupBox_colors;

	private: System::Windows::Forms::Label *  label_rotationvelocity;
	private: System::Windows::Forms::TextBox *  textBox_rotationvelocity;
	private: System::Windows::Forms::Label *  label_degpersec;
	private: TerawattManagedControls::ColorRGBEdit *  colorRGBEdit_night;
	private: TerawattManagedControls::ColorRGBEdit *  colorRGBEdit_sunrisesunset;
	private: TerawattManagedControls::ColorRGBEdit *  colorRGBEdit_day;
	private: TerawattManagedControls::Vector3Edit *  vector3Edit_rotationaxis;






















		// Holds reference to data, changing the data
		// within the caller's structure
	private: bool m_bDisableNotify;



		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl_sky = new System::Windows::Forms::TabControl();
			this->tabPage_sky = new System::Windows::Forms::TabPage();
			this->label3 = new System::Windows::Forms::Label();
			this->colorRGBEdit_night = new TerawattManagedControls::ColorRGBEdit();
			this->label_sunrisesunset = new System::Windows::Forms::Label();
			this->colorRGBEdit_sunrisesunset = new TerawattManagedControls::ColorRGBEdit();
			this->label_day = new System::Windows::Forms::Label();
			this->colorRGBEdit_day = new TerawattManagedControls::ColorRGBEdit();
			this->groupBox_layerproperties = new System::Windows::Forms::GroupBox();
			this->label_degpersec = new System::Windows::Forms::Label();
			this->textBox_rotationvelocity = new System::Windows::Forms::TextBox();
			this->label_rotationvelocity = new System::Windows::Forms::Label();
			this->vector3Edit_rotationaxis = new TerawattManagedControls::Vector3Edit();
			this->label_rotationaxis = new System::Windows::Forms::Label();
			this->groupBox_layers = new System::Windows::Forms::GroupBox();
			this->button_delete = new System::Windows::Forms::Button();
			this->button_new = new System::Windows::Forms::Button();
			this->listBox_layermodels = new System::Windows::Forms::ListBox();
			this->groupBox_colors = new System::Windows::Forms::GroupBox();
			this->tabControl_sky->SuspendLayout();
			this->tabPage_sky->SuspendLayout();
			this->groupBox_layerproperties->SuspendLayout();
			this->groupBox_layers->SuspendLayout();
			this->SuspendLayout();
			//
			// tabControl_sky
			//
			this->tabControl_sky->Controls->Add(this->tabPage_sky);
			this->tabControl_sky->Location = System::Drawing::Point(8, 8);
			this->tabControl_sky->Name = S"tabControl_sky";
			this->tabControl_sky->SelectedIndex = 0;
			this->tabControl_sky->Size = System::Drawing::Size(352, 360);
			this->tabControl_sky->TabIndex = 3;
			this->tabControl_sky->Click += new System::EventHandler(this, tabControl_sky_Click);
			//
			// tabPage_sky
			//
			this->tabPage_sky->Controls->Add(this->label3);
			this->tabPage_sky->Controls->Add(this->colorRGBEdit_night);
			this->tabPage_sky->Controls->Add(this->label_sunrisesunset);
			this->tabPage_sky->Controls->Add(this->colorRGBEdit_sunrisesunset);
			this->tabPage_sky->Controls->Add(this->label_day);
			this->tabPage_sky->Controls->Add(this->colorRGBEdit_day);
			this->tabPage_sky->Controls->Add(this->groupBox_layerproperties);
			this->tabPage_sky->Controls->Add(this->groupBox_layers);
			this->tabPage_sky->Controls->Add(this->groupBox_colors);
			this->tabPage_sky->Location = System::Drawing::Point(4, 22);
			this->tabPage_sky->Name = S"tabPage_sky";
			this->tabPage_sky->Size = System::Drawing::Size(344, 334);
			this->tabPage_sky->TabIndex = 0;
			this->tabPage_sky->Text = S"Sky";
			//
			// label3
			//
			this->label3->Location = System::Drawing::Point(16, 296);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(80, 21);
			this->label3->TabIndex = 18;
			this->label3->Text = S"Color:";
			//
			// colorRGBEdit_night
			//
			this->colorRGBEdit_night->Color = System::Drawing::Color::FromArgb((System::Byte)128, (System::Byte)128, (System::Byte)128);
			this->colorRGBEdit_night->Location = System::Drawing::Point(104, 296);
			this->colorRGBEdit_night->Name = S"colorRGBEdit_night";
			this->colorRGBEdit_night->Size = System::Drawing::Size(224, 24);
			this->colorRGBEdit_night->TabIndex = 19;
			this->colorRGBEdit_night->ValueChanged += new System::EventHandler(this, colorRGBEdit_night_ValueChanged);
			//
			// label_sunrisesunset
			//
			this->label_sunrisesunset->Location = System::Drawing::Point(16, 256);
			this->label_sunrisesunset->Name = S"label_sunrisesunset";
			this->label_sunrisesunset->Size = System::Drawing::Size(80, 21);
			this->label_sunrisesunset->TabIndex = 16;
			this->label_sunrisesunset->Text = S"Sunrise/set:";
			//
			// colorRGBEdit_sunrisesunset
			//
			this->colorRGBEdit_sunrisesunset->Color = System::Drawing::Color::FromArgb((System::Byte)128, (System::Byte)128, (System::Byte)128);
			this->colorRGBEdit_sunrisesunset->Location = System::Drawing::Point(104, 256);
			this->colorRGBEdit_sunrisesunset->Name = S"colorRGBEdit_sunrisesunset";
			this->colorRGBEdit_sunrisesunset->Size = System::Drawing::Size(224, 24);
			this->colorRGBEdit_sunrisesunset->TabIndex = 17;
			this->colorRGBEdit_sunrisesunset->ValueChanged += new System::EventHandler(this, colorRGBEdit_sunrisesunset_ValueChanged);
			//
			// label_day
			//
			this->label_day->Location = System::Drawing::Point(16, 216);
			this->label_day->Name = S"label_day";
			this->label_day->Size = System::Drawing::Size(80, 21);
			this->label_day->TabIndex = 14;
			this->label_day->Text = S"Day:";
			//
			// colorRGBEdit_day
			//
			this->colorRGBEdit_day->Color = System::Drawing::Color::FromArgb((System::Byte)128, (System::Byte)128, (System::Byte)128);
			this->colorRGBEdit_day->Location = System::Drawing::Point(104, 216);
			this->colorRGBEdit_day->Name = S"colorRGBEdit_day";
			this->colorRGBEdit_day->Size = System::Drawing::Size(224, 24);
			this->colorRGBEdit_day->TabIndex = 15;
			this->colorRGBEdit_day->ValueChanged += new System::EventHandler(this, colorRGBEdit_day_ValueChanged);
			//
			// groupBox_layerproperties
			//
			this->groupBox_layerproperties->Controls->Add(this->label_degpersec);
			this->groupBox_layerproperties->Controls->Add(this->textBox_rotationvelocity);
			this->groupBox_layerproperties->Controls->Add(this->label_rotationvelocity);
			this->groupBox_layerproperties->Controls->Add(this->vector3Edit_rotationaxis);
			this->groupBox_layerproperties->Controls->Add(this->label_rotationaxis);
			this->groupBox_layerproperties->Location = System::Drawing::Point(8, 104);
			this->groupBox_layerproperties->Name = S"groupBox_layerproperties";
			this->groupBox_layerproperties->Size = System::Drawing::Size(328, 88);
			this->groupBox_layerproperties->TabIndex = 1;
			this->groupBox_layerproperties->TabStop = false;
			this->groupBox_layerproperties->Text = S"Layer Properties";
			//
			// label_degpersec
			//
			this->label_degpersec->Location = System::Drawing::Point(192, 56);
			this->label_degpersec->Name = S"label_degpersec";
			this->label_degpersec->Size = System::Drawing::Size(104, 24);
			this->label_degpersec->TabIndex = 12;
			this->label_degpersec->Text = S"(degrees/second)";
			//
			// textBox_rotationvelocity
			//
			this->textBox_rotationvelocity->Location = System::Drawing::Point(120, 56);
			this->textBox_rotationvelocity->Name = S"textBox_rotationvelocity";
			this->textBox_rotationvelocity->Size = System::Drawing::Size(64, 20);
			this->textBox_rotationvelocity->TabIndex = 11;
			this->textBox_rotationvelocity->Text = S"";
			this->textBox_rotationvelocity->TextChanged += new System::EventHandler(this, textBox_rotationvelocity_TextChanged);
			//
			// label_rotationvelocity
			//
			this->label_rotationvelocity->Location = System::Drawing::Point(8, 56);
			this->label_rotationvelocity->Name = S"label_rotationvelocity";
			this->label_rotationvelocity->Size = System::Drawing::Size(104, 24);
			this->label_rotationvelocity->TabIndex = 10;
			this->label_rotationvelocity->Text = S"rotation velocity";
			//
			// vector3Edit_rotationaxis
			//
			this->vector3Edit_rotationaxis->Location = System::Drawing::Point(120, 24);
			this->vector3Edit_rotationaxis->Name = S"vector3Edit_rotationaxis";
			this->vector3Edit_rotationaxis->Size = System::Drawing::Size(187, 20);
			this->vector3Edit_rotationaxis->TabIndex = 9;
			this->vector3Edit_rotationaxis->ValueChanged += new System::EventHandler(this, vector3Edit_rotationaxis_ValueChanged);
			//
			// label_rotationaxis
			//
			this->label_rotationaxis->Location = System::Drawing::Point(8, 24);
			this->label_rotationaxis->Name = S"label_rotationaxis";
			this->label_rotationaxis->Size = System::Drawing::Size(104, 24);
			this->label_rotationaxis->TabIndex = 0;
			this->label_rotationaxis->Text = S"rotation axis (X,Y,Z)";
			//
			// groupBox_layers
			//
			this->groupBox_layers->Controls->Add(this->button_delete);
			this->groupBox_layers->Controls->Add(this->button_new);
			this->groupBox_layers->Controls->Add(this->listBox_layermodels);
			this->groupBox_layers->Location = System::Drawing::Point(8, 8);
			this->groupBox_layers->Name = S"groupBox_layers";
			this->groupBox_layers->Size = System::Drawing::Size(328, 88);
			this->groupBox_layers->TabIndex = 0;
			this->groupBox_layers->TabStop = false;
			this->groupBox_layers->Text = S"Layer Models";
			//
			// button_delete
			//
			this->button_delete->Location = System::Drawing::Point(264, 56);
			this->button_delete->Name = S"button_delete";
			this->button_delete->Size = System::Drawing::Size(56, 24);
			this->button_delete->TabIndex = 2;
			this->button_delete->Text = S"Delete";
			this->button_delete->Click += new System::EventHandler(this, button_delete_Click);
			//
			// button_new
			//
			this->button_new->Location = System::Drawing::Point(264, 24);
			this->button_new->Name = S"button_new";
			this->button_new->Size = System::Drawing::Size(56, 24);
			this->button_new->TabIndex = 1;
			this->button_new->Text = S"New";
			this->button_new->Click += new System::EventHandler(this, button_new_Click);
			//
			// listBox_layermodels
			//
			this->listBox_layermodels->Location = System::Drawing::Point(8, 24);
			this->listBox_layermodels->Name = S"listBox_layermodels";
			this->listBox_layermodels->Size = System::Drawing::Size(240, 56);
			this->listBox_layermodels->TabIndex = 0;
			this->listBox_layermodels->SelectedIndexChanged += new System::EventHandler(this, listBox_layermodels_SelectedIndexChanged);
			//
			// groupBox_colors
			//
			this->groupBox_colors->Location = System::Drawing::Point(8, 200);
			this->groupBox_colors->Name = S"groupBox_colors";
			this->groupBox_colors->Size = System::Drawing::Size(328, 128);
			this->groupBox_colors->TabIndex = 20;
			this->groupBox_colors->TabStop = false;
			this->groupBox_colors->Text = S"Layer Colors";
			//
			// skySkyDataForm
			//
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(368, 373);
			this->Controls->Add(this->tabControl_sky);
			this->Name = S"skySkyDataForm";
			this->Text = S"Sky Data Form";
			this->tabControl_sky->ResumeLayout(false);
			this->tabPage_sky->ResumeLayout(false);
			this->groupBox_layerproperties->ResumeLayout(false);
			this->groupBox_layers->ResumeLayout(false);
			this->ResumeLayout(false);

		}

		//

private: System::Void listBox_layermodels_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
		 {
 			if (!m_bDisableNotify)
			{
				int index = listBox_layermodels->get_SelectedIndex();

				if ( index >= 0 )
				{
					SetupData( daySkyMgr::GetData(index) );
					enable_controls();
				}
				else
				{
					disable_controls();
				}
			}
		 }

private: System::Void vector3Edit_rotationaxis_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
 			if (!m_bDisableNotify)
			{
				int index = listBox_layermodels->get_SelectedIndex();
				skyOperations::ChangeRotationAxis( index, maVector3d(   (float)vector3Edit_rotationaxis->get_ValueX(),
																		(float)vector3Edit_rotationaxis->get_ValueY(),
																		(float)vector3Edit_rotationaxis->get_ValueZ() ) );
			}
		 }

private: System::Void textBox_rotationvelocity_TextChanged(System::Object *  sender, System::EventArgs *  e)
		 {
 			if (!m_bDisableNotify)
			{
				int index = listBox_layermodels->get_SelectedIndex();
				float length = 0.0f;

				if ( textBox_rotationvelocity->get_Text()->get_Length() > 0 )
				{
					try
					{
						length = Convert::ToSingle( textBox_rotationvelocity->get_Text() );
						skyOperations::ChangeRotationVelocity( index, length );
					}
					catch (System::OverflowException*)
					{
						length = 0.0f;
					}
					catch (System::FormatException*)
					{
						length = 0.0f;
					}
					catch (System::ArgumentNullException*)
					{
						length = 0.0f;
					}
				}
				//skyOperations::ChangeRotationVelocity( index, length );
			}
		 }

private: System::Void colorRGBEdit_day_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
 			if (!m_bDisableNotify)
			{
				int index = listBox_layermodels->get_SelectedIndex();
				skyOperations::ChangeLayerColorDay( index, tmaManagedConversionUtil::ConvertColorRGBA(colorRGBEdit_day->get_Color()) );
			}
		 }

private: System::Void colorRGBEdit_sunrisesunset_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
 			if (!m_bDisableNotify)
			{
				int index = listBox_layermodels->get_SelectedIndex();
				skyOperations::ChangeLayerColorSunRiseSunSet( index, tmaManagedConversionUtil::ConvertColorRGBA(colorRGBEdit_sunrisesunset->get_Color()) );
			}
		 }

private: System::Void colorRGBEdit_night_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		 {
 			if (!m_bDisableNotify)
			{
				int index = listBox_layermodels->get_SelectedIndex();
				skyOperations::ChangeLayerColorNight( index, tmaManagedConversionUtil::ConvertColorRGBA(colorRGBEdit_night->get_Color()) );
			}
		 }

private: System::Void button_new_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			System::Windows::Forms::OpenFileDialog* dialog =
			new System::Windows::Forms::OpenFileDialog();
			dialog->Filter = S"Model Files (*.mx)|*.mx";
			//dialog->InitialDirectory = tmaManagedStringUtils::LocatorToManagedString(m_Driver.GetSoundDir());
			// FIX: !!! this hard-coded directory
			std::string artdir;
			std::string artfile;
			fsFileUtil::LocatorToANSIFilename( gfPaths::GetPath(mnmPaths::e_Sky), artdir );
			artfile = artdir;
			artfile += gfPaths::GetSubPath( gfPaths::e_Models );
			dialog->InitialDirectory = artfile.c_str();

			if (dialog->ShowDialog() == DialogResult::OK)
			{
				fsLocator full_path;
				tmaManagedStringUtils::ManagedStringToLocator(dialog->FileName, full_path);

				itString filename = full_path.GetLastName();
				int index = listBox_layermodels->Items->Add( tmaManagedStringUtils::ItStringToManagedString(filename) );

				daySkyLayerData newdata;
				newdata.m_ModelFileName = itStringUtil::GetStdString( filename );

				daySkyMgr::Add( newdata );

				listBox_layermodels->set_SelectedIndex( index );
			}
		 }

private: System::Void button_delete_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			int index = listBox_layermodels->get_SelectedIndex();
			if ( index >= 0 )
			{
				skyOperations::DeleteLayer( index );
			}
		 }

private: System::Void tabControl_sky_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 this->Update( daySkyMgr::GetListData() );
		 }

private:
		void enable_controls()
		{
			m_bDisableNotify = true;

			vector3Edit_rotationaxis->set_Enabled( true );
			textBox_rotationvelocity->set_Enabled( true );
			colorRGBEdit_night->set_Enabled( true );
			colorRGBEdit_sunrisesunset->set_Enabled( true );
			colorRGBEdit_day->set_Enabled( true );

			m_bDisableNotify = false;
		}

		void disable_controls()
		{
			m_bDisableNotify = true;

			vector3Edit_rotationaxis->set_Enabled( false );
			textBox_rotationvelocity->set_Enabled( false );
			colorRGBEdit_night->set_Enabled( false );
			colorRGBEdit_sunrisesunset->set_Enabled( false );
			colorRGBEdit_day->set_Enabled( false );

			m_bDisableNotify = false;
		}

		void SetupData(const daySkyLayerData &i_Data)
		{
			m_bDisableNotify = true;

			vector3Edit_rotationaxis->set_ValueX( i_Data.m_RotationAxis.GetX() );
			vector3Edit_rotationaxis->set_ValueY( i_Data.m_RotationAxis.GetY() );
			vector3Edit_rotationaxis->set_ValueZ( i_Data.m_RotationAxis.GetZ() );
			textBox_rotationvelocity->set_Text( __box(i_Data.m_fRotationVelocity)->ToString("F2") );
			colorRGBEdit_night->set_Color( tmaManagedConversionUtil::SetColorRGB(i_Data.m_NightColor) );
			colorRGBEdit_sunrisesunset->set_Color( tmaManagedConversionUtil::SetColorRGB(i_Data.m_SunRiseSetColor) );
			colorRGBEdit_day->set_Color( tmaManagedConversionUtil::SetColorRGB(i_Data.m_DayColor) );

			m_bDisableNotify = false;
		}

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage * GetTabPage( int i_Index )
		{
			return tabPage_sky;
		}
};
}
