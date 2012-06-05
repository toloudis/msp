/********************************************************************************************\
**  tmlnBlendDriver.hpp
**
**		Interface for getting aspects of a driver's begin and end values
**	and gradients in order to blend smoothly.
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef TMLN_BLENDDRIVER_HPP
#error tmlnBlendDriver.hpp multiply included
#endif
#define TMLN_BLENDDRIVER_HPP

#ifndef G3D_CONSTANTS_HPP
#include "Graphics/g3d/g3dConstants.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif


//============================================================================
//============================================================================
class tmlnChannel;


//============================================================================
//============================================================================
template<class V>
class tmlnBlendDriver
{
public:
	//--------------------------------------------------------------------
	// Get the value of the driver at its begin time for this channel.
	//--------------------------------------------------------------------
	virtual void GetBeginValue(tmlnChannel* i_pChannel, V& o_Value) = 0;

	//--------------------------------------------------------------------
	// Get the value of the driver at its end time for this channel.
	//--------------------------------------------------------------------
	virtual void GetEndValue(tmlnChannel* i_pChannel, V& o_Value) = 0;

	//--------------------------------------------------------------------
	// Get the value of the gradient of the driver at its
	// end time in order to maintain tangent continuity while blending.
	//--------------------------------------------------------------------
	virtual void GetEndGradient(tmlnChannel* i_pChannel, V& o_Gradient) = 0;

	//--------------------------------------------------------------------
	// Blend from Val1 to Val2 with cublic spline. Requires gradients
	// at the two value points. i_U is a value from 0 to 1 for the
	// percentage from Val1 to Val2.
	// This function is not used as is, but is included as reference
	// for the cubic blend that we compute. The components related
	// to Val1 and Val2 are handled in the base class 
	// tmlnDriver::GetBlendAlpha() when the blend is SmoothBlend.
	// The gradient components are added in later in drivers that
	// support gradients through the ComputeGradientInfluence()
	// function below.
	//--------------------------------------------------------------------
	V DoCubicBlend(const V& i_Val1, const V& i_Val2,
				   const V& i_Gradient1, const V& i_Gradient2,
				   float i_U)
	{
		float u2 = i_U*i_U;
		float u3 = u2*i_U;

		return (i_Val1 * (2*u3 - 3*u2 + 1) +
			    i_Val2 * (3*u2 - 2*u3) +
				i_Gradient1 * (u3 - 2*u2 + i_U) +
				i_Gradient2 * (u3 - u2));
	}

	//--------------------------------------------------------------------
	// Convenience function for drivers that are static keys.
	// It computes the influence on the interpolated value
	// based on the gradients of the current and previous drivers.
	// This should be called with BlendType == SmoothBlend when the
	// time is between two drivers (i.e before the key's BeginTime).
	// i_Time is the simulation time
	// i_BeginTime is driver's begin time
	// i_EndTime is driver's end time
	// i_KeyValue is the static key value of the driver
	// io_ComputedValue is added to and is assumed to be the regular
	//	interpolation computed from using tmlnDriver::GetBlendAlpha().
	// i_pBeginGradient can be used to give the entry gradient for the
	//	driver that is being eased into. If left to NULL, this driver
	//  will be treated as a static key and will use ComputeKeyGradient().
	//--------------------------------------------------------------------
	void AddGradientInfluence(tmlnChannel* i_pChannel,
							   float i_Time,
							   float i_BeginTime,
							   float i_EndTime,
							   float i_EaseInWeight,
							   const V& i_KeyValue,
							   V &io_ComputedValue,
							   V* i_pBeginGradient = NULL)
	{
		// Find a previous driver, see if it has a gradient
		tmlnDriver *pPrevDriver = i_pChannel->GetPreviousDriver(i_Time);
		if (pPrevDriver)
		{
			float diff = (i_BeginTime - pPrevDriver->GetEndTime());
			if (diff > 0) // prevent division by zero
			{
				float u = (i_Time - pPrevDriver->GetEndTime()) / diff;

				tmlnBlendDriver<V>* pPrevBlend = dynamic_cast<tmlnBlendDriver<V>*>(pPrevDriver);
				if (pPrevBlend)
				{
					V end_gradient;
					pPrevBlend->GetEndGradient(i_pChannel, end_gradient);
					io_ComputedValue += end_gradient * ((u*u*u - 2*u*u + u) * diff) * pPrevDriver->GetEaseOutWeight();
				}

				if (i_pBeginGradient != NULL)
				{
					io_ComputedValue += (*i_pBeginGradient) * ((u*u*u - u*u) * diff) * i_EaseInWeight;
				}
				else
				{
					// Compute gradient for this driver
					V begin_gradient;
					if (this->ComputeKeyGradient(i_pChannel, 
												i_BeginTime, 
												i_EndTime, 
												i_KeyValue, 
												begin_gradient))
					{
						io_ComputedValue += begin_gradient * ((u*u*u - u*u) * diff) * i_EaseInWeight;
					}
				}
			}
		}
	}
	
	//--------------------------------------------------------------------
	// This function is designed for drivers that have zero length.
	// In this case the tangent is computed from the values
	// of the drivers on either side of the key.
	// i_Time and i_Value are from the single frame key.
	// Returns false if there are not any neighboring drivers, 
	// the tangent could not be computed.
	//--------------------------------------------------------------------
	bool ComputeKeyGradient(tmlnChannel* i_pChannel, 
							float i_BeginTime,
							float i_EndTime,
							const V& i_Value,
							V& o_Gradient)
	{
		// Constructing our tangent depends on the length of the static key
		const float c_ShortKeyDuration = 1 / g3dConstants::c_fDefaultFrameRate;
		if (i_EndTime-i_BeginTime < c_ShortKeyDuration)
		{
			// If we have a short key, then compute vector from
			// the previous and next values
			bool bHaveGradient = false;

			// Find a previous driver, get its end value
			tmlnDriver *pPrevDriver = i_pChannel->GetPreviousDriver(i_BeginTime);
			if (pPrevDriver)
			{
				float time_diff = (i_BeginTime - pPrevDriver->GetEndTime());
				tmlnBlendDriver<V>* pPrevBlend = dynamic_cast<tmlnBlendDriver<V>*>(pPrevDriver);
				if (pPrevBlend && (time_diff > 0))
				{
					V prev(i_Value);
					pPrevBlend->GetEndValue(i_pChannel, prev);
					o_Gradient += (i_Value - prev) * (0.5f / time_diff);
					bHaveGradient = true;
				}
			}

			// Find a next driver, get its begin value
			tmlnDriver *pNextDriver = i_pChannel->GetNextDriver(i_EndTime);
			if (pNextDriver)
			{
				float time_diff = (pNextDriver->GetBeginTime() - i_EndTime);
				tmlnBlendDriver<V>* pNextBlend = dynamic_cast<tmlnBlendDriver<V>*>(pNextDriver);
				if (pNextBlend && (time_diff > 0))
				{
					V next(i_Value);
					pNextBlend->GetBeginValue(i_pChannel, next);
					o_Gradient += (next - i_Value) * (0.5f / time_diff);
					bHaveGradient = true;
				}
			}
			
			return bHaveGradient;
		}
		return false;
	}
};


namespace tmlnBlendDriverOrientation
{
	//--------------------------------------------------------------------
	// Do spherical cubic interpolation for orientation channels
	//--------------------------------------------------------------------
	maRotation DoSquadBlend(tmlnChannel* i_pChannel,
								float i_Time,
								float i_BeginTime,
								float i_EndTime,
								const maRotation& i_Goal,
								const maRotation& i_Current,
								maRotation* i_pBeginGradient = NULL);

	//--------------------------------------------------------------------
	// Special gradient computation for orientation, which
	//	prepares for use in Squad - Spherical Cubic Interpolation
	//--------------------------------------------------------------------
	void ComputeOrientationGradient(tmlnChannel* i_pChannel,  
								float i_BeginTime,
								float i_EndTime,
								const maRotation& i_Value,
								maRotation& o_Gradient);

}

