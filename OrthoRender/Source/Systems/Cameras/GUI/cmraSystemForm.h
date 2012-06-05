#pragma once

#ifndef CAM_CAMERA_HPP
#include "camCamera.hpp"
#endif
#ifndef CAMS_CAMERAMGR_HPP
#include "camsCameraMgr.hpp"
#endif
#ifndef CAMS_DIRECTORSCUTMGR_HPP
#include "camsDirectorsCutMgr.hpp"
#endif
#ifndef CMRA_SCRIPTDATA_HPP
#include "cmraScriptData.hpp"
#endif
#ifndef CAM3D_MGR_HPP
#include "cam3dMgr.hpp"
#endif
#ifndef CMRA_OPERATIONS_HPP
#include "cmraOperations.hpp"
#endif
#ifndef CMRA_FOLLOWUTIL_HPP
#include "cmraFollowUtil.hpp"
#endif
#ifndef CAMS_FOLLOWUTIL_HPP
#include "camsFollowUtil.hpp"
#endif
#ifndef MNM_CONSTANTS_HPP
#include "mnmConstants.hpp"
#endif
#ifndef MUI_STATUSBARMGR_HPP
#include "muiStatusBarMgr.hpp"
#endif

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
	/// Summary for cmraSystemForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmraSystemForm : public System::Windows::Forms::Form
	{
	public: 
		cmraSystemForm(void)
		{
			InitializeComponent();

			m_CameraLastIndex = 0;
		}

	protected: 
		~cmraSystemForm()
		{
			if (cmraSystemForm::FormInstance == this)
				cmraSystemForm::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TabControl ^  tabControl_cmraSystem;
	private: System::Windows::Forms::TabPage ^  tabPage_cmra;

	private: System::Windows::Forms::Button ^  button_swapcam;
	private: System::Windows::Forms::Button ^  button_seteditcam;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_editcamFOV;
	private: System::Windows::Forms::Label ^  label_editcamFOV;
	private: System::Windows::Forms::Label ^  label_selectedcam;
	private: System::Windows::Forms::ListBox ^  listBox_cameras;



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
			this->tabControl_cmraSystem = gcnew System::Windows::Forms::TabControl();
			this->tabPage_cmra = gcnew System::Windows::Forms::TabPage();
			this->listBox_cameras = gcnew System::Windows::Forms::ListBox();
			this->label_selectedcam = gcnew System::Windows::Forms::Label();
			this->label_editcamFOV = gcnew System::Windows::Forms::Label();
			this->floatEdit_editcamFOV = gcnew TerawattManagedControls::FloatEdit();
			this->button_seteditcam = gcnew System::Windows::Forms::Button();
			this->button_swapcam = gcnew System::Windows::Forms::Button();
			this->tabControl_cmraSystem->SuspendLayout();
			this->tabPage_cmra->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_cmraSystem
			// 
			this->tabControl_cmraSystem->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_cmraSystem->Controls->Add(this->tabPage_cmra);
			this->tabControl_cmraSystem->Location = System::Drawing::Point(8, 8);
			this->tabControl_cmraSystem->Name = "tabControl_cmraSystem";
			this->tabControl_cmraSystem->SelectedIndex = 0;
			this->tabControl_cmraSystem->Size = System::Drawing::Size(360, 288);
			this->tabControl_cmraSystem->TabIndex = 0;
			// 
			// tabPage_cmra
			// 
			this->tabPage_cmra->Controls->Add(this->listBox_cameras);
			this->tabPage_cmra->Controls->Add(this->label_selectedcam);
			this->tabPage_cmra->Controls->Add(this->label_editcamFOV);
			this->tabPage_cmra->Controls->Add(this->floatEdit_editcamFOV);
			this->tabPage_cmra->Controls->Add(this->button_seteditcam);
			this->tabPage_cmra->Controls->Add(this->button_swapcam);
			this->tabPage_cmra->Location = System::Drawing::Point(4, 22);
			this->tabPage_cmra->Name = "Camera";
			this->tabPage_cmra->Size = System::Drawing::Size(352, 262);
			this->tabPage_cmra->TabIndex = 0;
			this->tabPage_cmra->Text = "Cam Select";
			// 
			// listBox_cameras
			// 
			this->listBox_cameras->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->listBox_cameras->Location = System::Drawing::Point(8, 32);
			this->listBox_cameras->Name = "listBox_cameras";
			this->listBox_cameras->Size = System::Drawing::Size(333, 147);
			this->listBox_cameras->TabIndex = 6;
			this->listBox_cameras->SelectedIndexChanged += gcnew System::EventHandler(this, &cmraSystemForm::listBox_cameras_SelectedIndexChanged);
			// 
			// label_selectedcam
			// 
			this->label_selectedcam->Location = System::Drawing::Point(8, 8);
			this->label_selectedcam->Name = "label_selectedcam";
			this->label_selectedcam->Size = System::Drawing::Size(336, 16);
			this->label_selectedcam->TabIndex = 5;
			this->label_selectedcam->Text = "Selected Camera";
			// 
			// label_editcamFOV
			// 
			this->label_editcamFOV->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->label_editcamFOV->Location = System::Drawing::Point(56, 233);
			this->label_editcamFOV->Name = "label_editcamFOV";
			this->label_editcamFOV->Size = System::Drawing::Size(80, 20);
			this->label_editcamFOV->TabIndex = 4;
			this->label_editcamFOV->Text = "Edit Cam FOV";
			// 
			// floatEdit_editcamFOV
			// 
			this->floatEdit_editcamFOV->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->floatEdit_editcamFOV->Location = System::Drawing::Point(144, 231);
			this->floatEdit_editcamFOV->Name = "floatEdit_editcamFOV";
			this->floatEdit_editcamFOV->Precision = (System::Int16)2;
			this->floatEdit_editcamFOV->Size = System::Drawing::Size(72, 24);
			this->floatEdit_editcamFOV->TabIndex = 3;
			this->floatEdit_editcamFOV->ValueChanged += gcnew System::EventHandler(this, &cmraSystemForm::floatEdit_editcamFOV_ValueChanged);
			// 
			// button_seteditcam
			// 
			this->button_seteditcam->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_seteditcam->Location = System::Drawing::Point(144, 190);
			this->button_seteditcam->Name = "button_seteditcam";
			this->button_seteditcam->Size = System::Drawing::Size(120, 32);
			this->button_seteditcam->TabIndex = 2;
			this->button_seteditcam->Text = "Set EditCam";
			this->button_seteditcam->Click += gcnew System::EventHandler(this, &cmraSystemForm::button_seteditcam_Click);
			// 
			// button_swapcam
			// 
			this->button_swapcam->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_swapcam->Location = System::Drawing::Point(8, 190);
			this->button_swapcam->Name = "button_swapcam";
			this->button_swapcam->Size = System::Drawing::Size(120, 32);
			this->button_swapcam->TabIndex = 1;
			this->button_swapcam->Text = "Swap Cam";
			this->button_swapcam->Click += gcnew System::EventHandler(this, &cmraSystemForm::button_swapcam_Click);
			// 
			// cmraSystemForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(376, 302);
			this->Controls->Add(this->tabControl_cmraSystem);
			this->Name = "cmraSystemForm";
			this->Text = "Edit Camera Control";
			this->tabControl_cmraSystem->ResumeLayout(false);
			this->tabPage_cmra->ResumeLayout(false);
			this->ResumeLayout(false);

		}		

private: System::Void listBox_cameras_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			//if (!m_bDisableNotify)
			{
				int sel_index;
				sel_index = UpdateSelectedIndex();

				if (sel_index >= 0)
				{
					if ( sel_index > 0 )
					{
						m_CameraLastIndex = sel_index;
						nameString name_str;
						camsCameraMgr::GetCameraName(sel_index-1, name_str);						
						muiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Camera, name_str.GetString().c_str() );
					}
					else
					{
						SelectEditorCam();
						muiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Camera, "EditCam" );
					}
				}
			}
		 }

