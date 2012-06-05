/*****************************************************************************
**  cmpsCompassScale.hpp
**
**      cmpsCompassScale is an geometric object used to display axis
**
**	Extra Large Technology
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_COMPASSSCALE_HPP
#error cmpsCompassScale.hpp multiply included
#endif
#define CMPS_COMPASSSCALE_HPP

#ifndef CMPS_COMPASS_HPP
#include "Support/cmps/cmpsCompass.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class cmpsCompassObjectScale;


//============================================================================
//============================================================================
class cmpsCompassScale : public cmpsCompass
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmpsCompassScale(RenderLayer i_RenderLayer = e_World);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmpsCompassScale();


	private:
		cmpsCompassObjectScale * m_pCompassObjectScale;
};
