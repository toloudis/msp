/****************************************************************************\
**	chnlMarkerBar.hpp
**
**		Custom control to display markers and notes
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_MARKERBAR_HPP
#error chnlMarkerBar.hpp multiply included
#endif
#define CHNL_MARKERBAR_HPP

#ifndef CHNL_MARKERICON_HPP
#include "Features/Channels/wxGUI/chnlMarkerIcon.hpp"
#endif 
#ifndef CHNL_NOTEICON_HPP
#include "Features/Channels/wxGUI/chnlNoteIcon.hpp"
#endif 
#ifndef CHNL_TIMECOMMON_HPP
#include "Features/Channels/wxGUI/chnlTimeCommon.hpp"
#endif 
#ifndef TWC_EVENT_HPP
#include "ToolUIWx/twc/twcEvent.hpp"
#endif

#include <list>

#ifdef USE_WXWIDGETS

//============================================================================
// wxEVT_VALUE_CHANGED event is thrown when the current time is changed
//============================================================================

//============================================================================
//============================================================================
class chnlMarkerBar : public wxControl, public chnlTimeCommon
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlMarkerBar(wxWindow* i_pParent);

		//--------------------------------------------------------------------
		// Add marker to display
		//--------------------------------------------------------------------
		void AddMarker(float i_Time, chnlMarkerIcon::MarkerType i_Type, const std::string& i_Note);

		//--------------------------------------------------------------------
		// Clear markers from display
		//--------------------------------------------------------------------
		void ClearMarkers();

		//--------------------------------------------------------------------
		// Add note to display
		//--------------------------------------------------------------------
		void AddNote(float i_Time, chnlNoteIcon::NoteStatus i_Status, const std::string& i_Note);

		//--------------------------------------------------------------------
		// Clear notes from display
		//--------------------------------------------------------------------
		void ClearNotes();

	private:
		//--------------------------------------------------------------------
		// private functions
		//--------------------------------------------------------------------
		virtual void update_size();
		void compute_size();
		const chnlTimeIcon* pick_icon(int i_X, int i_Y) const;
		void show_properties(const chnlTimeIcon* i_pIcon);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnPaint(wxPaintEvent &i_Event);
		void OnContextMenu(wxContextMenuEvent& i_Event);
		void OnDoubleClick(wxMouseEvent& i_Event);
		void OnShowProperties(wxCommandEvent& i_Event);
		void OnMouseMove(wxMouseEvent &i_Event);

		std::list<chnlMarkerIcon> m_Markers;
		std::list<chnlNoteIcon> m_Notes;
		const chnlTimeIcon* m_pContextIcon;

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
