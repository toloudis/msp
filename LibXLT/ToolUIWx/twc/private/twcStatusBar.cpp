/****************************************************************************\
**	twcStatusBar.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/twcStatusBar.hpp"

#ifdef USE_WXWIDGETS

BEGIN_EVENT_TABLE(twcStatusBar, wxStatusBar)
	EVT_MOTION (twcStatusBar::OnMouseMove)
END_EVENT_TABLE()

//--------------------------------------------------------------------
//--------------------------------------------------------------------
twcStatusBar::twcStatusBar(wxWindow* i_pParent, 
						   wxWindowID i_Id,
						   long i_Style,
						   const wxString& i_Name)
: wxStatusBar(i_pParent, i_Id, i_Style & ~wxSTB_SHOW_TIPS, i_Name)
{

}

//--------------------------------------------------------------------
// Set the tool tip for a given panel by index
//--------------------------------------------------------------------
void twcStatusBar::SetPanelToolTip(int i_Panel, const std::string& i_String)
{
	if (i_Panel >= m_ToolTips.size())
		m_ToolTips.resize(i_Panel+1);
	m_ToolTips[i_Panel] = i_String;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twcStatusBar::OnMouseMove(wxMouseEvent &i_Event)
{
	const int num_fields = this->GetFieldsCount();
	wxRect rect;
	int panel_index = -1;
	for (int i=0; i<num_fields; ++i)
	{
		if (this->GetFieldRect(i, rect))
		{
			if (rect.Contains(i_Event.GetX(), i_Event.GetY()))
			{
				panel_index = i;
				break;
			}
		}
	}

	// Set the tool tip based on the text for the panel
	if ((panel_index >= 0) && (panel_index < m_ToolTips.size()))
	{
		this->SetToolTip( wxString(m_ToolTips[panel_index].c_str(), wxConvUTF8) );
	}
	else
		this->SetToolTip(L"");

}

#endif // USE_WXWIDGETS
