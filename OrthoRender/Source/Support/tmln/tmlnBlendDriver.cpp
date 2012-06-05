/*****************************************************************************
**	tmlnBlendDriver.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnBlendDriver.hpp"

#include "Support/tmln/tmlnChannel.hpp"
#include "Support/tmln/tmlnDriver.hpp"

#include "Core/ma/maFunctions.hpp"


//--------------------------------------------------------------------
// Do spherical cubic interpolation for orientation channels
//--------------------------------------------------------------------
maRotation tmlnBlendDriverOrientation::DoSquadBlend(tmlnChannel* i_pChannel,
							   float i_Time,
							   float i_BeginTime,
							   float i_EndTime,
							   const maRotation& i_Goal,
							   const maRotation& i_Current,
							   maRotation* i_pBeginGradient)
{		
	maRotation prev(i_Current);
	tmlnDriver *pPrevDriver = i_pChannel->GetPreviousDriver(i_Time);
	if (pPrevDriver)
	{
		tmlnBlendDriver<maRotation>* pPrevBlend = dynamic_cast<tmlnBlendDriver<maRotation>*>(pPrevDriver);
		if (pPrevBlend)
		{
			pPrevBlend->GetEndGradient(i_pChannel, prev);
		}
	}

	maRotation next(i_Goal);
	if (i_pBeginGradient != NULL)
	{
		next = (*i_pBeginGradient);
	}
	else
	{
		ComputeOrientationGradient(i_pChannel, 
			i_BeginTime, i_EndTime, i_Goal, next);
	}

	// Spherical Cubic Blend
	float prev_time = i_pChannel->GetPreviousTime(i_Time);
	float blend_time = i_BeginTime - prev_time;
	if (blend_time > 0.0f)
	{
		float alpha = (i_Time - prev_time) / blend_time;
		maFunctions::Clamp(alpha, 0.0f, 1.0f);
		maRotation ori = maRotation::Squad(i_Current, prev, next, i_Goal, alpha);
		return ori;	
	}
	return i_Goal;
}

//--------------------------------------------------------------------
// Special gradient computation for orientation, which
//	prepares for use in Squad - Spherical Cubic Interpolation
//--------------------------------------------------------------------
void tmlnBlendDriverOrientation::ComputeOrientationGradient(tmlnChannel* i_pChannel,  
											 float i_BeginTime,
											 float i_EndTime,
											 const maRotation& i_Value,
											 maRotation& o_Gradient)
{		
	// Constructing our tangent depends on the length of the static key
	const float c_ShortKeyDuration = 1 / g3dConstants::c_fDefaultFrameRate;
	if (i_EndTime-i_BeginTime < c_ShortKeyDuration)
	{
		// If we have a short key, then compute vector from
		// the previous and next values
		bool bHaveGradient = false;

		maRotation value(i_Value);
		maRotation prev(value), next(value);

		// Find a previous driver, get its end value
		tmlnDriver *pPrevDriver = i_pChannel->GetPreviousDriver(i_BeginTime);
		if (pPrevDriver)
		{
			//float time_diff = (this->GetBeginTime() - pPrevDriver->GetEndTime());
			tmlnBlendDriver<maRotation>* pPrevBlend = dynamic_cast<tmlnBlendDriver<maRotation>*>(pPrevDriver);
			if (pPrevBlend)
			{
				pPrevBlend->GetEndValue(i_pChannel, prev);
				bHaveGradient = true;
			}
		}

		// Find a next driver, get its begin value
		tmlnDriver *pNextDriver = i_pChannel->GetNextDriver(i_EndTime);
		if (pNextDriver)
		{
			//float time_diff = (pNextDriver->GetBeginTime() - i_EndTime);
			tmlnBlendDriver<maRotation>* pNextBlend = dynamic_cast<tmlnBlendDriver<maRotation>*>(pNextDriver);
			if (pNextBlend)
			{
				pNextBlend->GetBeginValue(i_pChannel, next);
				bHaveGradient = true;
			}
		}

		if (bHaveGradient)
		{
			// Need to incorporate time difference somehow
			o_Gradient = maRotation::ComputeInnerPoint(prev, value, next);
		}
	}
}

