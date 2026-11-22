/****************************************************************************\
**	chnlNoteIcon.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/qtGUI/chnlNoteIcon.hpp"


#ifdef USE_QT

#include <QPainter>

namespace
{
	const QColor c_Red(255,0,0);
	const QColor c_Orange(255,165,0);
	const QColor c_Black(0,0,0);
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
void chnlNoteIcon::Paint(QPainter &i_DC) const
{
	i_DC.setPen( Qt::NoPen ); // turn off outlining

	// Set color based on status of note
    switch (m_Status)
    {
		default:
        case e_Open:
			i_DC.setBrush(c_Red);
            break;
        case e_Pending:
			i_DC.setBrush(c_Orange);
            break;
        case e_Closed:
			i_DC.setBrush(c_Black);
            break;
    }
	
	//	draw the "point" at the bottom of the object
	//
	int ht = 16 - 4;		// 16 is the height of the bar
	int pos = m_Position;
	int xoff = 2;			// thickness
	int yoff = 7;			// shift notes lower into the marker bar

	QPoint points[4];
	points[0] = QPoint(pos-xoff, ht+yoff);
	points[1] = QPoint(pos+xoff, ht+yoff);
	points[2] = QPoint(pos+xoff, yoff);
	points[3] = QPoint(pos-xoff, yoff);
	i_DC.drawPolygon(points, 4);
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
// Return string to display when mouse hovers over icon
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

#endif // USE_QT