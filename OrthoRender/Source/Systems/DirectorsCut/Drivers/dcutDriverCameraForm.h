#pragma once

#ifndef DCUT_DRIVERCAMERA_HPP
#include "Systems/DirectorsCut/Drivers/dcutDriverCamera.hpp"
#endif
#ifndef DCUT_DRIVERCAMERAINFO_HPP
#include "Systems/DirectorsCut/Drivers/dcutDriverCameraInfo.hpp"
#endif
#ifndef CAMS_CAMERAMGR_HPP
#include "Support/cams/camsCameraMgr.hpp"
#endif
#ifndef CHNL_DIALOGUTIL_HPP
#include "Features/Channels/chnlDialogUtil.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif

#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace SystemDirectorsCut
{
	/// <summary> 
	/// Summary for dcutDriverCameraForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class dcutDriverCameraForm : public System::Windows::Forms::Form
	{
	public: 
		dcutDriverCameraForm(dcutDriverCamera &i_Driver)
			: m_Driver(i_Driver)
		{
			m_bDisableNotify = true;

			InitializeComponent();

			SetupDialog();

			m_bDisableNotify = false;
		}

		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_camera;
		}

        
	protected: 
		~dcutDriverCameraForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: dcutDriverCamera &m_Driver;
	private: bool m_bDisableNotify;


	private: System::Windows::Forms::TabPage ^  tabPage_camera;
	private: System::Windows::Forms::ComboBox ^  comboBox_cameras;
	private: System::Windows::Forms::Label ^  label_attachobject;
	private: System::Windows::Forms::TabControl ^  tabControl_CameraCut;

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
			this->tabControl_CameraCut = gcnew System::Windows::Forms::TabControl();
			this->tabPage_camera = gcnew System::Windows::Forms::TabPage();
			this->comboBox_cameras = gcnew System::Windows::Forms::ComboBox();
			this->label_attachobject = gcnew System::Windows::Forms::Label();
			this->tabControl_CameraCut->SuspendLayout();
			this->tabPage_camera->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_CameraCut
			// 
			this->tabControl_CameraCut->Controls->Add(this->tabPage_camera);
			this->tabControl_CameraCut->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl_CameraCut->Location = System::Drawing::Point(0, 0);
			this->tabControl_CameraCut->Name = "tabControl_CameraCut";
			this->tabControl_CameraCut->SelectedIndex = 0;
			this->tabControl_CameraCut->Size = System::Drawing::Size(376, 262);
			this->tabControl_CameraCut->TabIndex = 8;
			// 
			// tabPage_camera
			// 
			this->tabPage_camera->Controls->Add(this->comboBox_cameras);
			this->tabPage_camera->Controls->Add(this->label_attachobject);
			this->tabPage_camera->Location = System::Drawing::Point(4, 22);
			this->tabPage_camera->Name = "tabPage_camera";
			this->tabPage_camera->Size = System::Drawing::Size(368, 236);
			this->tabPage_camera->TabIndex = 0;
			this->tabPage_camera->Text = "Camera Cut";
			// 
			// comboBox_cameras
			// 
			this->comboBox_cameras->Location = System::Drawing::Point(96, 16);
			this->comboBox_cameras->Name = "comboBox_cameras";
			this->comboBox_cameras->Size = System::Drawing::Size(192, 21);
			this->comboBox_cameras->TabIndex = 7;
			this->comboBox_cameras->SelectedIndexChanged += gcnew System::EventHandler(this, &dcutDriverCameraForm::comboBox_cameras_SelectedIndexChanged);
			// 
			// label_attachobject
			// 
			this->label_attachobject->Location = System::Drawing::Point(16, 16);
			this->label_attachobject->Name = "label_attachobject";
			this->label_attachobject->Size = System::Drawing::Size(80, 28);
			this->label_attachobject->TabIndex = 0;
			this->label_attachobject->Text = "Camera:";
			// 
			// dcutDriverCameraForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(376, 262);
			this->Controls->Add(this->tabControl_CameraCut);
			this->Name = "dcutDriverCameraForm";
			this->Text = "Camera Cut Properties";
			this->tabControl_CameraCut->ResumeLayout(false);
			this->tabPage_camera->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//
		private:
		//	Set-up the components on construction
		//
		void SetupDialog()
		{

			//	fill-in combobox
			//
			nameString name;
			const int num_cameras = camsCameraMgr::GetNumCameras();
			for ( int i = 0 ; i < num_cameras; i++ )
			{
				//DBG_LOG2( "%02d) %s", i, name.GetString().c_str() );
		
				camsCameraMgr::GetCameraName(i, name);
				comboBox_cameras->Items->Add( gcnew String(name.GetString().c_str()) );
			}

			const nameString &camera_name = m_Driver.GetCameraName();
			if ( !camera_name.GetString().empty() )
			{

				System::String ^mstr = comboBox_cameras->Text;
				std::string str;
				tmaManagedStringUtils::ManagedStringToStdString(mstr, str);

				//DBG_LOG2( "object (%s) vs object (%s)", str.c_str(), pInfo->m_ObjectName.GetString().c_str() );

				if ( camera_name.GetString() != str )
				{
					comboBox_cameras->Text = gcnew String( camera_name.GetString().c_str() );
				}
			}

		}

	private: System::Void comboBox_cameras_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			if (!m_bDisableNotify)
			{
				m_bDisableNotify = true;
				dcutDriverCameraInfo *pInfo = dynamic_cast<dcutDriverCameraInfo*>(m_Driver.GetDriverInfo());
				int sel_ind = this->comboBox_cameras->SelectedIndex;
				if (sel_ind >= 0)
					camsCameraMgr::GetCameraName(sel_ind, pInfo->m_CameraName);
				m_Driver.SetDriverInfo(*pInfo, prtyProperty::eNewUndo);
				delete pInfo;
				chnlDialogUtil::UpdateDriver(&m_Driver);
				m_bDisableNotify = false;
			}	
		}

};
}
#endif // _MANAGED
