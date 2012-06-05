#include "stdafx.h"
#include "MainApp/MainForm.h"
#include "MainApp/MRUManager.hpp"

//#include "Features/Channels/mGUI/chnlDriverSelectForm.h"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Systems/Common/TimeData/cmmTimeDocumentChunk.hpp"
#include "Features/Capture/cptrRenderStatsDataUtil.hpp"
#include "Features/FilmGates/fgtFrameMgr.hpp"
#include "Features/Import/ImportUtil.hpp"
#include "Features/Keyframing/keyfKeyframeUtil.hpp"
#include "Features/MayaExport/MayaExportUtil.hpp"
#include "Features/ObjectManip/mnpConstants.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/ProjectSetup/Data/ProjectSetupData.hpp"
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Features/RenderPanels/rpnRenderPane.hpp"
#include "Features/RenderPanels/rpnPanelLayout.hpp"
#include "Features/SceneSetup/GUI/SceneSetupDialogUtil.hpp"
#include "Features/UndoHistory/UndoHistoryUtil.hpp"

//	non-managed includes
#include "Features/Channels/chnlDialogUtil.hpp"
#include "MainApp/mnmApp.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmAutoSaveMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mode/modeConstants.hpp"
#include "Core/prty/prtyFlags.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeInterest.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

//	 Tool layer includes
#include "Tool/api3d/api3dSubdiv.hpp"
#include "Features/Channels/chnlPackage.hpp"
#include "Tool/cma/cmaPackage.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Systems/DirectorsCut/Cue/dcutCueDialogUtil.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiToolbarMgr.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "ToolUIManaged/mui/muiPackage.hpp"
#include "ToolUIManaged/mui/muiToolbarMgr.hpp"
#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"
#include "ToolUIManaged/tma/tmaMessaging.hpp"
#include "ToolUIManaged/tma/tmaStatusBarMgr.hpp"
#include "ToolUIManaged/tma/tmaToolBarMgr.hpp"
#include "Core/undo/undoUndoMgr.hpp"

//	library
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "AudioDS/sn/snSoundManager.hpp"

// for PeekMessage()
#include <windows.h>


#ifdef _MANAGED

//============================================================================
//============================================================================
using namespace System::Reflection;
using namespace StudioFramework;
using namespace TerawattManagedControls;


//============================================================================
//============================================================================
namespace
{
	const int c_TicksPerSecond = g3dConstants::c_fDefaultFrameRate;

	public ref class main_form
	{
	public: static MRUManager^ l_pMRUManager;				// MRU list manager
	};

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	class mainTimeInterest : public tmlnTimeInterest
	{
		virtual void TimeChanged(float i_Time)
		{
		}
		virtual void TimeRangeChanged(float i_MinTime, float i_MaxTime)
		{
			StudioFramework::MainForm::FormInstance->updateTimelineRange();
		}
		virtual void TimeFormatChanged(int i_TimeFormat)
		{
		}
	};

	mainTimeInterest*	l_pTimeInterest = NULL;


	//---------------------------------------------------------------------------
	//	if the prefs data has changed, have various components set the
	//	appropriate data.
	//---------------------------------------------------------------------------
	class mainPrefsDataInterest : public prefsDataInterest
	{
		virtual void PrefsDataUpdated(const PrefsData& i_Data)
		{
			//	undo
			float memory = (float)(i_Data.m_UndoMemory.GetValue());
			undoUndoMgr::SetLimits( memory, i_Data.m_UndoLevels.GetValue() );

			//	MRU
			if (main_form::l_pMRUManager != nullptr)
			{
				main_form::l_pMRUManager->SetMRULimit( i_Data.m_MRUHistory.GetValue() );
			}

			//	auto-save
			mnmAutoSaveMgr::DeInitialize();
			mnmAutoSaveMgr::Initialize( i_Data.m_bAutoSave.GetValue(), i_Data.m_AutoSaveFrequency.GetValue()*60, i_Data.m_AutoSaveBackups.GetValue() );
		}
	};

	prefsDataInterest*	l_pPrefsDataInterest = NULL;

	// callback to resize main window when toolbar visibility changes
	void resize_mainwindow_callback()
	{
		if (MainForm::FormInstance != nullptr)
			MainForm::FormInstance->resize_mainwindow();
	}
}


