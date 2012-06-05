#pragma once

#ifndef DBG_ASSERT_HPP
#include "Core/dbg/dbgAssert.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "Core/dbg/dbgLog.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef GF_PATHS_HPP
#include "Core/gf/gfPaths.hpp"
#endif
#ifndef MODE_MODEMGR_HPP
#include "Support/mode/modeModeMgr.hpp"
#endif
#ifndef PLBK_MODEPLAYBACK_HPP
#include "Features/Playback/plbkModePlayback.hpp"
#endif
#ifndef PREFSDATA_HPP
#include "Features/Prefs/PrefsData.hpp"
#endif
#ifndef PREFSMGR_HPP
#include "Features/Prefs/PrefsMgr.hpp"
#endif
#ifndef MNM_PATHS_HPP
#include "Support/mnm/mnmPaths.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif
#ifndef TMA_MANAGEDCONTROLUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedControlUtil.hpp"
#endif
#ifndef CPTR_RENDEROUTPUTDATAUTIL_HPP
#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#endif

#include <assert.h>

#ifdef _MANAGED

//============================================================================
//============================================================================
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


//============================================================================
//============================================================================
namespace StudioFramework
{
	/// <summary>
	/// Summary for plbkPlaybackControlsForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class plbkPlaybackControlsForm : public System::Windows::Forms::Form
	{
	public:
		plbkPlaybackControlsForm()
		{
			InitializeComponent();

			std::string artdir;
			std::string artfile;
			fsFileUtil::LocatorToANSIFilename( gfPaths::GetPath(mnmPaths::e_ExeArt), artdir );
			artfile = artdir + "\\playback-pause.png";
			create_button_image( button_pause, artfile );
			artfile = artdir + "\\playback-end.png";
			create_button_image( button_end, artfile );
			artfile = artdir + "\\playback-begin.png";
			create_button_image( button_beginning, artfile );
			artfile = artdir + "\\playback-playrev.png";
			create_button_image( button_playrev, artfile );
			artfile = artdir + "\\playback-playfwd.png";
			create_button_image( button_playfwd, artfile );
			artfile = artdir + "\\playback-fastrev.png";
			create_button_image( button_fastreverse, artfile );
			artfile = artdir + "\\playback-fastfwd.png";
			create_button_image( button_fastforward, artfile );

			trackBar_slomo->Value = 0;

			//	set the default from the prefs
			PrefsData& data = PrefsMgr::Data();
			this->checkBox_loop->Checked = data.m_bPlaybackLoopAtEnd.GetValue();
			this->checkBox_lowRes->Checked = data.m_bPlaybackLowRes.GetValue();
			this->checkBox_mute->Checked = data.m_bPlaybackLowRes.GetValue();
		}

	protected:
		~plbkPlaybackControlsForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::ComponentModel::IContainer ^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

	private: System::Windows::Forms::CheckBox ^  checkBox_loop;

	private: System::Windows::Forms::Button ^  button_pause;
	private: System::Windows::Forms::Button ^  button_end;
	private: System::Windows::Forms::Button ^  button_beginning;


	private: System::Windows::Forms::Button ^  button_fastforward;
	private: System::Windows::Forms::TrackBar ^  trackBar_slomo;
	private: System::Windows::Forms::Label ^  label_slomo;
	private: System::Windows::Forms::Button ^  button_playrev;
	private: System::Windows::Forms::Button ^  button_playfwd;
	private: System::Windows::Forms::CheckBox^  checkBox_lowRes;
	private: System::Windows::Forms::CheckBox^  checkBox_mute;
	private: System::Windows::Forms::Button ^  button_fastreverse;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->checkBox_loop = (gcnew System::Windows::Forms::CheckBox());
			this->button_pause = (gcnew System::Windows::Forms::Button());
			this->button_end = (gcnew System::Windows::Forms::Button());
			this->button_beginning = (gcnew System::Windows::Forms::Button());
			this->button_playrev = (gcnew System::Windows::Forms::Button());
			this->button_playfwd = (gcnew System::Windows::Forms::Button());
			this->button_fastforward = (gcnew System::Windows::Forms::Button());
			this->button_fastreverse = (gcnew System::Windows::Forms::Button());
			this->trackBar_slomo = (gcnew System::Windows::Forms::TrackBar());
			this->label_slomo = (gcnew System::Windows::Forms::Label());
			this->checkBox_lowRes = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_mute = (gcnew System::Windows::Forms::CheckBox());
			(safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar_slomo))->BeginInit();
			this->SuspendLayout();
			// 
			// checkBox_loop
			// 
			this->checkBox_loop->Location = System::Drawing::Point(8, 72);
			this->checkBox_loop->Name = "checkBox_loop";
			this->checkBox_loop->Size = System::Drawing::Size(88, 24);
			this->checkBox_loop->TabIndex = 7;
			this->checkBox_loop->Text = "Loop At End";
			this->checkBox_loop->Click += gcnew System::EventHandler(this, &plbkPlaybackControlsForm::checkBox_loop_Click);
			// 
			// button_pause
			// 
			this->button_pause->Location = System::Drawing::Point(88, 4);
			this->button_pause->Name = "button_pause";
			this->button_pause->Size = System::Drawing::Size(32, 32);
			this->button_pause->TabIndex = 19;
			this->button_pause->Click += gcnew System::EventHandler(this, &plbkPlaybackControlsForm::button_pause_Click);
			// 
			// button_end
			// 
			this->button_end->Location = System::Drawing::Point(156, 38);
			this->button_end->Name = "button_end";
			this->button_end->Size = System::Drawing::Size(32, 32);
			this->button_end->TabIndex = 20;
			this->button_end->Click += gcnew System::EventHandler(this, &plbkPlaybackControlsForm::button_end_Click);
			// 
			// button_beginning
			// 
			this->button_beginning->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button_beginning->Location = System::Drawing::Point(20, 38);
			this->button_beginning->Name = "button_beginning";
			this->button_beginning->Size = System::Drawing::Size(32, 32);
			this->button_beginning->TabIndex = 21;
			this->button_beginning->Click += gcnew System::EventHandler(this, &plbkPlaybackControlsForm::button_beginning_Click);
			// 
			// button_playrev
			// 
			this->button_playrev->Location = System::Drawing::Point(54, 4);
			this->button_playrev->Name = "button_playrev";
			this->button_playrev->Size = System::Drawing::Size(32, 32);
			this->button_playrev->TabIndex = 22;
			this->button_playrev->Click += gcnew System::EventHandler(this, &plbkPlaybackControlsForm::button_playrev_Click);
			// 
			// button_playfwd
			// 
			this->button_playfwd->Location = System::Drawing::Point(122, 4);
			this->button_playfwd->Name = "button_playfwd";
			this->button_playfwd->Size = System::Drawing::Size(32, 32);
			this->button_playfwd->TabIndex = 23;
			this->button_playfwd->Click += gcnew System::EventHandler(this, &plbkPlaybackControlsForm::button_playfwd_Click);
			// 
			// button_fastforward
			// 
			this->button_fastforward->Location = System::Drawing::Point(122, 38);
			this->button_fastforward->Name = "button_fastforward";
			this->button_fastforward->Size = System::Drawing::Size(32, 32);
			this->button_fastforward->TabIndex = 24;
			this->button_fastforward->Click += gcnew System::EventHandler(this, &plbkPlaybackControlsForm::button_fastforward_Click);
			// 
			// button_fastreverse
			// 
			this->button_fastreverse->Location = System::Drawing::Point(54, 38);
			this->button_fastreverse->Name = "button_fastreverse";
			this->button_fastreverse->Size = System::Drawing::Size(32, 32);
			this->button_fastreverse->TabIndex = 25;
			this->button_fastreverse->Click += gcnew System::EventHandler(this, &plbkPlaybackControlsForm::button_fastreverse_Click);
			// 
			// trackBar_slomo
			// 
			this->trackBar_slomo->LargeChange = 1;
			this->trackBar_slomo->Location = System::Drawing::Point(144, 72);
			this->trackBar_slomo->Maximum = 5;
			this->trackBar_slomo->Name = "trackBar_slomo";
			this->trackBar_slomo->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->trackBar_slomo->Size = System::Drawing::Size(64, 45);
			this->trackBar_slomo->TabIndex = 27;
			this->trackBar_slomo->ValueChanged += gcnew System::EventHandler(this, &plbkPlaybackControlsForm::trackBar_slomo_ValueChanged);
			// 
			// label_slomo
			// 
			this->label_slomo->Location = System::Drawing::Point(104, 72);
			this->label_slomo->Name = "label_slomo";
			this->label_slomo->Size = System::Drawing::Size(40, 23);
			this->label_slomo->TabIndex = 28;
			this->label_slomo->Text = "SloMo";
			this->label_slomo->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// checkBox_lowRes
			// 
			this->checkBox_lowRes->Location = System::Drawing::Point(110, 102);
			this->checkBox_lowRes->Name = "checkBox_lowRes";
			this->checkBox_lowRes->Size = System::Drawing::Size(130, 24);
			this->checkBox_lowRes->TabIndex = 30;
			this->checkBox_lowRes->Text = "Low Resolution";
			this->checkBox_lowRes->CheckedChanged += gcnew System::EventHandler(this, &plbkPlaybackControlsForm::checkBox_lowRes_CheckedChanged);
			// 
			// checkBox_mute
			// 
			this->checkBox_mute->Location = System::Drawing::Point(8, 102);
			this->checkBox_mute->Name = "checkBox_mute";
			this->checkBox_mute->Size = System::Drawing::Size(92, 24);
			this->checkBox_mute->TabIndex = 29;
			this->checkBox_mute->Text = "Mute";
			this->checkBox_mute->CheckedChanged += gcnew System::EventHandler(this, &plbkPlaybackControlsForm::checkBox_mute_CheckedChanged);
			// 
			// plbkPlaybackControlsForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(216, 133);
			this->Controls->Add(this->checkBox_mute);
			this->Controls->Add(this->checkBox_lowRes);
			this->Controls->Add(this->label_slomo);
			this->Controls->Add(this->trackBar_slomo);
			this->Controls->Add(this->button_fastreverse);
			this->Controls->Add(this->button_fastforward);
			this->Controls->Add(this->button_playfwd);
			this->Controls->Add(this->button_playrev);
			this->Controls->Add(this->button_beginning);
			this->Controls->Add(this->button_end);
			this->Controls->Add(this->button_pause);
			this->Controls->Add(this->checkBox_loop);
			this->KeyPreview = true;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = "plbkPlaybackControlsForm";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = "Playback Controls";
			this->TopMost = true;
			this->Activated += gcnew System::EventHandler(this, &plbkPlaybackControlsForm::plbkPlaybackControlsForm_Activated);
			this->Closing += gcnew System::ComponentModel::CancelEventHandler(this, &plbkPlaybackControlsForm::plbkPlaybackControlsForm_Closing);
			this->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &plbkPlaybackControlsForm::plbkPlaybackControlsForm_KeyPress);
			this->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &plbkPlaybackControlsForm::plbkPlaybackControlsForm_KeyUp);
			this->Load += gcnew System::EventHandler(this, &plbkPlaybackControlsForm::plbkPlaybackControlsForm_Load);
			(safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar_slomo))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}

