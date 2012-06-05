/*----------------------------------------------------------------------------
** inTPCInkOverly.hpp
**
**		Class that will keep track of the current strokes on the tablet. 
**		The overlay class allows for stroke size, color, transparency manipulation
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_TPCINKOVERLAY_HPP
#error inTPCInkOverly.hpp multiply included
#endif
#define IN_TPCINKOVERLAY_HPP

#ifndef IN_TPCHEADER_HPP
#include "InputTPC/in/private/inTPCHeader.hpp"
#endif

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class inTPCInkOverlay
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	inTPCInkOverlay();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~inTPCInkOverlay();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	HRESULT Init(HWND i_Hwnd);


private:
	IInkOverlay* m_pInkOverlay;

};  // end class inTPCInkOverlay