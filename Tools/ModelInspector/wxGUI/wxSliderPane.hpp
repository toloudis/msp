/*****************************************************************************
**  wxSliderPane.hpp
**
**     Window using wxWidgets for doing Terawatt rendering
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef WX_SLIDERPANE_HPP
#error wxSliderPane.hpp multiply included
#endif
#define WX_SLIDERPANE_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#ifdef USE_WXWIDGETS

class prtyProperty;
class prtyPropertyCallback;
class twcRangedFloat;

//============================================================================
// Panel for timeline slider
//============================================================================
class wxSliderPane : public wxPanel
{
public:
    // ctor(s)
    wxSliderPane(wxWindow* parent);
	~wxSliderPane();

private:
	// slider callback
	void Slider_ValueChanged(wxCommandEvent& i_Event);

	// property callbacks
	void CurrentFrameChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void PlaybackRangeChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void PauseAnimationChanged(prtyProperty *i_pProperty, bool i_bDirty);

	twcRangedFloat* m_pSlider;
	shared_ptr<prtyPropertyCallback> m_pCurrentFrameCallback;
	shared_ptr<prtyPropertyCallback> m_pPlaybackBeginCallback;
	shared_ptr<prtyPropertyCallback> m_pPlaybackEndCallback;
	shared_ptr<prtyPropertyCallback> m_pPauseAnimationCallback;

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
