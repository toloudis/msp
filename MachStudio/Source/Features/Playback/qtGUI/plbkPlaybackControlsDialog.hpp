/*****************************************************************************
**	plbkPlaybackControlsDialog.hpp
**
**		Floating pane with the playback controls, in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PLBK_PLAYBACKCONTROLDIALOG_HPP
#error plbkPlaybackControlDialog.hpp multiply included
#endif
#define PLBK_PLAYBACKCONTROLDIALOG_HPP

//	needed before App so USE_QT is set
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifndef PLBK_MODEPLAYBACK_HPP
#include "Features/Playback/plbkModePlayback.hpp"
#endif

#ifdef USE_QT

class QCheckBox;
class QSlider;

//============================================================================
// Class plbkPlaybackControlsDialog
//============================================================================
class plbkPlaybackControlsDialog : public QWidget
{
	public:
		static plbkPlaybackControlsDialog* DialogInstance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		plbkPlaybackControlsDialog( QWidget* parent,
									const char* title = "Playback Controls" );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~plbkPlaybackControlsDialog();

	private:
		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		void CheckBox_Loop_OnCheck();
		void CheckBox_LowRes_OnCheck();
		void CheckBox_Mute_OnCheck();
		void Slider_SloMo_ScrollChanged();

		plbkModePlayback* get_playbackmode();
		void set_playbackmode();
		void close_playbackmode();

		//	return true if the current mode is playback
		bool is_playbackmode();

		//	set the slow motion from the slider
		void set_slomo(plbkModePlayback* i_pMode);

		QCheckBox* m_checkBox_loop;
		QCheckBox* m_checkBox_lowres;
		QCheckBox* m_checkBox_mute;
		QSlider* m_slider_SloMo;
};

#endif
