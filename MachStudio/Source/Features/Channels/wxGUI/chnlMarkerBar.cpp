/****************************************************************************\
**	chnlMarkerBar.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/wxGUI/chnlMarkerBar.hpp"

#include "Features/Channels/Markers/chnlMarkerOperations.hpp"
#include "Features/Channels/Notes/chnlNotesOperations.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/Ma/maTime.hpp"

#include <algorithm>

#ifdef USE_WXWIDGETS

namespace
{
	//--------------------------------------------------------------------
	// IDs for the menu commands
	//--------------------------------------------------------------------
	enum
	{
		MENU_SHOW_PROPERTIES = wxID_HIGHEST
	};

	//--------------------------------------------------------------------
	// Predicate for picking icons
	//--------------------------------------------------------------------
	template <class Icon>
	struct icon_pick
	{
		icon_pick(int i_X, int i_Y) : m_X(i_X), m_Y(i_Y) {}
		bool operator()(const Icon& i_Icon) const
		{
			return i_Icon.Pick(m_X, m_Y);
		}
		int m_X, m_Y;
	};
}


//--------------------------------------------------------------------
// event table
//--------------------------------------------------------------------
BEGIN_EVENT_TABLE(chnlMarkerBar, wxControl)
    EVT_PAINT(chnlMarkerBar::OnPaint)
    EVT_CONTEXT_MENU(chnlMarkerBar::OnContextMenu)
    EVT_MOTION(chnlMarkerBar::OnMouseMove)
    EVT_MENU(MENU_SHOW_PROPERTIES, chnlMarkerBar::OnShowProperties)
	EVT_LEFT_DCLICK(chnlMarkerBar::OnDoubleClick)
END_EVENT_TABLE()


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlMarkerBar::chnlMarkerBar(wxWindow* i_pParent)
:	wxControl(i_pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
	m_pContextIcon(NULL)
{	
	this->compute_size();	
}

//--------------------------------------------------------------------
// Add marker to display
//--------------------------------------------------------------------
void chnlMarkerBar::AddMarker(float i_Time, chnlMarkerIcon::MarkerType i_Type, const std::string& i_Note)
{
	m_Markers.push_back( chnlMarkerIcon(i_Time, i_Type, i_Note, m_MinTime, m_TimeScale) );
	m_Markers.sort();
	this->Refresh();
}

//--------------------------------------------------------------------
// Clear markers from display
//--------------------------------------------------------------------
void chnlMarkerBar::ClearMarkers()
{
	m_Markers.clear();
	m_pContextIcon = NULL;
	this->Refresh();
}

//--------------------------------------------------------------------
// Add note to display
//--------------------------------------------------------------------
void chnlMarkerBar::AddNote(float i_Time, chnlNoteIcon::NoteStatus i_Status, const std::string& i_Note)
{
	m_Notes.push_back( chnlNoteIcon(i_Time, i_Status, i_Note, m_MinTime, m_TimeScale) );
	m_Notes.sort();
	this->Refresh();
}

//--------------------------------------------------------------------
// Clear notes from display
//--------------------------------------------------------------------
void chnlMarkerBar::ClearNotes()
{
	m_Notes.clear();
	m_pContextIcon = NULL;
	this->Refresh();
}

//--------------------------------------------------------------------
// Virtual function called when time range or scale has changed
//--------------------------------------------------------------------
void chnlMarkerBar::update_size()
{	
	this->compute_size();

	// Scale markers
	std::list<chnlMarkerIcon>::iterator mit;
	for (mit = m_Markers.begin(); mit != m_Markers.end(); ++mit)
	{
		mit->AlterTimeScale(m_MinTime, m_TimeScale);
	}

	// Scale notes
	std::list<chnlNoteIcon>::iterator nit;
	for (nit = m_Notes.begin(); nit != m_Notes.end(); ++nit)
	{
		nit->AlterTimeScale(m_MinTime, m_TimeScale);
	}

	this->Refresh();
}

//----------------------------------------------------------------------------
// Adjust size of control based on expanded and categories of clips
//----------------------------------------------------------------------------
void chnlMarkerBar::compute_size()
{
	wxSize size(this->get_full_width(), 16);
	this->SetMinSize(size);
	this->SetSize(size);
}

//----------------------------------------------------------------------------
// Pick marker or note under given position
//----------------------------------------------------------------------------
const chnlTimeIcon* chnlMarkerBar::pick_icon(int i_X, int i_Y) const
{
	// The notes are lower than the markers, so just decide
	// which list to search based on the Y value
	if (i_Y >= 8)
	{
		std::list<chnlNoteIcon>::const_iterator nit =
			std::find_if(m_Notes.begin(), m_Notes.end(), icon_pick<chnlNoteIcon>(i_X, i_Y));
		if (nit != m_Notes.end())
			return &(*nit);
	}
	else
	{
		std::list<chnlMarkerIcon>::const_iterator mit =
			std::find_if(m_Markers.begin(), m_Markers.end(), icon_pick<chnlMarkerIcon>(i_X, i_Y));
		if (mit != m_Markers.end())
			return &(*mit);
	}

	return NULL;
}

//----------------------------------------------------------------------------
// Display properties for marker or note icon.
//----------------------------------------------------------------------------
void chnlMarkerBar::show_properties(const chnlTimeIcon* i_pIcon)
{
	if (const chnlMarkerIcon* pMarkerIcon = dynamic_cast<const chnlMarkerIcon*>(i_pIcon))
	{
		chnlMarkerOperations::ShowProperties(maTime::FromSeconds(i_pIcon->GetTime()));
	}
	else if (const chnlNoteIcon* pNoteIcon = dynamic_cast<const chnlNoteIcon*>(i_pIcon))
	{
		chnlNotesOperations::ShowProperties(maTime::FromSeconds(i_pIcon->GetTime()));
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlMarkerBar::OnPaint(wxPaintEvent &WXUNUSED(event))
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

	// Outline the client rectangle (instead of using a windows border)
	wxSize size = this->GetSize();
	dc.SetPen( wxPen( *wxLIGHT_GREY, 1, wxSOLID ) );
	dc.SetBrush(*wxTRANSPARENT_BRUSH); // turn off fill
	dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

	// Draw markers
	std::list<chnlMarkerIcon>::const_iterator mit;
	for (mit = m_Markers.begin(); mit != m_Markers.end(); ++mit)
	{
		mit->Paint(dc);
	}

	// Draw notes
	std::list<chnlNoteIcon>::const_iterator nit;
	for (nit = m_Notes.begin(); nit != m_Notes.end(); ++nit)
	{
		nit->Paint(dc);
	}

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlMarkerBar::OnContextMenu(wxContextMenuEvent& i_Event)
{
	wxPoint point = i_Event.GetPosition();
	// Skip, if from keyboard (x and y == -1 if from keyboard)
	if (point.x != -1 || point.y != -1) 
	{
		point = ScreenToClient(point);
		m_pContextIcon = pick_icon(point.x, point.y);
		if (m_pContextIcon)
		{
			wxMenu menu;
			menu.Append(MENU_SHOW_PROPERTIES, _T("Show &Properties"));
			PopupMenu(&menu, point.x, point.y);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlMarkerBar::OnDoubleClick(wxMouseEvent& i_Event)
{
	const chnlTimeIcon* pIcon = pick_icon(i_Event.GetX(), i_Event.GetY());
	if (pIcon)
	{
		m_pContextIcon = pIcon;
		show_properties(m_pContextIcon);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlMarkerBar::OnShowProperties(wxCommandEvent& i_Event)
{
	if (m_pContextIcon)
		show_properties(m_pContextIcon);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlMarkerBar::OnMouseMove(wxMouseEvent &i_Event)
{
	const chnlTimeIcon* pIcon = pick_icon(i_Event.GetX(), i_Event.GetY());
	if (pIcon)
	{
		this->SetToolTip( wxString(pIcon->GetHoverDescription().c_str(), wxConvUTF8) );
	}
	else
	{
		this->SetToolTip( wxT("") );
	}
}

#endif // USE_WXWIDGETS
