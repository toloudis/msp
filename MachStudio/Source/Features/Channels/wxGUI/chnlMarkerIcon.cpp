/****************************************************************************\
**	chnlMarkerIcon.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/wxGUI/chnlMarkerIcon.hpp"


#ifdef USE_WXWIDGETS

namespace
{
	const wxColour c_DarkRed(139,0,0);
	const wxColour c_DarkGreen(0,100,0);
	const wxColour c_CornflowerBlue(100, 149, 237);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chnlMarkerIcon::chnlMarkerIcon(float i_Time, 
							   MarkerType i_Type, 
							   const std::string& i_Note,
							   float i_MinTime,
							   float i_TimeScale)
:	chnlTimeIcon(i_Time, i_MinTime, i_TimeScale),
	m_Type(i_Type),
	m_Note(i_Note)
{		
}

//--------------------------------------------------------------------
// paint the icon
//--------------------------------------------------------------------
void chnlMarkerIcon::Paint(wxDC &i_DC) const
{
	i_DC.SetPen( *wxTRANSPARENT_PEN ); // turn off outlining

	// Set color based on type of marker
    switch (m_Type)
    {
        case e_Out:
			i_DC.SetBrush(wxBrush(c_DarkRed));
            break;
        case e_In:
			i_DC.SetBrush(wxBrush(c_DarkGreen));
            break;
		default:
        case e_Normal:
			i_DC.SetBrush(wxBrush(c_CornflowerBlue));
            break;
    }

	
	//	draw the "point" at the bottom of the object
	//
	int ht = 16 - 4;		// 16 is the height of the bar
	int pos = m_Position;	
	int off = ht / 4;

	wxPoint points[3];
	points[0].x = pos; points[0].y = ht;
	points[1].x = pos-off; points[1].y = 0;
	points[2].x = pos+off; points[2].y = 0;
	i_DC.DrawPolygon(3, points);

}

//--------------------------------------------------------------------
// Return true if given pixel is over icon
//--------------------------------------------------------------------
bool chnlMarkerIcon::Pick(int i_X, int i_Y) const
{
	//if (i_Y <= 7) // doing this comparison in marker bar
	{
		if (::abs(i_X - m_Position) <= 2)
			return true;
	}
	return false;
}


//--------------------------------------------------------------------
// Return string to display when mouse hovers over icon
//--------------------------------------------------------------------
std::string chnlMarkerIcon::GetHoverDescription() const
{
	std::string type;
	switch (m_Type)
	{
        case e_Out:
			type = " Out\n";
            break;
        case e_In:
			type = " In\n";
            break;
        case e_Normal:
			type = " Normal\n";
            break;
	}
	return (chnlTimeIcon::GetHoverDescription() + type + this->m_Note);
}


#endif // USE_WXWIDGETS