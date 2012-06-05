/*******************************************************************\
** chnlTimelineMarquee.hpp
**
** This namespace will b eused to draw rectangles over scrolled windows and channels
**
** StudioGPU
** Copyright(C) 2008 - All Rights Reserved
\*******************************************************************/

#ifdef CHNL_TIMELINEMARQUEE_HPP
#error chnlTimelineMarquee.hpp multiply included
#endif
#define CHNL_TIMELINEMARQUEE_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

namespace chnlTimelineMarquee
{
	void PaintRectangle( wxDC& i_DC, int m_CurX, int m_CurY);
	void DrawAllMarquee(int x, int y);
	void SetIsMarquee(const bool i_bMarquee);
	bool IsMarquee();
	
	void SetInitPos(const int i_initX, const int i_initY);
	void GetInitPos(int& o_initX, int& o_initY);

}

#endif //USE_WXWIDGETS