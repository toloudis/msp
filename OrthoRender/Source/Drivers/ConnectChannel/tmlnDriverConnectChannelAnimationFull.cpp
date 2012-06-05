/*****************************************************************************
**	tmlnDriverConnectChannelAnimationFull.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelAnimationFull.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverConnectChannelAnimationFull::tmlnDriverConnectChannelAnimationFull(tmlnChannelAnimationFull &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverConnectChannelTemplate<tmlnChannelAnimationFull>(i_Channel, i_ChunkName)
{
}


//--------------------------------------------------------------------
//  Update the value of things that are being driven
//--------------------------------------------------------------------
void tmlnDriverConnectChannelAnimationFull::Operate(float i_Time)
{
	//	check to make sure master name and channel are valid
	//	if not, try to correct it.
	//
	CheckMasterNameAndChannel();

	////	if master channel grab its value
	////
	//maPoint3d goal(0.0f, 0.0f, 0.0f);
	//if (m_pMasterChannel)
	//	goal = m_pMasterChannel->GetPosition();
	//
	//// Check and handle blend into driver
	//if (IsBefore(i_Time))
	//{
	//	maPoint3d cur = m_Channel.GetPosition();
	//	float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
	//	maPoint3d pos = goal*percent + cur*(1.0f - percent);
	//
	//	// If smooth blend, add in influence of the gradients
	//	if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
	//	{
	//		AddGradientInfluence(&m_Channel, i_Time, 
	//			this->GetBeginTime(), this->GetEndTime(), this->GetEaseInWeight(),
	//			goal, pos);
	//	}
	//
	//	m_Channel.SetPosition(pos);
	//}
	//else
	//{
	//	m_Channel.SetPosition(goal);
	//}

	//
	// TODO - need to handle blending properly
	//

	// Check and handle blend into driver
	if (m_pMasterChannel != NULL)
	{
//		entAnimation* pAnim = this->m_pMasterChannel->GetAnimation();
//		if (pAnim != NULL)
		{
			if (IsBefore(i_Time))
			{
				float blend_time = 0.0f;
				//if (get_blend_time(i_Time, blend_time))
				{
					bool bSmoothBlend = (this->GetBlendType() == tmlnDriver::e_SmoothBlend);
					float easeIn = this->GetEaseInWeight();
					float easeOut = 1.0f;
					if (bSmoothBlend)
					{
						tmlnDriver *pPrevDriver = m_Channel.GetPreviousDriver(i_Time);
						if (pPrevDriver)
							easeOut = pPrevDriver->GetEaseOutWeight();
					}
//					m_Channel.BlendAnimation( pAnim, this->GetBeginTime(), blend_time,
//						bSmoothBlend, easeIn, easeOut);
				}
				// otherwise, don't do anything, let last state continue
			}
			else
			{
//				m_Channel.SetAnimation( pAnim, this->GetBeginTime() );
			}

			// This forces a call to Animate() at the given simulation time
			// in order to update the matrices for attachments
			m_Channel.Animate(i_Time);
		}
	}
}
