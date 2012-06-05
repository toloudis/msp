/*****************************************************************************
**  cmpsCompassTranslate.hpp
**
**      cmpsCompassTranslate is an geometric object used to display axis
**
**	StudioGPU
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
		cmpsCompassTranslate(cmpsRenderLayer::RenderLayer i_RenderLayer,
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
