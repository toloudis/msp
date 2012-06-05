/****************************************************************************\
**	chnlMarkerIcon.hpp
**
**		Data class for a marker to display in marker bar
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_MARKERICON_HPP
#error chnlMarkerIcon.hpp multiply included
#endif
#define CHNL_MARKERICON_HPP

#ifndef CHNL_TIMEICON_HPP
#include "Features/Channels/wxGUI/chnlTimeIcon.hpp"
#endif 

#ifdef USE_WXWIDGETS

#include <string>

//============================================================================
//============================================================================
class chnlMarkerIcon : public chnlTimeIcon
{
	public:
		//--------------------------------------------------------------------
		// enumeration of marker types
		//--------------------------------------------------------------------
		enum MarkerType
		{
			e_Normal = 0,
			e_In,
			e_Out
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlMarkerIcon(float i_Time, 
					   MarkerType i_Type, 
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
		MarkerType m_Type;
		std::string m_Note;
};

#endif // USE_WXWIDGETS
