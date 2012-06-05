/*****************************************************************************
**	tmlnDriverSplineOriented.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Drivers/Spline/tmlnDriverSplineOriented.hpp"

#include "Support/spln/splnSpline.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Drivers/Spline/tmlnDriverSplineInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverSplineOriented::tmlnDriverSplineOriented(tmlnChannelPosition &i_ChannelP, 
												   tmlnChannelOrientation &i_ChannelO, 
												   chDefs::Name i_ChunkName, 
												   pick3dPickObject* i_pParent)
:	tmlnDriverSpline( i_ChannelP, i_ChunkName, i_pParent ),
	m_ChannelO(i_ChannelO)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverSplineOriented::~tmlnDriverSplineOriented()
{
	//splnCurveMgr::DeleteCurve(m_pSpline);
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverSplineOriented::Operate(float i_Time)
{
	// Let the base class handle the position channel
	tmlnDriverSpline::Operate(i_Time);

	maPoint3d tangent;

	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		m_pSpline->Evaluate(0.0f, &tangent);
		maRotation ogoal = calculate_orientation_goal( tangent );
		maRotation ocur = m_ChannelO.GetQuaternion();
		float opercent = this->GetBlendAlpha(m_ChannelO.GetPreviousTime(i_Time), i_Time);
		//maRotation ori = ogoal*opercent + ocur*(1.0f - opercent);	// linear blend
		maRotation ori;
		ori.Slerp(ocur, ogoal, opercent);
		m_ChannelO.SetQuaternion(ori);
	}
	else
	{
		// within driver range
		float percent = (i_Time - this->GetBeginTime()) / this->GetDuration();
		m_pSpline->Evaluate(percent, &tangent);
		maRotation ogoal = calculate_orientation_goal( tangent );
		m_ChannelO.SetQuaternion(ogoal);
	}
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverSplineOriented::GetClipFillColor() const
{
	return maFloatRGBA( 0.9643f, 0.7525f, 0.5369f, 1.0f );
}

//--------------------------------------------------------------------
// calculate the orientation based on the tangent
//--------------------------------------------------------------------
maRotation tmlnDriverSplineOriented::calculate_orientation_goal( maPoint3d& i_Tangent )
{
	maRotation orientation;

	// calculate the orientation based on the tangent
	//
	//	Get "pitch" and "yaw" from the tangent using atan2, etc. and 
	//	then reform a rotation from these.  This will keep the object 
	//	upright (like a bird), and you can enforce certain limits on 
	//	the pitch if you want.  This decays if the tangent points 
	//	straight up however.
	//

	float yaw, pitch, roll;
	yaw		= atan2( i_Tangent.GetX(), i_Tangent.GetZ() );
	pitch	= asin( -i_Tangent.GetY() );
	roll	= 0.0f;
	orientation.SetEuler( pitch, yaw, roll );

	return orientation;
}

