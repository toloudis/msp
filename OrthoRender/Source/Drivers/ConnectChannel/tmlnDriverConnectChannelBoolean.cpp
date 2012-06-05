/*****************************************************************************
**	tmlnDriverConnectChannelBoolean.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelBoolean.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverConnectChannelBoolean::tmlnDriverConnectChannelBoolean(tmlnChannelBoolean &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverConnectChannelTemplate<tmlnChannelBoolean>(i_Channel, i_ChunkName)
{
}

//--------------------------------------------------------------------
//  Update the value of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverConnectChannelBoolean::Operate(float i_Time)
{
	if (!IsBefore(i_Time))
	{
		//	check to make sure master name and channel are valid
		//	if not, try to correct it.
		//
		CheckMasterNameAndChannel();

		//	if master channel grab its value
		bool goal = false;
		if (m_pMasterChannel)
			goal = m_pMasterChannel->GetState();

		m_Channel.SetState(goal);
	}
}
