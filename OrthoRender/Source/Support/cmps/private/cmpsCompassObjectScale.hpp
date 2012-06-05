/*****************************************************************************
**  cmpsCompassObjectScale.hpp
**
**      cmpsCompassObjectScale is an geometric object used to display axis
**	for Scale.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_COMPASSOBJECTSCALE_HPP
#error cmpsCompassObjectScale.hpp multiply included
#endif
#define CMPS_COMPASSOBJECTSCALE_HPP

#ifndef CMPS_COMPASSOBJECT3AXIS_HPP
#include "Support/cmps/private/cmpsCompassObject3Axis.hpp"
#endif


class cmpsCompassObjectScale : public cmpsCompassObject3Axis
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmpsCompassObjectScale(int i_RenderLayer);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmpsCompassObjectScale();

	//--------------------------------------------------------------------
	//	SetScale changes the scale of the compass.
	//--------------------------------------------------------------------
	virtual void SetScale(const maPoint3d& i_Scale);

protected:
	//----------------------------------------------------------------------------
	//	GetDefaultSize() - get the default size of the shape
	//----------------------------------------------------------------------------
	virtual float GetDefaultSize() const;

	//--------------------------------------------------------------------
	//	using the latest compass settings, modify the object's shape.
	//--------------------------------------------------------------------
	virtual void ResizeObject(const maPoint3d& i_CameraPos);

private:
	//--------------------------------------------------------------------
	//	Create_Cylinder creates a object
	//--------------------------------------------------------------------
	cmpsObjectSimple* Create_Cylinder( const maFloatRGBA& i_Color );
};
