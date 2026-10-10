/*****************************************************************************
**	cptrRenderProgressDialog.hpp
**
**		Progress of a render, in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERPROGRESSDIALOG_HPP
#error cptrRenderProgressDialog.hpp multiply included
#endif
#define CPTR_RENDERPROGRESSDIALOG_HPP

//	needed before App so USE_QT is set
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

//	Library
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

#ifdef USE_QT

class QLabel;
class QProgressBar;
class QPushButton;

//============================================================================
// Class cptrRenderProgressDialog
//============================================================================
class cptrRenderProgressDialog : public QDialog
{
	public:
		static cptrRenderProgressDialog* DialogInstance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrRenderProgressDialog( QWidget* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~cptrRenderProgressDialog();

		//	set the percentage for scenes
		void SetScenesRenderPercentage( float i_percentage, const itString& i_label );

		void AbortProgress();

		//	set the percentage for scene
		void SetSceneLabel( const itString& i_label );

		//	set the label for the current render layer
		void SetRenderLayerLabel( const itString& i_label );

		//	set the percentage for cameras
		void SetCamerasRenderPercentage( float i_percentage, const itString& i_label );

		//	set the percentage for camera
		void SetCameraRenderPercentage( float i_percentage, const itString& i_label );

		//	set the time that has elapsed
		void SetTimeElapsed( float i_fTimeElapsed );

		void EnablePauseButton();
		void DisablePauseButton();

		void Reset();

		void SetRenderBatchID( int i_ID );

	protected:
		//--------------------------------------------------------------------
		// Closing the dialog (or pressing Escape) aborts the render
		//--------------------------------------------------------------------
		void closeEvent( QCloseEvent* i_pEvent ) override;
		void reject() override;
		bool event( QEvent* i_pEvent ) override;

	private:
		void Button_Save_OnButtonClick();

		// way to combine pause + toggle_pause?
		void pause_rendering(bool i_bPause);
		void toggle_pause_rendering();

		std::string format_time_left(float i_fTimeLeft);
		float calculate_time_left(float i_fTimeElapsed);

		QLabel* m_staticText_SceneNumber;
		QProgressBar* m_gauge_Scenes;
		QLabel* m_staticText_SceneName;
		QLabel* m_staticText_CameraNumber;
		QLabel* m_staticText_LayerName;
		QProgressBar* m_gauge_Cameras;
		QLabel* m_staticText_CameraName;
		QProgressBar* m_gauge_Camera;
		QLabel* m_staticText_EstimatedTimeLeft;
		QPushButton* m_button_Pause;
		QPushButton* m_button_Save;
		QPushButton* m_button_Abort;

		int m_RenderBatchID;
		float m_fLastScenesPercentage;
};

#endif
