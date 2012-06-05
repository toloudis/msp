/*****************************************************************************
**  cmpsCompassObjectSelect.hpp
**
**      cmpsCompassObjectSelect is a geometric object used to display Select.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_COMPASSOBJECTSELECT_HPP
#error cmpsCompassObjectSelect.hpp multiply included
#endif
#define CMPS_COMPASSOBJECTSELECT_HPP

#ifndef CMPS_COMPASSOBJECTSINGLE_HPP
#include "Support/cmps/private/cmpsCompassObjectSingle.hpp"
#endif


class cmpsCompassObjectSelect : public cmpsCompassObjectSingle
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmpsCompassObjectSelect(int i_RenderLayer);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmpsCompassObjectSelect();

	//--------------------------------------------------------------------
	//	Recreate the selection box for the object
	//--------------------------------------------------------------------
	void RecreateObject( const maFloatRGBA& i_Color, const float i_XSize, const float i_YSize, const float i_ZSize );

	//--------------------------------------------------------------------
	//	SetPosition  moves the compass to the position passed in
	//--------------------------------------------------------------------
	void SetPosition(const maPoint3d& i_Position);

	//----------------------------------------------------------------------------
	//	SetBounds sets the bounding area for this compass
	//----------------------------------------------------------------------------
	void SetBounds( const maAxisBox& i_Bounds,
					const maPoint3d& i_WorldPivot, 
					const maPoint3d& i_CameraPos  );

private:

	//--------------------------------------------------------------------
	//	Create_Block creates a object
	//--------------------------------------------------------------------
	cmpsObjectSimple* Create_Block( const maFloatRGBA& i_Color, const float i_XSize, const float i_YSize, const float i_ZSize );

	//--------------------------------------------------------------------
	//	using the latest compass settings, modify the object's shape.
	//--------------------------------------------------------------------
	void ResizeObject(const maPoint3d& i_CameraPos);
};
