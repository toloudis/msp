#include "stdafx.h"
#include "Form1.h"

#include "tasApp.hpp"

#include "tasTUVData.hpp"
#include "tasTUVDataUtil.hpp"
#include "tasTUVMgr.hpp"

#include "tasUVAData.hpp"
#include "tasUVADataUtil.hpp"
#include "tasUVAMgr.hpp"

#include "dbgLog.hpp"
#include "dbgPackage.hpp"
#include "docSingleDocumentMgr.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "g2dExceptionX.hpp"
#include "g2dImageCreate.hpp"
#include "itStringUtil.hpp"
#include "muiFileDialogUtils.hpp"
#include "muiMessageBox.hpp"
#include "tmaManagedControlUtil.hpp"
#include "tmaManagedStringUtils.hpp"

#include <assert.h>
#include <string>
#include <windows.h>


//============================================================================
//============================================================================
using namespace TextureAnimationStudio;
//using namespace SplashNS;
using namespace System::Windows::Forms;


//============================================================================
//============================================================================
const char *c_UVAFileFilter = "Animated UV files (*.uva)|*.uva|All files (*.*)|*.*";
const char *c_TUVFileFilter = "Animated Texture files (*.tuv)|*.tuv|All files (*.*)|*.*";
const char *c_ImageFileFilter = "images files (*.bmp;*.png;*.dds;*.tga;*.jpg))|*.bmp;*.png;*.dds;*.tga;*.jpg|All files (*.*)|*.*";

//============================================================================
//============================================================================


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
Form1::Form1(void)
{
	InitializeComponent();

	// Set-up the application idle event handler
	//
	System::Windows::Forms::Application::Idle += gcnew System::EventHandler(this, &Form1::OnApplicationIdle);				

	//	Custom configure controls
	this->textBox_instructions->Text = \
"First go to the TEXTURE tab.\r\n\
	- Select images that will get put together on texture pages.\r\n\
	- When list is set, save the texture.\r\n\
	- hit the send button to send it to the animation step.\r\n\r\n\
Animation Tab\r\n\
	- Take an image and create a file that sets the parameters for\r\n\
	animated playback.\r\n\
	- save.\r\n\r\n\
These steps will create 3 files:\r\n\
	the texture,\r\n\
	the UVA file (UV offsets),\r\n\
	and the TUV file (animation parameters)\r\n";

	this->tabControl_tools->SelectedTab = this->tabPage_texture;
	this->checkedListBox_UVA_resolution->SelectedIndex = 3;	// default it to the highest supported resolution
	this->comboBox_format->SelectedIndex = 0;

	m_UVA_image_max_width = 0;
	m_bDisableNotify = false;
	m_bInIdleCallback = false;

	m_pDocument = new tasDocument();
	m_pImageList = gcnew tmaImageList();
}