private: System::Void button_beginning_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 plbkModePlayback* pMode = get_playbackmode();
			 pMode->GoBegin();
		 }

private: System::Void button_fastreverse_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 plbkModePlayback* pMode = get_playbackmode();
			 pMode->FastRev();
		 }

private: System::Void button_fastforward_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 plbkModePlayback* pMode = get_playbackmode();
			 pMode->FastFwd();
		 }

private: System::Void button_end_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 plbkModePlayback* pMode = get_playbackmode();
			 pMode->GoEnd();
		 }

private: System::Void button_playrev_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 plbkModePlayback* pMode = get_playbackmode();
			 pMode->PlayRev();
		 }

private: System::Void button_pause_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 plbkModePlayback* pMode = get_playbackmode();
			 pMode->Pause();
		 }

private: System::Void button_playfwd_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 plbkModePlayback* pMode = get_playbackmode();
			 pMode->Play();
		 }

private: System::Void checkBox_loop_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 plbkModePlayback* pMode = get_playbackmode();
			 pMode->SetLooping( checkBox_loop->Checked );
		 }
private:
	plbkModePlayback* get_playbackmode()
	{
		modeMode* pMode = modeModeMgr::GetCurrentMode();
		plbkModePlayback* pPBMode = dynamic_cast<plbkModePlayback*>( pMode );

		DBG_ASSERT0( pPBMode != 0, "Mode Playback not found" );
		return pPBMode;
	}

