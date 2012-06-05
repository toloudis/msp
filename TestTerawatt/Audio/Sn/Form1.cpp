#include "stdafx.h"
#include "Form1.h"
#include <windows.h>

#include "appTime.hpp"
#include "mnmApp.hpp"

using namespace Sn;

namespace
{
	snSoundJob2D* l_pSound;
	float l_start_time;
}


System::Void Form1::timer1_Elapsed(System::Object *  sender, System::Timers::ElapsedEventArgs *  e)
{
	if (!mnmApp::IsActive())
	{
		this->timer1->Enabled = true;
		return;
	}

	this->timer1->Enabled = false;

	mnmApp::ThinkApp();

	if (l_pSound != 0)
	{
		if (l_pSound->IsPlaying())
		{
			float current_time = appTime::GetTime();
			label_time->Text = String::Format("{0}", __box(current_time - l_start_time));
		}
	}

	this->timer1->Enabled = true;
}

System::Void Form1::button_play_Click(System::Object *  sender, System::EventArgs *  e)
{
	if (textBox_sound->Text->Length > 0)
	{
		String* pstr = textBox_sound->Text;

		char soundname[256];
		int len = pstr->Length;
		for (int i=0; i<len; i++)
			soundname[i] = pstr->Chars[i];

		// Start sound
		//
		fsLocator FileLoc;
		fsFileUtil::ANSIFilenameToLocator( soundname, FileLoc );
		l_pSound = snSoundManager::CreateSoundJob2DStatic( FileLoc, itString("SoundName") );
		l_pSound->SetTypeMask( snSoundManager::SOUNDTYPE_EFFECT );
		l_pSound->Load();
		l_pSound->SetDeleteWhenFinished(false);
		snSoundManager::SetupSoundJob(l_pSound);

		l_pSound->SetVolume( 1.0f );
		l_pSound->Start( 0 );
		l_start_time = appTime::GetTime();
	}
}

System::Void Form1::button_browse_Click(System::Object *  sender, System::EventArgs *  e)
{
	System::Windows::Forms::OpenFileDialog*  FileBrowserDialog;
	FileBrowserDialog = new System::Windows::Forms::OpenFileDialog;
	FileBrowserDialog->set_FileName( textBox_sound->get_Text() );

	System::Windows::Forms::DialogResult result = FileBrowserDialog->ShowDialog();
	if (result == DialogResult::OK) 
	{
		textBox_sound->set_Text( FileBrowserDialog->get_FileName() );
	}
}
