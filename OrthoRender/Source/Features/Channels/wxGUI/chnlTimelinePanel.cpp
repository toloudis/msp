/****************************************************************************\
**	chnlTimelinePanel.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/wxGUI/chnlTimelinePanel.hpp"
#include "Features/Channels/wxGUI/chnlTimelineGuideUtil.hpp"

#include "Core/dbg/dbgAssert.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/ma/maFunctions.hpp"

#include <algorithm>

#ifdef USE_WXWIDGETS

namespace
{
	const int c_EdgeOffset = 16;

}


//--------------------------------------------------------------------
// event table
//--------------------------------------------------------------------
BEGIN_EVENT_TABLE(chnlTimelinePanel, wxScrolledWindow)
    EVT_PAINT(chnlTimelinePanel::OnPaint)
END_EVENT_TABLE()


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlTimelinePanel::chnlTimelinePanel(wxWindow* i_pParent)
:	wxScrolledWindow(i_pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxScrolledWindowStyle | wxCLIP_CHILDREN)
{	
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimelinePanel::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxPaintDC dc(this);
    PrepareDC(dc);

	// Outline the client rectangle (instead of using a windows border)
	wxSize size = this->GetSize();
	//dc.SetPen( wxPen( *wxLIGHT_GREY, 1, wxSOLID ) );
	//dc.DrawLine(20, 0, 2, size.GetHeight());
	chnlTimelineGuideUtil::PaintGuides(dc, size.GetHeight());
}

#endif // USE_WXWIDGETS