//
//	MainForm code
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
MainForm::MainForm( void )
:	m_bUpdatingTimeline(false),
	m_bInIdleCallback(false),
	m_bDisableNotify(false)
{
	FormInstance = this;

	InitializeComponent();

	//	initialize the managed stuff
	//
	tmaSystem::g_pMainForm = this;
	tmaSystem::g_nNumMenuOffset = 2;
	muiPackage::Initialize();
	cmaCommandMgr::SetMenuCheckedFunction(guiMenuMgr::MenuObjectsCheck);
	cmaCommandMgr::SetValidHotKeyFunction(guiMenuMgr::IsValidShortcut);

	//	Create some menu items so we control the order
	//
	//	TODO uncomment the menu items when they are completely command driven
	//
	//guiMenuMgr::AddMenuItem("File","");
	guiMenuMgr::AddMenuItem("Edit","");
	guiMenuMgr::AddMenuItem("View","");
	guiMenuMgr::AddMenuItem("Tools","");
	guiMenuMgr::AddMenuItem("Actions","");
	guiMenuMgr::AddMenuItem("Modes","");
	guiMenuMgr::AddMenuItem("Objects","");
	guiMenuMgr::AddMenuItem("Windows","");
	guiMenuMgr::AddMenuItem("Help","");

	// Create render panes and add them to the panel layout control
	rpnPanelLayout::CreateRenderPanes(this->panelLayoutRender);

	// Need to initialize this to four panel view in order
	// to correctly create the g2dWindows from the HWND
	this->panelLayoutRender->LayoutStyle = TerawattManagedControls::PanelLayout::Layouts::e_FourPanes;

	// Set-up the application idle event handler
	//
	System::Windows::Forms::Application::Idle += gcnew System::EventHandler(this, &MainForm::OnApplicationIdle);				

	//	Select a component on the main window
	this->panelLayoutRender->Select();

	//	update controls after InitializeComponent
	//
	//trackBar_TimeLine->set_Dock( DockStyle::Bottom );
	updateTimelineRange();

	if ( !textBox_currtime->ContainsFocus )
	{
		textBox_currtime->Text = System::String::Format("{0}", (tmlnTimeLine::GetValue()));
	}

	//	Now add statusbar panels
	int sbindex;
	sbindex = tmaStatusBarMgr::g_pMgr->AddPanel(100);	// Mode
	sbindex = tmaStatusBarMgr::g_pMgr->AddPanel(-1);	// Messages
	sbindex = tmaStatusBarMgr::g_pMgr->AddPanel(40);	// Key
	sbindex = tmaStatusBarMgr::g_pMgr->AddPanel(40);	// Mute
	sbindex = tmaStatusBarMgr::g_pMgr->AddPanel(40);	// Particles
	sbindex = tmaStatusBarMgr::g_pMgr->AddPanel(80);	// Camera
	sbindex = tmaStatusBarMgr::g_pMgr->AddPanel(60);	// Render Flags
	sbindex = tmaStatusBarMgr::g_pMgr->AddPanel(36);	// Subdiv

	//	create the toolstrips in code
	//
	System::Windows::Forms::ToolStrip^ toolBar_actions = tmaToolBarMgr::g_pMgr->AddToolBar( mnpConstants::mc_Toolbar_Actions_Name );
	toolBar_actions->Location = System::Drawing::Point(7, 24);
	toolBar_actions->Size = System::Drawing::Size(111, 25);
	this->Controls->Add(toolBar_actions);
	System::Windows::Forms::ToolStrip^ toolBar_modes = tmaToolBarMgr::g_pMgr->AddToolBar( modeConstants::mc_Toolbar_Modes_Name );
	toolBar_modes->Location = System::Drawing::Point(150, 24);
	toolBar_modes->Size = System::Drawing::Size(111, 25);
	this->Controls->Add(toolBar_modes);
	System::Windows::Forms::ToolStrip^ toolBar_materials = tmaToolBarMgr::g_pMgr->AddToolBar( "Materials" );
	toolBar_materials->Location = System::Drawing::Point(270, 24);
	toolBar_materials->Size = System::Drawing::Size(111, 25);
	toolBar_materials->Visible =  false;
	this->Controls->Add(toolBar_materials);
	System::Windows::Forms::ToolStrip^ toolBar_surfaces = tmaToolBarMgr::g_pMgr->AddToolBar( "Surfaces" );
	toolBar_surfaces->Location = System::Drawing::Point(420, 24);
	toolBar_surfaces->Size = System::Drawing::Size(111, 25);
	toolBar_surfaces->Visible =  false;
	this->Controls->Add(toolBar_surfaces);
	System::Windows::Forms::ToolStrip^ toolBar_splines = tmaToolBarMgr::g_pMgr->AddToolBar( "toolBar_Splines" );
	toolBar_splines->Location = System::Drawing::Point(540, 24);
	toolBar_splines->Size = System::Drawing::Size(111, 25);
	toolBar_splines->Visible =  false;
	this->Controls->Add(toolBar_splines);

	// Setup callback to resize window when toolbars change visibility:
	muiToolbarMgr::SetResizeMainWindowFunction(resize_mainwindow_callback);

	// command package
	cmaPackage::Init();

	//	system dialog used by all the systems.
	tmaDialogMemoryMgr::Initialize();
	tmaDialogTabbedMgr::Initialize();

	PrefsData prefsdata = PrefsMgr::Data();

	// MRU File list
	main_form::l_pMRUManager = gcnew MRUManager(this->menuItem_file_recent, prefsdata.m_MRUHistory.GetValue() );

	//	Interests
	l_pTimeInterest = new mainTimeInterest();
	tmlnTimeLine::AddTimeInterest(l_pTimeInterest);

	l_pPrefsDataInterest = new mainPrefsDataInterest();
	PrefsMgr::UnRegisterInterest( l_pPrefsDataInterest );

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
MainForm::~MainForm()
{
	if (FormInstance == this) FormInstance = nullptr;

	// Remove interests
	PrefsMgr::UnRegisterInterest( l_pPrefsDataInterest );
	tmlnTimeLine::RemoveTimeInterest(l_pTimeInterest);

	//	system dialog used by all the systems.
	//chnlPackage::CleanUp();
	tmaDialogTabbedMgr::DeInitialize();
	tmaDialogMemoryMgr::DeInitialize();

	cmaPackage::CleanUp();

	//	deinitialize the managed stuff
	//
	//muiPackage::DeInitialize(); //bga - this is going to have to be somewhere later

	// This used to be in Dispose(), but can't be there anymore because of
	// new CLR syntax. But, if these are disposed of here, then we have
	// problems where the systems try to free up their dialogs which are already gone.
	//
	// Destroy components
	//if (components)
	//{
	//	delete components;
	//}
}

//----------------------------------------------------------------------------
//	Return pointer to one of the render panes within the layout control
//----------------------------------------------------------------------------
rpnRenderPane ^  MainForm::GetRenderPane(int i_Index)
{
	DBG_ASSERT1(i_Index>=0 && i_Index<4, "Render Pane index %d out of range, 4", i_Index);
	return rpnPanelLayout::g_RenderPanes[i_Index];
}

//----------------------------------------------------------------------------
//	
//----------------------------------------------------------------------------
void MainForm::Exit()
{
	do_app_closing_work();
}

//----------------------------------------------------------------------------
//	
//----------------------------------------------------------------------------
void MainForm::FileClosed()
{
	//bga - moved this to mainDocumentChunnk in order
	// to work in non-managed
	//SetMenuItemsBasedOnFileLoaded();
}

//----------------------------------------------------------------------------
//	
//----------------------------------------------------------------------------
void MainForm::FileOpened()
{
	//bga - moved some of these commands to mainDocumentChunnk in order
	// to work in non-managed

	//mnmAppUtil::UpdateTitleBar( false );
	updateTimelineRange();
	
	//mnmAutoSaveMgr::ResetAutoSaveTimer();

	//cmmSystemDialogUtil::UpdateDialog();

	//SetMenuItemsBasedOnFileLoaded();
}

//----------------------------------------------------------------------------
// update range of timeline
//----------------------------------------------------------------------------
void MainForm::updateTimelineRange()
{
	//	adjust the value if it is outside the new range before setting the range
	//
	if ( trackBar_TimeLine->Value > (int)(c_TicksPerSecond * tmlnTimeLine::GetMaximum()))
	{
		trackBar_TimeLine->Value = (int)(c_TicksPerSecond * tmlnTimeLine::GetMaximum());
	}
	trackBar_TimeLine->SetRange((int)(c_TicksPerSecond * tmlnTimeLine::GetMinimum()), 
						(int)(c_TicksPerSecond * tmlnTimeLine::GetMaximum()));
	textBox_MaxTime->Text = System::String::Format("{0}", (tmlnTimeLine::GetMaximum()));

	chnlDialogUtil::SetTotalTime( tmlnTimeLine::GetMaximum() );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::MainForm_Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e)
{
	//	if the user hit cancel on a save dialog, then abort closing
	if (!do_app_closing_work())
	{
		e->Cancel = true;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::MainForm_Load(System::Object ^  sender, System::EventArgs ^  e)
{
	// Dialog memory remembers size, location, visiblity of dialog 
	m_pMemory = gcnew tmaDialogMemory( this );

	prefs_ReadAndApply();

	CreateDialogsAndTabs();
}

//----------------------------------------------------------------------------
//	OnApplicationIdle - when there are no messages pending, keep doing redraws
//----------------------------------------------------------------------------
System::Void MainForm::OnApplicationIdle(System::Object ^  sender, System::EventArgs ^  e)
{
	if (m_bInIdleCallback) 
		return;

	m_bInIdleCallback = true;

	MSG msg;
	while (!::PeekMessage(&msg, NULL, 0,0,0))
	{
		if (!mnmApp::IsActive())
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

		// update timeline
		m_bUpdatingTimeline = true;

		int TLvalue = (int)(c_TicksPerSecond * tmlnTimeLine::GetValue());

		if ( TLvalue > trackBar_TimeLine->Maximum )
		{
			//DBG_ASSERT3( 0, "about to set the trackbar timeline too high (%d > %d) [%10.4f]", TLvalue, trackBar_TimeLine->Maximum, tmlnTimeLine::GetValue() );

			DBG_WARNING3( "about to set the trackbar timeline too high (%d > %d) [%10.4f]", TLvalue, trackBar_TimeLine->Maximum, tmlnTimeLine::GetValue() );

			//TLvalue = (int)(c_TicksPerSecond * trackBar_TimeLine->Maximum);

			// FIX: - if we got here then let's reset the timeline range to it is in sync with the tmlnTimeLine
			//	I've noticed that this happens on batch capturing because in between loads the timeline Form
			//	timeline doesn't get set to the new tmlnTimeLine maximum because this happens in a non-managed
			//	part of code.
			//
			updateTimelineRange();

			DBG_WARNING1( "  Reset timeline [%d]", trackBar_TimeLine->Maximum );
		}

		//	make sure it is a valid value and set the value
		//
		if (TLvalue < trackBar_TimeLine->Minimum)
		{
			trackBar_TimeLine->Value = trackBar_TimeLine->Minimum;
		}
		else if (TLvalue > trackBar_TimeLine->Maximum)
		{
			trackBar_TimeLine->Value = trackBar_TimeLine->Maximum;
		}
		else
		{
			trackBar_TimeLine->Value = TLvalue;
		}

		//	
		if ( !textBox_currtime->ContainsFocus )
		{
			char text[64];
			sprintf(text, "%.2f", tmlnTimeLine::GetValue() );
			textBox_currtime->Text = gcnew System::String(text);
		}

		m_bUpdatingTimeline = false;

		mnmApp::ThinkApp();
	}
	
	m_bInIdleCallback = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::panelRender_Resize(System::Object ^  sender, System::EventArgs ^  e)
{
	// The individual render panels update the important info when resized.
	// No need for code here anymore.
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::panelRender_MouseEnter(System::Object ^  sender, System::EventArgs ^  e)
{
	tma3dCursorMgr::CursorOverViewCallback(true);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::panelRender_MouseLeave(System::Object ^  sender, System::EventArgs ^  e)
{
	tma3dCursorMgr::CursorOverViewCallback(false);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::AboutDialog()
{
	// the about box
	//
	//	the first two digits after the period is the internal "stage" the programmers are working
	//	on.  the number after it is for sequencing.
	//
	System::Reflection::Assembly^ pSRAss = System::Reflection::Assembly::GetExecutingAssembly();

#ifdef EON_REALITY
	String^ title = gcnew System::String("About Composer");
#else
	String^ fulltitle	= pSRAss->GetName()->ToString();
    int found = fulltitle->IndexOf(", ");
	String^ title = fulltitle->Substring(0, found);
#endif

	String^ pstrCompany;
	String^ pstrCopyright;
	String^ pstrProduct;
	String^ pstrExeVersion = this->GetExecutableVersion();

	cli::array<Reflection::AssemblyProductAttribute^>^ titles = dynamic_cast<cli::array<Reflection::AssemblyProductAttribute^>^ >(pSRAss->GetCustomAttributes(Reflection::AssemblyProductAttribute::typeid, false));
	pstrProduct = ((titles!=nullptr) && (titles->Length>0)) ? titles[0]->Product : gcnew System::String(mnmConstants::c_PRODUCT);
	pstrProduct = String::Concat(pstrProduct, " ");
	cli::array<Reflection::AssemblyCopyrightAttribute^>^ copyrights = dynamic_cast<cli::array<Reflection::AssemblyCopyrightAttribute^>^ >(pSRAss->GetCustomAttributes(Reflection::AssemblyCopyrightAttribute::typeid, false));
	pstrCopyright = ((copyrights!=nullptr) && (copyrights->Length>0)) ? copyrights[0]->Copyright : gcnew System::String(mnmConstants::c_COPYRIGHT);
	cli::array<Reflection::AssemblyCompanyAttribute^>^ Companys = dynamic_cast<cli::array<Reflection::AssemblyCompanyAttribute^>^ >(pSRAss->GetCustomAttributes(Reflection::AssemblyCompanyAttribute::typeid, false));
	pstrCompany = ((Companys!=nullptr) && (Companys->Length>0)) ? Companys[0]->Company : gcnew System::String(mnmConstants::c_COMPANY);

	pstrCopyright = String::Concat("\n\n", pstrCopyright, " ", pstrCompany);
	pstrCopyright = String::Concat(pstrCopyright,"\n");

	String^ msg;
	msg = msg->Concat(	pstrProduct, 
						pstrExeVersion, 
						pstrCopyright, 
						"All rights reserved\n" );
	std::string stdMsg;
	std::string stdTitle;
	tmaManagedStringUtils::ManagedStringToStdString(title,stdTitle);
	tmaManagedStringUtils::ManagedStringToStdString(msg,stdMsg);

	guiMessageBox::Show( stdMsg.c_str(), stdTitle.c_str(), guiMessageBox::e_OKOnly );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::HelpDialog()
{
    // Create a new instance of the form.
    Form^ help_form = gcnew Form();

	//	configure the form
    help_form->Text = "Help";
	help_form->FormBorderStyle = ::FormBorderStyle::FixedDialog;
    help_form->MaximizeBox = false;
    help_form->MinimizeBox = false;
	help_form->StartPosition = ::FormStartPosition::CenterScreen;
	help_form->Width = 350;
	help_form->Height = 700;
    
	// create the text box + configure it
    RichTextBox ^textbox_help = gcnew RichTextBox();

    textbox_help->Multiline = true;
	textbox_help->Dock = DockStyle::Fill;
	textbox_help->BorderStyle = BorderStyle::None;
    textbox_help->WordWrap = true;
	textbox_help->ReadOnly = true;
	textbox_help->AccessibleRole = ::AccessibleRole::StaticText;

	//
	//	TODO implement help as an XML file or some other external file
	//

    // Set the default text of the control.
    textbox_help->Text = "   Status Bar Render Flags\n\n";
    textbox_help->AppendText( "H/L - Render Type\n" );
    textbox_help->AppendText( "S - Shadows\n" );
    textbox_help->AppendText( "M - Matte\n" );
    textbox_help->AppendText( "D - Depth of Field\n" );
    textbox_help->AppendText( "F - Fur\n" );
    textbox_help->AppendText( "G - Glow\n" );
    textbox_help->AppendText( "A - Ambient Pass\n" );
    textbox_help->AppendText( "L - Lit Pass\n" );
    textbox_help->AppendText( "T - Transparent\n" );
    textbox_help->AppendText( "R - Reflection\n" );
    textbox_help->AppendText( "\n" );
    textbox_help->AppendText( "Camera Movement\n\n" );
    textbox_help->AppendText( "Left, Right Arrows - Camera Yaw\n" );
    textbox_help->AppendText( "Up, Down Arrows - Camera Pitch\n" );
    textbox_help->AppendText( "CTRL + arrows - slows down arrow movements\n" );
    textbox_help->AppendText( "ALT + mouse button 0 - move camera position\n" );
    textbox_help->AppendText( "ALT + mouse button 1 - move camera pos+tar (strafe)\n" );
    textbox_help->AppendText( "ALT + mouse wheel - zoom in/out\n" );
    textbox_help->AppendText( "CTRL + mouse button 0 - move camera target\n" );
    textbox_help->AppendText( "CTRL + mouse button 1 - n\n" );
    textbox_help->AppendText( "CTRL + ALT + mouse button 0 - nothing\n" );
    textbox_help->AppendText( "CTRL + ALT + mouse button 1 - nothing\n" );
    textbox_help->AppendText( "CTRL + ALT + mouse button 0 + 1 - zoom in/out\n" );
    textbox_help->AppendText( "\n" );
    textbox_help->AppendText( "Selecting Objects\n" );
    textbox_help->AppendText( "\n" );
    textbox_help->AppendText( "mouse button 0 - select object\n" );
    textbox_help->AppendText( "LEFT SHIFT + mouse button 0 - accumulate select objects\n" );
    textbox_help->AppendText( "mouse button 1 - deselect object\n" );
    textbox_help->AppendText( "\n" );
    textbox_help->AppendText( " \n" );

	//	Add the custom hot keys
	//
	//	TODO - need to sort this list by category and then name.  The list returned
	//	is the active list and should NOT be sorted.
	//
	std::string lastcat = "";
	char helpline[128];
	const std::vector<cmaCommand*>& cmds = cmaCommandMgr::GetCommandList();
	int csize = cmds.size();
	for (int j = 0; j < csize; ++j)
	{
		if (cmds[j]->GetHotKeyString().length() > 0)
		{
			if (lastcat != cmds[j]->GetCategory())
			{
				sprintf(helpline, "\n%s\n", cmds[j]->GetCategory().c_str());
				textbox_help->AppendText( gcnew System::String(helpline) );
				lastcat = cmds[j]->GetCategory();
			}
			sprintf(helpline, "%16s - %s: %s\n", cmds[j]->GetHotKeyString().c_str(), cmds[j]->GetCategory().c_str(), cmds[j]->GetTag().c_str() );
			textbox_help->AppendText( gcnew System::String(helpline) );
		}
	}

	//prtyPropertyUIInfoContainer pcont;
	//cmaCommandMgr::GetCommandPropertyUIInfoList( pcont );
	//pcont.SortByCategory();
	//prtyPropertyUIIList plist = pcont.GetList();
	//std::string category_name("");
	//prtyProperty* pProperty;
	//
	//PropertyUIIList::const_iterator it, end = plist.end();
	//for (it = plist.begin(); it != end; ++it)
	//{
	//	prtyPropertyUIInfo* pUIInfo = (*it);
	//	pProperty = pUIInfo->GetProperty(0);
	//	if ( strcmp(category_name.c_str(), pUIInfo->GetCategory().c_str()) != 0 )
	//	{
	//		category_name = pUIInfo->GetCategory();
	//	}
	//}

	//
	help_form->Controls->Add( textbox_help );

    // Display the form as a modal dialog box.
    help_form->ShowDialog();
    delete help_form;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool MainForm::ProcessCmdKey(Message% msg, Keys keyData)
{
	bool bProcessed = tmaMessaging::ProcessCmdKey(msg, keyData);

	return (/*bProcessed ||*/ (__super::ProcessCmdKey(msg,keyData)));
}
 
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::trackBar_timeline_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if (!m_bUpdatingTimeline)
	{
		float cur_time = trackBar_TimeLine->Value / (float)c_TicksPerSecond;

		tmlnTimeLine::SetValue( cur_time );
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::trackBar_TimeLine_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
	tmlnTimeLine::SetIsScrubbing( true );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::trackBar_TimeLine_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
	tmlnTimeLine::SetIsScrubbing( false );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::textBox_MaxTime_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
{
	// Update timeline on "enter" keypress
	if (e->KeyCode == System::Windows::Forms::Keys::Return)
	{
		if (this->textBox_MaxTime->Text->Length > 0)
		{
			try
			{
				double max_time = System::Double::Parse(this->textBox_MaxTime->Text);
				if (max_time > tmlnTimeLine::GetMinimum())
				{
					tmlnTimeLine::SetMaximum(max_time);
					updateTimelineRange();

					cmmTimeDocumentChunk::GetActiveChunk()->DataChanged();
				}
			}
			catch (System::FormatException^) {}
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void MainForm::updateViewTimeline(bool bVisible)
{
	this->panelTime->Visible =  bVisible;
	resize_mainwindow();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void MainForm::updateViewActionToolbar(bool bVisible)
{
	guiToolbarMgr::Show(mnpConstants::mc_Toolbar_Actions_Name, bVisible);
	resize_mainwindow();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void MainForm::updateViewModeToolbar(bool bVisible)
{
	guiToolbarMgr::Show(modeConstants::mc_Toolbar_Modes_Name, bVisible);
	resize_mainwindow();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void MainForm::updateViewStatusBar(bool bVisible)
{
	this->statusStripMain->Visible =  bVisible;
	resize_mainwindow();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool MainForm::IsViewTimeline()
{
	return this->panelTime->Visible;
}
bool MainForm::IsViewActionToolbar()
{
	return guiToolbarMgr::IsVisible(mnpConstants::mc_Toolbar_Actions_Name);
}
bool MainForm::IsViewModeToolbar()
{
	return guiToolbarMgr::IsVisible(modeConstants::mc_Toolbar_Modes_Name);

}
bool MainForm::IsViewStatusBar()
{
	return this->statusStripMain->Visible;
}

//----------------------------------------------------------------------------
// Resolution changes
//----------------------------------------------------------------------------
void MainForm::ResizeRenderWindow(int i_Width, int i_Height)
{
	// Resize window so that render panel is correct size
	int dw = i_Width - this->panelLayoutRender->Width;
	int dh = i_Height - this->panelLayoutRender->Height;
	System::Drawing::Size new_size(this->Width + dw, this->Height + dh);
	this->Size = new_size;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
String^ MainForm::GetExecutableVersion()
{
	System::Reflection::Assembly^ pSRAss = System::Reflection::Assembly::GetExecutingAssembly();
	return pSRAss->GetName()->Version->ToString();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::textBox_currtime_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
{
	// Update timeline on "enter" keypress
	if (e->KeyCode == System::Windows::Forms::Keys::Return)
	{
		Decimal dvalue;
		try
		{
			dvalue = Convert::ToDecimal(  textBox_currtime->Text );
		}
		catch( Exception^ /*ex*/ )
		{
			dvalue = Decimal(0.0f);
		}

		float value = Decimal::ToSingle(dvalue);
		if (   (value >= tmlnTimeLine::GetMinimum())
			&& (value <= tmlnTimeLine::GetMaximum()) )
		{
			tmlnTimeLine::SetValue( value );
		}
		else
		{
			char text[64];
			sprintf(text, "%.2f", tmlnTimeLine::GetValue() );
			textBox_currtime->Text = gcnew System::String(text);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::MainForm_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
{
	//	Check for universal hot keys
	//
	if (e->Control)
	{
		if (e->KeyCode == Keys::Z)
		{
			undoUndoMgr::Undo();
			e->Handled = true;
		}
		else if (e->KeyCode == Keys::Y)
		{
			undoUndoMgr::Redo();
			e->Handled = true;
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MainForm::MainForm_Activated(System::Object ^  sender, System::EventArgs ^  e)
{
	//	This function gets called each time the main form gets activated (gains focus).
	//
	// TODO consume any mouse button press so when clicking on the main window a selected
	//	object doesn't get unselected at first.
}

//----------------------------------------------------------------------------
//	read the prefs and apply them where they need to go
//----------------------------------------------------------------------------
void MainForm::prefs_ReadAndApply()
{
	//	get subdiv level + display
	int level = api3dSubdiv::GetSubdivLevel();
	char levelstr[8];
	sprintf(levelstr, "DIV%1d", level);
	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Subdiv, levelstr );

	//	display the render prefs data in the status bar
	std::string rpstring = rndrPrefsMgr::GetPrefsString(rndrPrefsMgr::e_ViewportPrefs);
	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Render_Flags, rpstring.c_str() );

	//	Preferences
	PrefsMgr::ReadPrefs();
	PrefsData prefsdata = PrefsMgr::Data();

	//	Hot Keys
	PrefsMgr::ReadHotKeys();

	//
	//	take the data and apply it where it is supposed to go.
	//

	//	render window size
	ResizeRenderWindow( prefsdata.m_RenderWindowWidth.GetValue(), prefsdata.m_RenderWindowHeight.GetValue() );

	//	Undo limits
	undoUndoMgr::SetLimits( prefsdata.m_UndoMemory.GetValue(), prefsdata.m_UndoLevels.GetValue() );

	//	Audio
	snSoundManager::Mute( prefsdata.m_bMuteAudio.GetValue() );
	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mute, prefsdata.m_bMuteAudio.GetValue()?"Mute":"" );

	//	visuals
	updateViewActionToolbar( prefsdata.m_bMainActionToolbarVisible.GetValue() );
	updateViewModeToolbar( prefsdata.m_bMainModeToolbarVisible.GetValue() );
	this->panelTime->Visible =  ( prefsdata.m_bMainTimelineVisible.GetValue() );
	this->statusStripMain->Visible =  ( prefsdata.m_bMainStatusBarVisible.GetValue() );
	resize_mainwindow();

	// axis compass
	cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_World, prefsdata.m_bAxisCompassVisible.GetValue() );

	//	property
	prtyFlags::SetShowToolTips( prefsdata.m_bShowToolTips.GetValue() );

	//	rewrite the prefs (why? [rjk])
	//PrefsMgr::WritePrefs();
}

//----------------------------------------------------------------------------
//	write the prefs on closing the app
//----------------------------------------------------------------------------
void MainForm::prefs_UpdateAndWrite()
{
	PrefsData& prefsdata = PrefsMgr::Data();

	//	render window size
	int wnd_width = this->panelLayoutRender->Width;
	int wnd_height = this->panelLayoutRender->Height;
	if ((wnd_width != 0) && (wnd_height != 0))
	{
		prefsdata.m_RenderWindowHeight	= wnd_height;
		prefsdata.m_RenderWindowWidth	= wnd_width;
	}

	//	visuals
	prefsdata.m_bMainActionToolbarVisible.SetValue( IsViewActionToolbar() );
	prefsdata.m_bMainModeToolbarVisible.SetValue( IsViewModeToolbar() );
	prefsdata.m_bMainTimelineVisible.SetValue( this->panelTime->Visible );
	prefsdata.m_bMainStatusBarVisible.SetValue( this->statusStripMain->Visible );

	//	axis compass
	prefsdata.m_bAxisCompassVisible.SetValue( cmpsCompassMgr::GetRenderable(cmpsCompassMgr::e_World) );

	//	property
	prefsdata.m_bShowToolTips.SetValue( prtyFlags::IsShowToolTips() );

	//
	PrefsMgr::WritePrefs();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void MainForm::CreateDialogsAndTabs()
{
	// Create other dialogs, the dialog memory will show them in correct location
	tmaDialogTabbedMgr::Create("System", "System Properties", true, false);
	//tmaDialogTabbedMgr::Create("Driver", "Driver Properties", true, false); // moved to FeaturesLayer::AddToMenu()
	tmaDialogTabbedMgr::Create("Object", "Object Properties", true, true);
	tmaDialogTabbedMgr::AddTabPage("Object","Properties");

	// FIX [rjk] - get other dialogs to handle keys (like undo + redo)
	//
	//tmaDialogTabbed^ pDialogTabbed = tmaDialogTabbedMgr::GetDialogFromName("Object");
	//System::Windows::Forms::Form^ pDialog;
	//pDialog = pDialogTabbed->GetDialog();
	//if (pDialog != 0)
	//{
	//	pDialog->set_KeyPreview(true);
	//	pDialog->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(pDialog, MainForm_KeyDown);
	//}
}

void MainForm::resize_mainwindow()
{
	const int c_BORDEREDGE = 7;
	const int c_CONTROLEDGE = 6;

	m_bDisableNotify = true;

	int mainx = 0;
	int mainy = c_BORDEREDGE;

	//	Main Menu
	//
	if (this->MainMenuStrip->Visible)
	{
		System::Drawing::Point pnt = this->MainMenuStrip->Location;
		System::Drawing::Size size = this->MainMenuStrip->ClientSize;

		mainy = MainMenuStrip->ClientRectangle.Bottom;
		if (size.Width > mainx)
			mainx = size.Width;
		//DBG_LOG2("TOOLBAR: main height %d  width %d", mainy, mainx);
	}

	//	Toolbars
	//
	if (this->IsViewActionToolbar() || this->IsViewModeToolbar())
	{
		System::Windows::Forms::ToolStrip^ thetoolstrip = tmaToolBarMgr::g_pMgr->get_toolstrip(mnpConstants::mc_Toolbar_Actions_Name);
		System::Drawing::Point pnt = thetoolstrip->Location;
		System::Drawing::Size size = thetoolstrip->ClientSize;
		//DBG_LOG2("TOOLBAR: pre height %d  width %d", pnt.Y, pnt.X);

		mainy += thetoolstrip->ClientRectangle.Bottom; // REMOVE EXTRA SPACE + c_CONTROLEDGE;
		if (size.Width > mainx)
			mainx = size.Width;
		//DBG_LOG2("TOOLBAR: main height %d  width %d", mainy, mainx);
	}

	System::Drawing::Point renderpoint = panelLayoutRender->Location;
	//DBG_LOG2("RENDER: pre-loc(%d,%d)", renderpoint.X, renderpoint.Y);
	System::Drawing::Size rendersize = panelLayoutRender->Size; //ClientSize
	//DBG_LOG2("RENDER: pre size %dx%d", rendersize.Width, rendersize.Height);

	//	Render Panel
	//
	if (this->panelLayoutRender->Visible)
	{
		//	adjust the location of the panel render based on the 
		renderpoint.Y = mainy;

		System::Drawing::Rectangle rect = panelLayoutRender->ClientRectangle;
		mainy = renderpoint.Y + rect.Bottom + c_CONTROLEDGE;
		int tempx = renderpoint.X + rect.Right + c_BORDEREDGE;
		if (tempx > mainx)
			mainx = tempx;
		//DBG_LOG2("RENDER: main height %d  width %d", mainy, mainx);
	}

	System::Drawing::Point timepnt = this->panelTime->Location;
	//System::Drawing::Size timesize = this->panelTime->ClientSize;
	//DBG_LOG2("TIME: pre-loc(%d,%d)", timepnt.X, timepnt.Y);

	if (this->panelTime->Visible)
	{
		timepnt.Y = mainy;

		System::Drawing::Rectangle rect = panelTime->ClientRectangle;

		mainy = timepnt.Y + rect.Bottom + c_CONTROLEDGE;
		int tempx = timepnt.X + rect.Right + c_BORDEREDGE;
		if ( tempx > mainx)
			mainx = tempx;
		//DBG_LOG2("TIME: main height %d  width %d", mainy, mainx);
	}

	System::Drawing::Point statuspoint = this->statusStripMain->Location;
	System::Drawing::Size statussize = this->statusStripMain->ClientSize;

	if (statusStripMain->Visible)
	{
		//DBG_LOG2("STATUSBAR: pre-loc(%d,%d)", statuspoint.X, statuspoint.Y);
		mainy += statussize.Height;
		int tempx = statussize.Width;
		if (tempx > mainx)
			mainx = tempx;
		//DBG_LOG2("STATUSBAR: main height %d  width %d", mainy, mainx);
	}

	System::Drawing::Size msize = this->ClientSize;

	//DBG_LOG2("FINAL SIZE: main height %d  width %d", mainy, mainx);

	msize.Width = mainx;
	msize.Height = mainy;
	this->ClientSize = msize;
	panelTime->Location = timepnt;
	//DBG_LOG2("TIME: post-loc(%d,%d)", timepnt.X, timepnt.Y);
	panelLayoutRender->Location = renderpoint;
	//DBG_LOG2("RENDER: post-loc(%d,%d)", renderpoint.X, renderpoint.Y);
	this->panelLayoutRender->Size = rendersize;

	m_bDisableNotify = false;
}

//----------------------------------------------------------------------------
//	return true if the app *IS* closing, false if it should be aborted.
//----------------------------------------------------------------------------
bool MainForm::do_app_closing_work()
{
	if (guiSingleDocHandler::SaveIfDirty())
	{
		prefs_UpdateAndWrite();
		
		mnmApp::EnableRender(false);
		Application::Exit();
		return true;
	}
	return false;
}


#endif // _MANAGED
