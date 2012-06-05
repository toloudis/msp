/*****************************************************************************
**  cmpsCompassObjectScale.hpp
**
**      cmpsCompassObjectScale is an geometric object used to display axis
**	for Scale.
**
**	StudioGPU
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
	cmpsCompassObjectScale(cmpsRenderLayer::RenderLayer i_RenderLayer);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmpsCompassObjectScale();

protected:
	//----------------------------------------------------------------------------
	//	GetDefaultSize() - get the default size of the shape
	//----------------------------------------------------------------------------
	virtual float GetDefaultSize() const;

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
	//	Create_Cylinder creates a object
	//--------------------------------------------------------------------
	cmpsObjectSimple* Create_Cylinder( const maFloatRGBA& i_Color, 
									   const maRotation& i_AxisRot,
									   const maVector3d& i_AxisPos,
									   cmpsRenderLayer::RenderLayer i_RenderLayer );
};
