/*****************************************************************************
**  cmpsCompassSelect.hpp
**
**      cmpsCompassSelect is an geometric object used to display
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_COMPASSSELECT_HPP
#error cmpsCompassSelect.hpp multiply included
#endif
#define CMPS_COMPASSSELECT_HPP

#ifndef CMPS_COMPASS_HPP
#include "Support/cmps/cmpsCompass.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class maRotation;
class cmpsCompassObjectSelect;


//============================================================================
//============================================================================
class cmpsCompassSelect : public cmpsCompass
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmpsCompassSelect(cmpsRenderLayer::RenderLayer i_RenderLayer);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmpsCompassSelect();

		//--------------------------------------------------------------------
		//	SetOrientation changes the orientation of the compass.
		//--------------------------------------------------------------------
		virtual void SetOrientation(const maRotation& i_Orientation);

	private:
		cmpsCompassObjectSelect * m_pCompassObjectSelect;
};
