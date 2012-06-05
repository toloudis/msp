/*****************************************************************************
**	tmlnDriverConnectChannelPosition.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelPosition.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverConnectChannelPosition::tmlnDriverConnectChannelPosition(tmlnChannelPosition &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverConnectChannelTemplate<tmlnChannelPosition>(i_Channel, i_ChunkName)
{
}

//--------------------------------------------------------------------
//  Update the value of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverConnectChannelPosition::Operate(const maTime& i_Time)
{
	//	check to make sure master name and channel are valid
	//	if not, try to correct it.
	//
	CheckMasterNameAndChannel();

	//	if master channel grab its value
	if (m_pMasterChannel)
	{
		maPoint3d goal = m_pMasterChannel->GetPosition();

		// Check and handle blend into driver
		if (IsBefore(i_Time))
		{
			maPoint3d cur = m_Channel.GetPosition();
			float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
			maPoint3d pos = goal*percent + cur*(1.0f - percent);

			// If smooth blend, add in influence of the gradients
			if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
			{
				AddGradientInfluence(&m_Channel, i_Time, 
					this->GetBeginTime(), this->GetEndTime(), this->GetEaseInWeight(),
					goal, pos);
			}

			m_Channel.SetPosition(pos);
		}
		else
		{
			m_Channel.SetPosition(goal);
		}
	}

	// Because the master channel might change at any time without telling us,
	// we need to always operate
	this->MarkDirty();
}

//--------------------------------------------------------------------
// Get the value of the driver at its begin time for this channel.
//--------------------------------------------------------------------
void tmlnDriverConnectChannelPosition::GetBeginValue(tmlnChannel* i_pChannel, maPoint3d& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Channel.GetPosition();
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void tmlnDriverConnectChannelPosition::GetEndValue(tmlnChannel* i_pChannel, maPoint3d& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Channel.GetPosition();
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void tmlnDriverConnectChannelPosition::GetEndGradient(tmlnChannel* i_pChannel, maPoint3d& o_Gradient)
{
	this->ComputeKeyGradient(i_pChannel, 
		this->GetBeginTime(), 
		this->GetEndTime(), 
		m_Channel.GetPosition(), 
		o_Gradient);
}