private:
	tmaDialogMemory^	m_pMemory;

private: System::Void plbkPlaybackControlsForm_Load(System::Object ^  sender, System::EventArgs ^  e)
		 {
			m_pMemory = gcnew tmaDialogMemory( this );
		 }

private: System::Void trackBar_slomo_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 plbkModePlayback* pMode = get_playbackmode();
			 int val = trackBar_slomo->Value;
			 if (val == 0)
				 pMode->SetSloMo( 1 );
			 else
			 {
                 cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
				 pMode->SetSloMo( 1.0f/(data.m_fCaptureFPS.GetValue() * (val)));
			 }
		 }

private: System::Void plbkPlaybackControlsForm_Activated(System::Object ^  sender, System::EventArgs ^  e)
		 {
			//	set the default from the prefs
			PrefsData& data = PrefsMgr::Data();
			this->checkBox_loop->Checked = data.m_bPlaybackLoopAtEnd.GetValue();
			this->checkBox_lowRes->Checked = data.m_bPlaybackLowRes.GetValue();
		 }

private: System::Void plbkPlaybackControlsForm_Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e)
		 {
			plbkModePlayback* pMode = get_playbackmode();
			pMode->ExitMode();
		 }
private: System::Void plbkPlaybackControlsForm_KeyPress(System::Object ^  sender, System::Windows::Forms::KeyPressEventArgs ^  e)
		 {
			switch ( e->KeyChar )
			{
				//	pause/restart playback
				case ' ':
				{
					plbkModePlayback* pMode = get_playbackmode();
					pMode->TogglePlayAndPause();
					e->Handled = true;
					break;
				}
				default:
					break;
			}
		}

