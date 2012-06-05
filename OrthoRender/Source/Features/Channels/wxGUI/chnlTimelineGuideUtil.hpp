/****************************************************************************\
**	chnlTimelineGuideUtil.hpp
**
**		This namespace cqn be used to draw the guidelines over the
**	scrolled window and the channels.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TIMELINEGUIDEUTIL_HPP
#error chnlTimelineGuideUtil.hpp multiply included
#endif
#define CHNL_TIMELINEGUIDEUTIL_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif


#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
namespace chnlTimelineGuideUtil
{
	//--------------------------------------------------------------------
	// Move timeline guide for current time.
	//--------------------------------------------------------------------
	void SetCurrentTime(float i_Time);

	//--------------------------------------------------------------------
	// SetMinTime - set minimum time in timeline, used to offset 
	//	the guidelines
	//--------------------------------------------------------------------
	void SetMinTime(float i_MinTime);

	//--------------------------------------------------------------------
	// SetTimeScale
	//--------------------------------------------------------------------
	void SetTimeScale(float i_Scale);

	//--------------------------------------------------------------------
	// Marker guide gives feedback on alignment of clips while interacting
	//--------------------------------------------------------------------
	void ShowMarkerGuide(float i_Time, wxColour i_Color);
	void HideMarkerGuide();

	//--------------------------------------------------------------------
	// Draw the timeline guides on the given paint dc
	//--------------------------------------------------------------------
	void PaintGuides(wxDC &i_DC, 
					 int i_Height);
}

#endif // USE_WXWIDGETS
