/****************************************************************************\
**	chnlMarkerBar.hpp
**
**		Bar above the timeline showing the markers and notes, in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_MARKERBAR_HPP
#error chnlMarkerBar.hpp multiply included
#endif
#define CHNL_MARKERBAR_HPP

#ifndef CHNL_MARKERICON_HPP
#include "Features/Channels/qtGUI/chnlMarkerIcon.hpp"
#endif
#ifndef CHNL_NOTEICON_HPP
#include "Features/Channels/qtGUI/chnlNoteIcon.hpp"
#endif
#ifndef CHNL_TIMECOMMON_HPP
#include "Features/Channels/qtGUI/chnlTimeCommon.hpp"
#endif

#include <list>

#ifdef USE_QT

//============================================================================
//============================================================================
class chnlMarkerBar : public QWidget, public chnlTimeCommon
{
	public:
		chnlMarkerBar(QWidget* i_pParent);

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

	protected:
		void paintEvent(QPaintEvent* i_pEvent) override;
		void contextMenuEvent(QContextMenuEvent* i_pEvent) override;
		void mouseDoubleClickEvent(QMouseEvent* i_pEvent) override;
		void mouseMoveEvent(QMouseEvent* i_pEvent) override;

	private:
		virtual void update_size();
		void compute_size();
		const chnlTimeIcon* pick_icon(int i_X, int i_Y) const;
		void show_properties(const chnlTimeIcon* i_pIcon);

		std::list<chnlMarkerIcon> m_Markers;
		std::list<chnlNoteIcon> m_Notes;
		const chnlTimeIcon* m_pContextIcon;
};

#endif // USE_QT
