/*****************************************************************************
**  cmpsCompassObjectTranslate.hpp
**
**      cmpsCompassObjectTranslate is a geometric object used to display axis
**	for Translate.
**
**	StudioGPU
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
	cmpsCompassObjectTranslate(cmpsRenderLayer::RenderLayer i_RenderLayer);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmpsCompassObjectTranslate();

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
	//	Create_Cone creates a object
	//--------------------------------------------------------------------
	cmpsObjectSimple* Create_Cone( const maFloatRGBA& i_Color,
								   const maRotation& i_AxisRot,
								   const maVector3d& i_AxisPos,
								   cmpsRenderLayer::RenderLayer i_RenderLayer );
};
