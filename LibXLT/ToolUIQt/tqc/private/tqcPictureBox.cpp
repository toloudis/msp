/****************************************************************************\
**	twcColorRGBAEdit.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/private/twcPictureBox.hpp"

#include <wx/dcclient.h>

#ifdef USE_WXWIDGETS

BEGIN_EVENT_TABLE(twcPictureBox, wxPanel)

// catch paint events
EVT_PAINT(twcPictureBox::paintEvent)
 
END_EVENT_TABLE()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
twcPictureBox::twcPictureBox(wxWindow *parent,
            wxWindowID winid,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
:	wxPanel(parent, winid, pos, size, style, name)
{		
	m_texture = new wxBitmap(size.x, size.y);
	this->Layout();
}

twcPictureBox::~twcPictureBox()
{
	delete m_texture;
	m_texture = NULL;
}

void twcPictureBox::LoadImage(wxString i_file, bool i_changePanelSize, wxBitmapType in_format)
{
	if (i_file.size() != 0)
	{
		if (!m_texture)
		{
			m_texture = new wxBitmap();
		}

		m_texture->LoadFile(i_file, in_format);

		if (i_changePanelSize)
		{
			this->SetSize(m_texture->GetWidth(), m_texture->GetHeight());
		}
	}
}

void twcPictureBox::SetImage(wxBitmap *i_tex, bool i_changePanelSize)
{
	if(m_texture)
	{
		delete m_texture;
		m_texture = NULL;
	}

	m_texture = i_tex;

	if (m_texture && i_changePanelSize)
	{
		this->SetSize(m_texture->GetWidth(), m_texture->GetHeight());
	}
}

void twcPictureBox::paintEvent(wxPaintEvent &evt)
{
	if (m_texture)
	{
		wxPaintDC dc(this);
		render(dc);
	}
}


void twcPictureBox::paintNow()
{
	if (m_texture)
	{
		wxClientDC dc(this);
		render(dc);
	}
}

void twcPictureBox::render(wxDC& i_dc)
{
	i_dc.DrawBitmap(*m_texture, 0, 0, false );
}


#endif // USE_WXWIDGETS
