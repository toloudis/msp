/*****************************************************************************
**	cptrRenderProgressDialog.hpp
**
**		ementation for the dialog
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERPROGRESSDIALOG_HPP
#error cptrRenderProgressDialog.hpp multiply included
#endif
#define CPTR_RENDERPROGRESSDIALOG_HPP

//	needed before App so USE_WXWIDGETS is set
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

//	App
#ifdef USE_WXWIDGETS
#include "Features/Capture/wxGUI/cptrRenderProgressDialogBase.h"
#endif

//	Library
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


#ifdef USE_WXWIDGETS

//============================================================================
// Class RenderProgressDialog
//============================================================================
class cptrRenderProgressDialog : public cptrRenderProgressDialogBase
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static cptrRenderProgressDialog* DialogInstance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrRenderProgressDialog( wxWindow* parent, wxWindowID id = wxID_ANY, 
										const wxString& title = wxT("Render Progress"), 
										const wxPoint& pos = wxPoint(10, 10), 
										const wxSize& size = wxSize( 477,130 ), 
										long style = wxDEFAULT_DIALOG_STYLE );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~cptrRenderProgressDialog();

		//----------------------------------------------------------------------------
		//	set the percentage for scenes
		//----------------------------------------------------------------------------
		void SetScenesRenderPercentage( float i_percentage, const itString& i_label );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void AbortProgress();

		//--------------------------------------------------------------------
		//	set the percentage for scene
		//--------------------------------------------------------------------
		void SetSceneLabel( const itString& i_label );

		//--------------------------------------------------------------------
		//	set the label for the current render layer
		//--------------------------------------------------------------------
		void SetRenderLayerLabel( const itString& i_label );

		//--------------------------------------------------------------------
		//	set the percentage for cameras
		//--------------------------------------------------------------------
		void SetCamerasRenderPercentage( float i_percentage, const itString& i_label );

		//--------------------------------------------------------------------
		//	set the percentage for camera
		//--------------------------------------------------------------------
		void SetCameraRenderPercentage( float i_percentage, const itString& i_label );

		//--------------------------------------------------------------------
		//	set the time that has elapsed
		//--------------------------------------------------------------------
		void SetTimeElapsed( float i_fTimeElapsed );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void EnablePauseButton();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void DisablePauseButton();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Reset();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetRenderBatchID( int i_ID );

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void OnActivate( wxActivateEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void OnClose( wxCloseEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Button_Pause_OnButtonClick( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Button_Save_OnButtonClick( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Button_Abort_OnButtonClick( wxCommandEvent& event );

	private:
		//--------------------------------------------------------------------
		// way to combine pause + toggle_pause?
		//--------------------------------------------------------------------
		void pause_rendering(bool i_bPause);

		//--------------------------------------------------------------------
		//
		//--------------------------------------------------------------------
		void toggle_pause_rendering();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		std::string format_time_left(float i_fTimeLeft);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		float calculate_time_left(float i_fTimeElapsed);

	private:
		int m_RenderBatchID;
		float m_fLastScenesPercentage;
};

#endif