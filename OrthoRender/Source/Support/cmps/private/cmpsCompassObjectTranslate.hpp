/*****************************************************************************
**  cmpsCompassObjectTranslate.hpp
**
**      cmpsCompassObjectTranslate is a geometric object used to display axis
**	for Translate.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_COMPASSOBJECTTRANSLATE_HPP
#error cmpsCompassObjectTranslate.hpp multiply included
#endif
#define CMPS_COMPASSOBJECTTRANSLATE_HPP

#ifndef CMPS_COMPASSOBJECT3AXIS_HPP
#include "Support/cmps/private/cmpsCompassObject3Axis.hpp"
#endif


class cmpsCompassObjectTranslate : public cmpsCompassObject3Axis
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmpsCompassObjectTranslate(int i_RenderLayer);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmpsCompassObjectTranslate();

protected:
	//----------------------------------------------------------------------------
	//	GetDefaultSize() - get the default size of the shape
	//----------------------------------------------------------------------------
	virtual float GetDefaultSize() const;

private:
	//--------------------------------------------------------------------
	//	Create_Cone creates a object
	//--------------------------------------------------------------------
	cmpsObjectSimple* Create_Cone( const maFloatRGBA& i_Color );
};
