/****************************************************************************\
**	chnlTimelineGuideUtil.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/wxGUI/chnlTimelineGuideUtil.hpp"

#ifdef USE_WXWIDGETS

namespace
{
	const int c_EdgeOffset = 16;

	float l_CurrentTime = 0.0f;
	float l_MinTime = 0.0f;
	float l_TimeScale = 60.0f;

	float l_MarkerTime = 0.0f;
	wxColour l_MarkerColor(0.5,0.5,0.5);
	bool l_bMarkerVisible = false;
}

//--------------------------------------------------------------------
// Move timeline guide for current time.
//--------------------------------------------------------------------
void chnlTimelineGuideUtil::SetCurrentTime(float i_Time)
{
	l_CurrentTime = i_Time;
}

//--------------------------------------------------------------------
// SetMinTime - set minimum time in timeline, used to offset 
//	the guidelines
//--------------------------------------------------------------------
void chnlTimelineGuideUtil::SetMinTime(float i_MinTime)
{
	l_MinTime = i_MinTime;
}

//--------------------------------------------------------------------
// SetTimeScale
//--------------------------------------------------------------------
void chnlTimelineGuideUtil::SetTimeScale(float i_Scale)
{
	l_TimeScale = i_Scale;
}

//--------------------------------------------------------------------
// Marker guide gives feedback on alignment of clips while interacting
//--------------------------------------------------------------------
void chnlTimelineGuideUtil::ShowMarkerGuide(float i_Time,
											wxColour i_Color)
{
	l_bMarkerVisible = true;
	l_MarkerTime = i_Time;
	l_MarkerColor = i_Color;
}
void chnlTimelineGuideUtil::HideMarkerGuide()
{
	l_bMarkerVisible = false;
}

//--------------------------------------------------------------------
// Draw the timeline guides on the given paint dc
//--------------------------------------------------------------------
void chnlTimelineGuideUtil::PaintGuides(wxDC &i_DC, 
										int i_Height)
{
	// Draw guide for current time
	i_DC.SetPen( wxPen( *wxLIGHT_GREY, 1, wxSOLID ) );

	int pos = (int)((l_CurrentTime-l_MinTime) * l_TimeScale + c_EdgeOffset);
	i_DC.DrawLine(pos, 0, pos, i_Height);

	if (l_bMarkerVisible)
	{
		i_DC.SetPen( wxPen( l_MarkerColor, 1, wxSOLID ) );
		int pos = (int)((l_MarkerTime-l_MinTime) * l_TimeScale + c_EdgeOffset);
		i_DC.DrawLine(pos, 0, pos, i_Height);
	}
}

#endif // USE_WXWIDGETS
