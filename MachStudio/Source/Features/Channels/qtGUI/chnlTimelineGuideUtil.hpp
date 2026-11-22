/****************************************************************************\
**	chnlTimelineGuideUtil.hpp
**
**		This namespace cqn be used to draw the guidelines over the
**	scrolled window and the channels.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TIMELINEGUIDEUTIL_HPP
#error chnlTimelineGuideUtil.hpp multiply included
#endif
#define CHNL_TIMELINEGUIDEUTIL_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

class maTime;

#ifdef USE_QT

//============================================================================
//============================================================================
namespace chnlTimelineGuideUtil
{
	//--------------------------------------------------------------------
	// Move timeline guide for current time.
	//--------------------------------------------------------------------
	void SetCurrentTime(const maTime& i_Time);

	//--------------------------------------------------------------------
	// SetMinTime - set minimum time in timeline, used to offset 
	//	the guidelines
	//--------------------------------------------------------------------
	void SetMinTime(const maTime& i_MinTime);

	//--------------------------------------------------------------------
	// SetTimeScale
	//--------------------------------------------------------------------
	void SetTimeScale(float i_Scale);

	//--------------------------------------------------------------------
	// Marker guide gives feedback on alignment of clips while interacting
	//--------------------------------------------------------------------
	void ShowMarkerGuide(const maTime& i_Time, QColor i_Color);
	void HideMarkerGuide();

	//--------------------------------------------------------------------
	// Draw the timeline guides on the given paint dc
	//--------------------------------------------------------------------
	void PaintGuides(QPainter &i_DC, 
					 int i_Height);
}

#endif // USE_QT