private: System::Void button_swapcam_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			// store the current selected index BEFORE it changes.
			//
			int sel_index;
			sel_index = UpdateSelectedIndex();

			//	only save the selected index if it ISN'T the editor cam.
			//
			if ( sel_index != m_CameraLastIndex )
			{
				this->listBox_cameras->SelectedIndex = m_CameraLastIndex;
			}
			else
			{
				SelectEditorCam();
			}
		 }

private: System::Void button_seteditcam_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			// store the current selected index BEFORE it changes.
			//
			int sel_index;
			sel_index = UpdateSelectedIndex();

			//	only save the selected index if it ISN'T the editor cam.
			//
			if ( sel_index != 0 )
			{
				//	set the editor cam to the current camera settings
				//	(position + target)
				//
				camCamera& CurCam = cam3dMgr::GetCamera();
				camCamera& EditCam = cam3dMgr::GetEditorCamera();

				EditCam.LookAt(CurCam.GetPosition(), CurCam.GetTarget(), CurCam.GetUp());
				//cam3dMgr::SetManipPositionAndTarget( CurCam.GetPosition(), CurCam.GetTarget() );
				cam3dMgr::SetCameraFOV( cam3dMgr::GetEditorCamera(), CurCam.GetFOV() );

				// set to editor cam
				listBox_cameras->SelectedIndex = 0;
			}
		 }

