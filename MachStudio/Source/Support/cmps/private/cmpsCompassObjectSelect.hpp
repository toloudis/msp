/*****************************************************************************
**  cmpsCompassObjectSelect.hpp
**
**      cmpsCompassObjectSelect is a geometric object used to display Select.
**
**	StudioGPU
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
	cmpsCompassObjectSelect(cmpsRenderLayer::RenderLayer i_RenderLayer);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmpsCompassObjectSelect();

	//--------------------------------------------------------------------
	//	Recreate the selection box for the object
	//--------------------------------------------------------------------
	//void RecreateObject( const maFloatRGBA& i_Color, const float i_XSize, const float i_YSize, const float i_ZSize );

	//--------------------------------------------------------------------
	//	SetPosition  moves the compass to the position passed in
	//--------------------------------------------------------------------
	void SetPosition(const maPoint3d& i_Position);

	//----------------------------------------------------------------------------
	//	AdjustCompassToBounds updates the compass to the position
	//		and bounding area of an object. This replaces the individual calls
	//		to SetPosition, SetOrientation and SetBounds.
	//----------------------------------------------------------------------------
	virtual void AdjustCompassToBounds( int i_IconLayerIndex,
										const maPoint3d& i_Position, 
										const maAxisBox& i_Bounds,
									    const maPoint3d& i_WorldPivot, 
									    const camCamera& i_Camera,
									    float i_ScalingFactor );

private:

	//--------------------------------------------------------------------
	//	Create_Block creates a object
	//--------------------------------------------------------------------
	cmpsObjectSimple* Create_Block( const maFloatRGBA& i_Color, 
		const float i_XSize, const float i_YSize, const float i_ZSize,
		cmpsRenderLayer::RenderLayer i_RenderLayer);

	//--------------------------------------------------------------------
	//	Resize the compass based on the given camera view
	//--------------------------------------------------------------------
	void ResizeObject(int i_IconLayerIndex, const camCamera& i_Camera );
};
