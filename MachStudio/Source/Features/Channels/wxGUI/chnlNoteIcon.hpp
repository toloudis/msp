/****************************************************************************\
**	chnlNoteIcon.hpp
**
**		Data class for a note to display in marker bar
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_NOTEICON_HPP
#error chnlNoteIcon.hpp multiply included
#endif
#define CHNL_NOTEICON_HPP

#ifndef CHNL_TIMEICON_HPP
#include "Features/Channels/wxGUI/chnlTimeIcon.hpp"
#endif 

#ifdef USE_WXWIDGETS

#include <string>

//============================================================================
//============================================================================
class chnlNoteIcon : public chnlTimeIcon
{
	public:
		//--------------------------------------------------------------------
		// enumeration of note status
		//--------------------------------------------------------------------
		enum NoteStatus
		{
			e_Open = 0,
			e_Pending,
			e_Closed
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlNoteIcon(float i_Time, 
					   NoteStatus i_Status, 
					   const std::string& i_Note,
					   float i_MinTime,
					   float i_TimeScale);

		//--------------------------------------------------------------------
		// paint the icon
		//--------------------------------------------------------------------
		virtual void Paint(wxDC &i_DC) const;

		//--------------------------------------------------------------------
		// Return true if given pixel is over icon
		//--------------------------------------------------------------------
		virtual bool Pick(int i_X, int i_Y) const;

		//--------------------------------------------------------------------
		// Return string to display when mouse hovers over icon
		//--------------------------------------------------------------------
		virtual std::string GetHoverDescription() const;


	private:
		NoteStatus m_Status;
		std::string m_Note;
};

#endif // USE_WXWIDGETS
