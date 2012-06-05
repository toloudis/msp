/****************************************************************************\
**	chnlTimeIcon.hpp
**
**		Base class for notes and markers to display in marker bar
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TIMEICON_HPP
#error chnlTimeIcon.hpp multiply included
#endif
#define CHNL_TIMEICON_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class chnlTimeIcon 
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlTimeIcon(float i_Time, 
					 float i_MinTime,
					 float i_TimeScale);

		//--------------------------------------------------------------------
		// Return time value for this icon
		//--------------------------------------------------------------------
		inline float GetTime() const;

		//--------------------------------------------------------------------
		// Alter position of note based on scaling of time values
		//--------------------------------------------------------------------
		void AlterTimeScale(float i_MinTime, float i_TimeScale);

		//--------------------------------------------------------------------
		// comparison function to sort note icons
		//--------------------------------------------------------------------
		bool operator<(const chnlTimeIcon& i_Icon) const;

		//--------------------------------------------------------------------
		// paint the icon
		//--------------------------------------------------------------------
		virtual void Paint(wxDC &i_DC) const = 0;

		//--------------------------------------------------------------------
		// Return true if given pixel is over icon
		//--------------------------------------------------------------------
		virtual bool Pick(int i_X, int i_Y) const = 0;

		//--------------------------------------------------------------------
		/// Return string to display when mouse hovers over icon
		//--------------------------------------------------------------------
		virtual std::string GetHoverDescription() const;

	protected:
		float m_Time;
		int m_Position;
};

//--------------------------------------------------------------------
// Return time value for this icon
//--------------------------------------------------------------------
inline float chnlTimeIcon::GetTime() const
{
	return m_Time;
}

#endif // USE_WXWIDGETS
