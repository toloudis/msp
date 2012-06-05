/****************************************************************************\
**	chnlTest.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "chnlTraxFrame.hpp"

#include "Features/Channels/wxGUI/chnlTimeLabel.hpp"
#include "Features/Channels/wxGUI/chnlTimeSlider.hpp"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    //MyFrame *frame = new MyFrame(_T("Test of channel controls under wxWidgets"));
    chnlTraxFrame *pTraxEditor = new chnlTraxFrame(NULL);

	pTraxEditor->AddCategory("Object Name");

	// Create channels and add some clips to the channels:
	chnlChannelControl *pCtrl1 = pTraxEditor->AddChannel("Animation");
	pCtrl1->AddClip( new chnlChannelClip("Clip1", 2.0f, 3.0f) );

	chnlChannelControl *pCtrl2 = pTraxEditor->AddChannel("Motion");
	pCtrl2->AddClip( new chnlChannelClip("Idle", 2.0f, 3.0f, chnlChannelClip::e_SmoothBlend, 1.0f, true) );
	pCtrl2->AddClip( new chnlChannelClip("Walk", 0.3f, 1.3f) );

	pTraxEditor->AddCategory("Keys");

	chnlChannelControl *pCtrl3 = pTraxEditor->AddChannel("Keys");
	chnlChannelClip *clip_shorty = new chnlChannelClip("Shorty", 1.2f, 1.3f);
	pCtrl3->AddClip( clip_shorty );
	pCtrl3->AddClip( new chnlChannelClip("Key", 2.20f, 2.20f) );
	pCtrl3->SelectClip( clip_shorty );

	pTraxEditor->SetTotalTime(10);
	pTraxEditor->SetTimeScale(200);
	
	// Add some test time ticks
	pTraxEditor->AddTimeTick(0.3f);
	pTraxEditor->AddTimeTick(1.2f);
	pTraxEditor->AddTimeTick(1.3f);
	pTraxEditor->AddTimeTick(2.0f);
	pTraxEditor->AddTimeTick(2.2f);
	pTraxEditor->AddTimeTick(3.0f);
	
	// Add some test markers
	pTraxEditor->AddMarker(1.1f, chnlMarkerIcon::e_In, "Begin");
	pTraxEditor->AddMarker(3.3f, chnlMarkerIcon::e_Out, "End");
	pTraxEditor->AddMarker(2.2f, chnlMarkerIcon::e_Normal, "Normal marker in the middle");

	// Add some test notes
	pTraxEditor->AddNote(0.5f, chnlNoteIcon::e_Open, "Open this");
	pTraxEditor->AddNote(1.5f, chnlNoteIcon::e_Closed, "Close this");
	pTraxEditor->AddNote(0.4f, chnlNoteIcon::e_Pending, "Pend this");
	
	// and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    pTraxEditor->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}
