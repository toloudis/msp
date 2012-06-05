/****************************************************************************\
**	pwxCategoryLabel.hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/private/pwxCategoryLabel.hpp"

#include <map>
#include <set>

#ifdef USE_WXWIDGETS

namespace
{
	// This map keeps track of which category names have been collpased
	// by the user in order to create them in the same state the next time.
	// The primary mapping is from DialogName to a Set of Category Names that
	// have been collapsed.
	std::map< std::string, std::set< wxString > > l_CollapsedStates;

	void set_collapsed_state(const std::string& i_DialogName,
							 const wxString &i_CategoryName,
							 bool i_bCollapsed)
	{
		if (i_bCollapsed)
			l_CollapsedStates[i_DialogName].insert(i_CategoryName);
		else
			l_CollapsedStates[i_DialogName].erase(i_CategoryName);
	}

	bool get_collapsed_state(const std::string& i_DialogName,
							 const wxString &i_CategoryName)
	{
		std::set< wxString > &collapsed_states = l_CollapsedStates[i_DialogName];
		bool bCollapsed = (collapsed_states.find(i_CategoryName) != collapsed_states.end());
		return bCollapsed;
	}
}

BEGIN_EVENT_TABLE(pwxCategoryLabel, wxControl)
    EVT_PAINT(pwxCategoryLabel::OnPaint)
	//EVT_LEFT_DCLICK(pwxCategoryLabel::OnDoubleClick)
	EVT_LEFT_DOWN(pwxCategoryLabel::OnClick)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pwxCategoryLabel::pwxCategoryLabel(wxWindow* i_pParent,
						 const std::string& i_DialogName, 
						 const wxString& i_LabelText,
						 wxSizer* i_pSizer,
						 int i_ItemIndex)
:	wxControl(i_pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
	m_DialogName(i_DialogName),
	m_LabelText(i_LabelText),
	m_pSizer(i_pSizer),
	m_ItemIndex(i_ItemIndex)
{		
	//this->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

//----------------------------------------------------------------------------
// Return true if this category label should be initially collapsed
// based on what the user has done previously.
//----------------------------------------------------------------------------
bool pwxCategoryLabel::GetInitialCollapsedState() const
{
	return get_collapsed_state(m_DialogName, m_LabelText);
}

//----------------------------------------------------------------------------
// This code is based on wxStaticText, but with the code about 
//	the border removed (assuming border is 0)
//----------------------------------------------------------------------------
wxSize pwxCategoryLabel::DoGetBestSize() const
{
    wxClientDC dc(wx_const_cast(pwxCategoryLabel *, this));
    wxFont font(GetFont());
    if (!font.Ok())
        font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

    dc.SetFont(font);

    wxCoord widthTextMax, heightTextTotal;
    dc.GetMultiLineTextExtent(GetLabelText(), &widthTextMax, &heightTextTotal);

	// icon needs at least 9 in height
	if (heightTextTotal < 9)
		heightTextTotal = 9;

    wxSize best(widthTextMax, heightTextTotal);
    CacheBestSize(best);
    return best;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pwxCategoryLabel::OnPaint(wxPaintEvent &i_Event)
{
    wxPaintDC pdc(this);

//#if wxUSE_GRAPHICS_CONTEXT
//     wxGCDC gdc( pdc ) ;
//    wxDC &dc = m_useContext ? (wxDC&) gdc : (wxDC&) pdc ;
//#else
    wxDC &dc = pdc ;
//#endif

    PrepareDC(dc);

    dc.Clear();
	wxRect client_rect = this->GetClientRect();

	// Expanded, compressed state
	const int icon_size = 7;
	int y = (client_rect.height - icon_size) / 2;
	dc.SetPen( wxPen(this->GetForegroundColour(), 1, wxSOLID) );
	dc.SetBrush(*wxTRANSPARENT_BRUSH); // turn off fill
	dc.DrawRectangle(2, y, icon_size, icon_size);

	bool bIsShown = m_pSizer->IsShown(m_ItemIndex);
	int half_size = icon_size / 2;
	dc.DrawLine(2, y+half_size, 2+icon_size, y+half_size);
	if (!bIsShown)
		dc.DrawLine(2+half_size, y, 2+half_size, y+icon_size);
	
	// Category Text
	dc.SetTextForeground(this->GetForegroundColour());
	dc.SetFont(this->GetFont());
	dc.DrawText(m_LabelText, 12, 0);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pwxCategoryLabel::OnClick(wxMouseEvent& i_Event)
{
	if (m_pSizer)
	{
		// Toggle visiblity of item and then redo layout
		bool bIsShown = m_pSizer->IsShown(m_ItemIndex);
		m_pSizer->Show(m_ItemIndex, !bIsShown);
		set_collapsed_state(m_DialogName, m_LabelText, bIsShown); // Collapsed is true, when bIsShown was true

		//m_pSizer->Layout();
		//this->Refresh();

		// Can't just lay it out, have to 
		wxWindow *pParent = this->GetParent();
		wxScrolledWindow *pScrollWindow = dynamic_cast<wxScrolledWindow*>(pParent);
		if (pScrollWindow)
		{
			wxSize size = pScrollWindow->GetBestVirtualSize();
		      // This will call Layout() and AdjustScrollbars()
		    pScrollWindow->SetVirtualSize( size );
		}
		else
		{
			pParent->Layout();
		}
		// Need to force a redraw also
		pParent->Refresh();
	}
}

#endif // USE_WXWIDGETS
