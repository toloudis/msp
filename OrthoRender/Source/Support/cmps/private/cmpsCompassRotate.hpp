/*****************************************************************************
**  cmpsCompassRotate.hpp
**
**      cmpsCompassRotate is an geometric object used to display axis
**
**	Extra Large Technology
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_COMPASSROTATE_HPP
#error cmpsCompassRotate.hpp multiply included
#endif
#define CMPS_COMPASSROTATE_HPP

#ifndef CMPS_COMPASS_HPP
#include "Support/cmps/cmpsCompass.hpp"
#endif

//============================================================================
//	forward references
//============================================================================
class cmpsCompassObjectRotate;


//============================================================================
//============================================================================
class cmpsCompassRotate : public cmpsCompass
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmpsCompassRotate(RenderLayer i_RenderLayer = e_World);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmpsCompassRotate();


	private:
		cmpsCompassObjectRotate * m_pCompassObjectRotate;
};
