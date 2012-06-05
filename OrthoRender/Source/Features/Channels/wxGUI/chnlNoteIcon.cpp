/****************************************************************************\
**	chnlNoteIcon.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/wxGUI/chnlNoteIcon.hpp"


#ifdef USE_WXWIDGETS

namespace
{
	const wxColour c_Red(255,0,0);
	const wxColour c_Orange(255,165,0);
	const wxColour c_Black(0,0,0);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chnlNoteIcon::chnlNoteIcon(float i_Time, 
							   NoteStatus i_Status, 
							   const std::string& i_Note,
							   float i_MinTime,		
							   float i_TimeScale)
:	chnlTimeIcon(i_Time, i_MinTime, i_TimeScale),
	m_Status(i_Status),
	m_Note(i_Note)
{		
}
//--------------------------------------------------------------------
// paint the icon
//--------------------------------------------------------------------
void chnlNoteIcon::Paint(wxDC &i_DC) const
{
	i_DC.SetPen( *wxTRANSPARENT_PEN ); // turn off outlining

	// Set color based on status of note
    switch (m_Status)
    {
		default:
        case e_Open:
			i_DC.SetBrush(wxBrush(c_Red));
            break;
        case e_Pending:
			i_DC.SetBrush(wxBrush(c_Orange));
            break;
        case e_Closed:
			i_DC.SetBrush(wxBrush(c_Black));
            break;
    }
	
	//	draw the "point" at the bottom of the object
	//
	int ht = 16 - 4;		// 16 is the height of the bar
	int pos = m_Position;
	int xoff = 2;			// thickness
	int yoff = 7;			// shift notes lower into the marker bar

	wxPoint points[4];
	points[0].x = pos-xoff; points[0].y = ht+yoff;
	points[1].x = pos+xoff; points[1].y = ht+yoff;
	points[2].x = pos+xoff; points[2].y = yoff;
	points[3].x = pos-xoff; points[3].y = yoff;
	i_DC.DrawPolygon(4, points);
}

//--------------------------------------------------------------------
// Return true if given pixel is over icon
//--------------------------------------------------------------------
bool chnlNoteIcon::Pick(int i_X, int i_Y) const
{
	//if (i_Y > 7) // doing this comparison in marker bar
	{
		if (::abs(i_X - m_Position) <= 2)
			return true;
	}
	return false;
}

//--------------------------------------------------------------------
/// Return string to display when mouse hovers over icon
//--------------------------------------------------------------------
std::string chnlNoteIcon::GetHoverDescription() const
{
	std::string status;
	switch (m_Status)
	{
        case e_Open:
			status = " Open\n";
            break;
        case e_Pending:
			status = " Pending\n";
            break;
        case e_Closed:
			status = " Closed\n";
            break;
	}
	return (chnlTimeIcon::GetHoverDescription() + status + this->m_Note);
}

#endif // USE_WXWIDGETS