private: System::Void floatEdit_editcamFOV_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			//	set the editor cam FOV
			//
			camCamera& EditorCam = cam3dMgr::GetEditorCamera();
			EditorCam.SetFOV( (float)floatEdit_editcamFOV->Value );
		 }

	public:
    	//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_cmra;
		}

		//
		int UpdateSelectedIndex()
		{
			int sel_index;
			sel_index = listBox_cameras->SelectedIndex;
			if (sel_index >= 0)
			{
				cmraOperations::SelectFollowCamera(sel_index-1);
			}

			return sel_index;
		}

		void SelectEditorCam()
		{
			// set the edit cam
			//
			listBox_cameras->SelectedIndex = 0;
			cam3dMgr::UseScriptedCamera(NULL);

			cmraFollowUtil::ResetManipCamera();
		}

		// Call this to update dialog to new data
		void Update()
		{
			m_bDisableNotify = true;

			System::String^ pCamName;
			System::String^ pCamDesc;
			System::String^ pCamDisplay;

			// listbox_cameras
			this->listBox_cameras->Items->Clear();
			this->listBox_cameras->Items->Add("Editor Camera");
			nameString name_str;
			std::string desc;
			// Add names of cameras
			const int num_cameras = camsCameraMgr::GetNumCameras();
			for (int i=0; i<num_cameras; i++)
			{
				camsCameraMgr::GetCameraName(i, name_str);
				pCamName = gcnew System::String( name_str.GetString().c_str() );
				camsCameraMgr::GetCameraDescription(i, desc);
				pCamDesc = gcnew System::String( desc.c_str() );
				pCamDisplay = String::Format("{0} - {1}", pCamName, pCamDesc );

				this->listBox_cameras->Items->Add( pCamDisplay );
			}
			// Add names of directors cuts
			const int num_dcuts = camsDirectorsCutMgr::GetNumDirectorsCuts();
			for (int i=0; i<num_dcuts; i++)
			{
				camsDirectorsCutMgr::GetDirectorsCutName(i, name_str);
				pCamName = gcnew System::String( name_str.GetString().c_str() );
				camsDirectorsCutMgr::GetDirectorsCutDescription(i, desc);
				pCamDesc = gcnew System::String( desc.c_str() );
				pCamDisplay = String::Format("{0} - {1}", pCamName, pCamDesc );

				this->listBox_cameras->Items->Add( pCamDisplay );
			}
			this->listBox_cameras->SelectedIndex = (camsFollowUtil::GetFollowIndex()+1);

			camCamera& EditorCam = cam3dMgr::GetEditorCamera();
			floatEdit_editcamFOV->Value = EditorCam.GetFOV();

			m_bDisableNotify = false;
		}


	public: static cmraSystemForm^ FormInstance = nullptr;
	public: bool m_bDisableNotify;
	private: int m_CameraLastIndex;

};
}
#endif // _MANAGED
