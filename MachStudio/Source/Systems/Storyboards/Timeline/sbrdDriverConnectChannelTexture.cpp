/*****************************************************************************
**	sbrdDriverConnectChannelTexture.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "sbrdDriverConnectChannelTexture.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
sbrdDriverConnectChannelTexture::sbrdDriverConnectChannelTexture(sbrdChannelTexture &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverConnectChannelTemplate<sbrdChannelTexture>(i_Channel, i_ChunkName)
{
}

//--------------------------------------------------------------------
//  Update the value of things that are being driven
//--------------------------------------------------------------------
void  sbrdDriverConnectChannelTexture::Operate(float i_Time)
{
	// Execute camera script
	if (m_Textures.size())
	{	
		// If the blend settings say no blend, and time is less than our begin time,
		// leave the old texture in place.
		if (i_Time < this->GetBeginTime() && this->GetBlendType() == tmlnDriver::e_NoBlending)
		{
			return;
		}


		int frame = compute_frame(i_Time);
		m_ChannelTexture.SetTexture(m_Textures[frame]);
	}

////
	int goal = 0;
	if (m_pMasterChannel)
		goal = m_pMasterChannel->GetValue();

	// Check and handle blend into driver
	if (i_Time < this->GetBeginTime())
	{
		float cur = m_Channel.GetValue();
		float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
		float value = goal*percent + cur*(1.0f - percent);	// linear blend

		// If smooth blend, add in influence of the gradients
		if (this->GetBlendType() == sbrdDriver::e_SmoothBlend)
		{
			AddGradientInfluence(&m_Channel, i_Time, 
				this->GetBeginTime(), this->GetEndTime(),
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
