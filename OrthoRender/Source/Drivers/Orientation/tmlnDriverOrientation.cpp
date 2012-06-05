/*****************************************************************************
**	tmlnDriverOrientation.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Drivers/Orientation/tmlnDriverOrientation.hpp"

#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Drivers/Orientation/tmlnDriverOrientationInfo.hpp"
//#include "tmlnDriverOrientationForm.h"	// GUI

#include "Core/dbg/dbgLog.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverOrientation::tmlnDriverOrientation(tmlnChannelOrientation &i_Channel, 
											 chDefs::Name i_ChunkName)
:	m_Channel(i_Channel), 
	m_Value("Orientation"), 
	m_bEulerInterpolation("Euler Interpolation", false),
	m_ChunkName(i_ChunkName)
{
	this->SetBlendType(tmlnDriver::GetDefaultBlendType());

	// Set initial value from euler angles
	float x = 0, y = 0, z = 0;
	m_Channel.GetEuler(x, y, z);
	m_Value.SetEuler(x, y, z);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyVector3dEditUIInfo(&(m_Value), "Value", "Orientation");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_bEulerInterpolation), "Interpolation", 
				"Interpolate Euler angles directly, do not convert to quaternions.");
	AddProperty( pPUII );

	// Register callbacks to update dirty bit when properties change
	m_Value.AddCallback(new prtyCallbackWrapper<tmlnDriverOrientation>(this, &tmlnDriverOrientation::PropertyChanged));
	m_bEulerInterpolation.AddCallback(new prtyCallbackWrapper<tmlnDriverOrientation>(this, &tmlnDriverOrientation::PropertyChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverOrientation::~tmlnDriverOrientation()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string tmlnDriverOrientation::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	float xAngle = 0, yAngle = 0, zAngle = 0;
	m_Value.GetEuler(xAngle, yAngle, zAngle);
	xAngle = ::floorf(xAngle * maConstants::c_fRadToAngle);
	yAngle = ::floorf(yAngle * maConstants::c_fRadToAngle);
	zAngle = ::floorf(zAngle * maConstants::c_fRadToAngle);

	char buffer[128];
	::sprintf(buffer, "Orientation: %.0f %.0f %.0f", xAngle, yAngle, zAngle);

	desc += std::string(buffer);
	return desc;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//void tmlnDriverOrientation::SetValue(const maRotation& i_Val)
//{
//	m_Value.SetValue(i_Val);
//	this->MarkDirty();
//}
void tmlnDriverOrientation::SetEulerInterpolation(bool i_bVal)
{
	m_bEulerInterpolation.SetValue(i_bVal);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverOrientation::GetDriverInfo() const
{
	tmlnDriverOrientationInfo *pInfo = new tmlnDriverOrientationInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	this->m_Value.GetEuler( pInfo->m_X, pInfo->m_Y, pInfo->m_Z );
	pInfo->m_bEulerInterpolation = this->m_bEulerInterpolation.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void tmlnDriverOrientation::SetDriverInfo(	const tmlnDriverOrientationInfo& i_Info, 
											prtyProperty::UndoFlags i_Undoable)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	// Store local info
	this->m_Value.SetEuler( i_Info.m_X, i_Info.m_Y, i_Info.m_Z, i_Undoable);
	this->m_bEulerInterpolation.SetValue( i_Info.m_bEulerInterpolation, i_Undoable );
}

//--------------------------------------------------------------------
//  Update orientation of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverOrientation::Operate(float i_Time)
{
	if (this->m_bEulerInterpolation.GetValue())
		OperateEuler(i_Time);
	else
		OperateQuaternion(i_Time);
}
void  tmlnDriverOrientation::OperateQuaternion(float i_Time)
{
	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		maRotation goal = this->m_Value.GetQuaternion();
		maRotation cur = m_Channel.GetQuaternion();

		// If smooth blend, add in influence of the gradients
		if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
		{
			maRotation ori = tmlnBlendDriverOrientation::DoSquadBlend(&m_Channel,
				i_Time, this->GetBeginTime(), this->GetEndTime(), goal, cur);
			m_Channel.SetQuaternion(ori);	
		}
		else
		{
			// Linear blend
			float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
			//maRotation ori = goal*percent + cur*(1.0f - percent);	// linear blend
			maRotation ori;
			ori.Slerp(cur, goal, percent);
			m_Channel.SetQuaternion(ori);	
		}
	}
	else 
	{
		// within driver range
		//m_Channel.SetQuaternion(this->m_Value.GetQuaternion());
		
		// Within driver range, set euler angles so that the
		// user interface is consistent. Only use quaternions
		// when interpolating
		float x=0, y=0, z=0;
		this->m_Value.GetEuler(x, y, z);
		m_Channel.SetEuler(x, y, z);
	}
}
void  tmlnDriverOrientation::OperateEuler(float i_Time)
{
	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		maVector3d goal, cur;
		this->m_Value.GetEuler(goal.m_X, goal.m_Y, goal.m_Z);
		m_Channel.GetEuler(cur.m_X, cur.m_Y, cur.m_Z);

		float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
		maVector3d angles = goal*percent + cur*(1.0f - percent);	

		// If smooth blend, add in influence of the gradients
		if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
		{
			tmlnBlendDriver<maVector3d>::AddGradientInfluence(&m_Channel, i_Time, 
				this->GetBeginTime(), this->GetEndTime(), this->GetEaseInWeight(),
				goal, angles);
		}

		m_Channel.SetEuler(angles.m_X, angles.m_Y, angles.m_Z);
	}
	else 
	{
		// within driver range
		float x=0, y=0, z=0;
		this->m_Value.GetEuler(x, y, z);
		m_Channel.SetEuler(x, y, z);
	}
}

//--------------------------------------------------------------------
//	AlterKey - look at the values in the channels to which this 
//	driver is connected and alter the driver in order to match 
//	these values.
//	Returns true if this driver was able to alter its value.
//--------------------------------------------------------------------
bool tmlnDriverOrientation::AlterKey()
{
	float x=0, y=0, z=0;
	m_Channel.GetEuler(x, y, z);

	// Store local info, using undo
	this->m_Value.SetEuler( x, y, z, prtyProperty::eNewUndo  );

	return true;
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverOrientation::GetClipFillColor() const
{
	if (this->m_bEulerInterpolation.GetValue())
		return maFloatRGBA( 0.8431f, 0.1412f, 0.8f, 1.0f );
	else
		return maFloatRGBA( 0.1412f, 0.8f, 0.8431f, 1.0f );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverOrientation::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}

//--------------------------------------------------------------------
// Get the value of the driver at its begin time for this channel.
//--------------------------------------------------------------------
void tmlnDriverOrientation::GetBeginValue(tmlnChannel* i_pChannel, maRotation& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Value.GetQuaternion();
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void tmlnDriverOrientation::GetEndValue(tmlnChannel* i_pChannel, maRotation& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Value.GetQuaternion();
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void tmlnDriverOrientation::GetEndGradient(tmlnChannel* i_pChannel, maRotation& o_Gradient)
{		
	tmlnBlendDriverOrientation::ComputeOrientationGradient(i_pChannel, 
		this->GetBeginTime(), 
		this->GetEndTime(), 
		m_Value.GetQuaternion(), 
		o_Gradient);
}

//--------------------------------------------------------------------
// Get the value of the driver at its begin time for this channel.
//--------------------------------------------------------------------
void tmlnDriverOrientation::GetBeginValue(tmlnChannel* i_pChannel, maVector3d& o_Value)
{
	// Single channel driver can ignore channel pointer
	m_Value.GetEuler(o_Value.m_X, o_Value.m_Y, o_Value.m_Z);
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void tmlnDriverOrientation::GetEndValue(tmlnChannel* i_pChannel, maVector3d& o_Value)
{
	// Single channel driver can ignore channel pointer
	m_Value.GetEuler(o_Value.m_X, o_Value.m_Y, o_Value.m_Z);
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void tmlnDriverOrientation::GetEndGradient(tmlnChannel* i_pChannel, maVector3d& o_Gradient)
{		
	maVector3d value;
	m_Value.GetEuler(value.m_X, value.m_Y, value.m_Z);

	tmlnBlendDriver<maVector3d>::ComputeKeyGradient(i_pChannel, 
		this->GetBeginTime(), 
		this->GetEndTime(), 
		value, 
		o_Gradient);
}


/* FOR REFERENCE: DIVISION VERSION


//--------------------------------------------------------------------
//  Update orientation of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverOrientation::Operate(float i_Time)
{
	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		maRotation goal = this->m_Value.GetValue();
		maRotation cur = m_Channel.GetOrientation();
		float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
		//maRotation ori = goal*percent + cur*(1.0f - percent);	// linear blend
		maRotation ori;
		ori.Slerp(cur, goal, percent);

		// If smooth blend, add in influence of the gradients
		if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
		{
			tmlnDriver *pPrevDriver = m_Channel.GetPreviousDriver(i_Time);
			if (pPrevDriver)
			{
				float diff = (this->GetBeginTime() - pPrevDriver->GetEndTime());
				if (diff > 0) // prevent division by zero
				{
					float u = (i_Time - pPrevDriver->GetEndTime()) / diff;

					tmlnBlendDriver<maRotation>* pPrevBlend = dynamic_cast<tmlnBlendDriver<maRotation>*>(pPrevDriver);
					if (pPrevBlend)
					{
						maRotation end_gradient;
						pPrevBlend->GetEndGradient(&m_Channel, end_gradient);
						end_gradient.ScaleAngle((u*u*u - 2*u*u + u) * diff);
						ori *= end_gradient;
					}
					
					// Compute gradient for this driver
					maRotation begin_gradient;
					this->GetEndGradient(&m_Channel, begin_gradient);
					begin_gradient.ScaleAngle((u*u*u - u*u) * diff);
					ori *= begin_gradient;
				}
			}
		}

		// Set the value
		m_Channel.SetOrientation(ori);	
	}
	else 
	{
		// within driver range
		m_Channel.SetOrientation(this->m_Value.GetValue());
	}
}


//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void tmlnDriverOrientation::GetEndGradient(tmlnChannel* i_pChannel, maRotation& o_Gradient)
{		
	// Constructing our tangent depends on the length of the static key
	const float c_ShortKeyDuration = 1 / g3dConstants::c_fDefaultFrameRate;
	if (this->GetDuration() < c_ShortKeyDuration)
	{
		// If we have a short key, then compute vector from
		// the previous and next values
		float begin_time = this->GetBeginTime();
		maRotation value(m_Value.GetValue());

		// Find a previous driver, get its end value
		tmlnDriver *pPrevDriver = i_pChannel->GetPreviousDriver(begin_time);
		if (pPrevDriver)
		{
			float time_diff = (begin_time - pPrevDriver->GetEndTime());
			tmlnBlendDriver<maRotation>* pPrevBlend = dynamic_cast<tmlnBlendDriver<maRotation>*>(pPrevDriver);
			if (pPrevBlend && (time_diff > 0))
			{
				maRotation prev(value), value_inv(value);
				pPrevBlend->GetEndValue(i_pChannel, prev);
				prev.Invert();
				maRotation prev_diff = value * prev; // multiply by inverse to do division
				prev_diff.ScaleAngle(0.5f / time_diff);	// scale based on time between keys
				o_Gradient *= prev_diff;
			}
		}

		// Find a next driver, get its begin value
		tmlnDriver *pNextDriver = i_pChannel->GetNextDriver(this->GetEndTime());
		if (pNextDriver)
		{
			float time_diff = (pNextDriver->GetBeginTime() - this->GetEndTime());
			tmlnBlendDriver<maRotation>* pNextBlend = dynamic_cast<tmlnBlendDriver<maRotation>*>(pNextDriver);
			if (pNextBlend && (time_diff > 0))
			{
				maRotation next(value), value_inv(value);
				pNextBlend->GetBeginValue(i_pChannel, next);
				value_inv.Invert();
				maRotation next_diff = next * value_inv; // multiply by inverse to do division
				next_diff.ScaleAngle(0.5f / time_diff);	// scale based on time between keys
				o_Gradient *= next_diff;
			}
		}
	}
}
*/