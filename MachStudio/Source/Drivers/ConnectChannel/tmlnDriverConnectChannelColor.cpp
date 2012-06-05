/*****************************************************************************
**	tmlnDriverConnectChannelColor.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelColor.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverConnectChannelColor::tmlnDriverConnectChannelColor(tmlnChannelColor &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverConnectChannelTemplate<tmlnChannelColor>(i_Channel, i_ChunkName)
{
}

//--------------------------------------------------------------------
//  Update the value of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverConnectChannelColor::Operate(const maTime& i_Time)
{
	//	check to make sure master name and channel are valid
	//	if not, try to correct it.
	//
	CheckMasterNameAndChannel();

	//	if master channel grab its value
	if (m_pMasterChannel)
	{
		maFloatRGBA goal = m_pMasterChannel->GetColor();

		// Check and handle blend into driver
		//
		if (IsBefore(i_Time))
		{
			maFloatRGBA cur = m_Channel.GetColor();

			float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
			maFloatRGBA color = goal*percent + cur*(1.0f - percent);	// linear blend
			m_Channel.SetColor(color);
		}
		else
		{
			// Within driver range
			//
			m_Channel.SetColor(goal);
		}
	}

	// Because the master channel might change at any time without telling us,
	// we need to always operate
	this->MarkDirty();
}
