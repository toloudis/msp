#pragma once

#ifndef CPTR_RENDERPROGRESSDIALOGUTIL_HPP
#include "Features/Capture/cptrRenderProgressDialogUtil.hpp"
#endif
#ifndef MODE_MODEMGR_HPP
#include "Support/mode/modeModeMgr.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif

#ifdef _MANAGED

#ifndef CPTR_MODERENDER_HPP
#include "Features/Capture/cptrModeRender.hpp"
#endif
#ifndef CPTR_MODERENDERBATCH_HPP
#include "Features/Capture/cptrModeRenderBatch.hpp"
#endif


//
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


//
namespace StudioFramework
{
	/// <summary>
	/// Summary for cptrRenderProgressForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cptrRenderProgressForm : public System::Windows::Forms::Form
	{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrRenderProgressForm()
		{
			InitializeComponent();

			m_RenderBatchID = 0;
		}

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~cptrRenderProgressForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button ^  button_abort;
	private: System::Windows::Forms::Button ^  button_pause;
	private: System::Windows::Forms::Button ^  button_save;
	private: System::Windows::Forms::Label^  label_progress_scenes;
	private: System::Windows::Forms::ProgressBar^  progressBar_scenes;
	private: System::Windows::Forms::Label^  label_progress_scene;
	private: System::Windows::Forms::Label^  label_progress_cameras;
	private: System::Windows::Forms::ProgressBar^  progressBar_cameras;
	private: System::Windows::Forms::Label^  label_progress_camera;
	private: System::Windows::Forms::ProgressBar^  progressBar_camera;
	private: System::Windows::Forms::Label^  label_estimatedtimeleft;
	private: System::Windows::Forms::Label^  label_timeleft;
	private: System::ComponentModel::IContainer ^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->button_abort = (gcnew System::Windows::Forms::Button());
			this->button_pause = (gcnew System::Windows::Forms::Button());
			this->button_save = (gcnew System::Windows::Forms::Button());
			this->label_progress_scenes = (gcnew System::Windows::Forms::Label());
			this->progressBar_scenes = (gcnew System::Windows::Forms::ProgressBar());
			this->progressBar_cameras = (gcnew System::Windows::Forms::ProgressBar());
			this->label_progress_scene = (gcnew System::Windows::Forms::Label());
			this->label_progress_cameras = (gcnew System::Windows::Forms::Label());
			this->progressBar_camera = (gcnew System::Windows::Forms::ProgressBar());
			this->label_progress_camera = (gcnew System::Windows::Forms::Label());
			this->label_estimatedtimeleft = (gcnew System::Windows::Forms::Label());
			this->label_timeleft = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// button_abort
			// 
			this->button_abort->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_abort->BackColor = System::Drawing::SystemColors::Control;
			this->button_abort->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button_abort->ForeColor = System::Drawing::SystemColors::ControlText;
			this->button_abort->Location = System::Drawing::Point(48, 239);
			this->button_abort->Name = L"button_abort";
			this->button_abort->Size = System::Drawing::Size(84, 40);
			this->button_abort->TabIndex = 0;
			this->button_abort->Text = L"ABORT Render";
			this->button_abort->UseVisualStyleBackColor = false;
			this->button_abort->Click += gcnew System::EventHandler(this, &cptrRenderProgressForm::button_abort_Click);
			// 
			// button_pause
			// 
			this->button_pause->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_pause->BackColor = System::Drawing::SystemColors::Control;
			this->button_pause->Enabled = false;
			this->button_pause->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button_pause->ForeColor = System::Drawing::Color::Black;
			this->button_pause->Location = System::Drawing::Point(8, 191);
			this->button_pause->Name = L"button_pause";
			this->button_pause->Size = System::Drawing::Size(84, 40);
			this->button_pause->TabIndex = 5;
			this->button_pause->Text = L"PAUSE Render";
			this->button_pause->UseVisualStyleBackColor = false;
			this->button_pause->Click += gcnew System::EventHandler(this, &cptrRenderProgressForm::button_pause_Click);
			// 
			// button_save
			// 
			this->button_save->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_save->BackColor = System::Drawing::SystemColors::Control;
			this->button_save->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button_save->ForeColor = System::Drawing::Color::Black;
			this->button_save->Location = System::Drawing::Point(96, 191);
			this->button_save->Name = L"button_save";
			this->button_save->Size = System::Drawing::Size(84, 40);
			this->button_save->TabIndex = 6;
			this->button_save->Text = L"SAVE Render Pt";
			this->button_save->UseVisualStyleBackColor = false;
			this->button_save->Click += gcnew System::EventHandler(this, &cptrRenderProgressForm::button_save_Click);
			// 
			// label_progress_scenes
			// 
			this->label_progress_scenes->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->label_progress_scenes->Location = System::Drawing::Point(8, 2);
			this->label_progress_scenes->Name = L"label_progress_scenes";
			this->label_progress_scenes->Size = System::Drawing::Size(165, 23);
			this->label_progress_scenes->TabIndex = 7;
			this->label_progress_scenes->Text = L"Scenes";
			this->label_progress_scenes->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// progressBar_scenes
			// 
			this->progressBar_scenes->Location = System::Drawing::Point(7, 28);
			this->progressBar_scenes->Name = L"progressBar_scenes";
			this->progressBar_scenes->Size = System::Drawing::Size(166, 17);
			this->progressBar_scenes->Step = 100;
			this->progressBar_scenes->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
			this->progressBar_scenes->TabIndex = 8;
			// 
			// progressBar_cameras
			// 
			this->progressBar_cameras->Location = System::Drawing::Point(8, 93);
			this->progressBar_cameras->Name = L"progressBar_cameras";
			this->progressBar_cameras->Size = System::Drawing::Size(166, 17);
			this->progressBar_cameras->Step = 100;
			this->progressBar_cameras->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
			this->progressBar_cameras->TabIndex = 10;
			// 
			// label_progress_scene
			// 
			this->label_progress_scene->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->label_progress_scene->Location = System::Drawing::Point(8, 47);
			this->label_progress_scene->Name = L"label_progress_scene";
			this->label_progress_scene->Size = System::Drawing::Size(165, 23);
			this->label_progress_scene->TabIndex = 9;
			this->label_progress_scene->Text = L"Current Scene";
			this->label_progress_scene->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_progress_cameras
			// 
			this->label_progress_cameras->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->label_progress_cameras->Location = System::Drawing::Point(8, 70);
			this->label_progress_cameras->Name = L"label_progress_cameras";
			this->label_progress_cameras->Size = System::Drawing::Size(165, 23);
			this->label_progress_cameras->TabIndex = 11;
			this->label_progress_cameras->Text = L"Cameras";
			this->label_progress_cameras->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// progressBar_camera
			// 
			this->progressBar_camera->Location = System::Drawing::Point(8, 135);
			this->progressBar_camera->Name = L"progressBar_camera";
			this->progressBar_camera->Size = System::Drawing::Size(166, 17);
			this->progressBar_camera->Step = 100;
			this->progressBar_camera->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
			this->progressBar_camera->TabIndex = 14;
			// 
			// label_progress_camera
			// 
			this->label_progress_camera->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->label_progress_camera->Location = System::Drawing::Point(8, 112);
			this->label_progress_camera->Name = L"label_progress_camera";
			this->label_progress_camera->Size = System::Drawing::Size(165, 23);
			this->label_progress_camera->TabIndex = 13;
			this->label_progress_camera->Text = L"Current Camera";
			this->label_progress_camera->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_estimatedtimeleft
			// 
			this->label_estimatedtimeleft->AutoSize = true;
			this->label_estimatedtimeleft->Location = System::Drawing::Point(8, 165);
			this->label_estimatedtimeleft->Name = L"label_estimatedtimeleft";
			this->label_estimatedtimeleft->Size = System::Drawing::Size(72, 13);
			this->label_estimatedtimeleft->TabIndex = 15;
			this->label_estimatedtimeleft->Text = L"Est. Time Left";
			// 
			// label_timeleft
			// 
			this->label_timeleft->AutoSize = true;
			this->label_timeleft->Location = System::Drawing::Point(99, 165);
			this->label_timeleft->Name = L"label_timeleft";
			this->label_timeleft->Size = System::Drawing::Size(64, 13);
			this->label_timeleft->TabIndex = 16;
			this->label_timeleft->Text = L"00:00:00.00";
			// 
			// cptrRenderProgressForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->BackColor = System::Drawing::SystemColors::Control;
			this->ClientSize = System::Drawing::Size(186, 288);
			this->ControlBox = false;
			this->Controls->Add(this->label_timeleft);
			this->Controls->Add(this->label_estimatedtimeleft);
			this->Controls->Add(this->progressBar_camera);
			this->Controls->Add(this->label_progress_camera);
			this->Controls->Add(this->label_progress_cameras);
			this->Controls->Add(this->progressBar_cameras);
			this->Controls->Add(this->label_progress_scene);
			this->Controls->Add(this->progressBar_scenes);
			this->Controls->Add(this->label_progress_scenes);
			this->Controls->Add(this->button_save);
			this->Controls->Add(this->button_pause);
			this->Controls->Add(this->button_abort);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->MinimumSize = System::Drawing::Size(194, 322);
			this->Name = L"cptrRenderProgressForm";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Render In Progress";
			this->TopMost = true;
			this->Load += gcnew System::EventHandler(this, &cptrRenderProgressForm::cptrRenderProgressForm_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}

private:
	tmaDialogMemory^	m_pMemory;
	int m_RenderBatchID;
	float m_fLastScenesPercentage;

public:
		//--------------------------------------------------------------------
		//	set the percentage for scenes
		//--------------------------------------------------------------------
		void SetScenesRenderPercentage( float i_percentage, std::string& i_label )
		{
			m_fLastScenesPercentage = i_percentage;
			progressBar_scenes->Value =(int)i_percentage;

			if (i_label.length() > 0)
			{
				label_progress_scenes->Text = gcnew String(i_label.c_str());
			}
		}

		//--------------------------------------------------------------------
		//	set the percentage for scene
		//--------------------------------------------------------------------
		void SetSceneLabel( std::string& i_label )
		{
			if (i_label.length() > 0)
			{
				label_progress_scene->Text = gcnew String(i_label.c_str());
			}
		}

		//--------------------------------------------------------------------
		//	set the percentage for cameras
		//--------------------------------------------------------------------
		void SetCamerasRenderPercentage( float i_percentage, std::string& i_label )
		{
			progressBar_cameras->Value =(int)i_percentage;

			if (i_label.length() > 0)
			{
				label_progress_cameras->Text = gcnew String(i_label.c_str());
			}
		}

		//--------------------------------------------------------------------
		//	set the percentage for camera
		//--------------------------------------------------------------------
		void SetCameraRenderPercentage( float i_percentage, std::string& i_label )
		{
			progressBar_camera->Value =( int)i_percentage;

			if (i_label.length() > 0)
			{
				label_progress_camera->Text = gcnew String(i_label.c_str());
			}
		}

		//--------------------------------------------------------------------
		//	set the time that has elapsed
		//--------------------------------------------------------------------
		void SetTimeElapsed( float i_fTimeElapsed )
		{
			float timeleft = calculate_time_left( i_fTimeElapsed );
			this->label_timeleft->Text = format_time_left( timeleft );
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void EnablePauseButton()
		{
			this->button_pause->Enabled = true;
		}
		void DisablePauseButton()
		{
			this->button_pause->Enabled = false;
		}

	void Reset()
	{
		button_abort->Enabled = true;
		button_save->Enabled = true;
	}

	void SetRenderBatchID( int i_ID )
	{
		m_RenderBatchID = i_ID;
	}

private: System::Void button_abort_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			//
			modeMode* pMode = modeModeMgr::GetCurrentMode();
			cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(pMode);
			cptrModeRenderBatch* pModeRenderBatch = 0;
			if ( pModeRender != 0 )
			{
				pModeRender->AbortRender();
				button_abort->Enabled = false;

				pModeRender->PauseRender( false );
			}

			pModeRenderBatch = dynamic_cast<cptrModeRenderBatch*>( modeModeMgr::GetMode( m_RenderBatchID ) );
			if ( pModeRenderBatch != 0 )
			{
				if ( modeModeMgr::IsInStack( m_RenderBatchID ) )
				{
					pModeRenderBatch->AbortRenders();
				}
			}
		 }

private: System::Void cptrRenderProgressForm_Load(System::Object ^  sender, System::EventArgs ^  e)
		 {
			m_pMemory = gcnew tmaDialogMemory( this );

			//	if the progress dialog has been resized too small then reset it to the minimum size
			//
			//	NOTE: force setting of this dialog, so it looks "correct".
			//
			//if (   (this->Width < this->MinimumSize.Width)
			//	|| (this->Height < this->MinimumSize.Height))
			{
				this->Size = this->MinimumSize;
			}
		 }

private:
		// way to combine pause + toggle_pause?
		//
		void pause_rendering(bool i_bPause)
		{
			modeMode* pMode = modeModeMgr::GetCurrentMode();
			cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(pMode);
			cptrModeRenderBatch* pModeRenderBatch = 0;
			if ( pModeRender != 0 )
			{
				//	if the mode is about to unpause then enable the save again.
				//
				if (pModeRender->IsRenderPaused())
					button_save->Enabled = true;

				pModeRender->PauseRender( i_bPause );
			}
		}
		//
		void toggle_pause_rendering()
		 {
			modeMode* pMode = modeModeMgr::GetCurrentMode();
			cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(pMode);
			cptrModeRenderBatch* pModeRenderBatch = 0;
			if ( pModeRender != 0 )
			{
				//	if the mode is about to unpause then enable the save again.
				//
				if (pModeRender->IsRenderPaused())
					button_save->Enabled = true;

				if (pModeRender->IsRenderPaused())
				{
					this->button_pause->Text = L"PAUSE Render";
				}
				else
				{
					this->button_pause->Text = L"RESUME Render";
				}
				pModeRender->PauseRender( !pModeRender->IsRenderPaused() );
			}
		 }

private: System::Void button_pause_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 toggle_pause_rendering();
		 }

private: System::Void button_save_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 pause_rendering(true);

			 button_save->Enabled = false;

			 // now, save the time of the render.
			 cptrRenderProgressDialogUtil::SaveRenderPosition();
		 }

private: System::String^ format_time_left(float i_fTimeLeft)
		 {
			 TimeSpan ts = TimeSpan::FromSeconds(i_fTimeLeft);
			 return System::String::Format("{0:00}:{1:00}:{2:00}.{3:00}", ts.Hours, ts.Minutes, ts.Seconds, ts.Milliseconds/10);
		 }
private: float calculate_time_left(float i_fTimeElapsed)
		 {
			 if (m_fLastScenesPercentage != 0.0f)
				return i_fTimeElapsed * ( (100.0f - m_fLastScenesPercentage) / m_fLastScenesPercentage );
			 return 0.0f;
		 }
};
}

#endif // _MANAGED