//----------------------------------------------------------------------------
//	OnApplicationIdle - when there are no messages pending, keep doing redraws
//----------------------------------------------------------------------------
System::Void Form1::OnApplicationIdle(System::Object ^  sender, System::EventArgs ^  e)
{
	if (m_bInIdleCallback) 
		return;

	m_bInIdleCallback = true;

	MSG msg;
	while (!::PeekMessage(&msg, NULL, 0,0,0))
	{
		if (!tasApp::IsActive())
		{
			m_bInIdleCallback = false;
			return;
		}
		if (docSingleDocumentMgr::IsLoading() ||
			docSingleDocumentMgr::IsSaving())
		{
			m_bInIdleCallback = false;
			return;
		}

		//// update timeline
		//m_bUpdatingTimeline = true;

		//int TLvalue = (int)(c_TicksPerSecond * tmlnTimeLine::GetValue());

		//if ( TLvalue > trackBar_TimeLine->get_Maximum() )
		//{
		//	//DBG_ASSERT3( 0, "about to set the trackbar timeline too high (%d > %d) [%10.4f]", TLvalue, trackBar_TimeLine->get_Maximum(), tmlnTimeLine::GetValue() );

		//	DBG_WARNING3( "about to set the trackbar timeline too high (%d > %d) [%10.4f]", TLvalue, trackBar_TimeLine->get_Maximum(), tmlnTimeLine::GetValue() );

		//	//TLvalue = (int)(c_TicksPerSecond * trackBar_TimeLine->get_Maximum());

		//	// FIX: - if we got here then let's reset the timeline range to it is in sync with the tmlnTimeLine
		//	//	I've noticed that this happens on batch capturing because in between loads the timeline Form
		//	//	timeline doesn't get set to the new tmlnTimeLine maximum because this happens in a non-managed
		//	//	part of code.
		//	//
		//	updateTimelineRange();

		//	DBG_WARNING1( "  Reset timeline [%d]", trackBar_TimeLine->get_Maximum() );
		//}

		////	make sure it is a valid value and set the value
		////
		//if (TLvalue < trackBar_TimeLine->Minimum)
		//{
		//	trackBar_TimeLine->Value = trackBar_TimeLine->Minimum;
		//}
		//else if (TLvalue > trackBar_TimeLine->Maximum)
		//{
		//	trackBar_TimeLine->Value = trackBar_TimeLine->Maximum;
		//}
		//else
		//{
		//	trackBar_TimeLine->Value = TLvalue;
		//}

		////	
		//if ( !textBox_currtime->ContainsFocus )
		//{
		//	char text[64];
		//	sprintf(text, "%.2f", tmlnTimeLine::GetValue() );
		//	textBox_currtime->Text = new System::String(text);
		//}

		//m_bUpdatingTimeline = false;

		tasApp::ThinkApp();

		render_3dWindow();
	}
	
	m_bInIdleCallback = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::Form1_Load(System::Object ^  sender, System::EventArgs ^  e)
{
	//if( SplashScreen::SplashForm != 0 )
	//	SplashScreen::SplashForm::Owner = this;

	//SplashScreen::SetBackgroundImage(".\\Data\\Splash.png");
	//SplashScreen::ShowSplashScreen(); 
	//SplashScreen::SetStatus("Loading module 1");
	//System::Threading::Thread::Sleep(100);
	//SplashScreen::SetStatus("Loading module 2");
	//System::Threading::Thread::Sleep(100);

	dbgPackage::Init();

	//SplashScreen::CloseForm();

	set_state_UVA_buttons();
	set_state_TUV_buttons();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::Form1_Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e)
{
	dbgPackage::CleanUp();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::checkBox_TUV_looping_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if (m_bDisableNotify)
		return;

	set_TUVdata_from_controls();
	tasTUVMgr::DataChanged();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::checkBox_TUV_reversing_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if (m_bDisableNotify)
		return;

	set_TUVdata_from_controls();
	tasTUVMgr::DataChanged();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::numericUpDown_TUV_xdiv_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if (m_bDisableNotify)
		return;

	set_TUVdata_from_controls();
	tasTUVMgr::DataChanged();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::numericUpDown_TUV_ydiv_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if (m_bDisableNotify)
		return;

	set_TUVdata_from_controls();
	tasTUVMgr::DataChanged();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::rangedFloat_TUV_rate_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if (m_bDisableNotify)
		return;

	set_TUVdata_from_controls();
	tasTUVMgr::DataChanged();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::button_TUV_reset_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	tasTUVMgr::SetStartTime();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::button_UVA_add_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	// add to listBox_UVA_textures
	std::string filter = c_ImageFileFilter;
	fsLocator file_loc;
	fsLocator initial_dir;

	if (muiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
	{
		if ( file_loc.GetNumNames() > 0 )
		{
			//std::string name;
			//name = itStringUtil::GetStdString(file_loc.GetLastName());

			//	loop and try to load a series of images based on the one selected
			//
			int initnum, numdigits;
			itString fname, basename, ext;
			fname = file_loc.GetLastName();
			numdigits = itStringUtil::Breakup_Filename(fname, basename, ext, initnum);

			int number = initnum;

			while (true)
			{
				//	if digits then build the filename
				if (numdigits > 0)
				{
					file_loc.Pop();
					itStringUtil::Build_Filename(basename, ext, number, numdigits, fname);
					file_loc.Push(fname);
				}

				String^ pFullPath = tmaManagedStringUtils::LocatorToManagedString(file_loc);

				//	if the file isn't found then jump out
				if (!System::IO::File::Exists(pFullPath))
					break;

				System::Drawing::Image^ pImage = load_image(pFullPath);
				if (pImage == nullptr)
					break;

				if (m_UVA_image_max_width < pImage->Width)
				{
					m_UVA_image_max_width = pImage->Width;

					set_valid_resolutions();
				}

				m_pImageList->Add(pImage);

				this->listBox_UVA_textures->BeginUpdate();
				//int index = this->listBox_UVA_textures->Items->Add( new System::String(name.c_str()) );
				int index = this->listBox_UVA_textures->Items->Add( pFullPath );
				this->listBox_UVA_textures->EndUpdate();

				// set the new image as selected
				//int index = this->imageList1->Images->Count-1;
				this->listBox_UVA_textures->SelectedIndex = index;

				++number;

				//	if no digits, then jump out since this is a single file being loaded
				if (numdigits == 0)
				{
					break;
				}
			}
 		}
	}

	set_state_UVA_buttons();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::button_UVA_del_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	// del to listBox_UVA_textures
	if (listBox_UVA_textures->SelectedItem != nullptr)
	{
		int index = listBox_UVA_textures->SelectedIndex;
		this->listBox_UVA_textures->Items->RemoveAt( index );
		m_pImageList->RemoveAt(index);

		if (m_pImageList->Count() == 0)
		{
			m_UVA_image_max_width = 0;
		}
	}

	set_state_UVA_buttons();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::button_UVA_up_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	if (listBox_UVA_textures->SelectedItem != nullptr)
	{
		if (listBox_UVA_textures->SelectedIndex > 0)
		{
			swap_listbox_items(listBox_UVA_textures->SelectedIndex, listBox_UVA_textures->SelectedIndex-1);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::button_UVA_down_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	if (listBox_UVA_textures->SelectedItem != nullptr)
	{
		if (listBox_UVA_textures->SelectedIndex < (listBox_UVA_textures->Items->Count-1))
		{
			swap_listbox_items(listBox_UVA_textures->SelectedIndex, listBox_UVA_textures->SelectedIndex+1);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::listBox_UVA_textures_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	int index = this->listBox_UVA_textures->SelectedIndex;
	display_thumbnail(index);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::menuItem_exit_Click(System::Object ^  sender, System::EventArgs ^  e)
{
    // The user wants to exit the application. Close everything down.
	Application::Exit();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::menuItem_about_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	// the about box
	//
	//	the first two digits after the period is the internal "stage" the programmers are working
	//	on.  the number after it is for sequencing.
	//
	System::Reflection::Assembly^ pSRAss = System::Reflection::Assembly::GetExecutingAssembly();

	String^ fulltitle	= pSRAss->GetName()->ToString();
    int found = fulltitle->IndexOf(", ");
	String^ title = fulltitle->Substring(0, found);

	String^ msg;
	msg = msg->Concat(	"Texture Animation Studio version ", 
						this->GetExecutableVersion(), 
						"\n\nCopyright (c) 2006-7 Extra Large Technology\n", 
						"All rights reserved\n" );
	std::string stdMsg;
	std::string stdTitle;
	tmaManagedStringUtils::ManagedStringToStdString(title,stdTitle);
	tmaManagedStringUtils::ManagedStringToStdString(msg,stdMsg);

	muiMessageBox::Show( stdMsg.c_str(), stdTitle.c_str(), muiMessageBox::e_OKOnly );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::fileChooser_TUV_animtexture_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	load_animation_tab();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::checkedListBox_UVA_resolution_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	tmaManagedControlUtil::CheckedListBox_CheckOnlySelected(checkedListBox_UVA_resolution);

	int index = this->checkedListBox_UVA_resolution->SelectedIndex;
	if (index != -1)
	{
		System::Int32 resolution = Convert::ToInt32( this->checkedListBox_UVA_resolution->SelectedItem->ToString(), 10 );

		this->label_UVA_texture_size->Text = String::Format("Texture Size = {0:D} kilobytes",((resolution * resolution * 24)/8192) );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::button_UVA_saveUVA_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	this->set_UVAdata_from_controls();

	//	build the list of filenames
	tasUVAMgr::ClearUVAImages();

	fsLocator fullpath;
	int i;
	for (i = 0; i < this->listBox_UVA_textures->Items->Count; ++i)
	{
		String^ pFilename = this->listBox_UVA_textures->Items[i]->ToString();
		tmaManagedStringUtils::ManagedStringToLocator( pFilename, fullpath );

		tasUVAMgr::AddUVAImage(fullpath);
	}

	//	find the save name, either use the last one or the last added bitmap
	tasUVAData& data = tasUVADataUtil::Data();

	fsLocator new_loc(fullpath);
	itString filename;
	if (data.m_UVAFilename.GetNumNames() == 0)
	{
		filename = new_loc.GetLastName();

		//	strip off the extension and the number associated with this filename
		//	and only save the base.
		itString base, ext;
		int num;
		itStringUtil::Breakup_Filename(filename, base, ext, num);
		filename = base;
	}
	else
	{
		filename = data.m_UVAFilename.GetLastName();
	}
	new_loc.Pop();
	new_loc.Push(filename);

	//	dialog for save file
	if (muiFileDialogUtils::GetSaveFileName(c_UVAFileFilter, new_loc))
	{
		//	save the UVA
		try
		{
			tasUVAMgr::SaveUVA(new_loc);
		}
		catch( const fsReadOnlyX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

			std::string msg = "Cannot write to file " + filename;
			DBG_ERROR1("%s", msg.c_str() );
			muiMessageBox::Show(msg.c_str(), "Read only");
		}

		//	remove the visual image before saving
		//free_UVA_composite();
		if (this->pictureBox_UVA_composite->Image != nullptr)
		{
			delete this->pictureBox_UVA_composite->Image;
			this->pictureBox_UVA_composite->Image = nullptr;
		}

		//	save the image (the animated texture)
		itString fname = new_loc.GetLastName();
		fname.StripExtension();
		fname += itString(".");
		fname += itString(data.m_ImageFormat.c_str());
		new_loc.Pop();
		new_loc.Push(fname);

		try
		{
			tasUVAMgr::SaveImage(new_loc);
		}
		catch( const fsReadOnlyX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

			std::string msg = "Cannot write to file " + filename;
			DBG_ERROR1("%s", msg.c_str() );
			muiMessageBox::Show(msg.c_str(), "Read only");
		}

		display_UVA_composite();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::button_UVA_send_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	System::String^ filepath;
	tasUVAData& data = tasUVADataUtil::Data();
	filepath = tmaManagedStringUtils::LocatorToManagedString(data.m_UVAFilename);

	//	send the filename to the TUV tab and select that tab
	//
	this->fileChooser_TUV_animtexture->Fullpath = filepath;
	this->numericUpDown_TUV_xdiv->Value = data.m_ImagesInRow;
	this->numericUpDown_TUV_ydiv->Value = data.m_ImagesInCol;

	this->tabControl_tools->SelectedTab = this->tabPage_tuv;

	load_animation_tab();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::button_TUV_new_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	this->listBox_UVA_textures->Items->Clear();
	m_pImageList->RemoveAll();
	this->pictureBox_UVA_anim->Image = nullptr;
	this->pictureBox_UVA_composite->Image = nullptr;

	tasTUVMgr::SetToDefault();

	this->fileChooser_TUV_animtexture->Fullpath = "";

	set_TUVcontrols_from_data();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::button_TUV_open_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	fsLocator file_loc;
	fsLocator initial_dir;

	if (muiFileDialogUtils::GetOpenFileName(c_TUVFileFilter, initial_dir, file_loc))
	{
		if ( file_loc.GetNumNames() > 0 )
		{
			std::string name;
			name = itStringUtil::GetStdString(file_loc.GetLastName());

			m_pDocument->Load(file_loc);

			set_TUVcontrols_from_data();
 		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::button_TUV_save_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	set_TUVdata_from_controls();
	tasTUVMgr::DataChanged();

	//	figure out what name to use, the last or the uva
	//
	const fsLocator orig_name( m_pDocument->GetFilename() );
	fsLocator file_loc(orig_name);
	if ( file_loc.GetNumNames() == 0 )
	{
		tmaManagedStringUtils::ManagedStringToLocator(this->fileChooser_TUV_animtexture->Fullpath, file_loc);
		itString ofname = file_loc.GetLastName();
		ofname.StripExtension();
		file_loc.Pop();
		file_loc.Push(ofname);
	}

	//	save the file
	if (muiFileDialogUtils::GetSaveFileName(c_TUVFileFilter, file_loc))
	{
		//	if the names are different, replace them in the resource tracker
		//
		//if (orig_name != file_loc)
		{
			try
			{
				m_pDocument->Save( file_loc );
			}
			catch( const fsReadOnlyX& i_Ex )
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
				DBG_WARNING1("fsReadOnlyX: %s", filename.c_str());
				std::string msg = "File is read only: " + filename;
				muiMessageBox::Show(msg.c_str(), "Error");
			}
			catch( const fsDiskFullX& i_Ex )
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
				DBG_WARNING1("fsDiskFullX: %s", filename.c_str());

				std::string msg = "Out of disk space or the disk is corrupt.  Could not write " + filename;
				DBG_ERROR1("%s", msg.c_str() );
				muiMessageBox::Show(msg.c_str(), "Error");
				assert(false);
			}
			catch( ... )
			{
				DBG_WARNING0("General Error");
				muiMessageBox::Show("General exception error", "Error");
				throw;
			}

		}
	}

	//tasAppUtil::UpdateTitleBar( false );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void Form1::button_TUV_saveas_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	set_TUVdata_from_controls();

	//	figure out what name to use, the last or the uva
	//
	const fsLocator orig_name( m_pDocument->GetFilename() );
	fsLocator file_loc(orig_name);
	if ( file_loc.GetNumNames() == 0 )
	{
		tasUVAData& data = tasUVADataUtil::Data();
		file_loc = data.m_UVAFilename;
		itString ofname = file_loc.GetLastName();
		ofname.StripExtension();
		file_loc.Pop();
		file_loc.Push(ofname);
	}

	//	save the file
	//
	if (muiFileDialogUtils::GetSaveFileName(c_TUVFileFilter, file_loc))
	{
		//	if the names are different, replace them in the resource tracker
		//
		//if (orig_name != file_loc)
		{
			try
			{
				m_pDocument->Save( file_loc );
			}
			catch( const fsReadOnlyX& i_Ex )
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
				DBG_WARNING1("fsReadOnlyX: %s", filename.c_str());
				std::string msg = "File is read only: " + filename;
				muiMessageBox::Show(msg.c_str(), "Error");
			}
			catch( const fsDiskFullX& i_Ex )
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
				DBG_WARNING1("fsDiskFullX: %s", filename.c_str());

				std::string msg = "Out of disk space or the disk is corrupt.  Could not write " + filename;
				DBG_ERROR1("%s", msg.c_str() );
				muiMessageBox::Show(msg.c_str(), "Error");
				assert(false);
			}
			catch( ... )
			{
				DBG_WARNING0("General Error");
				muiMessageBox::Show("General exception error", "Error");
				throw;
			}

		}
	}

	//tasAppUtil::UpdateTitleBar( false );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
String^ Form1::GetExecutableVersion()
{
	System::Reflection::Assembly^ pSRAss = System::Reflection::Assembly::GetExecutingAssembly();
	return pSRAss->GetName()->Version->ToString();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Form1::display_UVA_composite()
{
	tasUVAData& data = tasUVADataUtil::Data();

	for (int i = 0; i < data.m_TexturePages.size(); ++i)
	{
		String^ imagepath;
		imagepath = tmaManagedStringUtils::LocatorToManagedString( data.m_TexturePages[i].m_Filename );

		this->pictureBox_UVA_composite->Image = load_image(imagepath);
		if (this->pictureBox_UVA_composite->Image == nullptr)
			break;
		this->pictureBox_UVA_composite->Invalidate();
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Form1::display_thumbnail(int i_Index)
{
	if (i_Index != -1)
	{
		this->pictureBox_UVA_anim->Image = m_pImageList->Get(i_Index);
		this->pictureBox_UVA_anim->Invalidate();
	}
	else
	{
		this->pictureBox_UVA_anim->Image = nullptr;
		this->pictureBox_UVA_anim->Invalidate();
	}

	//Graphics* theGraphics = Graphics::FromHwnd( this->Handle );
    //this->imageList1->Draw( theGraphics, Point(64,64), index );
	// Call Application.DoEvents to force a repaint of the form.
	//Application::DoEvents();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Form1::swap_listbox_items(int i_Index1, int i_Index2)
{
	//	switch both the listbox and the imagelist so they match
	//
	System::Object^pObject = this->listBox_UVA_textures->Items[i_Index2];
	this->listBox_UVA_textures->Items->RemoveAt(i_Index2);
	this->listBox_UVA_textures->Items->Insert(i_Index1, pObject);

	System::Drawing::Image^ pImage1 = m_pImageList->Get(i_Index1);
	System::Drawing::Image^ pImage2 = m_pImageList->Get(i_Index2);
	m_pImageList->Set(i_Index1, pImage2);
	m_pImageList->Set(i_Index2, pImage1);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Form1::set_UVAdata_from_controls()
{
	int count = this->listBox_UVA_textures->Items->Count;

	tasUVAData& data = tasUVADataUtil::Data();
	data.m_TexturePages.clear();
	data.m_TexturePages.resize(count);

	data.m_TexturePageWidth = Convert::ToInt32( this->checkedListBox_UVA_resolution->SelectedItem->ToString(), 10 );
	data.m_NumberOfFrames = count;
	data.m_NumberOfTexturePages = 1;
	tmaManagedStringUtils::ManagedStringToStdString( this->comboBox_format->Text, data.m_ImageFormat );

	fsLocator path;
	for (int i=0; i < count; ++i)
	{
		tmaManagedStringUtils::ManagedStringToLocator( this->listBox_UVA_textures->Items[i]->ToString(), path );

		tasUVAData& data = tasUVADataUtil::Data();
		data.m_TexturePages[i].m_Filename = path;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Form1::set_TUVcontrols_from_data()
{
	tasTUVData& data = tasTUVDataUtil::Data();

	m_bDisableNotify = true;

	checkBox_TUV_looping->Checked = data.m_bLooping;
	checkBox_TUV_reversing->Checked = data.m_bReversing;
	numericUpDown_TUV_xdiv->Value = data.m_WidthFrames;
	numericUpDown_TUV_ydiv->Value = data.m_HeightFrames;
	rangedFloat_TUV_rate->Value = data.m_Rate;

	if (data.m_Filenames.size() > 0)
	{
		fsLocator fullloc = data.m_TUVFilename;
		fullloc.Pop();
		fullloc.Push( data.m_Filenames[0].m_Filename.c_str() );
		std::string fullpath;
		fsFileUtil::LocatorToANSIFilename(fullloc, fullpath);

		fileChooser_TUV_animtexture->Fullpath = gcnew System::String(fullpath.c_str());
	}

	m_bDisableNotify = false;
	this->tabPage_tuv->Invalidate();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Form1::set_TUVdata_from_controls()
{
	tasTUVData& data	= tasTUVDataUtil::Data();
	//tasUVAData& uvadata = tasUVADataUtil::Data();

	data.m_bLooping = checkBox_TUV_looping->Checked;
	data.m_bReversing = checkBox_TUV_reversing->Checked;
	data.m_WidthFrames = System::Convert::ToInt32(numericUpDown_TUV_xdiv->Value);
	data.m_HeightFrames = System::Convert::ToInt32(numericUpDown_TUV_ydiv->Value);
	data.m_Rate = (float)rangedFloat_TUV_rate->Value;
	data.m_NumFrames = data.m_WidthFrames * data.m_HeightFrames;
	//data.m_NumFrames = uvadata.m_NumberOfFrames;

	std::string str;
	tmaManagedStringUtils::ManagedStringToStdString( fileChooser_TUV_animtexture->Fullpath, str );
	if (str.length() > 0)
	{
		if (data.m_Filenames.size() == 0)
			data.m_Filenames.resize(1);
		data.m_Filenames[0].m_Filename = str;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Form1::layout_images()
{
//	this->pictureBox_UVA_composite->Image = this->imageList1->Images->get_Item(0);
//
//	System::Drawing::Image* pImage = new System::Drawing::Image();
//	pImage->
	//GetBounds()
	//
	//
	// Loop through the images pixels to reset color.
	//for ( x = 0; x < image1->Width; x++ )
	//{
	//	for ( y = 0; y < image1->Height; y++ )
	//	{
	//		Color pixelColor = image1->GetPixel( x, y );
	//		Color newColor = Color::FromArgb( pixelColor.R, 0, 0 );
	//		image1->SetPixel( x, y, newColor );
	//	}
	//}

	this->pictureBox_UVA_composite->Invalidate();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
Image^ Form1::load_image(String^ i_pFilename)
{
	Image^ pImage;

	try
	{
		//	test!!
		fsLocator locator;
		tmaManagedStringUtils::ManagedStringToLocator(i_pFilename, locator);
		//g2dImage* pimg = g2dImageCreate::Load(locator);

		//	debug only
		std::string fnamestr;
		fsFileUtil::LocatorToANSIFilename(locator, fnamestr);
		DBG_LOG1("About to load image (%s)", fnamestr.c_str());

		// supported formats: BMP, GIF, JPEG, PNG, EXIF, TIFF
		pImage = System::Drawing::Image::FromFile(i_pFilename);

		int bpp = pImage->GetPixelFormatSize(pImage->PixelFormat);
		if (bpp < 24)
		{
			// TODO create our own exception for this case
			throw gcnew System::IO::FileLoadException;
		}
	}
	catch (System::IO::FileNotFoundException^)
	{
		String^ errormsg = String::Format("File {0} not found", i_pFilename);
		std::string stderrormsg;
		tmaManagedStringUtils::ManagedStringToStdString(errormsg, stderrormsg);
		muiMessageBox::Show(stderrormsg.c_str(), "File Not Found");
		return nullptr;
	}
	catch (System::IO::FileLoadException^)
	{
		String^ errormsg = String::Format("File {0} has an unsupported pixel format", i_pFilename);
		std::string stderrormsg;
		tmaManagedStringUtils::ManagedStringToStdString(errormsg, stderrormsg);
		muiMessageBox::Show(stderrormsg.c_str(), "Invalid File Format");
		return nullptr;
	}
	catch (System::OutOfMemoryException^)
	{
		// The file does not have a valid image format.
		//		-or-
		// GDI+ does not support the pixel format of the file.
		//
		String^ errormsg = String::Format("Unsupported image/pixel format for file {0}", i_pFilename);
		std::string stderrormsg;
		tmaManagedStringUtils::ManagedStringToStdString(errormsg, stderrormsg);
		muiMessageBox::Show(stderrormsg.c_str(), "Out of Memory");
		return nullptr;
	}
	catch (g2dUnknownImageFileTypeX&)
	{
		String^ errormsg = String::Format("Unsupported image/pixel format for file {0}", i_pFilename);
		std::string stderrormsg;
		tmaManagedStringUtils::ManagedStringToStdString(errormsg, stderrormsg);
		muiMessageBox::Show(stderrormsg.c_str(), "Unsupported Image Format");
		return nullptr;
	}
	
	return pImage;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Form1::set_valid_resolutions()
{
	// NOTE cannot do this with listboxes.
	//
	return;

	//	disable only those listbox items that are a high enough size
	//
	m_bDisableNotify = true;
	for (int i = 0; i < this->checkedListBox_UVA_resolution->Items->Count; ++i)
	{
		System::Int32 resolution = Convert::ToInt32( this->checkedListBox_UVA_resolution->SelectedItem->ToString(), 10 );
		if (resolution < m_UVA_image_max_width)
		{
			//this->checkedListBox_UVA_resolution->Items->get_Item(i)->Enabled = false;
		}
	}
	m_bDisableNotify = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Form1::set_state_UVA_buttons()
{
	if (this->listBox_UVA_textures->Items->Count > 0)
	{
		this->button_UVA_del->Enabled = true;
		this->button_UVA_saveUVA->Enabled = true;
		this->button_UVA_send->Enabled = true;
		this->button_UVA_up->Enabled = true;
		this->button_UVA_down->Enabled = true;
	}
	else
	{
		this->button_UVA_del->Enabled = false;
		this->button_UVA_saveUVA->Enabled = false;
		this->button_UVA_send->Enabled = false;
		this->button_UVA_up->Enabled = false;
		this->button_UVA_down->Enabled = false;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Form1::set_state_TUV_buttons()
{
	if (this->fileChooser_TUV_animtexture->Fullpath->Length >  0)
	{
		this->button_TUV_save->Enabled = true;
		this->button_TUV_saveas->Enabled = true;
		this->button_TUV_reset->Enabled = true;
	}
	else
	{
		this->button_TUV_save->Enabled = false;
		this->button_TUV_saveas->Enabled = false;
		this->button_TUV_reset->Enabled = false;
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Form1::load_animation_tab()
{
	tasTUVMgr::ClearTUVTextureFilenames();

	set_state_TUV_buttons();

	fsLocator fname;
	tmaManagedStringUtils::ManagedStringToLocator(this->fileChooser_TUV_animtexture->Fullpath, fname);
	if (fname.GetNumNames() == 0)
		return;

	//	debug only
	std::string fnamestr;
	fsFileUtil::LocatorToANSIFilename(fname, fnamestr);
	DBG_LOG1("About to load anim texture (%s)", fnamestr.c_str());

	//	Load + Add the texture
	//
	try
	{
		tasTUVMgr::AddTUVTextureFilename(fname);
	}
	catch (g2dUnknownImageFileTypeX)
	{
		muiMessageBox::Show("Image is not an acceptable format", "Invalid Image Format");

		//	invalid image type
		fileChooser_TUV_animtexture->Fullpath = "";
	}
	catch (fsFileDoesntExistX& i_Ex)
	{
		std::string fnamestr;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), fnamestr);
		DBG_ERROR1("Cannot find file (%s)", fnamestr.c_str());
	}

	//
	//	remove the visual image before saving
	if (this->pictureBox_TUV_tmap->Image != nullptr)
	{
		delete this->pictureBox_TUV_tmap->Image;
		this->pictureBox_TUV_tmap->Image = nullptr;
	}

	String^ pStr = fileChooser_TUV_animtexture->Fullpath;
	if (fileChooser_TUV_animtexture->Fullpath->EndsWith("uva"))
		pStr = fileChooser_TUV_animtexture->Fullpath->Replace("uva","bmp");

	this->pictureBox_TUV_tmap->Image = load_image(pStr);
	this->pictureBox_TUV_tmap->Invalidate();
}
