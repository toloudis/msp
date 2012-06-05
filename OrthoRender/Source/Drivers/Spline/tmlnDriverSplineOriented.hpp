/*****************************************************************************
**	tmlnDriverSplineOriented.hpp
**
**		Derived driver class which implements Spline motion with orientation
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_DRIVERSPLINEORIENTED_HPP
#error tmlnDriverSplineOriented.hpp multiply included
#endif
#define TMLN_DRIVERSPLINEORIENTED_HPP

#ifndef TMLN_DRIVERSPLINE_HPP
#include "Drivers/Spline/tmlnDriverSpline.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif


//============================================================================
//============================================================================
class tmlnChannelOrientation;
class tmlnChannelPosition;
class tmlnDriverSplineInfo;
class splnSpline;


//============================================================================
//============================================================================
class tmlnDriverSplineOriented : public tmlnDriverSpline
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverSplineOriented(tmlnChannelPosition &i_ChannelP, 
							 tmlnChannelOrientation &i_ChannelO, 
							 chDefs::Name i_ChunkName, 
							 pick3dPickObject* i_pParent);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~tmlnDriverSplineOriented();

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

private:
	//--------------------------------------------------------------------
	// calculate the orientation based on the tangent
	//--------------------------------------------------------------------
	maRotation calculate_orientation_goal( maPoint3d& i_Tangent );

	tmlnChannelOrientation&	m_ChannelO;
};
