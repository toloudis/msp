/*****************************************************************************
**	mnmReferencePosChannel.cpp
**
**	Reference implementation that attaches to the value of a position channel
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/mnm/mnmReferencePosChannel.hpp"

#include "Support/tmln/tmlnChannelPosition.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmReferencePosChannel::mnmReferencePosChannel(const tmlnChannelPosition &i_Channel)
: m_Channel(i_Channel)
{

}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmReferencePosChannel::~mnmReferencePosChannel()
{

}

//--------------------------------------------------------------------
// GetMatrix for this named refence
//--------------------------------------------------------------------
//virtual
maMatrix4x4 mnmReferencePosChannel::GetMatrix() const
{
	// need to do full transformation eventually
	maMatrix4x4 matx;
	matx.MakeTranslate(m_Channel.GetPosition());
	return matx;
}

