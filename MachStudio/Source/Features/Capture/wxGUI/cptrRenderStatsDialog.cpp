/*****************************************************************************
**	cptrRenderStatsDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/wxGUI/cptrRenderStatsDialog.hpp"


#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderStatsDialog::UpdateStatString( itString& i_pString )
{
	m_richText_stats->Freeze();
	m_richText_stats->BeginSuppressUndo();

	//m_richText_stats->Clear();
	m_richText_stats->WriteText( wxString(i_pString.GetString()) );

	m_richText_stats->EndSuppressUndo();
	m_richText_stats->Thaw();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderStatsDialog::AddStatString( itString& i_pString )
{
	m_richText_stats->Freeze();
	m_richText_stats->BeginSuppressUndo();

	m_richText_stats->WriteText( wxString(i_pString.GetString()) );
	m_richText_stats->Newline();

	m_richText_stats->EndSuppressUndo();
	m_richText_stats->Thaw();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderStatsDialog::Clear()
{
	m_richText_stats->Freeze();
	m_richText_stats->BeginSuppressUndo();

	m_richText_stats->Clear();

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