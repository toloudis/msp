/*****************************************************************************
**	cptrRenderStatsDialog.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/wxGUI/cptrRenderStatsDialog.hpp"


#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderStatsDialog::UpdateStatString( std::string& i_pString )
{
	m_richText_stats->Freeze();
	m_richText_stats->BeginSuppressUndo();

	m_richText_stats->Clear();
	m_richText_stats->WriteText( wxString(i_pString.c_str()) );

	m_richText_stats->EndSuppressUndo();
	m_richText_stats->Thaw();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrRenderStatsDialog::cptrRenderStatsDialog( wxWindow* parent )
:	cptrRenderStatsDialogBase(parent)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void cptrRenderStatsDialog::RenderStatsDialog_OnActivate( wxActivateEvent& event )
{ 
	event.Skip(); 
}


#endif