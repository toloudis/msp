/*******************************************************************\
** chnlTimelineMarquee.cpp
**
** This namespace will be used to draw rectangles over scrolled windows and channels
**
** StudioGPU
** Copyright(C) 2008 - All Rights Reserved
\*******************************************************************/
#include "Features/Channels/wxGUI/chnlTimelineMarquee.hpp"

#include "Core/dbg/dbgMsg.hpp"
#ifdef USE_WXWIDGETS

namespace
{
	int width = 0.0;
	int height = 0.0;
	int initX = 0.0;
	int initY = 0.0;
	wxPoint savePoint(0.0, 0.0);
	wxPoint endPoint(0.0, 0.0);
	bool l_isMarquee = false;
}
void chnlTimelineMarquee::SetIsMarquee(const bool i_bMarquee)
{
	l_isMarquee=  i_bMarquee;
}
bool chnlTimelineMarquee::IsMarquee()
{
	return l_isMarquee;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimelineMarquee::DrawAllMarquee(int x, int y)
{
	wxPoint temp(0,0);
	temp = ::wxGetMousePosition();
	
   DBG_LOG("Draw All Marquee ");
	//wxClientDC pdc(::wxFindWindowAtPointer(temp)->GetParent()) ;
	//wxDC& dc = pdc;
	
	
	//this->ScreenToClient(&temp.x, &temp.y);
	SetIsMarquee(true);
	//PaintRectangle(dc, x, y);
	wxSize rect;
	rect = ::wxFindWindowAtPointer(temp)->GetSize();

	wxScreenDC DC;
	wxBitmap OLD(rect.GetWidth() , rect.GetHeight() );
wxMemoryDC tDC( OLD );
//tDC.Blit( wxPoint( 0, 0 ), FromDIP(wxSize( OLD.GetWidth(), OLD.GetHeight() )), &DC, wxPoint( 0, 0 ) );
tDC.SelectObject( wxNullBitmap );
tDC.DrawRectangle(x,y, x-initX, y-initY);

	

}
void chnlTimelineMarquee::GetInitPos(int& o_initX, int& o_initY)
{
	o_initX = initX;
	o_initY = initY;
}

void chnlTimelineMarquee::SetInitPos(const int i_initX, const int i_initY)
{
	initX = i_initX;
	initY = i_initY;
	savePoint.x = initX;
	savePoint.y = initY;
}
void chnlTimelineMarquee::PaintRectangle(wxDC &i_DC, int m_CurX, int m_CurY)
{
	if(IsMarquee())
	{
		wxPen pen(*wxRED,3,wxDOT);
		i_DC.SetBrush(*wxBLUE_BRUSH);
		i_DC.SetPen(pen);
		width = savePoint.x - initX ;
		height = savePoint.y - initY;
		i_DC.SetLogicalFunction(wxINVERT);
		i_DC.DrawRectangle(initX, initY, width, height);
		DBG_LOG("x y w h 1 " <<initX << " "<< initY <<" "<<width<<" "<<height);
		DBG_LOG("svpt.x svpt.y "<<savePoint.x<<" "<<savePoint.y);
		endPoint.x = m_CurX;
		endPoint.y = m_CurY;
		width = endPoint.x - initX;
		height = endPoint.y - initY;
		i_DC.DrawRectangle(initX, initY, width, height);
		DBG_LOG("x y w h 2" <<initX << " "<< initY <<" "<<width<<" "<<height);
		DBG_LOG("endpt.x endpt.y "<<endPoint.x<<" "<<endPoint.y);
		
		i_DC.SetLogicalFunction(wxCOPY);
		savePoint = endPoint;
	}

		
}
#endif //USE_WXWIDGETS