/*****************************************************************************
**	tmlnDriverConnectChannelOrientation.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelOrientation.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverConnectChannelOrientation::tmlnDriverConnectChannelOrientation(tmlnChannelOrientation &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverConnectChannelTemplate<tmlnChannelOrientation>(i_Channel, i_ChunkName)
{
}

//--------------------------------------------------------------------
//  Update the value of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverConnectChannelOrientation::Operate(const maTime& i_Time)
{
	//	check to make sure master name and channel are valid
	//	if not, try to correct it.
	//
	CheckMasterNameAndChannel();

	//	if master channel grab its value
	if (m_pMasterChannel)
	{
		maRotation goal = m_pMasterChannel->GetQuaternion();

		// Check and handle blend into driver
		if (IsBefore(i_Time))
		{
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
			float x=0, y=0, z=0;
			if (m_pMasterChannel)
				m_pMasterChannel->GetEuler(x,y,z);
			m_Channel.SetEuler(x,y,z);
		}
	}

	// Because the master channel might change at any time without telling us,
	// we need to always operate
	this->MarkDirty();
}

//--------------------------------------------------------------------
// Get the value of the driver at its begin time for this channel.
//--------------------------------------------------------------------
void tmlnDriverConnectChannelOrientation::GetBeginValue(tmlnChannel* i_pChannel, maRotation& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Channel.GetQuaternion();
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void tmlnDriverConnectChannelOrientation::GetEndValue(tmlnChannel* i_pChannel, maRotation& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Channel.GetQuaternion();
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void tmlnDriverConnectChannelOrientation::GetEndGradient(tmlnChannel* i_pChannel, maRotation& o_Gradient)
{
	tmlnBlendDriverOrientation::ComputeOrientationGradient(i_pChannel, 
		this->GetBeginTime(), 
		this->GetEndTime(), 
		m_Channel.GetQuaternion(), 
		o_Gradient);
}
