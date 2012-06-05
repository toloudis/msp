/*****************************************************************************
**	tmlnDriverConnectChannelFloat.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelFloat.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverConnectChannelFloat::tmlnDriverConnectChannelFloat(tmlnChannelFloat &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverConnectChannelTemplate<tmlnChannelFloat>(i_Channel, i_ChunkName)
{
}

//--------------------------------------------------------------------
//  Update the value of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverConnectChannelFloat::Operate(float i_Time)
{
	//	check to make sure master name and channel are valid
	//	if not, try to correct it.
	//
	CheckMasterNameAndChannel();

	//	if master channel grab its value
	float goal = 0.0f;
	if (m_pMasterChannel)
		goal = m_pMasterChannel->GetValue();

	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		float cur = m_Channel.GetValue();
		float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
		float value = goal*percent + cur*(1.0f - percent);	// linear blend

		// If smooth blend, add in influence of the gradients
		if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
		{
			AddGradientInfluence(&m_Channel, i_Time, 
				this->GetBeginTime(), this->GetEndTime(), this->GetEaseInWeight(),
				goal, value);
		}

		m_Channel.SetValue(value);
	}
	else
	{
		// within driver range
		m_Channel.SetValue(goal);
	}
}

//--------------------------------------------------------------------
// Get the value of the driver at its begin time for this channel.
//--------------------------------------------------------------------
void tmlnDriverConnectChannelFloat::GetBeginValue(tmlnChannel* i_pChannel, float& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Channel.GetValue();
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void tmlnDriverConnectChannelFloat::GetEndValue(tmlnChannel* i_pChannel, float& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Channel.GetValue();
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void tmlnDriverConnectChannelFloat::GetEndGradient(tmlnChannel* i_pChannel, float& o_Gradient)
{
	this->ComputeKeyGradient(i_pChannel, 
		this->GetBeginTime(), 
		this->GetEndTime(), 
		m_Channel.GetValue(), 
		o_Gradient);
}

