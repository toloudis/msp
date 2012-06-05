/*****************************************************************************
**	cmraDriverKeyTarget.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverKeyTarget.hpp"

#include "Core/ma/maFunctions.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyUnits.hpp"
#include "Support/mnm/mnmObject.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Systems/Cameras/Drivers/cmraDriverKeyTargetInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverKeyTargetParser.hpp"

#include <sstream>
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraDriverKeyTarget::cmraDriverKeyTarget(tmlnChannelPosition &i_ChannelTarget)
:	m_ChannelTarget(i_ChannelTarget),
	m_CameraKeyTarget( "Key Target", i_ChannelTarget.GetPosition() )
{
	this->SetBlendType(tmlnDriver::e_Previous); //GetDefaultBlendType()); 

	//	set-up the UIInfos
	prtyVector3dEditUIInfo* pPUII;
	pPUII = new prtyVector3dEditUIInfo(&(m_CameraKeyTarget), "Camera", "Camera target position");
	AddProperty( pPUII );

	//	property callbacks
	m_CameraKeyTarget.AddCallback(new prtyCallbackWrapper<cmraDriverKeyTarget>(this, &cmraDriverKeyTarget::PropertyChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraDriverKeyTarget::~cmraDriverKeyTarget()
{
}

//----------------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//----------------------------------------------------------------------------
std::string cmraDriverKeyTarget::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	//char buffer[128];
	//::sprintf(buffer, "Tar: (%.3f, %.3f, %.3f)", m_CameraKeyTarget.GetValue().GetX(), m_CameraKeyTarget.GetValue().GetY(), m_CameraKeyTarget.GetValue().GetZ() );
	//desc += std::string(buffer);

	float unit_scale = 1.0f; 
	if( m_CameraKeyTarget.GetUseUnits() )
		unit_scale = prtyUnits::GetUnitScaling();
	
	std::ostringstream desc_stream(std::ostringstream::out);
	desc_stream.setf(std::ios::fixed, std::ios::floatfield);
	desc_stream.precision(3);

	desc_stream << "Tar: (" << m_CameraKeyTarget.GetValue().GetX() / unit_scale << ", " 
								<< m_CameraKeyTarget.GetValue().GetY() / unit_scale << ", " 
								<< m_CameraKeyTarget.GetValue().GetZ() / unit_scale << ")";

	desc += desc_stream.str();
	return desc;
}

//----------------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//----------------------------------------------------------------------------
tmlnDriverInfo*  cmraDriverKeyTarget::GetDriverInfo() const
{
	cmraDriverKeyTargetInfo *pInfo = new cmraDriverKeyTargetInfo(cmraDriverKeyTargetParser::GetChunkName());

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_CameraKeyTarget	= this->m_CameraKeyTarget.GetValue();

	return pInfo;
}

//----------------------------------------------------------------------------
// Set internal variables from data structure
//----------------------------------------------------------------------------
void cmraDriverKeyTarget::SetDriverInfo(const cmraDriverKeyTargetInfo& i_Info)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// Store local info
	this->m_CameraKeyTarget.SetValue( i_Info.m_CameraKeyTarget );
}

//----------------------------------------------------------------------------
//  Update position of things that are being driven
//----------------------------------------------------------------------------
void  cmraDriverKeyTarget::Operate(const maTime& i_Time)
{
	OperateChannel( i_Time, m_ChannelTarget, m_CameraKeyTarget.GetValue() );
}

//--------------------------------------------------------------------
//	AlterKey - look at the values in the channels to which this 
//	driver is connected and alter the driver in order to match 
//	these values.
//	Returns true if this driver was able to alter its value.
//--------------------------------------------------------------------
bool cmraDriverKeyTarget::AlterKey()
{
	// Store local info, using undo
	this->CreateUndoForProperty(m_CameraKeyTarget);
	const bool bSetDirty = true;
	this->m_CameraKeyTarget.SetValue( m_ChannelTarget.GetPosition(), bSetDirty  );
	return true;
}

//----------------------------------------------------------------------------
//	Perform the operate on a single channel
//----------------------------------------------------------------------------
void cmraDriverKeyTarget::OperateChannel( const maTime& i_Time, tmlnChannelPosition& i_Channel, const maPoint3d& i_Goal )
{
	// Check and handle blend into driver
	//
	if (IsBefore(i_Time))
	{
		maPoint3d goal = i_Goal;
		maPoint3d cur = i_Channel.GetPosition();

		maTime prevtime = i_Channel.GetPreviousTime(i_Time);
		float percent = this->GetBlendAlpha( prevtime, i_Time );
		//maFunctions::Clamp( percent, 0.0f, 100.0f );

		maPoint3d pos = goal*percent + cur*(1.0f - percent);	// linear blend

		//DBG_LOG3( "camkey TIME = %9.4f prev(%9.4f)  percent(%6.3f%%)", i_Time, prevtime, percent );
		//DBG_LOG3( "  curr (%6.2f,%6.2f,%6.2f)", cur.GetX(), cur.GetY(), cur.GetZ() );
		//DBG_LOG3( "  goal (%6.2f,%6.2f,%6.2f)", goal.GetX(), goal.GetY(), goal.GetZ() );
		//DBG_LOG3( "  pos  (%6.2f,%6.2f,%6.2f)", pos.GetX(), pos.GetY(), pos.GetZ() );
	
		// If smooth blend, add in influence of the gradients
		if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
		{
			AddGradientInfluence(&i_Channel, i_Time, 
				this->GetBeginTime(), this->GetEndTime(), this->GetEaseInWeight(),
				goal, pos);
		}

		i_Channel.SetPosition(pos);
	}
	else
	{
		// within driver range
		//
		//float percent = (i_Time - this->GetBeginTime()) / this->GetDuration();

		i_Channel.SetPosition( i_Goal );
	}
}

//--------------------------------------------------------------------
//	Set the target
//--------------------------------------------------------------------
void cmraDriverKeyTarget::SetTarget( maPoint3d& i_Target )
{
	this->m_CameraKeyTarget.SetValue( i_Target );
	this->MarkDirty();
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA cmraDriverKeyTarget::GetClipFillColor() const
{
	return maFloatRGBA( 0.4322f, 0.8784f, 0.9f, 1.0f );
}

//--------------------------------------------------------------------
// Get the value of the driver at its begin time for this channel.
//--------------------------------------------------------------------
void cmraDriverKeyTarget::GetBeginValue(tmlnChannel* i_pChannel, maVector3d& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_CameraKeyTarget.GetValue();
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void cmraDriverKeyTarget::GetEndValue(tmlnChannel* i_pChannel, maVector3d& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_CameraKeyTarget.GetValue();
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void cmraDriverKeyTarget::GetEndGradient(tmlnChannel* i_pChannel, maVector3d& o_Gradient)
{
	this->ComputeKeyGradient(i_pChannel, 
		this->GetBeginTime(), 
		this->GetEndTime(), 
		m_CameraKeyTarget.GetValue(), 
		o_Gradient);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverKeyTarget::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	this->MarkDirty();
}
