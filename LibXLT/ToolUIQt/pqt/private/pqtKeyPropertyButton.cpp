/****************************************************************************\
**	pqtKeyPropertyButton.hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/private/pqtKeyPropertyButton.hpp"

#include "Core/Fs/fsFileUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


#ifdef USE_QT

#ifdef QT_FINISH_PORT
BEGIN_EVENT_TABLE(pqtKeyPropertyButton, wxBitmapButton)
    EVT_BUTTON(wxID_ANY, pqtKeyPropertyButton::DoKeyProperty)
END_EVENT_TABLE()
#endif

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pqtKeyPropertyButton::pqtKeyPropertyButton(QWidget* i_pParent, 
											 const std::string& i_PropertyName,
											 KeyPropertyFunction i_KeyPropertyFunction)
:
#ifdef QT_FINISH_PORT
	wxBitmapButton(	i_pParent, wxID_ANY, 
					wxBitmap(), 
					wxDefaultPosition, wxDefaultSize, wxNO_BORDER ),
#endif
	m_PropertyName(i_PropertyName),
	m_KeyPropertyFunction(i_KeyPropertyFunction)
{		
	itString icon_filename;
	fsLocator icon_loc(guiMenuMgr::GetIconDirectory());
	fsLocator default_loc, selected_loc, hover_loc;

#ifdef QT_FINISH_PORT
	default_loc = icon_loc;
	default_loc.Push( "property-key.PNG" );
	fsFileUtil::LocatorToUnicodeString(default_loc, icon_filename);
	this->SetBitmapLabel( wxBitmap( wxString(icon_filename.GetString()), wxBITMAP_TYPE_ANY ) );

	selected_loc = icon_loc;
	selected_loc.Push( "property-key_pressed.PNG" );
	fsFileUtil::LocatorToUnicodeString(selected_loc, icon_filename);
	this->SetBitmapSelected( wxBitmap( wxString(icon_filename.GetString()), wxBITMAP_TYPE_ANY ) );

	hover_loc = icon_loc;
	hover_loc.Push( "property-key_highlight.PNG" );
	fsFileUtil::LocatorToUnicodeString(hover_loc, icon_filename);
	this->SetBitmapHover( wxBitmap( wxString(icon_filename.GetString()), wxBITMAP_TYPE_ANY ) );
#endif
}

#ifdef QT_FINISH_PORT
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pqtKeyPropertyButton::DoKeyProperty(wxCommandEvent& i_Event)
{
	// Call user function to key this property
	if (m_KeyPropertyFunction)
	{
		(*m_KeyPropertyFunction)(m_PropertyName);
	}
}
#endif


#endif // USE_QT