private: System::Void plbkPlaybackControlsForm_KeyUp(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
		 {
			 //	pause/restart playback
			 //
			//if (e->KeyCode == Keys::Space)
			//{
			//	plbkModePlayback* pMode = get_playbackmode();
			//	pMode->TogglePlayAndPause();
			//	e->set_Handled(true);
			//}
		 }

private: System::Void checkBox_lowRes_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 plbkModePlayback* pMode = get_playbackmode();
			 pMode->SetLowRes( checkBox_lowRes->Checked );
		 }
private: System::Void checkBox_mute_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 plbkModePlayback* pMode = get_playbackmode();
			 pMode->SetMute( checkBox_mute->Checked );
		 }
 private:
		//--------------------------------------------------------------------------
		//	create a button image with exception handling
		//--------------------------------------------------------------------------
		void create_button_image( System::Windows::Forms::Button^ i_pButton, std::string& i_Path )
		{
			try
			{
				tmaManagedControlUtil::Create_Button_Image( i_pButton, gcnew System::String(i_Path.c_str()) );
			}
			catch (System::IO::FileNotFoundException^ )
			{
				//std::string exfn;
				//tmaManagedStringUtils::ManagedStringToStdString( ex->FileName, exfn );
				DBG_ERROR1("Cannot find file (%s)", i_Path.c_str());
			
				//	Message Box shows up behind the loading dialog
				//guiMessageBox::Show(icon_filename, "File Missing Error");
			
				assert(false);
			}
		}
};
}

#endif // _MANAGED
