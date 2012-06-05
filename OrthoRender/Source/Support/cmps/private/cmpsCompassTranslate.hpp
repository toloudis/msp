/*****************************************************************************
**  cmpsCompassTranslate.hpp
**
**      cmpsCompassTranslate is an geometric object used to display axis
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_COMPASSTRANSLATE_HPP
#error cmpsCompassTranslate.hpp multiply included
#endif
#define CMPS_COMPASSTRANSLATE_HPP

#ifndef CMPS_COMPASS_HPP
#include "Support/cmps/cmpsCompass.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class maRotation;
class cmpsCompassObjectTranslate;


//============================================================================
//============================================================================
class cmpsCompassTranslate : public cmpsCompass
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmpsCompassTranslate(RenderLayer i_RenderLayer = e_World,
							 bool i_bAxisAligned = true);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmpsCompassTranslate();

		//--------------------------------------------------------------------
		//	SetOrientation changes the orientation of the compass.
		//--------------------------------------------------------------------
		virtual void SetOrientation(const maRotation& i_Orientation);

	private:
		cmpsCompassObjectTranslate * m_pCompassObjectTranslate;
		bool m_bAxisAligned;
};
