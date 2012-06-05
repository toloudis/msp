/*****************************************************************************
**	chtrDriverConnectChannelAnimationFull.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "chtrDriverConnectChannelAnimationFull.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrDriverConnectChannelAnimationFull::chtrDriverConnectChannelAnimationFull(chtrChannelAnimationFull &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverConnectChannelTemplate<chtrChannelAnimationFull>(i_Channel, i_ChunkName)
{
}

//--------------------------------------------------------------------
//  Update the value of things that are being driven
//--------------------------------------------------------------------
void  chtrDriverConnectChannelAnimationFull::Operate(float i_Time)
{
	if (!m_pMasterChannel)
		return;

	// Check and handle blend into driver
	if (i_Time < this->GetBeginTime())
	{
		// TODO [rjk] implement the blend.

		float blend_time = 0.0f;
		//if (get_blend_time(i_Time, blend_time))
		{
//			m_Channel.BlendAnimation( m_pMasterChannel->GetAnimation(), this->GetBeginTime(), blend_time);
		}
		// otherwise, don't do anything, let last state continue
	}
	else
	{
//		m_Channel.SetAnimation( m_pMasterChannel->GetAnimation(), this->GetBeginTime() );
	}
}
